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
			{ 1, 1, 15, "霊視"},
			{ 2, 1, 20, "神経攻撃"},
			{ 3, 2, 25, "次元の瞬き"},
			{ 7, 6, 35, "虚空の幻影"},
			{ 9, 7, 50, "精神支配"},
			{ 11, 7, 30, "念動衝撃弾"},
			{ 13, 12, 50, "鎧化"},
			{ 15, 12, 60, "サイコメトリー"},
			{ 18, 10, 45, "精神波動"},
			{ 23, 15, 50, "アドレナリン・ドーピング"},
			{ 26, 28, 60, "テレキネシス"},
			{ 28, 10, 40, "サイキック・ドレイン"},
			{ 35, 35, 75, "光の剣"},
			{ 45,150, 85, "完全な世界"},
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
			{ 1, 1, 15, "小龍"},
			{ 3, 3, 30, "閃光"},
			{ 5, 6, 35, "舞空術"},
			{ 8, 5, 40, "カメハメ波"},
			{ 10, 7, 45, "対魔法防御"},
			{ 13, 5, 60, "練気"},
			{ 17, 17, 50, "纏闘気"},
			{ 20, 20, 50, "衝波"},
			{ 23, 18, 55, "彗龍"},
			{ 25, 30, 70, "いてつく波動"},
			{ 28, 26, 50, "幻霊召喚"},
			{ 32, 35, 65, "煉獄火炎"},
			{ 38, 42, 75, "超カメハメ波"},
			{ 44, 50, 80, "光速移動"},
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
			{  8, 5, 40, "殺気感知"},
			{ 15, 20, 0, "突撃"},
			{ 20, 15, 0, "トラップ粉砕"},
			{ 25, 20, 60, "地震"},
			{ 30, 80, 75, "皆殺し"},
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
			{ 1, 1, 15, "真見の鏡"},
			{ 1, 2, 40, "鏡生成"},
			{ 2, 2, 20, "光のしずく"},
			{ 3, 2, 20, "歪んだ鏡"},
			{ 5, 3, 35, "閃光鏡"},
			{ 6, 5, 35, "彷える鏡"},

			{ 10, 5, 30, "微塵隠れ"},
			{ 12, 12, 30, "追放の鏡"},
			{ 15, 15, 30, "鏡砕き"},
			{ 19, 13, 30, "催眠鏡"},
			{ 23, 18, 50, "シーカーレイ"},

			{ 25, 20, 40, "鏡の封印"},
			{ 27, 30, 60, "水鏡の盾"},
			{ 29, 30, 60, "スーパーレイ"},
			{ 31, 35, 60, "幻惑の光"},
			{ 33, 50, 80, "鏡の国"},

			{ 36, 30, 80, "鏡抜け"},
			{ 38, 40, 70, "帰還の鏡"},
			{ 40, 50, 55, "影分身"},
			{ 43, 55, 70, "封魔結界"},
			{ 46, 70, 75, "ラフノールの鏡"},
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
			{  1, 1, 20, "暗闇生成"},
			{  2, 2, 25, "周辺調査"},
			{  3, 3, 25, "葉隠れ"},
			{  5, 3, 30, "変わり身"},
			{  7, 8, 35, "高飛び"},
			{  8, 10, 35, "一撃離脱"},
			{ 10, 10, 40, "金縛り"},
			{ 12, 12, 70, "古の口伝"},
			{ 15, 10, 50, "浮雲"},
			{ 17, 12, 45, "火遁"},
			{ 18, 20, 40, "入身"},
			{ 20, 5, 50, "八方手裏剣"},
			{ 22, 15, 55, "鎖鎌"},
			{ 25, 32, 60, "煙玉"},
			{ 28, 32, 60, "転身"},
			{ 30, 30, 70, "爆発の紋章"},
			{ 32, 40, 40, "土遁"},
			{ 34, 35, 50, "霧隠れ"},
			{ 38, 40, 60, "煉獄火炎"},
			{ 41, 50, 55, "分身"},
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
bool psychometry(creature_type *creature_ptr)
{
	OBJECT_ID item;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];
	byte            feel;
	cptr            q, s;
	bool okay = FALSE;

#ifdef JP
	q = "どのアイテムを調べますか？";
	s = "調べるアイテムがありません。";
#else
	q = "Meditate on which item? ";
	s = "You have nothing appropriate.";
#endif

	if(!get_item(creature_ptr, &item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR), NULL, 0)) return FALSE;
	object_ptr = GET_ITEM(creature_ptr, item);

	/* It is fully known, no information needed */
	if(object_is_known(object_ptr))
	{
#ifdef JP
		msg_print("何も新しいことは判らなかった。");
#else
		msg_print("You cannot find out anything more about that.");
#endif

		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(creature_ptr, object_ptr);

	/* Get an object description */
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

	/* Skip non-feelings */
	if(!feel)
	{
#ifdef JP
		msg_format("%sからは特に変わった事は感じとれなかった。", object_name);
#else
		msg_format("You do not perceive anything unusual about the %s.", object_name);
#endif

		return TRUE;
	}

#ifdef JP
	msg_format("%sは%sという感じがする...", object_name, game_inscriptions[feel]);
#else
	msg_format("You feel that the %s %s %s...", object_name, ((object_ptr->number == 1) ? "is" : "are"), game_inscriptions[feel]);
#endif


	/* We have "felt" it */
	object_ptr->ident |= (IDENT_SENSE);

	/* "Inscribe" it */
	object_ptr->feeling = feel;

	/* Player touches it */
	object_ptr->marked |= OM_TOUCHED;

	/* Combine / Reorder the pack (later) */
	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);

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

	/* Auto-inscription/destroy */
	autopick_alter_item(creature_ptr, item, (bool)(okay && destroy_feeling));

	/* Something happened */
	return TRUE;
}




