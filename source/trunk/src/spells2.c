/* File: spells2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spell code (part 2) */

#include "angband.h"
#include "grid.h"


/*
 * self-knowledge... idea from nethack.  Useful for determining powers and
 * resistences of items.  It saves the screen, clears it, then starts listing
 * attributes, a screenful at a time.  (There are a LOT of attributes to
 * list.  It will probably take 2 or 3 screens for a powerful character whose
 * using several artifacts...) -CFT
 *
 * It is now a lot more efficient. -BEN-
 *
 * See also "identify_fully(creature_ptr, )".
 *
 * XXX XXX XXX Use the "show_file()" method, perhaps.
 */
void creature_knowledge(creature_type *creature_ptr)
{
	int i = 0, j, k;

	int v_nr = 0;
	char v_string [MAX_KARMA] [128];
	char s_string [6] [128];

	u32b flgs[TRAIT_FLAG_MAX];

	object_type *object_ptr;

	char Dummy[80];
	char Dummy2[10][80];
	char buf[2][80];
	char buf2[100];

	cptr info[280];

	int plev = creature_ptr->lev;

	int percent;

	Term_clear();

	for (j = 0; j < TRAIT_FLAG_MAX; j++)
		flgs[j] = 0L;

	creature_ptr->knowledge |= (KNOW_STAT | KNOW_HPRATE);

	strcpy(Dummy, "");

	percent = (int)(((long)creature_ptr->base_hp[PY_MAX_LEVEL - 1] * 200L) /
		(2 * creature_ptr->hitdice +
		((PY_MAX_LEVEL - 1+3) * (creature_ptr->hitdice + 1))));

#ifdef JP
    sprintf(Dummy, "現在の体力ランク : %d/100", percent);
#else
	sprintf(Dummy, "Your current Life Rating is %d/100.", percent);
#endif

	strcpy(buf[0], Dummy);
	info[i++] = buf[0];
	info[i++] = "";

	/* Acquire item flags from equipment */
	for (k = 0; k < INVEN_TOTAL; k++)
	{
		u32b tflgs[TRAIT_FLAG_MAX];

		object_ptr = &creature_ptr->inventory[k];
		if(!IS_EQUIPPED(object_ptr)) continue;

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Extract the flags */
		object_flags(object_ptr, tflgs);

		/* Extract flags */
		for (j = 0; j < TRAIT_FLAG_MAX; j++)
			flgs[j] |= tflgs[j];
	}

#ifdef JP
	info[i++] = "能力の最大値";
#else
	info[i++] = "Limits of maximum stats";
#endif

	for (v_nr = 0; v_nr < 6; v_nr++)
	{
		char stat_desc[80];
		int n = creature_ptr->stat_mod_max_max[v_nr];
		sprintf(stat_desc, "%s %2d.%d", stat_names[v_nr], n / STAT_FRACTION, n % STAT_FRACTION);
		strcpy(s_string[v_nr], stat_desc);

		info[i++] = s_string[v_nr];
	}
	info[i++] = "";

	show_alignment(buf2, creature_ptr);
#ifdef JP
	sprintf(Dummy, "現在の属性 : %s", buf2);
#else
	sprintf(Dummy, "Your alighnment : %s", buf2);
#endif
	strcpy(buf[1], Dummy);
	info[i++] = buf[1];
	for (v_nr = 0; v_nr < MAX_KARMA; v_nr++)
	{
		if(creature_ptr->karmas[v_nr] > 0)
		{
#ifdef JP
			sprintf(v_string[v_nr], "[%s]の業: %d\n",
#else
			sprintf(v_string[v_nr], "Your karma of %s is %d.",
#endif
				karma[v_nr].title, creature_ptr->karmas[v_nr]);

			info[i++] = v_string[v_nr];
		}
	}

	info[i++] = "";


	if(is_player(creature_ptr))
	{
#ifdef JP
		info[i++] = "あなたはプレイヤーである。";
#else
		info[i++] = "You are a player. ";
#endif
	}

	if(creature_ptr->stigmatic)
	{
#ifdef JP
		info[i++] = "あなたは「＠」のルーンの〈烙印〉を背負う者である。";
#else
		info[i++] = "You are a stigmatic of the cursed rune \"@\". ";
#endif
	}

	if(IS_DEAD(creature_ptr))
	{
#ifdef JP
		info[i++] = "あなたは死んでいる";
#else
		info[i++] = "You are dead. ";
#endif
	}
		

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_GOOD);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[0], "あなたは〈善〉の敵である。(対邪スレイx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[0], "You are a enemy of good(x%d.%02d by evil slaying).", percent / 100, percent % 100);
#endif
		info[i++] = Dummy2[0];
	}

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_EVIL);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[1], "あなたは〈悪〉の敵である。(対善スレイx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[1], "You are a enemy of evil(x%d.%02d by good slaying).", percent / 100, percent % 100);
#endif
		info[i++] = Dummy2[1];
	}

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_ORDER);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[2], "あなたは〈秩序〉の敵である。(対混沌スレイx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[2], "You are a enemy of order(x%d.%02d by chaos slaying).", percent / 100, percent % 100);
#endif
		info[i++] = Dummy2[2];
	}

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_CHAOS);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[3], "あなたは〈混沌〉の敵である。(対秩序スレイx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[3], "You are a enemy of chaos(x%d.%02d by order slaying.)", percent / 100, percent % 100);
#endif
		info[i++] = Dummy;
	}

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_BALANCE);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[4], "あなたは〈天秤〉の敵である。(スレイx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[4], "You are a enemy of balance(x%d.%02d by slaying).", percent / 100, percent % 100);
#endif
		info[i++] = Dummy2[4];
	}


	switch (creature_ptr->action)
	{
		case ACTION_SEARCH:
#ifdef JP
info[i++] = "あなたはひじょうに注意深く周囲を見渡している。";
#else
			info[i++] = "You are looking around very carefully.";
#endif
			break;
	}

	if(creature_ptr->new_spells)
	{
#ifdef JP
		info[i++] = "あなたは呪文や祈りを学ぶことができる。";
#else
		info[i++] = "You can learn some spells/prayers.";
#endif

	}

	if(creature_ptr->lite)
	{
#ifdef JP
		info[i++] = "あなたの身体は光っている。";
#else
		info[i++] = "You are carrying a permanent light.";
#endif
	}

	//TODO old pval status

	/* Access the current weapon */
	object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 1);

	/* Save the screen */
	screen_save();

	/* Erase the screen */
	for (k = 1; k < 24; k++) prt("", k, 3);

	/* Label the information */
#ifdef JP
	prt("        あなたの状態:", 1, 5);
#else
	prt("     Your Attributes:", 1, 5);
#endif


	/* We will print on top of the map (column 13) */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
		prt(info[j], k++, 5);

		/* Every 20 entries (lines 2 to 21), start over */
		if((k == 22) && (j+1 < i))
		{
#ifdef JP
			prt("-- 続く --", k, 5);
#else
			prt("-- more --", k, 5);
#endif

			inkey();
			for (; k > 2; k--) prt("", k, 5);
		}
	}

	/* Pause */
#ifdef JP
	prt("[何かキーを押すとゲームに戻ります]", k, 3);
#else
	prt("[Press any key to continue]", k, 3);
#endif

	inkey();

	/* Restore the screen */
	screen_load();
}


static int report_magics_aux(int dur)
{
	if(dur <= 5) return 0;
	else if(dur <= 10) return 1;
	else if(dur <= 20) return 2;
	else if(dur <= 50) return 3;
	else if(dur <= 100) return 4;
	else if(dur <= 200) return 5;
	else return 6;
}

static cptr report_magic_durations[] =
{
#ifdef JP
	"ごく短い間",
	"少しの間",
	"しばらくの間",
	"多少長い間",
	"長い間",
	"非常に長い間",
	"信じ難いほど長い間",
	"クリーチャーを攻撃するまで"
#else
	"for a short time",
	"for a little while",
	"for a while",
	"for a long while",
	"for a long time",
	"for a very long time",
	"for an incredibly long time",
	"until you hit a creature"
#endif
};


/*
 * Report all currently active magical effects.
 */
void report_magics(creature_type *creature_ptr)
{
	int     i = 0, j, k;
	char    Dummy[80];
	cptr    info[128];
	int     info2[128];

	/* Save the screen */
	screen_save();

	/* Erase the screen */
	for (k = 1; k < 24; k++) prt("", k, 3);

	/* Label the information */
#ifdef JP
	prt("           魔法        :", 1, 5);
#else
	prt("     Your Current Magic:", 1, 5);
#endif


	/* We will print on top of the map (column 13) */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
#ifdef JP
		sprintf(Dummy, "あなたは%s%s", info[j],
#else
		sprintf(Dummy, "%s %s.", info[j],
#endif

			report_magic_durations[info2[j]]);
		prt(Dummy, k++, 5);

		/* Every 20 entries (lines 2 to 21), start over */
		if((k == 22) && (j + 1 < i))
		{
#ifdef JP
			prt("-- 続く --", k, 5);
#else
			prt("-- more --", k, 5);
#endif

			inkey();
			for (; k > 2; k--) prt("", k, 5);
		}
	}

	/* Pause */
#ifdef JP
	prt("[何かキーを押すとゲームに戻ります]", k, 3);
#else
	prt("[Press any key to continue]", k, 3);
#endif

	inkey();

	/* Restore the screen */
	screen_load();
}


static bool detect_feat_flag(creature_type *creature_ptr, int range, int flag, bool known)
{
	int       x, y;
	bool      detect = FALSE;
	cave_type *c_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan the current panel */
	for (y = 1; y < floor_ptr->height - 1; y++)
	{
		for (x = 1; x <= floor_ptr->width - 1; x++)
		{
			int dist = distance(creature_ptr->fy, creature_ptr->fx, y, x);
			if(dist > range) continue;

			/* Access the grid */
			c_ptr = &floor_ptr->cave[y][x];

			/* Hack -- Safe */
			if(flag == FF_TRAP)
			{
				/* Mark as detected */
				if(dist <= range && known)
				{
					if(dist <= range - 1) c_ptr->info |= (CAVE_IN_DETECT);

					c_ptr->info &= ~(CAVE_UNSAFE);

					/* Redraw */
					lite_spot(floor_ptr, y, x);
				}
			}

			/* Detect flags */
			if(cave_have_flag_grid(c_ptr, flag))
			{
				/* Detect secrets */
				disclose_grid(floor_ptr, y, x);

				/* Hack -- Memorize */
				c_ptr->info |= (CAVE_MARK);

				/* Redraw */
				lite_spot(floor_ptr, y, x);

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	/* Result */
	return detect;
}


/*
 * Detect all traps on current panel
 */
bool detect_traps(creature_type *creature_ptr, int range, bool known)
{
	bool detect = detect_feat_flag(creature_ptr, range, FF_TRAP, known);

	if(known) detect_trap = TRUE;

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 0) detect = FALSE;

	/* Describe */
	if(detect)
	{
#ifdef JP
		msg_print("トラップの存在を感じとった！");
#else
		msg_print("You sense the presence of traps!");
#endif
	}

	/* Result */
	return detect;
}


/*
 * Detect all doors on current panel
 */
bool detect_doors(creature_type *creature_ptr, int range)
{
	bool detect = detect_feat_flag(creature_ptr, range, FF_DOOR, TRUE);

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 0) detect = FALSE;

	/* Describe */
	if(detect)
	{
#ifdef JP
		msg_print("ドアの存在を感じとった！");
#else
		msg_print("You sense the presence of doors!");
#endif
	}

	/* Result */
	return detect;
}


/*
 * Detect all stairs on current panel
 */
bool detect_stairs(creature_type *creature_ptr, int range)
{
	bool detect = detect_feat_flag(creature_ptr, range, FF_STAIRS, TRUE);

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 0) detect = FALSE;

	/* Describe */
	if(detect)
	{
#ifdef JP
		msg_print("階段の存在を感じとった！");
#else
		msg_print("You sense the presence of stairs!");
#endif
	}

	/* Result */
	return detect;
}


/*
 * Detect any treasure on the current panel
 */
bool detect_treasure(creature_type *creature_ptr, int range)
{
	bool detect = detect_feat_flag(creature_ptr, range, FF_HAS_GOLD, TRUE);

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 6) detect = FALSE;

	/* Describe */
	if(detect)
	{
#ifdef JP
		msg_print("埋蔵された財宝の存在を感じとった！");
#else
		msg_print("You sense the presence of buried treasure!");
#endif
	}

	/* Result */
	return detect;
}



// Detect all "gold" objects on the current panel
bool detect_objects_gold(creature_type *creature_ptr, int range)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS) range2 /= 3;

	// Scan objects
	for (i = 1; i < object_max; i++)
	{
		object_type *object_ptr = &object_list[i];

		/* Skip dead objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Skip held objects */
		if(object_ptr->held_m_idx) continue;

		/* Location */
		y = object_ptr->fy;
		x = object_ptr->fx;

		/* Only detect nearby objects */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range2) continue;

		/* Detect "gold" objects */
		if(object_ptr->tval == TV_GOLD)
		{
			/* Hack -- memorize it */
			object_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(floor_ptr, y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 6) detect = FALSE;

	/* Describe */
	if(detect)
	{
#ifdef JP
msg_print("財宝の存在を感じとった！");
#else
		msg_print("You sense the presence of treasure!");
#endif

	}

	if(detect_creatures_string(creature_ptr, range, "$"))
	{
		detect = TRUE;
	}

	/* Result */
	return (detect);
}


