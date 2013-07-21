/* File: mind.c */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

/* Purpose: Mindcrafter code */

#include "angband.h"
#include "autopick.h"
#include "creature_melee.h"
#include "creature_throwing.h"
#include "mindtips.h"



mind_power mind_powers[5] =
{
	{
		{
			/* Level gained, cost, %fail, name */
#ifdef JP
			{ 1, 1, 15, "óÏéã"},
			{ 2, 1, 20, "ê_åoçUåÇ"},
			{ 3, 2, 25, "éüå≥ÇÃèuÇ´"},
			{ 7, 6, 35, "ãïãÛÇÃå∂âe"},
			{ 9, 7, 50, "ê∏ê_éxîz"},
			{ 11, 7, 30, "îOìÆè’åÇíe"},
			{ 13, 12, 50, "äZâª"},
			{ 15, 12, 60, "ÉTÉCÉRÉÅÉgÉäÅ["},
			{ 18, 10, 45, "ê∏ê_îgìÆ"},
			{ 23, 15, 50, "ÉAÉhÉåÉiÉäÉìÅEÉhÅ[ÉsÉìÉO"},
			{ 26, 28, 60, "ÉeÉåÉLÉlÉVÉX"},
			{ 28, 10, 40, "ÉTÉCÉLÉbÉNÅEÉhÉåÉCÉì"},
			{ 35, 35, 75, "åıÇÃåï"},
			{ 45,150, 85, "äÆëSÇ»ê¢äE"},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
#else
			{ 1, 1, 15, "Precognition"},
			{ 2, 1, 20, "Neural Blast"},
			{ 3, 2, 25, "Minor Displacement"},
			{ 7, 6, 35, "Major Displacement"},
			{ 9, 7, 50, "Domination"},
			{ 11, 7, 30, "Pulverise"},
			{ 13, 12, 50, "Character Armour"},
			{ 15, 12, 60, "Psychometry" },
			{ 18, 10, 45, "Mind Wave" },
			{ 23, 15, 50, "Adrenaline Channeling"},
			{ 26, 28, 60, "Telekinesis"},
			{ 28, 10, 40, "Psychic Drain"},
			{ 35, 35, 75, "Psycho-Spear"},
			{ 45,150, 85, "The World"},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
#endif

		}
	},

	{
		{
			/* Level gained, cost, %fail, name */
#ifdef JP
			{ 1, 1, 15, "è¨ó¥"},
			{ 3, 3, 30, "ëMåı"},
			{ 5, 6, 35, "ïëãÛèp"},
			{ 8, 5, 40, "ÉJÉÅÉnÉÅîg"},
			{ 10, 7, 45, "ëŒñÇñ@ñhå‰"},
			{ 13, 5, 60, "ó˚ãC"},
			{ 17, 17, 50, "ìZì¨ãC"},
			{ 20, 20, 50, "è’îg"},
			{ 23, 18, 55, "úaó¥"},
			{ 25, 30, 70, "Ç¢ÇƒÇ¬Ç≠îgìÆ"},
			{ 28, 26, 50, "å∂óÏè¢ä´"},
			{ 32, 35, 65, "ó˘çñâŒâä"},
			{ 38, 42, 75, "í¥ÉJÉÅÉnÉÅîg"},
			{ 44, 50, 80, "åıë¨à⁄ìÆ"},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
#else
			{ 1, 1, 15, "Small Force Ball"},
			{ 3, 3, 30, "Flash Light"},
			{ 5, 6, 35, "Flying Technique"},
			{ 8, 5, 40, "Kamehameha"},
			{ 10, 7, 45, "Magic Resistance"},
			{ 13, 5, 60, "Improve Force"},
			{ 17, 17, 50, "Aura of Force"},
			{ 20, 20, 50, "Shock Power"},
			{ 23, 18, 55, "Large Force Ball"},
			{ 25, 30, 70, "Dispel Magic"},
			{ 28, 26, 50, "Summon Ghost"},
			{ 32, 35, 65, "Exploding Frame"},
			{ 38, 42, 75, "Super Kamehameha"},
			{ 44, 50, 80, "Light Speed"},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
#endif

		}
	},

	{
		{
			/* Level gained, cost, %fail, name */
#ifdef JP
			{  8, 5, 40, "éEãCä¥ím"},
			{ 15, 20, 0, "ìÀåÇ"},
			{ 20, 15, 0, "ÉgÉâÉbÉvï≤ç”"},
			{ 25, 20, 60, "ínêk"},
			{ 30, 80, 75, "äFéEÇµ"},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
#else
			{  8, 5, 40, "Detect Atmosphere of Menace"},
			{ 15, 20, 0, "Charge"},
			{ 20, 15, 0, "Smash a Trap"},
			{ 25, 20, 60, "Quake"},
			{ 30, 80, 75, "Massacre"},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
			{ 99, 0, 0, ""},
#endif

		}
	},

	{
		{
			/* Level gained, cost, %fail, name */
#ifdef JP
			{ 1, 1, 15, "ê^å©ÇÃãæ"},
			{ 1, 2, 40, "ãæê∂ê¨"},
			{ 2, 2, 20, "åıÇÃÇµÇ∏Ç≠"},
			{ 3, 2, 20, "òcÇÒÇæãæ"},
			{ 5, 3, 35, "ëMåıãæ"},
			{ 6, 5, 35, "úfÇ¶ÇÈãæ"},

			{ 10, 5, 30, "î˜êoâBÇÍ"},
			{ 12, 12, 30, "í«ï˙ÇÃãæ"},
			{ 15, 15, 30, "ãæç”Ç´"},
			{ 19, 13, 30, "ç√ñ∞ãæ"},
			{ 23, 18, 50, "ÉVÅ[ÉJÅ[ÉåÉC"},

			{ 25, 20, 40, "ãæÇÃïïàÛ"},
			{ 27, 30, 60, "êÖãæÇÃèÇ"},
			{ 29, 30, 60, "ÉXÅ[ÉpÅ[ÉåÉC"},
			{ 31, 35, 60, "å∂òfÇÃåı"},
			{ 33, 50, 80, "ãæÇÃçë"},

			{ 36, 30, 80, "ãæî≤ÇØ"},
			{ 38, 40, 70, "ãAä“ÇÃãæ"},
			{ 40, 50, 55, "âeï™êg"},
			{ 43, 55, 70, "ïïñÇåãäE"},
			{ 46, 70, 75, "ÉâÉtÉmÅ[ÉãÇÃãæ"},
#else
			{ 1, 1, 15, "Mirror of Seeing"},
			{ 1, 2, 40, "Making a Mirror"},
			{ 2, 2, 20, "Drip of Light"},
			{ 3, 2, 20, "Warped Mirror"},
			{ 5, 3, 35, "Mirror of Light"},
			{ 6, 5, 35, "Mirror of Wandering"},

			{ 10, 5, 30, "Robe of Dust"},
			{ 12, 12, 30, "Banishing Mirror"},
			{ 15, 15, 30, "Mirror Clashing"},
			{ 19, 13, 30, "Mirror Sleeping"},
			{ 23, 18, 50, "Seeker Ray"},

			{ 25, 20, 40, "Seal of Mirror"},
			{ 27, 30, 60, "Shield of Water"},
			{ 29, 30, 60, "Super Ray"},
			{ 31, 35, 60, "Illusion Light"},
			{ 33, 50, 80, "Mirror Shift"},

			{ 36, 30, 80, "Mirror Tunnel"},
			{ 38, 40, 70, "Mirror of Recall"},
			{ 40, 50, 55, "Multi-Shadow"},
			{ 43, 55, 70, "Binding Field"},
			{ 46, 70, 75, "Mirror of Ruffnor"},
#endif

		}
	},

	{
		{
			/* Level gained, cost, %fail, name */
#ifdef JP
			{  1, 1, 20, "à√à≈ê∂ê¨"},
			{  2, 2, 25, "é¸ï”í≤ç∏"},
			{  3, 3, 25, "ótâBÇÍ"},
			{  5, 3, 30, "ïœÇÌÇËêg"},
			{  7, 8, 35, "çÇîÚÇ—"},
			{  8, 10, 35, "àÍåÇó£íE"},
			{ 10, 10, 40, "ã‡îõÇË"},
			{ 12, 12, 70, "å√ÇÃå˚ì`"},
			{ 15, 10, 50, "ïÇâ_"},
			{ 17, 12, 45, "âŒìŸ"},
			{ 18, 20, 40, "ì¸êg"},
			{ 20, 5, 50, "î™ï˚éËó†åï"},
			{ 22, 15, 55, "çΩäô"},
			{ 25, 32, 60, "âåã "},
			{ 28, 32, 60, "ì]êg"},
			{ 30, 30, 70, "îöî≠ÇÃñ‰èÕ"},
			{ 32, 40, 40, "ìyìŸ"},
			{ 34, 35, 50, "ñ∂âBÇÍ"},
			{ 38, 40, 60, "ó˘çñâŒâä"},
			{ 41, 50, 55, "ï™êg"},
			{ 99, 0, 0, ""},
#else
			{  1, 1, 20, "Create Darkness"},
			{  2, 2, 25, "Detect Near"},
			{  3, 3, 25, "Hide in Leafs"},
			{  5, 3, 30, "Kawarimi"},
			{  7, 8, 35, "Absconding"},
			{  8, 10, 35, "Hit and Away"},
			{ 10, 10, 40, "Bind Creature"},
			{ 12, 12, 70, "Ancient Knowledge"},
			{ 15, 10, 50, "Floating"},
			{ 17, 12, 45, "Hide in Flame"},
			{ 18, 20, 40, "Nyusin"},
			{ 20, 5, 50, "Syuriken Spreading"},
			{ 22, 15, 55, "Chain Hook"},
			{ 25, 32, 60, "Smoke Ball"},
			{ 28, 32, 60, "Swap Position"},
			{ 30, 30, 70, "Glyph of Explosion"},
			{ 32, 40, 40, "Hide in Mud"},
			{ 34, 35, 50, "Hide in Mist"},
			{ 38, 40, 60, "Rengoku-Kaen"},
			{ 41, 50, 55, "Bunshin"},
			{ 99, 0, 0, ""},
#endif

		}
	},
};

