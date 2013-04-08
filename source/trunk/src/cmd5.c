/* File: cmd5.c */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

/* Purpose: Spell/Prayer commands */

#include "angband.h"

cptr spell_category_name(TVAL tval)
{
	switch (tval)
	{
	case TV_HISSATSU_BOOK: return KW_HISSATSU;
	case TV_LIFE_BOOK: return KW_MIRACLE;
	case TV_MUSIC_BOOK: return KW_SONG;
	default: return KW_SPELL;
	}
}

/*
* Allow user to choose a spell/prayer from the given book.
*
* If a valid spell is chosen, saves it in '*sn' and returns TRUE
* If the user hits escape, returns FALSE, and set '*sn' to -1
* If there are no legal choices, returns FALSE, and sets '*sn' to -2
*
* The "prompt" should be "cast", "recite", or "study"
* The "known" should be TRUE for cast/pray, FALSE for study
*/
static int get_spell(creature_type *creature_ptr, KEY *sn, cptr prompt, SVAL sval, bool learned, int use_realm)
{
	int         i;
	KEY spell = -1;
	int         num = 0;
	int         ask = TRUE;
	int         need_mana;
	KEY spells[64];
	bool        flag, redraw, okay;
	char        choice;
	magic_type  *s_ptr;
	char        out_val[160];
	cptr        p;
#ifdef JP
	char jverb_buf[128];
#endif
	int menu_line = (use_menu ? 1 : 0);

	/* Get the spell, if available */
	if(repeat_pull(sn))
	{
		/* Verify the spell */
		if(spell_okay(creature_ptr, *sn, learned, FALSE, use_realm))
		{
			return TRUE;
		}
	}

	p = spell_category_name(magic_info[creature_ptr->class_idx].spell_book);

	/* Extract spells */
	for (spell = 0; spell < REALM_MAGIC_NUMBER; spell++)
	{
		/* Check for this spell */
		if((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}

	/* Assume no usable spells */
	okay = FALSE;

	/* Assume no spells available */
	(*sn) = -2;

	/* Check for "okay" spells */
	for (i = 0; i < num; i++)
	{
		/* Look for "okay" spells */
		if(spell_okay(creature_ptr, spells[i], learned, FALSE, use_realm)) okay = TRUE;
	}

	/* No "okay" spells */
	if(!okay) return FALSE;
	if(((use_realm) != creature_ptr->realm1) && ((use_realm) != creature_ptr->realm2) && (creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_RED_MAGE)) return FALSE;
	if(((creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE)) && !is_magic(use_realm)) return FALSE;
	if((creature_ptr->class_idx == CLASS_RED_MAGE) && ((use_realm) != REALM_ARCANE) && (sval > 1)) return FALSE;

	/* Assume cancelled */
	*sn = (-1);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Show choices */
	prepare_window(PW_SPELL);

	window_stuff(player_ptr);

	/* Build a prompt (accept all spells) */
#ifdef JP
	jverb1(prompt, jverb_buf);
	(void) strnfmt(out_val, 78, "(%^s:%c-%c, '*'で一覧, ESCで中断) どの%sを%^sますか? ",
		p, I2A(0), I2A(num - 1), p, jverb_buf );
#else
	(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) %^s which %s? ",
		p, I2A(0), I2A(num - 1), prompt, p);
#endif

	/* Get a spell from the user */

	choice = (use_menu) ? ESCAPE : 1;
	while (!flag)
	{
		if(choice == ESCAPE) choice = ' '; 
		else if(!get_com(out_val, &choice, TRUE))break;

		if(use_menu && choice != ' ')
		{
			switch (choice)
			{
			case '0':
				{
					screen_load();
					return FALSE;
				}

			case '8':
			case 'k':
			case 'K':
				{
					menu_line += (num - 1);
					break;
				}

			case '2':
			case 'j':
			case 'J':
				{
					menu_line++;
					break;
				}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
			if(menu_line > num) menu_line -= num;
			/* Display a list of spells */
			print_spells(creature_ptr, menu_line, spells, num, 1, 15, use_realm);
			if(ask) continue;
		}
		else
		{
			/* Request redraw */
			if((choice == ' ') || (choice == '*') || (choice == '?'))
			{
				if(!redraw)
				{
					redraw = TRUE;
					screen_save();
					print_spells(creature_ptr, menu_line, spells, num, 1, 15, use_realm);
				}

				else
				{
					if(use_menu) continue;

					redraw = FALSE;

					screen_load();
				}

				/* Redo asking */
				continue;
			}


			ask = (isupper(choice));

			if(ask) choice = (char)tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = spells[i];

		/* Require "okay" spells */
		if(!spell_okay(creature_ptr, spell, learned, FALSE, use_realm))
		{
			bell();
			msg_format(MES_CAST_DISABLE(p, prompt));
			continue;
		}

		if(ask)
		{
			char tmp_val[160];

			/* Access the spell */
			if(!is_magic(use_realm))
				s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
			else
				s_ptr = &magic_info[creature_ptr->class_idx].info[use_realm][spell];

			/* Extract mana consumption rate */
			if(use_realm == REALM_HISSATSU)
				need_mana = s_ptr->smana;
			else
				need_mana = mod_need_mana(creature_ptr, s_ptr->smana, spell, use_realm);

#ifdef JP
			jverb1( prompt, jverb_buf );
			/* 英日切り替え機能に対応 */
			(void) strnfmt(tmp_val, 78, "%s(MP%d, 失敗率%d%%)を%sますか? ",
				do_spell(creature_ptr, use_realm, spell, SPELL_NAME), need_mana,
				spell_chance(creature_ptr, spell, use_realm),jverb_buf);
#else
			(void)strnfmt(tmp_val, 78, "%^s %s (%d mana, %d%% fail)? ",
				prompt, do_spell(creature_ptr, use_realm, spell, SPELL_NAME), need_mana,
				spell_chance(creature_ptr, spell, use_realm));
#endif

			/* Belay that order */
			if(!get_check(tmp_val)) continue;
		}
		flag = TRUE;
	}

	if(redraw) screen_load();
	prepare_window(PW_SPELL);
	window_stuff(player_ptr);

	/* Abort if needed */
	if(!flag) return FALSE;

	/* Save the choice */
	(*sn) = spell;
	repeat_push(*sn);
	return TRUE;
}


static bool item_tester_learn_trait(creature_type *creature_ptr, object_type *object_ptr)
{
	s32b choices = realm_choices2[creature_ptr->class_idx];

	if(creature_ptr->class_idx == CLASS_PRIEST)
	{
		if(is_good_realm(creature_ptr->realm1))
		{
			choices &= ~(CH_DEATH | CH_DAEMON);
		}
		else
		{
			choices &= ~(CH_LIFE | CH_CRUSADE);
		}
	}

	if((object_ptr->tval < TV_LIFE_BOOK) || (object_ptr->tval > (TV_LIFE_BOOK + MAX_REALM - 1))) return FALSE;
	if((object_ptr->tval == TV_MUSIC_BOOK) && (creature_ptr->class_idx == CLASS_BARD)) return TRUE;
	else if(!is_magic(tval2realm(object_ptr->tval))) return FALSE;
	if((REALM1_BOOK(creature_ptr) == object_ptr->tval) || (REALM2_BOOK(creature_ptr) == object_ptr->tval)) return TRUE;
	if(choices & (0x0001 << (tval2realm(object_ptr->tval) - 1))) return TRUE;
	return FALSE;
}


static bool creature_has_no_spellbooks(creature_type *creature_ptr)
{
	floor_type  *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int         i;
	object_type *object_ptr;

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		if(object_ptr->k_idx && check_book_realm(creature_ptr, object_ptr->tval, object_ptr->sval)) return FALSE;
	}

	for (i = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx; i; i = object_ptr->next_object_idx)
	{
		object_ptr = &object_list[i];
		if(object_ptr->k_idx && (object_ptr->marked & OM_FOUND) && check_book_realm(creature_ptr, object_ptr->tval, object_ptr->sval)) return FALSE;
	}

	return TRUE;
}


static void confirm_use_force(creature_type *creature_ptr, bool browse_only)
{
	INVENTORY_ID item;
	char which;

	/* Get the item index */
	if(repeat_pull((KEY *)&item) && (item == INVEN_FORCE))
	{
		browse_only ? do_cmd_mind_browse(creature_ptr) : do_cmd_mind(creature_ptr);
		return;
	}

	prt(MES_INTERFACE_FORCE, 0, 0);

	while(TRUE)
	{
		which = inkey();

		if(which == ESCAPE) break;
		else if(which == 'w')
		{
			repeat_push(INVEN_FORCE);
			break;
		}
	}

	/* Clear the prompt line */
	prt("", 0, 0);

	if(which == 'w')
	{
		browse_only ? do_cmd_mind_browse(creature_ptr) : do_cmd_mind(creature_ptr);
	}
}


/*
* Peruse the spells/prayers in a book
*
* Note that *all* spells in the book are listed
*
* Note that browsing is allowed while confused or blind,
* and in the dark, primarily to allow browsing in stores.
*/
void do_cmd_browse(creature_type *creature_ptr)
{
	OBJECT_ID item;
	SVAL sval;
	int use_realm = 0, j, line;
	KEY spell = -1;
	int num = 0;
	int select_flag = 0;

	byte	spells[64];
	char	temp[62*4];

	int item_tester_tval;
	object_type	*object_ptr;

	/* Warriors are illiterate */
	if(!(creature_ptr->realm1 || creature_ptr->realm2) && (creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_RED_MAGE))
	{
		msg_print(MES_STUDY_NO_READING);
		return;
	}

	free_posture(creature_ptr);

	if(creature_ptr->class_idx == CLASS_FORCETRAINER)
	{
		select_flag = USE_INVEN | USE_FLOOR | USE_FORCE;
		if(creature_has_no_spellbooks(creature_ptr))
		{
			confirm_use_force(creature_ptr, TRUE);
			return;
		}
	}
	else
	{
		select_flag = USE_INVEN | USE_FLOOR;
	}

	// Restrict choices to "useful" books
	if(creature_ptr->realm2 == REALM_NONE) item_tester_tval = magic_info[creature_ptr->class_idx].spell_book;

	if(!get_item(creature_ptr, &item, MES_STUDY_WHICH_BOOK, MES_STUDY_NO_BOOK, (USE_INVEN | USE_FLOOR | USE_FORCE), item_tester_hook_readable, 0))
		return;

	if(item == INVEN_FORCE) /* the_force */
	{
		do_cmd_mind_browse(creature_ptr);
		return;
	}

	object_ptr = GET_ITEM(creature_ptr, item);

	/* Access the item's sval */
	sval = object_ptr->sval;

	use_realm = tval2realm(object_ptr->tval);

	/* Track the object kind */
	object_kind_track(object_ptr->k_idx);

	handle_stuff(creature_ptr);


	/* Extract spells */
	for (spell = 0; spell < REALM_MAGIC_NUMBER; spell++)
	{
		/* Check for this spell */
		if((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}


	screen_save();

	/* Clear the top line */
	prt("", 0, 0);

	/* Keep browsing spells.  Exit browsing on cancel. */
	while(TRUE)
	{
		/* Ask for a spell, allow cancel */
		if(!get_spell(creature_ptr, &spell, MES_CAST_BROWSE, object_ptr->sval, TRUE, use_realm))
		{
			/* If cancelled, leave immediately. */
			if(spell == -1) break;

			/* Display a list of spells */
			print_spells(creature_ptr, 0, spells, num, 1, 15, use_realm);

			/* Notify that there's nothing to see, and wait. */
			if(use_realm == REALM_HISSATSU)
				prt(MES_NO_STUDY_SPELL, 0, 0);
			else
				prt(MES_NO_STUDY_TECH, 0, 0);
			(void)inkey();

			screen_load();
			return;
		}

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(14, 14, 255);
		Term_erase(14, 13, 255);
		Term_erase(14, 12, 255);
		Term_erase(14, 11, 255);

		roff_to_buf(do_spell(creature_ptr, use_realm, spell, SPELL_DESC), 62, temp, sizeof(temp));

		for (j = 0, line = 11; temp[j]; j += 1 + strlen(&temp[j]))
		{
			prt(&temp[j], line, 15);
			line++;
		}
	}

	screen_load();
}


static void change_realm2(creature_type *creature_ptr, int next_realm)
{
	int i, j = 0;
	char tmp[80];

	for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++)
	{
		creature_ptr->spell_order[j] = creature_ptr->spell_order[i];
		if(creature_ptr->spell_order[i] < REALM_MAGIC_NUMBER) j++;
	}
	for (; j < (REALM_MAGIC_NUMBER * 2); j++)
		creature_ptr->spell_order[j] = 99;

	creature_ptr->spell_learned2 = 0L;
	creature_ptr->spell_worked2 = 0L;
	creature_ptr->spell_forgotten2 = 0L;

#ifdef JP
	sprintf(tmp, "魔法の領域を%sから%sに変更した。", realm_names[creature_ptr->realm2], realm_names[next_realm]);
#else
	sprintf(tmp, "change magic realm from %s to %s.", realm_names[creature_ptr->realm2], realm_names[next_realm]);
#endif
	do_cmd_write_diary(DIARY_BUNSHOU, 0, tmp);
	creature_ptr->old_realm |= 1 << (creature_ptr->realm2-1);
	creature_ptr->realm2 = next_realm;

	prepare_update(creature_ptr, CRU_REORDER | CRU_SPELLS);
	handle_stuff(creature_ptr);

	/* Load an autopick preference file */
	autopick_load_pref(FALSE);
}


/*
* Study a book to gain a new spell/prayer
*/
void do_cmd_study(creature_type *creature_ptr)
{
	int	i;
	OBJECT_ID item;
	SVAL sval;
	KEY increment = 0;
	bool learned = FALSE;
	int item_tester_tval = 0;

	/* Spells of realm2 will have an increment of +32 */
	KEY	spell = -1;
	cptr p = spell_category_name(magic_info[creature_ptr->class_idx].spell_book);
	object_type *object_ptr;

	if(!creature_ptr->realm1)
	{
		msg_print(MES_STUDY_NO_READING);
		return;
	}

	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr))
	{
		msg_print(MES_IS_BLIND);
		return;
	}

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return;
	}

	if(!(creature_ptr->new_spells))
	{
		msg_format(MES_STUDY_NO_SLOT(p));
		return;
	}

	free_posture(creature_ptr);
	msg_format(MES_STUDY_SLOT(creature_ptr->new_spells, p));
	msg_print(NULL);

	/* Restrict choices to "useful" books */
	if(creature_ptr->realm2 == REALM_NONE) item_tester_tval = magic_info[creature_ptr->class_idx].spell_book;

	if(!get_item(creature_ptr, &item, MES_STUDY_WHICH_BOOK, MES_STUDY_NO_BOOK, (USE_INVEN | USE_FLOOR), item_tester_learn_trait, item_tester_tval)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Access the item's sval */
	sval = object_ptr->sval;

	if(object_ptr->tval == REALM2_BOOK(creature_ptr)) increment = 32;
	else if(object_ptr->tval != REALM1_BOOK(creature_ptr))
	{
		if(!get_check(MES_SPELL_CHANGE_REALM_ASK)) return;
		change_realm2(creature_ptr, tval2realm(object_ptr->tval));
		increment = 32;
	}

	/* Track the object kind */
	object_kind_track(object_ptr->k_idx);

	handle_stuff(creature_ptr);

	/* Mage -- Learn a selected spell */
	if(magic_info[creature_ptr->class_idx].spell_book != TV_LIFE_BOOK)
		if(!get_spell(creature_ptr, &spell, KW_STUDY, sval, FALSE, object_ptr->tval - TV_LIFE_BOOK + 1) && (spell == -1)) return;

	/* Priest -- Learn a random prayer */
	else
	{
		int k = 0;
		KEY gift = -1;

		/* Extract spells */
		for (spell = 0; spell < REALM_MAGIC_NUMBER; spell++)
		{
			/* Check spells in the book */
			if((fake_spell_flags[sval] & (1L << spell)))
			{
				/* Skip non "okay" prayers */
				if(!spell_okay(creature_ptr, spell, FALSE, TRUE,
					(increment ? creature_ptr->realm2 : creature_ptr->realm1))) continue;

				/* Hack -- Prepare the randomizer */
				k++;

				/* Hack -- Apply the randomizer */
				if(one_in_(k)) gift = spell;
			}
		}

		/* Accept gift */
		spell = gift;
	}

	/* Nothing to study */
	if(spell < 0)
	{
		msg_format(MES_STUDY_NO_SPELL(p));
		return;
	}


	if(increment) spell += increment;

	/* Learn the spell */
	if(spell < REALM_MAGIC_NUMBER)
	{
		if(creature_ptr->spell_learned1 & (1L << spell)) learned = TRUE;
		else creature_ptr->spell_learned1 |= (1L << spell);
	}
	else
	{
		if(creature_ptr->spell_learned2 & (1L << (spell - 32))) learned = TRUE;
		else creature_ptr->spell_learned2 |= (1L << (spell - 32));
	}

	if(learned)
	{
		msg_format(MES_STUDY_PERFECT_LEARNED(p));
		return;
	}
	else
	{
		/* Find the next open entry in "creature_ptr->spell_order[]" */
		for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++) if(creature_ptr->spell_order[i] == 99) break;
		creature_ptr->spell_order[i++] = spell; // Add the spell to the known list

		/* Mention the result */
#ifdef JP
		/* 英日切り替え機能に対応 */
		if(magic_info[creature_ptr->class_idx].spell_book == TV_MUSIC_BOOK)
			msg_format("%sを学んだ。", do_spell(creature_ptr, increment ? creature_ptr->realm2 : creature_ptr->realm1, spell % 32, SPELL_NAME));
		else
			msg_format("%sの%sを学んだ。", do_spell(creature_ptr, increment ? creature_ptr->realm2 : creature_ptr->realm1, spell % 32, SPELL_NAME) ,p);
#else
		msg_format("You have learned the %s of %s.", p, do_spell(creature_ptr, increment ? creature_ptr->realm2 : creature_ptr->realm1, spell % 32, SPELL_NAME));
#endif
	}

	cost_tactical_energy(creature_ptr, 100);
	sound(SOUND_STUDY);

	/* One less spell available */
	creature_ptr->learned_spells++;

	/* Update Study */
	prepare_update(creature_ptr, CRU_SPELLS);
	update_creature(creature_ptr, TRUE);

	prepare_window(PW_OBJECT);
}


