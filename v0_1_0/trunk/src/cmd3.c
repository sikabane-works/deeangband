/* File: cmd3.c */

/* Purpose: inventory commands */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"



/*
 * Display inventory
 */
void do_cmd_inven(creature_type *cr_ptr)
{
	char out_val[160];


	/* Note that we are in "inventory" mode */
	command_wrk = FALSE;

#ifdef ALLOW_EASY_FLOOR

	/* Note that we are in "inventory" mode */
	if (easy_floor) command_wrk = (USE_INVEN);

#endif /* ALLOW_EASY_FLOOR */

	/* Save screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the inventory */
	(void)show_inven(0, cr_ptr);

	/* Hack -- hide empty slots */
	item_tester_full = FALSE;

#ifdef JP
	sprintf(out_val, "�������F ���v %3d.%1d kg (���E��%ld%%) �R�}���h: ",
	    lbtokg1(cr_ptr->total_weight) , lbtokg2(cr_ptr->total_weight) ,
	    (cr_ptr->total_weight * 100) / weight_limit(cr_ptr));
#else
	sprintf(out_val, "Inventory: carrying %d.%d pounds (%ld%% of capacity). Command: ",
	    (int)(cr_ptr->total_weight / 10), (int)(cr_ptr->total_weight % 10),
	    (cr_ptr->total_weight * 100) / weight_limit(cr_ptr));
#endif


	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Load screen */
	screen_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Hack -- Use "display" mode */
		command_see = TRUE;
	}
}


/*
 * Display equipment
 */
void do_cmd_equip(creature_type *cr_ptr)
{
	char out_val[160];


	/* Note that we are in "equipment" mode */
	command_wrk = TRUE;

#ifdef ALLOW_EASY_FLOOR

	/* Note that we are in "equipment" mode */
	if (easy_floor) command_wrk = (USE_EQUIP);

#endif /* ALLOW_EASY_FLOOR  */

	/* Save the screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the equipment */
	(void)show_equip(0, cr_ptr);

	/* Hack -- undo the hack above */
	item_tester_full = FALSE;

	/* Build a prompt */
#ifdef JP
	sprintf(out_val, "�����F ���v %3d.%1d kg (���E��%ld%%) �R�}���h: ",
	    lbtokg1(cr_ptr->total_weight) , lbtokg2(cr_ptr->total_weight) ,
	    (cr_ptr->total_weight * 100) / weight_limit(cr_ptr));
#else
	sprintf(out_val, "Equipment: carrying %d.%d pounds (%ld%% of capacity). Command: ",
	    (int)(cr_ptr->total_weight / 10), (int)(cr_ptr->total_weight % 10),
	    (cr_ptr->total_weight * 100) / weight_limit(cr_ptr));
#endif


	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Restore the screen */
	screen_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Enter "display" mode */
		command_see = TRUE;
	}
}


/*
 * The "wearable" tester
 */
static bool item_tester_hook_wear(creature_type *cr_ptr, object_type *o_ptr)
{
	if ((o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_ABUNAI_MIZUGI))
		if (cr_ptr->sex == SEX_MALE) return FALSE;

	/* Check for a usable slot */
	if (wield_slot(cr_ptr, o_ptr) >= INVEN_1STARM) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


static bool item_tester_hook_mochikae(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Check for a usable slot */
	if (((o_ptr->tval >= TV_DIGGING) && (o_ptr->tval <= TV_SWORD)) ||
	    (o_ptr->tval == TV_SHIELD) || (o_ptr->tval == TV_CAPTURE) ||
	    (o_ptr->tval == TV_CARD)) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


static bool item_tester_hook_melee_weapon(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Check for a usable slot */
	if ((o_ptr->tval >= TV_DIGGING) && (o_ptr->tval <= TV_SWORD))return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


bool select_ring_slot = FALSE;

/*
 * Wield or wear a single item from the pack or floor
 */
void do_cmd_wield(creature_type *cr_ptr)
{
	int i, item, slot, rate;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr act;

	char o_name[MAX_NLEN];

	cptr q, s;

	int need_switch_wielding = 0;

	if (cr_ptr->special_defense & KATA_MUSOU)
	{
		set_action(cr_ptr, ACTION_NONE);
	}

	/* Restrict the choices */
	item_tester_hook = item_tester_hook_wear;

	/* Get an item */
#ifdef JP
	q = "�ǂ�𑕔����܂���? ";
	s = "�����\�ȃA�C�e�����Ȃ��B";
#else
	q = "Wear/Wield which item? ";
	s = "You have nothing you can wear or wield.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Check the slot */
	slot = wield_slot(cr_ptr, o_ptr);

	switch (o_ptr->tval)
	{
	/* Shields and some misc. items */
	case TV_CAPTURE:
	case TV_SHIELD:
	case TV_CARD:
		/* Dual wielding */
		if (have_weapon(cr_ptr, INVEN_1STARM) && have_weapon(cr_ptr, INVEN_2NDARM))
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_melee_weapon;
			item_tester_no_ryoute = TRUE;

			/* Choose a weapon from the equipment only */
#ifdef JP
			q = "�ǂ���̕���Ǝ��ւ��܂���?";
			s = "�����ƁB";
#else
			q = "Replace which weapon? ";
			s = "Oops.";
#endif

			if (!get_item(cr_ptr, &slot, q, s, (USE_EQUIP))) return;
			if (slot == INVEN_1STARM) need_switch_wielding = INVEN_2NDARM;
		}

		else if (have_weapon(cr_ptr, INVEN_2NDARM)) slot = INVEN_1STARM;

		/* Both arms are already used by non-weapon */
		else if (cr_ptr->inventory[INVEN_1STARM].k_idx && !object_is_melee_weapon(cr_ptr, &cr_ptr->inventory[INVEN_1STARM]) &&
		         cr_ptr->inventory[INVEN_2NDARM].k_idx && !object_is_melee_weapon(cr_ptr, &cr_ptr->inventory[INVEN_2NDARM]))
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			/* Choose a hand */
#ifdef JP
			q = "�ǂ���̎�ɑ������܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif

			if (!get_item(cr_ptr, &slot, q, s, (USE_EQUIP))) return;
		}
		break;

	/* Melee weapons */
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_SWORD:
		/* Asking for dual wielding */
		if (slot == INVEN_2NDARM)
		{
#ifdef JP
			if (!get_check("�񓁗��Ő킢�܂����H")) slot = INVEN_1STARM;
#else
			if (!get_check("Dual wielding? ")) slot = INVEN_1STARM;
#endif
		}

		else if (!cr_ptr->inventory[INVEN_1STARM].k_idx && have_weapon(cr_ptr, INVEN_2NDARM))
		{
#ifdef JP
			if (!get_check("�񓁗��Ő킢�܂����H")) slot = INVEN_2NDARM;
#else
			if (!get_check("Dual wielding? ")) slot = INVEN_2NDARM;
#endif
		}

		/* Both arms are already used */
		else if (cr_ptr->inventory[INVEN_2NDARM].k_idx && cr_ptr->inventory[INVEN_1STARM].k_idx)
		{
			/* Restrict the choices */
			item_tester_hook = item_tester_hook_mochikae;

			/* Choose a hand */
#ifdef JP
			q = "�ǂ���̎�ɑ������܂���?";
			s = "�����ƁB";
#else
			q = "Equip which hand? ";
			s = "Oops.";
#endif

			if (!get_item(cr_ptr, &slot, q, s, (USE_EQUIP))) return;
			if ((slot == INVEN_2NDARM) && !have_weapon(cr_ptr, INVEN_1STARM))
				need_switch_wielding = INVEN_1STARM;
		}
		break;

	/* Rings */
	case TV_RING:
		/* Choose a ring slot */
		if (cr_ptr->inventory[INVEN_LEFT].k_idx && cr_ptr->inventory[INVEN_RIGHT].k_idx)
		{
#ifdef JP
			q = "�ǂ���̎w�ւƎ��ւ��܂���?";
#else
			q = "Replace which ring? ";
#endif
		}
		else
		{
#ifdef JP
			q = "�ǂ���̎�ɑ������܂���?";
#else
			q = "Equip which hand? ";
#endif
		}

#ifdef JP
		s = "�����ƁB";
#else
		s = "Oops.";
#endif

		/* Restrict the choices */
		select_ring_slot = TRUE;
		item_tester_no_ryoute = TRUE;

		if (!get_item(cr_ptr, &slot, q, s, (USE_EQUIP)))
		{
			select_ring_slot = FALSE;
			return;
		}
		select_ring_slot = FALSE;
		break;
	}


	/* Prevent wielding into a cursed slot */
	if (object_is_cursed(&cr_ptr->inventory[slot]))
	{
		/* Describe it */
		object_desc(o_name, &cr_ptr->inventory[slot], (OD_OMIT_PREFIX | OD_NAME_ONLY));

		/* Message */
#ifdef JP
		msg_format("%s%s�͎���Ă���悤���B",
			   describe_use(cr_ptr, slot) , o_name );
#else
		msg_format("The %s you are %s appears to be cursed.",
			   o_name, describe_use(cr_ptr, slot));
#endif

		/* Cancel the command */
		return;
	}

	if (confirm_wear &&
		((object_is_cursed(o_ptr) && object_is_known(o_ptr)) ||
		((o_ptr->ident & IDENT_SENSE) &&
			(FEEL_BROKEN <= o_ptr->feeling) && (o_ptr->feeling <= FEEL_CURSED))))
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "�{����%s{����Ă���}���g���܂����H", o_name);
#else
		sprintf(dummy, "Really use the %s {cursed}? ", o_name);
#endif

		if (!get_check(dummy)) return;
	}

	if ((o_ptr->name1 == ART_STONEMASK) && object_is_known(o_ptr) && (cr_ptr->race_idx1 != RACE_VAMPIRE) && (cr_ptr->race_idx1 != RACE_ANDROID))
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