/*
 * Detect all "normal" objects on the current panel
 */
bool detect_objects_normal(creature_type *creature_ptr, int range)
{
	int i, y, x;
	int range2 = range;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	bool detect = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < object_max; i++)
	{
		object_type *object_ptr = &object_list[i];

		/* Skip dead objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Skip held objects */
		if(object_ptr->held_m_idx) continue;

		/* Location */
		y = object_ptr->fy;
		x = object_ptr->fx;

		/* Only detect nearby objects */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range2) continue;

		/* Detect "real" objects */
		if(object_ptr->tval != TV_GOLD)
		{
			/* Hack -- memorize it */
			object_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(floor_ptr, y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 6) detect = FALSE;

	/* Describe */
	if(detect)
	{
#ifdef JP
msg_print("アイテムの存在を感じとった！");
#else
		msg_print("You sense the presence of objects!");
#endif

	}

	if(detect_creatures_string(creature_ptr, range, "!=?|/`"))
	{
		detect = TRUE;
	}

	/* Result */
	return (detect);
}


/*
 * Detect all "magic" objects on the current panel.
 *
 * This will light up all spaces with "magic" items, including artifacts,
 * ego-items, potions, scrolls, books, rods, wands, staves, amulets, rings,
 * and "enchanted" items of the "good" variety.
 *
 * It can probably be argued that this function is now too powerful.
 */