// Cast a spell
void do_cmd_cast(creature_type *creature_ptr)
{
	OBJECT_ID item;
	SVAL sval;
	KEY spell;
	REALM_ID realm;
	int	chance;
	int	increment = 0;
	int	use_realm;
	int	need_mana;
	int select_flag;
	int item_tester_tval;

	bool over_exerted = FALSE;
	cptr prayer;
	object_type	*object_ptr;
	magic_type	*s_ptr;

	/* Require spell ability */
	if(!creature_ptr->realm1 && (creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_RED_MAGE))
	{
		msg_print(MES_SPELL_CANNOT);
		return;
	}

	/* Require lite */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr))
	{
		if(creature_ptr->class_idx == CLASS_FORCETRAINER) confirm_use_force(creature_ptr, FALSE);
		else
		{
			msg_print(MES_IS_BLIND);
			flush();
		}
		return;
	}

	/* Not when confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		flush();
		return;
	}

	/* Hex */
	if(creature_ptr->realm1 == REALM_HEX)
	{
		if(hex_spell_fully(creature_ptr))
		{
			bool flag = FALSE;
			msg_print(MES_SPELL_NO_MORE_SPELLING);
			flush();
			if(creature_ptr->lev >= 35) flag = stop_hex_spell(creature_ptr);
			if(!flag) return;
		}
	}

	if(creature_ptr->class_idx == CLASS_FORCETRAINER)
	{
		if(creature_has_no_spellbooks(creature_ptr))
		{
			confirm_use_force(creature_ptr, FALSE);
			return;
		}
		select_flag = USE_INVEN | USE_FLOOR | USE_FORCE;
	}
	else
	{
		select_flag = USE_INVEN | USE_FLOOR;
	}

	prayer = spell_category_name(magic_info[creature_ptr->class_idx].spell_book);

	/* Restrict choices to spell books */
	item_tester_tval = magic_info[creature_ptr->class_idx].spell_book;

	if(!get_item(creature_ptr, &item, MES_CAST_WHICH_BOOK, MES_CAST_NO_BOOK, select_flag, NULL, item_tester_tval)) return;

	if(item == INVEN_FORCE) /* the_force */
	{
		do_cmd_mind(creature_ptr);
		return;
	}
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Access the item's sval */
	sval = object_ptr->sval;

	if((creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_RED_MAGE) && (object_ptr->tval == REALM2_BOOK(creature_ptr))) increment = 32;


	/* Track the object kind */
	object_kind_track(object_ptr->k_idx);

	handle_stuff(creature_ptr);

	if((creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE))
		realm = object_ptr->tval - TV_LIFE_BOOK + 1;
	else if(increment) realm = creature_ptr->realm2;
	else realm = creature_ptr->realm1;

	/* Ask for a spell */
