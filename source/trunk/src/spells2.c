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
 * See also "identify_fully(cr_ptr, )".
 *
 * XXX XXX XXX Use the "show_file()" method, perhaps.
 */
void creature_knowledge(creature_type *creature_ptr)
{
	int i = 0, j, k;

	int v_nr = 0;
	char v_string [MAX_KARMA] [128];
	char s_string [6] [128];

	u32b flgs[TR_FLAG_SIZE];

	object_type *o_ptr;

	char Dummy[80];
	char Dummy2[10][80];
	char buf[2][80];
	char buf2[100];

	cptr info[280];

	int plev = creature_ptr->lev;

	int percent;

	Term_clear();

	for (j = 0; j < TR_FLAG_SIZE; j++)
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
		u32b tflgs[TR_FLAG_SIZE];

		if(!creature_ptr->equip_now[k]) continue;

		o_ptr = &creature_ptr->inventory[k];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Extract the flags */
		object_flags(o_ptr, tflgs);

		/* Extract flags */
		for (j = 0; j < TR_FLAG_SIZE; j++)
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
		int n = 180 + (creature_ptr->stat_mod_max_max[v_nr] - 18);
		sprintf(stat_desc, "%s %2d.%d", stat_names[v_nr], n / 10, n % 10 );

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


	if(creature_ptr->player)
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

	if(gameover_e)
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

	if (has_cf_creature(creature_ptr, CF_POWERFUL))
	{
#ifdef JP
			info[i++] = "あなたはあらゆるレイシャルパワーを強力に扱うことができる。";
#else
			info[i++] = "You can use every racial power strongly.";
#endif

	}


	/* Racial powers... */
	if (creature_ptr->mimic_form)
	{
		switch (creature_ptr->mimic_form)
		{
			case MIMIC_DEMON:
			case MIMIC_DEMON_LORD:
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの地獄か火炎のブレスを吐くことができる。(%d MP)", 3 * plev, 10+plev/3);
#else
				sprintf(Dummy, "You can nether breathe, dam. %d (cost %d).", 3 * plev, 10+plev/3);
#endif

				info[i++] = Dummy;
			break;
		case MIMIC_VAMPIRE:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "あなたは敵から %d-%d HP の生命力を吸収できる。(%d MP)",
#else
				sprintf(Dummy, "You can steal life from a foe, dam. %d-%d (cost %d).",
#endif

				    plev + MAX(1, plev / 10), plev + plev * MAX(1, plev / 10), 1 + (plev / 3));
				info[i++] = Dummy;
			}
			break;
		}
	}
	else
	{
	switch (creature_ptr->race_idx1)
	{
		case RACE_NIBELUNG:
		case RACE_DWARF:
			if (plev > 4)
#ifdef JP
info[i++] = "あなたは罠とドアと階段を感知できる。(5 MP)";
#else
				info[i++] = "You can find traps, doors and stairs (cost 5).";
#endif

			break;
		case RACE_HOBBIT:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "あなたは食料を生成できる。(10 MP)";
#else
				info[i++] = "You can produce food (cost 10).";
#endif

			}
			break;
		case RACE_GNOME:
			if (plev > 4)
			{
#ifdef JP
sprintf(Dummy, "あなたは範囲 %d 以内にテレポートできる。(%d MP)",
#else
				sprintf(Dummy, "You can teleport, range %d (cost %d).",
#endif

				    (1 + plev), (5 + (plev / 5)));
				info[i++] = Dummy;
			}
			break;
		case RACE_ORC:
			if (plev > 2)
#ifdef JP
info[i++] = "あなたは恐怖を除去できる。(5 MP)";
#else
				info[i++] = "You can remove fear (cost 5).";
#endif

			break;
		case RACE_TROLL:
			if (plev > 9)
#ifdef JP
info[i++] = "あなたは狂暴化することができる。(12 MP) ";
#else
				info[i++] = "You enter berserk fury (cost 12).";
#endif

			break;
		case RACE_AMBERITE:
			if (plev > 29)
#ifdef JP
info[i++] = "あなたはシャドウシフトすることができる。(50 MP)";
#else
				info[i++] = "You can Shift Shadows (cost 50).";
#endif

			if (plev > 39)
#ifdef JP
info[i++] = "あなたは「パターン」を心に描いて歩くことができる。(75 MP)";
#else
				info[i++] = "You can mentally Walk the Pattern (cost 75).";
#endif

			break;
		case RACE_BARBARIAN:
			if (plev > 7)
#ifdef JP
info[i++] = "あなたは狂暴化することができる。(10 MP) ";
#else
				info[i++] = "You can enter berserk fury (cost 10).";
#endif

			break;
		case RACE_OGRE:
			if (plev > 24)
#ifdef JP
info[i++] = "あなたは爆発のルーンを仕掛けることができる。(35 MP)";
#else
				info[i++] = "You can set an Explosive Rune (cost 35).";
#endif

			break;
		case RACE_GIANT:
			if (plev > 19)
#ifdef JP
info[i++] = "あなたは石の壁を壊すことができる。(10 MP)";
#else
				info[i++] = "You can break stone walls (cost 10).";
#endif

			break;
		case RACE_TITAN:
			if (plev > 34)
#ifdef JP
info[i++] = "あなたはモンスターをスキャンすることができる。(20 MP)";
#else
				info[i++] = "You can probe monsters (cost 20).";
#endif

			break;
		case RACE_CYCLOPS:
			if (plev > 19)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの岩石を投げることができる。(15 MP)",
#else
				sprintf(Dummy, "You can throw a boulder, dam. %d (cost 15).",
#endif

				    3 * plev);
				info[i++] = Dummy;
			}
			break;
		case RACE_YEEK:
			if (plev > 14)
#ifdef JP
info[i++] = "あなたは恐怖を呼び起こす叫び声を発することができる。(15 MP)";
#else
				info[i++] = "You can make a terrifying scream (cost 15).";
#endif

			break;
		case RACE_KLACKON:
			if (plev > 8)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの酸を吹きかけることができる。(9 MP)", plev);
#else
				sprintf(Dummy, "You can spit acid, dam. %d (cost 9).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
		case RACE_KOBOLD:
			if (plev > 11)
			{
				sprintf(Dummy,
#ifdef JP
    "あなたは %d ダメージの毒矢を投げることができる。(8 MP)", plev);
#else
				    "You can throw a dart of poison, dam. %d (cost 8).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
		case RACE_DARK_ELF:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのマジック・ミサイルの呪文を使える。(2 MP)",
#else
				sprintf(Dummy, "You can cast a Magic Missile, dam %d (cost 2).",
#endif

				    (3 + ((plev-1) / 5)));
				info[i++] = Dummy;
			}
			break;
		case RACE_DRACONIAN:
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのブレスを吐くことができる。(%d MP)", 2 * plev, plev);
#else
			sprintf(Dummy, "You can breathe, dam. %d (cost %d).", 2 * plev, plev);
#endif

			info[i++] = Dummy;
			break;
		case RACE_MIND_FLAYER:
			if (plev > 14)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの精神攻撃をすることができる。(12 MP)", plev);
#else
				sprintf(Dummy, "You can mind blast your enemies, dam %d (cost 12).", plev);
#endif

			info[i++] = Dummy;
			break;
		case RACE_IMP:
			if (plev > 29)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのファイア・ボールの呪文を使える。(15 MP)", plev);
#else
				sprintf(Dummy, "You can cast a Fire Ball, dam. %d (cost 15).", plev);
#endif

				info[i++] = Dummy;
			}
			else if (plev > 8)
			{
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのファイア・ボルトの呪文を使える。(15 MP)", plev);
#else
				sprintf(Dummy, "You can cast a Fire Bolt, dam. %d (cost 15).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
/* TODO
		case ZOMBIE:
		case SKELETON:
			if (plev > 29)
#ifdef JP
info[i++] = "あなたは失った生命力を回復することができる。(30 MP)";
#else
				info[i++] = "You can restore lost life forces (cost 30).";
#endif

			break;
		case VAMPIRE:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "あなたは敵から %d-%d HP の生命力を吸収できる。(%d MP)",
#else
				sprintf(Dummy, "You can steal life from a foe, dam. %d-%d (cost %d).",
#endif

				    plev + MAX(1, plev / 10), plev + plev * MAX(1, plev / 10), 1 + (plev / 3));
				info[i++] = Dummy;
			}
			break;
		case LICH:
			if (plev > 3)
			{
#ifdef JP
info[i++] = "あなたは泣き叫んで敵を恐怖させることができる。(3 MP)";
#else
				info[i++] = "You can wail to terrify your enemies (cost 3).";
#endif

			}
			break;
*/
		case RACE_SPRITE:
			if (plev > 11)
			{
#ifdef JP
info[i++] = "あなたは敵を眠らせる魔法の粉を投げることができる。(12 MP)";
#else
				info[i++] = "You can throw magical dust which induces sleep (cost 12).";
#endif

			}
			break;
		case RACE_DEMON:
		case RACE_BALROG:
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージの地獄か火炎のブレスを吐くことができる。(%d MP)", 3 * plev, 10+plev/3);
#else
			sprintf(Dummy, "You can breathe nether, dam. %d (cost %d).", 3 * plev, 10+plev/3);
#endif

			info[i++] = Dummy;
			break;
		case RACE_KUTAR:
			if (plev > 19)
#ifdef JP
info[i++] = "あなたは d20+30 ターンの間横に伸びることができる。(15 MP)";
#else
				info[i++] = "You can expand horizontally, dur d20+30 (cost 15).";
#endif

			break;
/*TODO
		case RACE_ANDROID:
			if (plev < 10)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのレイガンを撃つことができる。(7 MP)", (plev + 1) / 2);
#else
				sprintf(Dummy, "You can fire a ray gun with damage %d (cost 7).", (plev+1)/2);
#endif
			else if (plev < 25)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのブラスターを撃つことができる。(13 MP)", plev);
#else
				sprintf(Dummy, "You can fire a blaster with damage %d (cost 13).", plev);
#endif
			else if (plev < 35)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのバズーカを撃つことができる。(26 MP)", plev * 2);
#else
				sprintf(Dummy, "You can fire a bazooka with damage %d (cost 26).", plev * 2);
#endif
			else if (plev < 45)
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのビームキャノンを撃つことができる。(40 MP)", plev * 2);
#else
				sprintf(Dummy, "You can fire a beam cannon with damage %d (cost 40).", plev * 2);
#endif
			else
#ifdef JP
sprintf(Dummy, "あなたは %d ダメージのロケットを撃つことができる。(60 MP)", plev * 5);
#else
				sprintf(Dummy, "You can fire a rocket with damage %d (cost 60).", plev * 5);
#endif

			info[i++] = Dummy;
			break;
*/
		default:
			break;
	}
	}

	switch(creature_ptr->cls_idx)
	{
		case CLASS_WARRIOR:
			if (plev > 39)
			{
#ifdef JP
info[i++] = "あなたはランダムな方向に対して数回攻撃することができる。(75 MP)";
#else
				info[i++] = "You can attack some random directions at a time (cost 75).";
#endif
			}
			break;
		case CLASS_HIGH_MAGE:
			if (creature_ptr->realm1 == REALM_HEX) break;
		case CLASS_MAGE:
		case CLASS_SORCERER:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたはアイテムの魔力を吸収することができる。(1 MP)";
#else
				info[i++] = "You can absorb charges from an item (cost 1).";
#endif
			}
			break;
		case CLASS_PRIEST:
			if (is_good_realm(creature_ptr->realm1))
			{
				if (plev > 34)
				{
#ifdef JP
info[i++] = "あなたは武器を祝福することができる。(70 MP)";
#else
					info[i++] = "You can bless a weapon (cost 70).";
#endif
				}
			}
			else
			{
				if (plev > 41)
				{
#ifdef JP
info[i++] = "あなたは周りのすべてのモンスターを攻撃することができる。(40 MP)";
#else
					info[i++] = "You can damages all monsters in sight (cost 40).";
#endif
				}
			}
			break;
		case CLASS_ROGUE:
			if (plev > 7)
			{
#ifdef JP
info[i++] = "あなたは攻撃して即座に逃げることができる。(12 MP)";
#else
				info[i++] = "You can hit a monster and teleport at a time (cost 12).";
#endif
			}
			break;
		case CLASS_RANGER:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "あなたは怪物を調査することができる。(20 MP)";
#else
				info[i++] = "You can prove monsters (cost 20).";
#endif
			}
			break;
		case CLASS_PALADIN:
			if (is_good_realm(creature_ptr->realm1))
			{
				if (plev > 29)
				{
#ifdef JP
info[i++] = "あなたは聖なる槍を放つことができる。(30 MP)";
#else
					info[i++] = "You can fires a holy spear (cost 30).";
#endif
				}
			}
			else
			{
				if (plev > 29)
				{
#ifdef JP
info[i++] = "あなたは生命力を減少させる槍を放つことができる。(30 MP)";
#else
					info[i++] = "You can fires a spear which drains vitality (cost 30).";
#endif
				}
			}
			break;
		case CLASS_WARRIOR_MAGE:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたはＨＰをＭＰに変換することができる。(0 MP)";
#else
				info[i++] = "You can convert HP to SP (cost 0).";
#endif
#ifdef JP
info[i++] = "あなたはＭＰをＨＰに変換することができる。(0 MP)";
#else
				info[i++] = "You can convert SP to HP (cost 0).";
#endif
			}
			break;
		case CLASS_CHAOS_WARRIOR:
			if (plev > 39)
			{
#ifdef JP
info[i++] = "あなたは周囲に怪物を惑わす光を発生させることができる。(50 MP)";
#else
				info[i++] = "You can radiate light which confuses nearby monsters (cost 50).";
#endif
			}
			break;
		case CLASS_MONK:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたは構えることができる。(0 MP)";
#else
				info[i++] = "You can assume a posture of special form (cost 0).";
#endif
			}
			if (plev > 29)
			{
#ifdef JP
info[i++] = "あなたは通常の2倍の攻撃を行うことができる。(30 MP)";
#else
				info[i++] = "You can perform double attacks in a time (cost 30).";
#endif
			}
			break;
		case CLASS_MINDCRAFTER:
		case CLASS_FORCETRAINER:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "あなたは精神を集中してＭＰを回復させることができる。(0 MP)";
#else
				info[i++] = "You can concentrate to regenerate your mana (cost 0).";
#endif
			}
			break;
		case CLASS_TOURIST:
#ifdef JP
info[i++] = "あなたは写真を撮影することができる。(0 MP)";
#else
				info[i++] = "You can take a photograph (cost 0).";
#endif
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたはアイテムを完全に鑑定することができる。(20 MP)";
#else
				info[i++] = "You can *identify* items (cost 20).";
#endif
			}
			break;
		case CLASS_IMITATOR:
			if (plev > 29)
			{
#ifdef JP
info[i++] = "あなたは怪物の特殊攻撃をダメージ2倍でまねることができる。(100 MP)";
#else
				info[i++] = "You can imitate monster's special attacks with double damage (cost 100).";
#endif
			}
			break;
		case CLASS_BEASTMASTER:
#ifdef JP
info[i++] = "あなたは1体の生命のあるモンスターを支配することができる。(レベル/4 MP)";
#else
			info[i++] = "You can dominate a monster (cost level/4).";
#endif
			if (plev > 29)
			{
#ifdef JP
info[i++] = "あなたは視界内の生命のあるモンスターを支配することができる。((レベル+20)/2 MP)";
#else
				info[i++] = "You can dominate living monsters in sight (cost (level+20)/4).";
#endif
			}
			break;
		case CLASS_MAGIC_EATER:
#ifdef JP
info[i++] = "あなたは杖/魔法棒/ロッドの魔力を自分のものにすることができる。";
#else
			info[i++] = "You can absorb a staff, wand or rod itself.";
#endif
			break;
		case CLASS_RED_MAGE:
			if (plev > 47)
			{
#ifdef JP
info[i++] = "あなたは1ターンに2回魔法を唱えることができる。(20 MP)";
#else
				info[i++] = "You can cast two spells in one time (cost 20).";
#endif
			}
			break;
		case CLASS_SAMURAI:
			{
#ifdef JP
info[i++] = "あなたは精神を集中して気合いを溜めることができる。";
#else
				info[i++] = "You can concentrate to regenerate your mana.";
#endif
			}
			if (plev > 24)
			{
#ifdef JP
info[i++] = "あなたは特殊な型で構えることができる。";
#else
				info[i++] = "You can assume a posture of special form.";
#endif
			}
			break;
		case CLASS_BLUE_MAGE:
#ifdef JP
info[i++] = "あなたは相手に使われた魔法を学ぶことができる。";
#else
			info[i++] = "You can study spells which your enemy casts on you.";
#endif
			break;
		case CLASS_CAVALRY:
			if (plev > 9)
			{
#ifdef JP
info[i++] = "あなたはモンスターに乗って無理矢理ペットにすることができる。";
#else
				info[i++] = "You can ride on a hostile monster forcibly to turn it into pet.";
#endif
			}
			break;
		case CLASS_BERSERKER:
			if (plev > 9)
			{
#ifdef JP
info[i++] = "あなたは街とダンジョンの間を行き来することができる。";
#else
			info[i++] = "You can travel between town and the depths.";
#endif
			}
			break;
		case CLASS_MIRROR_MASTER:
#ifdef JP
info[i++] = "あなたは鏡を作り出すことができる。(2 MP)";
#else
				info[i++] = "You can create a Mirror (cost 2).";
#endif
#ifdef JP
info[i++] = "あなたは鏡を割ることができる。(0 MP)";
#else
				info[i++] = "You can break distant Mirrors (cost 0).";