bool detect_objects_magic(creature_type *creature_ptr, int range)
{
	int i, y, x, tv;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	bool detect = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;
	/* Scan all objects */
	for (i = 1; i < object_max; i++)
	{
		object_type *object_ptr = &object_list[i];

		/* Skip dead objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Skip held objects */
		if(object_ptr->held_m_idx) continue;

		/* Location */
		y = object_ptr->fy;
		x = object_ptr->fx;

		/* Only detect nearby objects */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Examine the tval */
		tv = object_ptr->tval;

		/* Artifacts, misc magic items, or enchanted wearables */
		if(object_is_artifact(object_ptr) ||
			object_is_ego(object_ptr) ||
		    (tv == TV_WHISTLE) ||
		    (tv == TV_AMULET) ||
			(tv == TV_RING) ||
		    (tv == TV_STAFF) ||
			(tv == TV_WAND) ||
			(tv == TV_ROD) ||
		    (tv == TV_SCROLL) ||
			(tv == TV_POTION) ||
		    (tv == TV_LIFE_BOOK) ||
			(tv == TV_SORCERY_BOOK) ||
		    (tv == TV_NATURE_BOOK) ||
			(tv == TV_CHAOS_BOOK) ||
		    (tv == TV_DEATH_BOOK) ||
		    (tv == TV_TRUMP_BOOK) ||
			(tv == TV_ARCANE_BOOK) ||
			(tv == TV_CRAFT_BOOK) ||
			(tv == TV_DAEMON_BOOK) ||
			(tv == TV_CRUSADE_BOOK) ||
			(tv == TV_MUSIC_BOOK) ||
			(tv == TV_HISSATSU_BOOK) ||
			(tv == TV_HEX_BOOK) ||
		    ((object_ptr->to_ac > 0) || (object_ptr->to_hit + object_ptr->to_damage > 0)))
		{
			/* Memorize the item */
			object_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(floor_ptr, y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	/* Describe */
	if(detect)
	{
#ifdef JP
msg_print("魔法のアイテムの存在を感じとった！");
#else
		msg_print("You sense the presence of magic objects!");
#endif

	}

	/* Return result */
	return (detect);
}


/*
 * Detect all "normal" creatures on the current panel
 */
bool detect_creatures_normal(creature_type *creature_ptr, int range)
{
	int i, y, x;

	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *target_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[target_ptr->species_idx];

		/* Skip dead creatures */
		if(!target_ptr->species_idx) continue;

		/* Location */
		y = target_ptr->fy;
		x = target_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect all non-invisible creatures */
		if(!has_trait(target_ptr, TRAIT_INVISIBLE) || has_trait(creature_ptr, TRAIT_SEE_INVISIBLE))
		{
			/* Repair visibility later */
			repair_creatures = TRUE;

			/* Hack -- Detect creature */
			target_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 3) flag = FALSE;

	/* Describe */
	if(flag)
	{
		/* Describe result */
#ifdef JP
msg_print("クリーチャーの存在を感じとった！");
#else
		msg_print("You sense the presence of creatures!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all "invisible" creatures around the player
 */
bool detect_creatures_invis(creature_type *creature_ptr, int range)
{
	int i, y, x;
	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect invisible creatures */
		if(has_trait(m_ptr, TRAIT_INVISIBLE))
		{
			/* Update creature recall window */
			if(species_window_idx == m_ptr->species_idx)
			{
				/* Window stuff */
				play_window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_creatures = TRUE;

			/* Hack -- Detect creature */
			m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 3) flag = FALSE;

	/* Describe */
	if(flag)
	{
		/* Describe result */
#ifdef JP
msg_print("透明な生物の存在を感じとった！");
#else
		msg_print("You sense the presence of invisible creatures!");
#endif

	}

	/* Result */
	return (flag);
}



/*
 * Detect all "evil" creatures on current panel
 */
bool detect_creatures_evil(creature_type *creature_ptr, int range)
{
	int i, y, x;
	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect evil creatures */
		if(is_enemy_of_good_creature(m_ptr))
		{
			if(is_original_ap(m_ptr))
			{
				/* Take note that they are evil */
				//r_ptr->r_flags3 |= (RF3_EVIL);

				/* Update creature recall window */
				if(species_window_idx == m_ptr->species_idx)
				{
					/* Window stuff */
					play_window |= (PW_MONSTER);
				}
			}

			/* Repair visibility later */
			repair_creatures = TRUE;

			/* Hack -- Detect creature */
			m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if(flag)
	{
		/* Describe result */
#ifdef JP
msg_print("邪悪なる生物の存在を感じとった！");
#else
		msg_print("You sense the presence of evil creatures!");
#endif

	}

	/* Result */
	return (flag);
}




/*
 * Detect all "nonliving", "undead" or "demonic" creatures on current panel
 */
bool detect_creatures_nonliving(creature_type *creature_ptr, int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect non-living creatures */
		if(!creature_living(creature_ptr))
		{
			/* Update creature recall window */
			if(species_window_idx == m_ptr->species_idx)
			{
				/* Window stuff */
				play_window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_creatures = TRUE;

			/* Hack -- Detect creature */
			m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if(flag)
	{
		/* Describe result */
#ifdef JP
msg_print("自然でないクリーチャーの存在を感じた！");
#else
		msg_print("You sense the presence of unnatural beings!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all creatures it has mind on current panel
 */
bool detect_creatures_mind(creature_type *creature_ptr, int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect non-living creatures */
		if(!has_trait(m_ptr, TRAIT_EMPTY_MIND))
		{
			/* Update creature recall window */
			if(species_window_idx == m_ptr->species_idx)
			{
				/* Window stuff */
				play_window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_creatures = TRUE;

			/* Hack -- Detect creature */
			m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if(flag)
	{
		/* Describe result */
#ifdef JP
msg_print("殺気を感じとった！");
#else
		msg_print("You sense the presence of someone's mind!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all (string) creatures on current panel
 */
bool detect_creatures_string(creature_type *creature_ptr, int range, cptr Match)
{
	int i, y, x;
	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect creatures with the same symbol */
		if(my_strchr(Match, r_ptr->d_char))
		{
			/* Update creature recall window */
			if(species_window_idx == m_ptr->species_idx)
			{
				/* Window stuff */
				play_window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_creatures = TRUE;

			/* Hack -- Detect creature */
			m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->class_skills.old_skills.magic_num1[2] > 3) flag = FALSE;

	/* Describe */
	if(flag)
	{
		/* Describe result */
#ifdef JP
msg_print("クリーチャーの存在を感じとった！");
#else
		msg_print("You sense the presence of creatures!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * A "generic" detect creatures routine, tagged to flags3
 */
bool detect_creatures_xxx(creature_type *creature_ptr, int range, u32b match_flag)
{
	int  i, y, x;
	bool flag = FALSE;
#ifdef JP
cptr desc_creatures = "変なクリーチャー";
#else
	cptr desc_creatures = "weird creatures";
#endif

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect evil creatures */
		//TODO if(r_ptr->flags3 & (match_flag))
		if(1)
		{
			if(is_original_ap(m_ptr))
			{
				/* Take note that they are something */
				//TODO r_ptr->r_flags3 |= (match_flag);

				/* Update creature recall window */
				if(species_window_idx == m_ptr->species_idx)
				{
					/* Window stuff */
					play_window |= (PW_MONSTER);
				}
			}

			/* Repair visibility later */
			repair_creatures = TRUE;

			/* Hack -- Detect creature */
			m_ptr->sc_flag2 |= (SC_FLAG2_MARK | SC_FLAG2_SHOW);

			/* Update the creature */
			update_creature_view(player_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if(flag)
	{
		switch (match_flag)
		{
			default:
#ifdef JP
				desc_creatures = "敵";
#else
				desc_creatures = "the enemy";
#endif
				break;
		}

		/* Describe result */
#ifdef JP
msg_format("%sの存在を感じとった！", desc_creatures);
#else
		msg_format("You sense the presence of %s!", desc_creatures);
#endif

		msg_print(NULL);
	}

	/* Result */
	return (flag);
}


/*
 * Detect everything
 */
bool detect_all(creature_type *creature_ptr, int range)
{
	bool detect = FALSE;

	/* Detect everything */
	if(detect_traps(creature_ptr, range, TRUE)) detect = TRUE;
	if(detect_doors(creature_ptr, range)) detect = TRUE;
	if(detect_stairs(creature_ptr, range)) detect = TRUE;

	/* There are too many hidden treasure.  So... */
	/* if(detect_treasure(range)) detect = TRUE; */

	if(detect_objects_gold(creature_ptr, range)) detect = TRUE;
	if(detect_objects_normal(creature_ptr, range)) detect = TRUE;
	if(detect_creatures_invis(creature_ptr, range)) detect = TRUE;
	if(detect_creatures_normal(creature_ptr, range)) detect = TRUE;

	/* Result */
	return (detect);
}


/*
 * Apply a "project()" directly to all viewable creatures
 *
 * Note that affected creatures are NOT auto-tracked by this usage.
 *
 * To avoid misbehavior when creature deaths have side-effects,
 * this is done in two passes. -- JDL
 */
bool project_hack(creature_type *caster_ptr, int typ, int dam)
{
	int     i, x, y;
	bool    obvious = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	creature_type *target_ptr;

	for (i = 1; i < creature_max; i++)	// Mark all (nearby) creatures
	{
		target_ptr = &creature_list[i];
		if(!is_valid_creature(target_ptr) || IS_IN_THIS_FLOOR(target_ptr)) continue;	// Paranoia -- Skip dead creatures

		// Require line of sight
		y = target_ptr->fy;
		x = target_ptr->fx;
		if(!player_has_los_bold(y, x) || !projectable(floor_ptr, caster_ptr->fy, caster_ptr->fx, y, x)) continue;

		target_ptr->sc_flag |= (SC_FLAG_TEMP);	// Mark the creature
	}

	for (i = 1; i < creature_max; i++)	// Affect all marked creatures
	{
		target_ptr = &creature_list[i];
		if(!(target_ptr->sc_flag & (SC_FLAG_TEMP))) continue;	// Skip unmarked creatures
		target_ptr->sc_flag &= ~(SC_FLAG_TEMP);	// Remove mark

		// Jump directly to the target creature
		y = target_ptr->fy;
		x = target_ptr->fx;
		if(project(caster_ptr, 0, y, x, dam, typ, PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE, -1)) obvious = TRUE;
	}

	return (obvious);	// Result
}


/*
 * Speed creatures
 */
bool speed_creatures(creature_type *caster_ptr)
{
	return (project_hack(caster_ptr, GF_OLD_SPEED, caster_ptr->lev));
}

/*
 * Slow creatures
 */
bool slow_creatures(creature_type *caster_ptr)
{
	return (project_hack(caster_ptr, GF_OLD_SLOW, caster_ptr->lev));
}

/*
 * Sleep creatures
 */
bool sleep_creatures(creature_type *caster_ptr)
{
	return (project_hack(caster_ptr, GF_OLD_SLEEP, caster_ptr->lev));
}


/*
 * Banish evil creatures
 */
bool banish_evil(creature_type *caster_ptr, int dist)
{
	return (project_hack(caster_ptr, GF_AWAY_EVIL, dist));
}


/*
 * Turn undead
 */
bool turn_undead(creature_type *caster_ptr)
{
	bool tester = (project_hack(caster_ptr, GF_TURN_UNDEAD, caster_ptr->lev));
	return tester;
}


/*
 * Dispel undead creatures
 */
bool dispel_undead(creature_type *caster_ptr, int dam)
{
	bool tester = (project_hack(caster_ptr, GF_DISP_UNDEAD, dam));
	return tester;
}

/*
 * Dispel evil creatures
 */
bool dispel_evil(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_DISP_EVIL, dam));
}

/*
 * Dispel good creatures
 */
bool dispel_good(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_DISP_GOOD, dam));
}

/*
 * Dispel all creatures
 */
bool dispel_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_DISP_ALL, dam));
}

/*
 * Dispel 'living' creatures
 */
bool dispel_living(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_DISP_LIVING, dam));
}

/*
 * Dispel demons
 */
bool dispel_demons(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_DISP_DEMON, dam));
}


/*
 * Crusade
 */
bool crusade(creature_type *creature_ptr)
{
	return (project_hack(creature_ptr, GF_CRUSADE, creature_ptr->lev*4));
}


/*
 * Wake up all creatures, and speed up "los" creatures.
 */
void aggravate_creatures(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int     i;
	bool    sleep = FALSE;
	bool    speed = FALSE;


	/* Aggravate everyone nearby */
	for (i = 1; i < creature_max; i++)
	{
		creature_type    *m_ptr = &creature_list[i];

		/* Paranoia -- Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Skip aggravating creature (or player) */
		if(m_ptr == creature_ptr) continue;

		/* Wake up nearby sleeping creatures */
		if(m_ptr->cdis < MAX_SIGHT * 2)
		{
			/* Wake up */
			if(m_ptr->timed_trait[TRAIT_PARALYZED])
			{
				(void)set_timed_trait(m_ptr, TRAIT_PARALYZED, 0);
				sleep = TRUE;
			}
			if(!is_pet(player_ptr, m_ptr)) m_ptr->sc_flag2 |= SC_FLAG2_NOPET;
		}

		/* Speed up creatures in line of sight */
		if(player_has_los_bold(m_ptr->fy, m_ptr->fx))
		{
			if(!is_pet(player_ptr, m_ptr))
			{
				(void)set_timed_trait(m_ptr, TRAIT_FAST, m_ptr->timed_trait[TRAIT_FAST] + 100);
				speed = TRUE;
			}
		}
	}

	/* Messages */
#ifdef JP
	if(speed) msg_print("付近で何かが突如興奮したような感じを受けた！");
	else if(sleep) msg_print("何かが突如興奮したような騒々しい音が遠くに聞こえた！");
#else
	if(speed) msg_print("You feel a sudden stirring nearby!");
	else if(sleep) msg_print("You hear a sudden stirring in the distance!");
#endif
	if(creature_ptr->riding) creature_ptr->creature_update |= CRU_BONUS;
}



// Delete a non-unique/non-quest creature
bool genocide_aux(creature_type *user_ptr, int m_idx, int power, bool player_cast, int dam_side, cptr spell_name)
{
	int          msec = delay_factor * delay_factor * delay_factor;
	creature_type *target_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
	bool         resist = FALSE;

	if(is_pet(player_ptr, target_ptr) && !player_cast) return FALSE;

	// Hack -- Skip Unique Creatures or Quest Creatures
	if((has_trait(target_ptr, TRAIT_QUESTOR)) || has_trait(target_ptr, TRAIT_UNIQUE)) resist = TRUE;
	else if(has_trait(target_ptr, TRAIT_UNIQUE2)) resist = TRUE;
	else if(m_idx == user_ptr->riding) resist = TRUE;
	else if((floor_ptr->quest && !random_quest_number(floor_ptr)) || floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode) resist = TRUE;
	else if(player_cast && (target_ptr->lev * 2 > randint0(power))) resist = TRUE;
	else if(player_cast && (target_ptr->sc_flag2 & SC_FLAG2_NOGENO)) resist = TRUE;

	// Delete the creature
	else
	{
		if(record_named_pet && is_pet(player_ptr, target_ptr) && target_ptr->nickname)
		{
			char target_name[80];

			creature_desc(target_name, target_ptr, CD_INDEF_VISIBLE);
			do_cmd_write_nikki(DIARY_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, target_name);
		}

		delete_species_idx(&creature_list[m_idx]);
	}

	if(resist && player_cast)
	{
		bool see_m = is_seen(user_ptr, target_ptr);
		char target_name[80];

		creature_desc(target_name, target_ptr, 0);
		if(see_m)
		{
#ifdef JP
			msg_format("%^sには効果がなかった。", target_name);
#else
			msg_format("%^s is unaffected.", target_name);
#endif
		}
		if(has_trait(target_ptr, TRAIT_PARALYZED))
		{
			(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, 0);
			if(target_ptr->see_others || target_ptr->hear_noise)
			{
#ifdef JP
				msg_format("%^sが目を覚ました。", target_name);
#else
				msg_format("%^s wakes up.", target_name);
#endif
			}
		}
		if(is_friendly(player_ptr, target_ptr) && !is_pet(player_ptr, target_ptr))
		{
			if(see_m)
			{
#ifdef JP
				msg_format("%sは怒った！", target_name);
#else
				msg_format("%^s gets angry!", target_name);
#endif
			}
			set_hostile(user_ptr, target_ptr);
		}
		if(one_in_(13)) target_ptr->sc_flag2 |= SC_FLAG2_NOGENO;
	}

	if(player_cast)
	{
		/* Take damage */
#ifdef JP
		take_hit(NULL, user_ptr, DAMAGE_GENO, randint1(dam_side), format("%^sの呪文を唱えた疲労", spell_name), NULL, -1);
#else
		take_hit(NULL, user_ptr, DAMAGE_GENO, randint1(dam_side), format("the strain of casting %^s", spell_name), NULL, -1);
#endif
	}

	/* Visual feedback */
	move_cursor_relative(user_ptr->fy, user_ptr->fx);

	/* Redraw */
	play_redraw |= (PR_HP);

	/* Window stuff */
	play_window |= (PW_PLAYER);

	/* Handle */
	handle_stuff();

	/* Fresh */
	Term_fresh();

	/* Delay */
	Term_xtra(TERM_XTRA_DELAY, msec);

	return !resist;
}


/*
 * Delete all non-unique/non-quest creatures of a given "type" from the level
 */
bool symbol_genocide(creature_type *caster_ptr, int power, bool player_cast)
{
	int  i;
	char typ;
	bool result = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	/* Prevent genocide in quest levels */
	if((floor_ptr->quest && !random_quest_number(floor_ptr)) || floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		return (FALSE);
	}

	/* Mega-Hack -- Get a creature symbol */
#ifdef JP
	while (!get_com("どの種類(文字)のクリーチャーを抹殺しますか: ", &typ, FALSE)) ;
#else
	while (!get_com("Choose a creature race (by symbol) to genocide: ", &typ, FALSE)) ;
#endif

	/* Delete the creatures of that "type" */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Paranoia -- Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Skip "wrong" creatures */
		if(r_ptr->d_char != typ) continue;

		/* Take note */
#ifdef JP
		result |= genocide_aux(caster_ptr, i, power, player_cast, 4, "抹殺");
#else
		result |= genocide_aux(caster_ptr, i, power, player_cast, 4, "Genocide");
#endif
	}

	return result;
}


/*
 * Delete all nearby (non-unique) creatures
 */
bool mass_genocide(creature_type *caster_ptr, int power, bool player_cast)
{
	int  i;
	bool result = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	/* Prevent mass genocide in quest levels */
	if((floor_ptr->quest && !random_quest_number(floor_ptr)) || floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		return (FALSE);
	}

	/* Delete the (nearby) creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Paranoia -- Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Skip distant creatures */
		if(m_ptr->cdis > MAX_SIGHT) continue;

		/* Note effect */
#ifdef JP
		result |= genocide_aux(caster_ptr, i, power, player_cast, 3, "周辺抹殺");
#else
		result |= genocide_aux(caster_ptr, i, power, player_cast, 3, "Mass Genocide");
#endif
	}

	return result;
}



/*
 * Delete all nearby (non-unique) undead
 */
bool mass_genocide_undead(creature_type *caster_ptr, int power, bool player_cast)
{
	int  i;
	bool result = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	/* Prevent mass genocide in quest levels */
	if((floor_ptr->quest && !random_quest_number(floor_ptr)) || floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		return (FALSE);
	}

	/* Delete the (nearby) creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Paranoia -- Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		if(!has_trait(m_ptr, TRAIT_UNDEAD)) continue;

		/* Skip distant creatures */
		if(m_ptr->cdis > MAX_SIGHT) continue;

		/* Note effect */
#ifdef JP
		result |= genocide_aux(caster_ptr, i, power, player_cast, 3, "アンデッド消滅");
#else
		result |= genocide_aux(caster_ptr, i, power, player_cast, 3, "Annihilate Undead");
#endif
	}

	return result;
}



/*
 * Probe nearby creatures
 */
bool probing(floor_type *floor_ptr)
{
	int     i, speed;
	int cu, cv;
	bool    probe = FALSE;
	char buf[256];
	cptr align;
	cptr sex;

	cu = Term->scr->cu;
	cv = Term->scr->cv;
	Term->scr->cu = 0;
	Term->scr->cv = 1;

	/* Probe all (nearby) creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Paranoia -- Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		/* Require line of sight */
		if(!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;

		/* Probe visible creatures */
		if(m_ptr->see_others)
		{
			char m_name[80];

			/* Start the message */
			if(!probe)
			{
#ifdef JP
				msg_print("調査中...");
#else
				msg_print("Probing...");
#endif
			}

			msg_print(NULL);

			if(!is_original_ap(m_ptr))
			{
				if(has_trait(m_ptr, TRAIT_KAGE)) lose_mutative_trait(m_ptr, TRAIT_KAGE);

				m_ptr->ap_species_idx = m_ptr->species_idx;
				lite_spot(floor_ptr, m_ptr->fy, m_ptr->fx);
			}
			/* Get "the creature" or "something" */
			creature_desc(m_name, m_ptr, CD_IGNORE_HALLU | CD_INDEF_HIDDEN);

			speed = m_ptr->speed;
			if(m_ptr->timed_trait[TRAIT_FAST]) speed += 10;
			if(m_ptr->timed_trait[TRAIT_SLOW]) speed -= 10;

			/* Get the creature's alignment */
			/* TODO: New Alignment View */
			align = "----";

#ifdef JP
			if(m_ptr->sex == SEX_MALE) sex = "男性";
			else if(m_ptr->sex == SEX_FEMALE) sex = "女性";
			else if(m_ptr->sex == SEX_INTERSEX) sex = "両性";
			else if(m_ptr->sex == SEX_NONE) sex = "無性";
#else
			if(m_ptr->sex == SEX_MALE) sex = "Male";
			else if(m_ptr->sex == SEX_FEMALE) sex = "Female";
			else if(m_ptr->sex == SEX_INTERSEX) sex = "Intersex";
			else if(m_ptr->sex == SEX_NONE) sex = "Asexual";
#endif

			/* Describe the creature */
#ifdef JP
sprintf(buf,"%s 属性:%s 性別:%s HP:%d/%d AC:%d 速度:%s%d 腕:%d 知:%d 賢:%d 器:%d 耐:%d 魅:%d 経験:",
		m_name, align, sex, m_ptr->chp, m_ptr->mhp, m_ptr->ac + m_ptr->to_ac, (speed > 0) ? "+" : "", speed,
		m_ptr->stat_use[0] / STAT_FRACTION, m_ptr->stat_use[1] / STAT_FRACTION,
		m_ptr->stat_use[2] / STAT_FRACTION, m_ptr->stat_use[3] / STAT_FRACTION,
		m_ptr->stat_use[4] / STAT_FRACTION, m_ptr->stat_use[5] / STAT_FRACTION);
#else
sprintf(buf, "%s align:%s sex:%s HP:%d/%d AC:%d speed:%s%d STR:%d INT:%d WIS:%d DEX:%d CON:%d CHA:%d exp:",
		m_name, align, sex, m_ptr->chp, m_ptr->mhp, m_ptr->ac + m_ptr->to_ac, (speed > 0) ? "+" : "", speed,
		m_ptr->stat_use[0] / STAT_FRACTION, m_ptr->stat_use[1] / STAT_FRACTION,
		m_ptr->stat_use[2] / STAT_FRACTION, m_ptr->stat_use[3] / STAT_FRACTION,
		m_ptr->stat_use[4] / STAT_FRACTION, m_ptr->stat_use[5] / STAT_FRACTION);
#endif
			if(r_ptr->next_species_idx)
			{
				strcat(buf, format("%d/%d ", m_ptr->exp, r_ptr->next_exp));
			}
			else
			{
				strcat(buf, "xxx ");
			}

#ifdef JP
			if(m_ptr->timed_trait[TRAIT_PARALYZED]) strcat(buf,"睡眠 ");
			if(m_ptr->timed_trait[TRAIT_STUN]) strcat(buf,"朦朧 ");
			if(m_ptr->timed_trait[TRAIT_AFRAID]) strcat(buf,"恐怖 ");
			if(m_ptr->timed_trait[TRAIT_CONFUSED]) strcat(buf,"混乱 ");
			if(has_trait(m_ptr, TRAIT_INVULNERABLE)) strcat(buf,"無敵 ");
#else
			if(m_ptr->timed_trait[TRAIT_PARALYZED]) strcat(buf,"sleeping ");
			if(m_ptr->timed_trait[TRAIT_STUN]) strcat(buf,"stunned ");
			if(m_ptr->timed_trait[TRAIT_AFRAID]) strcat(buf,"scared ");
			if(m_ptr->timed_trait[TRAIT_CONFUSED]) strcat(buf,"confused ");
			if(has_trait(m_ptr, TRAIT_INVULNERABLE)) strcat(buf,"invulnerable ");
#endif
			buf[strlen(buf)-1] = '\0';
			prt(buf,0,0);

			/* HACK : Add the line to message buffer */
			message_add(buf);
			play_window |= (PW_MESSAGE);
			window_stuff(player_ptr);

			if(m_ptr->see_others) move_cursor_relative(m_ptr->fy, m_ptr->fx);
			inkey();

			Term_erase(0, 0, 255);

			/* Learn everything about this creature */
			if(lore_do_probe(m_ptr->species_idx))
			{
				char buf[80];

				/* Get base name of creature */
				strcpy(buf, (species_name + r_ptr->name));

#ifdef JP
				/* Note that we learnt some new flags  -Mogami- */
				msg_format("%sについてさらに詳しくなった気がする。", buf);
#else
				/* Pluralize it */
				plural_aux(buf);

				/* Note that we learnt some new flags  -Mogami- */
				msg_format("You now know more about %s.", buf);
#endif
				/* Clear -more- prompt */
				msg_print(NULL);
			}

			/* Probe worked */
			probe = TRUE;
		}
	}

	Term->scr->cu = cu;
	Term->scr->cv = cv;
	Term_fresh();

	/* Done */
	if(probe)
	{

#ifdef JP
msg_print("これで全部です。");
#else
		msg_print("That's all.");
#endif

	}

	/* Result */
	return (probe);
}



/*
 * The spell of destruction
 *
 * This spell "deletes" creatures (instead of "killing" them).
 *
 * Later we may use one function for both "destruction" and
 * "earthquake" by using the "full" to select "destruction".
 */
bool destroy_area(creature_type *caster_ptr, int y1, int x1, int r, bool in_generate)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int       y, x, k, t;
	cave_type *c_ptr;
	bool      flag = FALSE;

	/* Prevent destruction of quest levels and town */
	if((floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest)) || !floor_ptr->floor_level)
	{
		return (FALSE);
	}

	/* Lose creature light */
	if(!in_generate) clear_creature_lite(floor_ptr);

	/* Big area of affect */
	for (y = (y1 - r); y <= (y1 + r); y++)
	{
		for (x = (x1 - r); x <= (x1 + r); x++)
		{
			/* Skip illegal grids */
			if(!in_bounds(floor_ptr, y, x)) continue;

			/* Extract the distance */
			k = distance(y1, x1, y, x);

			/* Stay in the circle of death */
			if(k > r) continue;

			/* Access the grid */
			c_ptr = &floor_ptr->cave[y][x];

			// Erase Message
			c_ptr->message[0] = '\0';

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

			/* Lose light and knowledge */
			c_ptr->info &= ~(CAVE_MARK | CAVE_GLOW);

			if(!in_generate) /* Normal */
			{
				/* Lose unsafety */
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Hack -- Notice player affect */
				if(creature_bold(caster_ptr, y, x))
				{
					/* Hurt the player later */
					flag = TRUE;

					/* Do not hurt this grid */
					continue;
				}
			}

			/* Hack -- Skip the epicenter */
			if((y == y1) && (x == x1)) continue;

			if(c_ptr->creature_idx)
			{
				creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

				if(in_generate) /* In generation */
				{
					/* Delete the creature (if any) */
					delete_creature(floor_ptr, y, x);
				}
				else if(has_trait(m_ptr, TRAIT_QUESTOR))
				{
					/* Heal the creature */
					m_ptr->chp = m_ptr->mhp;

					/* Try to teleport away quest creatures */
					if(!teleport_away(&creature_list[c_ptr->creature_idx], (r * 2) + 1, TELEPORT_DEC_VALOUR)) continue;
				}
				else
				{
					if(record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
					{
						char m_name[80];

						creature_desc(m_name, m_ptr, CD_INDEF_VISIBLE);
						do_cmd_write_nikki(DIARY_NAMED_PET, RECORD_NAMED_PET_DESTROY, m_name);
					}

					/* Delete the creature (if any) */
					delete_creature(floor_ptr, y, x);
				}
			}

			/* During generation, destroyed artifacts are "preserved" */
			if(preserve_mode || in_generate)
			{
				s16b this_object_idx, next_object_idx = 0;

				/* Scan all objects in the grid */
				for (this_object_idx = c_ptr->object_idx; this_object_idx; this_object_idx = next_object_idx)
				{
					object_type *object_ptr;

					/* Acquire object */
					object_ptr = &object_list[this_object_idx];

					/* Acquire next object */
					next_object_idx = object_ptr->next_object_idx;

					/* Hack -- Preserve unknown artifacts */
					if(object_is_fixed_artifact(object_ptr) && (!object_is_known(object_ptr) || in_generate))
					{
						/* Mega-Hack -- Preserve the artifact */
						artifact_info[object_ptr->name1].cur_num = 0;

						if(in_generate && cheat_peek)
						{
							char object_name[MAX_NLEN];
							object_desc(object_name, object_ptr, (OD_NAME_ONLY | OD_STORE));
#ifdef JP
							msg_format("伝説のアイテム (%s) は生成中に*破壊*された。", object_name);
#else
							msg_format("Artifact (%s) was *destroyed* during generation.", object_name);
#endif
						}
					}
					else if(in_generate && cheat_peek && object_ptr->art_name)
					{
#ifdef JP
						msg_print("ランダム・アーティファクトの1つは生成中に*破壊*された。");
#else
						msg_print("One of the random artifacts was *destroyed* during generation.");
#endif
					}
				}
			}

			/* Delete objects */
			delete_object(floor_ptr, y, x);

			/* Destroy "non-permanent" grids */
			if(!cave_perma_grid(c_ptr))
			{
				/* Wall (or floor) type */
				t = randint0(200);

				if(!in_generate) /* Normal */
				{
					if(t < 20)
					{
						/* Create granite wall */
						cave_set_feat(floor_ptr, y, x, feat_granite);
					}
					else if(t < 70)
					{
						/* Create quartz vein */
						cave_set_feat(floor_ptr, y, x, feat_quartz_vein);
					}
					else if(t < 100)
					{
						/* Create magma vein */
						cave_set_feat(floor_ptr, y, x, feat_magma_vein);
					}
					else
					{
						/* Create floor */
						cave_set_feat(floor_ptr, y, x, feat_floor_rand_table[randint0(100)]);
					}
				}
				else /* In generation */
				{
					if(t < 20)
					{
						/* Create granite wall */
						place_extra_grid(c_ptr);
					}
					else if(t < 70)
					{
						/* Create quartz vein */
						c_ptr->feat = feat_quartz_vein;
					}
					else if(t < 100)
					{
						/* Create magma vein */
						c_ptr->feat = feat_magma_vein;
					}
					else
					{
						/* Create floor */
						place_floor_grid(c_ptr);
					}

					/* Clear garbage of hidden trap or door */
					c_ptr->mimic = 0;
				}
			}
		}
	}

	if(!in_generate)
	{
		/* Process "re-glowing" */
		for (y = (y1 - r); y <= (y1 + r); y++)
		{
			for (x = (x1 - r); x <= (x1 + r); x++)
			{
				/* Skip illegal grids */
				if(!in_bounds(floor_ptr, y, x)) continue;

				/* Extract the distance */
				k = distance(y1, x1, y, x);

				/* Stay in the circle of death */
				if(k > r) continue;

				/* Access the grid */
				c_ptr = &floor_ptr->cave[y][x];

				if(is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
				else if(!(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS))
				{
					int i, yy, xx;
					cave_type *cc_ptr;

					for (i = 0; i < 9; i++)
					{
						yy = y + ddy_ddd[i];
						xx = x + ddx_ddd[i];
						if(!in_bounds2(floor_ptr, yy, xx)) continue;
						cc_ptr = &floor_ptr->cave[yy][xx];
						if(have_flag(feature_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
						{
							c_ptr->info |= CAVE_GLOW;
							break;
						}
					}
				}
			}
		}

		/* Hack -- Affect player */
		if(flag)
		{
			/* Message */
#ifdef JP
			msg_print("燃えるような閃光が発生した！");
#else
			msg_print("There is a searing blast of light!");
#endif

			/* Blind the player */
			if(!has_trait(caster_ptr, TRAIT_NO_BLIND) && !caster_ptr->resist_lite)
			{
				/* Become blind */
				(void)set_timed_trait(caster_ptr, TRAIT_BLIND, caster_ptr->timed_trait[TRAIT_BLIND] + 10 + randint1(10));
			}
		}

		forget_flow(floor_ptr);

		/* Mega-Hack -- Forget the view and lite */
		update |= (PU_UN_VIEW | PU_UN_LITE);

		/* Update stuff */
		update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE | PU_CREATURES);

		/* Redraw map */
		play_redraw |= (PR_MAP);

		/* Window stuff */
		play_window |= (PW_OVERHEAD | PW_DUNGEON);

		if(caster_ptr->posture & NINJA_S_STEALTH)
		{
			if(floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].info & CAVE_GLOW) set_superstealth(caster_ptr, FALSE);
		}
	}

	/* Success */
	return (TRUE);
}


/*
 * Induce an "earthquake" of the given radius at the given location.
 *
 * This will turn some walls into floors and some floors into walls.
 *
 * The player will take damage and "jump" into a safe grid if possible,
 * otherwise, he will "tunnel" through the rubble instantaneously.
 *
 * Creatures will take damage, and "jump" into a safe grid if possible,
 * otherwise they will be "buried" in the rubble, disappearing from
 * the level in the same way that they do when genocided.
 *
 * Note that thus the player and creatures (except eaters of walls and
 * passers through walls) will never occupy the same grid as a wall.
 * Note that as of now (2.7.8) no creature may occupy a "wall" grid, even
 * for a single turn, unless that creature can pass_walls or kill_walls.
 * This has allowed massive simplification of the "creature" code.
 */
bool earthquake_aux(creature_type *target_ptr, int cy, int cx, int r, int m_idx)
{
	int             i, t, y, x, yy, xx, dy, dx;
	int             damage = 0;
	int             sn = 0, sy = 0, sx = 0;
	bool            hurt = FALSE;
	cave_type       *c_ptr;
	bool            map[32][32];
	floor_type      *floor_ptr = GET_FLOOR_PTR(target_ptr);


	/* Prevent destruction of quest levels and town */
	if((floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest)) || !floor_ptr->floor_level)
	{
		return (FALSE);
	}

	/* Paranoia -- Enforce maximum range */
	if(r > 12) r = 12;

	/* Clear the "maximal blast" area */
	for (y = 0; y < 32; y++)
	{
		for (x = 0; x < 32; x++)
		{
			map[y][x] = FALSE;
		}
	}

	/* Check around the epicenter */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if(!in_bounds(floor_ptr, yy, xx)) continue;

			/* Skip distant grids */
			if(distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &floor_ptr->cave[yy][xx];

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY | CAVE_UNSAFE);

			/* Lose light and knowledge */
			c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);

			/* Skip the epicenter */
			if(!dx && !dy) continue;

			/* Skip most grids */
			if(randint0(100) < 85) continue;

			/* Damage this grid */
			map[16+yy-cy][16+xx-cx] = TRUE;

			/* Hack -- Take note of player damage */
			if(creature_bold(target_ptr, yy, xx)) hurt = TRUE;
		}
	}

	/* First, affect the player (if necessary) */
	if(hurt && !has_trait(target_ptr, TRAIT_PASS_WALL) && !has_trait(target_ptr, TRAIT_KILL_WALL))
	{
		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			/* Access the location */
			y = target_ptr->fy + ddy_ddd[i];
			x = target_ptr->fx + ddx_ddd[i];

			/* Skip non-empty grids */
			if(!cave_empty_bold(floor_ptr, y, x)) continue;

			/* Important -- Skip "quake" grids */
			if(map[16+y-cy][16+x-cx]) continue;

			if(floor_ptr->cave[y][x].creature_idx) continue;

			/* Count "safe" grids */
			sn++;

			/* Randomize choice */
			if(randint0(sn) > 0) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}

		/* Random message */
		switch (randint1(3))
		{
			case 1:
			{
#ifdef JP
				msg_print("ダンジョンの壁が崩れた！");
#else
				msg_print("The cave ceiling collapses!");
#endif
				break;
			}
			case 2:
			{
#ifdef JP
				msg_print("ダンジョンの床が不自然にねじ曲がった！");
#else
				msg_print("The cave floor twists in an unnatural way!");
#endif
				break;
			}
			default:
			{
#ifdef JP
				msg_print("ダンジョンが揺れた！崩れた岩が頭に降ってきた！");
#else
				msg_print("The cave quakes!  You are pummeled with debris!");
#endif
				break;
			}
		}

		/* Hurt the player a lot */
		if(!sn)
		{
			/* Message and damage */
#ifdef JP
			msg_print("あなたはひどい怪我を負った！");
#else
			msg_print("You are severely crushed!");
#endif
			damage = 200;
		}

		/* Destroy the grid, and push the player to safety */
		else
		{
			/* Calculate results */
			switch (randint1(3))
			{
				case 1:
				{
#ifdef JP
					msg_print("降り注ぐ岩をうまく避けた！");
#else
					msg_print("You nimbly dodge the blast!");
#endif
					damage = 0;
					break;
				}
				case 2:
				{
#ifdef JP
					msg_print("岩石があなたに直撃した!");
#else
					msg_print("You are bashed by rubble!");
#endif
					damage = diceroll(10, 4);
					(void)set_timed_trait(target_ptr, TRAIT_STUN, target_ptr->timed_trait[TRAIT_STUN] + randint1(50));
					break;
				}
				case 3:
				{
#ifdef JP
					msg_print("あなたは床と壁との間に挟まれてしまった！");
#else
					msg_print("You are crushed between the floor and ceiling!");
#endif
					damage = diceroll(10, 4);
					(void)set_timed_trait(target_ptr, TRAIT_STUN, target_ptr->timed_trait[TRAIT_STUN] + randint1(50));
					break;
				}
			}

			/* Move the player to the safe location */
			(void)move_creature(target_ptr, NULL, sy, sx, MCE_DONT_PICKUP);
		}

		/* Important -- no wall on player */
		map[16+target_ptr->fy-cy][16+target_ptr->fx-cx] = FALSE;

		/* Take some damage */
		if(damage)
		{
			char *killer;

			if(m_idx)
			{
				char m_name[80];
				creature_type *m_ptr = &creature_list[m_idx];

				/* Get the creature's real name */
				creature_desc(m_name, m_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

#ifdef JP
				killer = format("%sの起こした地震", m_name);
#else
				killer = format("an earthquake caused by %s", m_name);
#endif
			}
			else
			{
#ifdef JP
				killer = "地震";
#else
				killer = "an earthquake";
#endif
			}

			take_hit(NULL, target_ptr, DAMAGE_ATTACK, damage, killer, NULL, -1);
		}
	}

	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if(!map[16+yy-cy][16+xx-cx]) continue;

			/* Access the grid */
			c_ptr = &floor_ptr->cave[yy][xx];

			if(c_ptr->creature_idx == target_ptr->riding) continue;

			/* Process creatures */
			if(c_ptr->creature_idx)
			{
				creature_type *m_ptr = &creature_list[c_ptr->creature_idx];

				/* Quest creatures */
				if(has_trait(m_ptr, TRAIT_QUESTOR))
				{
					/* No wall on quest creatures */
					map[16+yy-cy][16+xx-cx] = FALSE;

					continue;
				}

				/* Most creatures cannot co-exist with rock */
				if(!has_trait(m_ptr, TRAIT_KILL_WALL) && !has_trait(m_ptr, TRAIT_PASS_WALL))
				{
					char m_name[80];

					/* Assume not safe */
					sn = 0;

					/* Creature can move to escape the wall */
					if(!(has_trait(m_ptr, TRAIT_NEVER_MOVE)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Access the grid */
							y = yy + ddy_ddd[i];
							x = xx + ddx_ddd[i];

							/* Skip non-empty grids */
							if(!cave_empty_bold(floor_ptr, y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if(is_glyph_grid(&floor_ptr->cave[y][x])) continue;
							if(is_explosive_rune_grid(&floor_ptr->cave[y][x])) continue;

							/* ... nor on the Pattern */
							if(pattern_tile(floor_ptr, y, x)) continue;

							/* Important -- Skip "quake" grids */
							if(map[16+y-cy][16+x-cx]) continue;

							if(floor_ptr->cave[y][x].creature_idx) continue;
							if(creature_bold(target_ptr, y, x)) continue;

							/* Count "safe" grids */
							sn++;

							/* Randomize choice */
							if(randint0(sn) > 0) continue;

							/* Save the safe grid */
							sy = y; sx = x;
						}
					}

					/* Describe the creature */
					creature_desc(m_name, m_ptr, 0);

					/* Scream in pain */
#ifdef JP
					if(!ignore_unview || is_seen(target_ptr, m_ptr)) msg_format("%^sは苦痛で泣きわめいた！", m_name);
#else
					if(!ignore_unview || is_seen(target_ptr, m_ptr)) msg_format("%^s wails out in pain!", m_name);
#endif

					/* Take damage from the quake */
					damage = (sn ? diceroll(4, 8) : (m_ptr->chp + 1));

					/* Creature is certainly awake */
					(void)set_timed_trait(m_ptr, TRAIT_PARALYZED, 0);

					/* Apply damage directly */
					m_ptr->chp -= damage;

					/* Delete (not kill) "dead" creatures */
					if(m_ptr->chp < 0)
					{
						/* Message */
#ifdef JP
						if(!ignore_unview || is_seen(target_ptr, m_ptr)) msg_format("%^sは岩石に埋もれてしまった！", m_name);
#else
						if(!ignore_unview || is_seen(target_ptr, m_ptr)) msg_format("%^s is embedded in the rock!", m_name);
#endif

						if(c_ptr->creature_idx)
						{
							if(record_named_pet && is_pet(player_ptr, &creature_list[c_ptr->creature_idx]) && creature_list[c_ptr->creature_idx].nickname)
							{
								char m2_name[80];

								creature_desc(m2_name, m_ptr, CD_INDEF_VISIBLE);
								do_cmd_write_nikki(DIARY_NAMED_PET, RECORD_NAMED_PET_EARTHQUAKE, m2_name);
							}
						}

						/* Delete the creature */
						delete_creature(floor_ptr, yy, xx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if(sn)
					{
						int m_idx = floor_ptr->cave[yy][xx].creature_idx;

						/* Update the old location */
						floor_ptr->cave[yy][xx].creature_idx = 0;

						/* Update the new location */
						floor_ptr->cave[sy][sx].creature_idx = m_idx;

						/* Move the creature */
						m_ptr->fy = sy;
						m_ptr->fx = sx;

						/* Update the creature (new location) */
						update_creature_view(player_ptr, m_idx, TRUE);

						/* Redraw the old grid */
						lite_spot(floor_ptr, yy, xx);

						/* Redraw the new grid */
						lite_spot(floor_ptr, sy, sx);
					}
				}
			}
		}
	}

	/* Lose creature light */
	clear_creature_lite(floor_ptr);

	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if(!map[16+yy-cy][16+xx-cx]) continue;

			/* Access the cave grid */
			c_ptr = &floor_ptr->cave[yy][xx];

			/* Paranoia -- never affect player */
			if(creature_bold(target_ptr, yy, xx)) continue;

			// Destroy location (if valid)
			if(cave_valid_bold(floor_ptr, yy, xx))
			{
				/* Delete objects */
				delete_object(floor_ptr, yy, xx);

				/* Wall (or floor) type */
				t = cave_have_flag_bold(floor_ptr, yy, xx, FF_PROJECT) ? randint0(100) : 200;

				/* Granite */
				if(t < 20)
				{
					/* Create granite wall */
					cave_set_feat(floor_ptr, yy, xx, feat_granite);
				}

				/* Quartz */
				else if(t < 70)
				{
					/* Create quartz vein */
					cave_set_feat(floor_ptr, yy, xx, feat_quartz_vein);
				}

				/* Magma */
				else if(t < 100)
				{
					/* Create magma vein */
					cave_set_feat(floor_ptr, yy, xx, feat_magma_vein);
				}

				/* Floor */
				else
				{
					/* Create floor */
					cave_set_feat(floor_ptr, yy, xx, feat_floor_rand_table[randint0(100)]);
				}
			}
		}
	}


	/* Process "re-glowing" */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if(!in_bounds(floor_ptr, yy, xx)) continue;

			/* Skip distant grids */
			if(distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &floor_ptr->cave[yy][xx];

			if(is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
			else if(!(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS))
			{
				int ii, yyy, xxx;
				cave_type *cc_ptr;

				for (ii = 0; ii < 9; ii++)
				{
					yyy = yy + ddy_ddd[ii];
					xxx = xx + ddx_ddd[ii];
					if(!in_bounds2(floor_ptr, yyy, xxx)) continue;
					cc_ptr = &floor_ptr->cave[yyy][xxx];
					if(have_flag(feature_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
					{
						c_ptr->info |= CAVE_GLOW;
						break;
					}
				}
			}
		}
	}


	/* Mega-Hack -- Forget the view and lite */
	update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE | PU_CREATURES);

	/* Update the health bar */
	play_redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);

	if(target_ptr->posture & NINJA_S_STEALTH)
	{
		if(floor_ptr->cave[target_ptr->fy][target_ptr->fx].info & CAVE_GLOW) set_superstealth(target_ptr, FALSE);
	}

	/* Success */
	return (TRUE);
}

bool earthquake(creature_type *target_ptr, int cy, int cx, int r)
{
	return earthquake_aux(target_ptr, cy, cx, r, 0);
}


void discharge_minion(creature_type *caster_ptr)
{
	int i;
	bool okay = TRUE;

	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		if(!m_ptr->species_idx || !is_pet(player_ptr, m_ptr)) continue;
		if(m_ptr->nickname) okay = FALSE;
	}
	if(!okay || caster_ptr->riding)
	{
#ifdef JP
		if(!get_check("本当に全ペットを爆破しますか？"))
#else
		if(!get_check("You will blast all pets. Are you sure? "))
#endif
			return;
	}
	for (i = 1; i < creature_max; i++)
	{
		int dam;
		creature_type *m_ptr = &creature_list[i];

		if(!m_ptr->species_idx || !is_pet(player_ptr, m_ptr)) continue;

		/* Uniques resist discharging */
		if(has_trait(m_ptr, TRAIT_UNIQUE))
		{
			char m_name[80];
			creature_desc(m_name, m_ptr, 0x00);
#ifdef JP
			msg_format("%sは爆破されるのを嫌がり、勝手に自分の世界へと帰った。", m_name);
#else
			msg_format("%^s resists to be blasted, and run away.", m_name);
#endif
			delete_species_idx(&creature_list[i]);
			continue;
		}
		dam = m_ptr->mhp / 2;
		if(dam > 100) dam = (dam-100)/2 + 100;
		if(dam > 400) dam = (dam-400)/2 + 400;
		if(dam > 800) dam = 800;
		project(m_ptr, 2+(m_ptr->lev/10), m_ptr->fy,
			m_ptr->fx, dam, GF_PLASMA, 
			PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);

		if(record_named_pet && m_ptr->nickname)
		{
			char m_name[80];

			creature_desc(m_name, m_ptr, CD_INDEF_VISIBLE);
			do_cmd_write_nikki(DIARY_NAMED_PET, RECORD_NAMED_PET_BLAST, m_name);
		}

		delete_species_idx(&creature_list[i]);
	}
}


/*
 * This routine clears the entire "temp" set.
 *
 * This routine will Perma-Lite all "temp" grids.
 *
 * This routine is used (only) by "lite_room()"
 *
 * Dark grids are illuminated.
 *
 * Also, process all affected creatures.
 *
 * SMART creatures always wake up when illuminated
 * NORMAL creatures wake up 1/4 the time when illuminated
 * STUPID creatures wake up 1/10 the time when illuminated
 */
static void cave_temp_room_lite(creature_type *lite_ptr)
{
	int i;
	floor_type *floor_ptr = GET_FLOOR_PTR(lite_ptr);

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		cave_type *c_ptr = &floor_ptr->cave[y][x];

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Update only non-CAVE_GLOW grids */
		/* if(c_ptr->info & (CAVE_GLOW)) continue; */

		/* Perma-Lite */
		c_ptr->info |= (CAVE_GLOW);

		/* Process affected creatures */
		if(c_ptr->creature_idx)
		{
			int chance = 25;

			creature_type    *m_ptr = &creature_list[c_ptr->creature_idx];

			species_type    *r_ptr = &species_info[m_ptr->species_idx];

			/* Update the creature */
			update_creature_view(player_ptr, c_ptr->creature_idx, FALSE);

			/* Stupid creatures rarely wake up */
			if(has_trait(m_ptr, TRAIT_STUPID)) chance = 10;

			/* Smart creatures always wake up */
			if(has_trait(m_ptr, TRAIT_SMART)) chance = 100;

			/* Sometimes creatures wake up */
			if(m_ptr->timed_trait[TRAIT_PARALYZED] && (randint0(100) < chance))
			{
				/* Wake up! */
				(void)set_timed_trait(m_ptr, TRAIT_PARALYZED, 0);

				/* Notice the "waking up" */
				if(m_ptr->see_others || m_ptr->hear_noise)
				{
					char m_name[80];

					/* Acquire the creature name */
					creature_desc(m_name, m_ptr, 0);

					/* Dump a message */
#ifdef JP
					msg_format("%^sが目を覚ました。", m_name);
#else
					msg_format("%^s wakes up.", m_name);
#endif
				}
			}
		}

		/* Note */
		note_spot(floor_ptr, y, x);

		/* Redraw */
		lite_spot(floor_ptr, y, x);

		update_local_illumination(floor_ptr, y, x);
	}

	/* None left */
	temp_n = 0;
}



/*
 * This routine clears the entire "temp" set.
 *
 * This routine will "darken" all "temp" grids.
 *
 * In addition, some of these grids will be "unmarked".
 *
 * This routine is used (only) by "unlite_room()"
 *
 * Also, process all affected creatures
 */
static void cave_temp_room_unlite(floor_type *floor_ptr)
{
	int i;

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];
		int j;

		cave_type *c_ptr = &floor_ptr->cave[y][x];
		bool do_dark = !is_mirror_grid(c_ptr);

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Darken the grid */
		if(do_dark)
		{
			if(floor_ptr->floor_level || !is_daytime())
			{
				for (j = 0; j < 9; j++)
				{
					int by = y + ddy_ddd[j];
					int bx = x + ddx_ddd[j];

					if(in_bounds2(floor_ptr, by, bx))
					{
						cave_type *cc_ptr = &floor_ptr->cave[by][bx];

						if(have_flag(feature_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
						{
							do_dark = FALSE;
							break;
						}
					}
				}

				if(!do_dark) continue;
			}

			c_ptr->info &= ~(CAVE_GLOW);

			/* Hack -- Forget "boring" grids */
			if(!have_flag(feature_info[get_feat_mimic(c_ptr)].flags, FF_REMEMBER))
			{
				/* Forget the grid */
				if(!view_torch_grids) c_ptr->info &= ~(CAVE_MARK);

				/* Notice */
				note_spot(floor_ptr, y, x);
			}

			/* Process affected creatures */
			if(c_ptr->creature_idx)
			{
				/* Update the creature */
				update_creature_view(player_ptr, c_ptr->creature_idx, FALSE);
			}

			/* Redraw */
			lite_spot(floor_ptr, y, x);

			update_local_illumination(floor_ptr, y, x);
		}
	}

	/* None left */
	temp_n = 0;
}


/*
 * Determine how much contiguous open space this grid is next to
 */
static int next_to_open(floor_type *floor_ptr, int cy, int cx, bool (*pass_bold)(floor_type *, int, int))
{
	int i;

	int y, x;

	int len = 0;
	int blen = 0;

	for (i = 0; i < 16; i++)
	{
		y = cy + ddy_cdd[i % 8];
		x = cx + ddx_cdd[i % 8];

		// Found a wall, break the length
		if(!pass_bold(floor_ptr, y, x))
		{
			/* Track best length */
			if(len > blen)
			{
				blen = len;
			}

			len = 0;
		}
		else
		{
			len++;
		}
	}

	return (MAX(len, blen));
}


static int next_to_walls_adj(floor_type *floor_ptr, int cy, int cx, bool (*pass_bold)(floor_type *, int, int))
{
	int i;

	int y, x;

	int c = 0;

	for (i = 0; i < 8; i++)
	{
		y = cy + ddy_ddd[i];
		x = cx + ddx_ddd[i];

		if(!pass_bold(floor_ptr, y, x)) c++;
	}

	return c;
}

// Aux function -- see below
static void cave_temp_room_aux(creature_type *caster_ptr, int y, int x, bool only_room, bool (*pass_bold)(int, int))
{
	cave_type *c_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	c_ptr = &floor_ptr->cave[y][x]; // Get the grid

	if(c_ptr->info & (CAVE_TEMP)) return; // Avoid infinite recursion

	/* Do not "leave" the current room */
	if(!(c_ptr->info & (CAVE_ROOM)))
	{
		if(only_room) return;

		/* Verify */
		if(!in_bounds2(floor_ptr, y, x)) return;

		/* Do not exceed the maximum spell range */
		if(distance(caster_ptr->fy, caster_ptr->fx, y, x) > MAX_RANGE) return;

		/* Verify this grid */
		/*
		 * The reason why it is ==6 instead of >5 is that 8 is impossible
		 * due to the check for cave_bold above.
		 * 7 lights dead-end corridors (you need to do this for the
		 * checkboard interesting rooms, so that the boundary is lit
		 * properly.
		 * This leaves only a check for 6 bounding walls!
		 */
		if(in_bounds(floor_ptr, y, x) && pass_bold(y, x)) return;
		//    (next_to_walls_adj(floor_ptr, y, x, pass_bold) == 6) && (next_to_open(floor_ptr, y, x, pass_bold) <= 1)) return;
	}

	/* Paranoia -- verify space */
	if(temp_n == TEMP_MAX) return;

	/* Mark the grid as "seen" */
	c_ptr->info |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;
}

// Aux function -- see below
static void cave_temp_room_aux2(creature_type *caster_ptr, int y, int x, bool only_room, bool (*pass_bold)(floor_type *, int, int))
{
	cave_type *c_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	c_ptr = &floor_ptr->cave[y][x]; // Get the grid

	if(c_ptr->info & (CAVE_TEMP)) return; // Avoid infinite recursion

	/* Do not "leave" the current room */
	if(!(c_ptr->info & (CAVE_ROOM)))
	{
		if(only_room) return;

		/* Verify */
		if(!in_bounds2(floor_ptr, y, x)) return;

		/* Do not exceed the maximum spell range */
		if(distance(caster_ptr->fy, caster_ptr->fx, y, x) > MAX_RANGE) return;

		/* Verify this grid */
		/*
		 * The reason why it is ==6 instead of >5 is that 8 is impossible
		 * due to the check for cave_bold above.
		 * 7 lights dead-end corridors (you need to do this for the
		 * checkboard interesting rooms, so that the boundary is lit
		 * properly.
		 * This leaves only a check for 6 bounding walls!
		 */
		if(in_bounds(floor_ptr, y, x) && pass_bold(floor_ptr, y, x) &&
		    (next_to_walls_adj(floor_ptr, y, x, pass_bold) == 6) && (next_to_open(floor_ptr, y, x, pass_bold) <= 1)) return;
	}

	/* Paranoia -- verify space */
	if(temp_n == TEMP_MAX) return;

	/* Mark the grid as "seen" */
	c_ptr->info |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;
}

/*
 * Aux function -- see below
 */
static bool cave_pass_lite_bold(floor_type *floor_ptr, int y, int x)
{
	return cave_los_bold(floor_ptr, y, x);
}

/*
 * Aux function -- see below
 */
static void cave_temp_lite_room_aux(creature_type *caster_ptr, int y, int x)
{
	cave_temp_room_aux2(caster_ptr, y, x, FALSE, cave_pass_lite_bold);
}

/*
 * Aux function -- see below
 */
static bool cave_pass_dark_bold(floor_type *floor_ptr, int y, int x)
{
	return cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT);
}

/*
 * Aux function -- see below
 */
static void cave_temp_unlite_room_aux(creature_type *caster_ptr, int y, int x)
{
	cave_temp_room_aux2(caster_ptr, y, x, TRUE, cave_pass_dark_bold);
}




/*
 * Illuminate any room containing the given location.
 */
void lite_room(creature_type *creature_ptr, int y1, int x1)
{
	int i, x, y;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Add the initial grid */
	cave_temp_lite_room_aux(creature_ptr, y1, x1);

	/* While grids are in the queue, add their neighbors */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get lit, but stop light */
		if(!cave_pass_lite_bold(floor_ptr, y, x)) continue;

		/* Spread adjacent */
		cave_temp_lite_room_aux(creature_ptr, y + 1, x);
		cave_temp_lite_room_aux(creature_ptr, y - 1, x);
		cave_temp_lite_room_aux(creature_ptr, y, x + 1);
		cave_temp_lite_room_aux(creature_ptr, y, x - 1);

		/* Spread diagonal */
		cave_temp_lite_room_aux(creature_ptr, y + 1, x + 1);
		cave_temp_lite_room_aux(creature_ptr, y - 1, x - 1);
		cave_temp_lite_room_aux(creature_ptr, y - 1, x + 1);
		cave_temp_lite_room_aux(creature_ptr, y + 1, x - 1);
	}

	/* Now, lite them all up at once */
	cave_temp_room_lite(creature_ptr);

	if(creature_ptr->posture & NINJA_S_STEALTH)
	{
		if(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_GLOW) set_superstealth(creature_ptr, FALSE);
	}
}


/*
 * Darken all rooms containing the given location
 */
void unlite_room(creature_type *caster_ptr, int y1, int x1)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int i, x, y;

	cave_temp_unlite_room_aux(caster_ptr, y1, x1); // Add the initial grid

	// Spread, breadth first
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		// Walls get dark, but stop darkness
		if(!cave_pass_dark_bold(floor_ptr, y, x)) continue;

		// Spread adjacent
		cave_temp_unlite_room_aux(caster_ptr, y + 1, x);
		cave_temp_unlite_room_aux(caster_ptr, y - 1, x);
		cave_temp_unlite_room_aux(caster_ptr, y, x + 1);
		cave_temp_unlite_room_aux(caster_ptr, y, x - 1);

		// Spread diagonal
		cave_temp_unlite_room_aux(caster_ptr, y + 1, x + 1);
		cave_temp_unlite_room_aux(caster_ptr, y - 1, x - 1);
		cave_temp_unlite_room_aux(caster_ptr, y - 1, x + 1);
		cave_temp_unlite_room_aux(caster_ptr, y + 1, x - 1);
	}

	cave_temp_room_unlite(floor_ptr); // Now, darken them all at once
}