#ifdef JP
	if(!get_spell(creature_ptr, &spell, 
		((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "詠唱する" : (magic_info[creature_ptr->class_idx].spell_book == TV_MUSIC_BOOK) ? "歌う" : "唱える"), sval, TRUE, realm))
	{
		if(spell == -2) msg_format("その本には知っている%sがない。", prayer);
		return;
	}
#else
	if(!get_spell(creature_ptr, &spell, ((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
		sval, TRUE, realm))
	{
		if(spell == -2)
			msg_format("You don't know any %ss in that book.", prayer);
		return;
	}
#endif


	use_realm = tval2realm(object_ptr->tval);

	if(use_realm == REALM_HEX)
	{
		if(HEX_SPELLING(creature_ptr, spell))
		{
			msg_print(MES_SPELL_ALREADY);
			return;
		}
	}

	if(!is_magic(use_realm))
	{
		s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
	}
	else
	{
		s_ptr = &magic_info[creature_ptr->class_idx].info[realm - 1][spell];
	}

	/* Extract mana consumption rate */
	need_mana = mod_need_mana(creature_ptr, s_ptr->smana, spell, realm);

	/* Verify "dangerous" spells */
	if(need_mana > creature_ptr->csp)
	{
		if(flush_failure) flush();

#ifdef JP
		msg_format("その%sを%sのに十分なマジックポイントがない。",prayer,
			((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "詠唱する" : (magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "歌う" : "唱える"));
#else
		msg_format("You do not have enough mana to %s this %s.",
			((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
			prayer);
#endif
		if(!over_exert) return;
		if(!get_check_strict(MES_ATTEMPT_ANYWAY, CHECK_OKAY_CANCEL)) return;

	}

	chance = spell_chance(creature_ptr, spell, use_realm);

	if(need_mana <= creature_ptr->csp) creature_ptr->csp -= need_mana;
	else over_exerted = TRUE;

	if(PERCENT(chance))
	{
		if(flush_failure) flush();
		msg_format(MES_CAST_FAILED(prayer));
		sound(SOUND_FAIL);

		/* Failure casting may activate some side effect */
		do_spell(creature_ptr, realm, spell, SPELL_FAIL);

		if((object_ptr->tval == TV_CHAOS_BOOK) && PERCENT(spell))
		{
			msg_print(MES_CAST_CHAOS_PENALTY);
			wild_magic(creature_ptr, spell);
		}
		else if((object_ptr->tval == TV_DEATH_BOOK) && PERCENT(spell))
		{
			if((sval == 3) && one_in_(2))
			{
				msg_print(MES_CAST_NECRO_PENALTY1);
				sanity_blast_aux(creature_ptr, 120);
			}
			else
			{
				msg_print(MES_CAST_NECRO_PENALTY2);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(object_ptr->sval + 1, 6), COD_MISCAST_DEATH_SPELL, NULL, -1);

				if((spell > 15) && one_in_(6) && !has_trait(creature_ptr, TRAIT_HOLD_LIFE))
					lose_exp(creature_ptr, spell * 250);
			}
		}
		else if((object_ptr->tval == TV_MUSIC_BOOK) && PERCENT(spell / 2))
		{
			msg_print(MES_CAST_MUSIC_PENALTY);
			aggravate_creatures(creature_ptr);
		}
	}

	else
	{
		/* Canceled spells cost neither a turn nor mana */
		if(!do_spell(creature_ptr, realm, spell, SPELL_CAST)) return;

		/* A spell was cast */
		if(!(increment ? (creature_ptr->spell_worked2 & (1L << spell)) :
		(creature_ptr->spell_worked1 & (1L << spell)))
			&& (creature_ptr->class_idx != CLASS_SORCERER)
			&& (creature_ptr->class_idx != CLASS_RED_MAGE))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if(realm == creature_ptr->realm1)
				creature_ptr->spell_worked1 |= (1L << spell);
			else
				creature_ptr->spell_worked2 |= (1L << spell);

			/* Gain experience */
			gain_exp(creature_ptr, e * s_ptr->slevel, 0, FALSE);

			prepare_window(PW_OBJECT);

		}
		if(magic_info[creature_ptr->class_idx].spell_xtra & MAGIC_GAIN_EXP)
		{
			//TODO gain_skill(creature_ptr, TODO, amount)
		}
	}

	cost_tactical_energy(creature_ptr, 100);

	/* Over-exert the player */
	if(over_exerted)
	{
		int oops = need_mana;
		dec_mana(creature_ptr, need_mana);
		msg_print(MES_CAST_FAINT);
		(void)add_timed_trait(creature_ptr, TRAIT_SLEPT, randint1(5 * oops + 1), FALSE);

		/* Damage CON (possibly permanently) */
		if(PERCENT(50))
		{
			bool perm = (PERCENT(25));
			msg_print(MES_CAST_DAMAGE_HEALTH);
			(void)dec_stat(creature_ptr, STAT_CON, 15 + randint1(10), perm);
		}
	}

	prepare_window(PW_PLAYER);
	prepare_window(PW_SPELL);
}


static bool ang_sort_comp_pet_dismiss(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	creature_type *creature_ptr1 = &creature_list[w1];
	creature_type *creature_ptr2 = &creature_list[w2];
	species_type *species_ptr1 = &species_info[creature_ptr1->species_idx];
	species_type *species_ptr2 = &species_info[creature_ptr2->species_idx];

	/* Unused */
	(void)v;

	//TODO player's steed
	if(creature_ptr2->ridden) return TRUE;
	if(creature_ptr1->ridden) return FALSE;

	if(creature_ptr1->nickname && !creature_ptr2->nickname) return TRUE;
	if(creature_ptr2->nickname && !creature_ptr1->nickname) return FALSE;

	if(!creature_ptr1->parent_m_idx && creature_ptr2->parent_m_idx) return TRUE;
	if(!creature_ptr2->parent_m_idx && creature_ptr1->parent_m_idx) return FALSE;

	if(has_trait_species(species_ptr1, TRAIT_UNIQUE) && !has_trait_species(species_ptr2, TRAIT_UNIQUE)) return TRUE;
	if(has_trait_species(species_ptr2, TRAIT_UNIQUE) && !has_trait_species(species_ptr1, TRAIT_UNIQUE)) return FALSE;

	if(species_ptr1->level > species_ptr2->level) return TRUE;
	if(species_ptr2->level > species_ptr1->level) return FALSE;

	if(creature_ptr1->chp > creature_ptr2->chp) return TRUE;
	if(creature_ptr2->chp > creature_ptr1->chp) return FALSE;

	return w1 <= w2;
}

void check_pets_num_and_align(creature_type *master_ptr, creature_type *m_ptr, bool inc)
{
	s32b old_friend_align = friend_align;
	species_type *species_ptr = &species_info[m_ptr->species_idx];

	if(inc)
	{
		master_ptr->total_friends++;
		if(is_enemy_of_evil_creature(m_ptr)) friend_align += species_ptr->level;
		if(is_enemy_of_good_creature(m_ptr)) friend_align -= species_ptr->level;
	}
	else
	{
		master_ptr->total_friends--;
		if(is_enemy_of_evil_creature(m_ptr)) friend_align -= species_ptr->level;
		if(is_enemy_of_good_creature(m_ptr)) friend_align += species_ptr->level;
	}

	if(old_friend_align != friend_align) prepare_update(player_ptr, CRU_BONUS);
}

int calculate_upkeep_servant(creature_type *master_ptr)
{
	s32b old_friend_align = friend_align;
	int m_idx;
	bool have_a_unique = FALSE;
	s32b total_friend_levels = 0;

	master_ptr->total_friends = 0;
	friend_align = 0;

	for (m_idx = creature_max - 1; m_idx >=1; m_idx--)
	{
		creature_type *pet_ptr;

		pet_ptr = &creature_list[m_idx];
		if(!pet_ptr->species_idx) continue;

		if(is_pet(player_ptr, pet_ptr))
		{
			master_ptr->total_friends++;
			if(has_trait(master_ptr, TRAIT_UNIQUE))
			{
				if(master_ptr->class_idx == CLASS_CAVALRY)
				{
					if(master_ptr->riding == m_idx)
						total_friend_levels += (pet_ptr->lev+5)*2;
					else if(!have_a_unique && has_trait(pet_ptr, TRAIT_RIDING))
						total_friend_levels += (pet_ptr->lev+5)*7/2;
					else
						total_friend_levels += (pet_ptr->lev+5)*10;
					have_a_unique = TRUE;
				}
				else
					total_friend_levels += (pet_ptr->lev+5)*10;
			}
			else
				total_friend_levels += pet_ptr->lev;

			/* Determine pet alignment */
			if(is_enemy_of_evil_creature(pet_ptr)) friend_align += pet_ptr->lev;
			if(is_enemy_of_good_creature(pet_ptr)) friend_align -= pet_ptr->lev;
		}
	}
	if(old_friend_align != friend_align) prepare_update(master_ptr, CRU_BONUS);
	if(master_ptr->total_friends)
	{
		int upkeep_factor;
		upkeep_factor = (total_friend_levels - (master_ptr->lev * 80 / (class_info[master_ptr->class_idx].pet_upkeep_div)));
		if(upkeep_factor < 0) upkeep_factor = 0;
		if(upkeep_factor > 1000) upkeep_factor = 1000;
		return upkeep_factor;
	}
	else
		return 0;
}

void do_cmd_pet_dismiss(creature_type *creature_ptr)
{
	creature_type	*m_ptr;
	bool		all_pets = FALSE;
	CREATURE_ID pet_ctr;
	int i;
	int Dismissed = 0;

	CREATURE_ID *who;
	u16b dummy_why;
	int max_pet = 0;
	int cu, cv;

	cu = Term->scr->cu;
	cv = Term->scr->cv;
	Term->scr->cu = 0;
	Term->scr->cv = 1;

	/* Allocate the "who" array */
	C_MAKE(who, max_creature_idx, CREATURE_ID);

	/* Process the creatures (backwards) */
	for (pet_ctr = creature_max - 1; pet_ctr >= 1; pet_ctr--)
	{
		if(is_pet(player_ptr, &creature_list[pet_ctr])) who[max_pet++] = pet_ctr;
	}

	ang_sort(who, &dummy_why, max_pet, ang_sort_comp_pet_dismiss, ang_sort_swap_hook);

	/* Process the creatures (backwards) */
	for (i = 0; i < max_pet; i++)
	{
		bool delete_this;
		char friend_name[80];
		char buf[80];
		bool kakunin;

		pet_ctr = who[i];
		m_ptr = &creature_list[pet_ctr];

		delete_this = FALSE;
		kakunin = ((pet_ctr == creature_ptr->riding) || (m_ptr->nickname));
		creature_desc(friend_name, m_ptr, CD_ASSUME_VISIBLE);

		if(!all_pets)
		{
			health_track(pet_ctr);
			handle_stuff(creature_ptr);

			sprintf(buf, MES_PET_DISMISS_ASK(friend_name, max_pet - i));
			prt(buf, 0, 0);

			if(m_ptr->see_others)
				move_cursor_relative(m_ptr->fy, m_ptr->fx);

			while (TRUE)
			{
				char ch = inkey();

				if(ch == 'Y' || ch == 'y')
				{
					delete_this = TRUE;

					if(kakunin)
					{
						sprintf(buf, "%s (%s) ", MES_SYS_ASK_SURE, friend_name);
						if(!get_check(buf)) delete_this = FALSE;
					}
					break;
				}

				if(ch == 'U' || ch == 'u')
				{
					all_pets = TRUE;
					break;
				}

				if(ch == ESCAPE || ch == 'N' || ch == 'n')
					break;

				bell();
			}
		}

		if((all_pets && !kakunin) || (!all_pets && delete_this))
		{
			if(record_named_pet && m_ptr->nickname)
			{
				char m_name[MAX_NLEN];

				creature_desc(m_name, m_ptr, CD_INDEF_VISIBLE);
				do_cmd_write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_DISMISS, m_name);
			}

			if(pet_ctr == creature_ptr->riding)
			{
				msg_format(MES_STEED_GOT_OFF(friend_name));
				creature_ptr->riding = 0;
				m_ptr->ridden = 0;

				// Update creatures
				prepare_update(creature_ptr, CRU_BONUS);
				prepare_update(creature_ptr, PU_CREATURES);
				prepare_redraw(PR_EXTRA | PR_UHEALTH);
			}

			/* TODO : Add the line to message buffer */
			sprintf(buf, MES_PET_RELEASE(friend_name));

			message_add(buf);
			prepare_window(PW_MESSAGE);
			window_stuff(player_ptr);

			delete_species_idx(&creature_list[pet_ctr]);
			Dismissed++;
		}
	}

	Term->scr->cu = (bool_hack)cu;
	Term->scr->cv = (bool_hack)cv;
	Term_fresh();

	C_KILL(who, max_creature_idx, CREATURE_ID);
	msg_format(MES_PET_DISMISS(Dismissed));

	if(Dismissed == 0 && all_pets) msg_print(MES_PER_UNNAMED_DESC);
}

static bool player_can_ride_aux(creature_type *creature_ptr, cave_type *c_ptr, bool now_riding)
{
	bool can_enter;
	int  old_riding = creature_ptr->riding;
	bool old_riding_two_handed = creature_ptr->riding_two_handed;
	bool old_old_riding_two_handed = creature_ptr->old_riding_two_handed;
	bool old_pf_two_handed = (creature_ptr->pet_extra_flags & PF_RYOUTE) ? TRUE : FALSE;

	if(now_riding) creature_ptr->riding = c_ptr->creature_idx;
	else
	{
		creature_ptr->riding = 0;
		creature_ptr->pet_extra_flags &= ~(PF_RYOUTE);
		creature_ptr->riding_two_handed = creature_ptr->old_riding_two_handed = FALSE;
	}

	set_creature_bonuses(creature_ptr, TRUE);

	can_enter = creature_can_cross_terrain(creature_ptr, c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);

	creature_ptr->riding = old_riding;
	if(old_pf_two_handed) creature_ptr->pet_extra_flags |= (PF_RYOUTE);
	else creature_ptr->pet_extra_flags &= ~(PF_RYOUTE);
	creature_ptr->riding_two_handed = old_riding_two_handed;
	creature_ptr->old_riding_two_handed = old_old_riding_two_handed;

	set_creature_bonuses(creature_ptr, TRUE);

	return can_enter;
}

bool do_thrown_from_riding(creature_type *creature_ptr, POWER dam, bool force)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	COODINATES i, y, x, oy, ox, sn = 0, sy = 0, sx = 0;
	char m_name[MAX_NLEN];
	creature_type *m_ptr = &creature_list[creature_ptr->riding];
	species_type *species_ptr = &species_info[m_ptr->species_idx];
	bool fall_dam = FALSE;

	if(!creature_ptr->riding) return FALSE;
	if(floor_ptr->global_map) return FALSE;

	if(dam >= 0 || force)
	{
		if(!force)
		{
			int cur = creature_ptr->skill_exp[SKILL_RIDING];
			int max = 30000; //TODO
			int ridinglevel = species_ptr->level;

			/* 落馬のしやすさ */
			int do_thrown_from_ridinglevel = species_ptr->level;
			if(creature_ptr->riding_two_handed) do_thrown_from_ridinglevel += 20;

			if((cur < max) && (max > 1000) &&
				(dam / 2 + ridinglevel) > (cur / 30 + 10))
			{
				int inc = 0;

				if(ridinglevel > (cur / 100 + 15))
					inc += 1 + (ridinglevel - cur / 100 - 15);
				else
					inc += 1;

				creature_ptr->skill_exp[SKILL_RIDING] = MIN(max, cur + inc);
			}

			/* レベルの低い乗馬からは落馬しにくい */
			if(randint0(dam / 2 + do_thrown_from_ridinglevel * 2) < cur / 30 + 10)
				if((has_trait(creature_ptr, TRAIT_RODEO) && !creature_ptr->riding_two_handed) || !one_in_(creature_ptr->lev*(creature_ptr->riding_two_handed ? 2 : 3) + 30))
					return FALSE;
		}

		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			cave_type *c_ptr;

			/* Access the location */
			y = creature_ptr->fy + ddy_ddd[i];
			x = creature_ptr->fx + ddx_ddd[i];

			c_ptr = &floor_ptr->cave[y][x];

			if(c_ptr->creature_idx) continue;

			/* Skip non-empty grids */
			if(!CAVE_HAVE_FLAG_GRID(c_ptr, FF_MOVE) && !CAVE_HAVE_FLAG_GRID(c_ptr, FF_CAN_FLY))
			{
				if(!player_can_ride_aux(creature_ptr, c_ptr, FALSE)) continue;
			}

			if(CAVE_HAVE_FLAG_GRID(c_ptr, FF_PATTERN)) continue;

			/* Count "safe" grids */
			sn++;

			/* Randomize choice */
			if(randint0(sn) > 0) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}
		if(!sn)
		{
			creature_desc(m_name, m_ptr, 0);
			msg_format(MES_STEED_BUMP_WALL(m_name));
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, species_ptr->level+3, COD_BUNP_WALL, NULL, -1);
			return FALSE;
		}

		oy = creature_ptr->fy;
		ox = creature_ptr->fx;

		creature_ptr->fy = sy;
		creature_ptr->fx = sx;

		lite_spot(floor_ptr, oy, ox);
		lite_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);

		/* Check for new panel */
		verify_panel(creature_ptr);
	}

	creature_ptr->riding = 0;
	creature_ptr->pet_extra_flags &= ~(PF_RYOUTE);
	creature_ptr->riding_two_handed = creature_ptr->old_riding_two_handed = FALSE;

	prepare_update(creature_ptr, CRU_BONUS | PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE | PU_CREATURES);
	update_creature(creature_ptr, TRUE);

	prepare_window(PW_OVERHEAD | PW_DUNGEON);
	prepare_redraw(PR_EXTRA | PR_UHEALTH);

	if(has_trait(creature_ptr, TRAIT_CAN_FLY) && !force)
	{
		creature_desc(m_name, m_ptr, 0);
		msg_format(MES_STEED_GOOD_RANDING(m_name));
	}
	else
	{
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, species_ptr->level+3, COD_FALL_RIDING, NULL, -1);
		fall_dam = TRUE;
	}

	/* Move the player */
	if(sy && !gameover) (void)move_creature(creature_ptr, NULL, creature_ptr->fy, creature_ptr->fx, MCE_DONT_PICKUP | MCE_DONT_SWAP_MON);

	return fall_dam;
}

