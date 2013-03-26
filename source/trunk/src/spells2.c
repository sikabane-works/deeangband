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

bool cast_bolt(creature_type *caster_ptr, int typ, COODINATES range, POWER dam, int trait_id)
{
	return project(caster_ptr, range, 0, target_col, target_row, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_KILL | PROJECT_REFLECTABLE, trait_id);
}

bool cast_beam(creature_type *caster_ptr, int typ, COODINATES range, POWER dam, int trait_id)
{
	return project(caster_ptr, range, 0, target_col, target_row, dam, typ, PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, trait_id);
}

bool cast_bolt_or_beam(creature_type *caster_ptr, int typ, COODINATES range, POWER dam, int prob)
{
	if(PERCENT(prob)) return (cast_beam(caster_ptr, range, typ, dam, 0));
	else return cast_bolt(caster_ptr, typ, range, dam, 0);
}

void breath(creature_type *caster_ptr, int typ, COODINATES range, POWER power, COODINATES rad, int trait_id)
{
	(void)project(caster_ptr, range, rad, target_col, target_row, power, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_BREATH, trait_id);
}

void cast_ball_aux(COODINATES y, COODINATES x, creature_type *caster_ptr, int typ, POWER power, COODINATES rad, int trait_id)
{
	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	(void)project(caster_ptr, 0, rad, y, x, power, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, trait_id);
}

bool cast_ball(creature_type *caster_ptr, int typ, COODINATES range, POWER dam, COODINATES rad)
{
	COODINATES tx = 0, ty = 0;
	int dir = 0;
	if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(caster_ptr, range, rad, ty, tx, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1));
}

bool cast_grenade(creature_type *caster_ptr, int typ, COODINATES range, POWER dam, COODINATES rad)
{
	COODINATES tx = 0, ty = 0;
	return (project(caster_ptr, range, rad, ty, tx, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1));
}

bool cast_ball_hide(creature_type *caster_ptr, int typ, COODINATES range, POWER dam, COODINATES rad)
{
	COODINATES tx, ty;
	tx = target_col;
	ty = target_row;
	return (project(caster_ptr, range, rad, ty, tx, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_HIDE, -1));
}



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
*  Use the "show_file()" method, perhaps.
*/
void creature_knowledge(creature_type *creature_ptr)
{
	int i = 0, j, k;

	int v_nr = 0;
	char v_string [MAX_KARMA][128];
	char s_string [6] [128];
	u32b flgs[MAX_TRAITS_FLAG];
	object_type *object_ptr;

	char Dummy[80];
	char Dummy2[10][80];
	char buf[2][80];
	char buf2[100];
	cptr info[280];

	int percent;

	Term_clear();

	for (j = 0; j < MAX_TRAITS_FLAG; j++)
		flgs[j] = 0L;

	creature_ptr->knowledge |= (KNOW_STAT | KNOW_HPRATE);

	strcpy(Dummy, "");

	percent = (int)(((long)creature_ptr->base_hp[CREATURE_MAX_LEVEL - 1] * 200L) / (2 * creature_ptr->hitdice +
		((CREATURE_MAX_LEVEL - 1+3) * (creature_ptr->hitdice + 1))));

	sprintf(Dummy, MES_CREATURE_LIFE_RATE(percent));

	strcpy(buf[0], Dummy);
	info[i++] = buf[0];
	info[i++] = "";

	/* Acquire item flags from equipment */
	for (k = 0; k < INVEN_TOTAL; k++)
	{
		u32b tflgs[MAX_TRAITS_FLAG];

		object_ptr = &creature_ptr->inventory[k];
		if(!IS_EQUIPPED(object_ptr)) continue;

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Extract the flags */
		object_flags(object_ptr, tflgs);

		/* Extract flags */
		for (j = 0; j < MAX_TRAITS_FLAG; j++)
			flgs[j] |= tflgs[j];
	}

#ifdef JP
	info[i++] = "î\óÕÇÃç≈ëÂíl";
#else
	info[i++] = "Limits of maximum stats";
#endif

	for (v_nr = 0; v_nr < STAT_MAX; v_nr++)
	{
		char stat_desc[80];
		int n = creature_ptr->stat_mod_max_max[v_nr];
		sprintf(stat_desc, "%s %2d.%d", stat_names[v_nr], n / STAT_FRACTION, n % STAT_FRACTION);
		strcpy(s_string[v_nr], stat_desc);

		info[i++] = s_string[v_nr];
	}
	info[i++] = "";

	show_alignment(buf2, creature_ptr);
	sprintf(Dummy, "%s: %s", KW_ALIGNMENT, buf2);
	strcpy(buf[1], Dummy);
	info[i++] = buf[1];
	for (v_nr = 0; v_nr < MAX_KARMA; v_nr++)
	{
		if(creature_ptr->karmas[v_nr] > 0)
		{
			MES_CREATURE_KARMA(karma[v_nr].title, creature_ptr->karmas[v_nr]);
			info[i++] = v_string[v_nr];
		}
	}

	info[i++] = "";


	if(is_player(creature_ptr))
	{
#ifdef JP
		info[i++] = "Ç†Ç»ÇΩÇÕÉvÉåÉCÉÑÅ[Ç≈Ç†ÇÈÅB";
#else
		info[i++] = "You are a player. ";
#endif
	}

	if(IS_DEAD(creature_ptr))
	{
#ifdef JP
		info[i++] = "Ç†Ç»ÇΩÇÕéÄÇÒÇ≈Ç¢ÇÈ";
#else
		info[i++] = "You are dead. ";
#endif
	}


	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_GOOD);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[0], "Ç†Ç»ÇΩÇÕÅqëPÅrÇÃìGÇ≈Ç†ÇÈÅB(ëŒé◊ÉXÉåÉCx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[0], "You are a enemy of good(x%d.%02d by evil slaying).", percent / 100, percent % 100);
#endif
		info[i++] = Dummy2[0];
	}

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_EVIL);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[1], "Ç†Ç»ÇΩÇÕÅqà´ÅrÇÃìGÇ≈Ç†ÇÈÅB(ëŒëPÉXÉåÉCx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[1], "You are a enemy of evil(x%d.%02d by good slaying).", percent / 100, percent % 100);
#endif
		info[i++] = Dummy2[1];
	}

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_ORDER);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[2], "Ç†Ç»ÇΩÇÕÅqíÅèòÅrÇÃìGÇ≈Ç†ÇÈÅB(ëŒç¨ì◊ÉXÉåÉCx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[2], "You are a enemy of order(x%d.%02d by chaos slaying).", percent / 100, percent % 100);
#endif
		info[i++] = Dummy2[2];
	}

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_CHAOS);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[3], "Ç†Ç»ÇΩÇÕÅqç¨ì◊ÅrÇÃìGÇ≈Ç†ÇÈÅB(ëŒíÅèòÉXÉåÉCx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[3], "You are a enemy of chaos(x%d.%02d by order slaying.)", percent / 100, percent % 100);
#endif
		info[i++] = Dummy;
	}

	percent = calc_punishment_slay(creature_ptr, ALIGNMENT_BALANCE);
	if(percent > 100)
	{
#ifdef JP
		sprintf(Dummy2[4], "Ç†Ç»ÇΩÇÕÅqìVîâÅrÇÃìGÇ≈Ç†ÇÈÅB(ÉXÉåÉCx%d.%02d)", percent / 100, percent % 100);
#else
		sprintf(Dummy2[4], "You are a enemy of balance(x%d.%02d by slaying).", percent / 100, percent % 100);
#endif
		info[i++] = Dummy2[4];
	}


	switch (creature_ptr->action)
	{
	case ACTION_SEARCH:
#ifdef JP
		info[i++] = "Ç†Ç»ÇΩÇÕÇ–Ç∂ÇÂÇ§Ç…íçà”ê[Ç≠é¸àÕÇå©ìnÇµÇƒÇ¢ÇÈÅB";
#else
		info[i++] = "You are looking around very carefully.";
#endif
		break;
	}

	if(creature_ptr->new_spells)
	{
#ifdef JP
		info[i++] = "Ç†Ç»ÇΩÇÕéÙï∂Ç‚ãFÇËÇäwÇ‘Ç±Ç∆Ç™Ç≈Ç´ÇÈÅB";
#else
		info[i++] = "You can learn some spells/prayers.";
#endif

	}

	//TODO old pval status

	/* Access the current weapon */
	object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 0);

	screen_save();

	for (k = 1; k < 24; k++) prt("", k, 3);

	/* Label the information */