#ifdef JP
sprintf(dummy, "%s�𑕔�����Ƌz���S�ɂȂ�܂��B��낵���ł����H", o_name);
#else
		msg_format("%s will transforms you into a vampire permanently when equiped.", o_name);
		sprintf(dummy, "Do you become a vampire?");
#endif

		if (!get_check(dummy)) return;
	}

	if (need_switch_wielding && !object_is_cursed(&cr_ptr->inventory[need_switch_wielding]))
	{
		object_type *slot_o_ptr = &cr_ptr->inventory[slot];
		object_type *switch_o_ptr = &cr_ptr->inventory[need_switch_wielding];
		object_type object_tmp;
		object_type *otmcr_ptr = &object_tmp;
		char switch_name[MAX_NLEN];

		object_desc(switch_name, switch_o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		object_copy(otmcr_ptr, switch_o_ptr);
		object_copy(switch_o_ptr, slot_o_ptr);
		object_copy(slot_o_ptr, otmcr_ptr);
#ifdef JP
		msg_format("%s��%s�ɍ\���Ȃ������B", switch_name, (slot == INVEN_1STARM) ? (left_hander ? "����" : "�E��") : (left_hander ? "�E��" : "����"));
#else
		msg_format("You wield %s at %s hand.", switch_name, (slot == INVEN_1STARM) ? (left_hander ? "left" : "right") : (left_hander ? "right" : "left"));
#endif

		slot = need_switch_wielding;
	}

	/* Check if completed a quest */
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
		    (quest[i].k_idx == o_ptr->name1))
		{
			if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
			quest[i].status = QUEST_STATUS_COMPLETED;
			quest[i].complev = (byte)cr_ptr->lev;
#ifdef JP
msg_print("�N�G�X�g��B�������I");
#else
			msg_print("You completed the quest!");
#endif

			msg_print(NULL);
		}
	}

	if (cr_ptr->chara_idx == CHARA_MUNCHKIN)
	{
		identify_item(cr_ptr, o_ptr);

		/* Auto-inscription */
		autopick_alter_item(cr_ptr, item, FALSE);
	}

	/* Armor Size Information */

	if(slot == INVEN_BODY || slot == INVEN_1STHEAD || slot == INVEN_1STHANDS || slot == INVEN_FEET || slot == INVEN_OUTER)
	{
		rate = set_inventory_fitting_rate(cr_ptr, o_ptr, slot);

		if(o_ptr->fitting_size != ARMOR_SIZE_FREE){
		#ifdef JP
			msg_format("�i�����̑̊i��F%d���j", rate);
		#else
			msg_format("(Size�F%d%%)", rate);
		#endif
		}
	}

	switch (slot)
	{
	case INVEN_BODY:
	case INVEN_1STHEAD:
	case INVEN_1STHANDS:
	case INVEN_FEET:
		if(o_ptr->fitting_size == ARMOR_SIZE_FREE)
		{
#ifdef JP
			msg_format("���������Ȃ��̑̊i�ɍ����悤�傫����ς����I");
#else
#endif
		}
		else if(rate >= 150)
		{
#ifdef JP
			msg_format("����ȑ傫��������g�ɒ����邱�Ƃ͂ł��Ȃ��B");
#else
#endif
			return;
		}
		else if(rate >= 130)
		{
#ifdef JP
			msg_format("�������傫�����ău�J�u�J����B�h��Ƃ��Ă̋@�\���܂�ŉʂ����Ă��Ȃ��B");
#else
#endif
		}
		else if(rate >= 120)
		{
#ifdef JP
			msg_format("���Ȃ��̐g�Ɣ�ׂĂ��Ȃ�傫���悤���B�h��Ƃ��Ă̋@�\���ʂ����Ă��Ȃ��B");
#else
#endif
		}
		else if(rate >= 110)
		{
#ifdef JP
			msg_format("���Ȃ��̐g�Ɣ�ׂđ傫���悤���B�h��Ƃ��Ă̋@�\�����܂�ʂ����Ă��Ȃ��B");
#else
#endif
		}
		else if(rate <= 75)
		{
#ifdef JP
			msg_format("����ȏ�����������g�ɒ����邱�Ƃ͂ł��Ȃ��B");
#else
#endif
			return;
		}
		else if(rate <= 80)
		{
#ifdef JP
			msg_format("���������������Ē��ߕt������悤���I�ƂĂ��ꂵ���I");
#else
#endif
		}
		else if(rate <= 90)
		{
#ifdef JP
			msg_format("���Ȃ��̐g�Ɣ�ׂĂ��Ȃ菬�����悤���B���Ȃ蓮���Â炢�B");
#else
#endif
		}
		else if(rate <= 95)
		{
#ifdef JP
			msg_format("���Ȃ��̐g�Ɣ�ׂď������悤���B���������Â炢�B");
#else
#endif
		}
		else
		{
#ifdef JP
			msg_format("���Ȃ��ɂ͂��傤�Ǘǂ��傫�����B�h��Ƃ��Ă̋@�\�͏\���ɉʂ����Ă���B");
#else
#endif
		}

		break;

	case INVEN_OUTER:
		if(o_ptr->fitting_size == ARMOR_SIZE_FREE)
		{
#ifdef JP
			msg_format("���������Ȃ��̔w��ɍ����悤�����ƌ`���ς����I");
#else
#endif
		}
		else if(rate >= 180)
		{
#ifdef JP
			msg_format("��������I�܂Ƃ��Ă���̂����������Ă���̂�������Ȃ��L�l�ł܂Ƃ��ɓ����Ȃ��I");
#else
#endif
		}
		else if(rate >= 140)
		{
#ifdef JP
			msg_format("�������n�ʂɈ����������Ĕ��ɓ����Â炢�B");
#else
#endif
		}
		else if(rate >= 120)
		{
#ifdef JP
			msg_format("���̓����ɂ����肻���ȕ����܂ŐL�тĂ��ē����Â炢�B");
#else
#endif
		}
		else if(rate >= 110)
		{
#ifdef JP
			msg_format("���Ȃ��̔w��Ɣ�ׂď�������������Ɏx��͂Ȃ��������B");
#else
#endif
		}
		else if(rate <= 20)
		{
#ifdef JP
			msg_format("�ǂ����Ă���ȕz�؂�𑕔����悤�Ǝv�����̂��낤���H");
#else
#endif
			return;
		}
		else if(rate <= 40)
		{
#ifdef JP
			msg_format("���Ȃ��ɂƂ��Ă̓X�J�[�t�قǂ̑傫�����B�h��Ƃ��Ă܂�Ŗ𗧂��Ȃ��B");
#else
#endif
		}
		else if(rate <= 60)
		{
#ifdef JP
			msg_format("���Ȃ��ɂƂ��Ă͍������قǂ̑傫�����B�h��Ƃ��ĂقƂ�ǖ𗧂��Ȃ��B");
#else
#endif
		}
		else if(rate <= 80)
		{
#ifdef JP
			msg_format("�Z�����đ̂̔����ʂ͘I�o�����܂܂��B�h��Ƃ��Ă͗]��𗧂��Ȃ��B");
#else
#endif
		}
		else if(rate <= 90)
		{
#ifdef JP
			msg_format("���Ȃ��̔w��Ɣ�ׂĒZ�����g�����̂ɂ��قǎx��͂Ȃ��������B");
#else
#endif
		}
		else
		{
#ifdef JP
			msg_format("���Ȃ��ɂ҂�����̒������B�g�����₷�����������₷���B");
#else
#endif
		}

		break;
	default:
		break;
	}



	/* Take a turn */
	energy_use = 100;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain local object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = 1;

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Access the wield slot */
	o_ptr = &cr_ptr->inventory[slot];

	/* Take off existing item */
	if (o_ptr->k_idx)
	{
		/* Take off existing item */
		(void)inven_takeoff(cr_ptr, slot, 255);
	}

	/* Wear the new stuff */
	object_copy(o_ptr, q_ptr);

	/* Player touches it */
	o_ptr->marked |= OM_TOUCHED;

	/* Increase the weight */
	cr_ptr->total_weight += q_ptr->weight;

	/* Increment the equip counter by hand */
	cr_ptr->equip_cnt++;