bool do_riding(creature_type *rider_ptr, bool force)
{
	COODINATES x, y;
	DIRECTION dir = 0;
	cave_type *c_ptr;
	creature_type *steed_ptr;
	floor_type *floor_ptr = GET_FLOOR_PTR(rider_ptr);

	if(!get_rep_dir2(rider_ptr, &dir)) return FALSE;
	y = rider_ptr->fy + ddy[dir];
	x = rider_ptr->fx + ddx[dir];
	c_ptr = &floor_ptr->cave[y][x];

	if(rider_ptr->posture & KATA_MUSOU) set_action(rider_ptr, ACTION_NONE);

	if(rider_ptr->riding)
	{
		/* Skip non-empty grids */
		if(!player_can_ride_aux(rider_ptr, c_ptr, FALSE))
		{
			msg_print(MES_RIDING_NO_DIRECTION);
			return FALSE;
		}

		if(!pattern_seq(rider_ptr, rider_ptr->fy, rider_ptr->fx, y, x)) return FALSE;

		if(c_ptr->creature_idx)
		{
			cost_tactical_energy(rider_ptr, 100);
			msg_print(MES_CREATURE_IN_THE_WAY);
			close_combat(rider_ptr, y, x, 0);
			return FALSE;
		}

		rider_ptr->riding = 0;
		rider_ptr->pet_extra_flags &= ~(PF_RYOUTE);
		rider_ptr->riding_two_handed = rider_ptr->old_riding_two_handed = FALSE;
	}
	else
	{
		if(has_trait(rider_ptr, TRAIT_CONFUSED))
		{
			msg_print(MES_PREVENT_BY_CONFUSION);
			return FALSE;
		}

		steed_ptr = &creature_list[c_ptr->creature_idx];

		if(!c_ptr->creature_idx || !steed_ptr->see_others)
		{
			msg_print(MES_RIDING_NO_CREATURE);
			return FALSE;
		}
		if(!is_pet(player_ptr, steed_ptr) && !force)
		{
			msg_print(MES_RIDING_NOT_PET);
			return FALSE;
		}
		if(!has_trait(steed_ptr, TRAIT_RIDING))
		{
			msg_print(MES_RIDING_DISABLE);
			return FALSE;
		}

		if(!pattern_seq(rider_ptr, rider_ptr->fy, rider_ptr->fx, y, x)) return FALSE;

		if(!player_can_ride_aux(rider_ptr, c_ptr, TRUE))
		{
			/* Feature code (applying "mimic" field) */
			feature_type *f_ptr = &feature_info[get_feat_mimic(c_ptr)];
#ifdef JP
			msg_format("そのクリーチャーは%sの%sにいる。", feature_name + f_ptr->name,
				((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
				(!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE))) ?
				"中" : "上");
#else
			msg_format("This creature is %s the %s.",
				((!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY)) ||
				(!have_flag(f_ptr->flags, FF_LOS) && !have_flag(f_ptr->flags, FF_TREE))) ?
				"in" : "on", feature_name + f_ptr->name);
#endif

			return FALSE;
		}
		if(species_info[steed_ptr->species_idx].level > randint1((rider_ptr->skill_exp[SKILL_RIDING] / 50 + rider_ptr->lev / 2 + 20)))
		{
			msg_print(MES_RIDING_FAILED);
			cost_tactical_energy(rider_ptr, 100);
			return FALSE;
		}

		if(has_trait_from_timed(steed_ptr, TRAIT_PARALYZED) || has_trait_from_timed(steed_ptr, TRAIT_SLEPT))
		{
			char steed_name[MAX_NLEN];
			creature_desc(steed_name, steed_ptr, 0);
			(void)set_timed_trait(steed_ptr, TRAIT_PARALYZED, 0, TRUE);
			(void)set_timed_trait(steed_ptr, TRAIT_SLEPT, 0, TRUE);
			msg_format(MES_STEED_WAKE_UP(steed_name));
		}

		if(rider_ptr->action == ACTION_KAMAE) set_action(rider_ptr, ACTION_NONE);

		rider_ptr->riding = c_ptr->creature_idx;
		steed_ptr->ridden = (s16b)rider_ptr->creature_idx;

		/* Hack -- remove tracked creature */
		if(rider_ptr->riding == npc_status_id) health_track(0);
	}

	cost_tactical_energy(rider_ptr, 100);

	/* Mega-Hack -- Forget the view and lite */
	prepare_update(rider_ptr, PU_UN_VIEW | PU_UN_LITE);

	/* Update the creatures */
	prepare_update(rider_ptr, CRU_BONUS);
	prepare_redraw(PR_MAP | PR_EXTRA | PR_UHEALTH);

	/* Move the player */
	(void)move_creature(rider_ptr, NULL, y, x, MCE_HANDLE_STUFF | MCE_ENERGY_USE | MCE_DONT_PICKUP | MCE_DONT_SWAP_MON);

	return TRUE;
}