#ifdef JP
	prt("        Ç†Ç»ÇΩÇÃèÛë‘:", 1, 5);
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
			prt(MES_SYS_MORE, k, 5);
			inkey();
			for (; k > 2; k--) prt("", k, 5);
		}
	}

	/* Pause */
	prt(MES_SYS_HIT_ANY_KEY, k, 3);
	inkey();

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
	"Ç≤Ç≠íZÇ¢ä‘",
	"è≠ÇµÇÃä‘",
	"ÇµÇŒÇÁÇ≠ÇÃä‘",
	"ëΩè≠í∑Ç¢ä‘",
	"í∑Ç¢ä‘",
	"îÒèÌÇ…í∑Ç¢ä‘",
	"êMÇ∂ìÔÇ¢ÇŸÇ«í∑Ç¢ä‘",
	"ÉNÉäÅ[É`ÉÉÅ[ÇçUåÇÇ∑ÇÈÇ‹Ç≈"
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

static bool detect_feat_flag(creature_type *creature_ptr, COODINATES range, int flag, bool known)
{
	COODINATES  x, y;
	bool detect = FALSE;
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

					lite_spot(floor_ptr, y, x);
				}
			}

			/* Detect flags */
			if(CAVE_HAVE_FLAG_GRID(c_ptr, flag))
			{
				/* Detect secrets */
				disclose_grid(floor_ptr, y, x);

				/* Hack -- Memorize */
				c_ptr->info |= (CAVE_MARK);

				lite_spot(floor_ptr, y, x);

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	return detect;
}


/*
* Detect all traps on current panel
*/
bool detect_traps(creature_type *creature_ptr, COODINATES range, bool known)
{
	bool detect = detect_feat_flag(creature_ptr, range, FF_TRAP, known);

	if(known) detect_trap = TRUE;

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 0) detect = FALSE;

	if(detect) msg_print(MES_DETECT_TRAPS);
	return detect;
}


/*
* Detect all doors on current panel
*/
bool detect_doors(creature_type *creature_ptr, COODINATES range)
{
	bool detect = detect_feat_flag(creature_ptr, range, FF_DOOR, TRUE);
	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 0) detect = FALSE;
	if(detect) msg_print(MES_DETECT_DOORS);
	return detect;
}


/*
* Detect all stairs on current panel
*/
bool detect_stairs(creature_type *creature_ptr, COODINATES range)
{
	bool detect = detect_feat_flag(creature_ptr, range, FF_STAIRS, TRUE);
	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 0) detect = FALSE;
	if(detect) msg_print(MES_DETECT_STAIRS);
	return detect;
}


/*
* Detect any treasure on the current panel
*/
bool detect_treasure(creature_type *creature_ptr, COODINATES range)
{
	bool detect = detect_feat_flag(creature_ptr, range, FF_HAS_GOLD, TRUE);
	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 6) detect = FALSE;
	if(detect) msg_print(MES_DETECT_B_TREASURE);
	return detect;
}



// Detect all "gold" objects on the current panel
bool detect_objects_gold(creature_type *creature_ptr, COODINATES range)
{
	int i;
	COODINATES y, x;
	COODINATES range2 = range;

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

		y = object_ptr->fy;
		x = object_ptr->fx;

		/* Only detect nearby objects */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range2) continue;

		/* Detect "gold" objects */
		if(object_ptr->tval == TV_GOLD)
		{
			/* Hack -- memorize it */
			object_ptr->marked |= OM_FOUND;

			lite_spot(floor_ptr, y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 6) detect = FALSE;
	if(detect) msg_print(MES_DETECT_TREASURE);
	if(detect_creatures_string(creature_ptr, range, "$")) detect = TRUE;

	return (detect);
}


/*
* Detect all "normal" objects on the current panel
*/
bool detect_objects_normal(creature_type *creature_ptr, COODINATES range)
{
	int i;
	COODINATES y, x;
	COODINATES range2 = range;
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

		y = object_ptr->fy;
		x = object_ptr->fx;

		/* Only detect nearby objects */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range2) continue;

		/* Detect "real" objects */
		if(object_ptr->tval != TV_GOLD)
		{
			/* Hack -- memorize it */
			object_ptr->marked |= OM_FOUND;

			lite_spot(floor_ptr, y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 6) detect = FALSE;
	if(detect) msg_print(MES_DETECT_OBJECTS);
	if(detect_creatures_string(creature_ptr, range, "!=?|/`")) detect = TRUE;
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
bool detect_objects_magic(creature_type *creature_ptr, COODINATES range)
{
	int i;
	COODINATES y, x;
	TVAL tv;
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

			lite_spot(floor_ptr, y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if(detect) msg_print(MES_DETECT_M_OBJECTS);
	return (detect);
}


/*
* Detect all "normal" creatures on the current panel
*/
bool detect_creatures_normal(creature_type *creature_ptr, COODINATES range)
{
	int i;
	COODINATES y, x;

	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *target_ptr = &creature_list[i];

		/* Skip dead creatures */
		if(!target_ptr->species_idx) continue;

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

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 3) flag = FALSE;
	if(flag) msg_print(MES_DETECT_CREATURES);
	return (flag);
}


/*
* Detect all "invisible" creatures around the player
*/
bool detect_creatures_invis(creature_type *creature_ptr, COODINATES range)
{
	int i;
	COODINATES y, x;
	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		if(!is_valid_creature(m_ptr)) continue;

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
				prepare_window(PW_MONSTER);
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

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 3) flag = FALSE;
	if(flag) msg_print(MES_DETECT_I_CREATURES);
	return (flag);
}



/*
* Detect all "evil" creatures on current panel
*/
bool detect_creatures_evil(creature_type *creature_ptr, COODINATES range)
{
	int i;
	COODINATES y, x;
	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Skip dead creatures */
		if(!is_valid_creature(m_ptr)) continue;

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
					prepare_window(PW_MONSTER);
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

	if(flag) msg_print(MES_DETECT_E_CREATURES);
	return (flag);
}




/*
* Detect all "nonliving", "undead" or "demonic" creatures on current panel
*/
bool detect_creatures_nonliving(creature_type *creature_ptr, COODINATES range)
{
	int i;
	COODINATES y, x;
	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

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
				prepare_window(PW_MONSTER);
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

	if(flag) msg_print(MES_DETECT_NOLIVING);
	return (flag);
}


/*
* Detect all creatures it has mind on current panel
*/
bool detect_creatures_mind(creature_type *creature_ptr, COODINATES range)
{
	int i;
	COODINATES y, x;
	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

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
				prepare_window(PW_MONSTER);
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

	if(flag) msg_print(MES_DETECT_S_CREATURES);
	return (flag);
}