#ifdef JP
#define STR_WIELD_RARM "%s(%c)���E��ɑ��������B"
#define STR_WIELD_LARM "%s(%c)������ɑ��������B"
#define STR_WIELD_ARMS "%s(%c)�𗼎�ō\�����B"
#else
#define STR_WIELD_RARM "You are wielding %s (%c) in your right hand."
#define STR_WIELD_LARM "You are wielding %s (%c) in your left hand."
#define STR_WIELD_ARMS "You are wielding %s (%c) with both hands."
#endif

	/* Where is the item now */
	switch (slot)
	{
	case INVEN_1STARM:
		if (object_allow_two_hands_wielding(cr_ptr, o_ptr) && (cr_ptr, empty_hands(cr_ptr, FALSE) == EMPTY_HAND_LARM) && CAN_TWO_HANDS_WIELDING(cr_ptr))
			act = STR_WIELD_ARMS;
		else
			act = (left_hander ? STR_WIELD_LARM : STR_WIELD_RARM);
		break;

	case INVEN_2NDARM:
		if (object_allow_two_hands_wielding(cr_ptr, o_ptr) && (cr_ptr, empty_hands(cr_ptr, FALSE) == EMPTY_HAND_RARM) && CAN_TWO_HANDS_WIELDING(cr_ptr))
			act = STR_WIELD_ARMS;
		else
			act = (left_hander ? STR_WIELD_RARM : STR_WIELD_LARM);
		break;

	case INVEN_BOW:
#ifdef JP
		act = "%s(%c)���ˌ��p�ɑ��������B";
#else
		act = "You are shooting with %s (%c).";
#endif
		break;

	case INVEN_LITE:
#ifdef JP
		act = "%s(%c)�������ɂ����B";
#else
		act = "Your light source is %s (%c).";
#endif
		break;

	default:
#ifdef JP
		act = "%s(%c)�𑕔������B";
#else
		act = "You are wearing %s (%c).";
#endif
		break;
	}

	/* Describe the result */
	object_desc(o_name, o_ptr, 0);

	/* Message */
	msg_format(act, o_name, index_to_label(cr_ptr, slot));


	/* Cursed! */
	if (object_is_cursed(o_ptr))
	{
		/* Warn the player */
#ifdef JP
		msg_print("����I �����܂����₽���I");
#else
		msg_print("Oops! It feels deathly cold!");
#endif

		/* Note the curse */
		o_ptr->ident |= (IDENT_SENSE);
	}

	/* The Stone Mask make the player turn into a vampire! */
	if ((o_ptr->name1 == ART_STONEMASK) && (cr_ptr->race_idx1 != RACE_VAMPIRE) && (cr_ptr->race_idx1 != RACE_ANDROID))
	{
		/* Turn into a vampire */
		change_race(cr_ptr, RACE_VAMPIRE, "");
	}

	/* Recalculate bonuses */
	cr_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	cr_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	cr_ptr->update |= (PU_MANA);

	play_redraw |= (PR_EQUIPPY);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp(cr_ptr);
}