/*
* Forcibly pseudo-identify an object in the inventory
* (or on the floor)
*
* note: currently this function allows pseudo-id of any object,
* including silly ones like potions & scrolls, which always
* get '{average}'. This should be changed, either to stop such
* items from being pseudo-id'd, or to allow psychometry to
* detect whether the unidentified potion/scroll/etc is
* good (Cure Light Wounds, Restore Strength, etc) or
* bad (Poison, Weakness etc) or 'useless' (Slime Mold Juice, etc).
*/
bool psychometry(creature_type *caster_ptr)
{
	OBJECT_ID item;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];
	byte            feel;
	bool okay = FALSE;

	if(!get_item(caster_ptr, &item, MES_OBJECT_WHICH_OBSERVE, MES_OBJECT_NO_OBSERVE, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, 0)) return FALSE;
	object_ptr = GET_ITEM(caster_ptr, item);

	if(object_is_known(object_ptr)) /* It is fully known, no information needed */
	{
		msg_print(MES_TRAIT_PSYCHOMETRY_NO_FIND);
		return TRUE;
	}

	feel = value_check_aux1(caster_ptr, object_ptr); /* Check for a feeling */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY)); /* Get an object description */

	if(!feel) /* Skip non-feelings */
	{
		msg_format(MES_TRAIT_PSYCHOMETRY_NO_UNUSUAL(object_ptr));
		return TRUE;
	}

	msg_format(MES_TRAIT_PSYCHOMETRY_FELT(object_ptr, game_inscriptions[feel]));
	object_ptr->ident |= (IDENT_SENSE); /* We have "felt" it */
	object_ptr->feeling = feel; /* "Inscribe" it */
	object_ptr->marked |= OM_TOUCHED; /* Player touches it */
	prepare_update(caster_ptr, CRU_COMBINE | CRU_REORDER); /* Combine / Reorder the pack (later) */

	prepare_window(PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Valid "tval" codes */
	switch (object_ptr->tval)
	{
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
	case TV_BOW:
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_SWORD:
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HELM:
	case TV_CROWN:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_SOFT_ARMOR:
	case TV_HARD_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_CARD:
	case TV_RING:
	case TV_AMULET:
	case TV_LITE:
	case TV_FIGURINE:
		okay = TRUE;
		break;
	}

	autopick_alter_item(caster_ptr, item, (bool)(okay && destroy_feeling)); /* Auto-inscription/destroy */
	return TRUE; /* Something happened */
}