/*
* Detect all (string) creatures on current panel
*/
bool detect_creatures_string(creature_type *creature_ptr, COODINATES range, cptr Match)
{
	int i, y, x;
	bool flag = FALSE;

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *species_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect creatures with the same symbol */
		if(my_strchr(Match, species_ptr->d_char))
		{
			/* Update creature recall window */
			if(species_window_idx == m_ptr->species_idx)
			{
				prepare_window(PW_MONSTER);
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

	if(MUSIC_SINGING(creature_ptr, MUSIC_DETECT) && creature_ptr->singing_turn > 3) flag = FALSE;
	if(flag) msg_print(MES_DETECT_CREATURES);
	return (flag);
}


/*
* A "generic" detect creatures routine, tagged to flags3
*/
bool detect_creatures_xxx(creature_type *creature_ptr, COODINATES range, u32b match_flag)
{
	int  i;
	COODINATES y, x;
	bool flag = FALSE;
#ifdef JP
	cptr desc_creatures = "ïœÇ»ÉNÉäÅ[É`ÉÉÅ[";
#else
	cptr desc_creatures = "weird creatures";
#endif

	if(dungeon_info[GET_FLOOR_PTR(creature_ptr)->dun_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Skip dead creatures */
		if(!m_ptr->species_idx) continue;

		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby creatures */
		if(distance(creature_ptr->fy, creature_ptr->fx, y, x) > range) continue;

		/* Detect evil creatures */
		//TODO if(species_ptr->flags3 & (match_flag))
		if(1)
		{
			if(is_original_ap(m_ptr))
			{
				/* Take note that they are something */
				//TODO species_ptr->r_flags3 |= (match_flag);

				/* Update creature recall window */
				if(species_window_idx == m_ptr->species_idx)
				{
					prepare_window(PW_MONSTER);
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

	if(flag)
	{
		switch (match_flag)
		{
		default:
#ifdef JP
			desc_creatures = "ìG";
#else
			desc_creatures = "the enemy";
#endif
			break;
		}

		msg_print(MES_DETECT_SOMEONE);
		msg_print(NULL);
	}

	return (flag);
}


/*
* Detect everything
*/
bool detect_all(creature_type *creature_ptr, COODINATES range)
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
bool project_all_vision(creature_type *caster_ptr, int typ, POWER dam)
{
	int i;
	COODINATES x, y;
	bool obvious = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	creature_type *target_ptr;

	for (i = 1; i < creature_max; i++)	// Mark all (nearby) creatures
	{
		target_ptr = &creature_list[i];
		if(!is_valid_creature(target_ptr) || IS_IN_THIS_FLOOR(target_ptr)) continue;	// Paranoia -- Skip dead creatures

		// Require line of sight
		y = target_ptr->fy;
		x = target_ptr->fx;
		if(!player_has_los_bold(y, x) || !projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, y, x)) continue;

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
		if(project(caster_ptr, 0, 0, y, x, dam, typ, PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE, -1)) obvious = TRUE;
	}

	return (obvious);	// Result
}

/*
* Wake up all creatures, and speed up "los" creatures.
*/
void aggravate_creatures(creature_type *target_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
	int     i;
	bool    sleep = FALSE;
	bool    speed = FALSE;

	/* Aggravate everyone nearby */
	for (i = 1; i < creature_max; i++)
	{
		creature_type    *creature_ptr = &creature_list[i];

		if(!creature_ptr->species_idx) continue;
		if(!IS_IN_THIS_FLOOR(target_ptr)) continue;

		/* Skip aggravating creature (or player) */
		if(creature_ptr == target_ptr) continue;

		/* Wake up nearby sleeping creatures */
		if(creature_ptr->cdis < MAX_SIGHT * 2)
		{
			/* Wake up */
			if(has_trait_from_timed(creature_ptr, TRAIT_SLEPT))
			{
				(void)set_timed_trait(creature_ptr, TRAIT_SLEPT, 0, TRUE);
				sleep = TRUE;
			}
			if(!is_pet(player_ptr, creature_ptr)) set_timed_trait(creature_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
		}

		/* Speed up creatures in line of sight */
		if(player_has_los_bold(creature_ptr->fy, creature_ptr->fx))
		{
			if(!is_pet(player_ptr, creature_ptr))
			{
				(void)set_timed_trait(creature_ptr, TRAIT_FAST, 100, TRUE);
				speed = TRUE;
			}
		}
	}

#ifdef JP
	if(speed) msg_print("ïtãﬂÇ≈âΩÇ©Ç™ìÀî@ãªï±ÇµÇΩÇÊÇ§Ç»ä¥Ç∂ÇéÛÇØÇΩÅI");
	else if(sleep) msg_print("âΩÇ©Ç™ìÀî@ãªï±ÇµÇΩÇÊÇ§Ç»ëõÅXÇµÇ¢âπÇ™âìÇ≠Ç…ï∑Ç±Ç¶ÇΩÅI");
#else
	if(speed) msg_print("You feel a sudden stirring nearby!");
	else if(sleep) msg_print("You hear a sudden stirring in the distance!");
#endif

	if(target_ptr->riding) prepare_update(target_ptr, CRU_BONUS);
}



// Delete a non-unique/non-quest creature
bool genocide_aux(creature_type *user_ptr, int m_idx, POWER power, bool player_cast, POWER dam_side, cptr spell_name)
{
	int          msec = delay_factor * delay_factor * delay_factor;
	creature_type *target_ptr = &creature_list[m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);
	bool         resist = FALSE;
	char target_name[MAX_NLEN];

	if(is_pet(player_ptr, target_ptr) && !player_cast) return FALSE;

	// Hack -- Skip Unique Creatures or Quest Creatures
	if((has_trait(target_ptr, TRAIT_QUESTOR)) || has_trait(target_ptr, TRAIT_UNIQUE)) resist = TRUE;
	else if(has_trait(target_ptr, TRAIT_UNIQUE2)) resist = TRUE;
	else if(m_idx == user_ptr->riding) resist = TRUE;
	else if((floor_ptr->quest && !random_quest_number(floor_ptr)) || floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode) resist = TRUE;
	else if(player_cast && (target_ptr->lev * 2 > randint0(power))) resist = TRUE;
	else if(player_cast && has_trait(target_ptr, TRAIT_NO_GENOCIDE)) resist = TRUE;

	// Delete the creature
	else
	{
		if(record_named_pet && is_pet(player_ptr, target_ptr) && target_ptr->nickname)
		{
			creature_desc(target_name, target_ptr, CD_INDEF_VISIBLE);
			do_cmd_write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, target_name);
		}

		delete_species_idx(&creature_list[m_idx]);
	}

	if(resist && player_cast)
	{
		bool see_m = is_seen(user_ptr, target_ptr);

		creature_desc(target_name, target_ptr, 0);
		if(see_m) msg_format(MES_IS_UNAFFECTED, target_name);

		(void)set_timed_trait(target_ptr, TRAIT_SLEPT, 0, TRUE);

		if(is_friendly(player_ptr, target_ptr) && !is_pet(player_ptr, target_ptr))
		{
			if(see_m) msg_format(MES_CAMP_GET_ANGRY, target_name);
			set_hostile(user_ptr, target_ptr);
		}
		if(one_in_(13)) set_timed_trait(target_ptr, TRAIT_NO_GENOCIDE, PERMANENT_TIMED, FALSE);
	}

	if(player_cast) take_damage_to_creature(NULL, user_ptr, DAMAGE_GENO, randint1(dam_side), format(COD_GENOCIDE_COST, spell_name), NULL, -1);

	/* Visual feedback */
	move_cursor_relative(user_ptr->fy, user_ptr->fx);
	prepare_redraw(PR_HP);
	prepare_window(PW_PLAYER);

	/* Handle */
	handle_stuff(user_ptr);

	Term_fresh();

	Term_xtra(TERM_XTRA_DELAY, msec);

	return !resist;
}


/*
* Delete all non-unique/non-quest creatures of a given "type" from the level
*/
bool symbol_genocide(creature_type *caster_ptr, POWER power, bool player_cast)
{
	int  i;
	char typ;
	bool result = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	/* Prevent genocide in quest levels */
	if((floor_ptr->quest && !random_quest_number(floor_ptr)) || floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		return FALSE;
	}

	/* Mega-Hack -- Get a creature symbol */
#ifdef JP
	while (!get_com("Ç«ÇÃéÌóﬁ(ï∂éö)ÇÃÉNÉäÅ[É`ÉÉÅ[ÇñïéEÇµÇ‹Ç∑Ç©: ", &typ, FALSE)) ;
#else
	while (!get_com("Choose a creature race (by symbol) to genocide: ", &typ, FALSE)) ;
#endif

	/* Delete the creatures of that "type" */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *species_ptr = &species_info[m_ptr->species_idx];


		if(!m_ptr->species_idx) continue;

		/* Skip "wrong" creatures */
		if(species_ptr->d_char != typ) continue;

		result |= genocide_aux(caster_ptr, i, power, player_cast, 4, COD_GENOCIDE_SYMBOL);
	}

	return result;
}


/*
* Delete all nearby (non-unique) creatures
*/
bool mass_genocide(creature_type *caster_ptr, POWER power, bool player_cast)
{
	int  i;
	bool result = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	/* Prevent mass genocide in quest levels */
	if((floor_ptr->quest && !random_quest_number(floor_ptr)) || floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
		return FALSE;

	/* Delete the (nearby) creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];


		if(!m_ptr->species_idx) continue;

		/* Skip distant creatures */
		if(m_ptr->cdis > MAX_SIGHT) continue;

		result |= genocide_aux(caster_ptr, i, power, player_cast, 3, COD_GENOCIDE_MASS);
	}

	return result;
}



/*
* Delete all nearby (non-unique) undead
*/
bool mass_genocide_undead(creature_type *caster_ptr, POWER power, bool player_cast)
{
	int  i;
	bool result = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);

	/* Prevent mass genocide in quest levels */
	if((floor_ptr->quest && !random_quest_number(floor_ptr)) || floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode)
	{
		return FALSE;
	}

	/* Delete the (nearby) creatures */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		if(!m_ptr->species_idx) continue;
		if(!has_trait(m_ptr, TRAIT_UNDEAD)) continue;

		/* Skip distant creatures */
		if(m_ptr->cdis > MAX_SIGHT) continue;
		result |= genocide_aux(caster_ptr, i, power, player_cast, 3, COD_GENOCIDE_UNDEAD);
	}

	return result;
}



/*
* Probe nearby creatures
*/
bool probing(floor_type *floor_ptr)
{
	int i, speed;
	int cu, cv;
	bool probe = FALSE;
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
		species_type *species_ptr = &species_info[m_ptr->species_idx];


		if(!m_ptr->species_idx) continue;

		/* Require line of sight */
		if(!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;

		/* Probe visible creatures */
		if(m_ptr->see_others)
		{
			char m_name[MAX_NLEN];

			if(!probe) msg_print(MES_PROBING_START);
			msg_print(NULL);
			if(!is_original_ap(m_ptr))
			{
				if(has_trait(m_ptr, TRAIT_KAGE)) set_timed_trait(m_ptr, TRAIT_KAGE, 0, FALSE);

				m_ptr->ap_species_idx = m_ptr->species_idx;
				lite_spot(floor_ptr, m_ptr->fy, m_ptr->fx);
			}
			/* Get "the creature" or "something" */
			creature_desc(m_name, m_ptr, CD_IGNORE_HALLU | CD_INDEF_HIDDEN);

			speed = m_ptr->speed;

			/* Get the creature's alignment */
			/* TODO: New Alignment View */
			align = "----";

			if(m_ptr->sex == SEX_MALE) sex = KW_MALE;
			else if(m_ptr->sex == SEX_FEMALE) sex = KW_FEMALE;
			else if(m_ptr->sex == SEX_INTERSEX) sex = KW_INTERSEX;
			else if(m_ptr->sex == SEX_NONE) sex = KW_NOSEX;

			sprintf(buf, MES_PROBING_FORMAT,
				m_name, align, sex, m_ptr->chp, m_ptr->mhp, m_ptr->ac + m_ptr->to_ac, (speed > 0) ? "+" : "", speed,
				m_ptr->stat_use[0] / STAT_FRACTION, m_ptr->stat_use[1] / STAT_FRACTION,
				m_ptr->stat_use[2] / STAT_FRACTION, m_ptr->stat_use[3] / STAT_FRACTION,
				m_ptr->stat_use[4] / STAT_FRACTION, m_ptr->stat_use[5] / STAT_FRACTION);
			if(species_ptr->next_species_idx) strcat(buf, format("%d/%d ", m_ptr->exp, species_ptr->next_exp));
			else strcat(buf, "xxx ");

#ifdef JP
			if(has_trait(m_ptr, TRAIT_PARALYZED)) strcat(buf,"ñÉ·É ");
			if(has_trait(m_ptr, TRAIT_SLEPT)) strcat(buf,"êáñ∞ ");
			if(has_trait(m_ptr, TRAIT_STUN)) strcat(buf,"ûNûO ");
			if(has_trait(m_ptr, TRAIT_AFRAID)) strcat(buf,"ã∞ï| ");
			if(has_trait(m_ptr, TRAIT_CONFUSED)) strcat(buf,"ç¨óê ");
			if(has_trait(m_ptr, TRAIT_INVULNERABLE)) strcat(buf,"ñ≥ìG ");
#else
			if(has_trait(m_ptr, TRAIT_PARALYZED)) strcat(buf,"paralyzed ");
			if(has_trait(m_ptr, TRAIT_SLEPT)) strcat(buf,"slept ");
			if(has_trait(m_ptr, TRAIT_STUN)) strcat(buf,"stunned ");
			if(has_trait(m_ptr, TRAIT_AFRAID)) strcat(buf,"scared ");
			if(has_trait(m_ptr, TRAIT_CONFUSED)) strcat(buf,"confused ");
			if(has_trait(m_ptr, TRAIT_INVULNERABLE)) strcat(buf,"invulnerable ");
#endif
			buf[strlen(buf)-1] = '\0';
			prt(buf,0,0);

			/* HACK : Add the line to message buffer */
			message_add(buf);
			prepare_window(PW_MESSAGE);
			window_stuff(player_ptr);

			if(m_ptr->see_others) move_cursor_relative(m_ptr->fy, m_ptr->fx);
			inkey();

			Term_erase(0, 0, 255);

			/* Learn everything about this creature */
			if(lore_do_probe(m_ptr->species_idx))
			{
				char buf[80];

				/* Get base name of creature */
				strcpy(buf, (species_name + species_ptr->name));

#ifdef JP
				msg_format("%sÇ…Ç¬Ç¢ÇƒÇ≥ÇÁÇ…è⁄ÇµÇ≠Ç»Ç¡ÇΩãCÇ™Ç∑ÇÈÅB", buf);
#else
				/* Pluralize it */
				plural_aux(buf);
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
bool destroy_area(creature_type *caster_ptr, COODINATES y1, COODINATES x1, COODINATES r, bool in_generate)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	COODINATES y, x;
	int k, t;
	cave_type *c_ptr;
	bool      flag = FALSE;

	/* Prevent destruction of quest levels and town */
	if((floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest)) || !floor_ptr->depth)
		return FALSE;

	/* Lose creature light */
	if(!in_generate) clear_creature_lite(floor_ptr);

	/* Big area of affect */
	for (y = (y1 - r); y <= (y1 + r); y++)
	{
		for (x = (x1 - r); x <= (x1 + r); x++)
		{
			/* Skip illegal grids */
			if(!IN_BOUNDS(floor_ptr, y, x)) continue;

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
				if(CREATURE_BOLD(caster_ptr, y, x))
				{
					flag = TRUE;
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
						char m_name[MAX_NLEN];

						creature_desc(m_name, m_ptr, CD_INDEF_VISIBLE);
						do_cmd_write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_DESTROY, m_name);
					}

					/* Delete the creature (if any) */
					delete_creature(floor_ptr, y, x);
				}
			}

			/* During generation, destroyed artifacts are "preserved" */
			if(in_generate)
			{
				OBJECT_ID this_object_idx, next_object_idx = 0;

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
							msg_format(MES_DEBUG_ARTIFACT_CANCEL(object_name));
						}
					}
					else if(in_generate && cheat_peek && object_ptr->art_name)
						msg_print(MES_DEBUG_RANDOM_ARTIFACT_CANCEL);
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
					if(t < 20) cave_set_feat(floor_ptr, y, x, feat_granite);
					else if(t < 70) cave_set_feat(floor_ptr, y, x, feat_quartz_vein);
					else if(t < 100) cave_set_feat(floor_ptr, y, x, feat_magma_vein);
					else cave_set_feat(floor_ptr, y, x, feat_floor_rand_table[randint0(100)]);
				}
				else /* In generation */
				{
					if(t < 20)
					{
						place_extra_grid(c_ptr);
					}
					else if(t < 70) c_ptr->feat = feat_quartz_vein;
					else if(t < 100) c_ptr->feat = feat_magma_vein;
					else place_floor_grid(c_ptr);

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
				if(!IN_BOUNDS(floor_ptr, y, x)) continue;

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
						if(!IN_BOUNDS2(floor_ptr, yy, xx)) continue;
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
			msg_print(MES_DESTROY);

			if(!has_trait(caster_ptr, TRAIT_NO_BLIND) && !has_trait(caster_ptr, TRAIT_RES_LITE))
				(void)add_timed_trait(caster_ptr, TRAIT_BLIND, 10 + randint1(10), TRUE);
		}

		forget_flow(floor_ptr);

		// Mega-Hack -- Forget the view and lite
		prepare_update(caster_ptr, PU_UN_VIEW | PU_UN_LITE | PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE | PU_CREATURES);
		prepare_redraw(PR_MAP);
		prepare_window(PW_OVERHEAD | PW_DUNGEON);

		if(caster_ptr->posture & NINJA_S_STEALTH)
		{
			if(floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].info & CAVE_GLOW) set_superstealth(caster_ptr, FALSE);
		}
	}

	return TRUE;
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
bool earthquake_aux(creature_type *caster_ptr, COODINATES cy, COODINATES cx, COODINATES r, CREATURE_ID m_idx)
{
	COODINATES i, t, y, x, yy, xx, dy, dx;
	COODINATES sy = 0, sx = 0;
	POWER damage = 0;
	int             sn = 0;
	cave_type       *c_ptr;
	bool            map[32][32];
	floor_type      *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	char caster_name[MAX_NLEN];

	creature_desc(caster_name, caster_ptr, 0);

	if(r > EARTHQUAKE_MAX_RANGE) r = EARTHQUAKE_MAX_RANGE;	// Paranoia -- Enforce maximum range
	for (y = 0; y < 32; y++) for (x = 0; x < 32; x++) map[y][x] = FALSE; // Clear the "maximal blast" area

	// Check around the epicenter
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			// Extract the location
			yy = cy + dy;
			xx = cx + dx;

			if(!IN_BOUNDS(floor_ptr, yy, xx)) continue; // Skip illegal grids
			if(distance(cy, cx, yy, xx) > r) continue;	// Skip distant grids

			c_ptr = &floor_ptr->cave[yy][xx];	// Access the grid
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY | CAVE_UNSAFE | CAVE_GLOW | CAVE_MARK); // Lose room and vault
			if(!dx && !dy) continue; // Skip the epicenter
			if(PERCENT(85)) continue; // Skip most grids
			map[16+yy-cy][16+xx-cx] = TRUE;	// Damage this grid
		}
	}

	// Examine the quaked region
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			// Extract the location
			yy = cy + dy;
			xx = cx + dx;

			if(!map[16+yy-cy][16+xx-cx]) continue; // Skip unaffected grids
			c_ptr = &floor_ptr->cave[yy][xx]; // Access the grid

			if(c_ptr->creature_idx == caster_ptr->riding) continue;

			if(c_ptr->creature_idx) // Process creatures
			{
				creature_type *target_ptr = &creature_list[c_ptr->creature_idx];
				if(has_trait(target_ptr, TRAIT_QUESTOR)) // Quest creatures / No wall on quest creatures
				{
					map[16+yy-cy][16+xx-cx] = FALSE;
					continue;
				}

				// First, affect the player (if necessary)
				if(!has_trait(target_ptr, TRAIT_PASS_WALL) && !has_trait(target_ptr, TRAIT_KILL_WALL))
				{
					for (i = 0; i < 8; i++)	// Check around the player
					{
						// Access the location
						y = target_ptr->fy + ddy_ddd[i];
						x = target_ptr->fx + ddx_ddd[i];

						if(!cave_empty_bold(floor_ptr, y, x)) continue; // Skip non-empty grids
						if(map[16+y-cy][16+x-cx]) continue; // Important -- Skip "quake" grids
						if(floor_ptr->cave[y][x].creature_idx) continue;

						sn++; // Count "safe" grids
						if(randint0(sn) > 0) continue; // Randomize choice
						sy = y; sx = x;	// Save the safe location
					}

					switch (randint1(3))
					{
						case 1: msg_print(MES_EARTHQUAKE_DONE1); break;
						case 2: msg_print(MES_EARTHQUAKE_DONE2); break;
						default: msg_print(MES_EARTHQUAKE_DONE3); break;
					}

					// Hurt the player a lot / Message and damage
					if(!sn)
					{
						if(is_player(target_ptr)) msg_print(MES_EARTHQUAKE_HEAVY_CRUSHED);
						damage = 200;
					}
					else // Destroy the grid, and push the player to safety
					{
						switch (randint1(3)) // Calculate results
						{
						case 1:
							if(is_player(target_ptr)) msg_print(MES_EARTHQUAKE_EVADE);
							damage = 0;
							break;
						case 2:
							if(is_player(target_ptr)) msg_print(MES_EARTHQUAKE_CRUSHED1);
							damage = diceroll(10, 4);
							(void)add_timed_trait(target_ptr, TRAIT_STUN, randint1(50), TRUE);
							break;
						case 3:
							if(is_player(target_ptr)) msg_print(MES_EARTHQUAKE_CRUSHED2);
							damage = diceroll(10, 4);
							(void)add_timed_trait(target_ptr, TRAIT_STUN, randint1(50), TRUE);
							break;
						}
						(void)move_creature(target_ptr, NULL, sy, sx, MCE_DONT_PICKUP); // Move the player to the safe location
					}

					map[16 + target_ptr->fy - cy][16 + target_ptr->fx - cx] = FALSE; // Important -- no wall on player

					
					if(damage) // Take some damage
					{
						char *killer;

						if(m_idx)
						{
							char target_name[MAX_NLEN];
							creature_type *target_ptr = &creature_list[m_idx];
							creature_desc(target_name, target_ptr, CD_IGNORE_HALLU | CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE); // Get the creature's real name
							killer = format(COD_EARTHQUAKE_CASTER(caster_name));
						}
						else killer = COD_EARTHQUAKE;
						take_damage_to_creature(NULL, caster_ptr, DAMAGE_ATTACK, damage, killer, NULL, -1);
					}
				}
			}
		}
	}

	clear_creature_lite(floor_ptr); // Lose creature light
	for (dy = -r; dy <= r; dy++) // Examine the quaked region
	{
		for (dx = -r; dx <= r; dx++)
		{
			// Extract the location
			yy = cy + dy;
			xx = cx + dx;
			
			if(!map[16+yy-cy][16+xx-cx]) continue; // Skip unaffected grids
			c_ptr = &floor_ptr->cave[yy][xx]; // Access the cave grid

			if(CREATURE_BOLD(caster_ptr, yy, xx)) continue; // Paranoia -- never affect player
			if(cave_valid_bold(floor_ptr, yy, xx)) // Destroy location (if valid)
			{
				delete_object(floor_ptr, yy, xx); // Delete objects
				t = CAVE_HAVE_FLAG_BOLD(floor_ptr, yy, xx, FF_PROJECT) ? randint0(100) : 200; // Wall (or floor) type

				if(t < 20) cave_set_feat(floor_ptr, yy, xx, feat_granite); // Granite
				else if(t < 70) cave_set_feat(floor_ptr, yy, xx, feat_quartz_vein); // Quartz
				else if(t < 100) cave_set_feat(floor_ptr, yy, xx, feat_magma_vein); // Magma
				else cave_set_feat(floor_ptr, yy, xx, feat_floor_rand_table[randint0(100)]); // Floor
			}
		}
	}

	for (dy = -r; dy <= r; dy++) // Process "re-glowing"
	{
		for (dx = -r; dx <= r; dx++)
		{
			// Extract the location
			yy = cy + dy;
			xx = cx + dx;

			if(!IN_BOUNDS(floor_ptr, yy, xx)) continue;	// Skip illegal grids
			if(distance(cy, cx, yy, xx) > r) continue;	// Skip distant grids

			c_ptr = &floor_ptr->cave[yy][xx];	// Access the grid

			if(is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
			else if(!(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS))
			{
				int ii, yyy, xxx;
				cave_type *cc_ptr;

				for (ii = 0; ii < DIRECTION_NUM; ii++)
				{
					yyy = yy + ddy_ddd[ii];
					xxx = xx + ddx_ddd[ii];
					if(!IN_BOUNDS2(floor_ptr, yyy, xxx)) continue;
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

	prepare_update(player_ptr, PU_UN_VIEW | PU_UN_LITE | PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE | PU_CREATURES); // Update stuff
	prepare_redraw(PR_HEALTH | PR_UHEALTH | PR_MAP | PW_OVERHEAD | PW_DUNGEON); // Update the health bar

	if(caster_ptr->posture & NINJA_S_STEALTH && floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].info & CAVE_GLOW)
		set_superstealth(caster_ptr, FALSE);

	return TRUE; // Success
}

bool earthquake(creature_type *target_ptr, COODINATES cy, COODINATES cx, COODINATES r)
{
	msg_print(MES_EARTHQUAKE);
	return earthquake_aux(target_ptr, cy, cx, r, 0);
}

void discharge_minion(creature_type *caster_ptr)
{
	int i;
	bool okay = TRUE;

	for (i = 1; i < creature_max; i++)
	{
		creature_type *target_ptr = &creature_list[i];
		if(!target_ptr->species_idx || !is_pet(player_ptr, target_ptr)) continue;
		if(target_ptr->nickname) okay = FALSE;
	}
	if(!okay || caster_ptr->riding)
		if(!get_check(MES_DIS_MINION_ASK)) return;

	for (i = 1; i < creature_max; i++)
	{
		POWER dam;
		creature_type *target_ptr = &creature_list[i];

		if(!target_ptr->species_idx || !is_pet(player_ptr, target_ptr)) continue;

		/* Uniques resist discharging */
		if(has_trait(target_ptr, TRAIT_UNIQUE))
		{
			char target_name[MAX_NLEN];
			creature_desc(target_name, target_ptr, 0x00);
			msg_format(MES_DIS_MINION_CANCEL(target_name));
			delete_species_idx(&creature_list[i]);
			continue;
		}
		dam = target_ptr->mhp / 2;
		if(dam > 100) dam = (dam - 100) / 2 + 100;
		if(dam > 400) dam = (dam - 400) / 2 + 400;
		if(dam > 800) dam = 800;
		project(target_ptr, 0, 2+(target_ptr->lev/10), target_ptr->fy,
			target_ptr->fx, dam, DO_EFFECT_PLASMA, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);

		if(record_named_pet && target_ptr->nickname)
		{
			char target_name[MAX_NLEN];

			creature_desc(target_name, target_ptr, CD_INDEF_VISIBLE);
			do_cmd_write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_BLAST, target_name);
		}

		delete_species_idx(&creature_list[i]);
	}
}


/*
* This routine clears the entire "temp" set.
* This routine will Perma-Lite all "temp" grids.
* This routine is used (only) by "lite_room()"
* Dark grids are illuminated.
* Also, process all affected creatures.
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
		c_ptr->info &= ~(CAVE_TEMP); // No longer in the array
		if(c_ptr->info & (CAVE_GLOW)) continue; // Update only non-CAVE_GLOW grids

		c_ptr->info |= (CAVE_GLOW); // Perma-Lite

		if(c_ptr->creature_idx) // Process affected creatures
		{
			int chance = 25;
			creature_type *m_ptr = &creature_list[c_ptr->creature_idx];
			update_creature_view(player_ptr, c_ptr->creature_idx, FALSE); // Update the creature

			if(has_trait(m_ptr, TRAIT_STUPID)) chance = 10; // Stupid creatures rarely wake up
			if(has_trait(m_ptr, TRAIT_SMART)) chance = 100; // Smart creatures always wake up

			if(has_trait_from_timed(m_ptr, TRAIT_SLEPT) && (PERCENT(chance))) // Sometimes creatures wake up
				(void)set_timed_trait(m_ptr, TRAIT_SLEPT, 0, TRUE); // Wake up!
		}

		note_spot(floor_ptr, y, x);
		lite_spot(floor_ptr, y, x);
		update_local_illumination(floor_ptr, y, x);
	}

	temp_n = 0; // None left
}



/*
 * This routine clears the entire "temp" set.
 * This routine will "darken" all "temp" grids.
 * In addition, some of these grids will be "unmarked".
 * This routine is used (only) by "unlite_room()"
 * Also, process all affected creatures
 */
static void cave_temp_room_unlite(floor_type *floor_ptr)
{
	int i;
	for (i = 0; i < temp_n; i++) // Clear them all
	{
		int y = temp_y[i];
		int x = temp_x[i];
		int j;

		cave_type *c_ptr = &floor_ptr->cave[y][x];
		bool do_dark = !is_mirror_grid(c_ptr);
		c_ptr->info &= ~(CAVE_TEMP); // No longer in the array

		if(do_dark) // Darken the grid
		{
			if(floor_ptr->depth || !is_daytime())
			{
				for (j = 0; j < DIRECTION_NUM; j++)
				{
					int by = y + ddy_ddd[j];
					int bx = x + ddx_ddd[j];

					if(IN_BOUNDS2(floor_ptr, by, bx))
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

			if(!have_flag(feature_info[get_feat_mimic(c_ptr)].flags, FF_REMEMBER)) // Hack -- Forget "boring" grids
			{
				if(!view_torch_grids) c_ptr->info &= ~(CAVE_MARK); // Forget the grid
				note_spot(floor_ptr, y, x); // Notice
			}

			if(c_ptr->creature_idx) update_creature_view(player_ptr, c_ptr->creature_idx, FALSE); // Process affected creatures / Update the creature
			lite_spot(floor_ptr, y, x);

			update_local_illumination(floor_ptr, y, x);
		}
	}

	temp_n = 0; // None left
}

// Determine how much contiguous open space this grid is next to
static int next_to_open(floor_type *floor_ptr, COODINATES cy, COODINATES cx, bool (*pass_bold)(floor_type *, int, int))
{
	int i;
	COODINATES y, x;
	int len = 0;
	int blen = 0;

	for (i = 0; i < 16; i++)
	{
		y = cy + ddy_cdd[i % 8];
		x = cx + ddx_cdd[i % 8];

		if(!pass_bold(floor_ptr, y, x)) // Found a wall, break the length
		{
			if(len > blen) blen = len; // Track best length
			len = 0;
		}
		else len++;
	}
	return (MAX(len, blen));
}


static int next_to_walls_adj(floor_type *floor_ptr, int cy, int cx, bool (*pass_bold)(floor_type *, int, int))
{
	int i;
	COODINATES y, x;
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
static void cave_temp_room_aux(creature_type *caster_ptr, COODINATES y, COODINATES x, bool only_room, bool (*pass_bold)(int, int))
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
		if(!IN_BOUNDS2(floor_ptr, y, x)) return;

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
		if(IN_BOUNDS(floor_ptr, y, x) && pass_bold(y, x)) return;
		//    (next_to_walls_adj(floor_ptr, y, x, pass_bold) == 6) && (next_to_open(floor_ptr, y, x, pass_bold) <= 1)) return;
	}


	if(temp_n == TEMP_MAX) return;

	/* Mark the grid as "seen" */
	c_ptr->info |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;
}

// Aux function -- see below
static void cave_temp_room_aux2(creature_type *caster_ptr, COODINATES y, COODINATES x, bool only_room, bool (*pass_bold)(floor_type *, int, int))
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
		if(!IN_BOUNDS2(floor_ptr, y, x)) return;

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
		if(IN_BOUNDS(floor_ptr, y, x) && pass_bold(floor_ptr, y, x) &&
			(next_to_walls_adj(floor_ptr, y, x, pass_bold) == 6) && (next_to_open(floor_ptr, y, x, pass_bold) <= 1)) return;
	}


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
	return CAVE_LOS_BOLD(floor_ptr, y, x);
}

/*
* Aux function -- see below
*/
static void cave_temp_lite_room_aux(creature_type *caster_ptr, COODINATES y, COODINATES x)
{
	cave_temp_room_aux2(caster_ptr, y, x, FALSE, cave_pass_lite_bold);
}

/*
* Aux function -- see below
*/
static bool cave_pass_dark_bold(floor_type *floor_ptr, COODINATES y, COODINATES x)
{
	return CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT);
}