void mindcraft_info(creature_type *creature_ptr, char *p, int use_mind, POWER power)
{
	char buf[30];
	static const char s_dam[] = KW_DAM;
#ifdef JP
	cptr s_dur = "期間:";
	cptr s_range = "範囲:";
#else
	cptr s_dur = "dur ";
	cptr s_range = "range ";
#endif
	int lev_bonus = creature_ptr->lev;

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
		case 10: sprintf(p, " 最大重量:%s", buf);  break;
#else
		case 10: sprintf(p, " max wgt %s", buf);  break;
#endif
		case 11: sprintf(p, " %s%dd6", s_dam, lev_bonus / 2);  break;
		case 12: sprintf(p, " %sd%d+%d", s_dam, lev_bonus * 3, lev_bonus * 3); break;
#ifdef JP
		case 13: sprintf(p, " 行動:%ld回", (creature_ptr->csp + 100 - creature_ptr->energy_need - 50) / 100); break;
#else
		case 13: sprintf(p, " %ld acts.", (creature_ptr->csp + 100 - creature_ptr->energy_need - 50) / 100); break;
#endif
		}
		break;
	case MIND_KI:
		{
			int boost = creature_ptr->charged_force;

			if(heavy_armor(creature_ptr)) boost /= 2;

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
			case 10: sprintf(p, " 最大%d体", 1+boost/100); break;
#else
			case 10: sprintf(p, " max %d", 1+boost/100); break;
#endif
			case 11: sprintf(p, " %s%d", s_dam, 100 + lev_bonus + boost); break;
			case 12: sprintf(p, " %s%dd15", s_dam, 10 + lev_bonus / 2 + boost * 3 / 10); break;
#ifdef JP
			case 13: sprintf(p, " 行動:%d+d16回", 16+boost/20); break;
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
static int get_mind_power(creature_type *creature_ptr, KEY *sn, bool only_browse)
{
	KEY i;
	KEY num = 0;
	COODINATES y = 1;
	COODINATES x = 10;
	int             minfail = 0;
	CREATURE_LEV lev_bonus = creature_ptr->lev;
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

	switch(creature_ptr->class_idx)
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
		if(mind_ptr->info[i].min_lev <= lev_bonus)
		{
			num++;
		}
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
#ifdef JP
				put_str(format("Lv   %s   失率 効果", ((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU)) ? KW_HP : KW_MP), y, x + 35);
#else
				put_str(format("Lv   %s   Fail Info", ((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU)) ? KW_HP : KW_MP), y, x + 35);
#endif
				has_weapon[0] = get_equipped_slot_num(creature_ptr, INVENTORY_ID_HAND) > 0;
				has_weapon[1] = get_equipped_slot_num(creature_ptr, INVENTORY_ID_HAND) > 1;

				/* Dump the spells */
				for (i = 0; i < MAX_MIND_POWERS; i++)
				{
					int mana_cost;

					/* Access the spell */
					spell = mind_ptr->info[i];

					if(spell.min_lev > lev_bonus)   break;

					chance = spell.fail;

					mana_cost = spell.mana_cost;
					if(chance)
					{

						/* Reduce failure rate by "effective" level adjustment */
						chance -= 3 * (lev_bonus - spell.min_lev);

						/* Reduce failure rate by INT/WIS adjustment */
						chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] - 1);

						if(use_mind == MIND_KI)
						{
							if(heavy_armor(creature_ptr)) chance += 20;
							else if(has_weapon[0]) chance += 10;
							else if(has_weapon[1]) chance += 10;
							if(i == 5)
							{
								int j;
								for (j = 0; j < creature_ptr->charged_force / 50; j++)
									mana_cost += (j+1) * 3 / 2;
							}
						}

						/* Not enough mana to cast */
						if((use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU) && (mana_cost > creature_ptr->csp))
						{
							chance += 5 * (mana_cost - creature_ptr->csp);
						}

						chance += creature_ptr->to_m_chance;

						/* Extract the minimum failure rate */
						minfail = adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]];

						/* Minimum failure rate */
						if(chance < minfail) chance = minfail;

						/* Stunning makes spells harder */
						if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
						else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

						if(use_mind == MIND_KI)
						{
							if(heavy_armor(creature_ptr)) chance += 5;
						}
						/* Always a 5 percent chance of working */
						if(chance > MAX_CHANCE) chance = MAX_CHANCE;
					}

					/* Get info */
					mindcraft_info(creature_ptr, comment, use_mind, i);

					if(use_menu)
					{
						if(i == (menu_line-1)) strcpy(psi_desc, KET_D_ANGLE);
						else strcpy(psi_desc, "  ");
					}
					else
						sprintf(psi_desc, "  %c) ",I2A(i));
					/* Dump the spell --(-- */
					strcat(psi_desc,
						format("%-30s%2d %4d%s %3d%%%s",
						spell.name, spell.min_lev, mana_cost,
#ifdef JP
						(((use_mind == MIND_MINDCRAFTER) && (i == 13)) ? "～" : "  "), 
#else
						(((use_mind == MIND_MINDCRAFTER) && (i == 13)) ? "~ " : "  "), 
#endif
						chance, comment));
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			else if(!only_browse)
			{
				redraw = FALSE;

				screen_load();
			}

			/* Redo asking */
			continue;
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
			(void) strnfmt(tmp_val, 78, MES_SYS_ASK_USE, spell.name);
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


// do_cmd_cast calls this function if the player's class
// is 'mindcrafter'.

