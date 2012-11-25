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

cptr spell_category_name(int tval)
{
	switch (tval)
	{
#ifdef JP
	case TV_HISSATSU_BOOK:
		return "必殺技";
	case TV_LIFE_BOOK:
		return "祈り";
	case TV_MUSIC_BOOK:
		return "歌";
	default:
		return "呪文";
#else
	case TV_HISSATSU_BOOK:
		return "art";
	case TV_LIFE_BOOK:
		return "prayer";
	case TV_MUSIC_BOOK:
		return "song";
	default:
		return "spell";
#endif
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
static int get_spell(creature_type *creature_ptr, int *sn, cptr prompt, int sval, bool learned, int use_realm)
{
	int         i;
	int         spell = -1;
	int         num = 0;
	int         ask = TRUE;
	int         need_mana;
	byte        spells[64];
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
			/* Success */
			return TRUE;
		}
	}

	p = spell_category_name(magic_info[creature_ptr->class_idx].spell_book);

	/* Extract spells */
	for (spell = 0; spell < 32; spell++)
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
	play_window |= (PW_SPELL);

	/* Window stuff */
	window_stuff(player_ptr);

	/* Build a prompt (accept all spells) */
#ifdef JP
	jverb1( prompt, jverb_buf );
	(void) strnfmt(out_val, 78, "(%^s:%c-%c, '*'で一覧, ESCで中断) どの%sを%^sますか? ",
		p, I2A(0), I2A(num - 1), p, jverb_buf );
#else
	(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) %^s which %s? ",
		p, I2A(0), I2A(num - 1), prompt, p);
#endif

	/* Get a spell from the user */

	choice = (always_show_list || use_menu) ? ESCAPE : 1;
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
				/* Show the list */
				if(!redraw)
				{
					/* Show list */
					redraw = TRUE;

					/* Save the screen */
					screen_save();

					/* Display a list of spells */
					print_spells(creature_ptr, menu_line, spells, num, 1, 15, use_realm);
				}

				/* Hide the list */
				else
				{
					if(use_menu) continue;

					/* Hide list */
					redraw = FALSE;

					/* Restore the screen */
					screen_load();
				}

				/* Redo asking */
				continue;
			}


			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if(ask) choice = tolower(choice);

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
#ifdef JP
			msg_format("その%sを%sことはできません。", p, prompt);
#else
			msg_format("You may not %s that %s.", prompt, p);
#endif

			continue;
		}

		/* Verify it */
		if(ask)
		{
			char tmp_val[160];

			/* Access the spell */
			if(!is_magic(use_realm))
			{
				s_ptr = &technic_info[use_realm - MIN_TECHNIC][spell];
			}
			else
			{
				s_ptr = &magic_info[creature_ptr->class_idx].info[use_realm][spell];
			}

			/* Extract mana consumption rate */
			if(use_realm == REALM_HISSATSU)
			{
				need_mana = s_ptr->smana;
			}
			else
			{
				need_mana = mod_need_mana(creature_ptr, s_ptr->smana, spell, use_realm);
			}

			/* Prompt */
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

		/* Stop the loop */
		flag = TRUE;
	}


	/* Restore the screen */
	if(redraw) screen_load();


	/* Show choices */
	play_window |= (PW_SPELL);

	/* Window stuff */
	window_stuff(player_ptr);


	/* Abort if needed */
	if(!flag) return FALSE;

	/* Save the choice */
	(*sn) = spell;

	repeat_push(*sn);

	/* Success */
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
	int  item;
	char which;

	/* Get the item index */
	if(repeat_pull(&item) && (item == INVEN_FORCE))
	{
		browse_only ? do_cmd_mind_browse(creature_ptr) : do_cmd_mind(creature_ptr);
		return;
	}

	/* Show the prompt */
#ifdef JP
	prt("('w'練気術, ESC) 'w'かESCを押してください。 ", 0, 0);
#else
	prt("(w for the Force, ESC) Hit 'w' or ESC. ", 0, 0);