void kamaenaoshi(creature_type *cr_ptr, int item)
{
	object_type *o_ptr, *new_o_ptr;
	char o_name[MAX_NLEN];

	if (item == INVEN_1STARM)
	{
		if (have_weapon(cr_ptr, INVEN_2NDARM))
		{
			o_ptr = &cr_ptr->inventory[INVEN_2NDARM];
			object_desc(o_name, o_ptr, 0);

			if (!object_is_cursed(o_ptr))
			{
				new_o_ptr = &cr_ptr->inventory[INVEN_1STARM];
				object_copy(new_o_ptr, o_ptr);
				cr_ptr->total_weight += o_ptr->weight;
				inven_item_increase(INVEN_2NDARM, -((int)o_ptr->number));
				inven_item_optimize(INVEN_2NDARM);
				if (object_allow_two_hands_wielding(cr_ptr, o_ptr) && CAN_TWO_HANDS_WIELDING(cr_ptr))
#ifdef JP
					msg_format("%s�𗼎�ō\�����B", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
				 else
#ifdef JP
					msg_format("%s��%s�ō\�����B", o_name, (left_hander ? "����" : "�E��"));
#else
					msg_format("You are wielding %s in your %s hand.", o_name, (left_hander ? "left":"right"));
#endif
			}
			else
			{
				if (object_allow_two_hands_wielding(cr_ptr, o_ptr) && CAN_TWO_HANDS_WIELDING(cr_ptr))
#ifdef JP
					msg_format("%s�𗼎�ō\�����B", o_name);
#else
					msg_format("You are wielding %s with both hands.", o_name);
#endif
			}
		}
	}
	else if (item == INVEN_2NDARM)
	{
		o_ptr = &cr_ptr->inventory[INVEN_1STARM];
		if (o_ptr->k_idx) object_desc(o_name, o_ptr, 0);

		if (have_weapon(cr_ptr, INVEN_1STARM))
		{
			if (object_allow_two_hands_wielding(cr_ptr, o_ptr) && CAN_TWO_HANDS_WIELDING(cr_ptr))
#ifdef JP
				msg_format("%s�𗼎�ō\�����B", o_name);
#else
				msg_format("You are wielding %s with both hands.", o_name);
#endif
		}
		else if (!(empty_hands(cr_ptr, FALSE) & EMPTY_HAND_RARM) && !object_is_cursed(o_ptr))
		{
			new_o_ptr = &cr_ptr->inventory[INVEN_2NDARM];
			object_copy(new_o_ptr, o_ptr);
			cr_ptr->total_weight += o_ptr->weight;
			inven_item_increase(INVEN_1STARM, -((int)o_ptr->number));
			inven_item_optimize(INVEN_1STARM);
#ifdef JP
			msg_format("%s�������ւ����B", o_name);
#else
			msg_format("You switched hand of %s.", o_name);
#endif
		}
	}
}


/*
 * Take off an item
 */
void do_cmd_takeoff(creature_type *cr_ptr)
{
	int item;

	object_type *o_ptr;

	cptr q, s;

	if (cr_ptr->special_defense & KATA_MUSOU)
	{
		set_action(cr_ptr, ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ�𑕔�����͂����܂���? ";
	s = "�͂����鑕�����Ȃ��B";
#else
	q = "Take off which item? ";
	s = "You are not wearing anything to take off.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Item is cursed */
	if (object_is_cursed(o_ptr))
	{
		if (o_ptr->curse_flags & TRC_DIVINE_CURSE)
		{
			if (o_ptr->xtra1 >= cr_ptr->dr)
			{
			/* Oops */
#ifdef JP
			msg_print("�Ȃ�Ă������I���Ȃ��͐_��̗͂Ɏ���Ă���I");
#else
			msg_print("What the hell! You are cursed by divine power!");
#endif
			return;
			}
		}
		else if (cr_ptr->cls_idx != CLASS_BERSERKER)
		{
			/* Oops */
#ifdef JP
			msg_print("�Ӂ[�ށA�ǂ�������Ă���悤���B");
#else
			msg_print("Hmmm, it seems to be cursed.");
#endif

			/* Nope */
			return;
		}

		if(o_ptr->curse_flags & TRC_DIVINE_CURSE && o_ptr->xtra1 < cr_ptr->dr)
		{
#ifdef JP
			msg_print("���Ȃ��̐_��̗͎͂􂢂𗽉킵�Ă���B���Ȃ��͕��R�Ǝ􂢂̑������O�����B");
#else
			msg_print("Your divine power is exceeding curse. You teared a cursed equipment off calmly.");
#endif
			o_ptr->curse_flags = 0L;
		}
		else if (((o_ptr->curse_flags & TRC_HEAVY_CURSE) && one_in_(7)) || one_in_(4))
		{
#ifdef JP
			msg_print("���ꂽ������͂Â��Ŕ��������I");
#else
			msg_print("You teared a cursed equipment off by sheer strength!");
#endif

			/* Hack -- Assume felt */
			o_ptr->ident |= (IDENT_SENSE);

			o_ptr->curse_flags = 0L;

			/* Take note */
			o_ptr->feeling = FEEL_NONE;

			/* Recalculate the bonuses */
			cr_ptr->update |= (PU_BONUS);

			/* Window stuff */
			play_window |= (PW_EQUIP);

#ifdef JP
			msg_print("�􂢂�ł��j�����B");
#else
			msg_print("You break the curse.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("�������O���Ȃ������B");
#else
			msg_print("You couldn't remove the equipment.");
#endif
			energy_use = 50;
			return;
		}
	}

	/* Take a partial turn */
	energy_use = 50;

	/* Take off the item */
	(void)inven_takeoff(cr_ptr, item, 255);

	kamaenaoshi(cr_ptr, item);

	calc_android_exp(cr_ptr);

	play_redraw |= (PR_EQUIPPY);
}


/*
 * Drop an item
 */
void do_cmd_drop(creature_type *cr_ptr)
{
	int item, amt = 1;

	object_type *o_ptr;

	cptr q, s;

	if (cr_ptr->special_defense & KATA_MUSOU)
	{
		set_action(cr_ptr, ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���𗎂Ƃ��܂���? ";
	s = "���Ƃ���A�C�e���������Ă��Ȃ��B";
#else
	q = "Drop which item? ";
	s = "You have nothing to drop.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Hack -- Cannot remove cursed items */
	if ((item >= INVEN_1STARM) && object_is_cursed(o_ptr))
	{
		/* Oops */
#ifdef JP
		msg_print("�Ӂ[�ށA�ǂ�������Ă���悤���B");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif


		/* Nope */
		return;
	}


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Take a partial turn */
	energy_use = 50;

	/* Drop (some of) the item */
	inven_drop(cr_ptr, item, amt);

	if (item >= INVEN_1STARM)
	{
		kamaenaoshi(cr_ptr, item);
		calc_android_exp(cr_ptr);
	}

	play_redraw |= (PR_EQUIPPY);
}


static bool high_level_book(object_type *o_ptr)
{
	if ((o_ptr->tval == TV_LIFE_BOOK) ||
	    (o_ptr->tval == TV_SORCERY_BOOK) ||
	    (o_ptr->tval == TV_NATURE_BOOK) ||
	    (o_ptr->tval == TV_CHAOS_BOOK) ||
	    (o_ptr->tval == TV_DEATH_BOOK) ||
	    (o_ptr->tval == TV_TRUMP_BOOK) ||
	    (o_ptr->tval == TV_CRAFT_BOOK) ||
	    (o_ptr->tval == TV_DAEMON_BOOK) ||
	    (o_ptr->tval == TV_CRUSADE_BOOK) ||
	    (o_ptr->tval == TV_MUSIC_BOOK) ||
		(o_ptr->tval == TV_HEX_BOOK))
	{
		if (o_ptr->sval > 1)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}


/*
 * Destroy an item
 */
void do_cmd_destroy(creature_type *cr_ptr)
{
	int			item, amt = 1;
	int			old_number;

	bool		force = FALSE;

	object_type		*o_ptr;
	object_type             forge;
	object_type             *q_ptr = &forge;

	char		o_name[MAX_NLEN];

	char		out_val[MAX_NLEN+40];

	cptr q, s;

	if (cr_ptr->special_defense & KATA_MUSOU)
	{
		set_action(cr_ptr, ACTION_NONE);
	}

	/* Hack -- force destruction */
	if (command_arg > 0) force = TRUE;


	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e�����󂵂܂���? ";
	s = "�󂹂�A�C�e���������Ă��Ȃ��B";
#else
	q = "Destroy which item? ";
	s = "You have nothing to destroy.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Verify unless quantity given beforehand */
	if (!force && (confirm_destroy || (object_value(o_ptr) > 0)))
	{
		object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

		/* Make a verification */
		sprintf(out_val, 
#ifdef JP
			"�{����%s���󂵂܂���? [y/n/Auto]",
#else
			"Really destroy %s? [y/n/Auto]",
#endif
			o_name);

		msg_print(NULL);

		/* HACK : Add the line to message buffer */
		message_add(out_val);
		play_window |= (PW_MESSAGE);
		window_stuff();

		/* Get an acceptable answer */
		while (TRUE)
		{
			char i;

			/* Prompt */
			prt(out_val, 0, 0);

			i = inkey();

			/* Erase the prompt */
			prt("", 0, 0);


			if (i == 'y' || i == 'Y')
			{
				break;
			}
			if (i == ESCAPE || i == 'n' || i == 'N')
			{
				/* Cancel */
				return;
			}
			if (i == 'A')
			{
				/* Add an auto-destroy preference line */
				if (autopick_autoregister(cr_ptr, o_ptr))
				{
					/* Auto-destroy it */
					autopick_alter_item(cr_ptr, item, TRUE);
				}

				/* The object is already destroyed. */
				return;
			}
		} /* while (TRUE) */
	}

	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, 0);
	o_ptr->number = old_number;

	/* Take a turn */
	energy_use = 100;

	/* Artifacts cannot be destroyed */
	if (!can_player_destroy_object(cr_ptr, o_ptr))
	{
		energy_use = 0;

		/* Message */
#ifdef JP
		msg_format("%s�͔j��s�\���B", o_name);
#else
		msg_format("You cannot destroy %s.", o_name);
#endif

		/* Done */
		return;
	}

	object_copy(q_ptr, o_ptr);

	/* Message */
#ifdef JP
	msg_format("%s���󂵂��B", o_name);
#else
	msg_format("You destroy %s.", o_name);
#endif

	sound(SOUND_DESTITEM);

	/* Reduce the charges of rods/wands */
	reduce_charges(o_ptr, amt);

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	if (high_level_book(q_ptr))
	{
		bool gain_expr = FALSE;

		if (cr_ptr->race_idx1 == RACE_ANDROID)
		{
		}
		else if ((cr_ptr->cls_idx == CLASS_WARRIOR) || (cr_ptr->cls_idx == CLASS_BERSERKER))
		{
			gain_expr = TRUE;
		}
		else if (cr_ptr->cls_idx == CLASS_PALADIN)
		{
			if (is_good_realm(cr_ptr->realm1))
			{
				if (!is_good_realm(tval2realm(q_ptr->tval))) gain_expr = TRUE;
			}
			else
			{
				if (is_good_realm(tval2realm(q_ptr->tval))) gain_expr = TRUE;
			}
		}

		if (gain_expr && (cr_ptr->exp < PY_MAX_EXP))
		{
			s32b tester_exp = cr_ptr->max_exp / 20;
			if (tester_exp > 10000) tester_exp = 10000;
			if (q_ptr->sval < 3) tester_exp /= 4;
			if (tester_exp<1) tester_exp = 1;

#ifdef JP
msg_print("�X�Ɍo����ς񂾂悤�ȋC������B");
#else
			msg_print("You feel more experienced.");
#endif

			gain_exp(cr_ptr, tester_exp * amt);
		}
		}

	if (item >= INVEN_1STARM) calc_android_exp(cr_ptr);
}


/*
 * Observe an item which has been *identify*-ed
 */
void do_cmd_observe(creature_type *cr_ptr)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[MAX_NLEN];

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���𒲂ׂ܂���? ";
	s = "���ׂ���A�C�e�����Ȃ��B";
#else
	q = "Examine which item? ";
	s = "You have nothing to examine.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Require full knowledge */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
#ifdef JP
		msg_print("���̃A�C�e���ɂ��ē��ɒm���Ă��邱�Ƃ͂Ȃ��B");
#else
		msg_print("You have no special knowledge about that item.");
#endif

		return;
	}


	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
#ifdef JP
	msg_format("%s�𒲂ׂĂ���...", o_name);
#else
	msg_format("Examining %s...", o_name);
#endif

	/* Describe it fully */
#ifdef JP
	if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL)) msg_print("���ɕς�����Ƃ���͂Ȃ��悤���B");
#else
	if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL)) msg_print("You see nothing special.");
#endif

}