// Hack -- call light around the player
// Affect all creatures in the projection radius
bool lite_area(creature_type *creature_ptr, int dam, int rad)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS)
	{
#ifdef JP
		msg_print("ダンジョンが光を吸収した。");
#else
		msg_print("The darkness of this dungeon absorb your light.");
#endif
		return FALSE;
	}

	/* Hack -- Message */
	if(!has_trait(player_ptr, TRAIT_BLIND))
	{
#ifdef JP
		msg_print("白い光が辺りを覆った。");
#else
		msg_print("You are surrounded by a white light.");
#endif

	}

	/* Hook into the "project()" function */
	(void)project(creature_ptr, rad, creature_ptr->fy, creature_ptr->fx, dam, GF_LITE_WEAK, PROJECT_GRID | PROJECT_KILL, -1);

	/* Lite up the room */
	lite_room(creature_ptr, creature_ptr->fy, creature_ptr->fx);

	/* Assume seen */
	return (TRUE);
}


/*
 * Hack -- call darkness around the player
 * Affect all creatures in the projection radius
 */
bool unlite_area(creature_type *caster_ptr, int dam, int rad)
{

	/* Hack -- Message */
	if(!has_trait(player_ptr, TRAIT_BLIND))
	{
#ifdef JP
		msg_print("暗闇が辺りを覆った。");
#else
		msg_print("Darkness surrounds you.");
#endif

	}

	/* Hook into the "project()" function */
	(void)project(caster_ptr, rad, caster_ptr->fy, caster_ptr->fx, dam, GF_DARK_WEAK, PROJECT_GRID | PROJECT_KILL, -1);

	/* Lite up the room */
	unlite_room(caster_ptr, caster_ptr->fy, caster_ptr->fx);

	/* Assume seen */
	return (TRUE);
}