/*
* Aux function -- see below
*/
static void cave_temp_unlite_room_aux(creature_type *caster_ptr, COODINATES y, COODINATES x)
{
	cave_temp_room_aux2(caster_ptr, y, x, TRUE, cave_pass_dark_bold);
}




/*
 * Illuminate any room containing the given location.
 */
void lite_room(creature_type *creature_ptr, COODINATES y1, COODINATES x1)
{
	int i;
	COODINATES x, y;
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
		if(floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_GLOW) set_superstealth(creature_ptr, FALSE);
}


/*
 * Darken all rooms containing the given location
 */
void unlite_room(creature_type *caster_ptr, COODINATES y1, COODINATES x1)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int i;
	COODINATES x, y;

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
bool lite_area(creature_type *creature_ptr, POWER dam, COODINATES rad)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS)
	{
		msg_print(MES_PREVENT_LITE_BY_DUNGEON);
		return FALSE;
	}

	if(!has_trait(player_ptr, TRAIT_BLIND)) msg_print(MES_LITE_AREA);
	(void)project(creature_ptr, 0, rad, creature_ptr->fy, creature_ptr->fx, dam, DO_EFFECT_LITE_WEAK, PROJECT_GRID | PROJECT_KILL, -1);
	lite_room(creature_ptr, creature_ptr->fy, creature_ptr->fx);
	return TRUE;
}