#endif
			break;
		case CLASS_NINJA:
			if (plev > 19)
			{
#ifdef JP
info[i++] = "あなたは素早く移動することができる。";
#else
				info[i++] = "You can walk extremery fast.";
#endif
			}
			break;
	}

	if (creature_ptr->flags12)
	{
		if (has_cf_creature(creature_ptr, CF_SPIT_ACID))
		{
#ifdef JP
info[i++] = "あなたは酸を吹きかけることができる。(ダメージ レベルX1)";
#else
			info[i++] = "You can spit acid (dam lvl).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_BR_FIRE))
		{
#ifdef JP
info[i++] = "あなたは炎のブレスを吐くことができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can breathe fire (dam lvl * 2).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_HYPN_GAZE))
		{
#ifdef JP
info[i++] = "あなたの睨みは催眠効果をもつ。";
#else
			info[i++] = "Your gaze is hypnotic.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_TELEKINES))
		{
#ifdef JP
info[i++] = "あなたは念動力をもっている。";
#else
			info[i++] = "You are telekinetic.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_VTELEPORT))
		{
#ifdef JP
info[i++] = "あなたは自分の意思でテレポートできる。";
#else
			info[i++] = "You can teleport at will.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_MIND_BLST))
		{
#ifdef JP
info[i++] = "あなたは精神攻撃を行える。(ダメージ 3～12d3)";
#else
			info[i++] = "You can Mind Blast your enemies (3 to 12d3 dam).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_RADIATION))
		{
#ifdef JP
info[i++] = "あなたは自分の意思で強い放射線を発生することができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can emit hard radiation at will (dam lvl * 2).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_VAMPIRISM))
		{
#ifdef JP
info[i++] = "あなたは吸血鬼のように敵から生命力を吸収することができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can drain life from a foe like a vampire (dam lvl * 2).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SMELL_MET))
		{
#ifdef JP
info[i++] = "あなたは近くにある貴金属をかぎ分けることができる。";
#else
			info[i++] = "You can smell nearby precious metal.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SMELL_MON))
		{
#ifdef JP
info[i++] = "あなたは近くのモンスターの存在をかぎ分けることができる。";
#else
			info[i++] = "You can smell nearby monsters.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_BLINK))
		{
#ifdef JP
info[i++] = "あなたは短い距離をテレポートできる。";
#else
			info[i++] = "You can teleport yourself short distances.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_EAT_ROCK))
		{
#ifdef JP
info[i++] = "あなたは硬い岩を食べることができる。";
#else
			info[i++] = "You can consume solid rock.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SWAP_POS))
		{
#ifdef JP
info[i++] = "あなたは他の者と場所を入れ替わることができる。";
#else
			info[i++] = "You can switch locations with another being.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SHRIEK))
		{
#ifdef JP
info[i++] = "あなたは身の毛もよだつ叫び声を発することができる。(ダメージ レベルX2)";
#else
			info[i++] = "You can emit a horrible shriek (dam 2 * lvl).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ILLUMINE))
		{
#ifdef JP
info[i++] = "あなたは明るい光を放つことができる。";
#else
			info[i++] = "You can emit bright light.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_DET_CURSE))
		{
#ifdef JP
info[i++] = "あなたは邪悪な魔法の危険を感じとることができる。";
#else
			info[i++] = "You can feel the danger of evil magic.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_BERSERK))
		{
#ifdef JP
info[i++] = "あなたは自分の意思で狂乱戦闘状態になることができる。";
#else
			info[i++] = "You can drive yourself into a berserk frenzy.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_POLYMORPH))
		{
#ifdef JP
info[i++] = "あなたは自分の意志で変化できる。";
#else
			info[i++] = "You can polymorph yourself at will.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_MIDAS_TCH))
		{
#ifdef JP
info[i++] = "あなたは通常アイテムを金に変えることができる。";
#else
			info[i++] = "You can turn ordinary items to gold.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_GROW_MOLD))
		{
#ifdef JP
info[i++] = "あなたは周囲にキノコを生やすことができる。";
#else
			info[i++] = "You can cause mold to grow near you.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_RESIST))
		{
#ifdef JP
info[i++] = "あなたは元素の攻撃に対して身を硬くすることができる。";
#else
			info[i++] = "You can harden yourself to the ravages of the elements.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_EARTHQUAKE))
		{
#ifdef JP
info[i++] = "あなたは周囲のダンジョンを崩壊させることができる。";
#else
			info[i++] = "You can bring down the dungeon around your ears.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_EAT_MAGIC))
		{
#ifdef JP
info[i++] = "あなたは魔法のエネルギーを自分の物として使用できる。";
#else
			info[i++] = "You can consume magic energy for your own use.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_WEIGH_MAG))
		{
#ifdef JP
info[i++] = "あなたは自分に影響を与える魔法の力を感じることができる。";
#else
			info[i++] = "You can feel the strength of the magics affecting you.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_STERILITY))
		{
#ifdef JP
info[i++] = "あなたは集団的生殖不能を起こすことができる。";
#else
			info[i++] = "You can cause mass impotence.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_PANIC_HIT))
		{
#ifdef JP
info[i++] = "あなたは攻撃した後身を守るため逃げることができる。";
#else
			info[i++] = "You can run for your life after hitting something.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_DAZZLE))
		{
#ifdef JP
info[i++] = "あなたは混乱と盲目を引き起こす放射能を発生することができる。 ";
#else
			info[i++] = "You can emit confusing, blinding radiation.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_LASER_EYE))
		{
#ifdef JP
info[i++] = "あなたは目からレーザー光線を発することができる。(ダメージ レベルX2)";
#else
			info[i++] = "Your eyes can fire laser beams (dam 2 * lvl).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_RECALL))
		{
#ifdef JP
info[i++] = "あなたは街とダンジョンの間を行き来することができる。";
#else
			info[i++] = "You can travel between town and the depths.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_BANISH))
		{
#ifdef JP
info[i++] = "あなたは邪悪なモンスターを地獄に落とすことができる。";
#else
			info[i++] = "You can send evil creatures directly to Hell.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_COLD_TOUCH))
		{
#ifdef JP
info[i++] = "あなたは敵を触って凍らせることができる。(ダメージ レベルX3)";
#else
			info[i++] = "You can freeze things with a touch (dam 3 * lvl).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_LAUNCHER))
		{
#ifdef JP
info[i++] = "あなたはアイテムを力強く投げることができる。";
#else
			info[i++] = "You can hurl objects with great force.";
#endif

		}
	}

	if (creature_ptr->flags13)
	{
		if (has_cf_creature(creature_ptr, CF_BERS_RAGE))
		{
#ifdef JP
info[i++] = "あなたは狂戦士化の発作を起こす。";
#else
			info[i++] = "You are subject to berserker fits.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_COWARDICE))
		{
#ifdef JP
info[i++] = "あなたは時々臆病になる。";
#else
			info[i++] = "You are subject to cowardice.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_RTELEPORT))
		{
#ifdef JP
info[i++] = "あなたはランダムにテレポートする。";
#else
			info[i++] = "You are teleporting randomly.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ALCOHOL))
		{
#ifdef JP
info[i++] = "あなたの体はアルコールを分泌する。";
#else
			info[i++] = "Your body produces alcohol.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_HALLU))
		{
#ifdef JP
info[i++] = "あなたは幻覚を引き起こす精神錯乱に侵されている。";
#else
			info[i++] = "You have a hallucinatory insanity.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_FLATULENT))
		{
#ifdef JP
info[i++] = "あなたは制御できない強烈な屁をこく。";
#else
			info[i++] = "You are subject to uncontrollable flatulence.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_PROD_MANA))
		{
#ifdef JP
info[i++] = "あなたは制御不能な魔法のエネルギーを発している。";
#else
			info[i++] = "You are producing magical energy uncontrollably.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ATT_DEMON))
		{
#ifdef JP
info[i++] = "あなたはデーモンを引きつける。";
#else
			info[i++] = "You attract demons.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SCOR_TAIL))
		{
#ifdef JP
info[i++] = "あなたはサソリの尻尾が生えている。(毒、ダメージ 3d7)";
#else
			info[i++] = "You have a scorpion tail (poison, 3d7).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_HORNS))
		{
#ifdef JP
info[i++] = "あなたは角が生えている。(ダメージ 2d6)";
#else
			info[i++] = "You have horns (dam. 2d6).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_BEAK))
		{
#ifdef JP
info[i++] = "あなたはクチバシが生えている。(ダメージ 2d4)";
#else
			info[i++] = "You have a beak (dam. 2d4).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SPEED_FLUX))
		{
#ifdef JP
info[i++] = "あなたはランダムに早く動いたり遅く動いたりする。";
#else
			info[i++] = "You move faster or slower randomly.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_BANISH_ALL))
		{
#ifdef JP
info[i++] = "あなたは時々近くのモンスターを消滅させる。";
#else
			info[i++] = "You sometimes cause nearby creatures to vanish.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_EAT_LIGHT))
		{
#ifdef JP
info[i++] = "あなたは時々周囲の光を吸収して栄養にする。";
#else
			info[i++] = "You sometimes feed off of the light around you.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_TRUNK))
		{
#ifdef JP
info[i++] = "あなたは象のような鼻を持っている。(ダメージ 1d4)";
#else
			info[i++] = "You have an elephantine trunk (dam 1d4).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ATT_ANIMAL))
		{
#ifdef JP
info[i++] = "あなたは動物を引きつける。";
#else
			info[i++] = "You attract animals.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_TENTACLES))
		{
#ifdef JP
info[i++] = "あなたは邪悪な触手を持っている。(ダメージ 2d5)";
#else
			info[i++] = "You have evil looking tentacles (dam 2d5).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_RAW_CHAOS))
		{
#ifdef JP
info[i++] = "あなたはしばしば純カオスに包まれる。";
#else
			info[i++] = "You occasionally are surrounded with raw chaos.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_NORMALITY))
		{
#ifdef JP
info[i++] = "あなたは変異していたが、回復してきている。";
#else
			info[i++] = "You may be mutated, but you're recovering.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_WRAITH))
		{
#ifdef JP
info[i++] = "あなたの肉体は幽体化したり実体化したりする。";
#else
			info[i++] = "You fade in and out of physical reality.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_POLY_WOUND))
		{
#ifdef JP
info[i++] = "あなたの健康はカオスの力に影響を受ける。";
#else
			info[i++] = "Your health is subject to chaotic forces.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_WASTING))
		{
#ifdef JP
info[i++] = "あなたは衰弱する恐ろしい病気にかかっている。";
#else
			info[i++] = "You have a horrible wasting disease.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ATT_DRAGON))
		{
#ifdef JP
info[i++] = "あなたはドラゴンを引きつける。";
#else
			info[i++] = "You attract dragons.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_WEIRD_MIND))
		{
#ifdef JP
info[i++] = "あなたの精神はランダムに拡大したり縮小したりしている。";
#else
			info[i++] = "Your mind randomly expands and contracts.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_NAUSEA))
		{
#ifdef JP
info[i++] = "あなたの胃は非常に落ち着きがない。";
#else
			info[i++] = "You have a seriously upset stomach.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_CHAOS_GIFT))
		{
#ifdef JP
info[i++] = "あなたはカオスの守護悪魔から褒美をうけとる。";
#else
			info[i++] = "Chaos deities give you gifts.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_WALK_SHAD))
		{
#ifdef JP
info[i++] = "あなたはしばしば他の「影」に迷い込む。";
#else
			info[i++] = "You occasionally stumble into other shadows.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_WARNING))
		{
#ifdef JP
info[i++] = "あなたは敵に関する警告を感じる。";
#else
			info[i++] = "You receive warnings about your foes.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_INVULN))
		{
#ifdef JP
info[i++] = "あなたは時々負け知らずな気分になる。";
#else
			info[i++] = "You occasionally feel invincible.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SP_TO_HP))
		{
#ifdef JP
info[i++] = "あなたは時々血が筋肉にどっと流れる。";
#else
			info[i++] = "Your blood sometimes rushes to your muscles.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_HP_TO_SP))
		{
#ifdef JP
info[i++] = "あなたは時々頭に血がどっと流れる。";
#else
			info[i++] = "Your blood sometimes rushes to your head.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_DISARM))
		{
#ifdef JP
info[i++] = "あなたはよくつまづいて物を落とす。";
#else
			info[i++] = "You occasionally stumble and drop things.";
#endif

		}
	}

	if (creature_ptr->flags14)
	{
		if (has_cf_creature(creature_ptr, CF_HYPER_STR))
		{
#ifdef JP
info[i++] = "あなたは超人的に強い。(腕力+4)";
#else
			info[i++] = "You are superhumanly strong (+4 STR).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_PUNY))
		{
#ifdef JP
info[i++] = "あなたは虚弱だ。(腕力-4)";
#else
			info[i++] = "You are puny (-4 STR).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_HYPER_INT))
		{
#ifdef JP
info[i++] = "あなたの脳は生体コンピュータだ。(知能＆賢さ+4)";
#else
			info[i++] = "Your brain is a living computer (+4 INT/WIS).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_MORONIC))
		{
#ifdef JP
info[i++] = "あなたは精神薄弱だ。(知能＆賢さ-4)";
#else
			info[i++] = "You are moronic (-4 INT/WIS).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_RESILIENT))
		{
#ifdef JP
info[i++] = "あなたは非常にタフだ。(耐久+4)";
#else
			info[i++] = "You are very resilient (+4 CON).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_XTRA_FAT))
		{
#ifdef JP
info[i++] = "あなたは極端に太っている。(耐久+2,スピード-2)";
#else
			info[i++] = "You are extremely fat (+2 CON, -2 speed).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ALBINO))
		{
#ifdef JP
info[i++] = "あなたはアルビノだ。(耐久-4)";
#else
			info[i++] = "You are albino (-4 CON).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_FLESH_ROT))
		{
#ifdef JP
info[i++] = "あなたの肉体は腐敗している。(耐久-2,魅力-1)";
#else
			info[i++] = "Your flesh is rotting (-2 CON, -1 CHR).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SILLY_VOI))
		{
#ifdef JP
info[i++] = "あなたの声は間抜けなキーキー声だ。(魅力-4)";
#else
			info[i++] = "Your voice is a silly squeak (-4 CHR).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_BLANK_FAC))
		{
#ifdef JP
info[i++] = "あなたはのっぺらぼうだ。(魅力-1)";
#else
			info[i++] = "Your face is featureless (-1 CHR).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ILL_NORM))
		{
#ifdef JP
info[i++] = "あなたは幻影に覆われている。";
#else
			info[i++] = "Your appearance is masked with illusion.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_XTRA_EYES))
		{
#ifdef JP
info[i++] = "あなたは余分に二つの目を持っている。(探索+15)";
#else
			info[i++] = "You have an extra pair of eyes (+15 search).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_MAGIC_RES))
		{
#ifdef JP
info[i++] = "あなたは魔法への耐性をもっている。";
#else
			info[i++] = "You are resistant to magic.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_XTRA_NOIS))
		{
#ifdef JP
info[i++] = "あなたは変な音を発している。(隠密-3)";
#else
			info[i++] = "You make a lot of strange noise (-3 stealth).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_INFRAVIS))
		{
#ifdef JP
info[i++] = "あなたは素晴らしい赤外線視力を持っている。(+3)";
#else
			info[i++] = "You have remarkable infravision (+3).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_XTRA_LEGS))
		{
#ifdef JP
info[i++] = "あなたは余分に二本の足が生えている。(加速+3)";
#else
			info[i++] = "You have an extra pair of legs (+3 speed).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SHORT_LEG))
		{
#ifdef JP
info[i++] = "あなたの足は短い突起だ。(加速-3)";
#else
			info[i++] = "Your legs are short stubs (-3 speed).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ELEC_TOUC))
		{
#ifdef JP
info[i++] = "あなたの血管には電流が流れている。";
#else
			info[i++] = "Electricity is running through your veins.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_FIRE_BODY))
		{
#ifdef JP
info[i++] = "あなたの体は炎につつまれている。";
#else
			info[i++] = "Your body is enveloped in flames.";
#endif
		}
		if (has_cf_creature(creature_ptr, CF_WART_SKIN))
		{
#ifdef JP
info[i++] = "あなたの肌はイボに被われている。(魅力-2, AC+5)";
#else
			info[i++] = "Your skin is covered with warts (-2 CHR, +5 AC).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_SCALES))
		{
#ifdef JP
info[i++] = "あなたの肌は鱗になっている。(魅力-1, AC+10)";
#else
			info[i++] = "Your skin has turned into scales (-1 CHR, +10 AC).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_IRON_SKIN))
		{
#ifdef JP
info[i++] = "あなたの肌は鉄でできている。(器用-1, AC+25)";
#else
			info[i++] = "Your skin is made of steel (-1 DEX, +25 AC).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_WINGS))
		{
#ifdef JP
info[i++] = "あなたは羽を持っている。";
#else
			info[i++] = "You have wings.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_FEARLESS))
		{
			/* Unnecessary */
		}
		if (has_cf_creature(creature_ptr, CF_REGEN))
		{
			/* Unnecessary */
		}
		if (has_cf_creature(creature_ptr, CF_ESP))
		{
			/* Unnecessary */
		}
		if (has_cf_creature(creature_ptr, CF_LIMBER))
		{
#ifdef JP
info[i++] = "あなたの体は非常にしなやかだ。(器用+3)";
#else
			info[i++] = "Your body is very limber (+3 DEX).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_ARTHRITIS))
		{
#ifdef JP
info[i++] = "あなたはいつも関節に痛みを感じている。(器用-3)";
#else
			info[i++] = "Your joints ache constantly (-3 DEX).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_VULN_ELEM))
		{
#ifdef JP
info[i++] = "あなたは元素の攻撃に弱い。";
#else
			info[i++] = "You are susceptible to damage from the elements.";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_MOTION))
		{
#ifdef JP
info[i++] = "あなたの動作は正確で力強い。(隠密+1)";
#else
			info[i++] = "Your movements are precise and forceful (+1 STL).";
#endif

		}
		if (has_cf_creature(creature_ptr, CF_GOOD_LUCK))
		{
#ifdef JP
info[i++] = "あなたは白いオーラにつつまれている。";
#else
			info[i++] = "There is a white aura surrounding you.";
#endif
		}
		if (has_cf_creature(creature_ptr, CF_BAD_LUCK))
		{
#ifdef JP
info[i++] = "あなたは黒いオーラにつつまれている。";
#else
			info[i++] = "There is a black aura surrounding you.";
#endif
		}
	}

	if (creature_ptr->blind)
	{
#ifdef JP
info[i++] = "あなたは目が見えない。";
#else
		info[i++] = "You cannot see.";
#endif

	}
	if (creature_ptr->confused)
	{
#ifdef JP
info[i++] = "あなたは混乱している。";
#else
		info[i++] = "You are confused.";
#endif

	}
	if (creature_ptr->afraid)
	{
#ifdef JP
info[i++] = "あなたは恐怖に侵されている。";
#else
		info[i++] = "You are terrified.";
#endif

	}
	if (creature_ptr->cut)
	{
#ifdef JP
info[i++] = "あなたは出血している。";
#else
		info[i++] = "You are bleeding.";
#endif

	}
	if (creature_ptr->stun)
	{
#ifdef JP
info[i++] = "あなたはもうろうとしている。";
#else
		info[i++] = "You are stunned.";
#endif

	}
	if (creature_ptr->poisoned)
	{
#ifdef JP
info[i++] = "あなたは毒に侵されている。";
#else
		info[i++] = "You are poisoned.";
#endif

	}
	if (creature_ptr->image)
	{
#ifdef JP
info[i++] = "あなたは幻覚を見ている。";
#else
		info[i++] = "You are hallucinating.";
#endif

	}
	if (creature_ptr->cursed & TRC_TY_CURSE)
	{
#ifdef JP
info[i++] = "あなたは邪悪な怨念に包まれている。";
#else
		info[i++] = "You carry an ancient foul curse.";
#endif

	}
	if (creature_ptr->cursed & TRC_AGGRAVATE)
	{
#ifdef JP
info[i++] = "あなたはモンスターを怒らせている。";
#else
		info[i++] = "You aggravate monsters.";
#endif

	}
	if (creature_ptr->cursed & TRC_DRAIN_EXP)
	{
#ifdef JP
info[i++] = "あなたは経験値を吸われている。";
#else
		info[i++] = "You are drained.";
#endif

	}
	if (creature_ptr->cursed & TRC_SLOW_REGEN)
	{
#ifdef JP
info[i++] = "あなたの回復力は非常に遅い。";
#else
		info[i++] = "You regenerate slowly.";
#endif

	}
	if (creature_ptr->cursed & TRC_ADD_L_CURSE)
	{
#ifdef JP
info[i++] = "あなたの弱い呪いは増える。"; /* 暫定的 -- henkma */
#else
		info[i++] = "Your weak curses multiply.";
#endif

	}
	if (creature_ptr->cursed & TRC_ADD_H_CURSE)
	{
#ifdef JP
info[i++] = "あなたの強い呪いは増える。"; /* 暫定的 -- henkma */
#else
		info[i++] = "Your heavy curses multiply.";
#endif

	}
	if (creature_ptr->cursed & TRC_CALL_ANIMAL)
	{
#ifdef JP
info[i++] = "あなたは動物に狙われている。";
#else
		info[i++] = "You attract animals.";
#endif

	}
	if (creature_ptr->cursed & TRC_CALL_DEMON)
	{
#ifdef JP
info[i++] = "あなたは悪魔に狙われている。";
#else
		info[i++] = "You attract demons.";
#endif

	}
	if (creature_ptr->cursed & TRC_CALL_DRAGON)
	{
#ifdef JP
info[i++] = "あなたはドラゴンに狙われている。";
#else
		info[i++] = "You attract dragons.";
#endif

	}
	if (creature_ptr->cursed & TRC_COWARDICE)
	{
#ifdef JP
info[i++] = "あなたは時々臆病になる。";
#else
		info[i++] = "You are subject to cowardice.";
#endif

	}
	if (creature_ptr->cursed & TRC_TELEPORT)
	{
#ifdef JP
info[i++] = "あなたの位置はひじょうに不安定だ。";
#else
		info[i++] = "Your position is very uncertain.";
#endif

	}
	if (creature_ptr->cursed & TRC_LOW_MELEE)
	{
#ifdef JP
info[i++] = "あなたの武器は攻撃を外しやすい。";
#else
		info[i++] = "Your weapon causes you to miss blows.";
#endif

	}
	if (creature_ptr->cursed & TRC_LOW_AC)
	{
#ifdef JP
info[i++] = "あなたは攻撃を受けやすい。";
#else
		info[i++] = "You are subject to be hit.";
#endif

	}
	if (creature_ptr->cursed & TRC_LOW_MAGIC)
	{
#ifdef JP
info[i++] = "あなたは魔法を失敗しやすい。";
#else
		info[i++] = "You are subject to fail spellcasting.";
#endif

	}
	if (creature_ptr->cursed & TRC_FAST_DIGEST)
	{
#ifdef JP
info[i++] = "あなたはすぐお腹がへる。";
#else
		info[i++] = "You have a good appetite.";
#endif

	}
	if (creature_ptr->cursed & TRC_DRAIN_HP)
	{
#ifdef JP
info[i++] = "あなたは体力を吸われている。";
#else
		info[i++] = "You are drained.";
#endif

	}
	if (creature_ptr->cursed & TRC_DRAIN_MANA)
	{
#ifdef JP
info[i++] = "あなたは魔力を吸われている。";
#else
		info[i++] = "You brain is drained.";
#endif

	}
	if (IS_BLESSED(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは公正さを感じている。";
#else
		info[i++] = "You feel rightous.";
#endif

	}
	if (IS_HERO(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたはヒーロー気分だ。";
#else
		info[i++] = "You feel heroic.";
#endif

	}
	if (creature_ptr->shero)
	{
#ifdef JP
info[i++] = "あなたは戦闘狂だ。";
#else
		info[i++] = "You are in a battle rage.";
#endif

	}
	if (creature_ptr->protevil)
	{
#ifdef JP
info[i++] = "あなたは邪悪なる存在から守られている。";
#else
		info[i++] = "You are protected from evil.";
#endif

	}
	if (creature_ptr->shield)
	{
#ifdef JP
info[i++] = "あなたは神秘のシールドで守られている。";
#else
		info[i++] = "You are protected by a mystic shield.";
#endif

	}
	if (IS_INVULN(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは現在傷つかない。";
#else
		info[i++] = "You are temporarily invulnerable.";
#endif

	}
	if (creature_ptr->wraith_form)
	{
#ifdef JP
info[i++] = "あなたは一時的に幽体化している。";
#else
		info[i++] = "You are temporarily incorporeal.";
#endif

	}
	if (creature_ptr->special_attack & ATTACK_CONFUSE)
	{
#ifdef JP
info[i++] = "あなたの手は赤く輝いている。";
#else
		info[i++] = "Your hands are glowing dull red.";
#endif

	}
	if (creature_ptr->special_attack & ATTACK_FIRE)
	{
#ifdef JP
info[i++] = "あなたの手は火炎に覆われている。";
#else
		info[i++] = "You can strike the enemy with flame.";
#endif

	}
	if (creature_ptr->special_attack & ATTACK_COLD)
	{
#ifdef JP
info[i++] = "あなたの手は冷気に覆われている。";
#else
		info[i++] = "You can strike the enemy with cold.";
#endif

	}
	if (creature_ptr->special_attack & ATTACK_ACID)
	{
#ifdef JP
info[i++] = "あなたの手は酸に覆われている。";
#else
		info[i++] = "You can strike the enemy with acid.";
#endif

	}
	if (creature_ptr->special_attack & ATTACK_ELEC)
	{
#ifdef JP
info[i++] = "あなたの手は電撃に覆われている。";
#else
		info[i++] = "You can strike the enemy with electoric shock.";
#endif

	}
	if (creature_ptr->special_attack & ATTACK_POIS)
	{
#ifdef JP
info[i++] = "あなたの手は毒に覆われている。";
#else
		info[i++] = "You can strike the enemy with poison.";
#endif

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
	if (creature_ptr->new_spells)
	{
#ifdef JP
info[i++] = "あなたは呪文や祈りを学ぶことができる。";
#else
		info[i++] = "You can learn some spells/prayers.";
#endif

	}
	if (creature_ptr->word_recall)
	{
#ifdef JP
info[i++] = "あなたはすぐに帰還するだろう。";
#else
		info[i++] = "You will soon be recalled.";
#endif

	}
	if (creature_ptr->alter_reality)
	{
#ifdef JP
		info[i++] = "あなたはすぐにこの世界を離れるだろう。";
#else
		info[i++] = "You will soon be altered.";
#endif

	}
	if (creature_ptr->see_infra)
	{
#ifdef JP
info[i++] = "あなたの瞳は赤外線に敏感である。";
#else
		info[i++] = "Your eyes are sensitive to infrared light.";
#endif

	}
	if (creature_ptr->see_inv)
	{
#ifdef JP
info[i++] = "あなたは透明なモンスターを見ることができる。";
#else
		info[i++] = "You can see invisible creatures.";
#endif

	}
	if (creature_ptr->levitation)
	{
#ifdef JP
info[i++] = "あなたは飛ぶことができる。";
#else
		info[i++] = "You can fly.";
#endif

	}
	if (creature_ptr->free_act)
	{
#ifdef JP
info[i++] = "あなたは麻痺知らずの効果を持っている。";
#else
		info[i++] = "You have free action.";
#endif

	}
	if (creature_ptr->regenerate)
	{
#ifdef JP
info[i++] = "あなたは素早く体力を回復する。";
#else
		info[i++] = "You regenerate quickly.";
#endif

	}
	if (creature_ptr->slow_digest)
	{
#ifdef JP
info[i++] = "あなたは食欲が少ない。";
#else
		info[i++] = "Your appetite is small.";
#endif

	}
	if (creature_ptr->telepathy)
	{
#ifdef JP
info[i++] = "あなたはテレパシー能力を持っている。";
#else
		info[i++] = "You have ESP.";
#endif

	}
	if (creature_ptr->esp_animal)
	{
#ifdef JP
info[i++] = "あなたは自然界の生物の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense natural creatures.";
#endif

	}
	if (creature_ptr->esp_undead)
	{
#ifdef JP
info[i++] = "あなたはアンデッドの存在を感じる能力を持っている。";
#else
		info[i++] = "You sense undead.";
#endif

	}
	if (creature_ptr->esp_demon)
	{
#ifdef JP
info[i++] = "あなたは悪魔の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense demons.";
#endif

	}
	if (creature_ptr->esp_orc)
	{
#ifdef JP
info[i++] = "あなたはオークの存在を感じる能力を持っている。";
#else
		info[i++] = "You sense orcs.";
#endif

	}
	if (creature_ptr->esp_troll)
	{
#ifdef JP
info[i++] = "あなたはトロルの存在を感じる能力を持っている。";
#else
		info[i++] = "You sense trolls.";
#endif

	}
	if (creature_ptr->esp_giant)
	{
#ifdef JP
info[i++] = "あなたは巨人の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense giants.";
#endif

	}
	if (creature_ptr->esp_dragon)
	{
#ifdef JP
info[i++] = "あなたはドラゴンの存在を感じる能力を持っている。";
#else
		info[i++] = "You sense dragons.";
#endif

	}
	if (creature_ptr->esp_human)
	{
#ifdef JP
info[i++] = "あなたは人間の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense humans.";
#endif

	}
	if (creature_ptr->esp_evil)
	{
#ifdef JP
info[i++] = "あなたは邪悪な生き物の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense evil creatures.";
#endif

	}
	if (creature_ptr->esp_good)
	{
#ifdef JP
info[i++] = "あなたは善良な生き物の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense good creatures.";
#endif

	}
	if (creature_ptr->esp_nonliving)
	{
#ifdef JP
info[i++] = "あなたは活動する無生物体の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense non-living creatures.";
#endif

	}
	if (creature_ptr->esp_unique)
	{
#ifdef JP
info[i++] = "あなたは特別な強敵の存在を感じる能力を持っている。";
#else
		info[i++] = "You sense unique monsters.";
#endif

	}
	if (creature_ptr->hold_life)
	{
#ifdef JP
info[i++] = "あなたは自己の生命力をしっかりと維持する。";
#else
		info[i++] = "You have a firm hold on your life force.";
#endif

	}
	if (creature_ptr->reflect)
	{
#ifdef JP
info[i++] = "あなたは矢やボルトを反射する。";
#else
		info[i++] = "You reflect arrows and bolts.";
#endif

	}
	if (creature_ptr->sh_fire)
	{
#ifdef JP
info[i++] = "あなたは炎のオーラに包まれている。";
#else
		info[i++] = "You are surrounded with a fiery aura.";
#endif

	}
	if (creature_ptr->sh_elec)
	{
#ifdef JP
info[i++] = "あなたは電気に包まれている。";
#else
		info[i++] = "You are surrounded with electricity.";
#endif

	}
	if (creature_ptr->sh_cold)
	{
#ifdef JP
info[i++] = "あなたは冷気のオーラに包まれている。";
#else
		info[i++] = "You are surrounded with an aura of coldness.";
#endif

	}
	if (creature_ptr->tim_sh_holy)
	{
#ifdef JP
info[i++] = "あなたは聖なるオーラに包まれている。";
#else
		info[i++] = "You are surrounded with a holy aura.";
#endif

	}
	if (creature_ptr->tim_sh_touki)
	{
#ifdef JP
info[i++] = "あなたは闘気のオーラに包まれている。";
#else
		info[i++] = "You are surrounded with a energy aura.";
#endif

	}
	if (creature_ptr->anti_magic)
	{
#ifdef JP
info[i++] = "あなたは反魔法シールドに包まれている。";
#else
		info[i++] = "You are surrounded by an anti-magic shell.";
#endif

	}
	if (creature_ptr->anti_tele)
	{
#ifdef JP
info[i++] = "あなたはテレポートできない。";
#else
		info[i++] = "You cannot teleport.";
#endif

	}
	if (creature_ptr->lite)
	{
#ifdef JP
info[i++] = "あなたの身体は光っている。";
#else
		info[i++] = "You are carrying a permanent light.";
#endif

	}
	if (creature_ptr->warning)
	{
#ifdef JP
info[i++] = "あなたは行動の前に危険を察知することができる。";
#else
		info[i++] = "You will be warned before dangerous actions.";
#endif

	}
	if (creature_ptr->dec_mana)
	{
#ifdef JP
info[i++] = "あなたは少ない消費魔力で魔法を唱えることができる。";
#else
		info[i++] = "You can cast spells with fewer mana points.";
#endif

	}
	if (creature_ptr->easy_spell)
	{
#ifdef JP
info[i++] = "あなたは低い失敗率で魔法を唱えることができる。";
#else
		info[i++] = "Fail rate of your magic is decreased.";
#endif

	}
	if (creature_ptr->heavy_spell)
	{
#ifdef JP
info[i++] = "あなたは高い失敗率で魔法を唱えなければいけない。";
#else
		info[i++] = "Fail rate of your magic is increased.";
#endif

	}
	if (creature_ptr->mighty_throw)
	{
#ifdef JP
info[i++] = "あなたは強く物を投げる。";
#else
		info[i++] = "You can throw objects powerfully.";
#endif

	}

	if (creature_ptr->immune_acid)
	{
#ifdef JP
info[i++] = "あなたは酸に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to acid.";
#endif

	}
	else if (creature_ptr->resist_acid && IS_OPPOSE_ACID(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは酸への強力な耐性を持っている。";
#else
		info[i++] = "You resist acid exceptionally well.";
#endif

	}
	else if (creature_ptr->resist_acid || IS_OPPOSE_ACID(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは酸への耐性を持っている。";
#else
		info[i++] = "You are resistant to acid.";
#endif

	}

	if (creature_ptr->immune_elec)
	{
#ifdef JP
info[i++] = "あなたは電撃に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to lightning.";
#endif

	}
	else if (creature_ptr->resist_elec && IS_OPPOSE_ELEC(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは電撃への強力な耐性を持っている。";
#else
		info[i++] = "You resist lightning exceptionally well.";
#endif

	}
	else if (creature_ptr->resist_elec || IS_OPPOSE_ELEC(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは電撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to lightning.";
#endif

	}

	/*TODO
	if (race_is_(creature_ptr, RACE_ANDROID) && !creature_ptr->immune_elec)
	{
#ifdef JP
info[i++] = "あなたは電撃に弱い。";
#else
		info[i++] = "You are susceptible to damage from lightning.";
#endif

	}
	*/

	if (creature_ptr->immune_fire)
	{
#ifdef JP
info[i++] = "あなたは火に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to fire.";
#endif

	}
	else if (creature_ptr->resist_fire && IS_OPPOSE_FIRE(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは火への強力な耐性を持っている。";
#else
		info[i++] = "You resist fire exceptionally well.";
#endif

	}
	else if (creature_ptr->resist_fire || IS_OPPOSE_FIRE(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは火への耐性を持っている。";
#else
		info[i++] = "You are resistant to fire.";
#endif

	}

	if (race_is_(creature_ptr, RACE_ENT) && !creature_ptr->immune_fire)
	{
#ifdef JP
info[i++] = "あなたは火に弱い。";
#else
		info[i++] = "You are susceptible to damage from fire.";
#endif

	}

	if (creature_ptr->immune_cold)
	{
#ifdef JP
info[i++] = "あなたは冷気に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to cold.";
#endif

	}
	else if (creature_ptr->resist_cold && IS_OPPOSE_COLD(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは冷気への強力な耐性を持っている。";
#else
		info[i++] = "You resist cold exceptionally well.";
#endif

	}
	else if (creature_ptr->resist_cold || IS_OPPOSE_COLD(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは冷気への耐性を持っている。";
#else
		info[i++] = "You are resistant to cold.";
#endif

	}

	if (creature_ptr->resist_pois && IS_OPPOSE_POIS(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは毒への強力な耐性を持っている。";
#else
		info[i++] = "You resist poison exceptionally well.";
#endif

	}
	else if (creature_ptr->resist_pois || IS_OPPOSE_POIS(creature_ptr))
	{
#ifdef JP
info[i++] = "あなたは毒への耐性を持っている。";
#else
		info[i++] = "You are resistant to poison.";
#endif

	}

	if (creature_ptr->resist_lite)
	{
#ifdef JP
info[i++] = "あなたは閃光への耐性を持っている。";
#else
		info[i++] = "You are resistant to bright light.";
#endif

	}

	if (has_cf_creature(creature_ptr, CF_HURT_LITE))
	{
#ifdef JP
info[i++] = "あなたは閃光に弱い。";
#else
		info[i++] = "You are susceptible to damage from bright light.";
#endif

	}

	// TODO: Immune Darkness
	/*
	{
#ifdef JP
info[i++] = "あなたは暗黒に対する完全なる免疫を持っている。";
#else
		info[i++] = "You are completely immune to darkness.";
#endif
	}
	*/

	if (creature_ptr->resist_dark)
	{
#ifdef JP
info[i++] = "あなたは暗黒への耐性を持っている。";
#else
		info[i++] = "You are resistant to darkness.";
#endif

	}
	if (creature_ptr->resist_conf)
	{
#ifdef JP
info[i++] = "あなたは混乱への耐性を持っている。";
#else
		info[i++] = "You are resistant to confusion.";
#endif

	}
	if (creature_ptr->resist_sound)
	{
#ifdef JP
info[i++] = "あなたは音波の衝撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to sonic attacks.";
#endif

	}
	if (creature_ptr->resist_disen)
	{
#ifdef JP
info[i++] = "あなたは劣化への耐性を持っている。";
#else
		info[i++] = "You are resistant to disenchantment.";
#endif

	}
	if (creature_ptr->resist_chaos)
	{
#ifdef JP
info[i++] = "あなたはカオスの力への耐性を持っている。";
#else
		info[i++] = "You are resistant to chaos.";
#endif

	}
	if (creature_ptr->resist_shard)
	{
#ifdef JP
info[i++] = "あなたは破片の攻撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to blasts of shards.";
#endif

	}
	if (creature_ptr->resist_nexus)
	{
#ifdef JP
info[i++] = "あなたは因果混乱の攻撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to nexus attacks.";
#endif

	}

	//TODO
	/*
	if (race_is_(creature_ptr, RACE_LICH))
	{
#ifdef JP
info[i++] = "あなたは地獄の力を吸収できる。";
#else
		info[i++] = "You can drain nether forces.";
#endif

	}
	*/

	else if (creature_ptr->resist_neth)
	{
#ifdef JP
info[i++] = "あなたは地獄の力への耐性を持っている。";
#else
		info[i++] = "You are resistant to nether forces.";
#endif

	}
	if (creature_ptr->resist_fear)
	{
#ifdef JP
info[i++] = "あなたは全く恐怖を感じない。";
#else
		info[i++] = "You are completely fearless.";
#endif

	}
	if (creature_ptr->resist_blind)
	{
#ifdef JP
info[i++] = "あなたの目は盲目への耐性を持っている。";
#else
		info[i++] = "Your eyes are resistant to blindness.";
#endif

	}
	if (creature_ptr->resist_time)
	{
#ifdef JP
info[i++] = "あなたは時間逆転への耐性を持っている。";
#else
		info[i++] = "You are resistant to time.";
#endif

	}

	if (creature_ptr->sustain_str)
	{
#ifdef JP
info[i++] = "あなたの腕力は維持されている。";
#else
		info[i++] = "Your strength is sustained.";
#endif

	}
	if (creature_ptr->sustain_int)
	{
#ifdef JP
info[i++] = "あなたの知能は維持されている。";
#else
		info[i++] = "Your intelligence is sustained.";
#endif

	}
	if (creature_ptr->sustain_wis)
	{
#ifdef JP
info[i++] = "あなたの賢さは維持されている。";
#else
		info[i++] = "Your wisdom is sustained.";
#endif

	}
	if (creature_ptr->sustain_con)
	{
#ifdef JP
info[i++] = "あなたの耐久力は維持されている。";
#else
		info[i++] = "Your constitution is sustained.";
#endif

	}
	if (creature_ptr->sustain_dex)
	{
#ifdef JP
info[i++] = "あなたの器用さは維持されている。";
#else
		info[i++] = "Your dexterity is sustained.";
#endif

	}
	if (creature_ptr->sustain_chr)
	{
#ifdef JP
info[i++] = "あなたの魅力は維持されている。";
#else
		info[i++] = "Your charisma is sustained.";
#endif

	}

	if (have_flag(flgs, TR_STR))
	{
#ifdef JP
info[i++] = "あなたの腕力は装備によって影響を受けている。";
#else
		info[i++] = "Your strength is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_INT))
	{
#ifdef JP
info[i++] = "あなたの知能は装備によって影響を受けている。";
#else
		info[i++] = "Your intelligence is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_WIS))
	{
#ifdef JP
info[i++] = "あなたの賢さは装備によって影響を受けている。";
#else
		info[i++] = "Your wisdom is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_DEX))
	{
#ifdef JP
info[i++] = "あなたの器用さは装備によって影響を受けている。";
#else
		info[i++] = "Your dexterity is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_CON))
	{
#ifdef JP
info[i++] = "あなたの耐久力は装備によって影響を受けている。";
#else
		info[i++] = "Your constitution is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_CHR))
	{
#ifdef JP
info[i++] = "あなたの魅力は装備によって影響を受けている。";
#else
		info[i++] = "Your charisma is affected by your equipment.";
#endif

	}

	if (have_flag(flgs, TR_STEALTH))
	{
#ifdef JP
info[i++] = "あなたの隠密行動能力は装備によって影響を受けている。";
#else
		info[i++] = "Your stealth is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_SEARCH))
	{
#ifdef JP
info[i++] = "あなたの探索能力は装備によって影響を受けている。";
#else
		info[i++] = "Your searching ability is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_INFRA))
	{
#ifdef JP
info[i++] = "あなたの赤外線視力は装備によって影響を受けている。";
#else
		info[i++] = "Your infravision is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_TUNNEL))
	{
#ifdef JP
info[i++] = "あなたの採掘能力は装備によって影響を受けている。";
#else
		info[i++] = "Your digging ability is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_SPEED))
	{
#ifdef JP
info[i++] = "あなたのスピードは装備によって影響を受けている。";
#else
		info[i++] = "Your speed is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_BLOWS))
	{
#ifdef JP
info[i++] = "あなたの攻撃速度は装備によって影響を受けている。";
#else
		info[i++] = "Your attack speed is affected by your equipment.";
#endif

	}

	/* Access the current weapon */
	o_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 1);

	/* Analyze the weapon */
	if (o_ptr->k_idx)
	{
		/* Indicate Blessing */
		if (have_flag(flgs, TR_BLESSED))
		{
#ifdef JP
info[i++] = "あなたの武器は神の祝福を受けている。";
#else
			info[i++] = "Your weapon has been blessed by the gods.";
#endif

		}

		if (have_flag(flgs, TR_CHAOTIC))
		{
#ifdef JP
info[i++] = "あなたの武器はログルスの徴の属性をもつ。";
#else
			info[i++] = "Your weapon is branded with the Sign of Logrus.";
#endif

		}

		/* Hack */
		if (have_flag(flgs, TR_IMPACT))
		{
#ifdef JP
info[i++] = "あなたの武器は打撃で地震を発生することができる。";
#else
			info[i++] = "The impact of your weapon can cause earthquakes.";
#endif

		}

		if (have_flag(flgs, TR_VORPAL))
		{
#ifdef JP
info[i++] = "あなたの武器は非常に鋭い。";
#else
			info[i++] = "Your weapon is very sharp.";
#endif

		}

		if (have_flag(flgs, TR_VAMPIRIC))
		{
#ifdef JP
info[i++] = "あなたの武器は敵から生命力を吸収する。";
#else
			info[i++] = "Your weapon drains life from your foes.";
#endif

		}

		/* Special "Attack Bonuses" */
		if (have_flag(flgs, TR_BRAND_ACID))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を溶かす。";
#else
			info[i++] = "Your weapon melts your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_ELEC))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を感電させる。";
#else
			info[i++] = "Your weapon shocks your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_FIRE))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を燃やす。";
#else
			info[i++] = "Your weapon burns your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_COLD))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を凍らせる。";
#else
			info[i++] = "Your weapon freezes your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_POIS))
		{
#ifdef JP
info[i++] = "あなたの武器は敵を毒で侵す。";
#else
			info[i++] = "Your weapon poisons your foes.";
#endif

		}

		/* Special "slay" flags */
		if (have_flag(flgs, TR_KILL_ANIMAL))
		{
#ifdef JP
info[i++] = "あなたの武器は動物の天敵である。";
#else
			info[i++] = "Your weapon is a great bane of animals.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_ANIMAL))
		{
#ifdef JP
info[i++] = "あなたの武器は動物に対して強い力を発揮する。";
#else
			info[i++] = "Your weapon strikes at animals with extra force.";
#endif

		}
		if (have_flag(flgs, TR_KILL_EVIL))
		{
#ifdef JP
info[i++] = "あなたの武器は邪悪なる存在の天敵である。";
#else
			info[i++] = "Your weapon is a great bane of evil.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_EVIL))
		{
#ifdef JP
info[i++] = "あなたの武器は邪悪なる存在に対して強い力を発揮する。";
#else
			info[i++] = "Your weapon strikes at evil with extra force.";
#endif

		}

		if (have_flag(flgs, TR_KILL_GOOD))
		{
#ifdef JP
info[i++] = "あなたの武器は善良なる存在の天敵である。";
#else
			info[i++] = "Your weapon is a great bane of good.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_GOOD))
		{
#ifdef JP
info[i++] = "あなたの武器は善良なる存在に対して強い力を発揮する。";
#else
			info[i++] = "Your weapon strikes at good with extra force.";
#endif

		}

		if (have_flag(flgs, TR_KILL_HUMAN))
		{
#ifdef JP
info[i++] = "あなたの武器は人間の天敵である。";
#else
			info[i++] = "Your weapon is a great bane of humans.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_HUMAN))
		{
#ifdef JP
info[i++] = "あなたの武器は人間に対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against humans.";
#endif

		}
		if (have_flag(flgs, TR_KILL_UNDEAD))
		{
#ifdef JP
info[i++] = "あなたの武器はアンデッドの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of undead.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_UNDEAD))
		{
#ifdef JP
info[i++] = "あなたの武器はアンデッドに対して神聖なる力を発揮する。";
#else
			info[i++] = "Your weapon strikes at undead with holy wrath.";
#endif

		}
		if (have_flag(flgs, TR_KILL_DEMON))
		{
#ifdef JP
info[i++] = "あなたの武器はデーモンの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of demons.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_DEMON))
		{
#ifdef JP
info[i++] = "あなたの武器はデーモンに対して神聖なる力を発揮する。";
#else
			info[i++] = "Your weapon strikes at demons with holy wrath.";
#endif

		}
		if (have_flag(flgs, TR_KILL_ORC))
		{
#ifdef JP
info[i++] = "あなたの武器はオークの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of orcs.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_ORC))
		{
#ifdef JP
info[i++] = "あなたの武器はオークに対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against orcs.";
#endif

		}
		if (have_flag(flgs, TR_KILL_TROLL))
		{
#ifdef JP
info[i++] = "あなたの武器はトロルの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of trolls.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_TROLL))
		{
#ifdef JP
info[i++] = "あなたの武器はトロルに対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against trolls.";
#endif

		}
		if (have_flag(flgs, TR_KILL_GIANT))
		{
#ifdef JP
info[i++] = "あなたの武器はジャイアントの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of giants.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_GIANT))
		{
#ifdef JP
info[i++] = "あなたの武器はジャイアントに対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against giants.";
#endif

		}
		/* Special "kill" flags */
		if (have_flag(flgs, TR_KILL_DRAGON))
		{
#ifdef JP
info[i++] = "あなたの武器はドラゴンの天敵である。";
#else
			info[i++] = "Your weapon is a great bane of dragons.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_DRAGON))
		{
#ifdef JP
info[i++] = "あなたの武器はドラゴンに対して特に強い力を発揮する。";
#else
			info[i++] = "Your weapon is especially deadly against dragons.";
#endif

		}

		if (have_flag(flgs, TR_FORCE_WEAPON))
		{
#ifdef JP
info[i++] = "あなたの武器はMPを使って攻撃する。";
#else
			info[i++] = "Your weapon causes greate damages using your MP.";
#endif

		}
		if (have_flag(flgs, TR_THROW))
		{
#ifdef JP
info[i++] = "あなたの武器は投げやすい。";
#else
			info[i++] = "Your weapon can be thrown well.";
#endif
		}
	}


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
		if ((k == 22) && (j+1 < i))
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
	if (dur <= 5)
	{
		return 0;
	}
	else if (dur <= 10)
	{
		return 1;
	}
	else if (dur <= 20)
	{
		return 2;
	}
	else if (dur <= 50)
	{
		return 3;
	}
	else if (dur <= 100)
	{
		return 4;
	}
	else if (dur <= 200)
	{
		return 5;
	}
	else
	{
		return 6;
	}
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
"モンスターを攻撃するまで"
#else
	"for a short time",
	"for a little while",
	"for a while",
	"for a long while",
	"for a long time",
	"for a very long time",
	"for an incredibly long time",
	"until you hit a monster"
#endif

};


/*
 * Report all currently active magical effects.
 */
void report_magics(creature_type *cr_ptr)
{
	int     i = 0, j, k;
	char    Dummy[80];
	cptr    info[128];
	int     info2[128];


	if (cr_ptr->blind)
	{
		info2[i]  = report_magics_aux(cr_ptr->blind);
#ifdef JP
info[i++] = "あなたは目が見えない。";
#else
		info[i++] = "You cannot see";
#endif

	}
	if (cr_ptr->confused)
	{
		info2[i]  = report_magics_aux(cr_ptr->confused);
#ifdef JP
info[i++] = "あなたは混乱している。";
#else
		info[i++] = "You are confused";
#endif

	}
	if (cr_ptr->afraid)
	{
		info2[i]  = report_magics_aux(cr_ptr->afraid);
#ifdef JP
info[i++] = "あなたは恐怖に侵されている。";
#else
		info[i++] = "You are terrified";
#endif

	}
	if (cr_ptr->poisoned)
	{
		info2[i]  = report_magics_aux(cr_ptr->poisoned);
#ifdef JP
info[i++] = "あなたは毒に侵されている。";
#else
		info[i++] = "You are poisoned";
#endif

	}
	if (cr_ptr->image)
	{
		info2[i]  = report_magics_aux(cr_ptr->image);
#ifdef JP
info[i++] = "あなたは幻覚を見ている。";
#else
		info[i++] = "You are hallucinating";
#endif

	}
	if (cr_ptr->blessed)
	{
		info2[i]  = report_magics_aux(cr_ptr->blessed);
#ifdef JP
info[i++] = "あなたは公正さを感じている。";
#else
		info[i++] = "You feel rightous";
#endif

	}
	if (cr_ptr->hero)
	{
		info2[i]  = report_magics_aux(cr_ptr->hero);
#ifdef JP
info[i++] = "あなたはヒーロー気分だ。";
#else
		info[i++] = "You feel heroic";
#endif

	}
	if (cr_ptr->shero)
	{
		info2[i]  = report_magics_aux(cr_ptr->shero);
#ifdef JP
info[i++] = "あなたは戦闘狂だ。";
#else
		info[i++] = "You are in a battle rage";
#endif

	}
	if (cr_ptr->protevil)
	{
		info2[i]  = report_magics_aux(cr_ptr->protevil);
#ifdef JP
info[i++] = "あなたは邪悪なる存在から守られている。";
#else
		info[i++] = "You are protected from evil";
#endif

	}
	if (cr_ptr->shield)
	{
		info2[i]  = report_magics_aux(cr_ptr->shield);
#ifdef JP
info[i++] = "あなたは神秘のシールドで守られている。";
#else
		info[i++] = "You are protected by a mystic shield";
#endif

	}
	if (cr_ptr->invuln)
	{
		info2[i]  = report_magics_aux(cr_ptr->invuln);
#ifdef JP
info[i++] = "無敵でいられる。";
#else
		info[i++] = "You are invulnerable";
#endif

	}
	if (cr_ptr->wraith_form)
	{
		info2[i]  = report_magics_aux(cr_ptr->wraith_form);
#ifdef JP
info[i++] = "幽体化できる。";
#else
		info[i++] = "You are incorporeal";
#endif

	}
	if (cr_ptr->special_attack & ATTACK_CONFUSE)
	{
		info2[i]  = 7;
#ifdef JP
info[i++] = "あなたの手は赤く輝いている。";
#else
		info[i++] = "Your hands are glowing dull red.";
#endif

	}
	if (cr_ptr->word_recall)
	{
		info2[i]  = report_magics_aux(cr_ptr->word_recall);
#ifdef JP
		info[i++] = "この後帰還の詔を発動する。";
#else
		info[i++] = "You are waiting to be recalled";
#endif

	}
	if (cr_ptr->alter_reality)
	{
		info2[i]  = report_magics_aux(cr_ptr->alter_reality);
#ifdef JP
		info[i++] = "この後現実変容が発動する。";
#else
		info[i++] = "You waiting to be altered";
#endif

	}
	if (cr_ptr->oppose_acid)
	{
		info2[i]  = report_magics_aux(cr_ptr->oppose_acid);
#ifdef JP
info[i++] = "あなたは酸への耐性を持っている。";
#else
		info[i++] = "You are resistant to acid";
#endif

	}
	if (cr_ptr->oppose_elec)
	{
		info2[i]  = report_magics_aux(cr_ptr->oppose_elec);
#ifdef JP
info[i++] = "あなたは電撃への耐性を持っている。";
#else
		info[i++] = "You are resistant to lightning";
#endif

	}
	if (cr_ptr->oppose_fire)
	{
		info2[i]  = report_magics_aux(cr_ptr->oppose_fire);
#ifdef JP
info[i++] = "あなたは火への耐性を持っている。";
#else
		info[i++] = "You are resistant to fire";
#endif

	}
	if (cr_ptr->oppose_cold)
	{
		info2[i]  = report_magics_aux(cr_ptr->oppose_cold);
#ifdef JP
info[i++] = "あなたは冷気への耐性を持っている。";
#else
		info[i++] = "You are resistant to cold";
#endif

	}
	if (cr_ptr->oppose_pois)
	{
		info2[i]  = report_magics_aux(cr_ptr->oppose_pois);
#ifdef JP
		info[i++] = "あなたは毒への耐性を持っている。";
#else
		info[i++] = "You are resistant to poison";
#endif

	}

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
		if ((k == 22) && (j + 1 < i))
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

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan the current panel */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x <= cur_wid - 1; x++)
		{
			int dist = distance(creature_ptr->fy, creature_ptr->fx, y, x);
			if (dist > range) continue;

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Hack -- Safe */
			if (flag == FF_TRAP)
			{
				/* Mark as detected */
				if (dist <= range && known)
				{
					if (dist <= range - 1) c_ptr->info |= (CAVE_IN_DETECT);

					c_ptr->info &= ~(CAVE_UNSAFE);

					/* Redraw */
					lite_spot(y, x);
				}
			}

			/* Detect flags */
			if (cave_have_flag_grid(c_ptr, flag))
			{
				/* Detect secrets */
				disclose_grid(y, x);

				/* Hack -- Memorize */
				c_ptr->info |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

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
bool detect_traps(creature_type *cr_ptr, int range, bool known)
{
	bool detect = detect_feat_flag(cr_ptr, range, FF_TRAP, known);

	if (known) cr_ptr->dtrap = TRUE;

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 0) detect = FALSE;

	/* Describe */
	if (detect)
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
bool detect_doors(creature_type *cr_ptr, int range)
{
	bool detect = detect_feat_flag(cr_ptr, range, FF_DOOR, TRUE);

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 0) detect = FALSE;

	/* Describe */
	if (detect)
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
bool detect_stairs(creature_type *cr_ptr, int range)
{
	bool detect = detect_feat_flag(cr_ptr, range, FF_STAIRS, TRUE);

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 0) detect = FALSE;

	/* Describe */
	if (detect)
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
bool detect_treasure(creature_type *cr_ptr, int range)
{
	bool detect = detect_feat_flag(cr_ptr, range, FF_HAS_GOLD, TRUE);

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 6) detect = FALSE;

	/* Describe */
	if (detect)
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


/*
 * Detect all "gold" objects on the current panel
 */
bool detect_objects_gold(creature_type *cr_ptr, int range)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range2) continue;

		/* Detect "gold" objects */
		if (o_ptr->tval == TV_GOLD)
		{
			/* Hack -- memorize it */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 6) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("財宝の存在を感じとった！");
#else
		msg_print("You sense the presence of treasure!");
#endif

	}

	if (detect_monsters_string(cr_ptr, range, "$"))
	{
		detect = TRUE;
	}

	/* Result */
	return (detect);
}


/*
 * Detect all "normal" objects on the current panel
 */
bool detect_objects_normal(creature_type *cr_ptr, int range)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range2) continue;

		/* Detect "real" objects */
		if (o_ptr->tval != TV_GOLD)
		{
			/* Hack -- memorize it */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 6) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("アイテムの存在を感じとった！");
#else
		msg_print("You sense the presence of objects!");
#endif

	}

	if (detect_monsters_string(cr_ptr, range, "!=?|/`"))
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
bool detect_objects_magic(creature_type *cr_ptr, int range)
{
	int i, y, x, tv;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan all objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range) continue;

		/* Examine the tval */
		tv = o_ptr->tval;

		/* Artifacts, misc magic items, or enchanted wearables */
		if (object_is_artifact(o_ptr) ||
			object_is_ego(o_ptr) ||
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
		    ((o_ptr->to_a > 0) || (o_ptr->to_h + o_ptr->to_d > 0)))
		{
			/* Memorize the item */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	/* Describe */
	if (detect)
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
 * Detect all "normal" monsters on the current panel
 */
bool detect_monsters_normal(creature_type *cr_ptr, int range)
{
	int i, y, x;

	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range) continue;

		/* Detect all non-invisible monsters */
		if (!has_cf_creature(m_ptr, CF_INVISIBLE) || cr_ptr->see_inv)
		{
			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(cr_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 3) flag = FALSE;

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("モンスターの存在を感じとった！");
#else
		msg_print("You sense the presence of monsters!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all "invisible" monsters around the player
 */
bool detect_monsters_invis(creature_type *cr_ptr, int range)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range) continue;

		/* Detect invisible monsters */
		if (has_cf_creature(m_ptr, CF_INVISIBLE))
		{
			/* Update monster recall window */
			if (species_window_idx == m_ptr->species_idx)
			{
				/* Window stuff */
				play_window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(cr_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 3) flag = FALSE;

	/* Describe */
	if (flag)
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
 * Detect all "evil" monsters on current panel
 */
bool detect_monsters_evil(creature_type *cr_ptr, int range)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range) continue;

		/* Detect evil monsters */
		if (is_enemy_of_good_creature(m_ptr))
		{
			if (is_original_ap(m_ptr))
			{
				/* Take note that they are evil */
				//r_ptr->r_flags3 |= (RF3_EVIL);

				/* Update monster recall window */
				if (species_window_idx == m_ptr->species_idx)
				{
					/* Window stuff */
					play_window |= (PW_MONSTER);
				}
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(cr_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
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
 * Detect all "nonliving", "undead" or "demonic" monsters on current panel
 */
bool detect_monsters_nonliving(creature_type *cr_ptr, int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range) continue;

		/* Detect non-living monsters */
		if (!monster_living(r_ptr))
		{
			/* Update monster recall window */
			if (species_window_idx == m_ptr->species_idx)
			{
				/* Window stuff */
				play_window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(cr_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("自然でないモンスターの存在を感じた！");
#else
		msg_print("You sense the presence of unnatural beings!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all monsters it has mind on current panel
 */
bool detect_monsters_mind(creature_type *cr_ptr, int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range) continue;

		/* Detect non-living monsters */
		if (!has_cf_creature(m_ptr, CF_EMPTY_MIND))
		{
			/* Update monster recall window */
			if (species_window_idx == m_ptr->species_idx)
			{
				/* Window stuff */
				play_window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(cr_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
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
 * Detect all (string) monsters on current panel
 */
bool detect_monsters_string(creature_type *cr_ptr, int range, cptr Match)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range) continue;

		/* Detect monsters with the same symbol */
		if (my_strchr(Match, r_ptr->d_char))
		{
			/* Update monster recall window */
			if (species_window_idx == m_ptr->species_idx)
			{
				/* Window stuff */
				play_window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(cr_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if (music_singing(cr_ptr, MUSIC_DETECT) && cr_ptr->magic_num1[2] > 3) flag = FALSE;

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("モンスターの存在を感じとった！");
#else
		msg_print("You sense the presence of monsters!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * A "generic" detect monsters routine, tagged to flags3
 */
bool detect_monsters_xxx(creature_type *cr_ptr, int range, u32b match_flag)
{
	int  i, y, x;
	bool flag = FALSE;
#ifdef JP
cptr desc_monsters = "変なモンスター";
#else
	cptr desc_monsters = "weird monsters";
#endif

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(cr_ptr->fy, cr_ptr->fx, y, x) > range) continue;

		/* Detect evil monsters */
		//TODO if (r_ptr->flags3 & (match_flag))
		if(1)
		{
			if (is_original_ap(m_ptr))
			{
				/* Take note that they are something */
				r_ptr->r_flags3 |= (match_flag);

				/* Update monster recall window */
				if (species_window_idx == m_ptr->species_idx)
				{
					/* Window stuff */
					play_window |= (PW_MONSTER);
				}
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag2 |= (MFLAG2_MARK | MFLAG2_SHOW);

			/* Update the monster */
			update_mon(cr_ptr, i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		switch (match_flag)
		{
			default:
#ifdef JP
				desc_monsters = "敵";
#else
				desc_monsters = "the enemy";
#endif
				break;
		}

		/* Describe result */
#ifdef JP
msg_format("%sの存在を感じとった！", desc_monsters);
#else
		msg_format("You sense the presence of %s!", desc_monsters);
#endif

		msg_print(NULL);
	}

	/* Result */
	return (flag);
}


/*
 * Detect everything
 */
bool detect_all(creature_type *cr_ptr, int range)
{
	bool detect = FALSE;

	/* Detect everything */
	if (detect_traps(cr_ptr, range, TRUE)) detect = TRUE;
	if (detect_doors(cr_ptr, range)) detect = TRUE;
	if (detect_stairs(cr_ptr, range)) detect = TRUE;

	/* There are too many hidden treasure.  So... */
	/* if (detect_treasure(range)) detect = TRUE; */

	if (detect_objects_gold(cr_ptr, range)) detect = TRUE;
	if (detect_objects_normal(cr_ptr, range)) detect = TRUE;
	if (detect_monsters_invis(cr_ptr, range)) detect = TRUE;
	if (detect_monsters_normal(cr_ptr, range)) detect = TRUE;

	/* Result */
	return (detect);
}


/*
 * Apply a "project()" directly to all viewable monsters
 *
 * Note that affected monsters are NOT auto-tracked by this usage.
 *
 * To avoid misbehavior when monster deaths have side-effects,
 * this is done in two passes. -- JDL
 */
bool project_hack(creature_type *caster_ptr, int typ, int dam)
{
	int     i, x, y;
	int     flg = PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE;
	bool    obvious = FALSE;


	/* Mark all (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Require line of sight */
		if (!player_has_los_bold(y, x) || !projectable(caster_ptr->fy, caster_ptr->fx, y, x)) continue;

		/* Mark the monster */
		m_ptr->mflag |= (MFLAG_TEMP);
	}

	/* Affect all marked monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Skip unmarked monsters */
		if (!(m_ptr->mflag & (MFLAG_TEMP))) continue;

		/* Remove mark */
		m_ptr->mflag &= ~(MFLAG_TEMP);

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Jump directly to the target monster */
		if (project(caster_ptr, 0, y, x, dam, typ, flg, -1)) obvious = TRUE;
	}

	/* Result */
	return (obvious);
}


/*
 * Speed monsters
 */
bool speed_monsters(creature_type *caster_ptr)
{
	return (project_hack(caster_ptr, GF_OLD_SPEED, caster_ptr->lev));
}

/*
 * Slow monsters
 */
bool slow_creatures(creature_type *caster_ptr)
{
	return (project_hack(caster_ptr, GF_OLD_SLOW, caster_ptr->lev));
}

/*
 * Sleep monsters
 */
bool sleep_monsters(creature_type *caster_ptr)
{
	return (project_hack(caster_ptr, GF_OLD_SLEEP, caster_ptr->lev));
}


/*
 * Banish evil monsters
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
 * Dispel undead monsters
 */
bool dispel_undead(creature_type *caster_ptr, int dam)
{
	bool tester = (project_hack(caster_ptr, GF_DISP_UNDEAD, dam));
	return tester;
}

/*
 * Dispel evil monsters
 */
bool dispel_evil(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_DISP_EVIL, dam));
}

/*
 * Dispel good monsters
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
 * Dispel 'living' monsters
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
bool crusade(creature_type *cr_ptr)
{
	return (project_hack(cr_ptr, GF_CRUSADE, cr_ptr->lev*4));
}


/*
 * Wake up all monsters, and speed up "los" monsters.
 */
void aggravate_monsters(creature_type *cr_ptr)
{
	int     i;
	bool    sleep = FALSE;
	bool    speed = FALSE;


	/* Aggravate everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		creature_type    *m_ptr = &creature_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Skip aggravating monster (or player) */
		if (m_ptr == cr_ptr) continue;

		/* Wake up nearby sleeping monsters */
		if (m_ptr->cdis < MAX_SIGHT * 2)
		{
			/* Wake up */
			if (m_ptr->paralyzed)
			{
				(void)set_paralyzed(&creature_list[i], 0);
				sleep = TRUE;
			}
			if (!is_pet(player_ptr, m_ptr)) m_ptr->mflag2 |= MFLAG2_NOPET;
		}

		/* Speed up monsters in line of sight */
		if (player_has_los_bold(m_ptr->fy, m_ptr->fx))
		{
			if (!is_pet(player_ptr, m_ptr))
			{
				(void)set_fast(m_ptr, m_ptr->fast + 100, FALSE);
				speed = TRUE;
			}
		}
	}

	/* Messages */
#ifdef JP
	if (speed) msg_print("付近で何かが突如興奮したような感じを受けた！");
	else if (sleep) msg_print("何かが突如興奮したような騒々しい音が遠くに聞こえた！");
#else
	if (speed) msg_print("You feel a sudden stirring nearby!");
	else if (sleep) msg_print("You hear a sudden stirring in the distance!");
#endif
	if (cr_ptr->riding) cr_ptr->creature_update |= CRU_BONUS;
}


/*
 * Delete a non-unique/non-quest monster
 */
bool genocide_aux(creature_type *user_ptr, int m_idx, int power, bool player_cast, int dam_side, cptr spell_name)
{
	int          msec = delay_factor * delay_factor * delay_factor;
	creature_type *m_ptr = &creature_list[m_idx];
	species_type *r_ptr = &species_info[m_ptr->species_idx];
	bool         resist = FALSE;

	if (is_pet(player_ptr, m_ptr) && !player_cast) return FALSE;

	/* Hack -- Skip Unique Monsters or Quest Monsters */
	if ((is_quest_species(r_ptr)) || is_unique_species(r_ptr)) resist = TRUE;

	else if (is_sub_unique_species(r_ptr)) resist = TRUE;

	else if (m_idx == user_ptr->riding) resist = TRUE;

	else if ((inside_quest && !random_quest_number(dun_level)) || inside_arena || monster_arena_mode) resist = TRUE;

	else if (player_cast && (r_ptr->level > randint0(power))) resist = TRUE;

	else if (player_cast && (m_ptr->mflag2 & MFLAG2_NOGENO)) resist = TRUE;

	/* Delete the monster */
	else
	{
		if (record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
		{
			char m_name[80];

			creature_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, m_name);
		}

		delete_species_idx(&creature_list[m_idx]);
	}

	if (resist && player_cast)
	{
		bool see_m = is_seen(user_ptr, m_ptr);
		char m_name[80];

		creature_desc(m_name, m_ptr, 0);
		if (see_m)
		{
#ifdef JP
			msg_format("%^sには効果がなかった。", m_name);
#else
			msg_format("%^s is unaffected.", m_name);
#endif
		}
		if (m_ptr->paralyzed)
		{
			(void)set_paralyzed(&creature_list[m_idx], 0);
			if (m_ptr->ml)
			{
#ifdef JP
				msg_format("%^sが目を覚ました。", m_name);
#else
				msg_format("%^s wakes up.", m_name);
#endif
			}
		}
		if (is_friendly(m_ptr) && !is_pet(player_ptr, m_ptr))
		{
			if (see_m)
			{
#ifdef JP
				msg_format("%sは怒った！", m_name);
#else
				msg_format("%^s gets angry!", m_name);
#endif
			}
			set_hostile(user_ptr, m_ptr);
		}
		if (one_in_(13)) m_ptr->mflag2 |= MFLAG2_NOGENO;
	}

	if (player_cast)
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
 * Delete all non-unique/non-quest monsters of a given "type" from the level
 */
bool symbol_genocide(creature_type *caster_ptr, int power, bool player_cast)
{
	int  i;
	char typ;
	bool result = FALSE;

	/* Prevent genocide in quest levels */
	if ((inside_quest && !random_quest_number(dun_level)) || inside_arena || monster_arena_mode)
	{
		return (FALSE);
	}

	/* Mega-Hack -- Get a monster symbol */
#ifdef JP
	while (!get_com("どの種類(文字)のモンスターを抹殺しますか: ", &typ, FALSE)) ;
#else
	while (!get_com("Choose a monster race (by symbol) to genocide: ", &typ, FALSE)) ;
#endif

	/* Delete the monsters of that "type" */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Skip "wrong" monsters */
		if (r_ptr->d_char != typ) continue;

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
 * Delete all nearby (non-unique) monsters
 */
bool mass_genocide(creature_type *caster_ptr, int power, bool player_cast)
{
	int  i;
	bool result = FALSE;

	/* Prevent mass genocide in quest levels */
	if ((inside_quest && !random_quest_number(dun_level)) || inside_arena || monster_arena_mode)
	{
		return (FALSE);
	}

	/* Delete the (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > MAX_SIGHT) continue;

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

	/* Prevent mass genocide in quest levels */
	if ((inside_quest && !random_quest_number(dun_level)) || inside_arena || monster_arena_mode)
	{
		return (FALSE);
	}

	/* Delete the (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		if (!is_undead_species(r_ptr)) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > MAX_SIGHT) continue;

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
 * Probe nearby monsters
 */
bool probing(void)
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

	/* Probe all (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr = &species_info[m_ptr->species_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Require line of sight */
		if (!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;

		/* Probe visible monsters */
		if (m_ptr->ml)
		{
			char m_name[80];

			/* Start the message */
			if (!probe)
			{
#ifdef JP
				msg_print("調査中...");
#else
				msg_print("Probing...");
#endif
			}

			msg_print(NULL);

			if (!is_original_ap(m_ptr))
			{
				if (m_ptr->mflag2 & MFLAG2_KAGE)
					m_ptr->mflag2 &= ~(MFLAG2_KAGE);

				m_ptr->ap_species_idx = m_ptr->species_idx;
				lite_spot(m_ptr->fy, m_ptr->fx);
			}
			/* Get "the monster" or "something" */
			creature_desc(m_name, m_ptr, MD_IGNORE_HALLU | MD_INDEF_HIDDEN);

			speed = m_ptr->speed - 110;
			if (m_ptr->fast) speed += 10;
			if (m_ptr->slow) speed -= 10;

			/* Get the monster's alignment */
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

			/* Describe the monster */
#ifdef JP
sprintf(buf,"%s 属性:%s 性別:%s HP:%d/%d AC:%d 速度:%s%d 腕:%d 知:%d 賢:%d 器:%d 耐:%d 魅:%d 経験:",
		m_name, align, sex, m_ptr->chp, m_ptr->mhp,  m_ptr->ac + m_ptr->to_a, (speed > 0) ? "+" : "", speed,
		m_ptr->stat_use[0]/10, m_ptr->stat_use[1]/10, m_ptr->stat_use[2]/10, m_ptr->stat_use[3]/10, m_ptr->stat_use[4]/10, m_ptr->stat_use[5]/10);
#else
sprintf(buf, "%s align:%s sex:%s HP:%d/%d AC:%d speed:%s%d STR:%d INT:%d WIS:%d DEX:%d CON:%d CHA:%d exp:",
		m_name, align, sex, m_ptr->chp, m_ptr->mhp,  m_ptr->ac + m_ptr->to_a, (speed > 0) ? "+" : "", speed,
		m_ptr->stat_use[0]/10, m_ptr->stat_use[1]/10, m_ptr->stat_use[2]/10, m_ptr->stat_use[3]/10, m_ptr->stat_use[4]/10, m_ptr->stat_use[5]/10);
#endif
			if (r_ptr->next_species_idx)
			{
				strcat(buf, format("%d/%d ", m_ptr->exp, r_ptr->next_exp));
			}
			else
			{
				strcat(buf, "xxx ");
			}

#ifdef JP
			if (m_ptr->paralyzed) strcat(buf,"睡眠 ");
			if (m_ptr->stun) strcat(buf,"朦朧 ");
			if (m_ptr->afraid) strcat(buf,"恐怖 ");
			if (m_ptr->confused) strcat(buf,"混乱 ");
			if (m_ptr->invuln) strcat(buf,"無敵 ");
#else
			if (m_ptr->paralyzed) strcat(buf,"sleeping ");
			if (m_ptr->stun) strcat(buf,"stunned ");
			if (m_ptr->afraid) strcat(buf,"scared ");
			if (m_ptr->confused) strcat(buf,"confused ");
			if (m_ptr->invuln) strcat(buf,"invulnerable ");
#endif
			buf[strlen(buf)-1] = '\0';
			prt(buf,0,0);

			/* HACK : Add the line to message buffer */
			message_add(buf);
			play_window |= (PW_MESSAGE);
			window_stuff();

			if (m_ptr->ml) move_cursor_relative(m_ptr->fy, m_ptr->fx);
			inkey();

			Term_erase(0, 0, 255);

			/* Learn everything about this monster */
			if (lore_do_probe(m_ptr->species_idx))
			{
				char buf[80];

				/* Get base name of monster */
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
	if (probe)
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
 * This spell "deletes" monsters (instead of "killing" them).
 *
 * Later we may use one function for both "destruction" and
 * "earthquake" by using the "full" to select "destruction".
 */
bool destroy_area(creature_type *caster_ptr, int y1, int x1, int r, bool in_generate)
{
	int       y, x, k, t;
	cave_type *c_ptr;
	bool      flag = FALSE;

	/* Prevent destruction of quest levels and town */
	if ((inside_quest && is_fixed_quest_idx(inside_quest)) || !dun_level)
	{
		return (FALSE);
	}

	/* Lose monster light */
	if (!in_generate) clear_mon_lite();

	/* Big area of affect */
	for (y = (y1 - r); y <= (y1 + r); y++)
	{
		for (x = (x1 - r); x <= (x1 + r); x++)
		{
			/* Skip illegal grids */
			if (!in_bounds(y, x)) continue;

			/* Extract the distance */
			k = distance(y1, x1, y, x);

			/* Stay in the circle of death */
			if (k > r) continue;

			/* Access the grid */
			c_ptr = &cave[y][x];

			// Erase Message
			c_ptr->message[0] = '\0';

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY);

			/* Lose light and knowledge */
			c_ptr->info &= ~(CAVE_MARK | CAVE_GLOW);

			if (!in_generate) /* Normal */
			{
				/* Lose unsafety */
				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Hack -- Notice player affect */
				if (creature_bold(caster_ptr, y, x))
				{
					/* Hurt the player later */
					flag = TRUE;

					/* Do not hurt this grid */
					continue;
				}
			}

			/* Hack -- Skip the epicenter */
			if ((y == y1) && (x == x1)) continue;

			if (c_ptr->m_idx)
			{
				creature_type *m_ptr = &creature_list[c_ptr->m_idx];
				species_type *r_ptr = &species_info[m_ptr->species_idx];

				if (in_generate) /* In generation */
				{
					/* Delete the monster (if any) */
					delete_monster(y, x);
				}
				else if (is_quest_species(r_ptr))
				{
					/* Heal the monster */
					m_ptr->chp = m_ptr->mhp;

					/* Try to teleport away quest monsters */
					if (!teleport_away(&creature_list[c_ptr->m_idx], (r * 2) + 1, TELEPORT_DEC_VALOUR)) continue;
				}
				else
				{
					if (record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
					{
						char m_name[80];

						creature_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
						do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_DESTROY, m_name);
					}

					/* Delete the monster (if any) */
					delete_monster(y, x);
				}
			}

			/* During generation, destroyed artifacts are "preserved" */
			if (preserve_mode || in_generate)
			{
				s16b this_o_idx, next_o_idx = 0;

				/* Scan all objects in the grid */
				for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
				{
					object_type *o_ptr;

					/* Acquire object */
					o_ptr = &o_list[this_o_idx];

					/* Acquire next object */
					next_o_idx = o_ptr->next_o_idx;

					/* Hack -- Preserve unknown artifacts */
					if (object_is_fixed_artifact(o_ptr) && (!object_is_known(o_ptr) || in_generate))
					{
						/* Mega-Hack -- Preserve the artifact */
						a_info[o_ptr->name1].cur_num = 0;

						if (in_generate && cheat_peek)
						{
							char o_name[MAX_NLEN];
							object_desc(o_name, o_ptr, (OD_NAME_ONLY | OD_STORE));
#ifdef JP
							msg_format("伝説のアイテム (%s) は生成中に*破壊*された。", o_name);
#else
							msg_format("Artifact (%s) was *destroyed* during generation.", o_name);
#endif
						}
					}
					else if (in_generate && cheat_peek && o_ptr->art_name)
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
			delete_object(y, x);

			/* Destroy "non-permanent" grids */
			if (!cave_perma_grid(c_ptr))
			{
				/* Wall (or floor) type */
				t = randint0(200);

				if (!in_generate) /* Normal */
				{
					if (t < 20)
					{
						/* Create granite wall */
						cave_set_feat(caster_ptr, y, x, feat_granite);
					}
					else if (t < 70)
					{
						/* Create quartz vein */
						cave_set_feat(caster_ptr, y, x, feat_quartz_vein);
					}
					else if (t < 100)
					{
						/* Create magma vein */
						cave_set_feat(caster_ptr, y, x, feat_magma_vein);
					}
					else
					{
						/* Create floor */
						cave_set_feat(caster_ptr, y, x, floor_type[randint0(100)]);
					}
				}
				else /* In generation */
				{
					if (t < 20)
					{
						/* Create granite wall */
						place_extra_grid(c_ptr);
					}
					else if (t < 70)
					{
						/* Create quartz vein */
						c_ptr->feat = feat_quartz_vein;
					}
					else if (t < 100)
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

	if (!in_generate)
	{
		/* Process "re-glowing" */
		for (y = (y1 - r); y <= (y1 + r); y++)
		{
			for (x = (x1 - r); x <= (x1 + r); x++)
			{
				/* Skip illegal grids */
				if (!in_bounds(y, x)) continue;

				/* Extract the distance */
				k = distance(y1, x1, y, x);

				/* Stay in the circle of death */
				if (k > r) continue;

				/* Access the grid */
				c_ptr = &cave[y][x];

				if (is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
				else if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS))
				{
					int i, yy, xx;
					cave_type *cc_ptr;

					for (i = 0; i < 9; i++)
					{
						yy = y + ddy_ddd[i];
						xx = x + ddx_ddd[i];
						if (!in_bounds2(yy, xx)) continue;
						cc_ptr = &cave[yy][xx];
						if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
						{
							c_ptr->info |= CAVE_GLOW;
							break;
						}
					}
				}
			}
		}

		/* Hack -- Affect player */
		if (flag)
		{
			/* Message */
#ifdef JP
			msg_print("燃えるような閃光が発生した！");
#else
			msg_print("There is a searing blast of light!");
#endif

			/* Blind the player */
			if (!caster_ptr->resist_blind && !caster_ptr->resist_lite)
			{
				/* Become blind */
				(void)set_blind(caster_ptr, caster_ptr->blind + 10 + randint1(10));
			}
		}

		forget_flow();

		/* Mega-Hack -- Forget the view and lite */
		update |= (PU_UN_VIEW | PU_UN_LITE);

		/* Update stuff */
		update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_MONSTERS);

		/* Redraw map */
		play_redraw |= (PR_MAP);

		/* Window stuff */
		play_window |= (PW_OVERHEAD | PW_DUNGEON);

		if (caster_ptr->special_defense & NINJA_S_STEALTH)
		{
			if (cave[caster_ptr->fy][caster_ptr->fx].info & CAVE_GLOW) set_superstealth(caster_ptr, FALSE);
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
 * Monsters will take damage, and "jump" into a safe grid if possible,
 * otherwise they will be "buried" in the rubble, disappearing from
 * the level in the same way that they do when genocided.
 *
 * Note that thus the player and monsters (except eaters of walls and
 * passers through walls) will never occupy the same grid as a wall.
 * Note that as of now (2.7.8) no monster may occupy a "wall" grid, even
 * for a single turn, unless that monster can pass_walls or kill_walls.
 * This has allowed massive simplification of the "monster" code.
 */
bool earthquake_aux(creature_type *target_ptr, int cy, int cx, int r, int m_idx)
{
	int             i, t, y, x, yy, xx, dy, dx;
	int             damage = 0;
	int             sn = 0, sy = 0, sx = 0;
	bool            hurt = FALSE;
	cave_type       *c_ptr;
	bool            map[32][32];


	/* Prevent destruction of quest levels and town */
	if ((inside_quest && is_fixed_quest_idx(inside_quest)) || !dun_level)
	{
		return (FALSE);
	}

	/* Paranoia -- Enforce maximum range */
	if (r > 12) r = 12;

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
			if (!in_bounds(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY | CAVE_UNSAFE);

			/* Lose light and knowledge */
			c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);

			/* Skip the epicenter */
			if (!dx && !dy) continue;

			/* Skip most grids */
			if (randint0(100) < 85) continue;

			/* Damage this grid */
			map[16+yy-cy][16+xx-cx] = TRUE;

			/* Hack -- Take note of player damage */
			if (creature_bold(target_ptr, yy, xx)) hurt = TRUE;
		}
	}

	/* First, affect the player (if necessary) */
	if (hurt && !target_ptr->pass_wall && !target_ptr->kill_wall)
	{
		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			/* Access the location */
			y = target_ptr->fy + ddy_ddd[i];
			x = target_ptr->fx + ddx_ddd[i];

			/* Skip non-empty grids */
			if (!cave_empty_bold(y, x)) continue;

			/* Important -- Skip "quake" grids */
			if (map[16+y-cy][16+x-cx]) continue;

			if (cave[y][x].m_idx) continue;

			/* Count "safe" grids */
			sn++;

			/* Randomize choice */
			if (randint0(sn) > 0) continue;

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
		if (!sn)
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
					damage = damroll(10, 4);
					(void)set_stun(target_ptr, target_ptr->stun + randint1(50));
					break;
				}
				case 3:
				{
#ifdef JP
					msg_print("あなたは床と壁との間に挟まれてしまった！");
#else
					msg_print("You are crushed between the floor and ceiling!");
#endif
					damage = damroll(10, 4);
					(void)set_stun(target_ptr, target_ptr->stun + randint1(50));
					break;
				}
			}

			/* Move the player to the safe location */
			(void)move_creature_effect(target_ptr, sy, sx, MPE_DONT_PICKUP);
		}

		/* Important -- no wall on player */
		map[16+target_ptr->fy-cy][16+target_ptr->fx-cx] = FALSE;

		/* Take some damage */
		if (damage)
		{
			char *killer;

			if (m_idx)
			{
				char m_name[80];
				creature_type *m_ptr = &creature_list[m_idx];

				/* Get the monster's real name */
				creature_desc(m_name, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

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
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			if (c_ptr->m_idx == target_ptr->riding) continue;

			/* Process monsters */
			if (c_ptr->m_idx)
			{
				creature_type *m_ptr = &creature_list[c_ptr->m_idx];
				species_type *r_ptr = &species_info[m_ptr->species_idx];

				/* Quest monsters */
				if (is_quest_species(r_ptr))
				{
					/* No wall on quest monsters */
					map[16+yy-cy][16+xx-cx] = FALSE;

					continue;
				}

				/* Most monsters cannot co-exist with rock */
				if (!is_kill_wall_species(r_ptr) &&
				    !is_pass_wall_species(r_ptr))
				{
					char m_name[80];

					/* Assume not safe */
					sn = 0;

					/* Monster can move to escape the wall */
					if (!(is_never_move_species(r_ptr)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Access the grid */
							y = yy + ddy_ddd[i];
							x = xx + ddx_ddd[i];

							/* Skip non-empty grids */
							if (!cave_empty_bold(y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if (is_glyph_grid(&cave[y][x])) continue;
							if (is_explosive_rune_grid(&cave[y][x])) continue;

							/* ... nor on the Pattern */
							if (pattern_tile(y, x)) continue;

							/* Important -- Skip "quake" grids */
							if (map[16+y-cy][16+x-cx]) continue;

							if (cave[y][x].m_idx) continue;
							if (creature_bold(target_ptr, y, x)) continue;

							/* Count "safe" grids */
							sn++;

							/* Randomize choice */
							if (randint0(sn) > 0) continue;

							/* Save the safe grid */
							sy = y; sx = x;
						}
					}

					/* Describe the monster */
					creature_desc(m_name, m_ptr, 0);

					/* Scream in pain */
#ifdef JP
					if (!ignore_unview || is_seen(target_ptr, m_ptr)) msg_format("%^sは苦痛で泣きわめいた！", m_name);
#else
					if (!ignore_unview || is_seen(target_ptr, m_ptr)) msg_format("%^s wails out in pain!", m_name);
#endif

					/* Take damage from the quake */
					damage = (sn ? damroll(4, 8) : (m_ptr->chp + 1));

					/* Monster is certainly awake */
					(void)set_paralyzed(&creature_list[c_ptr->m_idx], 0);

					/* Apply damage directly */
					m_ptr->chp -= damage;

					/* Delete (not kill) "dead" monsters */
					if (m_ptr->chp < 0)
					{
						/* Message */
#ifdef JP
						if (!ignore_unview || is_seen(target_ptr, m_ptr)) msg_format("%^sは岩石に埋もれてしまった！", m_name);
#else
						if (!ignore_unview || is_seen(target_ptr, m_ptr)) msg_format("%^s is embedded in the rock!", m_name);
#endif

						if (c_ptr->m_idx)
						{
							if (record_named_pet && is_pet(player_ptr, &creature_list[c_ptr->m_idx]) && creature_list[c_ptr->m_idx].nickname)
							{
								char m2_name[80];

								creature_desc(m2_name, m_ptr, MD_INDEF_VISIBLE);
								do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_EARTHQUAKE, m2_name);
							}
						}

						/* Delete the monster */
						delete_monster(yy, xx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if (sn)
					{
						int m_idx = cave[yy][xx].m_idx;

						/* Update the old location */
						cave[yy][xx].m_idx = 0;

						/* Update the new location */
						cave[sy][sx].m_idx = m_idx;

						/* Move the monster */
						m_ptr->fy = sy;
						m_ptr->fx = sx;

						/* Update the monster (new location) */
						update_mon(target_ptr, m_idx, TRUE);

						/* Redraw the old grid */
						lite_spot(yy, xx);

						/* Redraw the new grid */
						lite_spot(sy, sx);
					}
				}
			}
		}
	}

	/* Lose monster light */
	clear_mon_lite();

	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Access the cave grid */
			c_ptr = &cave[yy][xx];

			/* Paranoia -- never affect player */
			if (creature_bold(target_ptr, yy, xx)) continue;

			/* Destroy location (if valid) */
			if (cave_valid_bold(yy, xx))
			{
				/* Delete objects */
				delete_object(yy, xx);

				/* Wall (or floor) type */
				t = cave_have_flag_bold(yy, xx, FF_PROJECT) ? randint0(100) : 200;

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					cave_set_feat(target_ptr, yy, xx, feat_granite);
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					cave_set_feat(target_ptr, yy, xx, feat_quartz_vein);
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					cave_set_feat(target_ptr, yy, xx, feat_magma_vein);
				}

				/* Floor */
				else
				{
					/* Create floor */
					cave_set_feat(target_ptr, yy, xx, floor_type[randint0(100)]);
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
			if (!in_bounds(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			if (is_mirror_grid(c_ptr)) c_ptr->info |= CAVE_GLOW;
			else if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS))
			{
				int ii, yyy, xxx;
				cave_type *cc_ptr;

				for (ii = 0; ii < 9; ii++)
				{
					yyy = yy + ddy_ddd[ii];
					xxx = xx + ddx_ddd[ii];
					if (!in_bounds2(yyy, xxx)) continue;
					cc_ptr = &cave[yyy][xxx];
					if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
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
	update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_MONSTERS);

	/* Update the health bar */
	play_redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	/* Window stuff */
	play_window |= (PW_OVERHEAD | PW_DUNGEON);

	if (target_ptr->special_defense & NINJA_S_STEALTH)
	{
		if (cave[target_ptr->fy][target_ptr->fx].info & CAVE_GLOW) set_superstealth(target_ptr, FALSE);
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

	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		if (!m_ptr->species_idx || !is_pet(player_ptr, m_ptr)) continue;
		if (m_ptr->nickname) okay = FALSE;
	}
	if (!okay || caster_ptr->riding)
	{
#ifdef JP
		if (!get_check("本当に全ペットを爆破しますか？"))
#else
		if (!get_check("You will blast all pets. Are you sure? "))
#endif
			return;
	}
	for (i = 1; i < m_max; i++)
	{
		int dam;
		creature_type *m_ptr = &creature_list[i];
		species_type *r_ptr;

		if (!m_ptr->species_idx || !is_pet(player_ptr, m_ptr)) continue;
		r_ptr = &species_info[m_ptr->species_idx];

		/* Uniques resist discharging */
		if (is_unique_species(r_ptr))
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
		if (dam > 100) dam = (dam-100)/2 + 100;
		if (dam > 400) dam = (dam-400)/2 + 400;
		if (dam > 800) dam = 800;
		project(m_ptr, 2+(r_ptr->level/20), m_ptr->fy,
			m_ptr->fx, dam, GF_PLASMA, 
			PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);

		if (record_named_pet && m_ptr->nickname)
		{
			char m_name[80];

			creature_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
			do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_BLAST, m_name);
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
 * Also, process all affected monsters.
 *
 * SMART monsters always wake up when illuminated
 * NORMAL monsters wake up 1/4 the time when illuminated
 * STUPID monsters wake up 1/10 the time when illuminated
 */
static void cave_temp_room_lite(creature_type *lite_ptr)
{
	int i;

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		cave_type *c_ptr = &cave[y][x];

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Update only non-CAVE_GLOW grids */
		/* if (c_ptr->info & (CAVE_GLOW)) continue; */

		/* Perma-Lite */
		c_ptr->info |= (CAVE_GLOW);

		/* Process affected monsters */
		if (c_ptr->m_idx)
		{
			int chance = 25;

			creature_type    *m_ptr = &creature_list[c_ptr->m_idx];

			species_type    *r_ptr = &species_info[m_ptr->species_idx];

			/* Update the monster */
			update_mon(lite_ptr, c_ptr->m_idx, FALSE);

			/* Stupid monsters rarely wake up */
			if (has_cf_creature(m_ptr, CF_STUPID)) chance = 10;

			/* Smart monsters always wake up */
			if (has_cf_creature(m_ptr, CF_SMART)) chance = 100;

			/* Sometimes monsters wake up */
			if (m_ptr->paralyzed && (randint0(100) < chance))
			{
				/* Wake up! */
				(void)set_paralyzed(&creature_list[c_ptr->m_idx], 0);

				/* Notice the "waking up" */
				if (m_ptr->ml)
				{
					char m_name[80];

					/* Acquire the monster name */
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
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);

		update_local_illumination(lite_ptr, y, x);
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
 * Also, process all affected monsters
 */
static void cave_temp_room_unlite(void)
{
	int i;

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];
		int j;

		cave_type *c_ptr = &cave[y][x];
		bool do_dark = !is_mirror_grid(c_ptr);

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Darken the grid */
		if (do_dark)
		{
			if (dun_level || !is_daytime())
			{
				for (j = 0; j < 9; j++)
				{
					int by = y + ddy_ddd[j];
					int bx = x + ddx_ddd[j];

					if (in_bounds2(by, bx))
					{
						cave_type *cc_ptr = &cave[by][bx];

						if (have_flag(f_info[get_feat_mimic(cc_ptr)].flags, FF_GLOW))
						{
							do_dark = FALSE;
							break;
						}
					}
				}

				if (!do_dark) continue;
			}

			c_ptr->info &= ~(CAVE_GLOW);

			/* Hack -- Forget "boring" grids */
			if (!have_flag(f_info[get_feat_mimic(c_ptr)].flags, FF_REMEMBER))
			{
				/* Forget the grid */
				if (!view_torch_grids) c_ptr->info &= ~(CAVE_MARK);

				/* Notice */
				note_spot(y, x);
			}

			/* Process affected monsters */
			if (c_ptr->m_idx)
			{
				/* Update the monster */
				update_mon(p_ptr, c_ptr->m_idx, FALSE);
			}

			/* Redraw */
			lite_spot(y, x);

			update_local_illumination(p_ptr, y, x);
		}
	}

	/* None left */
	temp_n = 0;
}


/*
 * Determine how much contiguous open space this grid is next to
 */
static int next_to_open(int cy, int cx, bool (*pass_bold)(int, int))
{
	int i;

	int y, x;

	int len = 0;
	int blen = 0;

	for (i = 0; i < 16; i++)
	{
		y = cy + ddy_cdd[i % 8];
		x = cx + ddx_cdd[i % 8];

		/* Found a wall, break the length */
		if (!pass_bold(y, x))
		{
			/* Track best length */
			if (len > blen)
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


static int next_to_walls_adj(int cy, int cx, bool (*pass_bold)(int, int))
{
	int i;

	int y, x;

	int c = 0;

	for (i = 0; i < 8; i++)
	{
		y = cy + ddy_ddd[i];
		x = cx + ddx_ddd[i];

		if (!pass_bold(y, x)) c++;
	}

	return c;
}


/*
 * Aux function -- see below
 */
static void cave_temp_room_aux(creature_type *caster_ptr, int y, int x, bool only_room, bool (*pass_bold)(int, int))
{
	cave_type *c_ptr;

	/* Get the grid */
	c_ptr = &cave[y][x];

	/* Avoid infinite recursion */
	if (c_ptr->info & (CAVE_TEMP)) return;

	/* Do not "leave" the current room */
	if (!(c_ptr->info & (CAVE_ROOM)))
	{
		if (only_room) return;

		/* Verify */
		if (!in_bounds2(y, x)) return;

		/* Do not exceed the maximum spell range */
		if (distance(caster_ptr->fy, caster_ptr->fx, y, x) > MAX_RANGE(caster_ptr)) return;

		/* Verify this grid */
		/*
		 * The reason why it is ==6 instead of >5 is that 8 is impossible
		 * due to the check for cave_bold above.
		 * 7 lights dead-end corridors (you need to do this for the
		 * checkboard interesting rooms, so that the boundary is lit
		 * properly.
		 * This leaves only a check for 6 bounding walls!
		 */
		if (in_bounds(y, x) && pass_bold(y, x) &&
		    (next_to_walls_adj(y, x, pass_bold) == 6) && (next_to_open(y, x, pass_bold) <= 1)) return;
	}

	/* Paranoia -- verify space */
	if (temp_n == TEMP_MAX) return;

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
static bool cave_pass_lite_bold(int y, int x)
{
	return cave_los_bold(y, x);
}

/*
 * Aux function -- see below
 */
static void cave_temp_lite_room_aux(creature_type *caster_ptr, int y, int x)
{
	cave_temp_room_aux(caster_ptr, y, x, FALSE, cave_pass_lite_bold);
}

/*
 * Aux function -- see below
 */
static bool cave_pass_dark_bold(int y, int x)
{
	return cave_have_flag_bold(y, x, FF_PROJECT);
}

/*
 * Aux function -- see below
 */
static void cave_temp_unlite_room_aux(creature_type *caster_ptr, int y, int x)
{
	cave_temp_room_aux(caster_ptr, y, x, TRUE, cave_pass_dark_bold);
}




/*
 * Illuminate any room containing the given location.
 */
void lite_room(creature_type *creature_ptr, int y1, int x1)
{
	int i, x, y;

	/* Add the initial grid */
	cave_temp_lite_room_aux(creature_ptr, y1, x1);

	/* While grids are in the queue, add their neighbors */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get lit, but stop light */
		if (!cave_pass_lite_bold(y, x)) continue;

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

	if (creature_ptr->special_defense & NINJA_S_STEALTH)
	{
		if (cave[creature_ptr->fy][creature_ptr->fx].info & CAVE_GLOW) set_superstealth(creature_ptr, FALSE);
	}
}


/*
 * Darken all rooms containing the given location
 */
void unlite_room(creature_type *caster_ptr, int y1, int x1)
{
	int i, x, y;

	/* Add the initial grid */
	cave_temp_unlite_room_aux(caster_ptr, y1, x1);

	/* Spread, breadth first */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get dark, but stop darkness */
		if (!cave_pass_dark_bold(y, x)) continue;

		/* Spread adjacent */
		cave_temp_unlite_room_aux(caster_ptr, y + 1, x);
		cave_temp_unlite_room_aux(caster_ptr, y - 1, x);
		cave_temp_unlite_room_aux(caster_ptr, y, x + 1);
		cave_temp_unlite_room_aux(caster_ptr, y, x - 1);

		/* Spread diagonal */
		cave_temp_unlite_room_aux(caster_ptr, y + 1, x + 1);
		cave_temp_unlite_room_aux(caster_ptr, y - 1, x - 1);
		cave_temp_unlite_room_aux(caster_ptr, y - 1, x + 1);
		cave_temp_unlite_room_aux(caster_ptr, y + 1, x - 1);
	}

	/* Now, darken them all at once */
	cave_temp_room_unlite();
}



/*
 * Hack -- call light around the player
 * Affect all monsters in the projection radius
 */
bool lite_area(creature_type *cr_ptr, int dam, int rad)
{
	int flg = PROJECT_GRID | PROJECT_KILL;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS)
	{
#ifdef JP
		msg_print("ダンジョンが光を吸収した。");
#else
		msg_print("The darkness of this dungeon absorb your light.");
#endif
		return FALSE;
	}

	/* Hack -- Message */
	if (!player_ptr->blind)
	{
#ifdef JP
		msg_print("白い光が辺りを覆った。");
#else
		msg_print("You are surrounded by a white light.");
#endif

	}

	/* Hook into the "project()" function */
	(void)project(cr_ptr, rad, cr_ptr->fy, cr_ptr->fx, dam, GF_LITE_WEAK, flg, -1);

	/* Lite up the room */
	lite_room(cr_ptr, cr_ptr->fy, cr_ptr->fx);

	/* Assume seen */
	return (TRUE);
}


/*
 * Hack -- call darkness around the player
 * Affect all monsters in the projection radius
 */
bool unlite_area(creature_type *caster_ptr, int dam, int rad)
{
	int flg = PROJECT_GRID | PROJECT_KILL;

	/* Hack -- Message */
	if (!player_ptr->blind)
	{
#ifdef JP
msg_print("暗闇が辺りを覆った。");
#else
		msg_print("Darkness surrounds you.");
#endif

	}

	/* Hook into the "project()" function */
	(void)project(caster_ptr, rad, caster_ptr->fy, caster_ptr->fx, dam, GF_DARK_WEAK, flg, -1);

	/* Lite up the room */
	unlite_room(caster_ptr, caster_ptr->fy, caster_ptr->fx);

	/* Assume seen */
	return (TRUE);
}



/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool fire_ball(creature_type *caster_ptr, int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	if (typ == GF_CONTROL_LIVING) flg|= PROJECT_HIDE;
	/* Use the given direction */
	tx = caster_ptr->fx + 99 * ddx[dir];
	ty = caster_ptr->fy + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(caster_ptr, rad, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool fire_rocket(creature_type *caster_ptr, int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	/* Use the given direction */
	tx = caster_ptr->fx + 99 * ddx[dir];
	ty = caster_ptr->fy + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool fire_ball_hide(creature_type *caster_ptr, int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_HIDE;

	/* Use the given direction */
	tx = caster_ptr->fx + 99 * ddx[dir];
	ty = caster_ptr->fy + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a meteor spell, defined as a ball spell cast by an arbitary monster, 
 * player, or outside source, that starts out at an arbitrary location, and 
 * leaving no trail from the "caster" to the target.  This function is 
 * especially useful for bombardments and similar. -LM-
 *
 * Option to hurt the player.
 */
bool fire_meteor(int who, int typ, int y, int x, int dam, int rad)
{
	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	/* Analyze the "target" and the caster. */
	return (project(&creature_list[who], rad, y, x, dam, typ, flg, -1));
}


bool fire_blast(creature_type *caster_ptr, int typ, int dir, int dd, int ds, int num, int dev)
{
	int ly, lx, ld;
	int ty, tx, y, x;
	int i;

	int flg = PROJECT_FAST | PROJECT_THRU | PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE | PROJECT_GRID;

	/* Assume okay */
	bool result = TRUE;

	/* Use the given direction */
	if (dir != 5)
	{
		ly = ty = caster_ptr->fy + 20 * ddy[dir];
		lx = tx = caster_ptr->fx + 20 * ddx[dir];
	}

	/* Use an actual "target" */
	else /* if (dir == 5) */
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

			if (distance(ly, lx, y, x) <= ld * dev / 20) break;
		}

		/* Analyze the "dir" and the "target". */
		if (!project(0, 0, y, x, damroll(dd, ds), typ, flg, -1))
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
	creature_type * m_ptr;
	species_type * r_ptr;

	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}
	else
	{
		tx = creature_ptr->fx + ddx[dir];
		ty = creature_ptr->fy + ddy[dir];
	}
	c_ptr = &cave[ty][tx];

	if (creature_ptr->anti_tele)
	{
#ifdef JP
msg_print("不思議な力がテレポートを防いだ！");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif

		return FALSE;
	}

	if (!c_ptr->m_idx || (c_ptr->m_idx == creature_ptr->riding))
	{
#ifdef JP
msg_print("それとは場所を交換できません。");
#else
		msg_print("You can't trade places with that!");
#endif


		/* Failure */
		return FALSE;
	}

	if ((c_ptr->info & CAVE_ICKY) || (distance(ty, tx, creature_ptr->fy, creature_ptr->fx) > creature_ptr->lev * 3 / 2 + 10))
	{
#ifdef JP
msg_print("失敗した。");
#else
		msg_print("Failed to swap.");
#endif


		/* Failure */
		return FALSE;
	}

	m_ptr = &creature_list[c_ptr->m_idx];
	r_ptr = &species_info[m_ptr->species_idx];

	(void)set_paralyzed(m_ptr, 0);

	if (has_cf_creature(m_ptr, CF_RES_TELE))
	{
#ifdef JP
		msg_print("テレポートを邪魔された！");
#else
		msg_print("Your teleportation is blocked!");
#endif
		if (is_original_ap_and_seen(player_ptr, m_ptr)) reveal_creature_info(m_ptr, CF_RES_TELE);

		/* Failure */
		return FALSE;
	}

	sound(SOUND_TELEPORT);

	/* Swap the player and monster */
	(void)move_creature_effect(creature_ptr, ty, tx, MPE_FORGET_FLOW | MPE_HANDLE_STUFF | MPE_DONT_PICKUP);

	/* Success */
	return TRUE;
}


/*
 * Hack -- apply a "projection()" in a direction (or at the target)
 */
bool project_hook(creature_type *caster_ptr, int typ, int dir, int dam, int flg)
{
	int tx, ty;

	/* Pass through the target if needed */
	flg |= (PROJECT_THRU);

	/* Use the given direction */
	tx = caster_ptr->fx + ddx[dir];
	ty = caster_ptr->fy + ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target", do NOT explode */
	return (project(caster_ptr, 0, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a bolt spell.
 * Stop if we hit a monster, as a "bolt".
 * Affect monsters and grids (not objects).
 */
bool fire_bolt(creature_type *caster_ptr, int typ, int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE | PROJECT_GRID;
	return (project_hook(caster_ptr, typ, dir, dam, flg));
}


/*
 * Cast a beam spell.
 * Pass through monsters, as a "beam".
 * Affect monsters, grids and objects.
 */
bool fire_beam(creature_type *caster_ptr, int typ, int dir, int dam)
{
	int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(caster_ptr, typ, dir, dam, flg));
}


/*
 * Cast a bolt spell, or rarely, a beam spell
 */
bool fire_bolt_or_beam(creature_type *caster_ptr, int prob, int typ, int dir, int dam)
{
	if (randint0(100) < prob)
	{
		return (fire_beam(caster_ptr, typ, dir, dam));
	}
	else
	{
		return (fire_bolt(caster_ptr, typ, dir, dam));
	}
}


/*
 * Some of the old functions
 */
bool lite_line(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_LITE_WEAK, dir, damroll(6, 8), flg));
}


bool drain_life(creature_type *caster_ptr, int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_OLD_DRAIN, dir, dam, flg));
}


bool wall_to_mud(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_KILL_WALL, dir, 20 + randint1(30), flg));
}


bool wizard_lock(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_JAM_DOOR, dir, 20 + randint1(30), flg));
}


bool destroy_door(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(caster_ptr, GF_KILL_DOOR, dir, 0, flg));
}


bool disarm_trap(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(caster_ptr, GF_KILL_TRAP, dir, 0, flg));
}


bool heal_other_creature(creature_type *caster_ptr, int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_OLD_HEAL, dir, dam, flg));
}


bool speed_other_creature(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_OLD_SPEED, dir, caster_ptr->lev, flg));
}


bool slow_creature(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_OLD_SLOW, dir, caster_ptr->lev, flg));
}


bool sleep_creature(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_OLD_SLEEP, dir, caster_ptr->lev, flg));
}


bool stasis_creature(creature_type *caster_ptr, int dir)
{
	return (fire_ball_hide(caster_ptr, GF_STASIS, dir, caster_ptr->lev*2, 0));
}


bool stasis_evil(creature_type *caster_ptr, int dir)
{
	return (fire_ball_hide(caster_ptr, GF_STASIS_EVIL, dir, caster_ptr->lev*2, 0));
}


bool confuse_creature(creature_type *caster_ptr, int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_OLD_CONF, dir, plev, flg));
}


bool stun_creature(creature_type *caster_ptr, int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_STUN, dir, plev, flg));
}


bool poly_creature(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	bool tester = (project_hook(caster_ptr, GF_OLD_POLY, dir, caster_ptr->lev, flg));
	return(tester);
}


bool clone_creature(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_OLD_CLONE, dir, 0, flg));
}


bool fear_creature(creature_type *caster_ptr, int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_TURN_ALL, dir, plev, flg));
}


bool death_ray(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(caster_ptr, GF_DEATH_RAY, dir, caster_ptr->lev * 200, flg));
}


bool teleport_creature(creature_type *caster_ptr, int dir)
{
	int flg = PROJECT_BEAM | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_AWAY_ALL, dir, MAX_SIGHT * 5, flg));
}

/*
 * Hooks -- affect adjacent grids (radius 1 ball attack)
 */
bool door_creation(creature_type *caster_ptr)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(caster_ptr, 1, caster_ptr->fy, caster_ptr->fx, 0, GF_MAKE_DOOR, flg, -1));
}


bool trap_creation(creature_type *caster_ptr, int y, int x)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(caster_ptr, 1, y, x, 0, GF_MAKE_TRAP, flg, -1));
}


bool tree_creation(creature_type *caster_ptr)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, caster_ptr->fy, caster_ptr->fx, 0, GF_MAKE_TREE, flg, -1));
}


bool glyph_creation(creature_type *cr_ptr)
{
	int flg = PROJECT_GRID | PROJECT_ITEM;
	return (project(0, 1, cr_ptr->fy, cr_ptr->fx, 0, GF_MAKE_GLYPH, flg, -1));
}


bool wall_stone(creature_type *caster_ptr)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;

	bool dummy = (project(caster_ptr, 1, caster_ptr->fy, caster_ptr->fx, 0, GF_STONE_WALL, flg, -1));

	/* Update stuff */
	update |= (PU_FLOW);

	/* Redraw map */
	play_redraw |= (PR_MAP);

	return dummy;
}


bool destroy_doors_touch(creature_type *caster_ptr)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(caster_ptr, 1, caster_ptr->fy, caster_ptr->fx, 0, GF_KILL_DOOR, flg, -1));
}


bool sleep_monsters_touch(creature_type *cr_ptr)
{
	int flg = PROJECT_KILL | PROJECT_HIDE;
	return (project(cr_ptr, 1, cr_ptr->fy, cr_ptr->fx, cr_ptr->lev, GF_OLD_SLEEP, flg, -1));
}


bool animate_dead(creature_type *cr_ptr, int y, int x)
{
	int flg = PROJECT_ITEM | PROJECT_HIDE;
	return (project(cr_ptr, 5, y, x, 0, GF_ANIM_DEAD, flg, -1));
}


void call_chaos(creature_type *cr_ptr)
{
	int Chaos_type, dummy, dir;
	int plev = cr_ptr->lev;
	bool line_chaos = FALSE;

	int hurt_types[31] =
	{
		GF_ELEC,      GF_POIS,    GF_ACID,    GF_COLD,
		GF_FIRE,      GF_MISSILE, GF_ARROW,   GF_PLASMA,
		GF_HOLY_FIRE, GF_WATER,   GF_LITE,    GF_DARK,
		GF_FORCE,     GF_INERTIA, GF_MANA,    GF_METEOR,
		GF_ICE,       GF_CHAOS,   GF_NETHER,  GF_DISENCHANT,
		GF_SHARDS,    GF_SOUND,   GF_NEXUS,   GF_CONFUSION,
		GF_TIME,      GF_GRAVITY, GF_ROCKET,  GF_NUKE,
		GF_HELL_FIRE, GF_DISINTEGRATE, GF_PSY_SPEAR
	};

	Chaos_type = hurt_types[randint0(31)];
	if (one_in_(4)) line_chaos = TRUE;

	if (one_in_(6))
	{
		for (dummy = 1; dummy < 10; dummy++)
		{
			if (dummy - 5)
			{
				if (line_chaos)
					fire_beam(cr_ptr, Chaos_type, dummy, 150);
				else
					fire_ball(cr_ptr, Chaos_type, dummy, 150, 2);
			}
		}
	}
	else if (one_in_(3))
	{
		fire_ball(cr_ptr, Chaos_type, 0, 500, 8);
	}
	else
	{
		if (!get_aim_dir(cr_ptr, &dir)) return;
		if (line_chaos)
			fire_beam(cr_ptr, Chaos_type, dir, 250);
		else
			fire_ball(cr_ptr, Chaos_type, dir, 250, 3 + (plev / 35));
	}
}


/*
 * Activate the evil Topi Ylinen curse
 * rr9: Stop the nasty things when a Cyberdemon is summoned
 * or the player gets paralyzed.
 */
bool activate_ty_curse(creature_type *cr_ptr, bool stop_ty, int *count)
{
	int     i = 0, j;

	int flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP);

	j = randint1(34);
	do
	{
		switch (j)
		{
		case 28: case 29:
			if (!(*count))
			{
#ifdef JP
msg_print("地面が揺れた...");
#else
				msg_print("The ground trembles...");
#endif

				earthquake(cr_ptr, cr_ptr->fy, cr_ptr->fx, 5 + randint0(10));
				if (!one_in_(6)) break;
			}
		case 30: case 31:
			if (!(*count))
			{
				int dam = damroll(10, 10);
#ifdef JP
msg_print("純粋な魔力の次元への扉が開いた！");
#else
				msg_print("A portal opens to a plane of raw mana!");
#endif

				project(0, 8, cr_ptr->fy, cr_ptr->fx, dam, GF_MANA, flg, -1);
#ifdef JP
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, "純粋な魔力の解放", NULL, -1);
#else
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, dam, "released pure mana", NULL, -1);
#endif
				if (!one_in_(6)) break;
			}
		case 32: case 33:
			if (!(*count))
			{
#ifdef JP
msg_print("周囲の空間が歪んだ！");
#else
				msg_print("Space warps about you!");
#endif

				teleport_player(cr_ptr, damroll(10, 10), TELEPORT_PASSIVE);
				if (randint0(13)) (*count) += activate_hi_summon(cr_ptr, cr_ptr->fy, cr_ptr->fx, FALSE);
				if (!one_in_(6)) break;
			}
		case 34:
#ifdef JP
msg_print("エネルギーのうねりを感じた！");
#else
			msg_print("You feel a surge of energy!");
#endif

			wall_breaker(cr_ptr);
			if (!randint0(7))
			{
				project(0, 7, cr_ptr->fy, cr_ptr->fx, 50, GF_KILL_WALL, flg, -1);
#ifdef JP
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 50, "エネルギーのうねり", NULL, -1);
#else
				take_hit(NULL, cr_ptr, DAMAGE_NOESCAPE, 50, "surge of energy", NULL, -1);
#endif
			}
			if (!one_in_(6)) break;
		case 1: case 2: case 3: case 16: case 17:
			aggravate_monsters(NULL);
			if (!one_in_(6)) break;
		case 4: case 5: case 6:
			(*count) += activate_hi_summon(cr_ptr, cr_ptr->fy, cr_ptr->fx, FALSE);
			if (!one_in_(6)) break;
		case 7: case 8: case 9: case 18:
			(*count) += summon_specific(0, cr_ptr->fy, cr_ptr->fx, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			if (!one_in_(6)) break;
		case 10: case 11: case 12:
#ifdef JP
msg_print("生命力が体から吸い取られた気がする！");
#else
			msg_print("You feel your life draining away...");
#endif

			lose_exp(cr_ptr, cr_ptr->exp / 16);
			if (!one_in_(6)) break;
		case 13: case 14: case 15: case 19: case 20:
			if (stop_ty || (cr_ptr->free_act && (randint1(125) < cr_ptr->skill_rob)) || (cr_ptr->cls_idx == CLASS_BERSERKER))
			{
				/* Do nothing */ ;
			}
			else
			{
#ifdef JP
msg_print("彫像になった気分だ！");
#else
				msg_print("You feel like a statue!");
#endif

				if (cr_ptr->free_act)
					set_paralyzed(cr_ptr, cr_ptr->paralyzed + randint1(3));
				else
					set_paralyzed(cr_ptr, cr_ptr->paralyzed + randint1(13));
				stop_ty = TRUE;
			}
			if (!one_in_(6)) break;
		case 21: case 22: case 23:
			(void)do_dec_stat(cr_ptr, randint0(6));
			if (!one_in_(6)) break;
		case 24:
#ifdef JP
msg_print("ほえ？私は誰？ここで何してる？");
#else
			msg_print("Huh? Who am I? What am I doing here?");
#endif

			lose_all_info(cr_ptr);
			if (!one_in_(6)) break;
		case 25:
			/*
			 * Only summon Cyberdemons deep in the dungeon.
			 */
			if ((dun_level > 65) && !stop_ty)
			{
				(*count) += summon_cyber(NULL, cr_ptr->fy, cr_ptr->fx);
				stop_ty = TRUE;
				break;
			}
			if (!one_in_(6)) break;
		default:
			while (i < 6)
			{
				do
				{
					(void)do_dec_stat(cr_ptr, i);
				}
				while (one_in_(2));

				i++;
			}
		}
	}
	while (one_in_(3) && !stop_ty);

	return stop_ty;
}


int activate_hi_summon(creature_type *cr_ptr, int y, int x, bool can_pet)
{
	int i;
	int count = 0;
	int summon_lev;
	u32b mode = PM_ALLOW_GROUP;
	bool pet = FALSE;

	if (can_pet)
	{
		if (one_in_(4))
		{
			mode |= PM_FORCE_FRIENDLY;
		}
		else
		{
			mode |= PM_FORCE_PET;
			pet = TRUE;
		}
	}

	if (!pet) mode |= PM_NO_PET;

	summon_lev = (pet ? cr_ptr->lev * 2 / 3 + randint1(cr_ptr->lev / 2) : dun_level);

	for (i = 0; i < (randint1(7) + (dun_level / 40)); i++)
	{
		switch (randint1(25) + (dun_level / 20))
		{
			case 1: case 2:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_ANT, mode);
				break;
			case 3: case 4:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_SPIDER, mode);
				break;
			case 5: case 6:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_HOUND, mode);
				break;
			case 7: case 8:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_HYDRA, mode);
				break;
			case 9: case 10:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_ANGEL, mode);
				break;
			case 11: case 12:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_UNDEAD, mode);
				break;
			case 13: case 14:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_DRAGON, mode);
				break;
			case 15: case 16:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_DEMON, mode);
				break;
			case 17:
				if (can_pet) break;
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_AMBERITES, (mode | PM_ALLOW_UNIQUE));
				break;
			case 18: case 19:
				if (can_pet) break;
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_UNIQUE, (mode | PM_ALLOW_UNIQUE));
				break;
			case 20: case 21:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_HI_UNDEAD, mode);
				break;
			case 22: case 23:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? cr_ptr : NULL), y, x, summon_lev, SUMMON_HI_DRAGON, mode);
				break;
			case 24:
				count += summon_specific((pet ? cr_ptr : NULL), y, x, 100, SUMMON_CYBER, mode);
				break;
			default:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? cr_ptr : NULL), y, x,pet ? summon_lev : (((summon_lev * 3) / 2) + 5), 0, mode);
		}
	}

	return count;
}


/* ToDo: check */
int summon_cyber(creature_type *cr_ptr, int y, int x)
{
	int i;
	int max_cyber = (dun_level / 50) + randint1(2);
	int count = 0;
	u32b mode = PM_ALLOW_GROUP;

	/* Summoned by a monster */
	if (cr_ptr)
		if (is_pet(player_ptr, cr_ptr)) mode |= PM_FORCE_PET;

	if (max_cyber > 4) max_cyber = 4;

	for (i = 0; i < max_cyber; i++)
	{
		count += summon_specific(cr_ptr, y, x, 100, SUMMON_CYBER, mode);
	}

	return count;
}


void wall_breaker(creature_type *cr_ptr)
{
	int i;
	int y, x;
	int attempts = 1000;

	if (randint1(80 + cr_ptr->lev) < 70)
	{
		while (attempts--)
		{
			scatter(&y, &x, cr_ptr->fy, cr_ptr->fx, 4, 0);

			if (!cave_have_flag_bold(y, x, FF_PROJECT)) continue;

			if (!creature_bold(cr_ptr, y, x)) break;
		}

		project(0, 0, y, x, 20 + randint1(30), GF_KILL_WALL,
				  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
	}
	else if (randint1(100) > 30)
	{
		earthquake(cr_ptr, cr_ptr->fy, cr_ptr->fx, 1);
	}
	else
	{
		int num = damroll(5, 3);

		for (i = 0; i < num; i++)
		{
			while (1)
			{
				scatter(&y, &x, cr_ptr->fy, cr_ptr->fx, 10, 0);

				if (!creature_bold(cr_ptr, y, x)) break;
			}

			project(0, 0, y, x, 20 + randint1(30), GF_KILL_WALL,
					  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
		}
	}
}


/*
 * Confuse monsters
 */
bool confuse_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_OLD_CONF, dam));
}


/*
 * Charm monsters
 */
bool charm_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_CHARM, dam));
}


/*
 * Charm animals
 */
bool charm_animals(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_CONTROL_ANIMAL, dam));
}


/*
 * Stun monsters
 */
bool stun_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_STUN, dam));
}


/*
 * Stasis monsters
 */
bool stasis_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_STASIS, dam));
}


/*
 * Mindblast monsters
 */
bool mindblast_creatures(creature_type *caster_ptr, int dam)
{
	return (project_hack(caster_ptr, GF_PSI, dam));
}


/*
 * Banish all monsters
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
 * Death-ray all monsters (note: OBSCENELY powerful)
 */
bool deathray_creatures(creature_type *caster_ptr)
{
	return (project_hack(caster_ptr, GF_DEATH_RAY, caster_ptr->lev * 200));
}


bool charm_monster(creature_type *caster_ptr, int dir, int power)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_CHARM, dir, power, flg));
}


bool control_one_undead(creature_type *caster_ptr, int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_CONTROL_UNDEAD, dir, plev, flg));
}


bool control_one_demon(creature_type *caster_ptr, int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_CONTROL_DEMON, dir, plev, flg));
}


bool charm_animal(creature_type *caster_ptr, int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_CONTROL_ANIMAL, dir, plev, flg));
}


bool charm_living(creature_type *caster_ptr, int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(caster_ptr, GF_CONTROL_LIVING, dir, plev, flg));
}


bool kawarimi(creature_type *user_ptr, bool success)
{
	object_type forge;
	object_type *q_ptr = &forge;
	int y, x;
	char user_name[80];
	creature_desc(user_name, user_ptr, 0);

	if (gameover_e) return FALSE;
	if (user_ptr->confused || user_ptr->blind || user_ptr->paralyzed || user_ptr->image) return FALSE;
	if (randint0(200) < user_ptr->stun) return FALSE;

	if (!success && one_in_(3))
	{
#ifdef JP
		msg_format("失敗！%sは変わり身で逃げられなかった。", user_name);
#else
		msg_format("Failed! %s couldn't run away.", user_name);
#endif
		user_ptr->special_defense &= ~(NINJA_KAWARIMI);
		play_redraw |= (PR_STATUS);
		return FALSE;
	}

	y = user_ptr->fy;
	x = user_ptr->fx;

	teleport_player(user_ptr, 10 + randint1(90), 0L);

	object_wipe(q_ptr);

	object_prep(q_ptr, lookup_kind(TV_STATUE, SV_WOODEN_STATUE), ITEM_FREE_SIZE);

	q_ptr->pval = MON_NINJA;

	/* Drop it in the dungeon */
	(void)drop_near(q_ptr, -1, y, x);

#ifdef JP
	if (success) msg_format("%sは攻撃を受ける前に素早く身をひるがえした。", user_name);
	else msg_format("%sは変わり身に失敗して攻撃を受けた。", user_name);
#else
	if (success) msg_format("%s have turned around just before the attack hit you.", user_name); // TODO
	else msg_format("Failed! %s are hit by the attack.", user_name); // TODO
#endif

	user_ptr->special_defense &= ~(NINJA_KAWARIMI);
	play_redraw |= (PR_STATUS);

	/* Teleported */
	return TRUE;
}


/*
 * "Rush Attack" routine for Samurai or Ninja
 * Return value is for checking "done"
 */
bool rush_attack(creature_type *cr_ptr, bool *mdeath)
{
	int dir;
	int tx, ty;
	int tm_idx = 0;
	u16b path_g[32];
	int path_n, i;
	bool tmp_mdeath = FALSE;
	bool moved = FALSE;

	if (mdeath) *mdeath = FALSE;

	project_length = 5;
	if (!get_aim_dir(cr_ptr, &dir)) return FALSE;

	/* Use the given direction */
	tx = cr_ptr->fx + project_length * ddx[dir];
	ty = cr_ptr->fy + project_length * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	if (in_bounds(ty, tx)) tm_idx = cave[ty][tx].m_idx;

	path_n = project_path(path_g, project_length, cr_ptr->fy, cr_ptr->fx, ty, tx, PROJECT_STOP | PROJECT_KILL);
	project_length = 0;

	/* No need to move */
	if (!path_n) return TRUE;

	/* Use ty and tx as to-move point */
	ty = cr_ptr->fy;
	tx = cr_ptr->fx;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		creature_type *m_ptr;

		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		if (cave_empty_bold(ny, nx) && player_can_enter(cr_ptr, cave[ny][nx].feat, 0))
		{
			ty = ny;
			tx = nx;

			/* Go to next grid */
			continue;
		}

		if (!cave[ny][nx].m_idx)
		{
			if (tm_idx)
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

		/* Move player before updating the monster */
		if (!creature_bold(cr_ptr, ty, tx)) teleport_creature_to(cr_ptr, ty, tx, TELEPORT_NONMAGICAL);

		/* Update the monster */
		update_mon(cr_ptr, cave[ny][nx].m_idx, TRUE);

		/* Found a monster */
		m_ptr = &creature_list[cave[ny][nx].m_idx];

		if (tm_idx != cave[ny][nx].m_idx)
		{
#ifdef JP
			msg_format("%s%sが立ちふさがっている！", tm_idx ? "別の" : "",
				   m_ptr->ml ? "モンスター" : "何か");
#else
			msg_format("There is %s in the way!", m_ptr->ml ? (tm_idx ? "another monster" : "a monster") : "someone");
#endif
		}
		else if (!creature_bold(cr_ptr, ty, tx))
		{
			/* Hold the monster name */
			char m_name[80];

			/* Get the monster name (BEFORE polymorphing) */
			creature_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("素早く%sの懐に入り込んだ！", m_name);
#else
			msg_format("You quickly jump in and attack %s!", m_name);
#endif
		}

		if (!creature_bold(cr_ptr, ty, tx)) teleport_creature_to(cr_ptr, ty, tx, TELEPORT_NONMAGICAL);
		moved = TRUE;
		tmp_mdeath = weapon_attack(cr_ptr, ny, nx, HISSATSU_NYUSIN);

		break;
	}

	if (!moved && !creature_bold(cr_ptr, ty, tx)) teleport_creature_to(cr_ptr, ty, tx, TELEPORT_NONMAGICAL);

	if (mdeath) *mdeath = tmp_mdeath;
	return TRUE;
}


/*
 * Remove all mirrors in this floor
 */
void remove_all_mirrors(creature_type *user_ptr, bool explode)
{
	int x, y;

	for (x = 0; x < cur_wid; x++)
	{
		for (y = 0; y < cur_hgt; y++)
		{
			if (is_mirror_grid(&cave[y][x]))
			{
				remove_mirror(player_ptr, y, x);
				if (explode)
					project(0, 2, y, x, user_ptr->lev / 2 + 5, GF_SHARDS,
						(PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}
		}
	}
}