/*
 * Remove the inscription from an object
 * XXX Mention item (when done)?
 */
void do_cmd_uninscribe(creature_type *cr_ptr)
{
	int   item;

	object_type *o_ptr;

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���̖��������܂���? ";
	s = "����������A�C�e�����Ȃ��B";
#else
	q = "Un-inscribe which item? ";
	s = "You have nothing to un-inscribe.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Nothing to remove */
	if (!o_ptr->inscription)
	{
#ifdef JP
		msg_print("���̃A�C�e���ɂ͏����ׂ������Ȃ��B");
#else
		msg_print("That item had no inscription to remove.");
#endif

		return;
	}

	/* Message */
#ifdef JP
	msg_print("�����������B");
#else
	msg_print("Inscription removed.");
#endif


	/* Remove the incription */
	o_ptr->inscription = 0;

	/* Combine the pack */
	cr_ptr->notice |= (PN_COMBINE);

	/* Window stuff */
	play_window |= (PW_INVEN | PW_EQUIP);

	/* .��$�̊֌W��, �Čv�Z���K�v�Ȃ͂� -- henkma */
	cr_ptr->update |= (PU_BONUS);

}


/*
 * Inscribe an object with a comment
 */
void do_cmd_inscribe(creature_type *cr_ptr)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[MAX_NLEN];

	char		out_val[80];

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ǂ̃A�C�e���ɖ������݂܂���? ";
	s = "�������߂�A�C�e�����Ȃ��B";