/*
* Hack -- call darkness around the player
* Affect all creatures in the projection radius
*/
bool unlite_area(creature_type *caster_ptr, POWER dam, COODINATES rad)
{
	if(!has_trait(player_ptr, TRAIT_BLIND)) msg_print(MES_UNLITE_AREA);
	(void)project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, dam, DO_EFFECT_DARK_WEAK, PROJECT_GRID | PROJECT_KILL, -1);
	unlite_room(caster_ptr, caster_ptr->fy, caster_ptr->fx);
	return TRUE;
}

bool fire_meteor(int who, int typ, COODINATES y, COODINATES x, POWER dam, COODINATES rad)
{
	/* Analyze the "target" and the caster. */
	return (project(&creature_list[who], 0, rad, y, x, dam, typ, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1));
}


bool fire_blast(creature_type *caster_ptr, int typ, int dir, int dd, int ds, int num, int dev)
{
	COODINATES ly, lx, ty, tx, y, x, ld;
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
		while(TRUE)
		{
			/* Get targets for some bolts */
			y = (COODINATES)rand_spread(ly, ld * dev / 20);
			x = (COODINATES)rand_spread(lx, ld * dev / 20);

			if(distance(ly, lx, y, x) <= ld * dev / 20) break;
		}

		/* Analyze the "dir" and the "target". */
		if(!project(0, 0, 0, y, x, diceroll(dd, ds), typ, PROJECT_FAST | PROJECT_THRU | PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE | PROJECT_GRID, -1))
		{
			result = FALSE;
		}
	}
	return (result);
}