static void do_name_pet(creature_type *master_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(master_ptr);
	creature_type *pet_ptr;
	char out_val[20];
	char m_name[MAX_NLEN];
	bool old_name = FALSE;
	bool old_target_pet = target_pet;

	target_pet = TRUE;
	if(!target_set(master_ptr, 0, TARGET_KILL))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	if(floor_ptr->cave[target_row][target_col].creature_idx)
	{
		pet_ptr = &creature_list[floor_ptr->cave[target_row][target_col].creature_idx];

		if(!is_pet(player_ptr, pet_ptr))
		{
			msg_print(MES_PET_NO_PET);
			return;
		}

		if(!has_trait_species(&species_info[pet_ptr->species_idx], TRAIT_UNIQUE))
		{
			msg_print(MES_PET_CANNOT_CHANGE_NAME);
			return;
		}
		creature_desc(m_name, pet_ptr, 0);

		msg_format(MES_PET_CHANGE_NAME(m_name));
		msg_print(NULL);

		/* Start with nothing */
		strcpy(out_val, "");

		/* Use old inscription */
		if(pet_ptr->nickname)
		{
			/* Start with the old inscription */
			strcpy(out_val, quark_str(pet_ptr->nickname));
			old_name = TRUE;
		}

		/* Get a new inscription (possibly empty) */
		if(get_string(KW_NAME, out_val, 15))
		{
			if(out_val[0])
			{
				/* Save the inscription */
				pet_ptr->nickname = quark_add(out_val);
				if(record_named_pet)
				{
					char m_name[MAX_NLEN];

					creature_desc(m_name, pet_ptr, CD_INDEF_VISIBLE);
					do_cmd_write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_NAME, m_name);
				}
			}
			else
			{
				if(record_named_pet && old_name)
				{
					char m_name[MAX_NLEN];

					creature_desc(m_name, pet_ptr, CD_INDEF_VISIBLE);
					do_cmd_write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_UNNAME, m_name);
				}
				pet_ptr->nickname = 0;
			}
		}
	}
}