/*
 * Cast a ball spell
 * Stop if we hit a creature, act as a "ball"
 * Allow "target" mode to pass over creatures
 * Affect grids, objects, and creatures
 */
bool cast_ball(creature_type *caster_ptr, int typ, int dir, int dam, int rad)
{
	int tx, ty;

	//TODO if(typ == GF_CONTROL_LIVING) flg |= PROJECT_HIDE;

	/* Use the given direction */
	tx = caster_ptr->fx + 99 * ddx[dir];
	ty = caster_ptr->fy + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if((dir == 5) && target_okay(caster_ptr))
	{
		//TODO flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(caster_ptr, rad, ty, tx, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1));
}


/*
 * Cast a ball spell
 * Stop if we hit a creature, act as a "ball"
 * Allow "target" mode to pass over creatures
 * Affect grids, objects, and creatures
 */
bool fire_rocket(creature_type *caster_ptr, int typ, int dir, int dam, int rad)
{
	int tx, ty;

	/* Use the given direction */
	tx = caster_ptr->fx + 99 * ddx[dir];
	ty = caster_ptr->fy + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if((dir == 5) && target_okay(caster_ptr))
	{
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1));
}


/*
 * Cast a ball spell
 * Stop if we hit a creature, act as a "ball"
 * Allow "target" mode to pass over creatures
 * Affect grids, objects, and creatures
 */