#else
	q = "Inscribe which item? ";
	s = "You have nothing to inscribe.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Describe the activity */
	object_desc(o_name, o_ptr, OD_OMIT_INSCRIPTION);

	/* Message */
#ifdef JP
	msg_format("%s�ɖ������ށB", o_name);
#else
	msg_format("Inscribing %s.", o_name);
#endif

	msg_print(NULL);

	/* Start with nothing */
	strcpy(out_val, "");

	/* Use old inscription */
	if (o_ptr->inscription)
	{
		/* Start with the old inscription */
		strcpy(out_val, quark_str(o_ptr->inscription));
	}

	/* Get a new inscription (possibly empty) */
#ifdef JP
	if (get_string("��: ", out_val, 80))
#else
	if (get_string("Inscription: ", out_val, 80))
#endif
	{
		/* Save the inscription */
		o_ptr->inscription = quark_add(out_val);

		/* Combine the pack */
		cr_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* .��$�̊֌W��, �Čv�Z���K�v�Ȃ͂� -- henkma */
		cr_ptr->update |= (PU_BONUS);
	}
}


/*
 * Inscribe caves with a comment
 */
void do_cmd_inscribe_caves(creature_type *cr_ptr)
{
	char tmp[CAVE_MESSAGE_LENGTH];

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	msg_format("�����̐^���Ƀ��b�Z�[�W������:");
#else
	msg_format("Inscribe messages on your floor.");
#endif

	msg_print(NULL);

	tmp[0] = '\0';
	strcpy(tmp, cave[cr_ptr->fy][cr_ptr->fx].message);

	/* Get a new inscription (possibly empty) */
#ifdef JP
	if(get_string("���b�Z�[�W: ", tmp, CAVE_MESSAGE_LENGTH)){
#else
	if(get_string("Message: ", tmp, CAVE_MESSAGE_LENGTH)){
#endif
		strcpy(cave[cr_ptr->fy][cr_ptr->fx].message, tmp);
	}
	else
	{
#ifdef JP
		msg_print("���f�����B");
#else
		msg_print("Canceled.");
#endif
		return;
	}

	if(strstr(cave[cr_ptr->fy][cr_ptr->fx].message, "Elbereth"))
	{
		char error_m[1024];
		bool stop_ty = FALSE;
		int count = 0;

		sound(SOUND_ILLEGAL);
#ifdef JP
		if (!get_rnd_line("error_j.txt", 0, error_m))
#else
		if (!get_rnd_line("error.txt", 0, error_m))
#endif
		msg_print(error_m);
		msg_print(NULL);

		do
		{
			stop_ty = activate_ty_curse(cr_ptr, stop_ty, &count);
		}
		while (one_in_(6));
		strcpy(cave[cr_ptr->fy][cr_ptr->fx].message, "");

#ifdef JP
		msg_print("���b�Z�[�W�͏����������B");
#else
		msg_print("Messages vanished.");
#endif

	}


}



/*
 * An "item_tester_hook" for refilling lanterns
 */
static bool item_tester_refill_lantern(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if (o_ptr->tval == TV_FLASK) return (TRUE);

	/* Laterns are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_LANTERN)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refill the players lamp (from the pack or floor)
 */
static void do_cmd_refill_lamp(creature_type *cr_ptr)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;

	cptr q, s;


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_lantern;

	/* Get an item */
#ifdef JP
	q = "�ǂ̖��ڂ��璍���܂���? ";
	s = "���ڂ��Ȃ��B";
#else
	q = "Refill with which flask? ";
	s = "You have no flasks of oil.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Take a partial turn */
	energy_use = 50;

	/* Access the lantern */
	j_ptr = &cr_ptr->inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->xtra4 += o_ptr->xtra4;

	/* Message */
#ifdef JP
	msg_print("�����v�ɖ��𒍂����B");
#else
	msg_print("You fuel your lamp.");
#endif

	/* Comment */
	if ((o_ptr->name2 == EGO_LITE_DARKNESS) && (j_ptr->xtra4 > 0))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�����v�������Ă��܂����I");
#else
		msg_print("Your lamp has gone out!");
#endif
	}
	else if ((o_ptr->name2 == EGO_LITE_DARKNESS) || (j_ptr->name2 == EGO_LITE_DARKNESS))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�����������v�͑S������Ȃ��B");
#else
		msg_print("Curiously, your lamp doesn't light.");
#endif
	}
	else if (j_ptr->xtra4 >= FUEL_LAMP)
	{
		j_ptr->xtra4 = FUEL_LAMP;
#ifdef JP
		msg_print("�����v�̖��͈�t���B");
#else
		msg_print("Your lamp is full.");
#endif

	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	cr_ptr->update |= (PU_TORCH);
}