static bool cast_mindcrafter_spell(creature_type *creature_ptr, int spell)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int b = 0;
	DIRECTION dir;
	CREATURE_LEV lev_bonus = creature_ptr->lev;

	switch (spell)
	{
	case 0:   /* Precog */
		if(lev_bonus > 44)
		{
			wiz_lite(floor_ptr, creature_ptr, FALSE);
		}
		else if(lev_bonus > 19)
			map_area(creature_ptr, DETECT_RAD_MAP);

		if(lev_bonus < 30)
		{
			b = detect_creatures_normal(creature_ptr, DETECT_RAD_DEFAULT);
			if(lev_bonus > 14) b |= detect_creatures_invis(creature_ptr, DETECT_RAD_DEFAULT);
			if(lev_bonus > 4)  {
				b |= detect_traps(creature_ptr, DETECT_RAD_DEFAULT, TRUE);
				b |= detect_doors(creature_ptr, DETECT_RAD_DEFAULT);
			}
		}
		else
			b = detect_all(creature_ptr, DETECT_RAD_DEFAULT);

		if((lev_bonus > 24) && (lev_bonus < 40))
			set_timed_trait(creature_ptr, TRAIT_ESP, lev_bonus, FALSE);

#ifdef JP
		if(!b) msg_print("安全な気がする。");
#else
		if(!b) msg_print("You feel safe.");
#endif

		break;
	case 1:
		if(randint1(100) < lev_bonus * 2)
			cast_beam(creature_ptr, DO_EFFECT_PSI, MAX_RANGE_SUB, diceroll(3 + ((lev_bonus - 1) / 4), (3 + lev_bonus / 15)), 0);
		else
			cast_ball(creature_ptr, DO_EFFECT_PSI, MAX_RANGE_SUB, diceroll(3 + ((lev_bonus - 1) / 4), (3 + lev_bonus / 15)), 0);
		break;
	case 2:
		/* Minor displace */
		teleport_creature(creature_ptr, 10, 0L);
		break;
	case 3:
		/* Major displace */
		teleport_creature(creature_ptr, (COODINATES)lev_bonus * 5, 0L);
		break;
	case 4:
		/* Domination */
		if(lev_bonus < 30)
		{
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
			cast_ball(creature_ptr, DO_EFFECT_DOMINATION, dir, lev_bonus, 0);
		}
		else
			project_all_vision(creature_ptr, DO_EFFECT_CHARM, lev_bonus * 2);
		break;
	case 5:
		cast_ball(creature_ptr, DO_EFFECT_TELEKINESIS, MAX_RANGE_SUB, diceroll(8 + ((lev_bonus - 5) / 4), 8), (lev_bonus > 20 ? (lev_bonus - 20) / 8 + 1 : 0));
		break;
	case 6:
		/* Character Armour */
		set_timed_trait(creature_ptr, TRAIT_SHIELD, lev_bonus, FALSE);
		if(lev_bonus > 14) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ACID, lev_bonus, FALSE);
		if(lev_bonus > 19) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_FIRE, lev_bonus, FALSE);
		if(lev_bonus > 24) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_COLD, lev_bonus, FALSE);
		if(lev_bonus > 29) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ELEC, lev_bonus, FALSE);
		if(lev_bonus > 34) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_POIS, lev_bonus, FALSE);
		break;
	case 7:
		/* Psychometry */
		if(lev_bonus < 25)
			return psychometry(creature_ptr);
		else
			return ident_spell(creature_ptr, FALSE);
	case 8:
		/* Mindwave */
#ifdef JP
		msg_print("精神を捻じ曲げる波動を発生させた！");
#else
		msg_print("Mind-warping forces emanate from your brain!");
#endif

		if(lev_bonus < 25)
			project(creature_ptr, 0, 2 + lev_bonus / 10, creature_ptr->fy, creature_ptr->fx, (lev_bonus * 3), DO_EFFECT_PSI, PROJECT_KILL, -1);
		else
			(void)project_all_vision(creature_ptr, DO_EFFECT_PSI, randint1(lev_bonus * ((lev_bonus - 5) / 10 + 1)));
		break;
	case 9:
		/* Adrenaline */
		set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE);
		set_timed_trait(creature_ptr, TRAIT_STUN, 0, TRUE);

		/*
		* Only heal when Adrenalin Channeling is not active. We check
		* that by checking if the player isn't fast and 'heroed' atm.
		*/
		if(!has_trait(creature_ptr, TRAIT_FAST) || !has_trait(creature_ptr, TRAIT_HERO)) heal_creature(creature_ptr, lev_bonus);

		b = 10 + randint1((lev_bonus * 3) / 2);
		(void)set_timed_trait(creature_ptr, TRAIT_HERO, b, FALSE);
		(void)set_timed_trait(creature_ptr, TRAIT_FAST, b, TRUE);
		break;

	case 10:
		/* Telekinesis */
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

		fetch(creature_ptr, MAX_RANGE, dir, lev_bonus * 15, FALSE);

		break;
	case 11:
		/* Psychic Drain */
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

		b = diceroll(lev_bonus / 2, 6);

		/* This is always a radius-0 ball now */
		if(cast_ball(creature_ptr, DO_EFFECT_PSI_DRAIN, dir, b, 0)) cost_tactical_energy(creature_ptr, randint1(150));
		break;
	case 12:
		cast_beam(creature_ptr, DO_EFFECT_PSY_SPEAR, MAX_RANGE_SUB, randint1(lev_bonus * 3) + lev_bonus * 3, 0);
		break;
	case 13:
			if(creature_ptr->time_stopper)
			{
#ifdef JP
				msg_print("既に時は止まっている。");
#else
				msg_print("Time is already stopped.");
#endif
				return FALSE;
			}
			creature_ptr->time_stopper = TRUE;
#ifdef JP
			msg_print("「時よ！」");
#else
			msg_print("You yell 'Time!'");