bool cast_ball_hide(creature_type *caster_ptr, int typ, int dir, int dam, int rad)
{
	int tx, ty;

	/* Use the given direction */
	tx = caster_ptr->fx + 99 * ddx[dir];
	ty = caster_ptr->fy + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if((dir == 5) && target_okay(caster_ptr))
	{
		//TODO flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_HIDE, -1));
}


/*
 * Cast a meteor spell, defined as a ball spell cast by an arbitary creature, 
 * player, or outside source, that starts out at an arbitrary location, and 
 * leaving no trail from the "caster" to the target.  This function is 
 * especially useful for bombardments and similar. -LM-
 *
 * Option to hurt the player.
 */
bool fire_meteor(int who, int typ, int y, int x, int dam, int rad)
{
	/* Analyze the "target" and the caster. */
	return (project(&creature_list[who], rad, y, x, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1));
}


bool fire_blast(creature_type *caster_ptr, int typ, int dir, int dd, int ds, int num, int dev)
{
	int ly, lx, ld;
	int ty, tx, y, x;
	int i;

	/* Assume okay */
	bool result = TRUE;

	/* Use the given direction */
	if(dir != 5)
	{
		ly = ty = caster_ptr->fy + 20 * ddy[dir];
		lx = tx = caster_ptr->fx + 20 * ddx[dir];
	}

	/* Use an actual "target" */
	else /* if(dir == 5) */
	{
		tx = target_col;
		ty = target_row;

		lx = 20 * (tx - caster_ptr->fx) + caster_ptr->fx;
		ly = 20 * (ty - caster_ptr->fy) + caster_ptr->fy;
	}

	ld = distance(caster_ptr->fy, caster_ptr->fx, ly, lx);

	/* Blast */
	for (i = 0; i < num; i++)
	{
		while (1)
		{
			/* Get targets for some bolts */
			y = rand_spread(ly, ld * dev / 20);
			x = rand_spread(lx, ld * dev / 20);

			if(distance(ly, lx, y, x) <= ld * dev / 20) break;
		}

		/* Analyze the "dir" and the "target". */
		if(!project(0, 0, y, x, diceroll(dd, ds), typ, PROJECT_FAST | PROJECT_THRU | PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE | PROJECT_GRID, -1))
		{
			result = FALSE;
		}
	}

	return (result);
}


/*
 * Switch position with a creature.
 */
bool teleport_swap(creature_type *creature_ptr, int dir)
{
	int tx, ty;
	cave_type * c_ptr;
	creature_type * target_ptr;
	species_type * r_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if((dir == 5) && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;
	}
	else
	{
		tx = creature_ptr->fx + ddx[dir];
		ty = creature_ptr->fy + ddy[dir];
	}
	c_ptr = &floor_ptr->cave[ty][tx];

	if(has_trait(creature_ptr, TRAIT_PREVENT_TELEPORT))
	{
#ifdef JP
msg_print("不思議な力がテレポートを防いだ！");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif

		return FALSE;
	}

	if(!c_ptr->creature_idx || (c_ptr->creature_idx == creature_ptr->riding))
	{
#ifdef JP
msg_print("それとは場所を交換できません。");
#else
		msg_print("You can't trade places with that!");
#endif


		/* Failure */
		return FALSE;
	}

	if((c_ptr->info & CAVE_ICKY) || (distance(ty, tx, creature_ptr->fy, creature_ptr->fx) > creature_ptr->lev * 3 / 2 + 10))
	{
#ifdef JP
msg_print("失敗した。");
#else
		msg_print("Failed to swap.");
#endif


		/* Failure */
		return FALSE;
	}

	target_ptr = &creature_list[c_ptr->creature_idx];
	r_ptr = &species_info[target_ptr->species_idx];

	(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, 0);

	if(has_trait(target_ptr, TRAIT_RES_TELE))
	{
#ifdef JP
		msg_print("テレポートを邪魔された！");
#else
		msg_print("Your teleportation is blocked!");
#endif
		if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);

		/* Failure */
		return FALSE;
	}

	sound(SOUND_TELEPORT);

	// Swap the player and creature
	(void)move_creature(creature_ptr, NULL, ty, tx, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP);
	return TRUE;
}