#endif

	while (1)
	{
		/* Get a key */
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
	int		item, sval, use_realm = 0, j, line;
	int		spell = -1;
	int		num = 0;
	int		select_flag = 0;

	byte	spells[64];
	char	temp[62*4];

	int item_tester_tval;

	object_type	*object_ptr;

	cptr q, s;

	/* Warriors are illiterate */
	if(!(creature_ptr->realm1 || creature_ptr->realm2) && (creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_RED_MAGE))
	{
#ifdef JP
		msg_print("本を読むことができない！");
#else
		msg_print("You cannot read books!");
#endif

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

#ifdef JP
	q = "どの本を読みますか? ";
	s = "読める本がない。";
#else
	q = "Browse which book? ";
	s = "You have no books that you can read.";
#endif

	// Restrict choices to "useful" books
	if(creature_ptr->realm2 == REALM_NONE) item_tester_tval = magic_info[creature_ptr->class_idx].spell_book;

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR | USE_FORCE), item_tester_hook_readable, 0))
	{
		return;
	}

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

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Extract spells */
	for (spell = 0; spell < 32; spell++)
	{
		/* Check for this spell */
		if((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}


	/* Save the screen */
	screen_save();

	/* Clear the top line */
	prt("", 0, 0);

	/* Keep browsing spells.  Exit browsing on cancel. */
	while(TRUE)
	{
		/* Ask for a spell, allow cancel */
#ifdef JP
		if(!get_spell(creature_ptr, &spell, "読む", object_ptr->sval, TRUE, use_realm))
#else
		if(!get_spell(creature_ptr, &spell, "browse", object_ptr->sval, TRUE, use_realm))
#endif
		{
			/* If cancelled, leave immediately. */
			if(spell == -1) break;

			/* Display a list of spells */
			print_spells(creature_ptr, 0, spells, num, 1, 15, use_realm);

			/* Notify that there's nothing to see, and wait. */
			if(use_realm == REALM_HISSATSU)
#ifdef JP
				prt("読める技がない。", 0, 0);
#else
				prt("No techniques to browse.", 0, 0);
#endif
			else
#ifdef JP
				prt("読める呪文がない。", 0, 0);
#else
				prt("No spells to browse.", 0, 0);
#endif
			(void)inkey();


			/* Restore the screen */
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

	/* Restore the screen */
	screen_load();
}


static void change_realm2(creature_type *creature_ptr, int next_realm)
{
	int i, j = 0;
	char tmp[80];

	for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++)
	{
		creature_ptr->spell_order[j] = creature_ptr->spell_order[i];
		if(creature_ptr->spell_order[i] < 32) j++;
	}
	for (; j < (REALM_MAGIC_NUMBER * 2); j++)
		creature_ptr->spell_order[j] = 99;

	for (i = 32; i < (REALM_MAGIC_NUMBER * 2); i++)
	{
		creature_ptr->spell_exp[i] = SPELL_EXP_UNSKILLED;
	}
	creature_ptr->spell_learned2 = 0L;
	creature_ptr->spell_worked2 = 0L;
	creature_ptr->spell_forgotten2 = 0L;

#ifdef JP
	sprintf(tmp,"魔法の領域を%sから%sに変更した。", realm_names[creature_ptr->realm2], realm_names[next_realm]);
#else
	sprintf(tmp,"change magic realm from %s to %s.", realm_names[creature_ptr->realm2], realm_names[next_realm]);
#endif
	do_cmd_write_nikki(DIARY_BUNSHOU, 0, tmp);
	creature_ptr->old_realm |= 1 << (creature_ptr->realm2-1);
	creature_ptr->realm2 = next_realm;

	creature_ptr->creature_update |= (CRU_REORDER);
	creature_ptr->creature_update |= (CRU_SPELLS);
	handle_stuff();

	/* Load an autopick preference file */
	autopick_load_pref(FALSE);
}


/*
* Study a book to gain a new spell/prayer
*/
void do_cmd_study(creature_type *creature_ptr)
{
	int	i, item, sval;
	int	increment = 0;
	bool    learned = FALSE;
	int item_tester_tval;

	/* Spells of realm2 will have an increment of +32 */
	int	spell = -1;

	cptr p = spell_category_name(magic_info[creature_ptr->class_idx].spell_book);

	object_type *object_ptr;

	cptr q, s;


	if(!creature_ptr->realm1)
	{
#ifdef JP
		msg_print("本を読むことができない！");
#else
		msg_print("You cannot read books!");
#endif
		return;
	}

	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr))
	{
		msg_print(game_messages[GAME_MESSAGE_IS_BLIND]);
		return;
	}

	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
#ifdef JP
		msg_print("混乱していて読めない！");
#else
		msg_print("You are too confused!");
#endif
		return;
	}

	if(!(creature_ptr->new_spells))
	{
#ifdef JP
		msg_format("新しい%sを覚えることはできない！", p);
#else
		msg_format("You cannot learn any new %ss!", p);
#endif
		return;
	}

	free_posture(creature_ptr);

#ifdef JP
	if( creature_ptr->new_spells < 10 ){
		msg_format("あと %d つの%sを学べる。", creature_ptr->new_spells, p);
	}else{
		msg_format("あと %d 個の%sを学べる。", creature_ptr->new_spells, p);
	}
#else
	msg_format("You can learn %d new %s%s.", creature_ptr->new_spells, p,
		(creature_ptr->new_spells == 1?"":"s"));
#endif

	msg_print(NULL);


	/* Restrict choices to "useful" books */
	if(creature_ptr->realm2 == REALM_NONE) item_tester_tval = magic_info[creature_ptr->class_idx].spell_book;

#ifdef JP
	q = "どの本から学びますか? ";
	s = "読める本がない。";
#else
	q = "Study which book? ";
	s = "You have no books that you can read.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_INVEN | USE_FLOOR), item_tester_learn_trait, item_tester_tval)) return;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* Access the item's sval */
	sval = object_ptr->sval;

	if(object_ptr->tval == REALM2_BOOK(creature_ptr)) increment = 32;
	else if(object_ptr->tval != REALM1_BOOK(creature_ptr))
	{
#ifdef JP
		if(!get_check("本当に魔法の領域を変更しますか？")) return;
#else
		if(!get_check("Really, change magic realm? ")) return;
#endif
		change_realm2(creature_ptr, tval2realm(object_ptr->tval));
		increment = 32;
	}

	/* Track the object kind */
	object_kind_track(object_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Mage -- Learn a selected spell */
	if(magic_info[creature_ptr->class_idx].spell_book != TV_LIFE_BOOK)
	{
		/* Ask for a spell, allow cancel */
#ifdef JP
		if(!get_spell(creature_ptr, &spell, "学ぶ", sval, FALSE, object_ptr->tval - TV_LIFE_BOOK + 1) && (spell == -1)) return;
#else
		if(!get_spell(creature_ptr, &spell, "study", sval, FALSE, object_ptr->tval - TV_LIFE_BOOK + 1) && (spell == -1)) return;
#endif
	}

	/* Priest -- Learn a random prayer */
	else
	{
		int k = 0;
		int gift = -1;

		/* Extract spells */
		for (spell = 0; spell < 32; spell++)
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
#ifdef JP
		msg_format("その本には学ぶべき%sがない。", p);
#else
		msg_format("You cannot learn any %ss in that book.", p);
#endif
		return;
	}


	if(increment) spell += increment;

	/* Learn the spell */
	if(spell < 32)
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
		int max_exp = (spell < 32) ? SPELL_EXP_MASTER : SPELL_EXP_EXPERT;
		int old_exp = creature_ptr->spell_exp[spell];
		int new_rank = EXP_LEVEL_UNSKILLED;
		cptr name = do_spell(creature_ptr, increment ? creature_ptr->realm2 : creature_ptr->realm1, spell%32, SPELL_NAME);

		if(old_exp >= max_exp)
		{
#ifdef JP
			msg_format("その%sは完全に使いこなせるので学ぶ必要はない。", p);
#else
			msg_format("You don't need to study this %s anymore.", p);
#endif
			return;
		}
#ifdef JP
		if(!get_check(format("%sの%sをさらに学びます。よろしいですか？", name, p)))
#else
		if(!get_check(format("You will study a %s of %s again. Are you sure? ", p, name)))
#endif
		{
			return;
		}
		else if(old_exp >= SPELL_EXP_EXPERT)
		{
			creature_ptr->spell_exp[spell] = SPELL_EXP_MASTER;
			new_rank = EXP_LEVEL_MASTER;
		}
		else if(old_exp >= SPELL_EXP_SKILLED)
		{
			if(spell >= 32) creature_ptr->spell_exp[spell] = SPELL_EXP_EXPERT;
			else creature_ptr->spell_exp[spell] += SPELL_EXP_EXPERT - SPELL_EXP_SKILLED;
			new_rank = EXP_LEVEL_EXPERT;
		}
		else if(old_exp >= SPELL_EXP_BEGINNER)
		{
			creature_ptr->spell_exp[spell] = SPELL_EXP_SKILLED + (old_exp - SPELL_EXP_BEGINNER) * 2 / 3;
			new_rank = EXP_LEVEL_SKILLED;
		}
		else
		{
			creature_ptr->spell_exp[spell] = SPELL_EXP_BEGINNER + old_exp / 3;
			new_rank = EXP_LEVEL_BEGINNER;
		}
#ifdef JP
		msg_format("%sの熟練度が%sに上がった。", name, exp_level_str[new_rank]);
#else
		msg_format("Your proficiency of %s is now %s rank.", name, exp_level_str[new_rank]);
#endif
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

	/* Sound */
	sound(SOUND_STUDY);

	/* One less spell available */
	creature_ptr->learned_spells++;
#if 0
	/* Message if needed */
	if(creature_ptr->new_spells)
	{
		/* Message */
#ifdef JP
		if(creature_ptr->new_spells < 10) msg_format("あと %d つの%sを学べる。", creature_ptr->new_spells, p);
		else msg_format("あと %d 個の%sを学べる。", creature_ptr->new_spells, p);
#else
		msg_format("You can learn %d more %s%s.", creature_ptr->new_spells, p,
			(creature_ptr->new_spells != 1) ? "s" : "");
#endif
	}
#endif

	/* Update Study */
	creature_ptr->creature_update |= (CRU_SPELLS);
	update_creature(creature_ptr, TRUE);

	/* Redraw object recall */
	play_window |= (PW_OBJECT);
}