/*
 * An "item_tester_hook" for refilling torches
 */
static bool item_tester_refill_torch(creature_type *cr_ptr, object_type *o_ptr)
{
	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_TORCH)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refuel the players torch (from the pack or floor)
 */
static void do_cmd_refill_torch(creature_type *cr_ptr)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;

	cptr q, s;


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_torch;

	/* Get an item */
#ifdef JP
	q = "�ǂ̏����Ŗ���������߂܂���? ";
	s = "���ɏ������Ȃ��B";
#else
	q = "Refuel with which torch? ";
	s = "You have no extra torches.";
#endif

	if (!get_item(cr_ptr, &item, q, s, (USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &cr_ptr->inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Take a partial turn */
	energy_use = 50;

	/* Access the primary torch */
	j_ptr = &cr_ptr->inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->xtra4 += o_ptr->xtra4 + 5;

	/* Message */
#ifdef JP
	msg_print("���������������B");
#else
	msg_print("You combine the torches.");
#endif


	/* Comment */
	if ((o_ptr->name2 == EGO_LITE_DARKNESS) && (j_ptr->xtra4 > 0))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�����������Ă��܂����I");
#else
		msg_print("Your torch has gone out!");
#endif
	}
	else if ((o_ptr->name2 == EGO_LITE_DARKNESS) || (j_ptr->name2 == EGO_LITE_DARKNESS))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�����������͑S������Ȃ��B");
#else
		msg_print("Curiously, your torche don't light.");
#endif
	}
	/* Over-fuel message */
	else if (j_ptr->xtra4 >= FUEL_TORCH)
	{
		j_ptr->xtra4 = FUEL_TORCH;
#ifdef JP
		msg_print("�����̎����͏\�����B");
#else
		msg_print("Your torch is fully fueled.");
#endif

	}

	/* Refuel message */
	else
	{
#ifdef JP
		msg_print("�����͂����������邭�P�����B");
#else
		msg_print("Your torch glows more brightly.");
#endif

	}

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	/* Recalculate torch */
	cr_ptr->update |= (PU_TORCH);
}


/*
 * Refill the players lamp, or restock his torches
 */
void do_cmd_refill(creature_type *cr_ptr)
{
	object_type *o_ptr;

	/* Get the light */
	o_ptr = &cr_ptr->inventory[INVEN_LITE];

	if (cr_ptr->special_defense & KATA_MUSOU)
	{
		set_action(cr_ptr, ACTION_NONE);
	}

	/* It is nothing */
	if (o_ptr->tval != TV_LITE)
	{
#ifdef JP
		msg_print("�����𑕔����Ă��Ȃ��B");
#else
		msg_print("You are not wielding a light.");
#endif

	}

	/* It's a lamp */
	else if (o_ptr->sval == SV_LITE_LANTERN)
	{
		do_cmd_refill_lamp(cr_ptr);
	}

	/* It's a torch */
	else if (o_ptr->sval == SV_LITE_TORCH)
	{
		do_cmd_refill_torch(cr_ptr);
	}

	/* No torch to refill */
	else
	{
#ifdef JP
		msg_print("���̌����͎��������΂��Ȃ��B");
#else
		msg_print("Your light cannot be refilled.");
#endif

	}
}


/*
 * Target command
 */
void do_cmd_target(creature_type *cr_ptr)
{
	/* Target set */
	if (target_set(cr_ptr,TARGET_KILL))
	{
#ifdef JP
		msg_print("�^�[�Q�b�g����B");
#else
		msg_print("Target Selected.");
#endif

	}

	/* Target aborted */
	else
	{
#ifdef JP
		msg_print("�^�[�Q�b�g�����B");
#else
		msg_print("Target Aborted.");
#endif

	}
}



/*
 * Look command
 */
void do_cmd_look(creature_type *cr_ptr)
{
	/* Look around */
	if (target_set(cr_ptr, TARGET_LOOK))
	{
#ifdef JP
		msg_print("�^�[�Q�b�g����B");
#else
		msg_print("Target Selected.");
#endif

	}
}



/*
 * Allow the player to examine other sectors on the map
 */
void do_cmd_locate(creature_type *cr_ptr)
{
	int		dir, y1, x1, y2, x2;

	char	tmp_val[80];

	char	out_val[160];

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);


	/* Start at current panel */
	y2 = y1 = panel_row_min;
	x2 = x1 = panel_col_min;

	/* Show panels until done */
	while (1)
	{
		/* Describe the location */
		if ((y2 == y1) && (x2 == x1))
		{
#ifdef JP
			strcpy(tmp_val, "�^��");
#else
			tmp_val[0] = '\0';
#endif

		}
		else
		{
#ifdef JP
			sprintf(tmp_val, "%s%s",
				((y2 < y1) ? "�k" : (y2 > y1) ? "��" : ""),
				((x2 < x1) ? "��" : (x2 > x1) ? "��" : ""));
#else
			sprintf(tmp_val, "%s%s of",
				((y2 < y1) ? " North" : (y2 > y1) ? " South" : ""),
				((x2 < x1) ? " West" : (x2 > x1) ? " East" : ""));
#endif

		}

		/* Prepare to ask which way to look */
		sprintf(out_val,
#ifdef JP
			"�}�b�v�ʒu [%d(%02d),%d(%02d)] (�v���C���[��%s)  ����?",
#else
			"Map sector [%d(%02d),%d(%02d)], which is%s your sector.  Direction?",
#endif

			y2 / (hgt / 2), y2 % (hgt / 2),
			x2 / (wid / 2), x2 % (wid / 2), tmp_val);

		/* Assume no direction */
		dir = 0;

		/* Get a direction */
		while (!dir)
		{
			char command;

			/* Get a command (or Cancel) */
			if (!get_com(out_val, &command, TRUE)) break;

			/* Extract the action (if any) */
			dir = get_keymap_dir(command);

			/* Error */
			if (!dir) bell();
		}

		/* No direction */
		if (!dir) break;

		/* Apply the motion */
		if (change_panel(ddy[dir], ddx[dir]))
		{
			y2 = panel_row_min;
			x2 = panel_col_min;
		}
	}


	/* Recenter the map around the player */
	verify_panel(cr_ptr);

	/* Update stuff */
	cr_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff(cr_ptr);
}