/*
 * Some of the old functions
 */
bool lite_line(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_LITE_WEAK, dir, diceroll(6, 8), PROJECT_BEAM | PROJECT_GRID | PROJECT_KILL));
}


bool drain_life(creature_type *caster_ptr, int dir, int dam)
{
	return (project_hook(caster_ptr, GF_OLD_DRAIN, dir, dam, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}


bool wall_to_mud(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_KILL_WALL, dir, 20 + randint1(30), PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL));
}

bool wizard_lock(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_JAM_DOOR, dir, 20 + randint1(30), PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL));
}


bool destroy_door(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_KILL_DOOR, dir, 0, PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM));
}


bool disarm_trap(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_KILL_TRAP, dir, 0, PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM));
}


bool heal_other_creature(creature_type *caster_ptr, int dir, int dam)
{
	return (project_hook(caster_ptr, GF_OLD_HEAL, dir, dam, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}


bool speed_other_creature(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_OLD_SPEED, dir, caster_ptr->lev, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}


bool slow_creature(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_OLD_SLOW, dir, caster_ptr->lev, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}


bool sleep_creature(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_OLD_SLEEP, dir, caster_ptr->lev, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}

bool stasis_creature(creature_type *caster_ptr, int dir)
{
	return (cast_ball_hide(caster_ptr, GF_STASIS, dir, caster_ptr->lev*2, 0));
}

bool stasis_evil(creature_type *caster_ptr, int dir)
{
	return (cast_ball_hide(caster_ptr, GF_STASIS_EVIL, dir, caster_ptr->lev*2, 0));
}

bool confuse_creature(creature_type *caster_ptr, int dir, int plev)
{
	return (project_hook(caster_ptr, GF_OLD_CONF, dir, plev, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}

bool stun_creature(creature_type *caster_ptr, int dir, int plev)
{
	return (project_hook(caster_ptr, GF_STUN, dir, plev, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}

bool poly_creature(creature_type *caster_ptr, int dir)
{
	bool tester = (project_hook(caster_ptr, GF_OLD_POLY, dir, caster_ptr->lev, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
	return(tester);
}

bool clone_creature(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_OLD_CLONE, dir, 0, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}

bool fear_creature(creature_type *caster_ptr, int dir, int plev)
{
	return (project_hook(caster_ptr, GF_TURN_ALL, dir, plev, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}

bool death_ray(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_DEATH_RAY, dir, caster_ptr->lev * 200, PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE));
}

bool teleport_creature(creature_type *caster_ptr, int dir)
{
	return (project_hook(caster_ptr, GF_AWAY_ALL, dir, MAX_SIGHT * 5, PROJECT_BEAM | PROJECT_KILL));
}

bool door_creation(creature_type *caster_ptr)
{
	return (project(caster_ptr, 1, caster_ptr->fy, caster_ptr->fx, 0, GF_MAKE_DOOR, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1));
}


bool trap_creation(creature_type *caster_ptr, int y, int x)
{
	return (project(caster_ptr, 1, y, x, 0, GF_MAKE_TRAP, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1));
}


bool tree_creation(creature_type *caster_ptr)
{
	return (project(0, 1, caster_ptr->fy, caster_ptr->fx, 0, GF_MAKE_TREE, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1));
}


bool glyph_creation(creature_type *creature_ptr)
{
	return (project(0, 1, creature_ptr->fy, creature_ptr->fx, 0, GF_MAKE_GLYPH, PROJECT_GRID | PROJECT_ITEM, -1));
}


bool wall_stone(creature_type *caster_ptr)
{
	bool dummy = (project(caster_ptr, 1, caster_ptr->fy, caster_ptr->fx, 0, GF_STONE_WALL, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1));

	/* Update stuff */
	update |= (PU_FLOW);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	return dummy;
}


bool destroy_doors_touch(creature_type *caster_ptr)
{
	return (project(caster_ptr, 1, caster_ptr->fy, caster_ptr->fx, 0, GF_KILL_DOOR, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1));
}


bool sleep_creatures_touch(creature_type *creature_ptr)
{
	return (project(creature_ptr, 1, creature_ptr->fy, creature_ptr->fx, creature_ptr->lev, GF_OLD_SLEEP, PROJECT_KILL | PROJECT_HIDE, -1));
}


bool animate_dead(creature_type *creature_ptr, int y, int x)
{
	return (project(creature_ptr, 5, y, x, 0, GF_ANIM_DEAD, PROJECT_ITEM | PROJECT_HIDE, -1));
}


void call_chaos(creature_type *creature_ptr)
{
	int Chaos_type, dummy, dir;
	int plev = creature_ptr->lev;
	bool line_chaos = FALSE;

	int hurt_types[31] =
	{
		GF_ELEC, GF_POIS, GF_ACID, GF_COLD,
		GF_FIRE, GF_MISSILE, GF_ARROW, GF_PLASMA,
		GF_HOLY_FIRE, GF_WATER, GF_LITE, GF_DARK,
		GF_FORCE, GF_INERTIA, GF_MANA, GF_METEOR,
		GF_ICE, GF_CHAOS, GF_NETHER, GF_DISENCHANT,
		GF_SHARDS, GF_SOUND, GF_NEXUS, GF_CONFUSION,
		GF_TIME, GF_GRAVITY, GF_ROCKET, GF_NUKE,
		GF_HELL_FIRE, GF_DISINTEGRATE, GF_PSY_SPEAR
	};

	Chaos_type = hurt_types[randint0(31)];
	if(one_in_(4)) line_chaos = TRUE;

	if(one_in_(6))
	{
		for (dummy = 1; dummy < 10; dummy++)
		{
			if(dummy - 5)
			{
				if(line_chaos)
					cast_beam_(creature_ptr, Chaos_type, dummy, 150);
				else
					cast_ball(creature_ptr, Chaos_type, dummy, 150, 2);
			}
		}
	}
	else if(one_in_(3))
	{
		cast_ball(creature_ptr, Chaos_type, 0, 500, 8);
	}
	else
	{
		if(!get_aim_dir(creature_ptr, &dir)) return;
		if(line_chaos)
			cast_beam_(creature_ptr, Chaos_type, dir, 250);
		else
			cast_ball(creature_ptr, Chaos_type, dir, 250, 3 + (plev / 35));
	}
}


/*
 * Activate the evil Topi Ylinen curse
 * rr9: Stop the nasty things when a Cyberdemon is summoned
 * or the player gets paralyzed.
 */
bool activate_ty_curse(creature_type *creature_ptr, bool stop_ty, int *count)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int     i = 0, j;

	j = randint1(34);
	do
	{
		switch (j)
		{

		case 28: case 29:
			if(!(*count))
			{
#ifdef JP
				msg_print("地面が揺れた...");
#else
				msg_print("The ground trembles...");
#endif
				earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 5 + randint0(10));
				if(!one_in_(6)) break;
			}

		case 30: case 31:
			if(!(*count))
			{
				int dam = diceroll(10, 10);
#ifdef JP
				msg_print("純粋な魔力の次元への扉が開いた！");
#else
				msg_print("A portal opens to a plane of raw mana!");
#endif

				project(0, 8, creature_ptr->fy, creature_ptr->fx, dam, GF_MANA, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP, -1);
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, "純粋な魔力の解放", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, "released pure mana", NULL, -1);
#endif
				if(!one_in_(6)) break;
			}

		case 32: case 33:
			if(!(*count))
			{
#ifdef JP
				msg_print("周囲の空間が歪んだ！");
#else
				msg_print("Space warps about you!");
#endif
				teleport_player(creature_ptr, diceroll(10, 10), TELEPORT_PASSIVE);
				if(randint0(13)) (*count) += activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
				if(!one_in_(6)) break;
			}

		case 34:
#ifdef JP
msg_print("エネルギーのうねりを感じた！");
#else
			msg_print("You feel a surge of energy!");
#endif

			wall_breaker(creature_ptr);
			if(!randint0(7))
			{
				project(0, 7, creature_ptr->fy, creature_ptr->fx, 50, GF_KILL_WALL, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP, -1);
#ifdef JP
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 50, "エネルギーのうねり", NULL, -1);
#else
				take_hit(NULL, creature_ptr, DAMAGE_NOESCAPE, 50, "surge of energy", NULL, -1);
#endif
			}
			if(!one_in_(6)) break;

		case 1: case 2: case 3: case 16: case 17:
			aggravate_creatures(creature_ptr);
			if(!one_in_(6)) break;

		case 4: case 5: case 6:
			(*count) += activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
			if(!one_in_(6)) break;

		case 7: case 8: case 9: case 18:
			(*count) += summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->floor_level, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			if(!one_in_(6)) break;

		case 10: case 11: case 12:
#ifdef JP
msg_print("生命力が体から吸い取られた気がする！");
#else
			msg_print("You feel your life draining away...");
#endif

			lose_exp(creature_ptr, creature_ptr->exp / 16);
			if(!one_in_(6)) break;

		case 13: case 14: case 15: case 19: case 20:
			/*
			if(stop_ty || (has_trait(creature_ptr, TRAIT_FREE_ACTION) && (randint1(125) < creature_ptr->skill_rob)) || (creature_ptr->class_idx == CLASS_BERSERKER))
			{
				// Do nothing ;
			}
			else
			*/
			{
#ifdef JP
				msg_print("彫像になった気分だ！");
#else
				msg_print("You feel like a statue!");
#endif

				if(has_trait(creature_ptr, TRAIT_FREE_ACTION))
					set_timed_trait(creature_ptr, TRAIT_PARALYZED, creature_ptr->timed_trait[TRAIT_PARALYZED] + randint1(3));
				else
					set_timed_trait(creature_ptr, TRAIT_PARALYZED, creature_ptr->timed_trait[TRAIT_PARALYZED] + randint1(13));
				stop_ty = TRUE;
			}
			if(!one_in_(6)) break;

		case 21: case 22: case 23:

			(void)do_dec_stat(creature_ptr, randint0(6));
			if(!one_in_(6)) break;

		case 24:
#ifdef JP
			msg_print("ほえ？私は誰？ここで何してる？");
#else
			msg_print("Huh? Who am I? What am I doing here?");
#endif

			lose_all_info(creature_ptr);
			if(!one_in_(6)) break;

		case 25:
			/*
			 * Only summon Cyberdemons deep in the dungeon.
			 */
			if((floor_ptr->floor_level > 65) && !stop_ty)
			{
				(*count) += summon_cyber(NULL, creature_ptr->fy, creature_ptr->fx);
				stop_ty = TRUE;
				break;
			}
			if(!one_in_(6)) break;

		default:
			while (i < STAT_MAX)
			{
				do
				{
					(void)do_dec_stat(creature_ptr, i);
				}
				while (one_in_(2));

				i++;
			}
		}
	}

	while (one_in_(3) && !stop_ty);

	return stop_ty;
}


int activate_hi_summon(creature_type *creature_ptr, int y, int x, bool can_pet)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;
	int count = 0;
	int summon_lev;
	u32b mode = PC_ALLOW_GROUP;
	bool pet = FALSE;

	if(can_pet)
	{
		if(one_in_(4))
		{
			mode |= PC_FORCE_FRIENDLY;
		}
		else
		{
			mode |= PC_FORCE_PET;
			pet = TRUE;
		}
	}

	if(!pet) mode |= PC_NO_PET;

	summon_lev = (pet ? creature_ptr->lev * 2 / 3 + randint1(creature_ptr->lev / 2) : floor_ptr->floor_level);

	for (i = 0; i < (randint1(7) + (floor_ptr->floor_level / 40)); i++)
	{
		switch (randint1(25) + (floor_ptr->floor_level / 20))
		{
			case 1: case 2:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_ANT, mode);
				break;
			case 3: case 4:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_SPIDER, mode);
				break;
			case 5: case 6:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_HOUND, mode);
				break;
			case 7: case 8:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_HYDRA, mode);
				break;
			case 9: case 10:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_ANGEL, mode);
				break;
			case 11: case 12:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_UNDEAD, mode);
				break;
			case 13: case 14:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_DRAGON, mode);
				break;
			case 15: case 16:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_DEMON, mode);
				break;
			case 17:
				if(can_pet) break;
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_AMBERITES, (mode | PC_ALLOW_UNIQUE));
				break;
			case 18: case 19:
				if(can_pet) break;
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_UNIQUE, (mode | PC_ALLOW_UNIQUE));
				break;
			case 20: case 21:
				if(!can_pet) mode |= PC_ALLOW_UNIQUE;
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_HI_UNDEAD, mode);
				break;
			case 22: case 23:
				if(!can_pet) mode |= PC_ALLOW_UNIQUE;
				count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, SUMMON_HI_DRAGON, mode);
				break;
			case 24:
				count += summon_specific((pet ? creature_ptr : NULL), y, x, 100, SUMMON_CYBER, mode);
				break;
			default:
				if(!can_pet) mode |= PC_ALLOW_UNIQUE;
				count += summon_specific((pet ? creature_ptr : NULL), y, x,pet ? summon_lev : (((summon_lev * 3) / 2) + 5), 0, mode);
		}
	}

	return count;
}