// Cast a spell
void do_cmd_cast(creature_type *creature_ptr)
{
	int	item, sval, spell, realm;
	int	chance;
	int	increment = 0;
	int	use_realm;
	int	need_mana;
	int select_flag;
	int item_tester_tval;

	bool over_exerted = FALSE;

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cptr prayer;

	object_type	*object_ptr;

	magic_type	*s_ptr;

	cptr q, s;

	/* Require spell ability */
	if(!creature_ptr->realm1 && (creature_ptr->class_idx != CLASS_SORCERER) && (creature_ptr->class_idx != CLASS_RED_MAGE))
	{
#ifdef JP
		msg_print("呪文を唱えられない！");
#else
		msg_print("You cannot cast spells!");
#endif

		return;
	}

	/* Require lite */
	if(has_trait(creature_ptr, TRAIT_BLIND) || no_lite(creature_ptr))
	{
		if(creature_ptr->class_idx == CLASS_FORCETRAINER) confirm_use_force(creature_ptr, FALSE);
		else
		{
			msg_print(game_messages[GAME_MESSAGE_IS_BLIND]);
			flush();
		}
		return;
	}

	/* Not when confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
#ifdef JP
		msg_print("混乱していて唱えられない！");
#else
		msg_print("You are too confused!");
#endif
		flush();
		return;
	}

	/* Hex */
	if(creature_ptr->realm1 == REALM_HEX)
	{
		if(hex_spell_fully(creature_ptr))
		{
			bool flag = FALSE;
#ifdef JP
			msg_print("これ以上新しい呪文を詠唱することはできない。");
#else
			msg_print("Can not spell new spells more.");
#endif
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

#ifdef JP
	q = "どの呪文書を使いますか? ";
	s = "呪文書がない！";
#else
	q = "Use which book? ";
	s = "You have no spell books!";
#endif

	if(!get_item(creature_ptr, &item, q, s, select_flag, NULL, item_tester_tval)) return;

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

	/* Hack -- Handle stuff */
	handle_stuff();

	if((creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE))
		realm = object_ptr->tval - TV_LIFE_BOOK + 1;
	else if(increment) realm = creature_ptr->realm2;
	else realm = creature_ptr->realm1;

	/* Ask for a spell */
#ifdef JP
	if(!get_spell(creature_ptr, &spell, 
		((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "詠唱する" : (magic_info[creature_ptr->class_idx].spell_book == TV_MUSIC_BOOK) ? "歌う" : "唱える"), 
		sval, TRUE, realm))
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

	/* Hex */
	if(use_realm == REALM_HEX)
	{
		if(HEX_SPELLING(creature_ptr, spell))
		{
#ifdef JP
			msg_print("その呪文はすでに詠唱中だ。");
#else
			msg_print("You are already casting it.");
#endif
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

		/* Warning */
#ifdef JP
		msg_format("その%sを%sのに十分なマジックポイントがない。",prayer,
			((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "詠唱する" : (magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "歌う" : "唱える"));
#else
		msg_format("You do not have enough mana to %s this %s.",
			((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
			prayer);
#endif


		if(!over_exert) return;

		/* Verify */
#ifdef JP
		if(!get_check_strict("それでも挑戦しますか? ", CHECK_OKAY_CANCEL)) return;
#else
		if(!get_check_strict("Attempt it anyway? ", CHECK_OKAY_CANCEL)) return;
#endif

	}

	/* Spell failure chance */
	chance = spell_chance(creature_ptr, spell, use_realm);

	/* Sufficient mana */
	if(need_mana <= creature_ptr->csp)
	{
		/* Use some mana */
		creature_ptr->csp -= need_mana;
	}
	else
	{
		over_exerted = TRUE;
	}

	/* Failed spell */
	if(randint0(100) < chance)
	{
		if(flush_failure) flush();

#ifdef JP
		msg_format("%sをうまく唱えられなかった！", prayer);
#else
		msg_format("You failed to get the %s off!", prayer);
#endif

		sound(SOUND_FAIL);

		/* Failure casting may activate some side effect */
		do_spell(creature_ptr, realm, spell, SPELL_FAIL);


		if((object_ptr->tval == TV_CHAOS_BOOK) && (randint1(100) < spell))
		{
#ifdef JP
			msg_print("カオス的な効果を発生した！");
#else
			msg_print("You produce a chaotic effect!");
#endif

			wild_magic(creature_ptr, spell);
		}
		else if((object_ptr->tval == TV_DEATH_BOOK) && (randint1(100) < spell))
		{
			if((sval == 3) && one_in_(2))
			{
				sanity_blast(creature_ptr, 0, TRUE);
			}
			else
			{
#ifdef JP
				msg_print("痛い！");
#else
				msg_print("It hurts!");
#endif

#ifdef JP
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(object_ptr->sval + 1, 6), "暗黒魔法の逆流", NULL, -1);
#else
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, diceroll(object_ptr->sval + 1, 6), "a miscast Death spell", NULL, -1);
#endif

				if((spell > 15) && one_in_(6) && !has_trait(creature_ptr, TRAIT_HOLD_LIFE))
					lose_exp(creature_ptr, spell * 250);
			}
		}
		else if((object_ptr->tval == TV_MUSIC_BOOK) && (randint1(200) < spell))
		{
#ifdef JP
			msg_print("いやな音が響いた");
#else
			msg_print("An infernal sound echoed.");
#endif

			aggravate_creatures(creature_ptr);
		}
	}

	/* Process spell */
	else
	{
		/* Canceled spells cost neither a turn nor mana */
		if(!do_spell(creature_ptr, realm, spell, SPELL_CAST)) return;

		/* A spell was cast */
		if(!(increment ?
			(creature_ptr->spell_worked2 & (1L << spell)) :
		(creature_ptr->spell_worked1 & (1L << spell)))
			&& (creature_ptr->class_idx != CLASS_SORCERER)
			&& (creature_ptr->class_idx != CLASS_RED_MAGE))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if(realm == creature_ptr->realm1)
			{
				creature_ptr->spell_worked1 |= (1L << spell);
			}
			else
			{
				creature_ptr->spell_worked2 |= (1L << spell);
			}

			/* Gain experience */
			gain_exp(creature_ptr, e * s_ptr->slevel);

			/* Redraw object recall */
			play_window |= (PW_OBJECT);

		}
		if(magic_info[creature_ptr->class_idx].spell_xtra & MAGIC_GAIN_EXP)
		{
			s16b cur_exp = creature_ptr->spell_exp[(increment ? 32 : 0)+spell];
			s16b exp_gain = 0;

			if(cur_exp < SPELL_EXP_BEGINNER)
				exp_gain += 60;
			else if(cur_exp < SPELL_EXP_SKILLED)
			{
				if((floor_ptr->floor_level > 4) && ((floor_ptr->floor_level + 10) > creature_ptr->lev))
					exp_gain = 8;
			}
			else if(cur_exp < SPELL_EXP_EXPERT)
			{
				if(((floor_ptr->floor_level + 5) > creature_ptr->lev) && ((floor_ptr->floor_level + 5) > s_ptr->slevel))
					exp_gain = 2;
			}
			else if((cur_exp < SPELL_EXP_MASTER) && !increment)
			{
				if(((floor_ptr->floor_level + 5) > creature_ptr->lev) && (floor_ptr->floor_level > s_ptr->slevel))
					exp_gain = 1;
			}
			creature_ptr->spell_exp[(increment ? 32 : 0) + spell] += exp_gain;
		}
	}

	cost_tactical_energy(creature_ptr, 100);

	/* Over-exert the player */
	if(over_exerted)
	{
		int oops = need_mana;
		dec_mana(creature_ptr, need_mana);

#ifdef JP
		msg_print("精神を集中しすぎて気を失ってしまった！");
#else
		msg_print("You faint from the effort!");
#endif
		(void)add_timed_trait(creature_ptr, TRAIT_SLEPT, randint1(5 * oops + 1), FALSE);

		/* Damage CON (possibly permanently) */
		if(randint0(100) < 50)
		{
			bool perm = (randint0(100) < 25);

			/* Message */
#ifdef JP
			msg_print("体を悪くしてしまった！");
#else
			msg_print("You have damaged your health!");
#endif
			/* Reduce constitution */
			(void)dec_stat(creature_ptr, STAT_CON, 15 + randint1(10), perm);
		}
	}

	/* Redraw mana */
	play_redraw |= (PR_MANA);

	/* Window stuff */
	play_window |= (PW_PLAYER);
	play_window |= (PW_SPELL);
}


static bool ang_sort_comp_pet_dismiss(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	creature_type *m_ptr1 = &creature_list[w1];
	creature_type *m_ptr2 = &creature_list[w2];
	species_type *r_ptr1 = &species_info[m_ptr1->species_idx];
	species_type *r_ptr2 = &species_info[m_ptr2->species_idx];

	/* Unused */
	(void)v;

	//TODO player's steed
	if(m_ptr2->ridden) return TRUE;
	if(m_ptr1->ridden) return FALSE;

	if(m_ptr1->nickname && !m_ptr2->nickname) return TRUE;
	if(m_ptr2->nickname && !m_ptr1->nickname) return FALSE;

	if(!m_ptr1->parent_m_idx && m_ptr2->parent_m_idx) return TRUE;
	if(!m_ptr2->parent_m_idx && m_ptr1->parent_m_idx) return FALSE;

	if(has_trait_species(r_ptr1, TRAIT_UNIQUE) && !has_trait_species(r_ptr2, TRAIT_UNIQUE)) return TRUE;
	if(has_trait_species(r_ptr2, TRAIT_UNIQUE) && !has_trait_species(r_ptr1, TRAIT_UNIQUE)) return FALSE;

	if(r_ptr1->level > r_ptr2->level) return TRUE;
	if(r_ptr2->level > r_ptr1->level) return FALSE;

	if(m_ptr1->chp > m_ptr2->chp) return TRUE;
	if(m_ptr2->chp > m_ptr1->chp) return FALSE;

	return w1 <= w2;
}

void check_pets_num_and_align(creature_type *master_ptr, creature_type *m_ptr, bool inc)
{
	s32b old_friend_align = friend_align;
	species_type *r_ptr = &species_info[m_ptr->species_idx];

	if(inc)
	{
		master_ptr->total_friends++;
		if(is_enemy_of_evil_creature(m_ptr)) friend_align += r_ptr->level;
		if(is_enemy_of_good_creature(m_ptr)) friend_align -= r_ptr->level;
	}
	else
	{
		master_ptr->total_friends--;
		if(is_enemy_of_evil_creature(m_ptr)) friend_align -= r_ptr->level;
		if(is_enemy_of_good_creature(m_ptr)) friend_align += r_ptr->level;
	}

	if(old_friend_align != friend_align) player_ptr->creature_update |= (CRU_BONUS);
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
	if(old_friend_align != friend_align) master_ptr->creature_update |= (CRU_BONUS);
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
	int pet_ctr, i;
	int Dismissed = 0;

	u16b *who;
	u16b dummy_why;
	int max_pet = 0;
	int cu, cv;

	cu = Term->scr->cu;
	cv = Term->scr->cv;
	Term->scr->cu = 0;
	Term->scr->cv = 1;

	/* Allocate the "who" array */
	C_MAKE(who, max_creature_idx, u16b);

	/* Process the creatures (backwards) */
	for (pet_ctr = creature_max - 1; pet_ctr >= 1; pet_ctr--)
	{
		if(is_pet(player_ptr, &creature_list[pet_ctr]))
			who[max_pet++] = pet_ctr;
	}

	ang_sort(who, &dummy_why, max_pet, ang_sort_comp_pet_dismiss, ang_sort_swap_hook);

	/* Process the creatures (backwards) */
	for (i = 0; i < max_pet; i++)
	{
		bool delete_this;
		char friend_name[80];
		char buf[80];
		bool kakunin;

		/* Access the creature */
		pet_ctr = who[i];
		m_ptr = &creature_list[pet_ctr];

		delete_this = FALSE;
		kakunin = ((pet_ctr == creature_ptr->riding) || (m_ptr->nickname));
		creature_desc(friend_name, m_ptr, CD_ASSUME_VISIBLE);

		if(!all_pets)
		{
			/* Hack -- health bar for this creature */
			health_track(pet_ctr);

			/* Hack -- handle stuff */
			handle_stuff();

#ifdef JP
			sprintf(buf, "%sを放しますか？ [Yes/No/Unnamed (%d体)]", friend_name, max_pet - i);
#else
			sprintf(buf, "Dismiss %s? [Yes/No/Unnamed (%d remain)]", friend_name, max_pet - i);
#endif
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
#ifdef JP
						sprintf(buf, "本当によろしいですか？ (%s) ", friend_name);
#else
						sprintf(buf, "Are you sure? (%s) ", friend_name);
#endif
						if(!get_check(buf))
							delete_this = FALSE;
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
				do_cmd_write_nikki(DIARY_NAMED_PET, RECORD_NAMED_PET_DISMISS, m_name);
			}

			if(pet_ctr == creature_ptr->riding)
			{
#ifdef JP
				msg_format("%sから降りた。", friend_name);
#else
				msg_format("You have got off %s. ", friend_name);
#endif

				creature_ptr->riding = 0;

				// Update creatures
				creature_ptr->creature_update |= (CRU_BONUS);
				creature_ptr->creature_update |= PU_CREATURES;
				play_redraw |= (PR_EXTRA | PR_UHEALTH);
			}

			/* HACK : Add the line to message buffer */
#ifdef JP
			sprintf(buf, "%s を放した。", friend_name);
#else
			sprintf(buf, "Dismissed %s.", friend_name);
#endif
			message_add(buf);
			play_window |= (PW_MESSAGE);
			window_stuff(player_ptr);

			delete_species_idx(&creature_list[pet_ctr]);
			Dismissed++;
		}
	}

	Term->scr->cu = cu;
	Term->scr->cv = cv;
	Term_fresh();

	C_KILL(who, max_creature_idx, u16b);

#ifdef JP
	msg_format("%d 体のペットを放しました。", Dismissed);
#else
	msg_format("You have dismissed %d pet%s.", Dismissed,
		(Dismissed == 1 ? "" : "s"));
#endif
	if(Dismissed == 0 && all_pets)
#ifdef JP
		msg_print("'U'nnamed は、乗馬以外の名前のないペットだけを全て解放します。");
#else
		msg_print("'U'nnamed means all your pets except named pets and your mount.");
#endif
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

bool do_thrown_from_riding(creature_type *creature_ptr, int dam, bool force)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i, y, x, oy, ox;
	int sn = 0, sy = 0, sx = 0;
	char m_name[MAX_NLEN];
	creature_type *m_ptr = &creature_list[creature_ptr->riding];
	species_type *r_ptr = &species_info[m_ptr->species_idx];
	bool fall_dam = FALSE;

	if(!creature_ptr->riding) return FALSE;
	if(floor_ptr->wild_mode) return FALSE;

	if(dam >= 0 || force)
	{
		if(!force)
		{
			int cur = creature_ptr->skill_exp[SKILL_RIDING];
			int max = skill_info[creature_ptr->class_idx].s_max[SKILL_RIDING];
			int ridinglevel = r_ptr->level;

			/* 落馬のしやすさ */
			int do_thrown_from_ridinglevel = r_ptr->level;
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
			{
				if((has_trait(creature_ptr, TRAIT_RODEO) && !creature_ptr->riding_two_handed) || !one_in_(creature_ptr->lev*(creature_ptr->riding_two_handed ? 2 : 3) + 30))
				{
					return FALSE;
				}
			}
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
			if(!cave_have_flag_grid(c_ptr, FF_MOVE) && !cave_have_flag_grid(c_ptr, FF_CAN_FLY))
			{
				if(!player_can_ride_aux(creature_ptr, c_ptr, FALSE)) continue;
			}

			if(cave_have_flag_grid(c_ptr, FF_PATTERN)) continue;

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
#ifdef JP
			msg_format("%sから振り落とされそうになって、壁にぶつかった。",m_name);
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, r_ptr->level+3, "壁への衝突", NULL, -1);
#else
			msg_format("You have nearly fallen from %s, but bumped into wall.",m_name);
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, r_ptr->level+3, "bumping into wall", NULL, -1);
#endif
			return FALSE;
		}

		oy = creature_ptr->fy;
		ox = creature_ptr->fx;

		creature_ptr->fy = sy;
		creature_ptr->fx = sx;

		/* Redraw the old spot */
		lite_spot(floor_ptr, oy, ox);

		/* Redraw the new spot */
		lite_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);

		/* Check for new panel */
		verify_panel(creature_ptr);
	}

	creature_ptr->riding = 0;
	creature_ptr->pet_extra_flags &= ~(PF_RYOUTE);
	creature_ptr->riding_two_handed = creature_ptr->old_riding_two_handed = FALSE;

	creature_ptr->creature_update |= (CRU_BONUS);
	update_creature(creature_ptr, TRUE);

	/* Update stuff */
	creature_ptr->creature_update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE | PU_CREATURES);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);

	play_redraw |= (PR_EXTRA);

	/* Update health track of mount */
	play_redraw |= (PR_UHEALTH);

	if(has_trait(creature_ptr, TRAIT_CAN_FLY) && !force)
	{
		creature_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("%sから落ちたが、空中でうまく体勢を立て直して着地した。",m_name);
#else
		msg_format("You are thrown from %s, but make a good landing.",m_name);
#endif
	}
	else
	{
#ifdef JP
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, r_ptr->level+3, "落馬", NULL, -1);
#else
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, r_ptr->level+3, "Falling from riding", NULL, -1);
#endif
		fall_dam = TRUE;
	}

	/* Move the player */
	if(sy && !gameover)
		(void)move_creature(creature_ptr, NULL, creature_ptr->fy, creature_ptr->fx, MCE_DONT_PICKUP | MCE_DONT_SWAP_MON);

	return fall_dam;
}