// Switch position with a creature.
bool teleport_swap(creature_type *creature_ptr, int dir)
{
	COODINATES tx, ty;
	cave_type * c_ptr;
	creature_type * target_ptr;
	species_type * species_ptr;
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
		msg_print(MES_TELEPORT_PREVENTED);
		return FALSE;
	}

	if(!c_ptr->creature_idx || (c_ptr->creature_idx == creature_ptr->riding))
	{
#ifdef JP
		msg_print("ÇªÇÍÇ∆ÇÕèÍèäÇåä∑Ç≈Ç´Ç‹ÇπÇÒÅB");
#else
		msg_print("You can't trade places with that!");
#endif
		return FALSE;
	}

	if((c_ptr->info & CAVE_ICKY) || (distance(ty, tx, creature_ptr->fy, creature_ptr->fx) > creature_ptr->lev * 3 / 2 + 10))
	{
		msg_print(MES_FAILED);
		return FALSE;
	}

	target_ptr = &creature_list[c_ptr->creature_idx];
	species_ptr = &species_info[target_ptr->species_idx];

	(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, 0, TRUE);

	if(has_trait(target_ptr, TRAIT_RES_TELE))
	{
		msg_print(MES_PREVENT_TELEPORT_BY_CREATURE);
		if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
		return FALSE;
	}

	sound(SOUND_TELEPORT);

	// Swap the player and creature
	(void)move_creature(creature_ptr, NULL, ty, tx, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP);
	return TRUE;
}