/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
bool ang_sort_comp_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;

	/* Sort by player kills */
	if (*why >= 4)
	{
		/* Extract player kills */
		z1 = r_info[w1].r_pkills;
		z2 = r_info[w2].r_pkills;

		/* Compare player kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = r_info[w1].r_tkills;
		z2 = r_info[w2].r_tkills;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = r_info[w1].level;
		z2 = r_info[w2].level;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = r_info[w1].mexp;
		z2 = r_info[w2].mexp;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
void ang_sort_swap_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* Unused */
	(void)v;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}



/*
 * Identify a character, allow recall of monsters
 *
 * Several "special" responses recall "multiple" monsters:
 *   ^A (all monsters)
 *   ^U (all unique monsters)
 *   ^N (all non-unique monsters)
 *
 * The responses may be sorted in several ways, see below.
 *
 * Note that the player ghosts are ignored. XXX XXX XXX
 */
void do_cmd_query_symbol(creature_type *cr_ptr)
{
	int		i, n, species_idx;
	char	sym, query;
	char	buf[128];

	bool	all = FALSE;
	bool	uniq = FALSE;
	bool	norm = FALSE;
	bool	ride = FALSE;
	char    temp[80] = "";

	bool	recall = FALSE;

	u16b	why = 0;
	u16b	*who;

	/* Get a character, or abort */
#ifdef JP
	if (!get_com("�m�肽����������͂��ĉ�����(�L�� or ^A�S,^U��,^N��,^R��n,^M���O): ", &sym, FALSE)) return;
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE)) return;
#endif

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

	/* Describe */
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "�S�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Full monster list.");
#endif
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "���j�[�N�E�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Unique monster list.");
#endif
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "���j�[�N�O�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Non-unique monster list.");
#endif
	}
	else if (sym == KTRL('R'))
	{
		all = ride = TRUE;
#ifdef JP
		strcpy(buf, "��n�\�����X�^�[�̃��X�g");
#else
		strcpy(buf, "Ridable monster list.");
#endif
	}
	/* XTRA HACK WHATSEARCH */
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("���O(�p��̏ꍇ�������ŉ�)",temp, 70))
#else
		if (!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;
			return;
		}
#ifdef JP
		sprintf(buf, "���O:%s�Ƀ}�b�`",temp);
#else
		sprintf(buf, "Monsters with a name \"%s\"",temp);
#endif
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
		sprintf(buf, "%c - %s", sym, "�����ȕ���");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif
	}

	/* Display the result */
	prt(buf, 0, 0);

	/* Allocate the "who" array */
	C_MAKE(who, max_species_idx, u16b);

	/* Collect matching monsters */
	for (n = 0, i = 1; i < max_species_idx; i++)
	{
		species_type *r_ptr = &r_info[i];

		/* Nothing to recall */
		if (!cheat_know && !r_ptr->r_sights) continue;

		/* Require non-unique monsters if needed */
		if (norm && is_unique_species(r_ptr)) continue;

		/* Require unique monsters if needed */
		if (uniq && !is_unique_species(r_ptr)) continue;

		/* Require ridable monsters if needed */
		if (ride && !is_riding_species(r_ptr)) continue;

		/* XTRA HACK WHATSEARCH */
		if (temp[0])
		{
		  int xx;
		  char temp2[80];
  
		  for (xx=0; temp[xx] && xx<80; xx++)
		  {
#ifdef JP
		    if (iskanji( temp[xx])) { xx++; continue; }
#endif
		    if (isupper(temp[xx])) temp[xx]=tolower(temp[xx]);
		  }
  
#ifdef JP
		  strcpy(temp2, r_name+r_ptr->E_name);
#else
		  strcpy(temp2, r_name+r_ptr->name);
#endif
		  for (xx=0; temp2[xx] && xx<80; xx++)
		    if (isupper(temp2[xx])) temp2[xx]=tolower(temp2[xx]);
  
#ifdef JP
		  if (my_strstr(temp2, temp) || my_strstr(r_name + r_ptr->name, temp) )
#else
		  if (my_strstr(temp2, temp))
#endif
			  who[n++]=i;
		}

		/* Collect "appropriate" monsters */
		else if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_species_idx, u16b);

		return;
	}


	/* Prompt XXX XXX XXX */
#ifdef JP
	put_str("�v���o�����܂���? (k:�E�Q��/y/n): ", 0, 36);
#else
	put_str("Recall details? (k/y/n): ", 0, 40);
#endif


	/* Query */
	query = inkey();

	/* Restore */
	prt(buf, 0, 0);

	why = 2;

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array */
	ang_sort(who, &why, n);

	/* Sort by kills (and level) */
	if (query == 'k')
	{
		why = 4;
		query = 'y';
	}

	/* Catch "escape" */
	if (query != 'y')
	{
		/* Free the "who" array */
		C_KILL(who, max_species_idx, u16b);

		return;
	}

	/* Sort if needed */
	if (why == 4)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	i = n - 1;

	/* Scan the monster memory */
	while (1)
	{
		/* Extract a race */
		species_idx = who[i];

		/* Hack -- Auto-recall */
		species_type_track(species_idx);

		/* Hack -- Handle stuff */
		handle_stuff(cr_ptr);

		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/* Save the screen */
				screen_save();

				/* Recall on screen */
				screen_roff(who[i], 0);
			}

			/* Hack -- Begin the prompt */
			roff_top(species_idx);

			/* Hack -- Complete the prompt */
#ifdef JP
			Term_addstr(-1, TERM_WHITE, " ['r'�v���o, ESC]");
#else
			Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");
#endif

			/* Command */
			query = inkey();

			/* Unrecall */
			if (recall)
			{
				/* Restore */
				screen_load();
			}

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}

	/* Free the "who" array */
	C_KILL(who, max_species_idx, u16b);

	/* Re-display the identity */
	prt(buf, 0, 0);
}