bool do_riding(creature_type *rider_ptr, bool force)
{
	int x, y, dir = 0;
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
#ifdef JP
			msg_print("そちらには降りられません。");
#else
			msg_print("You cannot go to that direction.");
#endif
			return FALSE;
		}

		if(!pattern_seq(rider_ptr, rider_ptr->fy, rider_ptr->fx, y, x)) return FALSE;

		if(c_ptr->creature_idx)
		{
			cost_tactical_energy(rider_ptr, 100);
			msg_print(game_messages[GAME_MESSAGE_CREATURE_IN_THE_WAY]);
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
#ifdef JP
			msg_print("混乱していて乗れない！");
#else
			msg_print("You are too confused!");
#endif
			return FALSE;
		}

		steed_ptr = &creature_list[c_ptr->creature_idx];

		if(!c_ptr->creature_idx || !steed_ptr->see_others)
		{
#ifdef JP
			msg_print("その場所にはクリーチャーはいません。");
#else
			msg_print("Here is no creature.");
#endif

			return FALSE;
		}
		if(!is_pet(player_ptr, steed_ptr) && !force)
		{
#ifdef JP
			msg_print("そのクリーチャーはペットではありません。");
#else
			msg_print("That creature is not a pet.");
#endif

			return FALSE;
		}
		if(!has_trait(steed_ptr, TRAIT_RIDING))
		{
#ifdef JP
			msg_print("そのクリーチャーには乗れなさそうだ。");
#else
			msg_print("This creature doesn't seem suitable for riding.");
#endif

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
#ifdef JP
			msg_print("うまく乗れなかった。");
#else
			msg_print("You failed to ride.");
#endif
			cost_tactical_energy(rider_ptr, 100);
			return FALSE;
		}

		if(has_trait(steed_ptr, TRAIT_PARALYZED))
		{
			char steed_name[80];
			creature_desc(steed_name, steed_ptr, 0);
			(void)set_timed_trait(steed_ptr, TRAIT_PARALYZED, 0);
#ifdef JP
			msg_format("%sを起こした。", steed_name);
#else
			msg_format("You have waked %s up.", steed_name);
#endif
		}

		if(rider_ptr->action == ACTION_KAMAE) set_action(rider_ptr, ACTION_NONE);

		rider_ptr->riding = c_ptr->creature_idx;
		steed_ptr->ridden = (s16b)rider_ptr->creature_idx;

		/* Hack -- remove tracked creature */
		if(rider_ptr->riding == health_who) health_track(0);
	}

	cost_tactical_energy(rider_ptr, 100);

	/* Mega-Hack -- Forget the view and lite */
	rider_ptr->creature_update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update the creatures */
	rider_ptr->creature_update |= (CRU_BONUS);
	play_redraw |= (PR_MAP | PR_EXTRA | PR_UHEALTH); // Redraw map

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
			/* Message */