bool wall_stone(creature_type *caster_ptr)
{
	bool dummy = (project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_STONE_WALL, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1));
	prepare_update(caster_ptr, PU_FLOW);
	prepare_redraw(PR_MAP);
	return dummy;
}

void call_chaos(creature_type *creature_ptr)
{
	EFFECT_ID Chaos_type;
	int dummy;
	int plev = creature_ptr->lev;
	bool line_chaos = FALSE;

	int hurt_types[31] =
	{
		DO_EFFECT_ELEC, DO_EFFECT_POIS, DO_EFFECT_ACID, DO_EFFECT_COLD,
		DO_EFFECT_FIRE, DO_EFFECT_MISSILE, DO_EFFECT_ARROW, DO_EFFECT_PLASMA,
		DO_EFFECT_HOLY_FIRE, DO_EFFECT_WATER, DO_EFFECT_LITE, DO_EFFECT_DARK,
		DO_EFFECT_FORCE, DO_EFFECT_INERTIA, DO_EFFECT_MANA, DO_EFFECT_METEOR,
		DO_EFFECT_ICE, DO_EFFECT_CHAOS, DO_EFFECT_NETHER, DO_EFFECT_DISENCHANT,
		DO_EFFECT_SHARDS, DO_EFFECT_SOUND, DO_EFFECT_NEXUS, DO_EFFECT_CONFUSION,
		DO_EFFECT_TIME, DO_EFFECT_GRAVITY, DO_EFFECT_ROCKET, DO_EFFECT_NUKE,
		DO_EFFECT_HELL_FIRE, DO_EFFECT_DISINTEGRATE, DO_EFFECT_PSY_SPEAR
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
					cast_beam(creature_ptr, Chaos_type, MAX_RANGE_SUB, 150, 0);
				else
					cast_ball(creature_ptr, Chaos_type, dummy, 150, 2);
			}
		}
	}
	else if(one_in_(3)) SELF_FIELD(creature_ptr, Chaos_type, 500, 8, -1);
	else
	{
		if(line_chaos)
			cast_beam(creature_ptr, Chaos_type, MAX_RANGE_SUB, 250, 0);
		else
			cast_ball(creature_ptr, Chaos_type, MAX_RANGE_SUB, 250, 3 + (plev / 35));
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
				earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 5 + randint0(10));
				if(!one_in_(6)) break;
			}

		case 30: case 31:
			if(!(*count))
			{
				POWER dam = diceroll(10, 10);
				msg_print(MES_TY_CURSE_PURE_MANA);
				project(0, 0, 8, creature_ptr->fy, creature_ptr->fx, dam, DO_EFFECT_MANA, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP, -1);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, COD_PURE_MANA, NULL, -1);
				if(!one_in_(6)) break;
			}

		case 32: case 33:
			if(!(*count))
			{
				msg_print(MES_TY_CURSE_TELEPORT);
				teleport_creature(creature_ptr, diceroll(10, 10), TELEPORT_PASSIVE);
				if(randint0(13)) (*count) += activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
				if(!one_in_(6)) break;
			}

		case 34:
			msg_print(MES_TY_CURSE_SURGE_ENERGY);
			wall_breaker(creature_ptr);
			if(!randint0(7))
			{
				project(0, 0, 7, creature_ptr->fy, creature_ptr->fx, 50, DO_EFFECT_KILL_WALL, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP, -1);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, 50, COD_TY_CURSE_ENERGY, NULL, -1);
			}
			if(!one_in_(6)) break;

		case 1: case 2: case 3: case 16: case 17:
			aggravate_creatures(creature_ptr);
			if(!one_in_(6)) break;

		case 4: case 5: case 6:
			(*count) += activate_hi_summon(creature_ptr, creature_ptr->fy, creature_ptr->fx, FALSE);
			if(!one_in_(6)) break;

		case 7: case 8: case 9: case 18:
			(*count) += summon_specific(0, creature_ptr->fy, creature_ptr->fx, floor_ptr->depth, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));
			if(!one_in_(6)) break;

		case 10: case 11: case 12:
			msg_print(MES_LOST_LIFE2);
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
				msg_print(MES_TY_CURSE_STATUE);
				if(has_trait(creature_ptr, TRAIT_FREE_ACTION))
					add_timed_trait(creature_ptr, TRAIT_PARALYZED, randint1(3), TRUE);
				else
					add_timed_trait(creature_ptr, TRAIT_PARALYZED, randint1(13), TRUE);
				stop_ty = TRUE;
			}
			if(!one_in_(6)) break;

		case 21: case 22: case 23:

			(void)do_dec_stat(creature_ptr, randint0(6));
			if(!one_in_(6)) break;

		case 24:
			msg_print(MES_TY_CURSE_AMNESIA);
			lose_all_info(creature_ptr);
			if(!one_in_(6)) break;

		case 25:
			/*
			* Only summon Cyberdemons deep in the dungeon.
			*/
			if((floor_ptr->depth > 65) && !stop_ty)
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


int activate_hi_summon(creature_type *creature_ptr, COODINATES y, COODINATES x, bool can_pet)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;
	int count = 0;
	int summon_lev;
	FLAGS_32 mode = PC_ALLOW_GROUP;
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

	summon_lev = (pet ? creature_ptr->lev * 2 / 3 + randint1(creature_ptr->lev / 2) : floor_ptr->depth);

	for (i = 0; i < (randint1(7) + (floor_ptr->depth / 40)); i++)
	{
		switch (randint1(25) + (floor_ptr->depth / 20))
		{
		case 1: case 2:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_ANT, mode);
			break;
		case 3: case 4:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_SPIDER, mode);
			break;
		case 5: case 6:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_HOUND, mode);
			break;
		case 7: case 8:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_HYDRA, mode);
			break;
		case 9: case 10:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_ANGEL, mode);
			break;
		case 11: case 12:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_UNDEAD, mode);
			break;
		case 13: case 14:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_DRAGON, mode);
			break;
		case 15: case 16:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_DEMON, mode);
			break;
		case 17:
			if(can_pet) break;
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_AMBERITES, (mode | PC_ALLOW_UNIQUE));
			break;
		case 18: case 19:
			if(can_pet) break;
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_UNIQUE, (mode | PC_ALLOW_UNIQUE));
			break;
		case 20: case 21:
			if(!can_pet) mode |= PC_ALLOW_UNIQUE;
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_HI_UNDEAD, mode);
			break;
		case 22: case 23:
			if(!can_pet) mode |= PC_ALLOW_UNIQUE;
			count += summon_specific((pet ? creature_ptr : NULL), y, x, summon_lev, TRAIT_S_HI_DRAGON, mode);
			break;
		case 24:
			count += summon_specific((pet ? creature_ptr : NULL), y, x, 100, TRAIT_S_CYBER, mode);
			break;
		default:
			if(!can_pet) mode |= PC_ALLOW_UNIQUE;
			count += summon_specific((pet ? creature_ptr : NULL), y, x,pet ? summon_lev : (((summon_lev * 3) / 2) + 5), 0, mode);
		}
	}

	return count;
}