#endif
			msg_print(NULL);

			/* Hack */
			creature_ptr->energy_need -= 1000 + (100 + (s16b)creature_ptr->csp - 50)*TURNS_PER_TICK/10;

			prepare_redraw(PR_MAP);
			prepare_update(creature_ptr, PU_CREATURES);
			prepare_window(PW_OVERHEAD | PW_DUNGEON);
			handle_stuff(creature_ptr);
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
static bool cast_force_spell(creature_type *creature_ptr, int spell)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	DIRECTION dir;
	FLOOR_LEV lev_bonus = creature_ptr->lev;
	int boost = creature_ptr->charged_force;

	if(heavy_armor(creature_ptr)) boost /= 2;

	switch (spell)
	{
	case 0:
		cast_ball(creature_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, diceroll(3 + ((lev_bonus - 1) / 5) + boost / 12, 4), 0);
		break;
	case 1:
		(void)lite_area(creature_ptr, diceroll(2, (COODINATES)(lev_bonus / 2)), (COODINATES)(lev_bonus / 10) + 1);
		break;
	case 2:
		set_timed_trait(creature_ptr, TRAIT_LEVITATION, randint1(30) + 30 + boost / 5, FALSE);
		break;
	case 3:
		if(!get_aim_dir(creature_ptr, (COODINATES)lev_bonus / 8 + 3, &dir)) return FALSE;
		cast_beam(creature_ptr, DO_EFFECT_MISSILE, lev_bonus / 8 + 3, diceroll(5 + ((lev_bonus - 1) / 5) + boost / 10, 5), 0);
		break;
	case 4:
		set_timed_trait(creature_ptr, TRAIT_RESIST_MAGIC, randint1(20) + 20 + boost / 5, FALSE);
		break;
	case 5:
#ifdef JP
		msg_print("気を練った。");
#else
		msg_print("You improved the Force.");
#endif
		creature_ptr->charged_force += (70 + lev_bonus);
		prepare_update(creature_ptr, CRU_BONUS);
		if(randint1(creature_ptr->charged_force) > (lev_bonus * 4 + 120))
		{
#ifdef JP
			msg_print("気が暴走した！");
#else
			msg_print("The Force exploded!");
#endif
			cast_ball(creature_ptr, DO_EFFECT_MANA, 0, creature_ptr->charged_force / 2, 10);
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, creature_ptr->charged_force / 2, COD_UNC_FORCE, NULL, -1);
		}
		else return TRUE;
		break;
	case 6:
		set_timed_trait(creature_ptr, TRAIT_AURA_MANA, randint1(lev_bonus / 2) + 15 + boost / 7, FALSE);
		break;
	case 7:
		{
			COODINATES y, x;
			POWER dam;

			if(!get_aim_dir(creature_ptr, 1, &dir)) return FALSE;
			y = creature_ptr->fy + ddy[dir];
			x = creature_ptr->fx + ddx[dir];
			dam = diceroll(8 + ((lev_bonus - 5) / 4) + boost / 12, 8);
			cast_beam(creature_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, dam, 0);
			if(floor_ptr->cave[y][x].creature_idx)
			{
				int i;
				COODINATES ty = y, tx = x;
				COODINATES oy = y, ox = x;
				CREATURE_ID creature_idx = floor_ptr->cave[y][x].creature_idx;
				creature_type *m_ptr = &creature_list[creature_idx];
				species_type *species_ptr = &species_info[m_ptr->species_idx];
				char m_name[MAX_NLEN];

				creature_desc(m_name, m_ptr, 0);

				if(randint1(species_ptr->level * 3 / 2) > randint0(dam / 2) + dam/2)
				{
#ifdef JP
					msg_format("%sは飛ばされなかった。", m_name);
#else
					msg_format("%^s was not blown away.", m_name);
#endif
				}
				else
				{
					for (i = 0; i < 5; i++)
					{
						y += ddy[dir];
						x += ddx[dir];
						if(cave_empty_bold(floor_ptr, y, x))
						{
							ty = y;
							tx = x;
						}
						else break;
					}
					if((ty != oy) || (tx != ox))
					{
						msg_format(MES_BLOE_AWAY, m_name);
						floor_ptr->cave[oy][ox].creature_idx = 0;
						floor_ptr->cave[ty][tx].creature_idx = creature_idx;
						m_ptr->fy = ty;
						m_ptr->fx = tx;

						update_creature_view(player_ptr, creature_idx, TRUE);
						lite_spot(floor_ptr, oy, ox);
						lite_spot(floor_ptr, ty, tx);

						if(is_lighting_creature(m_ptr) || is_darken_creature(m_ptr))
							prepare_update(creature_ptr, PU_SPECIES_LITE);
					}
				}
			}
			break;
		}
	case 8:
		cast_ball(creature_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, diceroll(10, 6) + lev_bonus * 3 / 2 + boost * 3 / 5, (lev_bonus < 30) ? 2 : 3);
		break;
	case 9:
		{
			CREATURE_ID creature_idx;

			if(!target_set(creature_ptr, 0, TARGET_KILL)) return FALSE;
			creature_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!creature_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, MAX_RANGE, creature_ptr->fy, creature_ptr->fx, target_row, target_col)) break;
			dispel_creature(creature_ptr);
			break;
		}
	case 10:
		{
			int i;
			bool success = FALSE;

			for (i = 0; i < 1 + boost/100; i++)
				if(summoning(NULL, creature_ptr->fy, creature_ptr->fx, lev_bonus, TRAIT_S_PHANTOM, PC_FORCE_PET))
					success = TRUE;
			if(success) msg_print(MES_SUMMON_SERVANT);
			else msg_print(MES_NO_HAPPEN);
			break;
		}
	case 11:
		SELF_FIELD(creature_ptr, DO_EFFECT_FIRE, 200 + (2 * lev_bonus) + boost * 2, 10, -1);
		break;
	case 12:
		cast_beam(creature_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, diceroll(10 + (lev_bonus / 2) + boost * 3 / 10, 15), 0);
		break;
	case 13:
		set_timed_trait(creature_ptr, TRAIT_LIGHT_SPEED, randint1(16) + 16 + boost / 20, FALSE);
		break;
	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);
	}
	creature_ptr->charged_force = 0;
	prepare_update(creature_ptr, CRU_BONUS);

	return TRUE;
}

// by henkma
// calculate mirrors
// Fixed by Deskull for D'angband
static int number_of_mirrors(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int x, y;
	int val = 0;
	for(x = 0; x < floor_ptr->width ; x++ ){
		for(y = 0; y < floor_ptr->height ; y++ ){
			if(is_mirror_grid(&floor_ptr->cave[y][x])) val++;
		}
	}
	return val;
}