// Issue a pet command
void do_cmd_pet(creature_type *master_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(master_ptr);
	KEY i = 0;
	KEY num;
	int ask;
	POWER powers[36];
	cptr power_desc[36];
	bool flag, redraw;
	char choice;
	char out_val[160];
	int pet_ctr;
	creature_type *pet_ptr;

	int mode = 0;

	char buf[160];
	char target_buf[160];
	KEY menu_line = use_menu ? 1 : 0;

	num = 0;

	power_desc[num] = MES_PETCOM_DISMISS_PET;
	powers[num++] = PET_DISMISS;

	sprintf(target_buf, MES_PETCOM_TARGETING((pet_t_m_idx ? (has_trait(master_ptr, TRAIT_HALLUCINATION) ? KW_SOMETHING_STRANGE : (species_name + species_info[creature_list[pet_t_m_idx].ap_species_idx].name)) : KW_NOTHING_TARGET)));
	power_desc[num] = target_buf;

	powers[num++] = PET_TARGET;
	power_desc[num] = MES_PETCOM_STAY_CLOSE;

	if(master_ptr->pet_follow_distance == PET_CLOSE_DIST) mode = num;
	powers[num++] = PET_STAY_CLOSE;

	power_desc[num] = MES_PETCOM_FOLLOW_ME;
	if(master_ptr->pet_follow_distance == PET_FOLLOW_DIST) mode = num;
	powers[num++] = PET_FOLLOW_ME;

	power_desc[num] = MES_PETCOM_SEEK_AND_DESTROY;
	if(master_ptr->pet_follow_distance == PET_DESTROY_DIST) mode = num;
	powers[num++] = PET_SEEK_AND_DESTROY;

	power_desc[num] = MES_PETCOM_GIVE_ME_SPACE;
	if(master_ptr->pet_follow_distance == PET_SPACE_DIST) mode = num;
	powers[num++] = PET_ALLOW_SPACE;

	power_desc[num] = MES_PETCOM_STAY_AWAY;
	if(master_ptr->pet_follow_distance == PET_AWAY_DIST) mode = num;
	powers[num++] = PET_STAY_AWAY;

	if(master_ptr->pet_extra_flags & PF_OPEN_DOORS)
		power_desc[num] = MES_PETCOM_OPEN_DOORS_ON;
	else
		power_desc[num] = MES_PETCOM_OPEN_DOORS_OFF;
	powers[num++] = PET_OPEN_DOORS;

	if(master_ptr->pet_extra_flags & PF_PICKUP_ITEMS)
		power_desc[num] = MES_PETCOM_OPEN_PICKUP_ITEM_ON;
	else
		power_desc[num] = MES_PETCOM_OPEN_PICKUP_ITEM_OFF;
	powers[num++] = PET_TAKE_ITEMS;

	if(master_ptr->pet_extra_flags & PF_TELEPORT)
		power_desc[num] = MES_PETCOM_ALLOW_TELEPORT_ON;
	else
		power_desc[num] = MES_PETCOM_ALLOW_TELEPORT_OFF;
	powers[num++] = PET_TELEPORT;

	if(master_ptr->pet_extra_flags & PF_ATTACK_SPELL)
		power_desc[num] = MES_PETCOM_ATTACK_SPELL_ON;
	else
		power_desc[num] = MES_PETCOM_ATTACK_SPELL_OFF;
	powers[num++] = PET_ATTACK_SPELL;

	if(master_ptr->pet_extra_flags & PF_TRAIT_S_SPELL)
		power_desc[num] = MES_PETCOM_SUMMON_SPELL_ON;
	else
		power_desc[num] = MES_PETCOM_SUMMON_SPELL_OFF;
	powers[num++] = PET_TRAIT_S_SPELL;

	if(master_ptr->pet_extra_flags & PF_BALL_SPELL)
		power_desc[num] = MES_PETCOM_INVOLVE_ON;
	else
		power_desc[num] = MES_PETCOM_INVOLVE_OFF;
	powers[num++] = PET_BALL_SPELL;

	if(master_ptr->riding)
		power_desc[num] = MES_PETCOM_GET_OFF;
	else
		power_desc[num] = MES_PETCOM_RIDE;
	powers[num++] = PET_RIDING;

	power_desc[num] = MES_PETCOM_GIVE_PET_NAME;
	powers[num++] = PET_NAME;

	if(master_ptr->riding)
	{
		if((master_ptr->can_melee[0] && (empty_hands(master_ptr, FALSE) == EMPTY_HAND_LARM) &&
			object_allow_two_hands_wielding(master_ptr, get_equipped_slot_ptr(master_ptr, INVENTORY_ID_HAND, 0))) ||
			(master_ptr->can_melee[1] && (empty_hands(master_ptr, FALSE) == EMPTY_HAND_RARM) &&
			object_allow_two_hands_wielding(master_ptr, get_equipped_slot_ptr(master_ptr, INVENTORY_ID_HAND, 1))))
		{
			if(master_ptr->pet_extra_flags & PF_RYOUTE) power_desc[num] = MES_COMBATOP_WEAPON_ONE_HANDED;
			else power_desc[num] = MES_COMBATOP_WEAPON_TWO_HANDED;
			powers[num++] = PET_RYOUTE;
		}
		else
		{
			switch (master_ptr->class_idx)
			{
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
			case CLASS_BERSERKER:
				if(empty_hands(master_ptr, FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))
				{
					if(master_ptr->pet_extra_flags & PF_RYOUTE)
						power_desc[num] = MES_COMBATOP_BAREHAND_ONE_HANDED;
					else
						power_desc[num] = MES_COMBATOP_BAREHAND_TWO_HANDED;
					powers[num++] = PET_RYOUTE;
				}
				else if((empty_hands(master_ptr, FALSE) != EMPTY_HAND_NONE) && !get_equipped_slot_num(master_ptr, INVENTORY_ID_HAND))
				{
					if(master_ptr->pet_extra_flags & PF_RYOUTE)
						power_desc[num] = MES_COMBATOP_BAREHAND_OFF;
					else
						power_desc[num] = MES_COMBATOP_BAREHAND_ON;
					powers[num++] = PET_RYOUTE;
				}
				break;
			}
		}
	}

	if(!(repeat_pull(&i) && (i >= 0) && (i < num)))
	{
		flag = FALSE; // Nothing chosen yet
		redraw = FALSE; // No redraw yet
		screen_save(); // Save the screen
		choice = (use_menu) ? ESCAPE : 1;

		while (!flag)
		{
			if(choice == ESCAPE) choice = ' ';
			else if(!get_com(out_val, &choice, TRUE)) break;

			if(use_menu && (choice != ' '))
			{
				switch (choice)
				{
				case '0':
					screen_load();
					return;

				case '8':
				case 'k':
				case 'K':
					menu_line += (num - 1);
					break;

				case '2':
				case 'j':
				case 'J':
					menu_line++;
					break;

				case '4':
				case 'h':
				case 'H':
					menu_line = 1;
					break;

				case '6':
				case 'l':
				case 'L':
					menu_line = num;
					break;

				case 'x':
				case 'X':
				case '\r':
				case '\n':
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
				if(menu_line > num) menu_line -= num;
			}

			/* Request redraw */
			if((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
			{
				if(!redraw || use_menu)
				{
					byte y = 1, x = 0;
					int ctr = 0;
					redraw = TRUE;
					if(!use_menu) screen_save();
					prt("", y++, x);

					/* Print list */
					for (ctr = 0; ctr < num; ctr++)
					{
						/* Letter/number for power selection */
#ifdef JP
						if(use_menu) sprintf(buf, "%c%s ", (ctr == mode) ? '*' : ' ', (ctr == (menu_line - 1)) ? "》" : "  ");
#else
						if(use_menu) sprintf(buf, "%c%s ", (ctr == mode) ? '*' : ' ', (ctr == (menu_line - 1)) ? "> " : "  ");
#endif
						else sprintf(buf, "%c%c) ", (ctr == mode) ? '*' : ' ', I2A(ctr));

						strcat(buf, power_desc[ctr]);

						prt(buf, y + ctr, x);
					}

					prt("", y + MIN(ctr, 17), x);
				}

				else
				{
					redraw = FALSE;

					screen_load();
				}

				/* Redo asking */
				continue;
			}

			if(!use_menu)
			{
				ask = (isupper(choice));

				if(ask) choice = (char)tolower(choice);

				/* Extract request */
				i = (islower(choice) ? A2I(choice) : -1);
			}

			/* Totally Illegal */
			if((i < 0) || (i >= num))
			{
				bell();
				continue;
			}

			if(ask)
			{
				strnfmt(buf, 78, MES_SYS_ASK_USE, power_desc[i]);
				/* Belay that order */
				if(!get_check(buf)) continue;
			}

			flag = TRUE;
		}

		if(redraw) screen_load();

		/* Abort if needed */
		if(!flag)
		{
			cancel_tactical_action(master_ptr);
			return;
		}

		repeat_push(i);
	}

	switch (powers[i])
	{
	case PET_DISMISS: /* Dismiss pets */
		{
			/* Check pets (backwards) */
			for (pet_ctr = creature_max - 1; pet_ctr >= 1; pet_ctr--)
			{
				/* Player has pet */
				if(is_pet(player_ptr, &creature_list[pet_ctr])) break;
			}

			if(!pet_ctr)
			{
				msg_print(MES_PET_NOTHING);
				break;
			}
			do_cmd_pet_dismiss(master_ptr);
			(void)calculate_upkeep_servant(master_ptr);
			break;
		}
	case PET_TARGET:
		{
			if(!target_set(master_ptr, NO_RANGE_LIMIT, TARGET_KILL)) pet_t_m_idx = 0;
			else
			{
				cave_type *c_ptr = &floor_ptr->cave[target_row][target_col];
				if(c_ptr->creature_idx && (creature_list[c_ptr->creature_idx].see_others))
				{
					pet_t_m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
					master_ptr->pet_follow_distance = PET_DESTROY_DIST;
				}
				else pet_t_m_idx = 0;
			}
			break;
		}
		/* Call pets */
	case PET_STAY_CLOSE:
		{
			master_ptr->pet_follow_distance = PET_CLOSE_DIST;
			pet_t_m_idx = 0;
			break;
		}
		/* "Follow Me" */
	case PET_FOLLOW_ME:
		{
			master_ptr->pet_follow_distance = PET_FOLLOW_DIST;
			pet_t_m_idx = 0;
			break;
		}
		/* "Seek and destoy" */
	case PET_SEEK_AND_DESTROY:
		{
			master_ptr->pet_follow_distance = PET_DESTROY_DIST;
			break;
		}
		/* "Give me space" */
	case PET_ALLOW_SPACE:
		{
			master_ptr->pet_follow_distance = PET_SPACE_DIST;
			break;
		}
		/* "Stay away" */
	case PET_STAY_AWAY:
		{
			master_ptr->pet_follow_distance = PET_AWAY_DIST;
			break;
		}
		/* flag - allow pets to open doors */
	case PET_OPEN_DOORS:
		{
			if(master_ptr->pet_extra_flags & PF_OPEN_DOORS) master_ptr->pet_extra_flags &= ~(PF_OPEN_DOORS);
			else master_ptr->pet_extra_flags |= (PF_OPEN_DOORS);
			break;
		}
		/* flag - allow pets to pickup items */
	case PET_TAKE_ITEMS:
		{
			if(master_ptr->pet_extra_flags & PF_PICKUP_ITEMS)
			{
				master_ptr->pet_extra_flags &= ~(PF_PICKUP_ITEMS);
				for (pet_ctr = creature_max - 1; pet_ctr >= 1; pet_ctr--)
				{
					/* Access the creature */
					pet_ptr = &creature_list[pet_ctr];
					if(is_pet(master_ptr, pet_ptr)) creature_drop_carried_objects(pet_ptr);
				}
			}
			else master_ptr->pet_extra_flags |= (PF_PICKUP_ITEMS);

			break;
		}
		/* flag - allow pets to teleport */
	case PET_TELEPORT:
		if(master_ptr->pet_extra_flags & PF_TELEPORT) master_ptr->pet_extra_flags &= ~(PF_TELEPORT);
		else master_ptr->pet_extra_flags |= (PF_TELEPORT);
		break;
		/* flag - allow pets to cast attack spell */
	case PET_ATTACK_SPELL:
		if(master_ptr->pet_extra_flags & PF_ATTACK_SPELL) master_ptr->pet_extra_flags &= ~(PF_ATTACK_SPELL);
		else master_ptr->pet_extra_flags |= (PF_ATTACK_SPELL);
		break;
		/* flag - allow pets to cast attack spell */
	case PET_TRAIT_S_SPELL:
		if(master_ptr->pet_extra_flags & PF_TRAIT_S_SPELL) master_ptr->pet_extra_flags &= ~(PF_TRAIT_S_SPELL);
		else master_ptr->pet_extra_flags |= (PF_TRAIT_S_SPELL);
		break;
		/* flag - allow pets to cast attack spell */
	case PET_BALL_SPELL:
		if(master_ptr->pet_extra_flags & PF_BALL_SPELL) master_ptr->pet_extra_flags &= ~(PF_BALL_SPELL);
		else master_ptr->pet_extra_flags |= (PF_BALL_SPELL);
		break;
	case PET_RIDING:
		(void)do_riding(master_ptr, FALSE);
		break;
	case PET_NAME:
		do_name_pet(master_ptr);
		break;
	case PET_RYOUTE:
		if(master_ptr->pet_extra_flags & PF_RYOUTE) master_ptr->pet_extra_flags &= ~(PF_RYOUTE);
		else master_ptr->pet_extra_flags |= (PF_RYOUTE);
		prepare_update(master_ptr, CRU_BONUS);
		handle_stuff(master_ptr);
		break;
	}
}