int summon_cyber(creature_type *summoner_ptr, COODINATES y, COODINATES x)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(summoner_ptr);
	int i;
	int max_cyber = (floor_ptr->depth / 50) + randint1(2);
	int count = 0;
	FLAGS_32 mode = PC_ALLOW_GROUP;

	// Summoned by a creature
	if(summoner_ptr) if(is_pet(player_ptr, summoner_ptr)) mode |= PC_FORCE_PET;

	if(max_cyber > 4) max_cyber = 4;

	for (i = 0; i < max_cyber; i++)
	{
		count += summon_specific(summoner_ptr, y, x, 100, TRAIT_S_CYBER, mode);
	}

	return count;
}


void wall_breaker(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;
	COODINATES y, x;
	int attempts = 1000;

	if(randint1(80 + creature_ptr->lev) < 70)
	{
		while (attempts--)
		{
			scatter(floor_ptr, &y, &x, creature_ptr->fy, creature_ptr->fx, 4, 0);
			if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) continue;
			if(!CREATURE_BOLD(creature_ptr, y, x)) break;
		}
		project(creature_ptr, 0, 0, creature_ptr->fy, creature_ptr->fx, 20 + randint1(30), DO_EFFECT_KILL_WALL, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
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
			while(TRUE)
			{
				scatter(floor_ptr, &y, &x, creature_ptr->fy, creature_ptr->fx, 10, 0);
				if(!CREATURE_BOLD(creature_ptr, y, x)) break;
			}
			project(0, 0, 0, y, x, 20 + randint1(30), DO_EFFECT_KILL_WALL, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
		}
	}
}

bool kawarimi(creature_type *user_ptr, bool success)
{
	object_type forge;
	object_type *quest_ptr = &forge;
	COODINATES y, x;
	char user_name[80];
	floor_type *floor_ptr = GET_FLOOR_PTR(user_ptr);

	creature_desc(user_name, user_ptr, 0);

	if(!(user_ptr->posture & NINJA_KAWARIMI) || !(randint0(55) < (user_ptr->lev * 3 / 5 + 20))) return FALSE;

	if(gameover) return FALSE;
	if(user_ptr->timed_trait[TRAIT_CONFUSED] || has_trait(user_ptr, TRAIT_BLIND) || user_ptr->timed_trait[TRAIT_PARALYZED] || user_ptr->timed_trait[TRAIT_HALLUCINATION]) return FALSE;
	if(randint0(200) < user_ptr->timed_trait[TRAIT_STUN]) return FALSE;

	if(!success && one_in_(3))
	{
#ifdef JP
		msg_format("é∏îsÅI%sÇÕïœÇÌÇËêgÇ≈ì¶Ç∞ÇÁÇÍÇ»Ç©Ç¡ÇΩÅB", user_name);
#else
		msg_format("Failed! %s couldn't run away.", user_name);
#endif
		user_ptr->posture &= ~(NINJA_KAWARIMI);
		prepare_redraw(PR_STATUS);
		return FALSE;
	}

	y = user_ptr->fy;
	x = user_ptr->fx;

	teleport_creature(user_ptr, 10 + randint1(90), 0L);
	object_wipe(quest_ptr);
	object_prep(quest_ptr, lookup_kind(TV_STATUE, SV_WOODEN_STATUE), ITEM_FREE_SIZE);
	quest_ptr->pval = SPECIES_NINJA;

	/* Drop it in the dungeon */
	(void)drop_near(floor_ptr, quest_ptr, -1, y, x);

#ifdef JP
	if(success) msg_format("%sÇÕçUåÇÇéÛÇØÇÈëOÇ…ëfëÅÇ≠êgÇÇ–ÇÈÇ™Ç¶ÇµÇΩÅB", user_name);
	else msg_format("%sÇÕïœÇÌÇËêgÇ…é∏îsÇµÇƒçUåÇÇéÛÇØÇΩÅB", user_name);
#else
	if(success) msg_format("%s have turned around just before the attack hit you.", user_name); // TODO
	else msg_format("Failed! %s are hit by the attack.", user_name); // TODO
#endif

	user_ptr->posture &= ~(NINJA_KAWARIMI);
	prepare_redraw(PR_STATUS);

	return TRUE;
}

// "Rush Attack" routine for Samurai or Ninja
// Return value is for checking "done"
bool rush_attack(creature_type *creature_ptr, bool *mdeath)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int dir;
	COODINATES tx, ty;
	int tm_idx = 0;
	u16b path_g[32];
	int path_n, i;
	bool tmp_mdeath = FALSE;
	bool moved = FALSE;
	COODINATES range = 5;
	char m_name[MAX_NLEN];

	if(mdeath) *mdeath = FALSE;

	if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

	/* Use the given direction */
	tx = creature_ptr->fx + range * ddx[dir];
	ty = creature_ptr->fy + range * ddy[dir];

	/* Hack -- Use an actual "target" */
	if((dir == 5) && target_okay(creature_ptr))
	{
		tx = target_col;
		ty = target_row;
	}

	if(IN_BOUNDS(floor_ptr, ty, tx)) tm_idx = floor_ptr->cave[ty][tx].creature_idx;
	path_n = project_path(path_g, range, floor_ptr, creature_ptr->fy, creature_ptr->fx, ty, tx, PROJECT_STOP | PROJECT_KILL);

	/* No need to move */
	if(!path_n) return TRUE;

	/* Use ty and tx as to-move point */
	ty = creature_ptr->fy;
	tx = creature_ptr->fx;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		creature_type *m_ptr;

		COODINATES ny = GRID_Y(path_g[i]);
		COODINATES nx = GRID_X(path_g[i]);

		if(cave_empty_bold(floor_ptr, ny, nx) && creature_can_cross_terrain(creature_ptr, floor_ptr->cave[ny][nx].feat, 0))
		{
			ty = ny;
			tx = nx;
			continue;
		}

		if(!floor_ptr->cave[ny][nx].creature_idx)
		{
			if(tm_idx) msg_print(MES_FAILED);
			else msg_print(MES_RUSH_NO_ENTER);
			break;
		}

		/* Move player before updating the creature */
		if(!CREATURE_BOLD(creature_ptr, ty, tx)) teleport_creature_to(creature_ptr, ty, tx, TELEPORT_NONMAGICAL);

		/* Update the creature */
		update_creature_view(player_ptr, floor_ptr->cave[ny][nx].creature_idx, TRUE);

		/* Found a creature */
		m_ptr = &creature_list[floor_ptr->cave[ny][nx].creature_idx];

		if(tm_idx != floor_ptr->cave[ny][nx].creature_idx)
		{
			msg_print(MES_CREATURE_IN_THE_WAY);
		}
		else if(!CREATURE_BOLD(creature_ptr, ty, tx))
		{
			creature_desc(m_name, m_ptr, 0);
			msg_format(MES_RUSH_DONE(m_name));
		}

		if(!CREATURE_BOLD(creature_ptr, ty, tx)) teleport_creature_to(creature_ptr, ty, tx, TELEPORT_NONMAGICAL);
		moved = TRUE;
		tmp_mdeath = close_combat(creature_ptr, ny, nx, HISSATSU_NYUSIN);

		break;
	}

	if(!moved && !CREATURE_BOLD(creature_ptr, ty, tx)) teleport_creature_to(creature_ptr, ty, tx, TELEPORT_NONMAGICAL);

	if(mdeath) *mdeath = tmp_mdeath;
	return TRUE;
}



// Remove all mirrors in this floor
void remove_all_mirrors(creature_type *user_ptr, floor_type *floor_ptr, bool explode)
{
	COODINATES x, y;

	for (x = 0; x < floor_ptr->width; x++)
	{
		for (y = 0; y < floor_ptr->height; y++)
		{
			if(is_mirror_grid(&floor_ptr->cave[y][x]))
			{
				remove_mirror(user_ptr, y, x);
				if(explode)
					project(0, 0, 2, y, x, user_ptr->lev / 2 + 5, DO_EFFECT_SHARDS,
					(PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}
		}
	}
}