static bool cast_mirror_spell(creature_type *creature_ptr, int spell)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	DIRECTION dir;
	CREATURE_LEV lev_bonus = creature_ptr->lev;
	int tmp;
	COODINATES x, y;

	switch (spell)
	{
		/* mirror of seeing */
	case 0:
		tmp = is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) ? 4 : 0;
		if( lev_bonus + tmp > 4) detect_creatures_normal(creature_ptr, DETECT_RAD_DEFAULT);
		if( lev_bonus + tmp > 18) detect_creatures_invis(creature_ptr, DETECT_RAD_DEFAULT);
		if( lev_bonus + tmp > 28) set_timed_trait(creature_ptr, TRAIT_ESP, lev_bonus,FALSE);
		if( lev_bonus + tmp > 38) map_area(creature_ptr, DETECT_RAD_MAP);
		if( tmp == 0 && lev_bonus < 5 ){
#ifdef JP
			msg_print("鏡がなくて集中できなかった！");
#else
			msg_print("You need a mirror to concentrate!");
#endif
		}
		break;
		// drip of light
	case 1:
		if(number_of_mirrors(creature_ptr) < 4 + lev_bonus/10 ){
			place_mirror(creature_ptr);
		}
		else {
#ifdef JP
			msg_format("これ以上鏡は制御できない！");
#else
			msg_format("There are too many mirrors to control!");
#endif
		}
		break;
	case 2:
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		if( lev_bonus > 9 && is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) ) {
			cast_beam(creature_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, diceroll(3+((lev_bonus-1)/5),4), 0);
		}
		else {
			cast_bolt(creature_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, diceroll(3+((lev_bonus-1)/5),4), 0);
		}
		break;
		/* warped mirror */
	case 3:
		teleport_creature(creature_ptr, 10, 0L);
		break;
		/* mirror of light */
	case 4:
		(void)lite_area(creature_ptr, diceroll(2, (COODINATES)(lev_bonus / 2)), (COODINATES)(lev_bonus / 10) + 1);
		break;
		/* mirror of wandering */
	case 5:
		teleport_creature(creature_ptr, (COODINATES)lev_bonus * 5, 0L);
		break;
		/* robe of dust */
	case 6:
		set_timed_trait(creature_ptr, TRAIT_DUST_ROBE, 20 + randint1(20), FALSE);
		break;
		/* banishing mirror */
	case 7:
		(void)cast_beam(creature_ptr, DO_EFFECT_AWAY_ALL, MAX_RANGE_SUB, lev_bonus, 0);
		break;
		/* mirror clashing */
	case 8:
		cast_ball(creature_ptr, DO_EFFECT_SHARDS, MAX_RANGE_SUB, (COODINATES)diceroll(8 + ((lev_bonus - 5) / 4), 8), (COODINATES)(lev_bonus > 20 ? (lev_bonus - 20) / 8 + 1 : 0));
		break;
		/* mirror sleeping */
	case 9:
		for(x=0;x<floor_ptr->width;x++){
			for(y=0;y<floor_ptr->height;y++){
				if(is_mirror_grid(&floor_ptr->cave[y][x])) {
					project(creature_ptr, 0,2,y,x,lev_bonus,DO_EFFECT_OLD_SLEEP,(PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP|PROJECT_NO_HANGEKI),-1);
				}
			}
		}
		break;
		/* seeker ray */
	case 10:
		cast_beam(creature_ptr, DO_EFFECT_SEEKER, MAX_RANGE_SUB, diceroll(11+(lev_bonus-5)/4,8), 0);
		break;
		/* seal of mirror */
	case 11:
		seal_of_mirror(creature_ptr, lev_bonus * 4 + 100);
		break;
		/* shield of water */
	case 12:
		tmp = 20+randint1(20);
		set_timed_trait(creature_ptr, TRAIT_SHIELD, tmp, FALSE);
		if(lev_bonus > 31) set_timed_trait(creature_ptr, TRAIT_REFLECTING, tmp, FALSE);
		if(lev_bonus > 39) set_timed_trait(creature_ptr, TRAIT_RESIST_MAGIC, tmp, FALSE);
		break;
		/* super ray */
	case 13:
		cast_beam(creature_ptr, DO_EFFECT_SUPER_RAY, MAX_RANGE_SUB, 150+randint1(2*lev_bonus), 0);
		break;
		/* illusion light */
	case 14:
		tmp = is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) ? 4 : 3;
		project_all_vision(creature_ptr, DO_EFFECT_SLOW_OTHERS, creature_ptr->lev);
		project_all_vision(creature_ptr, DO_EFFECT_TURN_ALL, lev_bonus*tmp);
		project_all_vision(creature_ptr, DO_EFFECT_CONF_OTHERS, lev_bonus*tmp);
		project_all_vision(creature_ptr, DO_EFFECT_STUN, lev_bonus*tmp);
		project_all_vision(creature_ptr, DO_EFFECT_STASIS, lev_bonus*tmp);
		break;
		/* mirror shift */
	case 15:
		if( !is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) ){
#ifdef JP
			msg_print("鏡の国の場所がわからない！");
#else
			msg_print("You cannot find out where is the world of mirror!");
#endif
			break;
		}
		alter_reality(creature_ptr);
		break;
		/* mirror tunnel */
	case 16:
#ifdef JP
		msg_print("鏡の世界を通り抜け…  ");
#else
		msg_print("Go through the world of mirror...");
#endif
		return mirror_tunnel(creature_ptr);

	case 17:
		return word_of_recall(creature_ptr, randint0(21) + 15);

	case 18:
		set_timed_trait(creature_ptr, TRAIT_MULTI_SHADOW, 6+randint1(6), FALSE);
		break;

	case 19:
#ifdef JP
		if(!binding_field(creature_ptr, MAX_RANGE, lev_bonus*11+5)) msg_print("適当な鏡を選べなかった！");
#else
		if(!binding_field(creature_ptr, MAX_RANGE, lev_bonus*11+5)) msg_print("You were not able to choose suitable mirrors!");
#endif
		break;
		/* mirror of Ruffnor */
	case 20:
		(void)set_timed_trait(creature_ptr, TRAIT_INVULNERABLE, randint1(4)+4,FALSE);
		break;
	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);
	}
	//TODO CHECK creature_ptr->magic_num1[0] = 0;

	return TRUE;
}


