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


void mindcraft_info(creature_type *creature_ptr, char *p, int use_mind, int power)
{
	char buf[30];
#ifdef JP
	cptr s_dam = "損傷:";
	cptr s_dur = "期間:";
	cptr s_range = "範囲:";
#else
	cptr s_dam = "dam ";
	cptr s_dur = "dur ";
	cptr s_range = "range ";
#endif
	int plev = creature_ptr->lev;

	strcpy(p, "");

	format_weight(buf, plev * 15);
	switch (use_mind)
	{
	case MIND_MINDCRAFTER:
		switch (power)
		{
		case 0:  break;
		case 1:  sprintf(p, " %s%dd%d", s_dam, 3 + ((plev - 1) / 4), 3 + plev/15); break;
		case 2:  sprintf(p, " %s10", s_range); break;
		case 3:  sprintf(p, " %s%d", s_range, plev * 5);  break;
		case 4:  break;
		case 5: sprintf(p, " %s%dd8", s_dam, 8 + ((plev - 5) / 4));  break;
		case 6:  sprintf(p, " %s%d", s_dur, plev);  break;
		case 7:  break;
		case 8:  sprintf(p, (plev < 25 ? " %s%d" : " %sd%d"), s_dam, (plev < 25 ? plev * 3 / 2 : plev * ((plev - 5) / 10 + 1))); break;
		case 9:  sprintf(p, " %s10+d%d", s_dur, plev * 3 / 2);  break;
#ifdef JP
		case 10: sprintf(p, " 最大重量:%s", buf);  break;
#else
		case 10: sprintf(p, " max wgt %s", buf);  break;
#endif
		case 11: sprintf(p, " %s%dd6", s_dam, plev / 2);  break;
		case 12: sprintf(p, " %sd%d+%d", s_dam, plev * 3, plev * 3); break;
#ifdef JP
		case 13: sprintf(p, " 行動:%ld回", (creature_ptr->csp + 100 - creature_ptr->energy_need - 50) / 100); break;
#else
		case 13: sprintf(p, " %ld acts.", (creature_ptr->csp + 100 - creature_ptr->energy_need - 50) / 100); break;
#endif
		}
		break;
	case MIND_KI:
		{
			int boost = creature_ptr->class_skills.old_skills.magic_num1[0];

			if(heavy_armor(creature_ptr)) boost /= 2;

			switch (power)
			{
			case 0:  sprintf(p, " %s%dd4", s_dam, 3 + ((plev - 1) / 5) + boost / 12); break;
			case 1:  break;
			case 2:  sprintf(p, " %s%d+d30", s_dur, 30 + boost / 5); break;
			case 3:  sprintf(p, " %s%dd5", s_dam, 5 + ((plev - 1) / 5) + boost / 10); break;
			case 4:  sprintf(p, " %s%d+d20", s_dur, 20 + boost / 5); break;
			case 5:  break;
			case 6:  sprintf(p, " %s%d+d%d", s_dur, 15 + boost / 7, plev / 2); break;
			case 7:  sprintf(p, " %s%dd8", s_dam, 8 + ((plev - 5) / 5) + boost / 12); break;
			case 8:  sprintf(p, " %s10d6+%d", s_dam, plev * 3 / 2 + boost * 3 / 5); break;
			case 9:  break;
#ifdef JP
			case 10: sprintf(p, " 最大%d体", 1+boost/100); break;
#else
			case 10: sprintf(p, " max %d", 1+boost/100); break;
#endif
			case 11: sprintf(p, " %s%d", s_dam, 100 + plev + boost); break;
			case 12: sprintf(p, " %s%dd15", s_dam, 10 + plev / 2 + boost * 3 / 10); break;
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
			case 2:  sprintf(p, " %s%dd4", s_dam, 3 + ((plev - 1) / 5) ); break;
			case 3:  sprintf(p, " %s10", s_range); break;
			case 4:  break;
			case 5:  sprintf(p, " %s%d", s_range, plev *5); break;
			case 6:  sprintf(p, " %s20+d20", s_dur);  break;
			case 7:  break;
			case 8:  sprintf(p, " %s%dd8", s_dam, 8+((plev -5)/4) ); break;
			case 9:  break;
			case 10: sprintf(p, " %s%dd8", s_dam, 11+(plev-5)/4 ); break;
			case 11: break;
			case 12: sprintf(p, " %s20+d20", s_dur);  break;
			case 13: sprintf(p, " %s150+d%d", s_dam, plev*2 ); break;
			case 14: break;
			case 15: break;
			case 16: sprintf(p, " %s%d", s_range, plev/2 +10); break;
			case 17: break;
			case 18: sprintf(p, " %s6+d6", s_dur);  break;
			case 19: sprintf(p, " %s%d", s_dam, plev*11+5 ); break;
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
			case 4:  sprintf(p, " %s%d", s_range , plev *5); break;
			case 5:  sprintf(p, " %s30", s_range); break;
			case 6:  break;
			case 7:  break;
			case 8:  sprintf(p, " %s20+d20", s_dur);  break;
			case 9:  sprintf(p, " %s%d", s_dam, (50+plev)/2 ); break;
			case 10: break;
			case 11: break;
			case 12: break;
			case 13: break;
			case 14: break;
			case 15: break;
			case 16: sprintf(p, " %s%d+d%d", s_dur, plev/2, plev/2);  break;
			case 17: sprintf(p, " %s%d*3", s_dam, (75+plev*2/3)/2 ); break;
			case 18: sprintf(p, " %s%dd10", s_dam, 6+plev/8 ); break;
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
static int get_mind_power(creature_type *creature_ptr, int *sn, bool only_browse)
{
	int             i;
	int             num = 0;
	int             y = 1;
	int             x = 10;
	int             minfail = 0;
	int             plev = creature_ptr->lev;
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
	int menu_line = (use_menu ? 1 : 0);

	switch(creature_ptr->class_idx)
	{
	case CLASS_MINDCRAFTER:
		{
			use_mind = MIND_MINDCRAFTER;
#ifdef JP
			p = "超能力";
#else
			p = "mindcraft";
#endif
			break;
		}
	case CLASS_FORCETRAINER:
		{
			use_mind = MIND_KI;
#ifdef JP
			p = "練気術";
#else
			p = "Force";
#endif
			break;
		}
	case CLASS_BERSERKER:
		{
			use_mind = MIND_BERSERKER;
#ifdef JP
			p = "技";
#else
			p = "brutal power";
#endif
			break;
		}
	case CLASS_MIRROR_MASTER:
		{
			use_mind = MIND_MIRROR_MASTER;
#ifdef JP
			p = "鏡魔法";
#else
			p = "magic";
#endif
			break;
		}
	case CLASS_NINJA:
		{
			use_mind = MIND_NINJUTSU;
#ifdef JP
			p = "忍術";
#else
			p = "ninjutsu";
#endif
			break;
		}
	default:
		{
			use_mind = 0;
#ifdef JP
			p = "超能力";
#else
			p = "mindcraft";
#endif
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
		if(mind_ptr->info[*sn].min_lev <= plev) return TRUE;
	}

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	for (i = 0; i < MAX_MIND_POWERS; i++)
	{
		if(mind_ptr->info[i].min_lev <= plev)
		{
			num++;
		}
	}

	/* Build a prompt (accept all spells) */
	if(only_browse)
	{
#ifdef JP
		(void) strnfmt(out_val, 78, "(%^s %c-%c, '*'で一覧, ESC) どの%sについて知りますか？",
#else
		(void) strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
#endif
			p, I2A(0), I2A(num - 1), p);
	}
	else
	{
#ifdef JP
		(void) strnfmt(out_val, 78, "(%^s %c-%c, '*'で一覧, ESC) どの%sを使いますか？",
#else
		(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
#endif
			p, I2A(0), I2A(num - 1), p);
	}

	if(use_menu && !only_browse) screen_save();
	/* Get a spell from the user */

	choice= (always_show_list || use_menu) ? ESCAPE:1 ;
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
			/* Show the list */
			if(!redraw || use_menu)
			{
				char psi_desc[80];
				bool has_weapon[2];

				/* Show list */
				redraw = TRUE;

				if(!only_browse && !use_menu) screen_save();

				/* Display a list of spells */
				prt("", y, x);
#ifdef JP
				put_str("名前", y, x + 5);
				put_str(format("Lv   %s   失率 効果", ((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU)) ? "HP" : "MP"), y, x + 35);
#else
				put_str("Name", y, x + 5);
				put_str(format("Lv   %s   Fail Info", ((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU)) ? "HP" : "MP"), y, x + 35);
#endif
				has_weapon[0] = get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0;
				has_weapon[1] = get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1;

				/* Dump the spells */
				for (i = 0; i < MAX_MIND_POWERS; i++)
				{
					int mana_cost;

					/* Access the spell */
					spell = mind_ptr->info[i];

					if(spell.min_lev > plev)   break;

					chance = spell.fail;

					mana_cost = spell.mana_cost;
					if(chance)
					{

						/* Reduce failure rate by "effective" level adjustment */
						chance -= 3 * (plev - spell.min_lev);

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
								for (j = 0; j < creature_ptr->class_skills.old_skills.magic_num1[0] / 50; j++)
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
						if(chance > 95) chance = 95;
					}

					/* Get info */
					mindcraft_info(creature_ptr, comment, use_mind, i);

					if(use_menu)
					{
#ifdef JP
						if(i == (menu_line-1)) strcpy(psi_desc, "  》 ");
#else
						if(i == (menu_line-1)) strcpy(psi_desc, "  >  ");
#endif
						else strcpy(psi_desc, "     ");
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

			/* Hide the list */
			else if(!only_browse)
			{
				/* Hide list */
				redraw = FALSE;

				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if(!use_menu)
		{
			ask = isupper(choice);

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
		spell = mind_ptr->info[i];

		/* Verify it */
		if(ask)
		{
			char tmp_val[160];

#ifdef JP
			(void) strnfmt(tmp_val, 78, "%sを使いますか？", spell.name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", spell.name);
#endif
			/* Belay that order */
			if(!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
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
	int        b = 0;
	int        dir;
	int        plev = creature_ptr->lev;

	/* spell code */
	switch (spell)
	{
	case 0:   /* Precog */
		if(plev > 44)
		{
			wiz_lite(floor_ptr, creature_ptr, FALSE);
		}
		else if(plev > 19)
			map_area(creature_ptr, DETECT_RAD_MAP);

		if(plev < 30)
		{
			b = detect_creatures_normal(creature_ptr, DETECT_RAD_DEFAULT);
			if(plev > 14) b |= detect_creatures_invis(creature_ptr, DETECT_RAD_DEFAULT);
			if(plev > 4)  {
				b |= detect_traps(creature_ptr, DETECT_RAD_DEFAULT, TRUE);
				b |= detect_doors(creature_ptr, DETECT_RAD_DEFAULT);
			}
		}
		else
			b = detect_all(creature_ptr, DETECT_RAD_DEFAULT);

		if((plev > 24) && (plev < 40))
			set_timed_trait(creature_ptr, TRAIT_ESP, plev, FALSE);

#ifdef JP
		if(!b) msg_print("安全な気がする。");
#else
		if(!b) msg_print("You feel safe.");
#endif

		break;
	case 1:
		/* Mindblast */
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

		if(randint1(100) < plev * 2)
			cast_beam(creature_ptr, MAX_RANGE_SUB, DO_EFFECT_PSI, diceroll(3 + ((plev - 1) / 4), (3 + plev / 15)), 0, FALSE);
		else
			cast_ball(creature_ptr, DO_EFFECT_PSI, dir, diceroll(3 + ((plev - 1) / 4), (3 + plev / 15)), 0);
		break;
	case 2:
		/* Minor displace */
		teleport_player(creature_ptr, 10, 0L);
		break;
	case 3:
		/* Major displace */
		teleport_player(creature_ptr, plev * 5, 0L);
		break;
	case 4:
		/* Domination */
		if(plev < 30)
		{
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
			cast_ball(creature_ptr, DO_EFFECT_DOMINATION, dir, plev, 0);
		}
		else
			project_hack(creature_ptr, DO_EFFECT_CHARM, plev * 2);
		break;
	case 5:
		/* Fist of Force  ---  not 'true' TK  */
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

		cast_ball(creature_ptr, DO_EFFECT_TELEKINESIS, dir, diceroll(8 + ((plev - 5) / 4), 8),
			(plev > 20 ? (plev - 20) / 8 + 1 : 0));
		break;
	case 6:
		/* Character Armour */
		set_timed_trait(creature_ptr, TRAIT_SHIELD, plev, FALSE);
		if(plev > 14) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ACID, plev, FALSE);
		if(plev > 19) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_FIRE, plev, FALSE);
		if(plev > 24) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_COLD, plev, FALSE);
		if(plev > 29) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ELEC, plev, FALSE);
		if(plev > 34) set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_POIS, plev, FALSE);
		break;
	case 7:
		/* Psychometry */
		if(plev < 25)
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

		if(plev < 25)
			project(creature_ptr, 0, 2 + plev / 10, creature_ptr->fy, creature_ptr->fx,
			(plev * 3), DO_EFFECT_PSI, PROJECT_KILL, -1);
		else
			(void)mindblast_creatures(creature_ptr, randint1(plev * ((plev - 5) / 10 + 1)));
		break;
	case 9:
		/* Adrenaline */
		set_timed_trait(creature_ptr, TRAIT_AFRAID, 0, TRUE);
		set_timed_trait(creature_ptr, TRAIT_STUN, 0, TRUE);

		/*
		* Only heal when Adrenalin Channeling is not active. We check
		* that by checking if the player isn't fast and 'heroed' atm.
		*/
		if(!IS_FAST(creature_ptr) || !IS_HERO(creature_ptr)) heal_creature(creature_ptr, plev);

		b = 10 + randint1((plev * 3) / 2);
		(void)set_timed_trait(creature_ptr, TRAIT_HERO, b, FALSE);
		(void)set_timed_trait(creature_ptr, TRAIT_FAST, b, TRUE);
		break;

	case 10:
		/* Telekinesis */
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

		fetch(creature_ptr, MAX_RANGE, dir, plev * 15, FALSE);

		break;
	case 11:
		/* Psychic Drain */
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

		b = diceroll(plev / 2, 6);

		/* This is always a radius-0 ball now */
		if(cast_ball(creature_ptr, DO_EFFECT_PSI_DRAIN, dir, b, 0)) cost_tactical_energy(creature_ptr, randint1(150));
		break;
	case 12:
		/* psycho-spear */
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

		cast_beam(creature_ptr, MAX_RANGE_SUB, DO_EFFECT_PSY_SPEAR, randint1(plev*3)+plev*3, 0, FALSE);
		break;
	case 13:
		{
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

			// Update creatures
			prepare_update(creature_ptr, PU_CREATURES);

			prepare_window(PW_OVERHEAD | PW_DUNGEON);

			handle_stuff(creature_ptr);
			break;
		}
	default:
#ifdef JP
		msg_print("なに？");
#else
		msg_print("Zap?");
#endif

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
	int             dir;
	int             plev = creature_ptr->lev;
	int             boost = creature_ptr->class_skills.old_skills.magic_num1[0];

	if(heavy_armor(creature_ptr)) boost /= 2;

	/* spell code */
	switch (spell)
	{
	case 0:
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		cast_ball(creature_ptr, DO_EFFECT_MISSILE, dir, diceroll(3 + ((plev - 1) / 5) + boost / 12, 4), 0);
		break;
	case 1:
		(void)lite_area(creature_ptr, diceroll(2, (plev / 2)), (plev / 10) + 1);
		break;
	case 2:
		set_timed_trait(creature_ptr, TRAIT_LEVITATION, randint1(30) + 30 + boost / 5, FALSE);
		break;
	case 3:
		if(!get_aim_dir(creature_ptr, plev / 8 + 3, &dir)) return FALSE;
		cast_beam(creature_ptr, plev / 8 + 3, DO_EFFECT_MISSILE, diceroll(5 + ((plev - 1) / 5) + boost / 10, 5), 0, FALSE);
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
		creature_ptr->class_skills.old_skills.magic_num1[0] += (70 + plev);
		prepare_update(creature_ptr, CRU_BONUS);
		if(randint1(creature_ptr->class_skills.old_skills.magic_num1[0]) > (plev * 4 + 120))
		{
#ifdef JP
			msg_print("気が暴走した！");
#else
			msg_print("The Force exploded!");
#endif
			cast_ball(creature_ptr, DO_EFFECT_MANA, 0, creature_ptr->class_skills.old_skills.magic_num1[0] / 2, 10);
#ifdef JP
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, creature_ptr->class_skills.old_skills.magic_num1[0] / 2, "気の暴走", NULL, -1);
#else
			take_damage_to_creature(NULL, creature_ptr, DAMAGE_LOSELIFE, creature_ptr->class_skills.old_skills.magic_num1[0] / 2, "Explosion of the Force", NULL, -1);
#endif
		}
		else return TRUE;
		break;
	case 6:
		set_timed_trait(creature_ptr, TRAIT_AURA_MANA, randint1(plev / 2) + 15 + boost / 7, FALSE);
		break;
	case 7:
		{
			int y, x, dam;
			if(!get_aim_dir(creature_ptr, 1, &dir)) return FALSE;

			y = creature_ptr->fy + ddy[dir];
			x = creature_ptr->fx + ddx[dir];
			dam = diceroll(8 + ((plev - 5) / 4) + boost / 12, 8);
			cast_beam(creature_ptr, MAX_RANGE_SUB, DO_EFFECT_MISSILE, dam, 0, FALSE);
			if(floor_ptr->cave[y][x].creature_idx)
			{
				int i;
				int ty = y, tx = x;
				int oy = y, ox = x;
				int m_idx = floor_ptr->cave[y][x].creature_idx;
				creature_type *m_ptr = &creature_list[m_idx];
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
#ifdef JP
						msg_format("%sを吹き飛ばした！", m_name);
#else
						msg_format("You blow %s away!", m_name);
#endif
						floor_ptr->cave[oy][ox].creature_idx = 0;
						floor_ptr->cave[ty][tx].creature_idx = m_idx;
						m_ptr->fy = ty;
						m_ptr->fx = tx;

						update_creature_view(player_ptr, m_idx, TRUE);
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
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		cast_ball(creature_ptr, DO_EFFECT_MISSILE, dir, diceroll(10, 6) + plev * 3 / 2 + boost * 3 / 5, (plev < 30) ? 2 : 3);
		break;
	case 9:
		{
			int m_idx;

			if(!target_set(creature_ptr, 0, TARGET_KILL)) return FALSE;
			m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!m_idx) break;
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
				if(summon_specific(NULL, creature_ptr->fy, creature_ptr->fx, plev, TRAIT_S_PHANTOM, PC_FORCE_PET))
					success = TRUE;
			if(success)
			{
#ifdef JP
				msg_print("御用でございますが、御主人様？");
#else
				msg_print("'Your wish, master?'");
#endif
			}
			else
			{
#ifdef JP
				msg_print("何も現れなかった。");
#else
				msg_print("Nothing happen.");
#endif
			}
			break;
		}
	case 11:
		cast_ball(creature_ptr, DO_EFFECT_FIRE, 0, 200 + (2 * plev) + boost * 2, 10);
		break;
	case 12:
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;

		cast_beam(creature_ptr, MAX_RANGE_SUB, DO_EFFECT_MANA, diceroll(10 + (plev / 2) + boost * 3 / 10, 15), 0, FALSE);
		break;
	case 13:
		set_timed_trait(creature_ptr, TRAIT_LIGHT_SPEED, randint1(16) + 16 + boost / 20, FALSE);
		break;
	default:
#ifdef JP
		msg_print("なに？");
#else
		msg_print("Zap?");
#endif

	}
	creature_ptr->class_skills.old_skills.magic_num1[0] = 0;
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
	int dir;
	int plev = creature_ptr->lev;
	int tmp;
	int	x,y;

	/* spell code */
	switch (spell)
	{
		/* mirror of seeing */
	case 0:
		tmp = is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) ? 4 : 0;
		if( plev + tmp > 4) detect_creatures_normal(creature_ptr, DETECT_RAD_DEFAULT);
		if( plev + tmp > 18) detect_creatures_invis(creature_ptr, DETECT_RAD_DEFAULT);
		if( plev + tmp > 28) set_timed_trait(creature_ptr, TRAIT_ESP, plev,FALSE);
		if( plev + tmp > 38) map_area(creature_ptr, DETECT_RAD_MAP);
		if( tmp == 0 && plev < 5 ){
#ifdef JP
			msg_print("鏡がなくて集中できなかった！");
#else
			msg_print("You need a mirror to concentrate!");
#endif
		}
		break;
		// drip of light
	case 1:
		if(number_of_mirrors(creature_ptr) < 4 + plev/10 ){
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
		if( plev > 9 && is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) ) {
			cast_beam(creature_ptr, MAX_RANGE_SUB, DO_EFFECT_LITE, diceroll(3+((plev-1)/5),4), 0, FALSE);
		}
		else {
			cast_bolt(creature_ptr, DO_EFFECT_LITE, diceroll(3+((plev-1)/5),4), 0, FALSE);
		}
		break;
		/* warped mirror */
	case 3:
		teleport_player(creature_ptr, 10, 0L);
		break;
		/* mirror of light */
	case 4:
		(void)lite_area(creature_ptr, diceroll(2, (plev / 2)), (plev / 10) + 1);
		break;
		/* mirror of wandering */
	case 5:
		teleport_player(creature_ptr, plev * 5, 0L);
		break;
		/* robe of dust */
	case 6:
		set_timed_trait(creature_ptr, TRAIT_DUST_ROBE,  20+randint1(20),FALSE);
		break;
		/* banishing mirror */
	case 7:
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		(void)cast_beam(creature_ptr, MAX_RANGE_SUB, DO_EFFECT_AWAY_ALL, plev, 0, FALSE);
		break;
		/* mirror clashing */
	case 8:
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		cast_ball(creature_ptr, DO_EFFECT_SHARDS, dir, diceroll(8 + ((plev - 5) / 4), 8),
			(plev > 20 ? (plev - 20) / 8 + 1 : 0));
		break;
		/* mirror sleeping */
	case 9:
		for(x=0;x<floor_ptr->width;x++){
			for(y=0;y<floor_ptr->height;y++){
				if(is_mirror_grid(&floor_ptr->cave[y][x])) {
					project(creature_ptr, 0,2,y,x,plev,DO_EFFECT_OLD_SLEEP,(PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP|PROJECT_NO_HANGEKI),-1);
				}
			}
		}
		break;
		/* seeker ray */
	case 10:
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		cast_beam(creature_ptr, MAX_RANGE_SUB, DO_EFFECT_SEEKER, diceroll(11+(plev-5)/4,8), 0, FALSE);
		break;
		/* seal of mirror */
	case 11:
		seal_of_mirror(creature_ptr, plev * 4 + 100);
		break;
		/* shield of water */
	case 12:
		tmp = 20+randint1(20);
		set_timed_trait(creature_ptr, TRAIT_SHIELD, tmp, FALSE);
		if(plev > 31) set_timed_trait(creature_ptr, TRAIT_REFLECTING, tmp, FALSE);
		if(plev > 39) set_timed_trait(creature_ptr, TRAIT_RESIST_MAGIC, tmp, FALSE);
		break;
		/* super ray */
	case 13:
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		cast_beam(creature_ptr, MAX_RANGE_SUB, DO_EFFECT_SUPER_RAY, 150+randint1(2*plev), 0, FALSE);
		break;
		/* illusion light */
	case 14:
		tmp = is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) ? 4 : 3;
		slow_creatures(creature_ptr);
		stun_creatures(creature_ptr, plev*tmp);
		confuse_creatures(creature_ptr, plev*tmp);
		turn_creatures(creature_ptr, plev*tmp);
		stun_creatures(creature_ptr, plev*tmp);
		stasis_creatures(creature_ptr, plev*tmp);
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
		if(!binding_field(creature_ptr, MAX_RANGE, plev*11+5)) msg_print("適当な鏡を選べなかった！");
#else
		if(!binding_field(creature_ptr, MAX_RANGE, plev*11+5)) msg_print("You were not able to choose suitable mirrors!");
#endif
		break;
		/* mirror of Ruffnor */
	case 20:
		(void)set_timed_trait(creature_ptr, TRAIT_INVULNERABLE, randint1(4)+4,FALSE);
		break;
	default:
#ifdef JP
		msg_print("なに？");
#else
		msg_print("Zap?");
#endif

	}
	creature_ptr->class_skills.old_skills.magic_num1[0] = 0;

	return TRUE;
}


// do_cmd_cast calls this function if the player's class
// is 'berserker'.
static bool cast_berserk_spell(creature_type *creature_ptr, int spell)
{
	int y, x;
	int dir;

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
				msg_print(GAME_MESSAGE_PREVENT_BY_RIDING);
				return FALSE;
			}

			if(!get_rep_dir2(creature_ptr, &dir)) return FALSE;

			if(dir == 5) return FALSE;
			y = creature_ptr->fy + ddy[dir];
			x = creature_ptr->fx + ddx[dir];

			if(!floor_ptr->cave[y][x].creature_idx)
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("There is no creature.");
#endif
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
		earthquake(creature_ptr, creature_ptr->fy, creature_ptr->fx, 8+randint0(5));
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
				if(c_ptr->creature_idx && (m_ptr->see_others || cave_have_flag_bold(floor_ptr, y, x, FF_PROJECT)))
					close_combat(creature_ptr, y, x, 0);
			}
			break;
		}
	default:
#ifdef JP
		msg_print("なに？");
#else
		msg_print("Zap?");
#endif

	}
	return TRUE;
}



// do_cmd_cast calls this function if the player's class
// is 'ninja'.
static bool cast_ninja_spell(creature_type *creature_ptr, int spell)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int x, y;
	int dir;
	int plev = creature_ptr->lev;

	/* spell code */
	switch (spell)
	{
	case 0:
		(void)unlite_area(creature_ptr, 0, 3);
		break;
	case 1:
		if(plev > 44)
		{
			wiz_lite(floor_ptr, creature_ptr, TRUE);
		}
		detect_creatures_normal(creature_ptr, DETECT_RAD_DEFAULT);
		if(plev > 4)
		{
			detect_traps(creature_ptr, DETECT_RAD_DEFAULT, TRUE);
			detect_doors(creature_ptr, DETECT_RAD_DEFAULT);
			detect_stairs(creature_ptr, DETECT_RAD_DEFAULT);
		}
		if(plev > 14)
		{
			detect_objects_normal(creature_ptr, DETECT_RAD_DEFAULT);
		}
		break;
	case 2:
		{
			teleport_player(creature_ptr, 10, 0L);
			break;
		}
	case 3:
		{
			if(!(creature_ptr->posture & NINJA_KAWARIMI))
			{
#ifdef JP
				msg_print("敵の攻撃に対して敏感になった。");
#else
				msg_print("You are now prepare to evade any attacks.");
#endif

				creature_ptr->posture |= NINJA_KAWARIMI;
				prepare_redraw(PR_STATUS);
			}
			break;
		}
	case 4:
		{
			teleport_player(creature_ptr, creature_ptr->lev * 5, 0L);
			break;
		}
	case 5:
		{
			if(!get_rep_dir(creature_ptr, &dir, FALSE)) return FALSE;
			y = creature_ptr->fy + ddy[dir];
			x = creature_ptr->fx + ddx[dir];
			if(floor_ptr->cave[y][x].creature_idx)
			{
				close_combat(creature_ptr, y, x, 0);
				if(randint0(creature_ptr->skill_dis) < 7)
#ifdef JP
					msg_print("うまく逃げられなかった。");
#else
					msg_print("You failed to run away.");
#endif
				else
				{
					teleport_player(creature_ptr, 30, 0L);
				}
			}
			else
			{
#ifdef JP
				msg_print("その方向にはクリーチャーはいません。");
#else
				msg_print("You don't see any creature in this direction");
#endif

				msg_print(NULL);
			}
			break;
		}
	case 6:
		{
			if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
			(void)stasis_creature(creature_ptr, dir);
			break;
		}
	case 7:
		return ident_spell(creature_ptr, FALSE);
	case 8:
		set_timed_trait(creature_ptr, TRAIT_LEVITATION, randint1(20) + 20, FALSE);
		break;
	case 9:
		cast_ball(creature_ptr, DO_EFFECT_FIRE, 0, 50+plev, plev/10+2);
		teleport_player(creature_ptr, 30, 0L);
		set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_FIRE, plev, FALSE);
		break;
	case 10:
		return rush_attack(creature_ptr, NULL);
	case 11:
		{
			int i;
			for (i = 0; i < 8; i++)
			{
				int slot;

				for (slot = 0; slot < INVEN_TOTAL; slot++)
				{
					if(creature_ptr->inventory[slot].tval == TV_SPIKE) break;
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
				do_cmd_throw_aux(creature_ptr, 1, FALSE, slot);

				cost_tactical_energy(creature_ptr, 100);
			}
			break;
		}
	case 12:
		{
			creature_type *m_ptr;
			int m_idx;
			char m_name[MAX_NLEN];
			int i;
			int path_n;
			u16b path_g[512];
			int ty,tx;

			if(!target_set(creature_ptr, 0, TARGET_KILL)) return FALSE;
			m_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!m_idx) break;
			if(m_idx == creature_ptr->riding) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, MAX_RANGE, creature_ptr->fy, creature_ptr->fx, target_row, target_col)) break;
			m_ptr = &creature_list[m_idx];
			creature_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%sを引き戻した。", m_name);
#else
			msg_format("You pull back %s.", m_name);
#endif

			path_n = project_path(path_g, MAX_RANGE, floor_ptr, target_row, target_col, creature_ptr->fy, creature_ptr->fx, 0);
			ty = target_row, tx = target_col;
			for (i = 1; i < path_n; i++)
			{
				int ny = GRID_Y(path_g[i]);
				int nx = GRID_X(path_g[i]);
				cave_type *c_ptr = &floor_ptr->cave[ny][nx];

				if(in_bounds(floor_ptr, ny, nx) && cave_empty_bold(floor_ptr, ny, nx) &&
					!(c_ptr->info & CAVE_OBJECT) && !pattern_tile(floor_ptr, ny, nx))
				{
					ty = ny;
					tx = nx;
				}
			}
			/* Update the old location */
			floor_ptr->cave[target_row][target_col].creature_idx = 0;

			/* Update the new location */
			floor_ptr->cave[ty][tx].creature_idx = m_idx;

			/* Move the creature */
			m_ptr->fy = ty;
			m_ptr->fx = tx;

			/* Wake the creature up */
			(void)set_timed_trait(m_ptr, TRAIT_PARALYZED, 0, TRUE);

			/* Update the creature (new location) */
			update_creature_view(player_ptr, m_idx, TRUE);

			lite_spot(floor_ptr, target_row, target_col);

			lite_spot(floor_ptr, ty, tx);

			if(is_lighting_creature(m_ptr) || is_darken_creature(m_ptr))
				prepare_update(creature_ptr, PU_SPECIES_LITE);

			if(m_ptr->see_others)
			{
				/* Auto-Recall if possible and visible */
				if(!has_trait(creature_ptr, TRAIT_HALLUCINATION)) species_type_track(m_ptr->ap_species_idx);

				/* Track a new creature */
				health_track(m_idx);
			}

			break;
		}
	case 13:
		if(!get_aim_dir(creature_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		cast_ball(creature_ptr, DO_EFFECT_OLD_CONF, dir, plev*3, 3);
		break;
	case 14:
		if(!get_aim_dir(creature_ptr, NO_RANGE_LIMIT, &dir)) return FALSE;
		(void)teleport_swap(creature_ptr, dir);
		break;
	case 15:
		explosive_rune(creature_ptr);
		break;
	case 16:
		(void)set_timed_trait(creature_ptr, TRAIT_PASS_WALL, randint1(plev/2) + plev/2, FALSE);
		set_timed_trait(creature_ptr, TRAIT_MAGIC_RES_ACID, plev, FALSE);
		break;
	case 17:
		cast_ball(creature_ptr, DO_EFFECT_POIS, 0, 75+plev*2/3, plev/5+2);
		cast_ball(creature_ptr, DO_EFFECT_OLD_DRAIN, 0, 75+plev*2/3, plev/5+2);
		cast_ball(creature_ptr, DO_EFFECT_CONFUSION, 0, 75+plev*2/3, plev/5+2);
		teleport_player(creature_ptr, 30, 0L);
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
					scatter(floor_ptr, &y, &x, creature_ptr->fy, creature_ptr->fx, 4, 0);

					if(!creature_bold(creature_ptr, y, x)) break;
				}
				project(creature_ptr, 0, 0, y, x, diceroll(6 + plev / 8, 10), typ, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
			}
			break;
		}
	case 19:
		set_timed_trait(creature_ptr, TRAIT_MULTI_SHADOW, 6+randint1(6), FALSE);
		break;
	default:
#ifdef JP
		msg_print("なに？");
#else
		msg_print("Zap?");
#endif

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
	int             n = 0, b = 0;
	int             chance;
	int             minfail = 0;
	int             plev = creature_ptr->lev;
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
#ifdef JP
		msg_print("混乱していて集中できない！");
#else
		msg_print("You are too confused!");
#endif

		return;
	}

	/* get power */
	if(!get_mind_power(creature_ptr, &n, FALSE)) return;

#ifdef JP
	switch(creature_ptr->class_idx)
	{
	case CLASS_MINDCRAFTER: use_mind = MIND_MINDCRAFTER;p = "精神";break;
	case CLASS_FORCETRAINER:          use_mind = MIND_KI;p = "気";break;
	case CLASS_BERSERKER:   use_mind = MIND_BERSERKER;p = "怒り";break;
	case CLASS_MIRROR_MASTER:   use_mind = MIND_MIRROR_MASTER;p = "鏡魔法";break;
	case CLASS_NINJA:       use_mind = MIND_NINJUTSU;p = "精神";break;
	default:                use_mind = 0;p = "超能力";break;
	}
#else
	switch(creature_ptr->class_idx)
	{
	case CLASS_MINDCRAFTER: use_mind = MIND_MINDCRAFTER;break;
	case CLASS_FORCETRAINER:          use_mind = MIND_KI;break;
	case CLASS_BERSERKER:   use_mind = MIND_BERSERKER;break;
	case CLASS_MIRROR_MASTER:   use_mind = MIND_MIRROR_MASTER;break;
	case CLASS_NINJA:       use_mind = MIND_NINJUTSU;break;
	default:                use_mind = 0;break;
	}
#endif
	spell = mind_powers[use_mind].info[n];

	/* Spell failure chance */
	chance = spell.fail;

	mana_cost = spell.mana_cost;
	if(use_mind == MIND_KI)
	{
		if(heavy_armor(creature_ptr)) chance += 20;
		else if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0) chance += 10;
		else if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1) chance += 10;
		if(n == 5)
		{
			int j;
			for (j = 0; j < creature_ptr->class_skills.old_skills.magic_num1[0] / 50; j++)
				mana_cost += (j+1) * 3 / 2;
		}
	}

	/* Verify "dangerous" spells */
	if((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU))
	{
		if(mana_cost > creature_ptr->chp)
		{
#ifdef JP
			msg_print("ＨＰが足りません。");
#else
			msg_print("You do not have enough hp to use this power.");
#endif
			return;
		}
	}
	else if(mana_cost > creature_ptr->csp)
	{
#ifdef JP
		msg_print("ＭＰが足りません。");
		if(!over_exert) return;
		if(!get_check("それでも挑戦しますか? ")) return;
#else
		msg_print("You do not have enough mana to use this power.");
		if(!over_exert) return;
		if(!get_check("Attempt it anyway? ")) return;
#endif
	}

	if(chance)
	{
		/* Reduce failure rate by "effective" level adjustment */
		chance -= 3 * (plev - spell.min_lev);
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
	if(chance > 95) chance = 95;

	/* Failed spell */
	if(randint0(100) < chance)
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
			if((use_mind == MIND_KI) && (n != 5) && creature_ptr->class_skills.old_skills.magic_num1[0])
			{
#ifdef JP
				msg_print("気が散ってしまった．．．");
#else
				msg_print("Your improved Force has gone away...");
#endif
				creature_ptr->class_skills.old_skills.magic_num1[0] = 0;
			}

			if(randint1(100) < (chance / 2))
			{
				/* Backfire */
				b = randint1(100);

				if( use_mind == MIND_MINDCRAFTER ){
					if(b < 5)
					{
#ifdef JP
						msg_print("なんてこった！頭の中が真っ白になった！");
#else
						msg_print("Oh, no! Your mind has gone blank!");
#endif

						lose_all_info(creature_ptr);
					}
					else if(b < 15)
					{
#ifdef JP
						msg_print("奇妙な光景が目の前で踊っている...");
#else
						msg_print("Weird visions seem to dance before your eyes...");
#endif
						add_timed_trait(creature_ptr, TRAIT_HALLUCINATION, 5 + randint1(10), TRUE);
					}
					else if(b < 45)
					{
#ifdef JP
						msg_print("あなたの頭は混乱した！");
#else
						msg_print("Your brain is addled!");
#endif
						add_timed_trait(creature_ptr, TRAIT_CONFUSED, randint1(8), TRUE);
					}
					else if(b < 90)
					{
						add_timed_trait(creature_ptr, TRAIT_STUN, randint1(8), TRUE);
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
						project(creature_ptr, 0, 2 + plev / 10, creature_ptr->fy, creature_ptr->fx, plev * 2,
							DO_EFFECT_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
						creature_ptr->csp = MAX(0, creature_ptr->csp - plev * MAX(1, plev / 10));
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
						teleport_player(creature_ptr, 10, TELEPORT_PASSIVE);
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
						project(creature_ptr, 0, 2 + plev / 10, creature_ptr->fy, creature_ptr->fx, plev * 2,
							DO_EFFECT_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
						creature_ptr->csp = MAX(0, creature_ptr->csp - plev * MAX(1, plev / 10));
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
			/* Cast the spell */
			cast = cast_mindcrafter_spell(creature_ptr, n);
			break;
		case MIND_KI:
			/* Cast the spell */
			cast = cast_force_spell(creature_ptr, n);
			break;
		case MIND_BERSERKER:
			/* Cast the spell */
			cast = cast_berserk_spell(creature_ptr, n);
			break;
		case MIND_MIRROR_MASTER:
			/* Cast the spell */
			if( is_mirror_grid(&floor_ptr->cave[creature_ptr->fy][creature_ptr->fx]) )on_mirror = TRUE;
			cast = cast_mirror_spell(creature_ptr, n);
			break;
		case MIND_NINJUTSU:
			/* Cast the spell */
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

	/* Take a turn */


	/* teleport from mirror costs small energy */
	if(on_mirror && creature_ptr->class_idx == CLASS_MIRROR_MASTER && (n==3 || n==5 || n==7 || n==16))
		cost_tactical_energy(creature_ptr, 50);
	else cost_tactical_energy(creature_ptr, 100);

	if((use_mind == MIND_BERSERKER) || (use_mind == MIND_NINJUTSU))
	{
#ifdef JP
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_USELIFE, mana_cost, "過度の集中", NULL, -1);
#else
		take_damage_to_creature(NULL, creature_ptr, DAMAGE_USELIFE, mana_cost, "concentrating too hard", NULL, -1);
#endif
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
		if(randint0(100) < 50)
		{
			bool perm = (randint0(100) < 25);

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
	int n = 0;
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

	while(1)
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
#ifdef JP
			prt("何かキーを押して下さい。",0,0);
#else
			prt("Hit any key.",0,0);
#endif
			(void)inkey();
		}
	}
}