void mindcraft_info(creature_type *caster_ptr, char *p, int use_mind, POWER power)
{
	char buf[30];
	static const char s_dam[] = KW_DAM;
	cptr s_dur = KW_DURING;
	cptr s_range = KW_RANGE;
	int lev_bonus = caster_ptr->lev;

	strcpy(p, "");

	format_weight(buf, lev_bonus * 15);
	switch (use_mind)
	{
	case MIND_MINDCRAFTER:
		switch (power)
		{
		case 0:  break;
		case 1:  sprintf(p, " %s%dd%d", s_dam, 3 + ((lev_bonus - 1) / 4), 3 + lev_bonus/15); break;
		case 2:  sprintf(p, " %s10", s_range); break;
		case 3:  sprintf(p, " %s%d", s_range, lev_bonus * 5);  break;
		case 4:  break;
		case 5: sprintf(p, " %s%dd8", s_dam, 8 + ((lev_bonus - 5) / 4));  break;
		case 6:  sprintf(p, " %s%d", s_dur, lev_bonus);  break;
		case 7:  break;
		case 8:  sprintf(p, (lev_bonus < 25 ? " %s%d" : " %sd%d"), s_dam, (lev_bonus < 25 ? lev_bonus * 3 / 2 : lev_bonus * ((lev_bonus - 5) / 10 + 1))); break;
		case 9:  sprintf(p, " %s10+d%d", s_dur, lev_bonus * 3 / 2);  break;
#ifdef JP
		case 10: sprintf(p, " ç≈ëÂèdó :%s", buf);  break;
#else
		case 10: sprintf(p, " max wgt %s", buf);  break;
#endif
		case 11: sprintf(p, " %s%dd6", s_dam, lev_bonus / 2);  break;
		case 12: sprintf(p, " %sd%d+%d", s_dam, lev_bonus * 3, lev_bonus * 3); break;
#ifdef JP
		case 13: sprintf(p, " çsìÆ:%ldâÒ", (caster_ptr->csp + 100 - caster_ptr->energy_need - 50) / 100); break;
#else
		case 13: sprintf(p, " %ld acts.", (caster_ptr->csp + 100 - caster_ptr->energy_need - 50) / 100); break;
#endif
		}
		break;
	case MIND_KI:
		{
			int boost = caster_ptr->charged_force;

			if(heavy_armor(caster_ptr)) boost /= 2;

			switch (power)
			{
			case 0:  sprintf(p, " %s%dd4", s_dam, 3 + ((lev_bonus - 1) / 5) + boost / 12); break;
			case 1:  break;
			case 2:  sprintf(p, " %s%d+d30", s_dur, 30 + boost / 5); break;
			case 3:  sprintf(p, " %s%dd5", s_dam, 5 + ((lev_bonus - 1) / 5) + boost / 10); break;
			case 4:  sprintf(p, " %s%d+d20", s_dur, 20 + boost / 5); break;
			case 5:  break;
			case 6:  sprintf(p, " %s%d+d%d", s_dur, 15 + boost / 7, lev_bonus / 2); break;
			case 7:  sprintf(p, " %s%dd8", s_dam, 8 + ((lev_bonus - 5) / 5) + boost / 12); break;
			case 8:  sprintf(p, " %s10d6+%d", s_dam, lev_bonus * 3 / 2 + boost * 3 / 5); break;
			case 9:  break;
#ifdef JP
			case 10: sprintf(p, " ç≈ëÂ%dëÃ", 1+boost/100); break;
#else
			case 10: sprintf(p, " max %d", 1+boost/100); break;
#endif
			case 11: sprintf(p, " %s%d", s_dam, 100 + lev_bonus + boost); break;
			case 12: sprintf(p, " %s%dd15", s_dam, 10 + lev_bonus / 2 + boost * 3 / 10); break;
#ifdef JP
			case 13: sprintf(p, " çsìÆ:%d+d16âÒ", 16+boost/20); break;
#else
			case 13: sprintf(p, " %d+d16 acts", 16+boost/20); break;
#endif
			}
			break;
	case MIND_MIRROR_MASTER:
		{
			switch (power)
			{
			case 0:  break;
			case 1:  break;
			case 2:  sprintf(p, " %s%dd4", s_dam, 3 + ((lev_bonus - 1) / 5) ); break;
			case 3:  sprintf(p, " %s10", s_range); break;
			case 4:  break;
			case 5:  sprintf(p, " %s%d", s_range, lev_bonus *5); break;
			case 6:  sprintf(p, " %s20+d20", s_dur);  break;
			case 7:  break;
			case 8:  sprintf(p, " %s%dd8", s_dam, 8+((lev_bonus -5)/4) ); break;
			case 9:  break;
			case 10: sprintf(p, " %s%dd8", s_dam, 11+(lev_bonus-5)/4 ); break;
			case 11: break;
			case 12: sprintf(p, " %s20+d20", s_dur);  break;
			case 13: sprintf(p, " %s150+d%d", s_dam, lev_bonus*2 ); break;
			case 14: break;
			case 15: break;
			case 16: sprintf(p, " %s%d", s_range, lev_bonus/2 +10); break;
			case 17: break;
			case 18: sprintf(p, " %s6+d6", s_dur);  break;
			case 19: sprintf(p, " %s%d", s_dam, lev_bonus*11+5 ); break;
			case 20: sprintf(p, " %s4+d4", s_dur);  break;
			}
			break;
		}
	case MIND_NINJUTSU:
		{
			switch (power)
			{
			case 0:  break;
			case 1:  break;
			case 2:  sprintf(p, " %s10", s_range); break;
			case 3:  break;
			case 4:  sprintf(p, " %s%d", s_range , lev_bonus *5); break;
			case 5:  sprintf(p, " %s30", s_range); break;
			case 6:  break;
			case 7:  break;
			case 8:  sprintf(p, " %s20+d20", s_dur);  break;
			case 9:  sprintf(p, " %s%d", s_dam, (50+lev_bonus)/2 ); break;
			case 10: break;
			case 11: break;
			case 12: break;
			case 13: break;
			case 14: break;
			case 15: break;
			case 16: sprintf(p, " %s%d+d%d", s_dur, lev_bonus/2, lev_bonus/2);  break;
			case 17: sprintf(p, " %s%d*3", s_dam, (75+lev_bonus*2/3)/2 ); break;
			case 18: sprintf(p, " %s%dd10", s_dam, 6+lev_bonus/8 ); break;
			case 19: sprintf(p, " %s6+d6", s_dur);  break;
			}
			break;
		}
		}
	}
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
static int get_mind_power(creature_type *caster_ptr, KEY *sn, bool only_browse)
{
	KEY i;
	KEY num = 0;
	COODINATES y = 1;
	COODINATES x = 10;
	CREATURE_LEV lev_bonus = caster_ptr->lev;
	int             chance = 0;
	int             ask = TRUE;
	char            choice;
	char            out_val[160];
	char            comment[80];
	cptr            p;

	mind_type       spell;
	mind_power      *mind_ptr;
	bool            flag, redraw;
	int             use_mind;
	KEY menu_line = (use_menu ? 1 : 0);

	switch(caster_ptr->class_idx)
	{
	case CLASS_MINDCRAFTER:
		{
			use_mind = MIND_MINDCRAFTER;
			p = SKILL_NAME_PSYONIC;
			break;
		}
	case CLASS_FORCETRAINER:
		{
			use_mind = MIND_KI;
			p = SKILL_NAME_FORCE;
			break;
		}
	case CLASS_BERSERKER:
		{
			use_mind = MIND_BERSERKER;
			p = SKILL_NAME_BRUTAL;
			break;
		}
	case CLASS_MIRROR_MASTER:
		{
			use_mind = MIND_MIRROR_MASTER;
			p = SKILL_NAME_MIRROR_M;
			break;
		}
	case CLASS_NINJA:
		{
			use_mind = MIND_NINJUTSU;
			p = SKILL_NAME_NINJUTSU;
			break;
		}
	default:
		{
			use_mind = 0;
			p = SKILL_NAME_PSYONIC;
			break;
		}
	}
	mind_ptr = &mind_powers[use_mind];

	/* Assume cancelled */
	*sn = (-1);

	/* Get the spell, if available */
	if(repeat_pull(sn))
	{
		/* Hack -- If requested INVEN_FORCE(1111), pull again */
		if(*sn == INVEN_FORCE) repeat_pull(sn);

		/* Verify the spell */
		if(mind_ptr->info[*sn].min_lev <= lev_bonus) return TRUE;
	}

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	for (i = 0; i < MAX_MIND_POWERS; i++)
	{
		if(mind_ptr->info[i].min_lev <= lev_bonus) num++;
	}

	/* Build a prompt (accept all spells) */
	if(only_browse) (void)strnfmt(out_val, 78, MES_CAST_WHICH_KNOW(p, I2A(0), I2A(num - 1)));
	else (void)strnfmt(out_val, 78, MES_CAST_WHICH_USE(p, I2A(0), I2A(num - 1)));

	if(use_menu && !only_browse) screen_save();
	/* Get a spell from the user */

	choice= use_menu;
	while (!flag)
	{
		if(choice==ESCAPE) choice = ' '; 
		else if( !get_com(out_val, &choice, TRUE) )break;

		if(use_menu && choice != ' ')
		{
			switch(choice)
			{
			case '0':
				{
					if(!only_browse) screen_load();
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
		}
		/* Request redraw */
		if((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			if(!redraw || use_menu)
			{
				char psi_desc[80];
				bool has_weapon[2];
				redraw = TRUE;

				if(!only_browse && !use_menu) screen_save();

				/* Display a list of spells */
				prt("", y, x);
				put_str(KW_NAME, y, x + 5);
				put_str(format(MES_INTERFACE_SKILL_LIST4(((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU)) ? KW_HP : KW_MP)), y, x + 35);
				has_weapon[0] = get_equipped_slot_num(caster_ptr, SLOT_ID_HAND) > 0;
				has_weapon[1] = get_equipped_slot_num(caster_ptr, SLOT_ID_HAND) > 1;

				/* Dump the spells */
				for (i = 0; i < MAX_MIND_POWERS; i++)
				{
					int mana_cost;
					TRAIT_ID trait_id = 0; /* TODO:Dammy */
					spell = mind_ptr->info[i]; /* Access the spell */
					if(spell.min_lev > lev_bonus)   break;
					chance = spell.fail;
					mana_cost = spell.mana_cost;
					if(chance)
					{
						chance -= 3 * (lev_bonus - spell.min_lev); /* Reduce failure rate by "effective" level adjustment */
						chance -= 3 * (adj_mag_stat[caster_ptr->stat_ind[magic_info[caster_ptr->class_idx].spell_stat]] - 1); /* Reduce failure rate by INT/WIS adjustment */
						if(use_mind == MIND_KI)
						{
							if(heavy_armor(caster_ptr)) chance += 20;
							else if(has_weapon[0]) chance += 10;
							else if(has_weapon[1]) chance += 10;
							if(i == 5)
							{
								int j;
								for (j = 0; j < caster_ptr->charged_force / 50; j++)
									mana_cost += (j+1) * 3 / 2;
							}
						}

						/* Not enough mana to cast */
						if((use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU) && (mana_cost > caster_ptr->csp))
						{
							chance += 5 * (mana_cost - caster_ptr->csp);
						}

						chance += caster_ptr->to_m_chance;
						chance += calc_trait_difficulty(caster_ptr, trait_id, magic_info[caster_ptr->class_idx].spell_stat);
					}

					mindcraft_info(caster_ptr, comment, use_mind, i); /* Get info */

					if(use_menu)
					{
						if(i == (menu_line-1)) strcpy(psi_desc, KET_D_ANGLE);
						else strcpy(psi_desc, "  ");
					}
					else sprintf(psi_desc, "  %c) ",I2A(i));
					/* Dump the spell --(-- */
					strcat(psi_desc,
						format("%-30s%2d %4d%s %3d%%%s", spell.name, spell.min_lev, mana_cost,
#ifdef JP
						(((use_mind == MIND_MINDCRAFTER) && (i == 13)) ? "Å`" : "  "), 
#else
						(((use_mind == MIND_MINDCRAFTER) && (i == 13)) ? "~ " : "  "), 
#endif
						chance, comment));
					prt(psi_desc, y + i + 1, x);
				}
				prt("", y + i + 1, x); /* Clear the bottom line */
			}

			else if(!only_browse)
			{
				redraw = FALSE;
				screen_load();
			}
			continue; /* Redo asking */
		}

		if(!use_menu)
		{
			ask = isupper(choice);

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
		spell = mind_ptr->info[i];

		if(ask)
		{
			char tmp_val[160];
			(void) strnfmt(tmp_val, 78, MES_SYS_ASK_USE, spell.name); /* Belay that order */
			if(!get_check(tmp_val)) continue;
		}
		flag = TRUE;
	}

	if(redraw && !only_browse) screen_load();

	prepare_window(PW_SPELL); /* Show choices */
	window_stuff(player_ptr);

	/* Abort if needed */
	if(!flag) return FALSE;
	(*sn) = i; /* Save the choice */
	repeat_push(*sn);
	return TRUE;
}


// do_cmd_cast calls this function if the player's class
// is 'mindcrafter'.

static bool cast_mindcrafter_spell(creature_type *caster_ptr, int spell)
{
	CREATURE_LEV lev_bonus = caster_ptr->lev;

	switch (spell)
	{
	case 0:
		do_active_trait_tmp(caster_ptr, TRAIT_DETECT_CREATURE, FALSE);
		break;
	case 1:
		do_active_trait_tmp(caster_ptr, TRAIT_MIND_BLST, FALSE);
		break;
	case 2:
		do_active_trait_tmp(caster_ptr, TRAIT_BLINK, FALSE);
		break;
	case 3:
		do_active_trait_tmp(caster_ptr, TRAIT_ACTIVE_TELEPORT, FALSE);
		break;
	case 4:
		do_active_trait_tmp(caster_ptr, TRAIT_CHARM_OTHER, FALSE);
		break;
	case 5:
		do_active_trait_tmp(caster_ptr, TRAIT_PULVERISE, FALSE);
		break;
	case 6:
		do_active_trait_tmp(caster_ptr, TRAIT_STONE_SKIN, FALSE);
		do_active_trait_tmp(caster_ptr, TRAIT_MAGIC_RES_ELEMENT, FALSE);
		break;
	case 7:
		do_active_trait_tmp(caster_ptr, TRAIT_IDENTIFY, FALSE);
		break;
	case 8:
		do_active_trait_tmp(caster_ptr, TRAIT_MIND_WAVE, FALSE);
		break;

	case 9: /* Adrenaline */
		do_active_trait_tmp(caster_ptr, TRAIT_BECOME_HERO, FALSE);
		do_active_trait_tmp(caster_ptr, TRAIT_HASTE, FALSE);
		break;

	case 10: /* Telekinesis */
		do_active_trait(caster_ptr, TRAIT_TELEKINES, TRUE, lev_bonus * 15, 0L);
		break;

	case 11:
		do_active_trait(caster_ptr, TRAIT_PSI_DRAIN, TRUE, lev_bonus * 15, 0L);
		break;

	case 12:
		do_active_trait(caster_ptr, TRAIT_PSY_SPEAR, TRUE, lev_bonus * 15, 0L);
		break;

	case 13:
		do_active_trait_tmp(caster_ptr, TRAIT_WORLD, TRUE);
		break;

	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);

	}

	return TRUE;
}


/*
* do_cmd_cast calls this function if the player's class
* is 'ForceTrainer'.
*/
static bool cast_force_spell(creature_type *caster_ptr, int spell)
{
	int boost = caster_ptr->charged_force;

	if(heavy_armor(caster_ptr)) boost /= 2;

	switch (spell)
	{
	case 0:
		do_active_trait(caster_ptr, TRAIT_MISSILE, TRUE, 100, 0L);
		break;

	case 1:
		do_active_trait(caster_ptr, TRAIT_LIGHT_AREA, TRUE, 100, 0L);
		break;

	case 2:
		do_active_trait(caster_ptr, TRAIT_GET_LEVITATION, TRUE, 100, 0L);
		break;

	case 3:
		do_active_trait(caster_ptr, TRAIT_BO_MANA, TRUE, 100, 0L);
		break;

	case 4:
		do_active_trait(caster_ptr, TRAIT_GET_RESIST_MAGIC, TRUE, 100, 0L);
		break;

	case 5:
		do_active_trait(caster_ptr, TRAIT_IMPROVE_FORCE, TRUE, 100, 0L);
		break;

	case 6:
		do_active_trait(caster_ptr, TRAIT_GET_AURA_MANA, TRUE, 100, 0L);
		break;

	case 7:
		do_active_trait(caster_ptr, TRAIT_SHOCK_WAVE, TRUE, 100, 0L);
		break;

	case 8:
		do_active_trait(caster_ptr, TRAIT_BAZOOKA, TRUE, 100, 0L);
		break;

	case 9:
		do_active_trait(caster_ptr, TRAIT_DISPEL, TRUE, 100, 0L);
		break;

	case 10:
		do_active_trait(caster_ptr, TRAIT_S_PHANTOM, TRUE, 100, 0L);
		break;

	case 11:
		do_active_trait(caster_ptr, TRAIT_STORM_FIRE, TRUE, 100, 0L);
		break;

	case 12:
		do_active_trait(caster_ptr, TRAIT_BEAM_MANA, TRUE, 100, 0L);
		break;

	case 13:
		do_active_trait(caster_ptr, TRAIT_GET_LIGHT_SPEED, TRUE, 100, 0L);
		break;

	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);
	}
	caster_ptr->charged_force = 0;
	prepare_update(caster_ptr, CRU_BONUS);

	return TRUE;
}

// by henkma
// calculate mirrors
// Fixed by Deskull for D'angband
static int number_of_mirrors(creature_type *caster_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	int x, y;
	int val = 0;
	for(x = 0; x < floor_ptr->width ; x++ ){
		for(y = 0; y < floor_ptr->height ; y++ ){
			if(is_mirror_grid(&floor_ptr->cave[y][x])) val++;
		}
	}
	return val;
}

static bool cast_mirror_spell(creature_type *caster_ptr, int spell)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	DIRECTION dir;
	CREATURE_LEV lev_bonus = caster_ptr->lev;
	int tmp;
	COODINATES x, y;

	switch (spell)
	{
	case 0: /* mirror of seeing */
		tmp = is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]) ? 4 : 0;
		if( lev_bonus + tmp > 4) detect_creatures_normal(caster_ptr, DETECT_RAD_DEFAULT);
		if( lev_bonus + tmp > 18) detect_creatures_invis(caster_ptr, DETECT_RAD_DEFAULT);
		if( lev_bonus + tmp > 28) set_timed_trait(caster_ptr, TRAIT_ESP, lev_bonus,FALSE);
		if( lev_bonus + tmp > 38) map_area(caster_ptr, DETECT_RAD_MAP);
		if( tmp == 0 && lev_bonus < 5 ) msg_print(MES_PREVENT_NO_MIRROR);
		break;
	case 1: /* drip of light */
		if(number_of_mirrors(caster_ptr) < 4 + lev_bonus/10 ) place_mirror(caster_ptr);
		else msg_format(MES_TRAIT_MIRROR_SET_LIMIT);
		break;
	case 2:
		if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		if( lev_bonus > 9 && is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]) ) {
			cast_beam(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, diceroll(3+((lev_bonus-1)/5),4), 0);
		}
		else cast_bolt(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, diceroll(3+((lev_bonus-1)/5),4), 0);
		break;

	case 3: /* warped mirror */
		do_active_trait(caster_ptr, TRAIT_BLINK, TRUE, 100, 0L);
		break;

	case 4: /* mirror of light */
		do_active_trait(caster_ptr, TRAIT_LIGHT_AREA, TRUE, 100, 0L);
		break;

	case 5: /* mirror of wandering */
		do_active_trait(caster_ptr, TRAIT_ACTIVE_TELEPORT, TRUE, 100, 0L);
		break;

	case 6: /* robe of dust */
		do_active_trait(caster_ptr, TRAIT_GET_DUST_ROBE, TRUE, 100, 0L);
		break;

	case 7: /* banishing mirror */
		do_active_trait(caster_ptr, TRAIT_TELE_AWAY, TRUE, 100, 0L);
		break;

		/* mirror clashing */
	case 8:
		cast_ball(caster_ptr, DO_EFFECT_SHARDS, MAX_RANGE_SUB, (COODINATES)diceroll(8 + ((lev_bonus - 5) / 4), 8), (COODINATES)(lev_bonus > 20 ? (lev_bonus - 20) / 8 + 1 : 0));
		break;
		/* mirror sleeping */
	case 9:
		for(x=0;x<floor_ptr->width;x++){
			for(y=0;y<floor_ptr->height;y++){
				if(is_mirror_grid(&floor_ptr->cave[y][x])) {
					project(caster_ptr, 0,2,y,x,lev_bonus,DO_EFFECT_OLD_SLEEP,(PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP|PROJECT_NO_HANGEKI),-1);
				}
			}
		}
		break;
		/* seeker ray */
	case 10:
		do_active_trait(caster_ptr, TRAIT_SEEKER_RAY, TRUE, 100, 0L);
		break;
		/* seal of mirror */
	case 11:
		seal_of_mirror(caster_ptr, lev_bonus * 4 + 100);
		break;
		/* shield of water */
	case 12:
		tmp = 20 + randint1(20);
		set_timed_trait(caster_ptr, TRAIT_SHIELD, tmp, FALSE);
		if(lev_bonus > 31) set_timed_trait(caster_ptr, TRAIT_REFLECTING, tmp, FALSE);
		if(lev_bonus > 39) set_timed_trait(caster_ptr, TRAIT_RESIST_MAGIC, tmp, FALSE);
		break;
		/* super ray */
	case 13:
		cast_beam(caster_ptr, DO_EFFECT_SUPER_RAY, MAX_RANGE_SUB, 150+randint1(2*lev_bonus), 0);
		break;
		/* illusion light */
	case 14:
		tmp = is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]) ? 4 : 3;
		project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, caster_ptr->lev);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, lev_bonus*tmp);
		project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, lev_bonus*tmp);
		project_all_vision(caster_ptr, DO_EFFECT_STUN, lev_bonus*tmp);
		project_all_vision(caster_ptr, DO_EFFECT_STASIS, lev_bonus*tmp);
		break;
		/* mirror shift */
	case 15:
		if(!is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]))
		{
			msg_print(MES_TRAIT_MIRROR_WORLD_FAILED);
			break;
		}
		alter_reality(caster_ptr);
		break;

	case 16: /* mirror tunnel */
		msg_print(MES_TRAIT_MIRROR_TUNNEL_DONE);
		return mirror_tunnel(caster_ptr);

	case 17:
		return do_active_trait(caster_ptr, TRAIT_RECALL, TRUE, 100, 0L);

	case 18:
		return do_active_trait(caster_ptr, TRAIT_GET_MULTI_SHADOW, TRUE, 100, 0L);

	case 19:
		if(!binding_field(caster_ptr, MAX_RANGE, lev_bonus * 11 + 5)) msg_print(MES_TRAIT_MIRROR_BINDING_FAILED);
		break;

	case 20: /* mirror of Ruffnor */
		(void)set_timed_trait(caster_ptr, TRAIT_INVULNERABLE, randint1(4)+4,FALSE);
		break;

	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);
	}
	//TODO CHECK caster_ptr->magic_num1[0] = 0;

	return TRUE;
}