#ifdef JP
			msg_print("そのクリーチャーはペットではない。");
#else
			msg_format("This creature is not a pet.");
#endif
			return;
		}

		if(!has_trait_species(&species_info[pet_ptr->species_idx], TRAIT_UNIQUE))
		{
#ifdef JP
			msg_print("そのクリーチャーの名前は変えられない！");
#else
			msg_format("You cannot change name of this creature!");
#endif
			return;
		}
		creature_desc(m_name, pet_ptr, 0);

		/* Message */
#ifdef JP
		msg_format("%sに名前をつける。", m_name);
#else
		msg_format("Name %s.", m_name);
#endif

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
#ifdef JP
		if(get_string("名前: ", out_val, 15))
#else
		if(get_string("Name: ", out_val, 15))
#endif

		{
			if(out_val[0])
			{
				/* Save the inscription */
				pet_ptr->nickname = quark_add(out_val);
				if(record_named_pet)
				{
					char m_name[MAX_NLEN];

					creature_desc(m_name, pet_ptr, CD_INDEF_VISIBLE);
					do_cmd_write_nikki(DIARY_NAMED_PET, RECORD_NAMED_PET_NAME, m_name);
				}
			}
			else
			{
				if(record_named_pet && old_name)
				{
					char m_name[MAX_NLEN];

					creature_desc(m_name, pet_ptr, CD_INDEF_VISIBLE);
					do_cmd_write_nikki(DIARY_NAMED_PET, RECORD_NAMED_PET_UNNAME, m_name);
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
	int	i = 0;
	int	num, ask;
	int powers[36];
	cptr power_desc[36];
	bool flag, redraw;
	char choice;
	char out_val[160];
	int pet_ctr;
	creature_type *pet_ptr;

	int mode = 0;

	char buf[160];
	char target_buf[160];

	int menu_line = use_menu ? 1 : 0;

	num = 0;

#ifdef JP
	power_desc[num] = "ペットを放す";
#else
	power_desc[num] = "dismiss pets";
#endif

	powers[num++] = PET_DISMISS;

#ifdef JP
	sprintf(target_buf, "ペットのターゲットを指定 (現在：%s)",
		(pet_t_m_idx ? (has_trait(master_ptr, TRAIT_HALLUCINATION) ? "何か奇妙な物" : (species_name + species_info[creature_list[pet_t_m_idx].ap_species_idx].name)) : "指定なし"));
#else
	sprintf(target_buf, "specify a target of pet (now:%s)",
		(pet_t_m_idx ? (has_trait(master_ptr, TRAIT_HALLUCINATION) ? "something strange" : (species_name + species_info[creature_list[pet_t_m_idx].ap_species_idx].name)) : "nothing"));
#endif
	power_desc[num] = target_buf;

	powers[num++] = PET_TARGET;

#ifdef JP
	power_desc[num] = "近くにいろ";
#else
	power_desc[num] = "stay close";
#endif

	if(master_ptr->pet_follow_distance == PET_CLOSE_DIST) mode = num;
	powers[num++] = PET_STAY_CLOSE;

#ifdef JP
	power_desc[num] = "ついて来い";
#else
	power_desc[num] = "follow me";
#endif

	if(master_ptr->pet_follow_distance == PET_FOLLOW_DIST) mode = num;
	powers[num++] = PET_FOLLOW_ME;

#ifdef JP
	power_desc[num] = "敵を見つけて倒せ";
#else
	power_desc[num] = "seek and destroy";
#endif

	if(master_ptr->pet_follow_distance == PET_DESTROY_DIST) mode = num;
	powers[num++] = PET_SEEK_AND_DESTROY;

#ifdef JP
	power_desc[num] = "少し離れていろ";
#else
	power_desc[num] = "give me space";
#endif

	if(master_ptr->pet_follow_distance == PET_SPACE_DIST) mode = num;
	powers[num++] = PET_ALLOW_SPACE;

#ifdef JP
	power_desc[num] = "離れていろ";
#else
	power_desc[num] = "stay away";
#endif

	if(master_ptr->pet_follow_distance == PET_AWAY_DIST) mode = num;
	powers[num++] = PET_STAY_AWAY;

	if(master_ptr->pet_extra_flags & PF_OPEN_DOORS)
	{
#ifdef JP
		power_desc[num] = "ドアを開ける (現在:ON)";
#else
		power_desc[num] = "pets open doors (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "ドアを開ける (現在:OFF)";
#else
		power_desc[num] = "pets open doors (now Off)";
#endif
	}
	powers[num++] = PET_OPEN_DOORS;

	if(master_ptr->pet_extra_flags & PF_PICKUP_ITEMS)
	{
#ifdef JP
		power_desc[num] = "アイテムを拾う (現在:ON)";
#else
		power_desc[num] = "pets pick up items (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "アイテムを拾う (現在:OFF)";
#else
		power_desc[num] = "pets pick up items (now Off)";
#endif
	}
	powers[num++] = PET_TAKE_ITEMS;

	if(master_ptr->pet_extra_flags & PF_TELEPORT)
	{
#ifdef JP
		power_desc[num] = "テレポート系魔法を使う (現在:ON)";
#else
		power_desc[num] = "allow teleport (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "テレポート系魔法を使う (現在:OFF)";
#else
		power_desc[num] = "allow teleport (now Off)";
#endif
	}
	powers[num++] = PET_TELEPORT;

	if(master_ptr->pet_extra_flags & PF_ATTACK_SPELL)
	{
#ifdef JP
		power_desc[num] = "攻撃魔法を使う (現在:ON)";
#else
		power_desc[num] = "allow cast attack spell (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "攻撃魔法を使う (現在:OFF)";
#else
		power_desc[num] = "allow cast attack spell (now Off)";
#endif
	}
	powers[num++] = PET_ATTACK_SPELL;

	if(master_ptr->pet_extra_flags & PF_TRAIT_S_SPELL)
	{
#ifdef JP
		power_desc[num] = "召喚魔法を使う (現在:ON)";
#else
		power_desc[num] = "allow cast summon spell (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "召喚魔法を使う (現在:OFF)";
#else
		power_desc[num] = "allow cast summon spell (now Off)";
#endif
	}
	powers[num++] = PET_TRAIT_S_SPELL;

	if(master_ptr->pet_extra_flags & PF_BALL_SPELL)
	{
#ifdef JP
		power_desc[num] = "プレイヤーを巻き込む範囲魔法を使う (現在:ON)";
#else
		power_desc[num] = "allow involve player in area spell (now On)";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "プレイヤーを巻き込む範囲魔法を使う (現在:OFF)";
#else
		power_desc[num] = "allow involve player in area spell (now Off)";
#endif
	}
	powers[num++] = PET_BALL_SPELL;

	if(master_ptr->riding)
	{
#ifdef JP
		power_desc[num] = "ペットから降りる";
#else
		power_desc[num] = "get off a pet";
#endif
	}
	else
	{
#ifdef JP
		power_desc[num] = "ペットに乗る";
#else
		power_desc[num] = "ride a pet";
#endif
	}
	powers[num++] = PET_RIDING;

#ifdef JP
	power_desc[num] = "ペットに名前をつける";
#else
	power_desc[num] = "name pets";
#endif

	powers[num++] = PET_NAME;

	if(master_ptr->riding)
	{
		if((master_ptr->can_melee[0] && (empty_hands(master_ptr, FALSE) == EMPTY_HAND_LARM) &&
			object_allow_two_hands_wielding(master_ptr, get_equipped_slot_ptr(master_ptr, INVEN_SLOT_HAND, 0))) ||
			(master_ptr->can_melee[1] && (empty_hands(master_ptr, FALSE) == EMPTY_HAND_RARM) &&
			object_allow_two_hands_wielding(master_ptr, get_equipped_slot_ptr(master_ptr, INVEN_SLOT_HAND, 1))))
		{
			if(master_ptr->pet_extra_flags & PF_RYOUTE)
			{
#ifdef JP
				power_desc[num] = "武器を片手で持つ";
#else
				power_desc[num] = "use one hand to control a riding pet";
#endif
			}
			else
			{
#ifdef JP
				power_desc[num] = "武器を両手で持つ";
#else
				power_desc[num] = "use both hands for a weapon";
#endif
			}

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
					{
#ifdef JP
						power_desc[num] = "片手で格闘する";
#else
						power_desc[num] = "use one hand to control a riding pet";
#endif
					}
					else
					{
#ifdef JP
						power_desc[num] = "両手で格闘する";
#else
						power_desc[num] = "use both hands for melee";
#endif
					}

					powers[num++] = PET_RYOUTE;
				}
				else if((empty_hands(master_ptr, FALSE) != EMPTY_HAND_NONE) && !get_equipped_slot_num(master_ptr, INVEN_SLOT_HAND))
				{
					if(master_ptr->pet_extra_flags & PF_RYOUTE)
					{
#ifdef JP
						power_desc[num] = "格闘を行わない";
#else
						power_desc[num] = "use one hand to control a riding pet";
#endif
					}
					else
					{
#ifdef JP
						power_desc[num] = "格闘を行う";
#else
						power_desc[num] = "use one hand for melee";
#endif
					}

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

		if(use_menu)
		{
			screen_save(); // Save the screen
#ifdef JP
			strnfmt(out_val, 78, "(コマンド、ESC=終了) コマンドを選んでください:");
#else
			strnfmt(out_val, 78, "(Command, ESC=exit) Choose command from menu.");
#endif
		}
		else
		{
			strnfmt(out_val, 78,
#ifdef JP
				"(コマンド %c-%c、'*'=一覧、ESC=終了) コマンドを選んでください:",
#else
				"(Command %c-%c, *=List, ESC=exit) Select a command: ",
#endif
				I2A(0), I2A(num - 1));
		}

		choice = (always_show_list || use_menu) ? ESCAPE : 1;

		/* Get a command from the user */
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
				/* Show the list */
				if(!redraw || use_menu)
				{
					byte y = 1, x = 0;
					int ctr = 0;

					/* Show list */
					redraw = TRUE;

					/* Save the screen */
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

				/* Hide the list */
				else
				{
					/* Hide list */
					redraw = FALSE;

					/* Restore the screen */
					screen_load();
				}

				/* Redo asking */
				continue;
			}

			if(!use_menu)
			{
				/* Note verify */
				ask = (isupper(choice));

				/* Lowercase */
				if(ask) choice = tolower(choice);

				/* Extract request */
				i = (islower(choice) ? A2I(choice) : -1);
			}

			/* Totally Illegal */
			if((i < 0) || (i >= num))
			{
				bell();
				continue;
			}

			/* Verify it */
			if(ask)
			{
				/* Prompt */
#ifdef JP
				strnfmt(buf, 78, "%sを使いますか？ ", power_desc[i]);
#else
				strnfmt(buf, 78, "Use %s? ", power_desc[i]);
#endif

				/* Belay that order */
				if(!get_check(buf)) continue;
			}

			/* Stop the loop */
			flag = TRUE;
		}

		/* Restore the screen */
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
#ifdef JP
				msg_print("ペットがいない！");
#else
				msg_print("You have no pets!");
#endif
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
		{
			if(master_ptr->pet_extra_flags & PF_TELEPORT) master_ptr->pet_extra_flags &= ~(PF_TELEPORT);
			else master_ptr->pet_extra_flags |= (PF_TELEPORT);
			break;
		}
		/* flag - allow pets to cast attack spell */
	case PET_ATTACK_SPELL:
		{
			if(master_ptr->pet_extra_flags & PF_ATTACK_SPELL) master_ptr->pet_extra_flags &= ~(PF_ATTACK_SPELL);
			else master_ptr->pet_extra_flags |= (PF_ATTACK_SPELL);
			break;
		}
		/* flag - allow pets to cast attack spell */
	case PET_TRAIT_S_SPELL:
		{
			if(master_ptr->pet_extra_flags & PF_TRAIT_S_SPELL) master_ptr->pet_extra_flags &= ~(PF_TRAIT_S_SPELL);
			else master_ptr->pet_extra_flags |= (PF_TRAIT_S_SPELL);
			break;
		}
		/* flag - allow pets to cast attack spell */
	case PET_BALL_SPELL:
		{
			if(master_ptr->pet_extra_flags & PF_BALL_SPELL) master_ptr->pet_extra_flags &= ~(PF_BALL_SPELL);
			else master_ptr->pet_extra_flags |= (PF_BALL_SPELL);
			break;
		}

	case PET_RIDING:
		{
			(void)do_riding(master_ptr, FALSE);
			break;
		}

	case PET_NAME:
		{
			do_name_pet(master_ptr);
			break;
		}

	case PET_RYOUTE:
		{
			if(master_ptr->pet_extra_flags & PF_RYOUTE) master_ptr->pet_extra_flags &= ~(PF_RYOUTE);
			else master_ptr->pet_extra_flags |= (PF_RYOUTE);
			master_ptr->creature_update |= (CRU_BONUS);
			handle_stuff();
			break;
		}
	}
}