int summon_cyber(creature_type *summoner_ptr, int y, int x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(summoner_ptr);
	int i;
	int max_cyber = (floor_ptr->floor_level / 50) + randint1(2);
	int count = 0;
	u32b mode = PC_ALLOW_GROUP;

	// Summoned by a creature
	if(summoner_ptr) if(is_pet(player_ptr, summoner_ptr)) mode |= PC_FORCE_PET;

	if(max_cyber > 4) max_cyber = 4;

	for (i = 0; i < max_cyber; i++)
	{
		count += summon_specific(summoner_ptr, y, x, 100, SUMMON_CYBER, mode);
	}

	return count;
}


void wall_breaker(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;
	int y, x;
	int attempts = 1000;

	if(randint1(80 + creature_ptr->lev) < 70)
	{
		while (attempts--)
		{
			scatter(floor_ptr, &y, &x, creature_ptr->fy, creature_ptr->fx, 4, 0);
			if(!cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)) continue;
			if(!creature_bold(creature_ptr, y, x)) break;
		}
		project(0, 0, y, x, 20 + randint1(30), GF_KILL_WALL, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
	}
	else if(randint1(100) > 30)
	{
		earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 1);
	}
	else
	{
		int num = diceroll(5, 3);

		for (i = 0; i < num; i++)
		{
			while (1)
			{
				scatter(floor_ptr, &y, &x, creature_ptr->fy, creature_ptr->fx, 10, 0);
				if(!creature_bold(creature_ptr, y, x)) break;
			}
			project(0, 0, y, x, 20 + randint1(30), GF_KILL_WALL, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
		}
	}
}


/*
 * Confuse creatures
 */
bool confuse_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_OLD_CONF, dam));
}

/*
 * Charm animals
 */
bool charm_animals(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_CONTROL_ANIMAL, dam));
}


/*
 * Stun creatures
 */
bool stun_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_STUN, dam));
}


/*
 * Stasis creatures
 */
bool stasis_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_STASIS, dam));
}


/*
 * Mindblast creatures
 */
bool mindblast_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_PSI, dam));
}


/*
 * Banish all creatures
 */
bool banish_creatures(creature_type *caster_ptr, int dist)
{
	return (project_hack(caster_ptr, GF_AWAY_ALL, dist));
}


/*
 * Turn evil
 */
bool turn_evil(creature_type *caster_ptr,int dam)
{
	return (project_hack(caster_ptr, GF_TURN_EVIL, dam));
}


/*
 * Turn everyone
 */
bool turn_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_TURN_ALL, dam));
}


/*
 * Death-ray all creatures (note: OBSCENELY powerful)
 */
bool deathray_creatures(creature_type *caster_ptr)
{
	return (project_hack(caster_ptr, GF_DEATH_RAY, caster_ptr->lev * 200));
}


bool charm_creature(creature_type *caster_ptr, int dir, int power)
{
	return (project_hook(caster_ptr, GF_CHARM, dir, power, PROJECT_STOP | PROJECT_KILL));
}


bool control_one_undead(creature_type *caster_ptr, int dir, int plev)
{
	return (project_hook(caster_ptr, GF_CONTROL_UNDEAD, dir, plev, PROJECT_STOP | PROJECT_KILL));
}


bool control_one_demon(creature_type *caster_ptr, int dir, int plev)
{
	return (project_hook(caster_ptr, GF_CONTROL_DEMON, dir, plev, PROJECT_STOP | PROJECT_KILL));
}


bool charm_animal(creature_type *caster_ptr, int dir, int plev)
{
	return (project_hook(caster_ptr, GF_CONTROL_ANIMAL, dir, plev, PROJECT_STOP | PROJECT_KILL));
}


bool charm_living(creature_type *caster_ptr, int dir, int plev)
{
	return (project_hook(caster_ptr, GF_CONTROL_LIVING, dir, plev, PROJECT_STOP | PROJECT_KILL));
}


bool kawarimi(creature_type *user_ptr, bool success)
{
	object_type forge;
	object_type *quest_ptr = &forge;
	int y, x;
	char user_name[80];
	floor_type *floor_ptr = GET_FLOOR_PTR(user_ptr);

	creature_desc(user_name, user_ptr, 0);

	if(!(user_ptr->posture & NINJA_KAWARIMI) || !(randint0(55) < (user_ptr->lev*3/5+20))) return FALSE;

	if(gameover) return FALSE;
	if(user_ptr->timed_trait[TRAIT_CONFUSED] || has_trait(user_ptr, TRAIT_BLIND) || user_ptr->timed_trait[TRAIT_PARALYZED] || user_ptr->timed_trait[TRAIT_HALLUCINATION]) return FALSE;
	if(randint0(200) < user_ptr->timed_trait[TRAIT_STUN]) return FALSE;

	if(!success && one_in_(3))
	{
#ifdef JP
		msg_format("失敗！%sは変わり身で逃げられなかった。", user_name);
#else
		msg_format("Failed! %s couldn't run away.", user_name);
#endif
		user_ptr->posture &= ~(NINJA_KAWARIMI);
		play_redraw |= (PR_STATUS);
		return FALSE;
	}

	y = user_ptr->fy;
	x = user_ptr->fx;

	teleport_player(user_ptr, 10 + randint1(90), 0L);

	object_wipe(quest_ptr);

	object_prep(quest_ptr, lookup_kind(TV_STATUE, SV_WOODEN_STATUE), ITEM_FREE_SIZE);

	quest_ptr->pval = SPECIES_NINJA;

	/* Drop it in the dungeon */
	(void)drop_near(floor_ptr, quest_ptr, -1, y, x);

#ifdef JP
	if(success) msg_format("%sは攻撃を受ける前に素早く身をひるがえした。", user_name);
	else msg_format("%sは変わり身に失敗して攻撃を受けた。", user_name);
#else
	if(success) msg_format("%s have turned around just before the attack hit you.", user_name); // TODO
	else msg_format("Failed! %s are hit by the attack.", user_name); // TODO
#endif

	user_ptr->posture &= ~(NINJA_KAWARIMI);
	play_redraw |= (PR_STATUS);

	/* Teleported */
	return TRUE;
}


// "Rush Attack" routine for Samurai or Ninja
// Return value is for checking "done"
bool rush_attack(creature_type *creature_ptr, bool *mdeath)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int dir;
	int tx, ty;
	int tm_idx = 0;
	u16b path_g[32];
	int path_n, i;
	bool tmp_mdeath = FALSE;
	bool moved = FALSE;

	if(mdeath) *mdeath = FALSE;

	project_length = 5;
	if(!get_aim_dir(creature_ptr, &dir)) return FALSE;

	/* Use the given direction */
	tx = creature_ptr->fx + project_length * ddx[dir];
	ty = creature_ptr->fy + project_length * ddy[dir];

	/* Hack -- Use an actual "target" */
	if((dir == 5) && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;
	}

	if(in_bounds(floor_ptr, ty, tx)) tm_idx = floor_ptr->cave[ty][tx].creature_idx;

	path_n = project_path(path_g, project_length, floor_ptr, creature_ptr->fy, creature_ptr->fx, ty, tx, PROJECT_STOP | PROJECT_KILL);
	project_length = 0;

	/* No need to move */
	if(!path_n) return TRUE;

	/* Use ty and tx as to-move point */
	ty = creature_ptr->fy;
	tx = creature_ptr->fx;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		creature_type *m_ptr;

		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		if(cave_empty_bold(floor_ptr, ny, nx) && creature_can_cross_terrain(creature_ptr, floor_ptr->cave[ny][nx].feat, 0))
		{
			ty = ny;
			tx = nx;

			/* Go to next grid */
			continue;
		}

		if(!floor_ptr->cave[ny][nx].creature_idx)
		{
			if(tm_idx)
			{
#ifdef JP
				msg_print("失敗！");
#else
				msg_print("Failed!");
#endif
			}
			else
			{
#ifdef JP
				msg_print("ここには入身では入れない。");
#else
				msg_print("You can't move to that place.");
#endif
			}

			/* Exit loop */
			break;
		}

		/* Move player before updating the creature */
		if(!creature_bold(creature_ptr, ty, tx)) teleport_creature_to(creature_ptr, ty, tx, TELEPORT_NONMAGICAL);

		/* Update the creature */
		update_creature_view(player_ptr, floor_ptr->cave[ny][nx].creature_idx, TRUE);

		/* Found a creature */
		m_ptr = &creature_list[floor_ptr->cave[ny][nx].creature_idx];

		if(tm_idx != floor_ptr->cave[ny][nx].creature_idx)
		{
			msg_print(game_messages[GAME_MESSAGE_CREATURE_IN_THE_WAY]);
		}
		else if(!creature_bold(creature_ptr, ty, tx))
		{
			/* Hold the creature name */
			char m_name[80];

			/* Get the creature name (BEFORE polymorphing) */
			creature_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("素早く%sの懐に入り込んだ！", m_name);
#else
			msg_format("You quickly jump in and attack %s!", m_name);
#endif
		}

		if(!creature_bold(creature_ptr, ty, tx)) teleport_creature_to(creature_ptr, ty, tx, TELEPORT_NONMAGICAL);
		moved = TRUE;
		tmp_mdeath = melee_attack(creature_ptr, ny, nx, HISSATSU_NYUSIN);

		break;
	}

	if(!moved && !creature_bold(creature_ptr, ty, tx)) teleport_creature_to(creature_ptr, ty, tx, TELEPORT_NONMAGICAL);

	if(mdeath) *mdeath = tmp_mdeath;
	return TRUE;
}



// Remove all mirrors in this floor
void remove_all_mirrors(creature_type *user_ptr, floor_type *floor_ptr, bool explode)
{
	int x, y;

	for (x = 0; x < floor_ptr->width; x++)
	{
		for (y = 0; y < floor_ptr->height; y++)
		{
			if(is_mirror_grid(&floor_ptr->cave[y][x]))
			{
				remove_mirror(user_ptr, y, x);
				if(explode)
					project(0, 2, y, x, user_ptr->lev / 2 + 5, GF_SHARDS,
						(PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}
		}
	}
}