// do_cmd_cast calls this function if the player's class
// is 'berserker'.
static bool cast_berserk_spell(creature_type *caster_ptr, int spell)
{
	// spell code
	switch (spell)
	{
	case 0:
		do_active_trait(caster_ptr, TRAIT_DETECT_FELOCITY, TRUE, 100, 0L);
		break;
	case 1:
		do_active_trait(caster_ptr, TRAIT_TRAP_WALK, TRUE, 100, 0L);
		break;
	case 2:
		do_active_trait(caster_ptr, TRAIT_TRAP_WALK, TRUE, 100, 0L);
		break;
	case 3:
		do_active_trait_tmp(caster_ptr, TRAIT_EARTHQUAKE, TRUE);
		break;
	case 4:
		do_active_trait(caster_ptr, TRAIT_MASSACRE, TRUE, 100, 0L);
		break;
	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);
	}
	return TRUE;
}



// do_cmd_cast calls this function if the player's class
// is 'ninja'.
static bool cast_ninja_spell(creature_type *caster_ptr, int spell)
{
	DIRECTION dir;

	switch (spell)
	{
	case 0:
		do_active_trait_tmp(caster_ptr, TRAIT_DARKNESS, TRUE);
		break;
	case 1:
		do_active_trait_tmp(caster_ptr, TRAIT_DETECT_CREATURE, TRUE);
		break;
	case 2:
		do_active_trait_tmp(caster_ptr, TRAIT_BLINK, TRUE);
		break;
	case 3:
		do_active_trait_tmp(caster_ptr, TRAIT_GET_KAWARIMI, TRUE);
		break;
	case 4:
		do_active_trait_tmp(caster_ptr, TRAIT_ACTIVE_TELEPORT, TRUE);
		break;
	case 5:
		do_active_trait_tmp(caster_ptr, TRAIT_PANIC_HIT, TRUE);
		break;
	case 6:
		do_active_trait_tmp(caster_ptr, TRAIT_ARREST_CREATURE, TRUE);
		break;
	case 7:
		do_active_trait_tmp(caster_ptr, TRAIT_IDENTIFY, TRUE);
		break;
	case 8:
		do_active_trait_tmp(caster_ptr, TRAIT_GET_LEVITATION, TRUE);
		break;
	case 9:
		do_active_trait_tmp(caster_ptr, TRAIT_KATON, TRUE);
		break;
	case 10:
		do_active_trait_tmp(caster_ptr, TRAIT_RUSH_ATTACK, TRUE);
		break;
	case 11:
		do_active_trait_tmp(caster_ptr, TRAIT_SPREADING_THROW, TRUE);
		break;
	case 12:
		do_active_trait_tmp(caster_ptr, TRAIT_CHAIN_HOOK, TRUE);
		break;
	case 13:
		do_active_trait_tmp(caster_ptr, TRAIT_SMOKE_BALL, TRUE);
		break;
	case 14:
		do_active_trait_tmp(caster_ptr, TRAIT_SWAP_POS, TRUE);
		break;
	case 15:
		do_active_trait_tmp(caster_ptr, TRAIT_EXPLOSIVE_RUNE, TRUE);
		break;
	case 16:
		do_active_trait_tmp(caster_ptr, TRAIT_GET_PASS_WALL, TRUE);
		do_active_trait_tmp(caster_ptr, TRAIT_MAGIC_RES_ACID, TRUE);
		break;
	case 17:
		break;
	case 18:
		do_active_trait_tmp(caster_ptr, TRAIT_FIRE_SWARM, TRUE);
		break;
	case 19:
		do_active_trait(caster_ptr, TRAIT_GET_MULTI_SHADOW, TRUE, 100, 0L);
		break;
	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);
	}
	return TRUE;
}