// do_cmd_cast calls this function if the player's class
// is 'berserker'.
static bool cast_berserk_spell(creature_type *creature_ptr, int spell)
{
	COODINATES y, x;
	DIRECTION dir;

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// spell code
	switch (spell)
	{
	case 0:
		detect_creatures_mind(creature_ptr, DETECT_RAD_DEFAULT);
		break;
	case 1:
		{
			if(creature_ptr->riding)
			{
				msg_print(MES_PREVENT_BY_RIDING);
				return FALSE;
			}

			if(!get_rep_dir2(creature_ptr, &dir)) return FALSE;

			if(dir == 5) return FALSE;
			y = creature_ptr->fy + ddy[dir];
			x = creature_ptr->fx + ddx[dir];

			if(!floor_ptr->cave[y][x].creature_idx)
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				return FALSE;
			}

			close_combat(creature_ptr, y, x, 0);

			if(!creature_can_cross_terrain(creature_ptr, floor_ptr->cave[y][x].feat, 0) || is_trap(floor_ptr->cave[y][x].feat))
				break;

			y += ddy[dir];
			x += ddx[dir];

			if(creature_can_cross_terrain(creature_ptr, floor_ptr->cave[y][x].feat, 0) && !is_trap(floor_ptr->cave[y][x].feat) && !floor_ptr->cave[y][x].creature_idx)
			{
				msg_print(NULL);

				/* Move the player */
				(void)move_creature(creature_ptr, NULL, y, x, MCE_FORGET_FLOW | MCE_HANDLE_STUFF | MCE_DONT_PICKUP);
			}
			break;
		}
	case 2:
		{
			if(!get_rep_dir2(creature_ptr, &dir)) return FALSE;
			y = creature_ptr->fy + ddy[dir];
			x = creature_ptr->fx + ddx[dir];
			walk_creature(creature_ptr, dir, easy_disarm, TRUE);
			break;
		}
	case 3:
		earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 8 + (COODINATES)randint0(5));
		break;
	case 4:
		{
			cave_type       *c_ptr;
			creature_type    *m_ptr;

			for (dir = 0; dir < 8; dir++)
			{
				y = creature_ptr->fy + ddy_ddd[dir];
				x = creature_ptr->fx + ddx_ddd[dir];
				c_ptr = &floor_ptr->cave[y][x];

				/* Get the creature */
				m_ptr = &creature_list[c_ptr->creature_idx];

				/* Hack -- attack creatures */
				if(c_ptr->creature_idx && (m_ptr->see_others || CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)))
					close_combat(creature_ptr, y, x, 0);
			}
			break;
		}
	default:
		msg_warning(MES_SYS_OUT_OF_SWITCH);
	}
	return TRUE;
}



// do_cmd_cast calls this function if the player's class
// is 'ninja'.
static bool cast_ninja_spell(creature_type *caster_ptr, int spell)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	COODINATES x, y;
	DIRECTION dir;

	switch (spell)
	{
	case 0:
		(void)unlite_area(caster_ptr, 0, 3);
		break;
	case 1:
		if(caster_ptr->lev > 44)
		{
			wiz_lite(floor_ptr, caster_ptr, TRUE);
		}
		detect_creatures_normal(caster_ptr, DETECT_RAD_DEFAULT);
		if(caster_ptr->lev > 4)
		{
			detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
			detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
			detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);
		}
		if(caster_ptr->lev > 14)
		{
			detect_objects_normal(caster_ptr, DETECT_RAD_DEFAULT);
		}
		break;
	case 2:
		{
			teleport_creature(caster_ptr, 10, 0L);
			break;
		}
	case 3:
		{
			if(!(caster_ptr->posture & NINJA_KAWARIMI))
			{
#ifdef JP
				msg_print("敵の攻撃に対して敏感になった。");
#else
				msg_print("You are now prepare to evade any attacks.");
#endif

				caster_ptr->posture |= NINJA_KAWARIMI;
				prepare_redraw(PR_STATUS);
			}
			break;
		}
	case 4:
		{
			teleport_creature(caster_ptr, caster_ptr->lev * 5, 0L);
			break;
		}
	case 5:
		{
			if(!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;
			y = caster_ptr->fy + ddy[dir];
			x = caster_ptr->fx + ddx[dir];
			if(floor_ptr->cave[y][x].creature_idx)
			{
				close_combat(caster_ptr, y, x, 0);
				if(randint0(caster_ptr->skill_dis) < 7) msg_print(MES_FAILED_RUNAWAY);
				else
				{
					teleport_creature(caster_ptr, 30, 0L);
				}
			}
			else
			{
				msg_print(MES_NO_DICRECTION_CREATURE);
				msg_print(NULL);
			}
			break;
		}
	case 6:
		(void)cast_ball_hide(caster_ptr, DO_EFFECT_STASIS, MAX_RANGE_SUB, caster_ptr->lev*2, 0);
		break;
	case 7:
		return ident_spell(caster_ptr, FALSE);
	case 8:
		set_timed_trait(caster_ptr, TRAIT_LEVITATION, randint1(20) + 20, FALSE);
		break;
	case 9:
		SELF_FIELD(caster_ptr, DO_EFFECT_FIRE, 50 + caster_ptr->lev, caster_ptr->lev / 10 + 2, -1);
		teleport_creature(caster_ptr, 30, 0L);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, caster_ptr->lev, FALSE);
		break;
	case 10:
		return rush_attack(caster_ptr, NULL);
	case 11:
		{
			int i;
			for (i = 0; i < 8; i++)
			{
				int slot;

				for (slot = 0; slot < INVEN_TOTAL; slot++)
				{
					if(caster_ptr->inventory[slot].tval == TV_SPIKE) break;
				}
				if(slot == INVEN_TOTAL)
				{
#ifdef JP
					if(!i) msg_print("くさびを持っていない。");
					else msg_print("くさびがなくなった。");
#else
					if(!i) msg_print("You have no Iron Spikes.");
					else msg_print("You have no more Iron Spikes.");
#endif
					return FALSE;
				}

				/* Gives a multiplier of 2 at first, up to 3 at 40th */
				do_cmd_throw_aux(caster_ptr, 1, FALSE, slot);

				cost_tactical_energy(caster_ptr, 100);
			}
			break;
		}
	case 12:
		{
			creature_type *m_ptr;
			CREATURE_ID creature_idx;
			char m_name[MAX_NLEN];
			int i;
			int path_n;
			COODINATES path_g[512];
			COODINATES ty, tx;

			if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
			creature_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!creature_idx) break;
			if(creature_idx == caster_ptr->riding) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			m_ptr = &creature_list[creature_idx];
			creature_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%sを引き戻した。", m_name);
#else
			msg_format("You pull back %s.", m_name);