/*
* do_cmd_cast calls this function if the player's class
* is 'mindcrafter'.
*/
void do_cmd_mind(creature_type *caster_ptr)
{
	floor_type      *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	KEY n = 0;
	int b = 0;
	PROB chance;
	int             lev_bonus = caster_ptr->lev;
	int             old_csp = caster_ptr->csp;
	mind_type       spell;
	bool            cast;
	int             use_mind, mana_cost;
#ifdef JP
	cptr            p;
#endif
	bool		on_mirror = FALSE;

	/* not if confused */
	if(has_trait(caster_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return;
	}

	/* get power */
	if(!get_mind_power(caster_ptr, &n, FALSE)) return;

	switch(caster_ptr->class_idx)
	{
	case CLASS_MINDCRAFTER:   use_mind = MIND_MINDCRAFTER; break;
	case CLASS_FORCETRAINER:  use_mind = MIND_KI; break;
	case CLASS_BERSERKER:     use_mind = MIND_BERSERKER; break;
	case CLASS_MIRROR_MASTER: use_mind = MIND_MIRROR_MASTER; break;
	case CLASS_NINJA:         use_mind = MIND_NINJUTSU; break;
	default:                  use_mind = 0;
	}
	spell = mind_powers[use_mind].info[n];
	p = SKILL_NAME_PSYONIC;

	/* Spell failure chance */
	chance = spell.fail;

	mana_cost = spell.mana_cost;
	if(use_mind == MIND_KI)
	{
		if(heavy_armor(caster_ptr)) chance += 20;
		else if(get_equipped_slot_num(caster_ptr, SLOT_ID_HAND) > 0) chance += 10;
		else if(get_equipped_slot_num(caster_ptr, SLOT_ID_HAND) > 1) chance += 10;
		if(n == 5)
		{
			int j;
			for (j = 0; j < caster_ptr->charged_force / 50; j++)
				mana_cost += (j+1) * 3 / 2;
		}
	}

	/* Verify "dangerous" spells */
	if((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU))
	{
		if(mana_cost > caster_ptr->chp)
		{
			msg_print(MES_PREVENT_BY_HP);
			return;
		}
	}
	else if(mana_cost > caster_ptr->csp)
	{
		msg_print(MES_PREVENT_BY_MP);
		if(!over_exert) return;
		if(!get_check(MES_ATTEMPT_ANYWAY)) return;
	}

	if(chance)
	{
		/* Reduce failure rate by "effective" level adjustment */
		chance -= 3 * (lev_bonus - spell.min_lev);
		chance += caster_ptr->to_m_chance;

		/* Not enough mana to cast */
		if((mana_cost > caster_ptr->csp) && (use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU))
			chance += 5 * (mana_cost - caster_ptr->csp);

		chance += calc_trait_difficulty(caster_ptr, 0, magic_info[caster_ptr->class_idx].spell_stat);

		if(use_mind == MIND_KI && heavy_armor(caster_ptr)) chance += 5;
	}

	/* Always a 5 percent chance of working */
	if(chance > MAX_CHANCE) chance = MAX_CHANCE;

	/* Failed spell */
	if(PROB_PERCENT(chance))
	{
		if(flush_failure) flush();
#ifdef JP
		msg_print("èWíÜÇ…é∏îsÇµÇΩÅI");
#else
		msg_print("You failed to concentrate hard enough!");
#endif
		sound(SOUND_FAIL);

		if((use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU))
		{
			if((use_mind == MIND_KI) && (n != 5) && caster_ptr->charged_force)
			{
				msg_print(MES_TRAIT_FORCE_GONEAWAY);
				caster_ptr->charged_force = 0;
			}

			if(randint1(100) < (chance / 2))
			{
				/* Backfire */
				b = randint1(100);

				if(use_mind == MIND_MINDCRAFTER){
					if(b < 5) lose_all_info(caster_ptr);
					else if(b < 15) add_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 5 + randint1(10), TRUE);
					else if(b < 45) add_timed_trait(caster_ptr, TRAIT_CONFUSED, randint1(8), TRUE);
					else if(b < 90) add_timed_trait(caster_ptr, TRAIT_STUN, randint1(8), TRUE);
					else
					{
						/* Mana storm */
						msg_format(MES_PSIONIC_FAILED_MANASTORM(p));
						/*TODO*/
						project(caster_ptr, 0, 2 + (COODINATES)lev_bonus / 10, caster_ptr->fy, caster_ptr->fx, (COODINATES)lev_bonus * 2,
							DO_EFFECT_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
						caster_ptr->csp = MAX(0, caster_ptr->csp - lev_bonus * MAX(1, lev_bonus / 10));
					}
				}
				if(use_mind == MIND_MIRROR_MASTER){
					if(b < 51)
					{
						/* Nothing has happen */
					}
					else if(b < 81)
					{
						msg_print(MES_PSIONIC_FAILED_TELEPORT);
						teleport_creature(caster_ptr, 10, TELEPORT_PASSIVE);
					}
					else if(b < 96)
					{
						msg_print(MES_PSIONIC_FAILED_HALLUCINATION);
						add_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 5 + randint1(10), TRUE);
					}
					else
					{
						msg_format(MES_PSIONIC_FAILED_MANASTORM(p));
						/*TODO*/
						project(caster_ptr, 0, 2 + (COODINATES)lev_bonus / 10, caster_ptr->fy, caster_ptr->fx, (COODINATES)lev_bonus * 2,
							DO_EFFECT_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
						caster_ptr->csp = MAX(0, caster_ptr->csp - lev_bonus * MAX(1, lev_bonus / 10));
					}
				}
			}
		}
	}
	else
	{
		sound(SOUND_ZAP);

		switch(use_mind)
		{
		case MIND_MINDCRAFTER:
			cast = cast_mindcrafter_spell(caster_ptr, n);
			break;
		case MIND_KI:
			cast = cast_force_spell(caster_ptr, n);
			break;
		case MIND_BERSERKER:
			cast = cast_berserk_spell(caster_ptr, n);
			break;
		case MIND_MIRROR_MASTER:
			if( is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]) )on_mirror = TRUE;
			cast = cast_mirror_spell(caster_ptr, n);
			break;
		case MIND_NINJUTSU:
			cast = cast_ninja_spell(caster_ptr, n);
			break;
		default:
#ifdef JP
			msg_format("ì‰ÇÃî\óÕ:%d, %d",use_mind, n);
#else
			msg_format("Mystery power:%d, %d",use_mind, n);
#endif
			return;
		}

		if(!cast) return;
	}



	/* teleport from mirror costs small energy */
	if(on_mirror && caster_ptr->class_idx == CLASS_MIRROR_MASTER && (n==3 || n==5 || n==7 || n==16))
		cost_tactical_energy(caster_ptr, 50);
	else cost_tactical_energy(caster_ptr, 100);

	if((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU))
	{
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, mana_cost, COD_HARD_CONCENTRATION, NULL, -1);
		prepare_redraw(PR_HP);
	}

	else if(mana_cost <= old_csp) // Sufficient mana
	{
		dec_mana(caster_ptr, mana_cost);

		if((use_mind == MIND_MINDCRAFTER) && (n == 13)) // THE WORLD
		{
			caster_ptr->csp = 0;
			caster_ptr->csp_frac = 0;
		}
	}

	/* Over-exert the player */
	else
	{
		int oops = mana_cost - old_csp;

		/* No mana left */
		if((caster_ptr->csp - mana_cost) < 0) caster_ptr->csp_frac = 0;
		caster_ptr->csp = MAX(0, caster_ptr->csp - mana_cost);

		msg_print(MES_CAST_FAINT);

		/* Hack -- Bypass free action */
		(void)add_timed_trait(caster_ptr, TRAIT_SLEPT, randint1(5 * oops + 1), TRUE);

		/* Damage WIS (possibly permanently) */
		if(PROB_PERCENT(50))
		{
			bool perm = (PROB_PERCENT(25));
			msg_print(MES_PSIONIC_SELF_DAMAGED);
			(void)dec_stat(caster_ptr, STAT_WIS, 15 + randint1(10), perm);
		}
	}

	prepare_window(PW_PLAYER);
	prepare_window(PW_SPELL);
}


/*
* do_cmd_cast calls this function if the player's class
* is 'mindcrafter'.
*/
void do_cmd_mind_browse(creature_type *caster_ptr)
{
	KEY n = 0;
	int j, line;
	char temp[62*5];
	int use_mind = 0;

	if(caster_ptr->class_idx == CLASS_MINDCRAFTER) use_mind = MIND_MINDCRAFTER;
	else if(caster_ptr->class_idx == CLASS_FORCETRAINER) use_mind = MIND_KI;
	else if(caster_ptr->class_idx == CLASS_BERSERKER) use_mind = MIND_BERSERKER;
	else if(caster_ptr->class_idx == CLASS_NINJA) use_mind = MIND_NINJUTSU;
	else if(caster_ptr->class_idx == CLASS_MIRROR_MASTER)
		use_mind = MIND_MIRROR_MASTER;

	screen_save();

	while(TRUE)
	{
		/* get power */
		if(!get_mind_power(caster_ptr, &n, TRUE))
		{
			screen_load();
			return;
		}

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(12, 21, 255);
		Term_erase(12, 20, 255);
		Term_erase(12, 19, 255);
		Term_erase(12, 18, 255);
		Term_erase(12, 17, 255);
		Term_erase(12, 16, 255);

		roff_to_buf(mind_tips[use_mind][n], 62, temp, sizeof(temp));
		for(j=0, line = 17;temp[j];j+=(1+strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
		switch (use_mind)
		{
		case MIND_MIRROR_MASTER:
		case MIND_NINJUTSU:
			prt(MES_SYS_HIT_ANY_KEY, 0, 0);
			(void)inkey();
		}
	}
}