#endif

			path_n = project_path(path_g, MAX_RANGE, floor_ptr, target_row, target_col, caster_ptr->fy, caster_ptr->fx, 0);
			ty = target_row, tx = target_col;
			for (i = 1; i < path_n; i++)
			{
				COODINATES ny = GRID_Y(path_g[i]);
				COODINATES nx = GRID_X(path_g[i]);
				cave_type *c_ptr = &floor_ptr->cave[ny][nx];

				if(IN_BOUNDS(floor_ptr, ny, nx) && cave_empty_bold(floor_ptr, ny, nx) &&
					!(c_ptr->info & CAVE_OBJECT) && !pattern_tile(floor_ptr, ny, nx))
				{
					ty = ny;
					tx = nx;
				}
			}
			/* Update the old location */
			floor_ptr->cave[target_row][target_col].creature_idx = 0;

			/* Update the new location */
			floor_ptr->cave[ty][tx].creature_idx = creature_idx;

			/* Move the creature */
			m_ptr->fy = ty;
			m_ptr->fx = tx;

			/* Wake the creature up */
			(void)set_timed_trait(m_ptr, TRAIT_PARALYZED, 0, TRUE);

			/* Update the creature (new location) */
			update_creature_view(player_ptr, creature_idx, TRUE);
			lite_spot(floor_ptr, target_row, target_col);
			lite_spot(floor_ptr, ty, tx);

			if(is_lighting_creature(m_ptr) || is_darken_creature(m_ptr))
				prepare_update(caster_ptr, PU_SPECIES_LITE);

			if(m_ptr->see_others)
			{
				/* Auto-Recall if possible and visible */
				if(!has_trait(caster_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

				/* Track a new creature */
				health_track(creature_idx);
			}

			break;
		}
	case 13:
		cast_ball(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, caster_ptr->lev*3, 3);
		break;
	case 14:
		if(!get_aim_dir(caster_ptr, NO_RANGE_LIMIT, &dir)) return FALSE;
		(void)teleport_swap(caster_ptr, dir);
		break;
	case 15:
		explosive_rune(caster_ptr);
		break;
	case 16:
		(void)set_timed_trait(caster_ptr, TRAIT_PASS_WALL, randint1(caster_ptr->lev/2) + caster_ptr->lev/2, FALSE);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, caster_ptr->lev, FALSE);
		break;
	case 17:
		SELF_FIELD(caster_ptr, DO_EFFECT_POIS, 75 + caster_ptr->lev * 2 / 3, caster_ptr->lev / 5 + 2, -1);
		SELF_FIELD(caster_ptr, DO_EFFECT_OLD_DRAIN, 75 + caster_ptr->lev * 2 / 3, caster_ptr->lev / 5 + 2, -1);
		SELF_FIELD(caster_ptr, DO_EFFECT_CONFUSION, 75 + caster_ptr->lev * 2 / 3, caster_ptr->lev / 5 + 2, -1);
		teleport_creature(caster_ptr, 30, 0L);
		break;
	case 18:
		{
			int k;
			int num = diceroll(3, 9);

			for (k = 0; k < num; k++)
			{
				int typ = one_in_(2) ? DO_EFFECT_FIRE : one_in_(3) ? DO_EFFECT_NETHER : DO_EFFECT_PLASMA;
				int attempts = 1000;

				while (attempts--)
				{
					scatter(floor_ptr, &y, &x, caster_ptr->fy, caster_ptr->fx, 4, 0);
					if(!CREATURE_BOLD(caster_ptr, y, x)) break;
				}
				project(caster_ptr, 0, 0, y, x, diceroll(6 + caster_ptr->lev / 8, 10), typ, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
			}
			break;
		}
	case 19:
		set_timed_trait(caster_ptr, TRAIT_MULTI_SHADOW, 6+randint1(6), FALSE);
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
void do_cmd_mind(creature_type *creature_ptr)
{
	floor_type      *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	KEY n = 0;
	int b = 0;
	int             chance;
	int             minfail = 0;
	int             lev_bonus = creature_ptr->lev;
	int             old_csp = creature_ptr->csp;
	mind_type       spell;
	bool            cast;
	int             use_mind, mana_cost;
#ifdef JP
	cptr            p;
#endif
	bool		on_mirror = FALSE;

	/* not if confused */
	if(has_trait(creature_ptr, TRAIT_CONFUSED))
	{
		msg_print(MES_PREVENT_BY_CONFUSION);
		return;
	}

	/* get power */
	if(!get_mind_power(creature_ptr, &n, FALSE)) return;

	switch(creature_ptr->class_idx)
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
		if(heavy_armor(creature_ptr)) chance += 20;
		else if(get_equipped_slot_num(creature_ptr, INVENTORY_ID_HAND) > 0) chance += 10;
		else if(get_equipped_slot_num(creature_ptr, INVENTORY_ID_HAND) > 1) chance += 10;
		if(n == 5)
		{
			int j;
			for (j = 0; j < creature_ptr->charged_force / 50; j++)
				mana_cost += (j+1) * 3 / 2;
		}
	}

	/* Verify "dangerous" spells */
	if((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU))
	{
		if(mana_cost > creature_ptr->chp)
		{
			msg_print(MES_PREVENT_BY_HP);
			return;
		}
	}
	else if(mana_cost > creature_ptr->csp)
	{
		msg_print(MES_PREVENT_BY_MP);
		if(!over_exert) return;
		if(!get_check(MES_ATTEMPT_ANYWAY)) return;
	}

	if(chance)
	{
		/* Reduce failure rate by "effective" level adjustment */
		chance -= 3 * (lev_bonus - spell.min_lev);
		chance += creature_ptr->to_m_chance;

		/* Reduce failure rate by INT/WIS adjustment */
		chance -= 3 * (adj_mag_stat[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] - 1);

		/* Not enough mana to cast */
		if((mana_cost > creature_ptr->csp) && (use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU))
			chance += 5 * (mana_cost - creature_ptr->csp);

		/* Extract the minimum failure rate */
		minfail = adj_mag_fail[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]];

		/* Minimum failure rate */
		if(chance < minfail) chance = minfail;

		/* Stunning makes spells harder */
		if(creature_ptr->timed_trait[TRAIT_STUN] > 50) chance += 25;
		else if(has_trait(creature_ptr, TRAIT_STUN)) chance += 15;

		if(use_mind == MIND_KI && heavy_armor(creature_ptr)) chance += 5;
	}

	/* Always a 5 percent chance of working */
	if(chance > MAX_CHANCE) chance = MAX_CHANCE;

	/* Failed spell */
	if(PERCENT(chance))
	{
		if(flush_failure) flush();
#ifdef JP
		msg_format("%sの集中に失敗した！",p);
#else
		msg_format("You failed to concentrate hard enough!");
#endif
		sound(SOUND_FAIL);

		if((use_mind != MIND_BERSERKER) && (use_mind != MIND_NINJUTSU))
		{
			if((use_mind == MIND_KI) && (n != 5) && creature_ptr->charged_force)
			{
#ifdef JP
				msg_print("気が散ってしまった．．．");
#else
				msg_print("Your improved Force has gone away...");
#endif
				creature_ptr->charged_force = 0;
			}

			if(randint1(100) < (chance / 2))
			{
				/* Backfire */
				b = randint1(100);

				if( use_mind == MIND_MINDCRAFTER ){
					if(b < 5) lose_all_info(creature_ptr);
					else if(b < 15) add_timed_trait(creature_ptr, TRAIT_HALLUCINATION, 5 + randint1(10), TRUE);
					else if(b < 45) add_timed_trait(creature_ptr, TRAIT_CONFUSED, randint1(8), TRUE);
					else if(b < 90) add_timed_trait(creature_ptr, TRAIT_STUN, randint1(8), TRUE);
					else
					{
						/* Mana storm */
#ifdef JP
						msg_format("%sの力が制御できない氾流となって解放された！", p);
#else
						msg_print("Your mind unleashes its power in an uncontrollable storm!");
#endif
						/*TODO*/
						project(creature_ptr, 0, 2 + (COODINATES)lev_bonus / 10, creature_ptr->fy, creature_ptr->fx, (COODINATES)lev_bonus * 2,
							DO_EFFECT_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
						creature_ptr->csp = MAX(0, creature_ptr->csp - lev_bonus * MAX(1, lev_bonus / 10));
					}
				}
				if( use_mind == MIND_MIRROR_MASTER ){
					if(b < 51)
					{
						/* Nothing has happen */
					}
					else if(b < 81)
					{
#ifdef JP
						msg_print("鏡の世界の干渉を受けた！");
#else
						msg_print("Weird visions seem to dance before your eyes...");
#endif
						teleport_creature(creature_ptr, 10, TELEPORT_PASSIVE);
					}
					else if(b < 96)
					{
#ifdef JP
						msg_print("まわりのものがキラキラ輝いている！");
#else
						msg_print("Your brain is addled!");
#endif

						add_timed_trait(creature_ptr, TRAIT_HALLUCINATION, 5 + randint1(10), TRUE);
					}
					else
					{
						/* Mana storm */
#ifdef JP
						msg_format("%sの力が制御できない氾流となって解放された！", p);
#else
						msg_print("Your mind unleashes its power in an uncontrollable storm!");
#endif

						/*TODO*/
						project(creature_ptr, 0, 2 + (COODINATES)lev_bonus / 10, creature_ptr->fy, creature_ptr->fx, (COODINATES)lev_bonus * 2,
							DO_EFFECT_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
						creature_ptr->csp = MAX(0, creature_ptr->csp - lev_bonus * MAX(1, lev_bonus / 10));
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
			cast = cast_mindcrafter_spell(creature_ptr, n);
			break;
		case MIND_KI:
			cast = cast_force_spell(creature_ptr, n);
			break;
		case MIND_BERSERKER:
			cast = cast_berserk_spell(creature_ptr, n);
			break;
		case MIND_MIRROR_MASTER:
			if( is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) )on_mirror = TRUE;
			cast = cast_mirror_spell(creature_ptr, n);
			break;
		case MIND_NINJUTSU:
			cast = cast_ninja_spell(creature_ptr, n);
			break;
		default:
#ifdef JP
			msg_format("謎の能力:%d, %d",use_mind, n);
#else
			msg_format("Mystery power:%d, %d",use_mind, n);
#endif
			return;
		}

		if(!cast) return;
	}



	/* teleport from mirror costs small energy */
	if(on_mirror && creature_ptr->class_idx == CLASS_MIRROR_MASTER && (n==3 || n==5 || n==7 || n==16))
		cost_tactical_energy(creature_ptr, 50);
	else cost_tactical_energy(creature_ptr, 100);

	if((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU))
	{
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_USELIFE, mana_cost, COD_HARD_CONCENTRATION, NULL, -1);
		prepare_redraw(PR_HP);
	}

	else if(mana_cost <= old_csp) // Sufficient mana
	{
		dec_mana(creature_ptr, mana_cost);

		if((use_mind == MIND_MINDCRAFTER) && (n == 13)) // THE WORLD
		{
			creature_ptr->csp = 0;
			creature_ptr->csp_frac = 0;
		}
	}

	/* Over-exert the player */
	else
	{
		int oops = mana_cost - old_csp;

		/* No mana left */
		if((creature_ptr->csp - mana_cost) < 0) creature_ptr->csp_frac = 0;
		creature_ptr->csp = MAX(0, creature_ptr->csp - mana_cost);

#ifdef JP
		msg_format("%sを集中しすぎて気を失ってしまった！",p);
#else
		msg_print("You faint from the effort!");
#endif

		/* Hack -- Bypass free action */
		(void)add_timed_trait(creature_ptr, TRAIT_SLEPT, randint1(5 * oops + 1), TRUE);

		/* Damage WIS (possibly permanently) */
		if(PERCENT(50))
		{
			bool perm = (PERCENT(25));

#ifdef JP
			msg_print("自分の精神を攻撃してしまった！");
#else
			msg_print("You have damaged your mind!");
#endif

			/* Reduce constitution */
			(void)dec_stat(creature_ptr, STAT_WIS, 15 + randint1(10), perm);
		}
	}

	prepare_window(PW_PLAYER);
	prepare_window(PW_SPELL);
}


/*
* do_cmd_cast calls this function if the player's class
* is 'mindcrafter'.
*/
void do_cmd_mind_browse(creature_type *creature_ptr)
{
	KEY n = 0;
	int j, line;
	char temp[62*5];
	int use_mind = 0;

	if(creature_ptr->class_idx == CLASS_MINDCRAFTER) use_mind = MIND_MINDCRAFTER;
	else if(creature_ptr->class_idx == CLASS_FORCETRAINER) use_mind = MIND_KI;
	else if(creature_ptr->class_idx == CLASS_BERSERKER) use_mind = MIND_BERSERKER;
	else if(creature_ptr->class_idx == CLASS_NINJA) use_mind = MIND_NINJUTSU;
	else if(creature_ptr->class_idx == CLASS_MIRROR_MASTER)
		use_mind = MIND_MIRROR_MASTER;

	screen_save();

	while(TRUE)
	{
		/* get power */
		if(!get_mind_power(creature_ptr, &n, TRUE))
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
