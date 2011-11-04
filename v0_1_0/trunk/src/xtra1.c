
/* File: misc.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: misc code */

#include "angband.h"




/*
 * Converts stat num into a six-char (right justified) string
 */
void cnv_stat(int val, char *out_val)
{
	/* Above 18 */
	if (val > 18)
	{
		int bonus = (val - 18);

		if (bonus >= 420)
		{
			sprintf(out_val, "  ****");
		}
		else
		{
			sprintf(out_val, "  %02d.%01d", (18 + bonus / 10),  bonus % 10);
		}
	}

	/* From 3 to 18 */
	else
	{
		sprintf(out_val, "  %2d.0", val);
	}
}


/*
 * Modify a stat value by a "modifier", return new value
 *
 * Stats go up: 3,4,...,17,18,18/10,18/20,...,18/220
 * Or even: 18/13, 18/23, 18/33, ..., 18/220
 *
 * Stats go down: 18/220, 18/210,..., 18/10, 18, 17, ..., 3
 * Or even: 18/13, 18/03, 18, 17, ..., 3
 */
s16b modify_stat_value(int value, int amount)
{
	int    i;

	/* Reward */
	if (amount > 0)
	{
		/* Apply each point */
		for (i = 0; i < amount; i++)
		{
			/* One point at a time */
			if (value < 18) value++;

			/* Ten "points" at a time */
			else value += 10;
		}
	}

	/* Penalty */
	else if (amount < 0)
	{
		/* Apply each point */
		for (i = 0; i < (0 - amount); i++)
		{
			/* Ten points at a time */
			if (value >= 18+10) value -= 10;

			/* Hack -- prevent weirdness */
			else if (value > 18) value = 18;

			/* One point at a time */
			else if (value > 3) value--;
		}
	}

	/* Return new value */
	return (value);
}



/*
 * Print character info at given row, column in a 13 char field
 */
static void prt_field(cptr info, int row, int col)
{
	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", row, col);

	/* Dump the info itself */
	c_put_str(TERM_L_BLUE, info, row, col);
}


/*
 *  Whether daytime or not
 */
bool is_daytime(void)
{
	s32b len = TURNS_PER_TICK * TOWN_DAWN;
	if ((turn % len) < (len / 2))
		return TRUE;
	else
		return FALSE;
}

/*
 * Extract day, hour, min
 */
void extract_day_hour_min(creature_type *cr_ptr, int *day, int *hour, int *min)
{
	const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;
	s32b turn_in_today = (turn + A_DAY / 4) % A_DAY;

	if(is_undead_creature(p_ptr))
	{
		*day = (turn - A_DAY * 3 / 4) / A_DAY + 1;
	}
	else
	{
		*day = (turn + A_DAY / 4) / A_DAY + 1;
	}
	*hour = (24 * turn_in_today / A_DAY) % 24;
	*min = (1440 * turn_in_today / A_DAY) % 60;
}

/*
 * Print time
 */
void prt_time(void)
{
	int day, hour, min;

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DAY, COL_DAY);

	extract_day_hour_min(p_ptr, &day, &hour, &min);

	/* Dump the info itself */
#ifdef JP
	if (day < 1000) c_put_str(TERM_WHITE, format("%2d日目", day), ROW_DAY, COL_DAY);
	else c_put_str(TERM_WHITE, "***日目", ROW_DAY, COL_DAY);
#else
	if (day < 1000) c_put_str(TERM_WHITE, format("Day%3d", day), ROW_DAY, COL_DAY);
	else c_put_str(TERM_WHITE, "Day***", ROW_DAY, COL_DAY);
#endif

	c_put_str(TERM_WHITE, format("%2d:%02d", hour, min), ROW_DAY, COL_DAY+7);
}


cptr map_name(void)
{
	if (inside_quest && is_fixed_quest_idx(inside_quest)
	    && (quest[inside_quest].flags & QUEST_FLAG_PRESET))
#ifdef JP
		return "クエスト";
#else
		return "Quest";
#endif
	else if (wild_mode)
#ifdef JP
		return "地上";
#else
		return "Surface";
#endif
	else if (inside_arena)
#ifdef JP
		return "アリーナ";
#else
		return "Arena";
#endif
	else if (inside_battle)
#ifdef JP
		return "闘技場";
#else
		return "Monster Arena";
#endif
	else if (!dun_level && town_num)
		return town[town_num].name;
	else
		return d_name+d_info[dungeon_type].name;
}

/*
 * Print dungeon
 */
static void prt_dungeon(void)
{
	cptr dungeon_name;
	int col;

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DUNGEON, COL_DUNGEON);

	dungeon_name = map_name();

	col = COL_DUNGEON + 6 - strlen(dungeon_name)/2;
	if (col < 0) col = 0;

	/* Dump the info itself */
	c_put_str(TERM_L_UMBER, format("%s",dungeon_name),
		  ROW_DUNGEON, col);
}




/*
 * Print character stat in given row, column
 */
static void prt_stat(creature_type *cr_ptr, int stat)
{
	char tmp[32];

	/* Display "injured" stat */
	if (cr_ptr->stat_cur[stat] < cr_ptr->stat_max[stat])
	{
		put_str(stat_names_reduced[stat], ROW_STAT + stat, 0);
		cnv_stat(cr_ptr->stat_use[stat], tmp);
		c_put_str(TERM_YELLOW, tmp, ROW_STAT + stat, COL_STAT + 6);
	}

	/* Display "healthy" stat */
	else
	{
		put_str(stat_names[stat], ROW_STAT + stat, 0);
		cnv_stat(cr_ptr->stat_use[stat], tmp);
		c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
	}

	/* Indicate natural maximum */
	if (cr_ptr->stat_max[stat] == cr_ptr->stat_max_max[stat])
	{
#ifdef JP
		/* 日本語にかぶらないように表示位置を変更 */
		put_str("!", ROW_STAT + stat, 5);
#else
		put_str("!", ROW_STAT + stat, 3);
#endif

	}
}


/*
 *  Data structure for status bar
 */
#define BAR_TSUYOSHI 0
#define BAR_HALLUCINATION 1
#define BAR_BLINDNESS 2
#define BAR_PARALYZE 3
#define BAR_CONFUSE 4
#define BAR_POISONED 5
#define BAR_AFRAID 6
#define BAR_LEVITATE 7
#define BAR_REFLECTION 8
#define BAR_PASSWALL 9
#define BAR_WRAITH 10
#define BAR_PROTEVIL 11
#define BAR_KAWARIMI 12
#define BAR_MAGICDEFENSE 13
#define BAR_EXPAND 14
#define BAR_STONESKIN 15
#define BAR_MULTISHADOW 16
#define BAR_REGMAGIC 17
#define BAR_ULTIMATE 18
#define BAR_INVULN 19
#define BAR_IMMACID 20
#define BAR_RESACID 21
#define BAR_IMMELEC 22
#define BAR_RESELEC 23
#define BAR_IMMFIRE 24
#define BAR_RESFIRE 25
#define BAR_IMMCOLD 26
#define BAR_RESCOLD 27
#define BAR_RESPOIS 28
#define BAR_RESNETH 29
#define BAR_RESTIME 30
#define BAR_DUSTROBE 31
#define BAR_SHFIRE 32
#define BAR_TOUKI 33
#define BAR_SHHOLY 34
#define BAR_EYEEYE 35
#define BAR_BLESSED 36
#define BAR_HEROISM 37
#define BAR_BERSERK 38
#define BAR_ATTKFIRE 39
#define BAR_ATTKCOLD 40
#define BAR_ATTKELEC 41
#define BAR_ATTKACID 42
#define BAR_ATTKPOIS 43
#define BAR_ATTKCONF 44
#define BAR_SENSEUNSEEN 45
#define BAR_TELEPATHY 46
#define BAR_REGENERATION 47
#define BAR_INFRAVISION 48
#define BAR_STEALTH 49
#define BAR_SUPERSTEALTH 50
#define BAR_RECALL 51
#define BAR_ALTER 52
#define BAR_SHCOLD 53
#define BAR_SHELEC 54
#define BAR_SHSHADOW 55
#define BAR_MIGHT 56
#define BAR_BUILD 57
#define BAR_ANTIMULTI 58
#define BAR_ANTITELE 59
#define BAR_ANTIMAGIC 60
#define BAR_PATIENCE 61
#define BAR_REVENGE 62
#define BAR_RUNESWORD 63
#define BAR_VAMPILIC 64
#define BAR_CURE 65
#define BAR_ESP_EVIL 66

static struct {
	byte attr;
	cptr sstr;
	cptr lstr;
} bar[]
#ifdef JP
= {
	{TERM_YELLOW, "つ", "つよし"},
	{TERM_VIOLET, "幻", "幻覚"},
	{TERM_L_DARK, "盲", "盲目"},
	{TERM_RED, "痺", "麻痺"},
	{TERM_VIOLET, "乱", "混乱"},
	{TERM_GREEN, "毒", "毒"},
	{TERM_BLUE, "恐", "恐怖"},
	{TERM_L_BLUE, "浮", "浮遊"},
	{TERM_SLATE, "反", "反射"},
	{TERM_SLATE, "壁", "壁抜け"},
	{TERM_L_DARK, "幽", "幽体"},
	{TERM_SLATE, "邪", "防邪"},
	{TERM_VIOLET, "変", "変わり身"},
	{TERM_YELLOW, "魔", "魔法鎧"},
	{TERM_L_UMBER, "伸", "伸び"},
	{TERM_WHITE, "石", "石肌"},
	{TERM_L_BLUE, "分", "分身"},
	{TERM_SLATE, "防", "魔法防御"},
	{TERM_YELLOW, "究", "究極"},
	{TERM_YELLOW, "無", "無敵"},
	{TERM_L_GREEN, "酸", "酸免疫"},
	{TERM_GREEN, "酸", "耐酸"},
	{TERM_L_BLUE, "電", "電免疫"},
	{TERM_BLUE, "電", "耐電"},
	{TERM_L_RED, "火", "火免疫"},
	{TERM_RED, "火", "耐火"},
	{TERM_WHITE, "冷", "冷免疫"},
	{TERM_SLATE, "冷", "耐冷"},
	{TERM_GREEN, "毒", "耐毒"},
	{TERM_L_DARK, "獄", "耐地獄"},
	{TERM_L_BLUE, "時", "耐時間"},
	{TERM_L_DARK, "鏡", "鏡オーラ"},
	{TERM_L_RED, "オ", "火オーラ"},
	{TERM_WHITE, "闘", "闘気"},
	{TERM_WHITE, "聖", "聖オーラ"},
	{TERM_VIOLET, "目", "目には目"},
	{TERM_WHITE, "祝", "祝福"},
	{TERM_WHITE, "勇", "勇"},
	{TERM_RED, "狂", "狂乱"},
	{TERM_L_RED, "火", "魔剣火"},
	{TERM_WHITE, "冷", "魔剣冷"},
	{TERM_L_BLUE, "電", "魔剣電"},
	{TERM_SLATE, "酸", "魔剣酸"},
	{TERM_L_GREEN, "毒", "魔剣毒"},
	{TERM_RED, "乱", "混乱打撃"},
	{TERM_L_BLUE, "視", "透明視"},
	{TERM_ORANGE, "テ", "テレパシ"},
	{TERM_L_BLUE, "回", "回復"},
	{TERM_L_RED, "赤", "赤外"},
	{TERM_UMBER, "隠", "隠密"},
	{TERM_YELLOW, "隠", "超隠密"},
	{TERM_WHITE, "帰", "帰還"},
	{TERM_WHITE, "現", "現実変容"},
	/* Hex */
	{TERM_WHITE, "オ", "氷オーラ"},
	{TERM_BLUE, "オ", "電オーラ"},
	{TERM_L_DARK, "オ", "影オーラ"},
	{TERM_YELLOW, "腕", "腕力強化"},
	{TERM_RED, "肉", "肉体強化"},
	{TERM_L_DARK, "殖", "反増殖"},
	{TERM_ORANGE, "テ", "反テレポ"},
	{TERM_RED, "魔", "反魔法"},
	{TERM_SLATE, "我", "我慢"},
	{TERM_SLATE, "宣", "宣告"},
	{TERM_L_DARK, "剣", "魔剣化"},
	{TERM_RED, "吸", "吸血打撃"},
	{TERM_WHITE, "回", "回復"},
	{TERM_L_DARK, "感", "邪悪感知"},
	{0, NULL, NULL}
};
#else
= {
	{TERM_YELLOW, "Ts", "Tsuyoshi"},
	{TERM_VIOLET, "Ha", "Halluc"},
	{TERM_L_DARK, "Bl", "Blind"},
	{TERM_RED, "Pa", "Paralyzed"},
	{TERM_VIOLET, "Cf", "Confused"},
	{TERM_GREEN, "Po", "Poisoned"},
	{TERM_BLUE, "Af", "Afraid"},
	{TERM_L_BLUE, "Lv", "Levit"},
	{TERM_SLATE, "Rf", "Reflect"},
	{TERM_SLATE, "Pw", "PassWall"},
	{TERM_L_DARK, "Wr", "Wraith"},
	{TERM_SLATE, "Ev", "PrtEvl"},
	{TERM_VIOLET, "Kw", "Kawarimi"},
	{TERM_YELLOW, "Md", "MgcArm"},
	{TERM_L_UMBER, "Eh", "Expand"},
	{TERM_WHITE, "Ss", "StnSkn"},
	{TERM_L_BLUE, "Ms", "MltShdw"},
	{TERM_SLATE, "Rm", "ResMag"},
	{TERM_YELLOW, "Ul", "Ultima"},
	{TERM_YELLOW, "Iv", "Invuln"},
	{TERM_L_GREEN, "IAc", "ImmAcid"},
	{TERM_GREEN, "Ac", "Acid"},
	{TERM_L_BLUE, "IEl", "ImmElec"},
	{TERM_BLUE, "El", "Elec"},
	{TERM_L_RED, "IFi", "ImmFire"},
	{TERM_RED, "Fi", "Fire"},
	{TERM_WHITE, "ICo", "ImmCold"},
	{TERM_SLATE, "Co", "Cold"},
	{TERM_GREEN, "Po", "Pois"},
	{TERM_L_DARK, "Nt", "Nthr"},
	{TERM_L_BLUE, "Ti", "Time"},
	{TERM_L_DARK, "Mr", "Mirr"},
	{TERM_L_RED, "SFi", "SFire"},
	{TERM_WHITE, "Fo", "Force"},
	{TERM_WHITE, "Ho", "Holy"},
	{TERM_VIOLET, "Ee", "EyeEye"},
	{TERM_WHITE, "Bs", "Bless"},
	{TERM_WHITE, "He", "Hero"},
	{TERM_RED, "Br", "Berserk"},
	{TERM_L_RED, "BFi", "BFire"},
	{TERM_WHITE, "BCo", "BCold"},
	{TERM_L_BLUE, "BEl", "BElec"},
	{TERM_SLATE, "BAc", "BAcid"},
	{TERM_L_GREEN, "BPo", "BPois"},
	{TERM_RED, "TCf", "TchCnf"},
	{TERM_L_BLUE, "Se", "SInv"},
	{TERM_ORANGE, "Te", "Telepa"},
	{TERM_L_BLUE, "Rg", "Regen"},
	{TERM_L_RED, "If", "Infr"},
	{TERM_UMBER, "Sl", "Stealth"},
	{TERM_YELLOW, "Stlt", "Stealth"},
	{TERM_WHITE, "Rc", "Recall"},
	{TERM_WHITE, "Al", "Alter"},
	/* Hex */
	{TERM_WHITE, "SCo", "SCold"},
	{TERM_BLUE, "SEl", "SElec"},
	{TERM_L_DARK, "SSh", "SShadow"},
	{TERM_YELLOW, "EMi", "ExMight"},
	{TERM_RED, "Bu", "BuildUp"},
	{TERM_L_DARK, "AMl", "AntiMulti"},
	{TERM_ORANGE, "AT", "AntiTele"},
	{TERM_RED, "AM", "AntiMagic"},
	{TERM_SLATE, "Pa", "Patience"},
	{TERM_SLATE, "Rv", "Revenge"},
	{TERM_L_DARK, "Rs", "RuneSword"},
	{TERM_RED, "Vm", "Vampiric"},
	{TERM_WHITE, "Cu", "Cure"},
	{TERM_L_DARK, "ET", "EvilTele"},
	{0, NULL, NULL}
};
#endif

#define ADD_FLG(FLG) (bar_flags[FLG / 32] |= (1L << (FLG % 32)))
#define IS_FLG(FLG) (bar_flags[FLG / 32] & (1L << (FLG % 32)))


/*
 *  Show status bar
 */
static void prt_status(creature_type *cr_ptr)
{
	u32b bar_flags[3];
	int wid, hgt, row_statbar, max_col_statbar;
	int i, col = 0, num = 0;
	int space = 2;

	Term_get_size(&wid, &hgt);
	row_statbar = hgt + ROW_STATBAR;
	max_col_statbar = wid + MAX_COL_STATBAR;

	Term_erase(0, row_statbar, max_col_statbar);

	bar_flags[0] = bar_flags[1] = bar_flags[2] = 0L;

	/* Tsuyoshi  */
	if (cr_ptr->tsuyoshi) ADD_FLG(BAR_TSUYOSHI);

	/* Hallucinating */
	if (cr_ptr->image) ADD_FLG(BAR_HALLUCINATION);

	/* Blindness */
	if (cr_ptr->blind) ADD_FLG(BAR_BLINDNESS);

	/* Paralysis */
	if (cr_ptr->paralyzed) ADD_FLG(BAR_PARALYZE);

	/* Confusion */
	if (cr_ptr->confused) ADD_FLG(BAR_CONFUSE);

	/* Posioned */
	if (cr_ptr->poisoned) ADD_FLG(BAR_POISONED);

	/* Times see-invisible */
	if (cr_ptr->tim_invis) ADD_FLG(BAR_SENSEUNSEEN);

	/* Timed esp */
	if (IS_TIM_ESP(cr_ptr)) ADD_FLG(BAR_TELEPATHY);

	/* Timed regenerate */
	if (cr_ptr->tim_regen) ADD_FLG(BAR_REGENERATION);

	/* Timed infra-vision */
	if (cr_ptr->tim_infra) ADD_FLG(BAR_INFRAVISION);

	/* Protection from evil */
	if (cr_ptr->protevil) ADD_FLG(BAR_PROTEVIL);

	/* Invulnerability */
	if (IS_INVULN(cr_ptr)) ADD_FLG(BAR_INVULN);

	/* Wraith form */
	if (cr_ptr->wraith_form) ADD_FLG(BAR_WRAITH);

	/* Kabenuke */
	if (cr_ptr->kabenuke) ADD_FLG(BAR_PASSWALL);

	if (cr_ptr->tim_reflect) ADD_FLG(BAR_REFLECTION);

	/* Heroism */
	if (IS_HERO(cr_ptr)) ADD_FLG(BAR_HEROISM);

	/* Super Heroism / berserk */
	if (cr_ptr->shero) ADD_FLG(BAR_BERSERK);

	/* Blessed */
	if (IS_BLESSED(cr_ptr)) ADD_FLG(BAR_BLESSED);

	/* Shield */
	if (cr_ptr->magicdef) ADD_FLG(BAR_MAGICDEFENSE);

	if (cr_ptr->tsubureru) ADD_FLG(BAR_EXPAND);

	if (cr_ptr->shield) ADD_FLG(BAR_STONESKIN);
	
	if (cr_ptr->special_defense & NINJA_KAWARIMI) ADD_FLG(BAR_KAWARIMI);

	/* Oppose Acid */
	if (cr_ptr->special_defense & DEFENSE_ACID) ADD_FLG(BAR_IMMACID);
	if (IS_OPPOSE_ACID(cr_ptr)) ADD_FLG(BAR_RESACID);

	/* Oppose Lightning */
	if (cr_ptr->special_defense & DEFENSE_ELEC) ADD_FLG(BAR_IMMELEC);
	if (IS_OPPOSE_ELEC(cr_ptr)) ADD_FLG(BAR_RESELEC);

	/* Oppose Fire */
	if (cr_ptr->special_defense & DEFENSE_FIRE) ADD_FLG(BAR_IMMFIRE);
	if (IS_OPPOSE_FIRE(cr_ptr)) ADD_FLG(BAR_RESFIRE);

	/* Oppose Cold */
	if (cr_ptr->special_defense & DEFENSE_COLD) ADD_FLG(BAR_IMMCOLD);
	if (IS_OPPOSE_COLD(cr_ptr)) ADD_FLG(BAR_RESCOLD);

	/* Oppose Poison */
	if (IS_OPPOSE_POIS(cr_ptr)) ADD_FLG(BAR_RESPOIS);

	/* Word of Recall */
	if (cr_ptr->word_recall) ADD_FLG(BAR_RECALL);

	/* Alter realiry */
	if (cr_ptr->alter_reality) ADD_FLG(BAR_ALTER);

	/* Afraid */
	if (cr_ptr->afraid) ADD_FLG(BAR_AFRAID);

	/* Resist time */
	if (cr_ptr->tim_res_time) ADD_FLG(BAR_RESTIME);

	if (cr_ptr->multishadow) ADD_FLG(BAR_MULTISHADOW);

	/* Confusing Hands */
	if (cr_ptr->special_attack & ATTACK_CONFUSE) ADD_FLG(BAR_ATTKCONF);

	if (cr_ptr->resist_magic) ADD_FLG(BAR_REGMAGIC);

	/* Ultimate-resistance */
	if (cr_ptr->ult_res) ADD_FLG(BAR_ULTIMATE);

	/* tim levitation */
	if (cr_ptr->tim_levitation) ADD_FLG(BAR_LEVITATE);

	if (cr_ptr->tim_res_nether) ADD_FLG(BAR_RESNETH);

	if (cr_ptr->dustrobe) ADD_FLG(BAR_DUSTROBE);

	/* Mahouken */
	if (cr_ptr->special_attack & ATTACK_FIRE) ADD_FLG(BAR_ATTKFIRE);
	if (cr_ptr->special_attack & ATTACK_COLD) ADD_FLG(BAR_ATTKCOLD);
	if (cr_ptr->special_attack & ATTACK_ELEC) ADD_FLG(BAR_ATTKELEC);
	if (cr_ptr->special_attack & ATTACK_ACID) ADD_FLG(BAR_ATTKACID);
	if (cr_ptr->special_attack & ATTACK_POIS) ADD_FLG(BAR_ATTKPOIS);
	if (cr_ptr->special_defense & NINJA_S_STEALTH) ADD_FLG(BAR_SUPERSTEALTH);

	if (cr_ptr->tim_sh_fire) ADD_FLG(BAR_SHFIRE);

	/* tim stealth */
	if (IS_TIM_STEALTH(cr_ptr)) ADD_FLG(BAR_STEALTH);

	if (cr_ptr->tim_sh_touki) ADD_FLG(BAR_TOUKI);

	/* Holy aura */
	if (cr_ptr->tim_sh_holy) ADD_FLG(BAR_SHHOLY);

	/* An Eye for an Eye */
	if (cr_ptr->tim_eyeeye) ADD_FLG(BAR_EYEEYE);

	/* Hex spells */
	if (cr_ptr->realm1 == REALM_HEX)
	{
		if (hex_spelling(cr_ptr, HEX_BLESS)) ADD_FLG(BAR_BLESSED);
		if (hex_spelling(cr_ptr, HEX_DEMON_AURA)) { ADD_FLG(BAR_SHFIRE); ADD_FLG(BAR_REGENERATION); }
		if (hex_spelling(cr_ptr, HEX_XTRA_MIGHT)) ADD_FLG(BAR_MIGHT);
		if (hex_spelling(cr_ptr, HEX_DETECT_EVIL)) ADD_FLG(BAR_ESP_EVIL);
		if (hex_spelling(cr_ptr, HEX_ICE_ARMOR)) ADD_FLG(BAR_SHCOLD);
		if (hex_spelling(cr_ptr, HEX_RUNESWORD)) ADD_FLG(BAR_RUNESWORD);
		if (hex_spelling(cr_ptr, HEX_BUILDING)) ADD_FLG(BAR_BUILD);
		if (hex_spelling(cr_ptr, HEX_ANTI_TELE)) ADD_FLG(BAR_ANTITELE);
		if (hex_spelling(cr_ptr, HEX_SHOCK_CLOAK)) ADD_FLG(BAR_SHELEC);
		if (hex_spelling(cr_ptr, HEX_SHADOW_CLOAK)) ADD_FLG(BAR_SHSHADOW);
		if (hex_spelling(cr_ptr, HEX_CONFUSION)) ADD_FLG(BAR_ATTKCONF);
		if (hex_spelling(cr_ptr, HEX_EYE_FOR_EYE)) ADD_FLG(BAR_EYEEYE);
		if (hex_spelling(cr_ptr, HEX_ANTI_MULTI)) ADD_FLG(BAR_ANTIMULTI);
		if (hex_spelling(cr_ptr, HEX_VAMP_BLADE)) ADD_FLG(BAR_VAMPILIC);
		if (hex_spelling(cr_ptr, HEX_ANTI_MAGIC)) ADD_FLG(BAR_ANTIMAGIC);
		if (hex_spelling(cr_ptr, HEX_CURE_LIGHT) ||
			hex_spelling(cr_ptr, HEX_CURE_SERIOUS) ||
			hex_spelling(cr_ptr, HEX_CURE_CRITICAL)) ADD_FLG(BAR_CURE);

		if (cr_ptr->magic_num2[2])
		{
			if (cr_ptr->magic_num2[1] == 1) ADD_FLG(BAR_PATIENCE);
			if (cr_ptr->magic_num2[1] == 2) ADD_FLG(BAR_REVENGE);
		}
	}

	/* Calcurate length */
	for (i = 0; bar[i].sstr; i++)
	{
		if (IS_FLG(i))
		{
			col += strlen(bar[i].lstr) + 1;
			num++;
		}
	}

	/* If there are not excess spaces for long strings, use short one */
	if (col - 1 > max_col_statbar)
	{
		space = 0;
		col = 0;

		for (i = 0; bar[i].sstr; i++)
		{
			if (IS_FLG(i))
			{
				col += strlen(bar[i].sstr);
			}
		}

		/* If there are excess spaces for short string, use more */
		if (col - 1 <= max_col_statbar - (num-1))
		{
			space = 1;
			col += num - 1;
		}
	}


	/* Centering display column */
	col = (max_col_statbar - col) / 2;

	/* Display status bar */
	for (i = 0; bar[i].sstr; i++)
	{
		if (IS_FLG(i))
		{
			cptr str;
			if (space == 2) str = bar[i].lstr;
			else str = bar[i].sstr;

			c_put_str(bar[i].attr, str, row_statbar, col);
			col += strlen(str);
			if (space > 0) col++;
			if (col > max_col_statbar) break;
		}
	}
}


/*
 * Prints level
 */
static void prt_level(creature_type *cr_ptr)
{
	char tmp[32];

	sprintf(tmp, "%2d/%2d", cr_ptr->lev, cr_ptr->max_lev);

	if (cr_ptr->lev >= cr_ptr->max_plv)
	{
		if(cr_ptr->lev >= cr_ptr->max_lev)
			put_str("LEVEL!", ROW_LEVEL, 0);
		else
			put_str("LEVEL ", ROW_LEVEL, 0);
				
		c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 7);
	}
	else
	{
		put_str("Level ", ROW_LEVEL, 0);
		c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 7);
	}
}


/*
 * Display the experience
 */
static void prt_exp(creature_type *cr_ptr)
{
	char out_val[32];

	if ((!exp_need)||(IS_RACE(cr_ptr, RACE_ANDROID)))
	{
		(void)sprintf(out_val, "%8ld", (long)cr_ptr->exp);
	}
	else
	{
		if (cr_ptr->lev >= cr_ptr->max_lev)
		{
			(void)sprintf(out_val, "********");
		}
		else
		{
			(void)sprintf(out_val, "%8ld", (long)(creature_exp[cr_ptr->lev - 1] * cr_ptr->expfact / 100L) - cr_ptr->exp);
		}
	}

	if (cr_ptr->exp >= cr_ptr->max_exp)
	{
		if (IS_RACE(cr_ptr, RACE_ANDROID)) put_str("Cst ", ROW_EXP, 0);
		else put_str("EXP ", ROW_EXP, 0);
		c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 4);

	}
	else
	{
		put_str("Exp ", ROW_EXP, 0);
		c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 4);
	}
}

/*
 * Prints current gold
 */
static void prt_gold(creature_type *cr_ptr)
{
	char tmp[32];

#ifdef JP
	put_str("＄ ", ROW_GOLD, COL_GOLD);
#else
	put_str("AU ", ROW_GOLD, COL_GOLD);
#endif

	sprintf(tmp, "%9ld", (long)cr_ptr->au);
	c_put_str(TERM_L_GREEN, tmp, ROW_GOLD, COL_GOLD + 3);
}



/*
 * Prints current AC
 */
static void prt_ac(creature_type *cr_ptr)
{
	char tmp[32];

#ifdef JP
/* AC の表示方式を変更している */
	put_str("  AC (     )", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", cr_ptr->dis_ac + cr_ptr->dis_to_a);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 6);
#else
	put_str("Cur AC ", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", cr_ptr->dis_ac + cr_ptr->dis_to_a);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 7);
#endif

}


/*
 * Prints Cur/Max hit points
 */
static void prt_hp(creature_type *cr_ptr)
{
/* ヒットポイントの表示方法を変更 */
	char tmp[32];
  
	byte color;
  
	/* タイトル */
/*	put_str(" ＨＰ・ＭＰ", ROW_HPMP, COL_HPMP); */

	put_str("HP", ROW_CURHP, COL_CURHP);

	/* 現在のヒットポイント */
	sprintf(tmp, "%4ld", cr_ptr->chp);

	if (cr_ptr->chp >= cr_ptr->mhp)
	{
		color = TERM_L_GREEN;
	}
	else if (cr_ptr->chp > (cr_ptr->mhp * hitpoint_warn) / 10)
	{
		color = TERM_YELLOW;
	}
	else
	{
		color = TERM_RED;
	}

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP+3);

	/* 区切り */
	put_str( "/", ROW_CURHP, COL_CURHP + 7 );

	/* 最大ヒットポイント */
	sprintf(tmp, "%4ld", cr_ptr->mhp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP + 8 );
}


/*
 * Prints players max/cur spell points
 */
static void prt_sp(creature_type *cr_ptr)
{
/* マジックポイントの表示方法を変更している */
	char tmp[32];
	byte color;

	/* タイトル */
/*	put_str(" ＭＰ / 最大", ROW_MAXSP, COL_MAXSP); */

#ifdef JP
	put_str("MP", ROW_CURSP, COL_CURSP);
#else
	put_str("SP", ROW_CURSP, COL_CURSP);
#endif

	/* 現在のマジックポイント */
	sprintf(tmp, "%4ld", cr_ptr->csp);

	if (cr_ptr->csp >= cr_ptr->msp)
	{
		color = TERM_L_GREEN;
	}
	else if (cr_ptr->csp > (cr_ptr->msp * mana_warn) / 10)
	{
		color = TERM_YELLOW;
	}
	else
	{
		color = TERM_RED;
	}

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP+3);

	/* 区切り */
	put_str( "/", ROW_CURSP, COL_CURSP + 7 );

	/* 最大マジックポイント */
	sprintf(tmp, "%4ld", cr_ptr->msp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP + 8);
}


/*
 * Prints depth in stat area
 */
static void prt_depth(creature_type *cr_ptr)
{
	char depths[32];
	int wid, hgt, row_depth, col_depth;
	byte attr = TERM_WHITE;

	Term_get_size(&wid, &hgt);
	col_depth = wid + COL_DEPTH;
	row_depth = hgt + ROW_DEPTH;

	if (!dun_level)
	{
#ifdef JP
		strcpy(depths, "地上");
#else
		strcpy(depths, "Surf.");
#endif
	}
	else if (inside_quest && !dungeon_type)
	{
#ifdef JP
		strcpy(depths, "地上");
#else
		strcpy(depths, "Quest");
#endif
	}
	else
	{
#ifdef JP
		if (depth_in_feet) (void)sprintf(depths, "%d ft", dun_level * 50);
		else (void)sprintf(depths, "%d 階", dun_level);
#else
		if (depth_in_feet) (void)sprintf(depths, "%d ft", dun_level * 50);
		else (void)sprintf(depths, "Lev %d", dun_level);
#endif


		/* Get color of level based on feeling  -JSV- */
		switch (cr_ptr->feeling)
		{
		case  0: attr = TERM_SLATE;   break; /* Unknown */
		case  1: attr = TERM_L_BLUE;  break; /* Special */
		case  2: attr = TERM_VIOLET;  break; /* Horrible visions */
		case  3: attr = TERM_RED;     break; /* Very dangerous */
		case  4: attr = TERM_L_RED;   break; /* Very bad feeling */
		case  5: attr = TERM_ORANGE;  break; /* Bad feeling */
		case  6: attr = TERM_YELLOW;  break; /* Nervous */
		case  7: attr = TERM_L_UMBER; break; /* Luck is turning */
		case  8: attr = TERM_L_WHITE; break; /* Don't like */
		case  9: attr = TERM_WHITE;   break; /* Reasonably safe */
		case 10: attr = TERM_WHITE;   break; /* Boring place */
		}
	}

	/* Right-Adjust the "depth", and clear old values */
	c_prt(attr, format("%7s", depths), row_depth, col_depth);
}


/*
 * Prints status of hunger
 */
static void prt_hunger(creature_type *cr_ptr)
{
	/* Fainting / Starving */
	if (cr_ptr->food < PY_FOOD_FAINT)
	{
#ifdef JP
		c_put_str(TERM_RED, "衰弱  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_RED, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Weak */
	else if (cr_ptr->food < PY_FOOD_WEAK)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "衰弱  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_ORANGE, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Hungry */
	else if (cr_ptr->food < PY_FOOD_ALERT)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "空腹  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_YELLOW, "Hungry", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Normal */
	else if (cr_ptr->food < PY_FOOD_FULL)
	{
		c_put_str(TERM_L_GREEN, "      ", ROW_HUNGRY, COL_HUNGRY);
	}

	/* Full */
	else if (cr_ptr->food < PY_FOOD_MAX)
	{
#ifdef JP
		c_put_str(TERM_L_GREEN, "満腹  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_L_GREEN, "Full  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Gorged */
	else
	{
#ifdef JP
		c_put_str(TERM_GREEN, "食過ぎ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_GREEN, "Gorged", ROW_HUNGRY, COL_HUNGRY);
#endif

	}
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status
 * Display is always exactly 10 characters wide (see below)
 *
 * This function was a major bottleneck when resting, so a lot of
 * the text formatting code was optimized in place below.
 */
static void prt_state(creature_type *cr_ptr)
{
	byte attr = TERM_WHITE;

	char text[5];

	/* Repeating */
	if (command_rep)
	{
		if (command_rep > 999)
		{
#ifdef JP
sprintf(text, "%2d00", command_rep / 100);
#else
			(void)sprintf(text, "%2d00", command_rep / 100);
#endif

		}
		else
		{
#ifdef JP
sprintf(text, "  %2d", command_rep);
#else
			(void)sprintf(text, "  %2d", command_rep);
#endif

		}
	}

	/* Action */
	else
	{
		switch(cr_ptr->action)
		{
			case ACTION_SEARCH:
			{
#ifdef JP
				strcpy(text, "探索");
#else
				strcpy(text, "Sear");
#endif
				break;
			}
			case ACTION_REST:
			{
				int i;

				/* Start with "Rest" */
#ifdef JP
				strcpy(text, "    ");
#else
				strcpy(text, "    ");
#endif


				/* Extensive (timed) rest */
				if (resting >= 1000)
				{
					i = resting / 100;
					text[3] = '0';
					text[2] = '0';
					text[1] = '0' + (i % 10);
					text[0] = '0' + (i / 10);
				}

				/* Long (timed) rest */
				else if (resting >= 100)
				{
					i = resting;
					text[3] = '0' + (i % 10);
					i = i / 10;
					text[2] = '0' + (i % 10);
					text[1] = '0' + (i / 10);
				}

				/* Medium (timed) rest */
				else if (resting >= 10)
				{
					i = resting;
					text[3] = '0' + (i % 10);
					text[2] = '0' + (i / 10);
				}

				/* Short (timed) rest */
				else if (resting > 0)
				{
					i = resting;
					text[3] = '0' + (i);
				}

				/* Rest until healed */
				else if (resting == -1)
				{
					text[0] = text[1] = text[2] = text[3] = '*';
				}

				/* Rest until done */
				else if (resting == -2)
				{
					text[0] = text[1] = text[2] = text[3] = '&';
				}
				break;
			}
			case ACTION_LEARN:
			{
#ifdef JP
				strcpy(text, "学習");
#else
				strcpy(text, "lear");
#endif
				if (new_mane) attr = TERM_L_RED;
				break;
			}
			case ACTION_FISH:
			{
#ifdef JP
				strcpy(text, "釣り");
#else
				strcpy(text, "fish");
#endif
				break;
			}
			case ACTION_KAMAE:
			{
				int i;
				for (i = 0; i < MAX_KAMAE; i++)
					if (cr_ptr->special_defense & (KAMAE_GENBU << i)) break;
				switch (i)
				{
					case 0: attr = TERM_GREEN;break;
					case 1: attr = TERM_WHITE;break;
					case 2: attr = TERM_L_BLUE;break;
					case 3: attr = TERM_L_RED;break;
				}
				strcpy(text, kamae_shurui[i].desc);
				break;
			}
			case ACTION_KATA:
			{
				int i;
				for (i = 0; i < MAX_KATA; i++)
					if (cr_ptr->special_defense & (KATA_IAI << i)) break;
				strcpy(text, kata_shurui[i].desc);
				break;
			}
			case ACTION_SING:
			{
#ifdef JP
				strcpy(text, "歌  ");
#else
				strcpy(text, "Sing");
#endif
				break;
			}
			case ACTION_HAYAGAKE:
			{
#ifdef JP
				strcpy(text, "速駆");
#else
				strcpy(text, "Fast");
#endif
				break;
			}
			case ACTION_SPELL:
			{
#ifdef JP
				strcpy(text, "詠唱");
#else
				strcpy(text, "Spel");
#endif
				break;
			}
			default:
			{
				strcpy(text, "    ");
				break;
			}
		}
	}

	/* Display the info (or blanks) */
	c_put_str(attr, format("%5.5s",text), ROW_STATE, COL_STATE);
}


/*
 * Prints the speed of a character.			-CJS-
 */
static void prt_speed(creature_type *cr_ptr)
{
	int i = cr_ptr->speed;
	bool is_fast = IS_FAST(cr_ptr);

	byte attr = TERM_WHITE;
	char buf[32] = "";
	int wid, hgt, row_speed, col_speed;

	Term_get_size(&wid, &hgt);
	col_speed = wid + COL_SPEED;
	row_speed = hgt + ROW_SPEED;

	/* Hack -- Visually "undo" the Search Mode Slowdown */
	if (cr_ptr->action == ACTION_SEARCH && !cr_ptr->lightspeed) i += 10;

	/* Fast */
	if (i > 110)
	{
		if (cr_ptr->riding)
		{
			creature_type *m_ptr = &creature_list[cr_ptr->riding];
			if (m_ptr->fast && !m_ptr->slow) attr = TERM_L_BLUE;
			else if (m_ptr->slow && !m_ptr->fast) attr = TERM_VIOLET;
			else attr = TERM_GREEN;
		}
		else if ((is_fast && !cr_ptr->slow) || cr_ptr->lightspeed) attr = TERM_YELLOW;
		else if (cr_ptr->slow && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_GREEN;
#ifdef JP
		sprintf(buf, "%s(+%d)", (cr_ptr->riding ? "乗馬" : "加速"), (i - 110));
#else
		sprintf(buf, "Fast(+%d)", (i - 110));
#endif

	}

	/* Slow */
	else if (i < 110)
	{
		if (cr_ptr->riding)
		{
			creature_type *m_ptr = &creature_list[cr_ptr->riding];
			if (m_ptr->fast && !m_ptr->slow) attr = TERM_L_BLUE;
			else if (m_ptr->slow && !m_ptr->fast) attr = TERM_VIOLET;
			else attr = TERM_RED;
		}
		else if (is_fast && !cr_ptr->slow) attr = TERM_YELLOW;
		else if (cr_ptr->slow && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_UMBER;
#ifdef JP
		sprintf(buf, "%s(-%d)", (cr_ptr->riding ? "乗馬" : "減速"), (110 - i));
#else
		sprintf(buf, "Slow(-%d)", (110 - i));
#endif
	}
	else if (cr_ptr->riding)
	{
		attr = TERM_GREEN;
#ifdef JP
		strcpy(buf, "乗馬中");
#else
		strcpy(buf, "Riding");
#endif
	}

	/* Display the speed */
	c_put_str(attr, format("%-9s", buf), row_speed, col_speed);
}


static void prt_study(creature_type *cr_ptr)
{
	int wid, hgt, row_study, col_study;

	Term_get_size(&wid, &hgt);
	col_study = wid + COL_STUDY;
	row_study = hgt + ROW_STUDY;

	if (cr_ptr->new_spells)
	{
#ifdef JP
		put_str("学習", row_study, col_study);
#else
		put_str("Stud", row_study, col_study);
#endif

	}
	else
	{
		put_str("    ", row_study, col_study);
	}
}


static void prt_imitation(creature_type *cr_ptr)
{
	int wid, hgt, row_study, col_study;

	Term_get_size(&wid, &hgt);
	col_study = wid + COL_STUDY;
	row_study = hgt + ROW_STUDY;

	if (cr_ptr->cls_idx == CLASS_IMITATOR)
	{
		if (cr_ptr->mane_num)
		{
			byte attr;
			if (new_mane) attr = TERM_L_RED;
			else attr = TERM_WHITE;
#ifdef JP
			c_put_str(attr, "まね", row_study, col_study);
#else
			c_put_str(attr, "Imit", row_study, col_study);
#endif
		}
		else
		{
			put_str("    ", row_study, col_study);
		}
	}
}


static void prt_cut(creature_type *cr_ptr)
{
	int c = cr_ptr->cut;

	if (c > 1000)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "致命傷      ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_L_RED, "Mortal wound", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 200)
	{
#ifdef JP
		c_put_str(TERM_RED, "ひどい深手  ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Deep gash   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "重傷        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Severe cut  ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "大変な傷    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Nasty cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 25)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "ひどい傷    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Bad cut     ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 10)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "軽傷        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Light cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "かすり傷    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Graze       ", ROW_CUT, COL_CUT);
#endif

	}
	else
	{
		put_str("            ", ROW_CUT, COL_CUT);
	}
}



static void prt_stun(creature_type *cr_ptr)
{
	int s = cr_ptr->stun;

	if (s > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "意識不明瞭  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_RED, "Knocked out ", ROW_STUN, COL_STUN);
#endif

	}
	else if (s > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "ひどく朦朧  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Heavy stun  ", ROW_STUN, COL_STUN);
#endif

	}
	else if (s)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "朦朧        ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Stun        ", ROW_STUN, COL_STUN);
#endif

	}
	else
	{
		put_str("            ", ROW_STUN, COL_STUN);
	}
}



/*
 * Redraw the "monster health bar"	-DRS-
 * Rather extensive modifications by	-BEN-
 *
 * The "monster health bar" provides visual feedback on the "health"
 * of the monster currently being "tracked".  There are several ways
 * to "track" a monster, including targetting it, attacking it, and
 * affecting it (and nobody else) with a ranged attack.
 *
 * Display the monster health bar (affectionately known as the
 * "health-o-meter").  Clear health bar if nothing is being tracked.
 * Auto-track current target monster when bored.  Note that the
 * health-bar stops tracking any monster that "disappears".
 */
static void health_redraw(creature_type *cr_ptr, bool riding)
{
	s16b health_who;
	int row, col;
	creature_type *m_ptr;

	if (riding)
	{
		health_who = cr_ptr->riding;
		row = ROW_RIDING_INFO;
		col = COL_RIDING_INFO;
	}
	else
	{
		health_who = cr_ptr->health_who;
		row = ROW_INFO;
		col = COL_INFO;
	}

	m_ptr = &creature_list[health_who];

	/* Not tracking */
	if (!health_who)
	{
		/* Erase the health bar */
		Term_erase(col, row, 12);
	}

	/* Tracking an unseen monster */
	else if (!m_ptr->ml)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a hallucinatory monster */
	else if (cr_ptr->image)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a dead monster (???) */
	else if (m_ptr->chp < 0)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a visible monster */
	else
	{
		/* Extract the "percent" of health */
		int pct = 100L * m_ptr->chp / m_ptr->mhp;
		int pct2 = 100L * m_ptr->chp / m_ptr->mmhp;

		/* Convert percent into "health" */
		int len = (pct2 < 10) ? 1 : (pct2 < 90) ? (pct2 / 10 + 1) : 10;

		/* Default to almost dead */
		byte attr = TERM_RED;

		/* Invulnerable */
		if (m_ptr->invuln) attr = TERM_WHITE;

		/* Asleep */
		else if (m_ptr->paralyzed) attr = TERM_BLUE;

		/* Afraid */
		else if (m_ptr->afraid) attr = TERM_VIOLET;

		/* Healthy */
		else if (pct >= 100) attr = TERM_L_GREEN;

		/* Somewhat Wounded */
		else if (pct >= 60) attr = TERM_YELLOW;

		/* Wounded */
		else if (pct >= 25) attr = TERM_ORANGE;

		/* Badly wounded */
		else if (pct >= 10) attr = TERM_L_RED;

		/* Default to "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");

		/* Dump the current "health" (use '*' symbols) */
		Term_putstr(col + 1, row, len, attr, "**********");
	}
}



/*
 * Display basic info (mostly left of map)
 */
static void prt_frame_basic(creature_type *cr_ptr)
{
	int i;

	/* Level/Experience */
	prt_level(cr_ptr);
	prt_exp(cr_ptr);

	/* All Stats */
	for (i = 0; i < 6; i++) prt_stat(cr_ptr, i);

	/* Armor */
	prt_ac(cr_ptr);

	/* Hitpoints */
	prt_hp(cr_ptr);

	/* Spellpoints */
	prt_sp(cr_ptr);

	/* Gold */
	prt_gold(cr_ptr);

	/* Current depth */
	prt_depth(cr_ptr);

	/* Special */
	health_redraw(cr_ptr, FALSE);
	health_redraw(cr_ptr, TRUE);
}


/*
 * Display extra info (mostly below map)
 */
static void prt_frame_extra(creature_type *cr_ptr)
{
	/* Cut/Stun */
	prt_cut(cr_ptr);
	prt_stun(cr_ptr);

	/* Food */
	prt_hunger(cr_ptr);

	/* State */
	prt_state(cr_ptr);

	/* Speed */
	prt_speed(cr_ptr);

	/* Study spells */
	prt_study(cr_ptr);

	prt_imitation(cr_ptr);

	prt_status(cr_ptr);
}


/*
 * Hack -- display inventory in sub-windows
 */
static void fix_inven(creature_type *cr_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_INVEN))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display inventory */
		display_inven(cr_ptr);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}



/*
 * Hack -- display equipment in sub-windows
 */
static void fix_equip(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_EQUIP))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display equipment */
		display_equip(p_ptr);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display equipment in sub-windows
 */
static void fix_spell(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_SPELL))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display spell list */
		display_spell_list(p_ptr);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display character in sub-windows
 */
static void fix_player(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_PLAYER))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		update_playtime();

		/* Display player */
		display_player(0, p_ptr);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}



/*
 * Hack -- display recent messages in sub-windows
 *
 * XXX XXX XXX Adjust for width and split messages
 */
static void fix_message(void)
{
	int j, i;
	int w, h;
	int x, y;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MESSAGE))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Get size */
		Term_get_size(&w, &h);

		/* Dump messages */
		for (i = 0; i < h; i++)
		{
			/* Dump the message on the appropriate line */
			Term_putstr(0, (h - 1) - i, -1, (byte)((i < now_message) ? TERM_WHITE : TERM_SLATE), message_str((s16b)i));

			/* Cursor */
			Term_locate(&x, &y);

			/* Clear to end of line */
			Term_erase(x, y, 255);
		}

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display overhead view in sub-windows
 *
 * Note that the "player" symbol does NOT appear on the map.
 */
static void fix_overhead(void)
{
	int j;

	int cy, cx;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;
		int wid, hgt;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_OVERHEAD))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Full map in too small window is useless  */
		Term_get_size(&wid, &hgt);
		if (wid > COL_MAP + 2 && hgt > ROW_MAP + 2)
		{
			/* Redraw map */
			display_map(p_ptr, &cy, &cx);

			/* Fresh */
			Term_fresh();
		}

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display dungeon view in sub-windows
 */
static void fix_dungeon(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_DUNGEON))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw dungeon view */
		display_dungeon(p_ptr);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display monster recall in sub-windows
 */
static void fix_monster(creature_type *cr_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MONSTER))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display monster race info */
		if (cr_ptr->species_type_idx) display_roff(cr_ptr->species_type_idx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display object recall in sub-windows
 */
static void fix_object(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_OBJECT))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display monster race info */
		if (p_ptr->object_kind_idx) display_koff(p_ptr->object_kind_idx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Calculate number of spells player should have, and forget,
 * or remember, spells until that number is properly reflected.
 *
 * Note that this function induces various "status" messages,
 * which must be bypasses until the character is created.
 */
static void calc_spells(creature_type *cr_ptr, bool message)
{
	int			i, j, k, levels;
	int			num_allowed;
	int                     num_boukyaku = 0;

	magic_type		*s_ptr;
	int which;
	int bonus = 0;


	cptr p;

	/* Hack -- must be literate */
	if (!m_info[cr_ptr->realm1].spell_book) return;

	/* Hack -- wait for creation */
	if (!character_generated) return;

	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	if ((cr_ptr->cls_idx == CLASS_SORCERER) || (cr_ptr->cls_idx == CLASS_RED_MAGE))
	{
		cr_ptr->new_spells = 0;
		return;
	}

	p = spell_category_name(m_info[cr_ptr->realm1].spell_book);

	/* Determine the number of spells allowed */
	levels = cr_ptr->lev - m_info[cr_ptr->realm1].spell_first + 1;

	/* Hack -- no negative spells */
	if (levels < 0) levels = 0;

	/* Extract total allowed spells */
	num_allowed = (adj_mag_study[cr_ptr->stat_ind[m_info[cr_ptr->realm1].spell_stat]] * levels / 2);

	if ((cr_ptr->cls_idx != CLASS_SAMURAI) && (m_info[cr_ptr->realm1].spell_book != TV_LIFE_BOOK))
	{
		bonus = 4;
	}
	if (cr_ptr->cls_idx == CLASS_SAMURAI)
	{
		num_allowed = 32;
	}
	else if (cr_ptr->realm2 == REALM_NONE)
	{
		num_allowed = (num_allowed+1)/2;
		if (num_allowed>(32+bonus)) num_allowed = 32+bonus;
	}
	else if ((cr_ptr->cls_idx == CLASS_MAGE) || (cr_ptr->cls_idx == CLASS_PRIEST))
	{
		if (num_allowed>(96+bonus)) num_allowed = 96+bonus;
	}
	else
	{
		if (num_allowed>(80+bonus)) num_allowed = 80+bonus;
	}

	/* Count the number of spells we know */
	for (j = 0; j < 64; j++)
	{
		/* Count known spells */
		if ((j < 32) ?
		    (cr_ptr->spell_forgotten1 & (1L << j)) :
		    (cr_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			num_boukyaku++;
		}
	}

	/* See how many spells we must forget or may learn */
	cr_ptr->new_spells = num_allowed + cr_ptr->add_spells + num_boukyaku - cr_ptr->learned_spells;

	/* Forget spells which are too hard */
	for (i = 63; i >= 0; i--)
	{
		/* Efficiency -- all done */
		if (!cr_ptr->spell_learned1 && !cr_ptr->spell_learned2) break;

		/* Access the spell */
		j = cr_ptr->spell_order[i];

		/* Skip non-spells */
		if (j >= 99) continue;


		/* Get the spell */
		if (!is_magic((j < 32) ? cr_ptr->realm1 : cr_ptr->realm2))
		{
			if (j < 32)
				s_ptr = &technic_info[cr_ptr->realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[cr_ptr->realm2 - MIN_TECHNIC][j%32];
		}
		else if (j < 32)
			s_ptr = &m_info[cr_ptr->realm1].info[cr_ptr->realm1-1][j];
		else
			s_ptr = &m_info[cr_ptr->realm1].info[cr_ptr->realm2-1][j%32];

		/* Skip spells we are allowed to know */
		if (s_ptr->slevel <= cr_ptr->lev) continue;

		/* Is it known? */
		if ((j < 32) ?
		    (cr_ptr->spell_learned1 & (1L << j)) :
		    (cr_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				cr_ptr->spell_forgotten1 |= (1L << j);
				which = cr_ptr->realm1;
			}
			else
			{
				cr_ptr->spell_forgotten2 |= (1L << (j - 32));
				which = cr_ptr->realm2;
			}

			/* No longer known */
			if (j < 32)
			{
				cr_ptr->spell_learned1 &= ~(1L << j);
				which = cr_ptr->realm1;
			}
			else
			{
				cr_ptr->spell_learned2 &= ~(1L << (j - 32));
				which = cr_ptr->realm2;
			}

			/* Message */
#ifdef JP
			if(message) msg_format("%sの%sを忘れてしまった。",
				   do_spell(cr_ptr, which, j%32, SPELL_NAME), p );
#else
			if(message) msg_format("You have forgotten the %s of %s.", p,
					do_spell(cr_ptr, which, j%32, SPELL_NAME));
#endif


			/* One more can be learned */
			cr_ptr->new_spells++;
		}
	}


	/* Forget spells if we know too many spells */
	for (i = 63; i >= 0; i--)
	{
		/* Stop when possible */
		if (cr_ptr->new_spells >= 0) break;

		/* Efficiency -- all done */
		if (!cr_ptr->spell_learned1 && !cr_ptr->spell_learned2) break;

		/* Get the (i+1)th spell learned */
		j = cr_ptr->spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) continue;

		/* Forget it (if learned) */
		if ((j < 32) ?
		    (cr_ptr->spell_learned1 & (1L << j)) :
		    (cr_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				cr_ptr->spell_forgotten1 |= (1L << j);
				which = cr_ptr->realm1;
			}
			else
			{
				cr_ptr->spell_forgotten2 |= (1L << (j - 32));
				which = cr_ptr->realm2;
			}

			/* No longer known */
			if (j < 32)
			{
				cr_ptr->spell_learned1 &= ~(1L << j);
				which = cr_ptr->realm1;
			}
			else
			{
				cr_ptr->spell_learned2 &= ~(1L << (j - 32));
				which = cr_ptr->realm2;
			}

			/* Message */
#ifdef JP
			if(message) msg_format("%sの%sを忘れてしまった。",
				   do_spell(cr_ptr, which, j%32, SPELL_NAME), p );
#else
			if(message) msg_format("You have forgotten the %s of %s.", p,
				   do_spell(cr_ptr, which, j%32, SPELL_NAME));
#endif


			/* One more can be learned */
			cr_ptr->new_spells++;
		}
	}


	/* Check for spells to remember */
	for (i = 0; i < 64; i++)
	{
		/* None left to remember */
		if (cr_ptr->new_spells <= 0) break;

		/* Efficiency -- all done */
		if (!cr_ptr->spell_forgotten1 && !cr_ptr->spell_forgotten2) break;

		/* Get the next spell we learned */
		j = cr_ptr->spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) break;

		/* Access the spell */
		if (!is_magic((j < 32) ? cr_ptr->realm1 : cr_ptr->realm2))
		{
			if (j < 32)
				s_ptr = &technic_info[cr_ptr->realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[cr_ptr->realm2 - MIN_TECHNIC][j%32];
		}
		else if (j<32)
			s_ptr = &m_info[cr_ptr->realm1].info[cr_ptr->realm1-1][j];
		else
			s_ptr = &m_info[cr_ptr->realm1].info[cr_ptr->realm2-1][j%32];

		/* Skip spells we cannot remember */
		if (s_ptr->slevel > cr_ptr->lev) continue;

		/* First set of spells */
		if ((j < 32) ?
		    (cr_ptr->spell_forgotten1 & (1L << j)) :
		    (cr_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			/* No longer forgotten */
			if (j < 32)
			{
				cr_ptr->spell_forgotten1 &= ~(1L << j);
				which = cr_ptr->realm1;
			}
			else
			{
				cr_ptr->spell_forgotten2 &= ~(1L << (j - 32));
				which = cr_ptr->realm2;
			}

			/* Known once more */
			if (j < 32)
			{
				cr_ptr->spell_learned1 |= (1L << j);
				which = cr_ptr->realm1;
			}
			else
			{
				cr_ptr->spell_learned2 |= (1L << (j - 32));
				which = cr_ptr->realm2;
			}

			/* Message */
#ifdef JP
			if(message) msg_format("%sの%sを思い出した。",
				   do_spell(cr_ptr, which, j%32, SPELL_NAME), p );
#else
			if(message) msg_format("You have remembered the %s of %s.",
				   p, do_spell(cr_ptr, which, j%32, SPELL_NAME));
#endif


			/* One less can be learned */
			cr_ptr->new_spells--;
		}
	}

	k = 0;

	if (cr_ptr->realm2 == REALM_NONE)
	{
		/* Count spells that can be learned */
		for (j = 0; j < 32; j++)
		{
			if (!is_magic(cr_ptr->realm1)) s_ptr = &technic_info[cr_ptr->realm1-MIN_TECHNIC][j];
			else s_ptr = &m_info[cr_ptr->realm1].info[cr_ptr->realm1-1][j];

			/* Skip spells we cannot remember */
			if (s_ptr->slevel > cr_ptr->lev) continue;

			/* Skip spells we already know */
			if (cr_ptr->spell_learned1 & (1L << j))
			{
				continue;
			}

			/* Count it */
			k++;
		}
		if (k>32) k = 32;
		if ((cr_ptr->new_spells > k) && ((m_info[cr_ptr->realm1].spell_book == TV_LIFE_BOOK) || (m_info[cr_ptr->realm1].spell_book == TV_HISSATSU_BOOK))) cr_ptr->new_spells = k;
	}

	if (cr_ptr->new_spells < 0) cr_ptr->new_spells = 0;

	/* Spell count changed */
	if (cr_ptr->old_spells != cr_ptr->new_spells)
	{
		/* Message if needed */
		if (cr_ptr->new_spells)
		{
			/* Message */
#ifdef JP
			if( cr_ptr->new_spells < 10 ){
				if(message) msg_format("あと %d つの%sを学べる。", cr_ptr->new_spells, p);
			}else{
				if(message) msg_format("あと %d 個の%sを学べる。", cr_ptr->new_spells, p);
			}
#else
			if(message) msg_format("You can learn %d more %s%s.",
				   cr_ptr->new_spells, p,
				   (cr_ptr->new_spells != 1) ? "s" : "");
#endif

		}

		/* Save the new_spells value */
		cr_ptr->old_spells = cr_ptr->new_spells;

		/* Redraw Study Status */
		play_redraw |= (PR_STUDY);

		/* Redraw object recall */
		play_window |= (PW_OBJECT);
	}
}


/*
 * Calculate maximum mana.  You do not need to know any spells.
 * Note that mana is lowered by heavy (or inappropriate) armor.
 *
 * This function induces status messages.
 */
static void calc_mana(creature_type *cr_ptr, bool message)
{
	int		msp, levels, cur_wgt, max_wgt;

	object_type	*o_ptr;

	levels = cr_ptr->lev;

	switch(cr_ptr->cls_idx)
	{
		case CLASS_SAMURAI:
			msp = (adj_mag_mana[cr_ptr->stat_ind[m_info[cr_ptr->realm1].spell_stat]] + 10) * 2;
			if (msp) msp += (msp * race_info[cr_ptr->race_idx1].r_adj[m_info[cr_ptr->realm1].spell_stat] / 20);
			break;

		default:
			/* Extract total mana */
			msp = adj_mag_mana[cr_ptr->stat_ind[STAT_INT]] * (levels+3) / 4;

			/* Hack -- usually add one mana */
			msp++;

			if (msp) msp += (msp * race_info[cr_ptr->race_idx1].r_adj[m_info[cr_ptr->realm1].spell_stat] / 20);

			if (msp && (cr_ptr->chara_idx == CHARA_MUNCHKIN)) msp += msp / 2;

			/* Hack: High mages have a 25% mana bonus */
			if (msp && (cr_ptr->cls_idx == CLASS_MAGE)) msp += msp / 6;
			if (msp && (cr_ptr->cls_idx == CLASS_HIGH_MAGE)) msp += msp / 4;
	}

	/* Only mages are affected */
	if (m_info[cr_ptr->realm1].spell_xtra & MAGIC_GLOVE_REDUCE_MANA)
	{
		u32b flgs[TR_FLAG_SIZE];

		/* Assume player is not encumbered by gloves */
		cr_ptr->cumber_glove = FALSE;

		/* Get the gloves */
		o_ptr = &cr_ptr->inventory[INVEN_1STHANDS];

		/* Examine the gloves */
		object_flags(o_ptr, flgs);

		/* Normal gloves hurt mage-type spells */
		if (o_ptr->k_idx &&
		    !(have_flag(flgs, TR_FREE_ACT)) &&
		    !(have_flag(flgs, TR_MAGIC_MASTERY)) &&
		    !((have_flag(flgs, TR_DEX)) && (o_ptr->pval > 0)))
		{
			/* Encumbered */
			cr_ptr->cumber_glove = TRUE;

			/* Reduce mana */
			msp = (3 * msp) / 4;
		}
	}


	/* Assume player not encumbered by armor */
	cr_ptr->cumber_armor = FALSE;

	/* Weigh the armor */
	cur_wgt = 0;
	if(cr_ptr->inventory[INVEN_1STARM].tval> TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_1STARM].weight;
	if(cr_ptr->inventory[INVEN_2NDARM].tval> TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_2NDARM].weight;
	cur_wgt += cr_ptr->inventory[INVEN_BODY].weight;
	cur_wgt += cr_ptr->inventory[INVEN_1STHEAD].weight;
	cur_wgt += cr_ptr->inventory[INVEN_OUTER].weight;
	cur_wgt += cr_ptr->inventory[INVEN_1STHANDS].weight;
	cur_wgt += cr_ptr->inventory[INVEN_FEET].weight;

	/* Subtract a percentage of maximum mana. */
	switch (cr_ptr->cls_idx)
	{
		/* For these classes, mana is halved if armour 
		 * is 30 pounds over their weight limit. */
		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_BLUE_MAGE:
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		case CLASS_SORCERER:
		{
			if (cr_ptr->inventory[INVEN_1STARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_1STARM].weight;
			if (cr_ptr->inventory[INVEN_2NDARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_2NDARM].weight;
			break;
		}

		/* Mana halved if armour is 40 pounds over weight limit. */
		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_TOURIST:
		{
			if (cr_ptr->inventory[INVEN_1STARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_1STARM].weight*2/3;
			if (cr_ptr->inventory[INVEN_2NDARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_2NDARM].weight*2/3;
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_BEASTMASTER:
		case CLASS_MIRROR_MASTER:
		{
			if (cr_ptr->inventory[INVEN_1STARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_1STARM].weight/2;
			if (cr_ptr->inventory[INVEN_2NDARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_2NDARM].weight/2;
			break;
		}

		/* Mana halved if armour is 50 pounds over weight limit. */
		case CLASS_ROGUE:
		case CLASS_RANGER:
		case CLASS_RED_MAGE:
		case CLASS_WARRIOR_MAGE:
		{
			if (cr_ptr->inventory[INVEN_1STARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_1STARM].weight/3;
			if (cr_ptr->inventory[INVEN_2NDARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_2NDARM].weight/3;
			break;
		}

		/* Mana halved if armour is 60 pounds over weight limit. */
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		{
			if (cr_ptr->inventory[INVEN_1STARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_1STARM].weight/5;
			if (cr_ptr->inventory[INVEN_2NDARM].tval <= TV_SWORD) cur_wgt += cr_ptr->inventory[INVEN_2NDARM].weight/5;
			break;
		}

		/* For new classes created, but not yet added to this formula. */
		default:
		{
			break;
		}
	}

	/* Determine the weight allowance */
	max_wgt = m_info[cr_ptr->realm1].spell_weight;

	/* Heavy armor penalizes mana by a percentage.  -LM- */
	if ((cur_wgt - max_wgt) > 0)
	{
		/* Encumbered */
		cr_ptr->cumber_armor = TRUE;

		/* Subtract a percentage of maximum mana. */
		switch (cr_ptr->cls_idx)
		{
			/* For these classes, mana is halved if armour 
			 * is 30 pounds over their weight limit. */
			case CLASS_MAGE:
			case CLASS_HIGH_MAGE:
			case CLASS_BLUE_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 600;
				break;
			}

			/* Mana halved if armour is 40 pounds over weight limit. */
			case CLASS_PRIEST:
			case CLASS_MINDCRAFTER:
			case CLASS_BEASTMASTER:
			case CLASS_BARD:
			case CLASS_FORCETRAINER:
			case CLASS_TOURIST:
			case CLASS_MIRROR_MASTER:
			{
				msp -= msp * (cur_wgt - max_wgt) / 800;
				break;
			}

			case CLASS_SORCERER:
			{
				msp -= msp * (cur_wgt - max_wgt) / 900;
				break;
			}

			/* Mana halved if armour is 50 pounds over weight limit. */
			case CLASS_ROGUE:
			case CLASS_RANGER:
			case CLASS_MONK:
			case CLASS_RED_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 1000;
				break;
			}

			/* Mana halved if armour is 60 pounds over weight limit. */
			case CLASS_PALADIN:
			case CLASS_CHAOS_WARRIOR:
			case CLASS_WARRIOR_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 1200;
				break;
			}

			case CLASS_SAMURAI:
			{
				cr_ptr->cumber_armor = FALSE;
				break;
			}

			/* For new classes created, but not yet added to this formula. */
			default:
			{
				msp -= msp * (cur_wgt - max_wgt) / 800;
				break;
			}
		}
	}

	/* Mana can never be negative */
	if (msp < 0) msp = 0;


	/* Maximum mana has changed */
	if (cr_ptr->msp != msp)
	{
		/* Enforce maximum */
		if ((cr_ptr->csp >= msp) && (cr_ptr->cls_idx != CLASS_SAMURAI))
		{
			cr_ptr->csp = msp;
			cr_ptr->csp_frac = 0;
		}

#ifdef JP
		/* レベルアップの時は上昇量を表示する */
		if ((level_up == 1) && (msp > cr_ptr->msp))
		{
			if(message) msg_format("最大マジック・ポイントが %d 増加した！",
				   (msp - cr_ptr->msp));
		}
#endif
		/* Save new mana */
		cr_ptr->msp = msp;

		/* Display mana later */
		play_redraw |= (PR_MANA);

		/* Window stuff */
		play_window |= (PW_PLAYER);
		play_window |= (PW_SPELL);
	}


	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	/* Take note when "glove state" changes */
	if (cr_ptr->old_cumber_glove != cr_ptr->cumber_glove)
	{
		/* Message */
		if (cr_ptr->cumber_glove)
		{
#ifdef JP
			if(message) msg_print("手が覆われて呪文が唱えにくい感じがする。");
#else
			if(message) msg_print("Your covered hands feel unsuitable for spellcasting.");
#endif

		}
		else
		{
#ifdef JP
			if(message) msg_print("この手の状態なら、ぐっと呪文が唱えやすい感じだ。");
#else
			if(message) msg_print("Your hands feel more suitable for spellcasting.");
#endif

		}

		/* Save it */
		cr_ptr->old_cumber_glove = cr_ptr->cumber_glove;
	}


	/* Take note when "armor state" changes */
	if (cr_ptr->old_cumber_armor != cr_ptr->cumber_armor)
	{
		/* Message */
		if (cr_ptr->cumber_armor)
		{
#ifdef JP
			if(message) msg_print("装備の重さで動きが鈍くなってしまっている。");
#else
			if(message) msg_print("The weight of your equipment encumbers your movement.");
#endif

		}
		else
		{
#ifdef JP
			if(message) msg_print("ぐっと楽に体を動かせるようになった。");
#else
			if(message) msg_print("You feel able to move more freely.");
#endif

		}

		/* Save it */
		cr_ptr->old_cumber_armor = cr_ptr->cumber_armor;
	}
}



/*
 * Calculate the players (maximal) hit points
 * Adjust current hitpoints if necessary
 */
static void calc_hitpoints(creature_type *cr_ptr, bool message)
{
	int bonus, mhp;
	byte tmp_hitdice;

	/* Un-inflate "half-hitpoint bonus per level" value */
	bonus = ((int)(adj_con_mhp[cr_ptr->stat_ind[STAT_CON]]) - 128) * cr_ptr->lev / 4;

	/* Divine Bonuses */
	if(cr_ptr->dr >= 0){
		bonus += adj_dr_mhp[cr_ptr->dr] * cr_ptr->hitdice;
	}

	/* Calculate hitpoints */
	mhp = cr_ptr->player_hp[cr_ptr->lev - 1];

	if (cr_ptr->mimic_form)
	{
		if (cr_ptr->cls_idx == CLASS_SORCERER) tmp_hitdice = mimic_info[cr_ptr->mimic_form].r_mhp/2;
		else tmp_hitdice = (byte)mimic_info[cr_ptr->mimic_form].r_mhp;

		if (cr_ptr->cls_idx != CLASS_NONE) tmp_hitdice += class_info[cr_ptr->cls_idx].c_mhp;
		if (cr_ptr->chara_idx != CHARA_NONE) tmp_hitdice += chara_info[cr_ptr->chara_idx].a_mhp;

		mhp = mhp * tmp_hitdice / cr_ptr->hitdice;
	}

	if (cr_ptr->cls_idx == CLASS_SORCERER)
	{
		if (cr_ptr->lev < 30)
			mhp = (mhp * (45+cr_ptr->lev) / 100);
		else
			mhp = (mhp * 75 / 100);
		bonus = (bonus * 65 / 100);
	}

	mhp += bonus;

	if (cr_ptr->cls_idx == CLASS_BERSERKER)
	{
		mhp = mhp * (110+(((cr_ptr->lev + 40) * (cr_ptr->lev + 40) - 1550) / 110))/100;
	}

	/* Always have at least one hitpoint per level */
	if (mhp < cr_ptr->lev + 1) mhp = cr_ptr->lev + 1;

	/* Factor in the hero / superhero settings */
	if (IS_HERO(cr_ptr)) mhp += 10;
	if (cr_ptr->shero && (cr_ptr->cls_idx != CLASS_BERSERKER)) mhp += 30;
	if (cr_ptr->tsuyoshi) mhp += 50;

	/* Factor in the hex spell settings */
	if (hex_spelling(cr_ptr, HEX_XTRA_MIGHT)) mhp += 15;
	if (hex_spelling(cr_ptr, HEX_BUILDING)) mhp += 60;

	/* New maximum hitpoints */
	if (cr_ptr->mhp != mhp)
	{
		/* Enforce maximum */
		if (cr_ptr->chp >= mhp)
		{
			cr_ptr->chp = mhp;
			cr_ptr->chp_frac = 0;
		}

#ifdef JP
		/* レベルアップの時は上昇量を表示する */
		if ((level_up == 1) && (mhp > cr_ptr->mhp))
		{
			if(message) msg_format("最大ヒット・ポイントが %d 増加した！", (mhp - cr_ptr->mhp) );
		}
#endif
		/* Save the new max-hitpoints */
		cr_ptr->mhp = mhp;
		cr_ptr->mmhp = mhp;

		/* Display hitpoints (later) */
		play_redraw |= (PR_HP);

		/* Window stuff */
		play_window |= (PW_PLAYER);
	}
}



/*
 * Extract and set the current "lite radius"
 *
 * SWD: Experimental modification: multiple light sources have additive effect.
 *
 */
static void calc_torch(creature_type *cr_ptr)
{
	int i;
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];

	/* Assume no light */
	cr_ptr->cur_lite = 0;

	/* Loop through all wielded items */
	for (i = INVEN_1STARM; i < INVEN_TOTAL; i++)
	{
		o_ptr = &cr_ptr->inventory[i];

		/* Examine actual lites */
		if ((i == INVEN_LITE) && (o_ptr->k_idx) && (o_ptr->tval == TV_LITE))
		{
			if (o_ptr->name2 == EGO_LITE_DARKNESS)
			{
				if (o_ptr->sval == SV_LITE_TORCH)
				{
					cr_ptr->cur_lite -= 2;
				}

				/* Lanterns (with fuel) provide more lite */
				else if (o_ptr->sval == SV_LITE_LANTERN)
				{
					cr_ptr->cur_lite -= 2;
				}

				else if (o_ptr->sval == SV_LITE_FEANOR)
				{
					cr_ptr->cur_lite -= 3;
				}
			}
			/* Torches (with fuel) provide some lite */
			else if ((o_ptr->sval == SV_LITE_TORCH) && (o_ptr->xtra4 > 0))
			{
				cr_ptr->cur_lite += 2;
			}

			/* Lanterns (with fuel) provide more lite */
			else if ((o_ptr->sval == SV_LITE_LANTERN) && (o_ptr->xtra4 > 0))
			{
				cr_ptr->cur_lite += 2;
			}

			else if (o_ptr->sval == SV_LITE_FEANOR)
			{
				cr_ptr->cur_lite += 3;
			}

			/* Artifact Lites provide permanent, bright, lite */
			else if (object_is_fixed_artifact(o_ptr))
			{
				cr_ptr->cur_lite += 4;
			}

			if (o_ptr->name2 == EGO_LITE_SHINE) cr_ptr->cur_lite++;
		}
		else
		{
			/* Skip empty slots */
			if (!o_ptr->k_idx) continue;

			/* Extract the flags */
			object_flags(o_ptr, flgs);

			/* does this item glow? */
			if (have_flag(flgs, TR_LITE))
			{
				if ((o_ptr->name2 == EGO_DARK) || (o_ptr->name1 == ART_NIGHT)) cr_ptr->cur_lite--;
				else cr_ptr->cur_lite++;
			}
		}

	}

	/* max radius is 14 (was 5) without rewriting other code -- */
	/* see cave.c:update_lite() and defines.h:LITE_MAX */
	if (d_info[dungeon_type].flags1 & DF1_DARKNESS && cr_ptr->cur_lite > 1)
		cr_ptr->cur_lite = 1;

	/*
	 * check if the player doesn't have light radius, 
	 * but does weakly glow as an intrinsic.
	 */
	if (cr_ptr->cur_lite <= 0 && cr_ptr->lite) cr_ptr->cur_lite++;

	if (cr_ptr->cur_lite > 14) cr_ptr->cur_lite = 14;
	if (cr_ptr->cur_lite < 0) cr_ptr->cur_lite = 0;

	/* end experimental mods */

	/* Notice changes in the "lite radius" */
	if (cr_ptr->old_lite != cr_ptr->cur_lite)
	{
		/* Update stuff */
		/* Hack -- PU_MON_LITE for monsters' darkness */
		cr_ptr->update |= (PU_LITE | PU_MON_LITE | PU_MONSTERS);

		/* Remember the old lite */
		cr_ptr->old_lite = cr_ptr->cur_lite;

		if ((cr_ptr->cur_lite > 0) && (cr_ptr->special_defense & NINJA_S_STEALTH))
			set_superstealth(cr_ptr, FALSE);
	}
}



/*
 * Computes current weight limit.
 */
u32b weight_limit(creature_type *cr_ptr)
{
	u32b i;

	/* Weight limit based only on strength */
	i = (u32b)adj_str_wgt[cr_ptr->stat_ind[STAT_STR]] * 50; /* Constant was 100 */
	if (cr_ptr->cls_idx == CLASS_BERSERKER) i = i * 3 / 2;

	/* Return the result */
	return i;
}


bool have_weapon(creature_type *cr_ptr, int i)
{
	return ((cr_ptr->inventory[i].k_idx && object_is_melee_weapon(cr_ptr, &cr_ptr->inventory[i])) ? TRUE : FALSE);
}


/*
 * Calculate the players current "state", taking into account
 * not only race/class intrinsics, but also objects being worn
 * and temporary spell effects.
 *
 * See also calc_mana() and calc_hitpoints().
 *
 * Take note of the new "speed code", in particular, a very strong
 * player will start slowing down as soon as he reaches 150 pounds,
 * but not until he reaches 450 pounds will he be half as fast as
 * a normal kobold.  This both hurts and helps the player, hurts
 * because in the old days a player could just avoid 300 pounds,
 * and helps because now carrying 300 pounds is not very painful.
 *
 * The "weapon" and "bow" do *not* add to the bonuses to hit or to
 * damage, since that would affect non-combat things.  These values
 * are actually added in later, at the appropriate place.
 *
 * This function induces various "status" messages.
 */
void calc_bonuses(creature_type *cr_ptr, bool message)
{
	int             i, j, k, hold, rate;
	int             new_speed;
	int             body_size;
	int             default_hand = 0;
	int             empty_hands_status = empty_hands(cr_ptr, TRUE);
	int             extra_blows[2];
	int             extra_shots;

	object_type     *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	bool            omoi = FALSE;
	bool            yoiyami = FALSE;
	bool            down_saving = FALSE;
#if 0
	bool            have_dd_s = FALSE, have_dd_t = FALSE;
#endif
	bool            have_sw = FALSE, have_kabe = FALSE;
	bool            easy_2weapon = FALSE;
	bool            riding_levitation = FALSE;
	s16b this_o_idx, next_o_idx = 0;

	race_type *tmp_rcr_ptr;
	race_type *tmp_rcr_ptr2;

	/* Save the old vision stuff */
	bool old_telepathy = cr_ptr->telepathy;
	bool old_esp_animal = cr_ptr->esp_animal;
	bool old_esp_undead = cr_ptr->esp_undead;
	bool old_esp_demon = cr_ptr->esp_demon;
	bool old_esp_orc = cr_ptr->esp_orc;
	bool old_esp_troll = cr_ptr->esp_troll;
	bool old_esp_giant = cr_ptr->esp_giant;
	bool old_esp_dragon = cr_ptr->esp_dragon;
	bool old_esp_human = cr_ptr->esp_human;
	bool old_esp_evil = cr_ptr->esp_evil;
	bool old_esp_good = cr_ptr->esp_good;
	bool old_esp_nonliving = cr_ptr->esp_nonliving;
	bool old_esp_unique = cr_ptr->esp_unique;
	bool old_see_inv = cr_ptr->see_inv;
	bool old_mighty_throw = cr_ptr->mighty_throw;

	/* Save the old armor class */
	bool old_dis_ac = (bool)cr_ptr->dis_ac;
	bool old_dis_to_a = (bool)cr_ptr->dis_to_a;


	/* Clear extra blows/shots */
	extra_blows[0] = extra_blows[1] = extra_shots = 0;

	/* Clear the stat modifiers */
	for (i = 0; i < 6; i++) cr_ptr->stat_add[i] = 0;

	/* Level Limit */
	cr_ptr->max_lev = 0;

	/* Clear the Displayed/Real armor class */
	cr_ptr->dis_ac = cr_ptr->ac = 0;

	/* Clear the Displayed/Real Bonuses */
	cr_ptr->dis_to_h[0] = cr_ptr->to_h[0] = 0;
	cr_ptr->dis_to_h[1] = cr_ptr->to_h[1] = 0;
	cr_ptr->dis_to_d[0] = cr_ptr->to_d[0] = 0;
	cr_ptr->dis_to_d[1] = cr_ptr->to_d[1] = 0;
	cr_ptr->dis_to_h_b = cr_ptr->to_h_b = 0;
	cr_ptr->dis_to_a = cr_ptr->to_a = 0;
	cr_ptr->to_h_m = 0;
	cr_ptr->to_d_m = 0;

	cr_ptr->to_m_chance = 0;

	/* Clear the Extra Dice Bonuses */
	cr_ptr->to_dd[0] = cr_ptr->to_ds[0] = 0;
	cr_ptr->to_dd[1] = cr_ptr->to_ds[1] = 0;

	/* Clear the Activate Rate */
	cr_ptr->to_ar[0] = cr_ptr->to_ar[0] = 100; 

	/* Start with "normal" speed */
	new_speed = 110;
	if(cr_ptr->dr >= 0) new_speed += adj_dr_speed[cr_ptr->dr];

	/* Plus AC on Divine Rank */
	if(cr_ptr->dr >= 0){
		cr_ptr->dis_to_a += adj_dr_ac[cr_ptr->dr];
		cr_ptr->to_a += adj_dr_ac[cr_ptr->dr];
	}

	cr_ptr->num_hand = 2;
	cr_ptr->num_head = 1;
	cr_ptr->num_tail = 0;
	cr_ptr->num_foot = 2;

	/* Start with a single blow per turn */
	cr_ptr->num_blow[0] = 1;
	cr_ptr->num_blow[1] = 1;

	/* Start with a single shot per turn */
	cr_ptr->num_fire = 100;

	/* Reset the "xtra" tval */
	cr_ptr->tval_xtra = 0;

	/* Reset the "ammo" tval */
	cr_ptr->tval_ammo = 0;

	/* Clear all the flags */
	cr_ptr->cursed = 0L;
	cr_ptr->bless_blade = FALSE;
	cr_ptr->xtra_might = FALSE;
	cr_ptr->impact[0] = FALSE;
	cr_ptr->impact[1] = FALSE;
	cr_ptr->pass_wall = FALSE;
	cr_ptr->kill_wall = FALSE;
	cr_ptr->dec_mana = FALSE;
	cr_ptr->easy_spell = FALSE;
	cr_ptr->heavy_spell = FALSE;
	cr_ptr->see_inv = FALSE;
	cr_ptr->free_act = FALSE;
	cr_ptr->slow_digest = FALSE;
	cr_ptr->regenerate = FALSE;
	cr_ptr->can_swim = FALSE;
	cr_ptr->levitation = FALSE;
	cr_ptr->hold_life = FALSE;
	cr_ptr->telepathy = FALSE;
	cr_ptr->esp_animal = FALSE;
	cr_ptr->esp_undead = FALSE;
	cr_ptr->esp_demon = FALSE;
	cr_ptr->esp_orc = FALSE;
	cr_ptr->esp_troll = FALSE;
	cr_ptr->esp_giant = FALSE;
	cr_ptr->esp_dragon = FALSE;
	cr_ptr->esp_human = FALSE;
	cr_ptr->esp_evil = FALSE;
	cr_ptr->esp_good = FALSE;
	cr_ptr->esp_nonliving = FALSE;
	cr_ptr->esp_unique = FALSE;
	cr_ptr->lite = FALSE;
	cr_ptr->sustain_str = FALSE;
	cr_ptr->sustain_int = FALSE;
	cr_ptr->sustain_wis = FALSE;
	cr_ptr->sustain_con = FALSE;
	cr_ptr->sustain_dex = FALSE;
	cr_ptr->sustain_chr = FALSE;
	cr_ptr->resist_acid = FALSE;
	cr_ptr->resist_elec = FALSE;
	cr_ptr->resist_fire = FALSE;
	cr_ptr->resist_cold = FALSE;
	cr_ptr->resist_pois = FALSE;
	cr_ptr->resist_conf = FALSE;
	cr_ptr->resist_sound = FALSE;
	cr_ptr->resist_lite = FALSE;
	cr_ptr->resist_dark = FALSE;
	cr_ptr->resist_chaos = FALSE;
	cr_ptr->resist_disen = FALSE;
	cr_ptr->resist_shard = FALSE;
	cr_ptr->resist_nexus = FALSE;
	cr_ptr->resist_blind = FALSE;
	cr_ptr->resist_neth = FALSE;
	cr_ptr->resist_time = FALSE;
	cr_ptr->resist_fear = FALSE;
	cr_ptr->reflect = FALSE;
	cr_ptr->sh_fire = FALSE;
	cr_ptr->sh_elec = FALSE;
	cr_ptr->sh_cold = FALSE;
	cr_ptr->anti_magic = FALSE;
	cr_ptr->anti_tele = FALSE;
	cr_ptr->warning = FALSE;
	cr_ptr->mighty_throw = FALSE;
	cr_ptr->see_nocto = FALSE;

	cr_ptr->immune_acid = FALSE;
	cr_ptr->immune_elec = FALSE;
	cr_ptr->immune_fire = FALSE;
	cr_ptr->immune_cold = FALSE;

	cr_ptr->ryoute = FALSE;
	cr_ptr->migite = FALSE;
	cr_ptr->hidarite = FALSE;
	cr_ptr->no_flowed = FALSE;

	if (cr_ptr->mimic_form) tmp_rcr_ptr = &mimic_info[cr_ptr->mimic_form];
	else tmp_rcr_ptr = &race_info[cr_ptr->race_idx1];
	tmp_rcr_ptr2 = &race_info[cr_ptr->race_idx2];

	/* Body Size */
	cr_ptr->size = body_size = calc_bodysize(cr_ptr->ht, cr_ptr->wt);

	/* Level Limit */
	/* ... EXPFACT has no relation at level limit ...
	for(i = 1; i < PY_MORTAL_LIMIT_LEVEL; i++)
		if(creature_exp[PY_MORTAL_LIMIT_LEVEL] < creature_exp[i + 1] * (cr_ptr->expfact - 50) / 100L)
			break;
	*/

	if (cr_ptr->dr >= 0)
		cr_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL + cr_ptr->dr;
	else
		cr_ptr->max_lev = PY_MORTAL_LIMIT_LEVEL;

	/* Base infravision (purely racial) */
	cr_ptr->see_infra = tmp_rcr_ptr->infra;
	j = 0;
	k = 1;


	/* Base skills */
	cr_ptr->skill_dis = 5;
	cr_ptr->skill_dev = 5;
	cr_ptr->skill_rob = 5 + (body_size - 10) / 2 * 3;
	cr_ptr->skill_eva = 5 - (body_size - 10) / 2 * 3;
	cr_ptr->skill_vol = 5;
	cr_ptr->skill_stl = 3 - body_size / 3;
	cr_ptr->skill_srh = 10;
	cr_ptr->skill_fos = 10;
	cr_ptr->skill_thn = body_size;
	cr_ptr->skill_thb = 10;
	cr_ptr->skill_tht = 10;
	cr_ptr->skill_dig = (body_size - 10) * 2;



	if(IS_PURE(cr_ptr))
	{
		cr_ptr->skill_dis += (-5 + race_info[cr_ptr->race_idx1].r_dis);
		cr_ptr->skill_dev += (-5 + race_info[cr_ptr->race_idx1].r_dev);
		cr_ptr->skill_rob += (-5 + race_info[cr_ptr->race_idx1].r_sav);
		cr_ptr->skill_eva += (-5 + race_info[cr_ptr->race_idx1].r_sav);
		cr_ptr->skill_vol += (-5 + race_info[cr_ptr->race_idx1].r_sav);
		cr_ptr->skill_stl += race_info[cr_ptr->race_idx1].r_stl;
		cr_ptr->skill_srh += (-10 + race_info[cr_ptr->race_idx1].r_srh);
		cr_ptr->skill_fos += (-10 + race_info[cr_ptr->race_idx1].r_fos);
		cr_ptr->skill_thn += (-10 + race_info[cr_ptr->race_idx1].r_thn);
		cr_ptr->skill_thb += (-10 + race_info[cr_ptr->race_idx1].r_thb);
		cr_ptr->skill_tht += (-10 + race_info[cr_ptr->race_idx1].r_thb);
	}
	else
	{
		cr_ptr->skill_dis += (-5 + race_info[cr_ptr->race_idx1].r_s_dis);
		cr_ptr->skill_dev += (-5 + race_info[cr_ptr->race_idx1].r_s_dev);
		cr_ptr->skill_rob += (-5 + race_info[cr_ptr->race_idx1].r_s_sav);
		cr_ptr->skill_eva += (-5 + race_info[cr_ptr->race_idx1].r_s_sav);
		cr_ptr->skill_vol += (-5 + race_info[cr_ptr->race_idx1].r_s_sav);
		cr_ptr->skill_stl += race_info[cr_ptr->race_idx1].r_s_stl;
		cr_ptr->skill_srh += (-10 + race_info[cr_ptr->race_idx1].r_s_srh);
		cr_ptr->skill_fos += (-10 + race_info[cr_ptr->race_idx1].r_s_fos);
		cr_ptr->skill_thn += (-10 + race_info[cr_ptr->race_idx1].r_s_thn);
		cr_ptr->skill_thb += (-10 + race_info[cr_ptr->race_idx1].r_s_thb);
		cr_ptr->skill_tht += (-10 + race_info[cr_ptr->race_idx1].r_s_thb);

		cr_ptr->skill_dis += race_info[cr_ptr->race_idx2].r_s_dis;
		cr_ptr->skill_dev += race_info[cr_ptr->race_idx2].r_s_dev;
		cr_ptr->skill_rob += race_info[cr_ptr->race_idx2].r_s_sav;
		cr_ptr->skill_eva += race_info[cr_ptr->race_idx2].r_s_sav;
		cr_ptr->skill_vol += race_info[cr_ptr->race_idx2].r_s_sav;
		cr_ptr->skill_stl += race_info[cr_ptr->race_idx2].r_s_stl;
		cr_ptr->skill_srh += race_info[cr_ptr->race_idx2].r_s_srh;
		cr_ptr->skill_fos += race_info[cr_ptr->race_idx2].r_s_fos;
		cr_ptr->skill_thn += race_info[cr_ptr->race_idx2].r_s_thn;
		cr_ptr->skill_thb += race_info[cr_ptr->race_idx2].r_s_thb;
		cr_ptr->skill_tht += race_info[cr_ptr->race_idx2].r_s_thb;

	}




	if(cr_ptr->cls_idx != CLASS_NONE)
	{
		cr_ptr->skill_dis += class_info[cr_ptr->cls_idx].c_dis;
		cr_ptr->skill_dev += class_info[cr_ptr->cls_idx].c_dev;
		cr_ptr->skill_rob += class_info[cr_ptr->cls_idx].c_sav;
		cr_ptr->skill_eva += class_info[cr_ptr->cls_idx].c_sav;
		cr_ptr->skill_vol += class_info[cr_ptr->cls_idx].c_sav;
		cr_ptr->skill_stl += class_info[cr_ptr->cls_idx].c_stl;
		cr_ptr->skill_srh += class_info[cr_ptr->cls_idx].c_srh;
		cr_ptr->skill_fos += class_info[cr_ptr->cls_idx].c_fos;
		cr_ptr->skill_thn += class_info[cr_ptr->cls_idx].c_thn;
		cr_ptr->skill_thb += class_info[cr_ptr->cls_idx].c_thb;
		cr_ptr->skill_tht += class_info[cr_ptr->cls_idx].c_thb;
	}
	else
	{
		cr_ptr->skill_dis += class_info[CLASS_TOURIST].c_dis;
		cr_ptr->skill_dev += class_info[CLASS_TOURIST].c_dev;
		cr_ptr->skill_rob += class_info[CLASS_TOURIST].c_sav;
		cr_ptr->skill_eva += class_info[CLASS_TOURIST].c_sav;
		cr_ptr->skill_vol += class_info[CLASS_TOURIST].c_sav;
		cr_ptr->skill_stl += class_info[CLASS_TOURIST].c_stl;
		cr_ptr->skill_srh += class_info[CLASS_TOURIST].c_srh;
		cr_ptr->skill_fos += class_info[CLASS_TOURIST].c_fos;
		cr_ptr->skill_thn += class_info[CLASS_TOURIST].c_thn;
		cr_ptr->skill_thb += class_info[CLASS_TOURIST].c_thb;
		cr_ptr->skill_tht += class_info[CLASS_TOURIST].c_thb;
	}

	if(cr_ptr->chara_idx != CHARA_NONE)
	{
		cr_ptr->skill_dis += chara_info[cr_ptr->chara_idx].a_dis;
		cr_ptr->skill_dev += chara_info[cr_ptr->chara_idx].a_dev;
		cr_ptr->skill_rob += chara_info[cr_ptr->chara_idx].a_sav;
		cr_ptr->skill_eva += chara_info[cr_ptr->chara_idx].a_sav;
		cr_ptr->skill_vol += chara_info[cr_ptr->chara_idx].a_sav;
		cr_ptr->skill_stl += chara_info[cr_ptr->chara_idx].a_stl;
		cr_ptr->skill_srh += chara_info[cr_ptr->chara_idx].a_srh;
		cr_ptr->skill_fos += chara_info[cr_ptr->chara_idx].a_fos;
		cr_ptr->skill_thn += chara_info[cr_ptr->chara_idx].a_thn;
		cr_ptr->skill_thb += chara_info[cr_ptr->chara_idx].a_thb;
		cr_ptr->skill_tht += chara_info[cr_ptr->chara_idx].a_thb;
	}

	if(cr_ptr->patron_idx != PATRON_NONE)
	{
		/*TODO
		cr_ptr->skill_dis += player_patrons[cr_ptr->patron_idx].p_dis;
		cr_ptr->skill_dev += player_patrons[cr_ptr->patron_idx].p_dev;
		cr_ptr->skill_rob += player_patrons[cr_ptr->patron_idx].p_sav;
		cr_ptr->skill_eva += player_patrons[cr_ptr->patron_idx].p_sav;
		cr_ptr->skill_vol += player_patrons[cr_ptr->patron_idx].p_sav;
		cr_ptr->skill_stl += player_patrons[cr_ptr->patron_idx].p_stl;
		cr_ptr->skill_srh += player_patrons[cr_ptr->patron_idx].p_srh;
		cr_ptr->skill_fos += player_patrons[cr_ptr->patron_idx].p_fos;
		cr_ptr->skill_thn += player_patrons[cr_ptr->patron_idx].p_thn;
		cr_ptr->skill_thb += player_patrons[cr_ptr->patron_idx].p_thb;
		cr_ptr->skill_tht += player_patrons[cr_ptr->patron_idx].p_thb;
		*/
	}

	if(cr_ptr->dr >= 0)
	{
		cr_ptr->skill_rob += adj_dr_saving[cr_ptr->dr];
		cr_ptr->skill_eva += adj_dr_saving[cr_ptr->dr];
		cr_ptr->skill_vol += adj_dr_saving[cr_ptr->dr];
	}

	cr_ptr->see_infra = (cr_ptr->see_infra + j) / k;

	if (have_weapon(cr_ptr, INVEN_1STARM)) cr_ptr->migite = TRUE;
	if (have_weapon(cr_ptr, INVEN_2NDARM))
	{
		cr_ptr->hidarite = TRUE;
		if (!cr_ptr->migite) default_hand = 1;
	}

	if (CAN_TWO_HANDS_WIELDING(cr_ptr))
	{
		if (cr_ptr->migite && (empty_hands(cr_ptr, FALSE) == EMPTY_HAND_LARM) &&
			object_allow_two_hands_wielding(cr_ptr, &cr_ptr->inventory[INVEN_1STARM]))
		{
			cr_ptr->ryoute = TRUE;
		}
		else if (cr_ptr->hidarite && (empty_hands(cr_ptr, FALSE) == EMPTY_HAND_RARM) &&
			object_allow_two_hands_wielding(cr_ptr, &cr_ptr->inventory[INVEN_2NDARM]))
		{
			cr_ptr->ryoute = TRUE;
		}
		else
		{
			switch (cr_ptr->cls_idx)
			{
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
			case CLASS_BERSERKER:
				if (empty_hands(cr_ptr, FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))
				{
					cr_ptr->migite = TRUE;
					cr_ptr->ryoute = TRUE;
				}
				break;
			}
		}
	}

	if (!cr_ptr->migite && !cr_ptr->hidarite)
	{
		if (empty_hands_status & EMPTY_HAND_RARM) cr_ptr->migite = TRUE;
		else if (empty_hands_status == EMPTY_HAND_LARM)
		{
			cr_ptr->hidarite = TRUE;
			default_hand = 1;
		}
	}

	if (cr_ptr->special_defense & KAMAE_MASK)
	{
		if (!(empty_hands_status & EMPTY_HAND_RARM))
		{
			set_action(cr_ptr, ACTION_NONE);
		}
	}

	switch (cr_ptr->cls_idx)
	{
		case CLASS_WARRIOR:
			if (cr_ptr->lev > 29) cr_ptr->resist_fear = TRUE;
			if (cr_ptr->lev > 44) cr_ptr->regenerate = TRUE;
			break;
		case CLASS_PALADIN:
			if (cr_ptr->lev > 39) cr_ptr->resist_fear = TRUE;
			break;
		case CLASS_CHAOS_WARRIOR:
			if (cr_ptr->lev > 29) cr_ptr->resist_chaos = TRUE;
			if (cr_ptr->lev > 39) cr_ptr->resist_fear = TRUE;
			break;
		case CLASS_MINDCRAFTER:
			if (cr_ptr->lev >  9) cr_ptr->resist_fear = TRUE;
			if (cr_ptr->lev > 19) cr_ptr->sustain_wis = TRUE;
			if (cr_ptr->lev > 29) cr_ptr->resist_conf = TRUE;
			if (cr_ptr->lev > 39) cr_ptr->telepathy = TRUE;
			break;
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
			/* Unencumbered Monks become faster every 10 levels */
			if (!(heavy_armor(cr_ptr)))
			{
				if (!(race_is_(cr_ptr, RACE_KLACKON) ||
				      race_is_(cr_ptr, RACE_SPRITE) ||
				      (cr_ptr->chara_idx == CHARA_MUNCHKIN)))
					new_speed += (cr_ptr->lev) / 10;

				/* Free action if unencumbered at level 25 */
				if  (cr_ptr->lev > 24)
					cr_ptr->free_act = TRUE;
			}
			break;
		case CLASS_SORCERER:
			cr_ptr->to_a -= 50;
			cr_ptr->dis_to_a -= 50;
			break;
		case CLASS_BARD:
			cr_ptr->resist_sound = TRUE;
			break;
		case CLASS_SAMURAI:
			if (cr_ptr->lev > 29) cr_ptr->resist_fear = TRUE;
			break;
		case CLASS_BERSERKER:
			cr_ptr->shero = 1;
			cr_ptr->sustain_str = TRUE;
			cr_ptr->sustain_dex = TRUE;
			cr_ptr->sustain_con = TRUE;
			cr_ptr->regenerate = TRUE;
			cr_ptr->free_act = TRUE;
			new_speed += 2;
			if (cr_ptr->lev > 29) new_speed++;
			if (cr_ptr->lev > 39) new_speed++;
			if (cr_ptr->lev > 44) new_speed++;
			if (cr_ptr->lev > 49) new_speed++;
			cr_ptr->to_a += 10+cr_ptr->lev/2;
			cr_ptr->dis_to_a += 10+cr_ptr->lev/2;
			cr_ptr->skill_dig += (100+cr_ptr->lev*8);
			if (cr_ptr->lev > 39) cr_ptr->reflect = TRUE;
			play_redraw |= PR_STATUS;
			break;
		case CLASS_MIRROR_MASTER:
			if (cr_ptr->lev > 39) cr_ptr->reflect = TRUE;
			break;
		case CLASS_NINJA:
			/* Unencumbered Ninjas become faster every 10 levels */
			if (heavy_armor(cr_ptr))
			{
				new_speed -= (cr_ptr->lev) / 10;
				cr_ptr->skill_stl -= (cr_ptr->lev)/10;
			}
			else if ((!cr_ptr->inventory[INVEN_1STARM].k_idx || cr_ptr->migite) &&
			         (!cr_ptr->inventory[INVEN_2NDARM].k_idx || cr_ptr->hidarite))
			{
				new_speed += 3;
				if (!(race_is_(cr_ptr, RACE_KLACKON) ||
				      race_is_(cr_ptr, RACE_SPRITE) ||
				      (cr_ptr->chara_idx == CHARA_MUNCHKIN)))
					new_speed += (cr_ptr->lev) / 10;
				cr_ptr->skill_stl += (cr_ptr->lev)/10;

				/* Free action if unencumbered at level 25 */
				if  (cr_ptr->lev > 24)
					cr_ptr->free_act = TRUE;
			}
			if ((!cr_ptr->inventory[INVEN_1STARM].k_idx || cr_ptr->migite) &&
			    (!cr_ptr->inventory[INVEN_2NDARM].k_idx || cr_ptr->hidarite))
			{
				cr_ptr->to_a += cr_ptr->lev/2+5;
				cr_ptr->dis_to_a += cr_ptr->lev/2+5;
			}
			cr_ptr->slow_digest = TRUE;
			cr_ptr->resist_fear = TRUE;
			if (cr_ptr->lev > 19) cr_ptr->resist_pois = TRUE;
			if (cr_ptr->lev > 24) cr_ptr->sustain_dex = TRUE;
			if (cr_ptr->lev > 29) cr_ptr->see_inv = TRUE;
			if (cr_ptr->lev > 44)
			{
				cr_ptr->oppose_pois = 1;
				play_redraw |= PR_STATUS;
			}
			cr_ptr->see_nocto = TRUE;
			break;
	}

	/***** Races ****/

	// TODO: Mimic Race control to creature.c
	if (!cr_ptr->mimic_form)
	{
		set_resistance(cr_ptr);

		if(IS_RACE(cr_ptr, RACE_SPRITE))
		{
			cr_ptr->levitation = TRUE;
			new_speed += (cr_ptr->lev) / 10;
		}
		if(IS_RACE(cr_ptr, RACE_ENT))
		{
			/* Ents dig like maniacs, but only with their hands. */
			if (!cr_ptr->inventory[INVEN_1STARM].k_idx) 
				cr_ptr->skill_dig += cr_ptr->lev * 10;
			/* Ents get tougher and stronger as they age, but lose dexterity. */
			if (cr_ptr->lev > 25) cr_ptr->stat_add[STAT_STR]++;
			if (cr_ptr->lev > 40) cr_ptr->stat_add[STAT_STR]++;
			if (cr_ptr->lev > 45) cr_ptr->stat_add[STAT_STR]++;

			if (cr_ptr->lev > 25) cr_ptr->stat_add[STAT_DEX]--;
			if (cr_ptr->lev > 40) cr_ptr->stat_add[STAT_DEX]--;
			if (cr_ptr->lev > 45) cr_ptr->stat_add[STAT_DEX]--;

			if (cr_ptr->lev > 25) cr_ptr->stat_add[STAT_CON]++;
			if (cr_ptr->lev > 40) cr_ptr->stat_add[STAT_CON]++;
			if (cr_ptr->lev > 45) cr_ptr->stat_add[STAT_CON]++;
		}
		if(IS_RACE(cr_ptr, RACE_KLACKON))
		{
			new_speed += (cr_ptr->lev) / 10;
		}
		//TODOO
		/*
			case LICH:
			cr_ptr->pass_wall = TRUE;
			break;
		*/
	}

	set_unreached_race_level_penalty(cr_ptr);

	if (cr_ptr->ult_res || (cr_ptr->special_defense & KATA_MUSOU))
	{
		cr_ptr->see_inv = TRUE;
		cr_ptr->free_act = TRUE;
		cr_ptr->slow_digest = TRUE;
		cr_ptr->regenerate = TRUE;
		cr_ptr->levitation = TRUE;
		cr_ptr->hold_life = TRUE;
		cr_ptr->telepathy = TRUE;
		cr_ptr->lite = TRUE;
		cr_ptr->sustain_str = TRUE;
		cr_ptr->sustain_int = TRUE;
		cr_ptr->sustain_wis = TRUE;
		cr_ptr->sustain_con = TRUE;
		cr_ptr->sustain_dex = TRUE;
		cr_ptr->sustain_chr = TRUE;
		cr_ptr->resist_acid = TRUE;
		cr_ptr->resist_elec = TRUE;
		cr_ptr->resist_fire = TRUE;
		cr_ptr->resist_cold = TRUE;
		cr_ptr->resist_pois = TRUE;
		cr_ptr->resist_conf = TRUE;
		cr_ptr->resist_sound = TRUE;
		cr_ptr->resist_lite = TRUE;
		cr_ptr->resist_dark = TRUE;
		cr_ptr->resist_chaos = TRUE;
		cr_ptr->resist_disen = TRUE;
		cr_ptr->resist_shard = TRUE;
		cr_ptr->resist_nexus = TRUE;
		cr_ptr->resist_blind = TRUE;
		cr_ptr->resist_neth = TRUE;
		cr_ptr->resist_fear = TRUE;
		cr_ptr->reflect = TRUE;
		cr_ptr->sh_fire = TRUE;
		cr_ptr->sh_elec = TRUE;
		cr_ptr->sh_cold = TRUE;
		cr_ptr->to_a += 100;
		cr_ptr->dis_to_a += 100;
	}
	/* Temporary shield */
	else if (cr_ptr->tsubureru || cr_ptr->shield || cr_ptr->magicdef)
	{
		cr_ptr->to_a += 50;
		cr_ptr->dis_to_a += 50;
	}

	if (cr_ptr->tim_res_nether)
	{
		cr_ptr->resist_neth = TRUE;
	}
	if (cr_ptr->tim_sh_fire)
	{
		cr_ptr->sh_fire = TRUE;
	}
	if (cr_ptr->tim_res_time)
	{
		cr_ptr->resist_time = TRUE;
	}


	/* Sexy Gal */
	if (cr_ptr->chara_idx == CHARA_SEXY) cr_ptr->cursed |= (TRC_AGGRAVATE);
	if (cr_ptr->chara_idx == CHARA_NAMAKE) cr_ptr->to_m_chance += 10;
	if (cr_ptr->chara_idx == CHARA_KIREMONO) cr_ptr->to_m_chance -= 3;
	if (cr_ptr->chara_idx == CHARA_CHARGEMAN) cr_ptr->resist_conf = TRUE;
	if ((cr_ptr->chara_idx == CHARA_GAMAN) || (cr_ptr->chara_idx == CHARA_CHIKARA)) cr_ptr->to_m_chance++;

	/* Lucky man */
	//TODO if (cr_ptr->chara_idx == CHARA_LUCKY) cr_ptr->flags14 |= RF14_GOOD_LUCK;

	if (cr_ptr->chara_idx == CHARA_MUNCHKIN)
	{
		cr_ptr->resist_blind = TRUE;
		cr_ptr->resist_conf  = TRUE;
		cr_ptr->hold_life = TRUE;
		if (cr_ptr->cls_idx != CLASS_NINJA) cr_ptr->lite = TRUE;

		if ((cr_ptr->race_idx1 != RACE_KLACKON) && (cr_ptr->race_idx1 != RACE_SPRITE))
			/* Munchkin become faster */
			new_speed += (cr_ptr->lev) / 10 + 5;
	}

	if (music_singing(cr_ptr, MUSIC_WALL))
	{
		cr_ptr->kill_wall = TRUE;
	}

	/* Hack -- apply racial/class stat maxes */
	/* Apply the racial modifiers */
	for (i = 0; i < 6; i++)
	{
		if(IS_PURE(cr_ptr))
			cr_ptr->stat_add[i] += tmp_rcr_ptr->r_adj[i];
		else
		{
			cr_ptr->stat_add[i] += tmp_rcr_ptr->r_s_adj[i];
			cr_ptr->stat_add[i] += tmp_rcr_ptr2->r_s_adj[i];
		}

		if(cr_ptr->cls_idx != CLASS_NONE)
		{
			cr_ptr->stat_add[i] += class_info[cr_ptr->cls_idx].c_adj[i];
			if(cr_ptr->cls_bonus) cr_ptr->stat_add[i] += class_info[cr_ptr->cls_idx].c_adj_b[i];
		}

		if(cr_ptr->chara_idx != CHARA_NONE)
			cr_ptr->stat_add[i] += chara_info[cr_ptr->chara_idx].a_adj[i];

/*TODO
		if(cr_ptr->patron_idx != PATRON_NONE)
			cr_ptr->stat_add[i] += player_patrons[cr_ptr->patron_idx].p_adj[i];
*/

	}



	/* I'm adding the mutations here for the lack of a better place... */
	if (cr_ptr->flags14)
	{
		/* Hyper Strength */
		if (has_cf_creature(cr_ptr, CF_HYPER_STR))
		{
			cr_ptr->stat_add[STAT_STR] += 4;
		}

		/* Puny */
		if (has_cf_creature(cr_ptr, CF_PUNY))
		{
			cr_ptr->stat_add[STAT_STR] -= 4;
		}

		/* Living computer */
		if (has_cf_creature(cr_ptr, CF_HYPER_INT))
		{
			cr_ptr->stat_add[STAT_INT] += 4;
			cr_ptr->stat_add[STAT_WIS] += 4;
		}

		/* Moronic */
		if (has_cf_creature(cr_ptr, CF_MORONIC))
		{
			cr_ptr->stat_add[STAT_INT] -= 4;
			cr_ptr->stat_add[STAT_WIS] -= 4;
		}

		if (has_cf_creature(cr_ptr, CF_RESILIENT))
		{
			cr_ptr->stat_add[STAT_CON] += 4;
		}

		if (has_cf_creature(cr_ptr, CF_XTRA_FAT))
		{
			cr_ptr->stat_add[STAT_CON] += 2;
			new_speed -= 2;
		}

		if (has_cf_creature(cr_ptr, CF_ALBINO))
		{
			cr_ptr->stat_add[STAT_CON] -= 4;
		}

		if (has_cf_creature(cr_ptr, CF_FLESH_ROT))
		{
			cr_ptr->stat_add[STAT_CON] -= 2;
			cr_ptr->stat_add[STAT_CHR] -= 1;
			cr_ptr->regenerate = FALSE;
			/* Cancel innate regeneration */
		}

		if (has_cf_creature(cr_ptr, CF_SILLY_VOI))
		{
			cr_ptr->stat_add[STAT_CHR] -= 4;
		}

		if (has_cf_creature(cr_ptr, CF_BLANK_FAC))
		{
			cr_ptr->stat_add[STAT_CHR] -= 1;
		}

		if (has_cf_creature(cr_ptr, CF_XTRA_EYES))
		{
			cr_ptr->skill_fos += 15;
			cr_ptr->skill_srh += 15;
		}

		if (has_cf_creature(cr_ptr, CF_MAGIC_RES))
		{
			cr_ptr->skill_rob += (15 + (cr_ptr->lev / 5));
			cr_ptr->skill_eva += (15 + (cr_ptr->lev / 5));
			cr_ptr->skill_vol += (15 + (cr_ptr->lev / 5));
		}

		if (has_cf_creature(cr_ptr, CF_XTRA_NOIS))
		{
			cr_ptr->skill_stl -= 3;
		}

		if (has_cf_creature(cr_ptr, CF_INFRAVIS))
		{
			cr_ptr->see_infra += 3;
		}

		if (has_cf_creature(cr_ptr, CF_XTRA_LEGS))
		{
			new_speed += 3;
		}

		if (has_cf_creature(cr_ptr, CF_SHORT_LEG))
		{
			new_speed -= 3;
		}

		if (has_cf_creature(cr_ptr, CF_ELEC_TOUC))
		{
			cr_ptr->sh_elec = TRUE;
		}

		if (has_cf_creature(cr_ptr, CF_FIRE_BODY))
		{
			cr_ptr->sh_fire = TRUE;
			cr_ptr->lite = TRUE;
		}

		if (has_cf_creature(cr_ptr, CF_WART_SKIN))
		{
			cr_ptr->stat_add[STAT_CHR] -= 2;
			cr_ptr->to_a += 5;
			cr_ptr->dis_to_a += 5;
		}

		if (has_cf_creature(cr_ptr, CF_SCALES))
		{
			cr_ptr->stat_add[STAT_CHR] -= 1;
			cr_ptr->to_a += 10;
			cr_ptr->dis_to_a += 10;
		}

		if (has_cf_creature(cr_ptr, CF_IRON_SKIN))
		{
			cr_ptr->stat_add[STAT_DEX] -= 1;
			cr_ptr->to_a += 25;
			cr_ptr->dis_to_a += 25;
		}

		if (has_cf_creature(cr_ptr, CF_WINGS))
		{
			cr_ptr->levitation = TRUE;
		}

		if (has_cf_creature(cr_ptr, CF_FEARLESS))
		{
			cr_ptr->resist_fear = TRUE;
		}

		if (has_cf_creature(cr_ptr, CF_REGEN))
		{
			cr_ptr->regenerate = TRUE;
		}

		if (has_cf_creature(cr_ptr, CF_ESP))
		{
			cr_ptr->telepathy = TRUE;
		}

		if (has_cf_creature(cr_ptr, CF_LIMBER))
		{
			cr_ptr->stat_add[STAT_DEX] += 3;
		}

		if (has_cf_creature(cr_ptr, CF_ARTHRITIS))
		{
			cr_ptr->stat_add[STAT_DEX] -= 3;
		}

		if (has_cf_creature(cr_ptr, CF_MOTION))
		{
			cr_ptr->free_act = TRUE;
			cr_ptr->skill_stl += 1;
		}

		if (has_cf_creature(cr_ptr, CF_ILL_NORM))
		{
			cr_ptr->stat_add[STAT_CHR] = 0;
		}
	}

	if (cr_ptr->tsuyoshi)
	{
		cr_ptr->stat_add[STAT_STR] += 4;
		cr_ptr->stat_add[STAT_CON] += 4;
	}

	/* Scan the usable cr_ptr->inventory */
	for (i = INVEN_1STARM; i < INVEN_TOTAL; i++)
	{

		int bonus_to_h, bonus_to_d;
		o_ptr = &cr_ptr->inventory[i];
		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		if(i == INVEN_BODY || i == INVEN_1STHEAD || i == INVEN_1STHANDS || i == INVEN_FEET || i == INVEN_OUTER)
		{
			rate = set_inventory_fitting_rate(cr_ptr, o_ptr, i);
		}

		/* Extract the item flags */
		object_flags(o_ptr, flgs);

		cr_ptr->cursed |= (o_ptr->curse_flags & (0xFFFFFFF0L));
		if (o_ptr->name1 == ART_CHAINSWORD) cr_ptr->cursed |= TRC_CHAINSWORD;

		/* Affect stats */
		if (have_flag(flgs, TR_STR)) cr_ptr->stat_add[STAT_STR] += o_ptr->pval;
		if (have_flag(flgs, TR_INT)) cr_ptr->stat_add[STAT_INT] += o_ptr->pval;
		if (have_flag(flgs, TR_WIS)) cr_ptr->stat_add[STAT_WIS] += o_ptr->pval;
		if (have_flag(flgs, TR_DEX)) cr_ptr->stat_add[STAT_DEX] += o_ptr->pval;
		if (have_flag(flgs, TR_CON)) cr_ptr->stat_add[STAT_CON] += o_ptr->pval;
		if (have_flag(flgs, TR_CHR)) cr_ptr->stat_add[STAT_CHR] += o_ptr->pval;

		if (have_flag(flgs, TR_MAGIC_MASTERY))    cr_ptr->skill_dev += 8*o_ptr->pval;

		/* Affect stealth */
		if (have_flag(flgs, TR_STEALTH)) cr_ptr->skill_stl += o_ptr->pval;

		/* Affect searching ability (factor of five) */
		if (have_flag(flgs, TR_SEARCH)) cr_ptr->skill_srh += (o_ptr->pval * 5);

		/* Affect searching frequency (factor of five) */
		if (have_flag(flgs, TR_SEARCH)) cr_ptr->skill_fos += (o_ptr->pval * 5);

		/* Affect infravision */
		if (have_flag(flgs, TR_INFRA)) cr_ptr->see_infra += o_ptr->pval;

		/* Affect digging (factor of 20) */
		if (have_flag(flgs, TR_TUNNEL)) cr_ptr->skill_dig += (o_ptr->pval * 20);

		/* Affect speed */
		if(i == INVEN_BODY || i == INVEN_1STHEAD || i ==  INVEN_1STHANDS || i == INVEN_FEET)
		{
			if (have_flag(flgs, TR_SPEED) && rate > 90 && rate < 120) new_speed += o_ptr->pval;
			if (rate <= 90 || rate > 120) new_speed -= 3;
		}
		else if(i == INVEN_OUTER)
		{
			if (have_flag(flgs, TR_SPEED)) new_speed += o_ptr->pval;
			if(rate >= 180) new_speed -= 10;
			else if(rate >= 140) new_speed -= 5;
			else if(rate >= 120) new_speed -= 2;
		}
		else
		{
			if (have_flag(flgs, TR_SPEED)) new_speed += o_ptr->pval;
		}


		/* Affect blows */
		if (have_flag(flgs, TR_BLOWS))
		{
			if((i == INVEN_1STARM || i == INVEN_RIGHT) && !cr_ptr->ryoute) extra_blows[0] += o_ptr->pval;
			else if((i == INVEN_2NDARM || i == INVEN_LEFT) && !cr_ptr->ryoute) extra_blows[1] += o_ptr->pval;
			else {extra_blows[0] += o_ptr->pval; extra_blows[1] += o_ptr->pval;}
		}

		/* Hack -- cause earthquakes */
		if (have_flag(flgs, TR_IMPACT)) cr_ptr->impact[(i == INVEN_1STARM) ? 0 : 1] = TRUE;

		/* Boost shots */
		if (have_flag(flgs, TR_XTRA_SHOTS)) extra_shots++;

		/* Various flags */
		if (have_flag(flgs, TR_AGGRAVATE))   cr_ptr->cursed |= TRC_AGGRAVATE;
		if (have_flag(flgs, TR_DRAIN_EXP))   cr_ptr->cursed |= TRC_DRAIN_EXP;
		if (have_flag(flgs, TR_TY_CURSE))    cr_ptr->cursed |= TRC_TY_CURSE;
		if (have_flag(flgs, TR_DEC_MANA))    cr_ptr->dec_mana = TRUE;
		if (have_flag(flgs, TR_BLESSED))     cr_ptr->bless_blade = TRUE;
		if (have_flag(flgs, TR_XTRA_MIGHT))  cr_ptr->xtra_might = TRUE;
		if (have_flag(flgs, TR_SLOW_DIGEST)) cr_ptr->slow_digest = TRUE;
		if (have_flag(flgs, TR_REGEN))       cr_ptr->regenerate = TRUE;

		if(i == INVEN_BODY || i == INVEN_1STHEAD || i == INVEN_1STHANDS || i == INVEN_FEET)
		{
			if (rate > 95 && rate < 110){
				if (have_flag(flgs, TR_TELEPATHY))   cr_ptr->telepathy = TRUE;
				if (have_flag(flgs, TR_ESP_ANIMAL))  cr_ptr->esp_animal = TRUE;
				if (have_flag(flgs, TR_ESP_UNDEAD))  cr_ptr->esp_undead = TRUE;
				if (have_flag(flgs, TR_ESP_DEMON))   cr_ptr->esp_demon = TRUE;
				if (have_flag(flgs, TR_ESP_ORC))     cr_ptr->esp_orc = TRUE;
				if (have_flag(flgs, TR_ESP_TROLL))   cr_ptr->esp_troll = TRUE;
				if (have_flag(flgs, TR_ESP_GIANT))   cr_ptr->esp_giant = TRUE;
				if (have_flag(flgs, TR_ESP_DRAGON))  cr_ptr->esp_dragon = TRUE;
				if (have_flag(flgs, TR_ESP_HUMAN))   cr_ptr->esp_human = TRUE;
				if (have_flag(flgs, TR_ESP_EVIL))    cr_ptr->esp_evil = TRUE;
				if (have_flag(flgs, TR_ESP_GOOD))    cr_ptr->esp_good = TRUE;
				if (have_flag(flgs, TR_ESP_NONLIVING)) cr_ptr->esp_nonliving = TRUE;
				if (have_flag(flgs, TR_ESP_UNIQUE))  cr_ptr->esp_unique = TRUE;
			}
		}
		else
		{
			if (have_flag(flgs, TR_TELEPATHY))   cr_ptr->telepathy = TRUE;
			if (have_flag(flgs, TR_ESP_ANIMAL))  cr_ptr->esp_animal = TRUE;
			if (have_flag(flgs, TR_ESP_UNDEAD))  cr_ptr->esp_undead = TRUE;
			if (have_flag(flgs, TR_ESP_DEMON))   cr_ptr->esp_demon = TRUE;
			if (have_flag(flgs, TR_ESP_ORC))     cr_ptr->esp_orc = TRUE;
			if (have_flag(flgs, TR_ESP_TROLL))   cr_ptr->esp_troll = TRUE;
			if (have_flag(flgs, TR_ESP_GIANT))   cr_ptr->esp_giant = TRUE;
			if (have_flag(flgs, TR_ESP_DRAGON))  cr_ptr->esp_dragon = TRUE;
			if (have_flag(flgs, TR_ESP_HUMAN))   cr_ptr->esp_human = TRUE;
			if (have_flag(flgs, TR_ESP_EVIL))    cr_ptr->esp_evil = TRUE;
			if (have_flag(flgs, TR_ESP_GOOD))    cr_ptr->esp_good = TRUE;
			if (have_flag(flgs, TR_ESP_NONLIVING)) cr_ptr->esp_nonliving = TRUE;
			if (have_flag(flgs, TR_ESP_UNIQUE))  cr_ptr->esp_unique = TRUE;
		}

		if (have_flag(flgs, TR_SEE_INVIS))   cr_ptr->see_inv = TRUE;
		if (have_flag(flgs, TR_LEVITATION))     cr_ptr->levitation = TRUE;
		if (have_flag(flgs, TR_FREE_ACT))    cr_ptr->free_act = TRUE;
		if (have_flag(flgs, TR_HOLD_LIFE))   cr_ptr->hold_life = TRUE;

		if (have_flag(flgs, TR_WARNING)){
			if (!o_ptr->inscription || !(my_strchr(quark_str(o_ptr->inscription),'$')))
			  cr_ptr->warning = TRUE;
		}

		if (have_flag(flgs, TR_TELEPORT))
		{
			if (object_is_cursed(o_ptr)) cr_ptr->cursed |= TRC_TELEPORT;
			else
			{
				cptr insc = quark_str(o_ptr->inscription);

				if (o_ptr->inscription && my_strchr(insc, '.'))
				{
					/*
					 * {.} will stop random teleportation.
					 */
				}
				else
				{
					/* Controlled random teleportation */
					cr_ptr->cursed |= TRC_TELEPORT_SELF;
				}
			}
		}

		/* Immunity flags */
		if (have_flag(flgs, TR_IM_FIRE)) cr_ptr->immune_fire = TRUE;
		if (have_flag(flgs, TR_IM_ACID)) cr_ptr->immune_acid = TRUE;
		if (have_flag(flgs, TR_IM_COLD)) cr_ptr->immune_cold = TRUE;
		if (have_flag(flgs, TR_IM_ELEC)) cr_ptr->immune_elec = TRUE;

		/* Resistance flags */
		if (have_flag(flgs, TR_RES_ACID))   cr_ptr->resist_acid += 1;
		if (have_flag(flgs, TR_RES_ELEC))   cr_ptr->resist_elec += 1;
		if (have_flag(flgs, TR_RES_FIRE))   cr_ptr->resist_fire += 1;
		if (have_flag(flgs, TR_RES_COLD))   cr_ptr->resist_cold += 1;
		if (have_flag(flgs, TR_RES_POIS))   cr_ptr->resist_pois += 1;
		if (have_flag(flgs, TR_RES_SOUND))  cr_ptr->resist_sound += 1;
		if (have_flag(flgs, TR_RES_LITE))   cr_ptr->resist_lite += 1;
		if (have_flag(flgs, TR_RES_DARK))   cr_ptr->resist_dark += 1;
		if (have_flag(flgs, TR_RES_CHAOS))  cr_ptr->resist_chaos += 1;
		if (have_flag(flgs, TR_RES_DISEN))  cr_ptr->resist_disen += 1;
		if (have_flag(flgs, TR_RES_SHARDS)) cr_ptr->resist_shard += 1;
		if (have_flag(flgs, TR_RES_NEXUS))  cr_ptr->resist_nexus += 1;
		if (have_flag(flgs, TR_RES_NETHER)) cr_ptr->resist_neth += 1;
		if (o_ptr->name2 == EGO_RING_RES_TIME) cr_ptr->resist_time += 1;

		if (have_flag(flgs, TR_RES_FEAR))   cr_ptr->resist_fear = TRUE;
		if (have_flag(flgs, TR_RES_CONF))   cr_ptr->resist_conf = TRUE;
		if (have_flag(flgs, TR_RES_BLIND))  cr_ptr->resist_blind = TRUE;

		if (have_flag(flgs, TR_REFLECT))  cr_ptr->reflect = TRUE;
		if (have_flag(flgs, TR_SH_FIRE))  cr_ptr->sh_fire = TRUE;
		if (have_flag(flgs, TR_SH_ELEC))  cr_ptr->sh_elec = TRUE;
		if (have_flag(flgs, TR_SH_COLD))  cr_ptr->sh_cold = TRUE;
		if (have_flag(flgs, TR_NO_MAGIC)) cr_ptr->anti_magic = TRUE;
		if (have_flag(flgs, TR_NO_TELE))  cr_ptr->anti_tele = TRUE;

		/* Sustain flags */
		if (have_flag(flgs, TR_SUST_STR)) cr_ptr->sustain_str = TRUE;
		if (have_flag(flgs, TR_SUST_INT)) cr_ptr->sustain_int = TRUE;
		if (have_flag(flgs, TR_SUST_WIS)) cr_ptr->sustain_wis = TRUE;
		if (have_flag(flgs, TR_SUST_DEX)) cr_ptr->sustain_dex = TRUE;
		if (have_flag(flgs, TR_SUST_CON)) cr_ptr->sustain_con = TRUE;
		if (have_flag(flgs, TR_SUST_CHR)) cr_ptr->sustain_chr = TRUE;

		if (o_ptr->name2 == EGO_YOIYAMI) yoiyami = TRUE;
		if (o_ptr->name2 == EGO_2WEAPON) easy_2weapon = TRUE;


		if (o_ptr->name2 == EGO_RING_THROW) cr_ptr->mighty_throw = TRUE;
		if (have_flag(flgs, TR_EASY_SPELL)) cr_ptr->easy_spell = TRUE;
		if (o_ptr->name2 == EGO_AMU_FOOL) cr_ptr->heavy_spell = TRUE;
		if (o_ptr->name2 == EGO_AMU_NAIVETY) down_saving = TRUE;

		if (o_ptr->curse_flags & TRC_LOW_MAGIC)
		{
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				cr_ptr->to_m_chance += 10;
			}
			else
			{
				cr_ptr->to_m_chance += 3;
			}
		}

		if (o_ptr->tval == TV_CAPTURE) continue;

		/* Modify the base armor class */
		/* The base armor class is always known */
		/* Apply the bonuses to armor class */
		/* Apply the mental bonuses to armor class, if known */

		if(i == INVEN_BODY || i == INVEN_1STHEAD || i == INVEN_1STHANDS || i == INVEN_FEET)
		{
			if(rate >= 130)
			{
				cr_ptr->ac += (o_ptr->ac / 8);
				cr_ptr->dis_ac += (o_ptr->ac / 8);
				cr_ptr->to_a += (o_ptr->to_a / 8);
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += (o_ptr->to_a / 8);
			}
			else if(rate >= 120)
			{
				cr_ptr->ac += (o_ptr->ac / 4);
				cr_ptr->dis_ac += (o_ptr->ac / 4);
				cr_ptr->to_a += (o_ptr->to_a / 4);
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += (o_ptr->to_a / 4);
			}
			else if(rate >= 110)
			{
				cr_ptr->ac += (o_ptr->ac / 2);
				cr_ptr->dis_ac += (o_ptr->ac / 2);
				cr_ptr->to_a += (o_ptr->to_a / 2);
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += (o_ptr->to_a / 2);
			}
			else
			{
				cr_ptr->ac += o_ptr->ac;
				cr_ptr->dis_ac += o_ptr->ac;
				cr_ptr->to_a += o_ptr->to_a;
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += o_ptr->to_a;
			}
		}
		else if(i == INVEN_OUTER)
		{
			if(rate <= 40)
			{
				cr_ptr->ac += (o_ptr->ac / 8);
				cr_ptr->dis_ac += (o_ptr->ac / 8);
				cr_ptr->to_a += (o_ptr->to_a / 8);
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += (o_ptr->to_a / 8);
			}
			else if(rate <= 60)
			{
				cr_ptr->ac += (o_ptr->ac / 4);
				cr_ptr->dis_ac += (o_ptr->ac / 4);
				cr_ptr->to_a += (o_ptr->to_a / 4);
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += (o_ptr->to_a / 4);
			}
			else if(rate <= 80)
			{
				cr_ptr->ac += (o_ptr->ac / 2);
				cr_ptr->dis_ac += (o_ptr->ac / 2);
				cr_ptr->to_a += (o_ptr->to_a / 2);
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += (o_ptr->to_a / 2);
			}
			else
			{
				cr_ptr->ac += o_ptr->ac;
				cr_ptr->dis_ac += o_ptr->ac;
				cr_ptr->to_a += o_ptr->to_a;
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += o_ptr->to_a;
			}
		}
		else
		{
			cr_ptr->ac += o_ptr->ac;
			cr_ptr->dis_ac += o_ptr->ac;
			cr_ptr->to_a += o_ptr->to_a;
				if (object_is_known(o_ptr)) cr_ptr->dis_to_a += o_ptr->to_a;
		}


		if (o_ptr->curse_flags & TRC_LOW_MELEE)
		{
			int slot = i - INVEN_1STARM;
			if (slot < 2)
			{
				if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
				{
					cr_ptr->to_h[slot] -= 15;
					if (o_ptr->ident & IDENT_MENTAL) cr_ptr->dis_to_h[slot] -= 15;
				}
				else
				{
					cr_ptr->to_h[slot] -= 5;
					if (o_ptr->ident & IDENT_MENTAL) cr_ptr->dis_to_h[slot] -= 5;
				}
			}
			else
			{
				if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
				{
					cr_ptr->to_h_b -= 15;
					if (o_ptr->ident & IDENT_MENTAL) cr_ptr->dis_to_h_b -= 15;
				}
				else
				{
					cr_ptr->to_h_b -= 5;
					if (o_ptr->ident & IDENT_MENTAL) cr_ptr->dis_to_h_b -= 5;
				}
			}
		}

		if (o_ptr->curse_flags & TRC_LOW_AC)
		{
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				cr_ptr->to_a -= 30;
				if (o_ptr->ident & IDENT_MENTAL) cr_ptr->dis_to_a -= 30;
			}
			else
			{
				cr_ptr->to_a -= 10;
				if (o_ptr->ident & IDENT_MENTAL) cr_ptr->dis_to_a -= 10;
			}
		}

		/* Hack -- do not apply "weapon" bonuses */
		if (i == INVEN_1STARM && have_weapon(cr_ptr, i)) continue;
		if (i == INVEN_2NDARM && have_weapon(cr_ptr, i)) continue;

		/* Hack -- do not apply "bow" bonuses */
		if (i == INVEN_BOW) continue;

		bonus_to_h = o_ptr->to_h;
		bonus_to_d = o_ptr->to_d;

		if (cr_ptr->cls_idx == CLASS_NINJA)
		{
			if (o_ptr->to_h > 0) bonus_to_h = (o_ptr->to_h+1)/2;
			if (o_ptr->to_d > 0) bonus_to_d = (o_ptr->to_d+1)/2;
		}

		/* To Bow and Natural attack */

		/* Apply the bonuses to hit/damage */
		cr_ptr->to_h_b += bonus_to_h;
		cr_ptr->to_h_m += bonus_to_h;
		cr_ptr->to_d_m += bonus_to_d;

		/* Apply the mental bonuses tp hit/damage, if known */
		if (object_is_known(o_ptr)) cr_ptr->dis_to_h_b += bonus_to_h;

		/* To Melee */
		if ((i == INVEN_LEFT || i == INVEN_RIGHT) && !cr_ptr->ryoute)
		{
			/* Apply the bonuses to hit/damage */
			cr_ptr->to_h[i-INVEN_RIGHT] += bonus_to_h;
			cr_ptr->to_d[i-INVEN_RIGHT] += bonus_to_d;

			/* Apply the mental bonuses tp hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				cr_ptr->dis_to_h[i-INVEN_RIGHT] += bonus_to_h;
				cr_ptr->dis_to_d[i-INVEN_RIGHT] += bonus_to_d;
			}
		}
		else if (cr_ptr->migite && cr_ptr->hidarite)
		{
			/* Apply the bonuses to hit/damage */
			cr_ptr->to_h[0] += (bonus_to_h > 0) ? (bonus_to_h+1)/2 : bonus_to_h;
			cr_ptr->to_h[1] += (bonus_to_h > 0) ? bonus_to_h/2 : bonus_to_h;
			cr_ptr->to_d[0] += (bonus_to_d > 0) ? (bonus_to_d+1)/2 : bonus_to_d;
			cr_ptr->to_d[1] += (bonus_to_d > 0) ? bonus_to_d/2 : bonus_to_d;

			/* Apply the mental bonuses tp hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				cr_ptr->dis_to_h[0] += (bonus_to_h > 0) ? (bonus_to_h+1)/2 : bonus_to_h;
				cr_ptr->dis_to_h[1] += (bonus_to_h > 0) ? bonus_to_h/2 : bonus_to_h;
				cr_ptr->dis_to_d[0] += (bonus_to_d > 0) ? (bonus_to_d+1)/2 : bonus_to_d;
				cr_ptr->dis_to_d[1] += (bonus_to_d > 0) ? bonus_to_d/2 : bonus_to_d;
			}
		}
		else
		{
			/* Apply the bonuses to hit/damage */
			cr_ptr->to_h[default_hand] += bonus_to_h;
			cr_ptr->to_d[default_hand] += bonus_to_d;

			/* Apply the mental bonuses to hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				cr_ptr->dis_to_h[default_hand] += bonus_to_h;
				cr_ptr->dis_to_d[default_hand] += bonus_to_d;
			}
		}
	}

	if (old_mighty_throw != cr_ptr->mighty_throw)
	{
		/* Redraw average damege display of Shuriken */
		play_window |= PW_INVEN;
	}

	if (cr_ptr->cursed & TRC_TELEPORT) cr_ptr->cursed &= ~(TRC_TELEPORT_SELF);

	/* Monks get extra ac for armour _not worn_ */
	if (((cr_ptr->cls_idx == CLASS_MONK) || (cr_ptr->cls_idx == CLASS_FORCETRAINER)) && !heavy_armor(cr_ptr))
	{
		if (!(cr_ptr->inventory[INVEN_BODY].k_idx))
		{
			cr_ptr->to_a += (cr_ptr->lev * 3) / 2;
			cr_ptr->dis_to_a += (cr_ptr->lev * 3) / 2;
		}
		if (!(cr_ptr->inventory[INVEN_OUTER].k_idx) && (cr_ptr->lev > 15))
		{
			cr_ptr->to_a += ((cr_ptr->lev - 13) / 3);
			cr_ptr->dis_to_a += ((cr_ptr->lev - 13) / 3);
		}
		if (!(cr_ptr->inventory[INVEN_2NDARM].k_idx) && (cr_ptr->lev > 10))
		{
			cr_ptr->to_a += ((cr_ptr->lev - 8) / 3);
			cr_ptr->dis_to_a += ((cr_ptr->lev - 8) / 3);
		}
		if (!(cr_ptr->inventory[INVEN_1STHEAD].k_idx) && (cr_ptr->lev > 4))
		{
			cr_ptr->to_a += (cr_ptr->lev - 2) / 3;
			cr_ptr->dis_to_a += (cr_ptr->lev -2) / 3;
		}
		if (!(cr_ptr->inventory[INVEN_1STHANDS].k_idx))
		{
			cr_ptr->to_a += (cr_ptr->lev / 2);
			cr_ptr->dis_to_a += (cr_ptr->lev / 2);
		}
		if (!(cr_ptr->inventory[INVEN_FEET].k_idx))
		{
			cr_ptr->to_a += (cr_ptr->lev / 3);
			cr_ptr->dis_to_a += (cr_ptr->lev / 3);
		}
		if (cr_ptr->special_defense & KAMAE_BYAKKO)
		{
			cr_ptr->stat_add[STAT_STR] += 2;
			cr_ptr->stat_add[STAT_DEX] += 2;
			cr_ptr->stat_add[STAT_CON] -= 3;
		}
		else if (cr_ptr->special_defense & KAMAE_SEIRYU)
		{
		}
		else if (cr_ptr->special_defense & KAMAE_GENBU)
		{
			cr_ptr->stat_add[STAT_INT] -= 1;
			cr_ptr->stat_add[STAT_WIS] -= 1;
			cr_ptr->stat_add[STAT_DEX] -= 2;
			cr_ptr->stat_add[STAT_CON] += 3;
		}
		else if (cr_ptr->special_defense & KAMAE_SUZAKU)
		{
			cr_ptr->stat_add[STAT_STR] -= 2;
			cr_ptr->stat_add[STAT_INT] += 1;
			cr_ptr->stat_add[STAT_WIS] += 1;
			cr_ptr->stat_add[STAT_DEX] += 2;
			cr_ptr->stat_add[STAT_CON] -= 2;
		}
	}

	if (cr_ptr->special_defense & KATA_KOUKIJIN)
	{
		for (i = 0; i < 6; i++)
			cr_ptr->stat_add[i] += 5;
		cr_ptr->to_a -= 50;
		cr_ptr->dis_to_a -= 50;
	}

	/* Set Species Blow. */
	cr_ptr->blow[0] = species_info[cr_ptr->species_idx].blow[0];
	cr_ptr->blow[1] = species_info[cr_ptr->species_idx].blow[1];
	cr_ptr->blow[2] = species_info[cr_ptr->species_idx].blow[2];
	cr_ptr->blow[3] = species_info[cr_ptr->species_idx].blow[3];


	/* Hack -- aura of fire also provides light */
	if (cr_ptr->sh_fire) cr_ptr->lite = TRUE;

	/* Racial AC bonus */
	if(IS_PURE(cr_ptr))
	{
		cr_ptr->to_a += race_info[cr_ptr->race_idx1].ac_base + (race_info[cr_ptr->race_idx1].ac_plus * (cr_ptr->lev < 30 ? cr_ptr->lev : 30 ) / 30);
		cr_ptr->dis_to_a += race_info[cr_ptr->race_idx1].ac_base + (race_info[cr_ptr->race_idx1].ac_plus * (cr_ptr->lev < 30 ? cr_ptr->lev : 30 ) / 30);
	}
	else
	{
		cr_ptr->to_a += race_info[cr_ptr->race_idx1].ac_s_base + (race_info[cr_ptr->race_idx1].ac_s_plus * (cr_ptr->lev < 30 ? cr_ptr->lev : 30 ) / 30);
		cr_ptr->dis_to_a += race_info[cr_ptr->race_idx1].ac_s_base + (race_info[cr_ptr->race_idx1].ac_s_plus * (cr_ptr->lev < 30 ? cr_ptr->lev : 30 ) / 30);
		cr_ptr->to_a += race_info[cr_ptr->race_idx2].ac_s_base + (race_info[cr_ptr->race_idx2].ac_s_plus * (cr_ptr->lev < 30 ? cr_ptr->lev : 30 ) / 30);
		cr_ptr->dis_to_a += race_info[cr_ptr->race_idx2].ac_s_base + (race_info[cr_ptr->race_idx2].ac_s_plus * (cr_ptr->lev < 30 ? cr_ptr->lev : 30 ) / 30);
	}

	/* Hex bonuses */
	if (cr_ptr->realm1 == REALM_HEX)
	{
		if (hex_spelling_any(cr_ptr)) cr_ptr->skill_stl -= (1 + cr_ptr->magic_num2[0]);
		if (hex_spelling(cr_ptr, HEX_DETECT_EVIL)) cr_ptr->esp_evil = TRUE;
		if (hex_spelling(cr_ptr, HEX_XTRA_MIGHT)) cr_ptr->stat_add[STAT_STR] += 4;
		if (hex_spelling(cr_ptr, HEX_BUILDING))
		{
			cr_ptr->stat_add[STAT_STR] += 4;
			cr_ptr->stat_add[STAT_DEX] += 4;
			cr_ptr->stat_add[STAT_CON] += 4;
		}
		if (hex_spelling(cr_ptr, HEX_DEMON_AURA))
		{
			cr_ptr->sh_fire = TRUE;
			cr_ptr->regenerate = TRUE;
		}
		if (hex_spelling(cr_ptr, HEX_ICE_ARMOR))
		{
			cr_ptr->sh_cold = TRUE; 
			cr_ptr->to_a += 30;
			cr_ptr->dis_to_a += 30;
		}
		if (hex_spelling(cr_ptr, HEX_SHOCK_CLOAK))
		{
			cr_ptr->sh_elec = TRUE;
			new_speed += 3;
		}
		for (i = INVEN_1STARM; i <= INVEN_FEET; i++)
		{
			int ac = 0;
			o_ptr = &cr_ptr->inventory[i];
			if (!o_ptr->k_idx) continue;
			if (!object_is_armour(cr_ptr, o_ptr)) continue;
			if (!object_is_cursed(o_ptr)) continue;
			ac += 5;
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE) ac += 7;
			if (o_ptr->curse_flags & TRC_DIVINE_CURSE) ac += 13;
			cr_ptr->to_a += ac;
			cr_ptr->dis_to_a += ac;
		}
	}

	/* Calculate stats */
	for (i = 0; i < 6; i++)
	{
		int top, use, ind;

		/* Extract the new "stat_use" value for the stat */
		top = modify_stat_value(cr_ptr->stat_max[i], cr_ptr->stat_add[i]);

		/* Notice changes */
		if (cr_ptr->stat_top[i] != top)
		{
			/* Save the new value */
			cr_ptr->stat_top[i] = top;

			/* Redisplay the stats later */
			play_redraw |= (PR_STATS);

			/* Window stuff */
			play_window |= (PW_PLAYER);
		}


		/* Extract the new "stat_use" value for the stat */
		use = modify_stat_value(cr_ptr->stat_cur[i], cr_ptr->stat_add[i]);

		if ((i == STAT_CHR) && (has_cf_creature(cr_ptr, CF_ILL_NORM)))
		{
			/* 10 to 18/90 charisma, guaranteed, based on level */
			if (use < 8 + 2 * cr_ptr->lev)
			{
				use = 8 + 2 * cr_ptr->lev;
			}
		}

		/* Notice changes */
		if (cr_ptr->stat_use[i] != use)
		{
			/* Save the new value */
			cr_ptr->stat_use[i] = use;

			/* Redisplay the stats later */
			play_redraw |= (PR_STATS);

			/* Window stuff */
			play_window |= (PW_PLAYER);
		}


		/* Values: 3, 4, ..., 17 */
		if (use <= 18) ind = (use - 3);

		/* Ranges: 18/00-18/09, ..., 18/210-18/219 */
		else if (use <= 18+419) ind = (15 + (use - 18) / 10);

		/* Range: 18/220+ */
		else ind = (57);

		/* Notice changes */
		if (cr_ptr->stat_ind[i] != ind)
		{
			/* Save the new index */
			cr_ptr->stat_ind[i] = ind;

			/* Change in CON affects Hitpoints */
			if (i == STAT_CON)
			{
				cr_ptr->update |= (PU_HP);
			}

			/* Change in INT may affect Mana/Spells */
			else if (i == STAT_INT)
			{
				if (m_info[cr_ptr->sex].spell_stat == STAT_INT)
				{
					cr_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == STAT_WIS)
			{
				if (m_info[cr_ptr->sex].spell_stat == STAT_WIS)
				{
					cr_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == STAT_CHR)
			{
				if (m_info[cr_ptr->sex].spell_stat == STAT_CHR)
				{
					cr_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Window stuff */
			play_window |= (PW_PLAYER);
		}
	}


	/* Apply temporary "stun" */
	if (cr_ptr->stun > 50)
	{
		cr_ptr->to_h[0] -= 20;
		cr_ptr->to_h[1] -= 20;
		cr_ptr->to_h_b  -= 20;
		cr_ptr->to_h_m  -= 20;
		cr_ptr->dis_to_h[0] -= 20;
		cr_ptr->dis_to_h[1] -= 20;
		cr_ptr->dis_to_h_b  -= 20;
		cr_ptr->to_d[0] -= 20;
		cr_ptr->to_d[1] -= 20;
		cr_ptr->to_d_m -= 20;
		cr_ptr->dis_to_d[0] -= 20;
		cr_ptr->dis_to_d[1] -= 20;
	}
	else if (cr_ptr->stun)
	{
		cr_ptr->to_h[0] -= 5;
		cr_ptr->to_h[1] -= 5;
		cr_ptr->to_h_b -= 5;
		cr_ptr->to_h_m -= 5;
		cr_ptr->dis_to_h[0] -= 5;
		cr_ptr->dis_to_h[1] -= 5;
		cr_ptr->dis_to_h_b -= 5;
		cr_ptr->to_d[0] -= 5;
		cr_ptr->to_d[1] -= 5;
		cr_ptr->to_d_m -= 5;
		cr_ptr->dis_to_d[0] -= 5;
		cr_ptr->dis_to_d[1] -= 5;
	}

	/* Wraith form */
	if (cr_ptr->wraith_form)
	{
		cr_ptr->reflect = TRUE;
		cr_ptr->pass_wall = TRUE;
	}

	if (cr_ptr->kabenuke)
	{
		cr_ptr->pass_wall = TRUE;
	}

	/* Temporary blessing */
	if (IS_BLESSED(cr_ptr))
	{
		cr_ptr->to_a += 5;
		cr_ptr->dis_to_a += 5;
		cr_ptr->to_h[0] += 10;
		cr_ptr->to_h[1] += 10;
		cr_ptr->to_h_b  += 10;
		cr_ptr->to_h_m  += 10;
		cr_ptr->dis_to_h[0] += 10;
		cr_ptr->dis_to_h[1] += 10;
		cr_ptr->dis_to_h_b += 10;
	}

	if (cr_ptr->magicdef)
	{
		cr_ptr->resist_blind = TRUE;
		cr_ptr->resist_conf = TRUE;
		cr_ptr->reflect = TRUE;
		cr_ptr->free_act = TRUE;
		cr_ptr->levitation = TRUE;
	}

	/* Temporary "Hero" */
	if (IS_HERO(cr_ptr))
	{
		cr_ptr->to_h[0] += 12;
		cr_ptr->to_h[1] += 12;
		cr_ptr->to_h_b  += 12;
		cr_ptr->to_h_m  += 12;
		cr_ptr->dis_to_h[0] += 12;
		cr_ptr->dis_to_h[1] += 12;
		cr_ptr->dis_to_h_b  += 12;
	}

	/* Temporary "Beserk" */
	if (cr_ptr->shero)
	{
		cr_ptr->to_h[0] += 12;
		cr_ptr->to_h[1] += 12;
		cr_ptr->to_h_b  -= 12;
		cr_ptr->to_h_m  += 12;
		cr_ptr->to_d[0] += 3+(cr_ptr->lev/5);
		cr_ptr->to_d[1] += 3+(cr_ptr->lev/5);
		cr_ptr->to_d_m  += 3+(cr_ptr->lev/5);
		cr_ptr->dis_to_h[0] += 12;
		cr_ptr->dis_to_h[1] += 12;
		cr_ptr->dis_to_h_b  -= 12;
		cr_ptr->dis_to_d[0] += 3+(cr_ptr->lev/5);
		cr_ptr->dis_to_d[1] += 3+(cr_ptr->lev/5);
		cr_ptr->to_a -= 10;
		cr_ptr->dis_to_a -= 10;
		cr_ptr->skill_stl -= 7;
		cr_ptr->skill_dev -= 20;
		cr_ptr->skill_rob += 20;
		cr_ptr->skill_eva += 20;
		cr_ptr->skill_vol -= 30;
		cr_ptr->skill_srh -= 15;
		cr_ptr->skill_fos -= 15;
		cr_ptr->skill_tht -= 20;
		cr_ptr->skill_dig += 30;
	}

	/* Temporary "fast" */
	if (IS_FAST(cr_ptr))
	{
		new_speed += 10;
	}

	/* Temporary "slow" */
	if (cr_ptr->slow)
	{
		new_speed -= 10;
	}

	/* Temporary "telepathy" */
	if (IS_TIM_ESP(cr_ptr))
	{
		cr_ptr->telepathy = TRUE;
	}

	if (cr_ptr->ele_immune)
	{
		if (cr_ptr->special_defense & DEFENSE_ACID)
			cr_ptr->immune_acid = TRUE;
		else if (cr_ptr->special_defense & DEFENSE_ELEC)
			cr_ptr->immune_elec = TRUE;
		else if (cr_ptr->special_defense & DEFENSE_FIRE)
			cr_ptr->immune_fire = TRUE;
		else if (cr_ptr->special_defense & DEFENSE_COLD)
			cr_ptr->immune_cold = TRUE;
	}

	/* Temporary see invisible */
	if (cr_ptr->tim_invis)
	{
		cr_ptr->see_inv = TRUE;
	}

	/* Temporary infravision boost */
	if (cr_ptr->tim_infra)
	{
		cr_ptr->see_infra+=3;
	}

	/* Temporary regeneration boost */
	if (cr_ptr->tim_regen)
	{
		cr_ptr->regenerate = TRUE;
	}

	/* Temporary levitation */
	if (cr_ptr->tim_levitation)
	{
		cr_ptr->levitation = TRUE;
	}

	/* Temporary reflection */
	if (cr_ptr->tim_reflect)
	{
		cr_ptr->reflect = TRUE;
	}

	/* Hack -- Hero/Shero -> Res fear */
	if (IS_HERO(cr_ptr) || cr_ptr->shero)
	{
		cr_ptr->resist_fear = TRUE;
	}


	/* Hack -- Telepathy Change */
	if (cr_ptr->telepathy != old_telepathy)
	{
		cr_ptr->update |= (PU_MONSTERS);
	}

	if ((cr_ptr->esp_animal != old_esp_animal) ||
	    (cr_ptr->esp_undead != old_esp_undead) ||
	    (cr_ptr->esp_demon != old_esp_demon) ||
	    (cr_ptr->esp_orc != old_esp_orc) ||
	    (cr_ptr->esp_troll != old_esp_troll) ||
	    (cr_ptr->esp_giant != old_esp_giant) ||
	    (cr_ptr->esp_dragon != old_esp_dragon) ||
	    (cr_ptr->esp_human != old_esp_human) ||
	    (cr_ptr->esp_evil != old_esp_evil) ||
	    (cr_ptr->esp_good != old_esp_good) ||
	    (cr_ptr->esp_nonliving != old_esp_nonliving) ||
	    (cr_ptr->esp_unique != old_esp_unique))
	{
		cr_ptr->update |= (PU_MONSTERS);
	}

	/* Hack -- See Invis Change */
	if (cr_ptr->see_inv != old_see_inv)
	{
		cr_ptr->update |= (PU_MONSTERS);
	}

	/* Bloating slows the player down (a little) */
	if (cr_ptr->food >= PY_FOOD_MAX) new_speed -= 10;

	if (cr_ptr->special_defense & KAMAE_SUZAKU) new_speed += 10;

	if ((cr_ptr->migite && (empty_hands_status & EMPTY_HAND_RARM)) ||
	    (cr_ptr->hidarite && (empty_hands_status & EMPTY_HAND_LARM)))
	{
		cr_ptr->to_h[default_hand] += (cr_ptr->skill_exp[GINOU_SUDE]) / 200;
		cr_ptr->dis_to_h[default_hand] += (cr_ptr->skill_exp[GINOU_SUDE]) / 200;
		cr_ptr->to_d[default_hand] += cr_ptr->size * (100 + (cr_ptr->skill_exp[GINOU_SUDE]) / 200) / 100;
		cr_ptr->dis_to_d[default_hand] += cr_ptr->size * (100 + (cr_ptr->skill_exp[GINOU_SUDE]) / 200) / 100;
	}

	if (have_weapon(cr_ptr, INVEN_1STARM) && have_weapon(cr_ptr, INVEN_2NDARM))
	{
		int penalty1, penalty2;
		penalty1 = ((100 - cr_ptr->skill_exp[GINOU_NITOURYU] / 160) - (130 - cr_ptr->inventory[INVEN_1STARM].weight) / 8);
		penalty2 = ((100 - cr_ptr->skill_exp[GINOU_NITOURYU] / 160) - (130 - cr_ptr->inventory[INVEN_2NDARM].weight) / 8);
		if ((cr_ptr->inventory[INVEN_1STARM].name1 == ART_QUICKTHORN) && (cr_ptr->inventory[INVEN_2NDARM].name1 == ART_TINYTHORN))
		{
			penalty1 = penalty1 / 2 - 5;
			penalty2 = penalty2 / 2 - 5;
			new_speed += 7;
			cr_ptr->to_a += 10;
			cr_ptr->dis_to_a += 10;
		}
		if (easy_2weapon)
		{
			if (penalty1 > 0) penalty1 /= 2;
			if (penalty2 > 0) penalty2 /= 2;
		}
		else if ((cr_ptr->inventory[INVEN_2NDARM].tval == TV_SWORD) && ((cr_ptr->inventory[INVEN_2NDARM].sval == SV_MAIN_GAUCHE) || (cr_ptr->inventory[INVEN_2NDARM].sval == SV_WAKIZASHI)))
		{
			penalty1 = MAX(0, penalty1 - 10);
			penalty2 = MAX(0, penalty2 - 10);
		}
		if ((cr_ptr->inventory[INVEN_1STARM].name1 == ART_MUSASI_KATANA) && (cr_ptr->inventory[INVEN_2NDARM].name1 == ART_MUSASI_WAKIZASI))
		{
			penalty1 = MIN(0, penalty1);
			penalty2 = MIN(0, penalty2);
			cr_ptr->to_a += 10;
			cr_ptr->dis_to_a += 10;
		}
		else
		{
			if ((cr_ptr->inventory[INVEN_1STARM].name1 == ART_MUSASI_KATANA) && (penalty1 > 0))
				penalty1 /= 2;
			if ((cr_ptr->inventory[INVEN_2NDARM].name1 == ART_MUSASI_WAKIZASI) && (penalty2 > 0))
				penalty2 /= 2;
		}
		if (cr_ptr->inventory[INVEN_1STARM].tval == TV_POLEARM) penalty1 += 10;
		if (cr_ptr->inventory[INVEN_2NDARM].tval == TV_POLEARM) penalty2 += 10;
		cr_ptr->to_h[0] -= penalty1;
		cr_ptr->to_h[1] -= penalty2;
		cr_ptr->dis_to_h[0] -= penalty1;
		cr_ptr->dis_to_h[1] -= penalty2;
	}

	/* Extract the current weight (in tenth pounds) */
	j = cr_ptr->total_weight;

	if (!cr_ptr->riding)
	{
		/* Extract the "weight limit" (in tenth pounds) */
		i = (int)weight_limit(cr_ptr);
	}
	else
	{
		creature_type *riding_m_ptr = &creature_list[cr_ptr->riding];
		species_type *riding_r_ptr = &species_info[riding_m_ptr->species_idx];
		int speed = riding_m_ptr->speed;

		if (riding_m_ptr->speed > 110)
		{
			new_speed = 110 + (s16b)((speed - 110) * (cr_ptr->skill_exp[GINOU_RIDING] * 3 + cr_ptr->lev * 160L - 10000L) / (22000L));
			if (new_speed < 110) new_speed = 110;
		}
		else
		{
			new_speed = speed;
		}
		new_speed += (cr_ptr->skill_exp[GINOU_RIDING] + cr_ptr->lev *160L)/3200;
		if (riding_m_ptr->fast) new_speed += 10;
		if (riding_m_ptr->slow) new_speed -= 10;
		riding_levitation = can_fly_species(riding_r_ptr) ? TRUE : FALSE;
		if (can_swim_species(riding_r_ptr) || is_aquatic_species(riding_r_ptr)) cr_ptr->can_swim = TRUE;

		if (!is_pass_wall_species(riding_r_ptr)) cr_ptr->pass_wall = FALSE;
		if (is_kill_wall_species(riding_r_ptr)) cr_ptr->kill_wall = TRUE;

		if (cr_ptr->skill_exp[GINOU_RIDING] < RIDING_EXP_SKILLED) j += (cr_ptr->wt * 3 * (RIDING_EXP_SKILLED - cr_ptr->skill_exp[GINOU_RIDING])) / RIDING_EXP_SKILLED;

		/* Extract the "weight limit" */
		i = 1500 + riding_r_ptr->level * 25;
	}

	/* XXX XXX XXX Apply "encumbrance" from weight */
	if (j > i) new_speed -= ((j - i) / (i / 5));

	/* Searching slows the player down */
	if (cr_ptr->action == ACTION_SEARCH) new_speed -= 10;

	/* Actual Modifier Bonuses (Un-inflate stat bonuses) */
	cr_ptr->to_a += ((int)(adj_dex_ta[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->to_d[0] += ((int)(adj_str_td[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->to_d[1] += ((int)(adj_str_td[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->to_d_m  += ((int)(adj_str_td[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->to_h[0] += ((int)(adj_dex_th[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->to_h[1] += ((int)(adj_dex_th[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->to_h_b  += ((int)(adj_dex_th[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->to_h_m  += ((int)(adj_dex_th[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->to_h[0] += ((int)(adj_str_th[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->to_h[1] += ((int)(adj_str_th[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->to_h_b  += ((int)(adj_str_th[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->to_h_m  += ((int)(adj_str_th[cr_ptr->stat_ind[STAT_STR]]) - 128);

	/* Displayed Modifier Bonuses (Un-inflate stat bonuses) */
	cr_ptr->dis_to_a += ((int)(adj_dex_ta[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->dis_to_d[0] += ((int)(adj_str_td[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->dis_to_d[1] += ((int)(adj_str_td[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->dis_to_h[0] += ((int)(adj_dex_th[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->dis_to_h[1] += ((int)(adj_dex_th[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->dis_to_h_b  += ((int)(adj_dex_th[cr_ptr->stat_ind[STAT_DEX]]) - 128);
	cr_ptr->dis_to_h[0] += ((int)(adj_str_th[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->dis_to_h[1] += ((int)(adj_str_th[cr_ptr->stat_ind[STAT_STR]]) - 128);
	cr_ptr->dis_to_h_b  += ((int)(adj_str_th[cr_ptr->stat_ind[STAT_STR]]) - 128);


	/* Obtain the "hold" value */
	hold = adj_str_hold[cr_ptr->stat_ind[STAT_STR]];


	/* Examine the "current bow" */
	o_ptr = &cr_ptr->inventory[INVEN_BOW];


	/* Assume not heavy */
	cr_ptr->heavy_shoot = FALSE;

	/* It is hard to carholdry a heavy bow */
	if (hold < o_ptr->weight / 10)
	{
		/* Hard to wield a heavy bow */
		cr_ptr->to_h_b  += 2 * (hold - o_ptr->weight / 10);
		cr_ptr->dis_to_h_b  += 2 * (hold - o_ptr->weight / 10);

		/* Heavy Bow */
		cr_ptr->heavy_shoot = TRUE;
	}


	/* Compute "extra shots" if needed */
	if (o_ptr->k_idx)
	{
		/* Analyze the launcher */
		switch (o_ptr->sval)
		{
			case SV_SLING:
			{
				cr_ptr->tval_ammo = TV_SHOT;
				break;
			}

			case SV_SHORT_BOW:
			case SV_LONG_BOW:
			case SV_NAMAKE_BOW:
			{
				cr_ptr->tval_ammo = TV_ARROW;
				break;
			}

			case SV_LIGHT_XBOW:
			case SV_HEAVY_XBOW:
			{
				cr_ptr->tval_ammo = TV_BOLT;
				break;
			}
			case SV_CRIMSON:
			{
				cr_ptr->tval_ammo = TV_NO_AMMO;
				break;
			}
		}

		/* Apply special flags */
		if (o_ptr->k_idx && !cr_ptr->heavy_shoot)
		{
			/* Extra shots */
			cr_ptr->num_fire += (extra_shots * 100);

			/* Hack -- Rangers love Bows */
			if ((cr_ptr->cls_idx == CLASS_RANGER) &&
			    (cr_ptr->tval_ammo == TV_ARROW))
			{
				cr_ptr->num_fire += (cr_ptr->lev * 4);
			}

			if ((cr_ptr->cls_idx == CLASS_CAVALRY) &&
			    (cr_ptr->tval_ammo == TV_ARROW))
			{
				cr_ptr->num_fire += (cr_ptr->lev * 3);
			}

			if (cr_ptr->cls_idx == CLASS_ARCHER)
			{
				if (cr_ptr->tval_ammo == TV_ARROW)
					cr_ptr->num_fire += ((cr_ptr->lev * 5)+50);
				else if ((cr_ptr->tval_ammo == TV_BOLT) || (cr_ptr->tval_ammo == TV_SHOT))
					cr_ptr->num_fire += (cr_ptr->lev * 4);
			}

			/*
			 * Addendum -- also "Reward" high level warriors,
			 * with _any_ missile weapon -- TY
			 */
			if (cr_ptr->cls_idx == CLASS_WARRIOR &&
			   (cr_ptr->tval_ammo <= TV_BOLT) &&
			   (cr_ptr->tval_ammo >= TV_SHOT))
			{
				cr_ptr->num_fire += (cr_ptr->lev * 2);
			}
			if ((cr_ptr->cls_idx == CLASS_ROGUE) &&
			    (cr_ptr->tval_ammo == TV_SHOT))
			{
				cr_ptr->num_fire += (cr_ptr->lev * 4);
			}

			/* Snipers love Cross bows */
			if ((cr_ptr->cls_idx == CLASS_SNIPER) &&
				(cr_ptr->tval_ammo == TV_BOLT))
			{
				cr_ptr->to_h_b += (10 + (cr_ptr->lev / 5));
				cr_ptr->dis_to_h_b += (10 + (cr_ptr->lev / 5));
			}
		}
	}

	if (cr_ptr->ryoute)
		hold *= 2;

	for(i = 0 ; i < 2 ; i++)
	{
		/* Examine the "main weapon" */
		o_ptr = &cr_ptr->inventory[INVEN_1STARM+i];

		object_flags(o_ptr, flgs);

		/* Assume not heavy */
		cr_ptr->heavy_wield[i] = FALSE;
		cr_ptr->icky_wield[i] = FALSE;
		cr_ptr->riding_wield[i] = FALSE;

		if (!have_weapon(cr_ptr, INVEN_1STARM+i)) {cr_ptr->num_blow[i]=1;continue;}
		/* It is hard to hold a heavy weapon */
		if (hold < o_ptr->weight / 10)
		{
			/* Hard to wield a heavy weapon */
			cr_ptr->to_h[i] += 2 * (hold - o_ptr->weight / 10);
			cr_ptr->dis_to_h[i] += 2 * (hold - o_ptr->weight / 10);

			/* Heavy weapon */
			cr_ptr->heavy_wield[i] = TRUE;
		}
		else if (cr_ptr->ryoute && (hold < o_ptr->weight/5)) omoi = TRUE;

		if ((i == 1) && (o_ptr->tval == TV_SWORD) && ((o_ptr->sval == SV_MAIN_GAUCHE) || (o_ptr->sval == SV_WAKIZASHI)))
		{
			cr_ptr->to_a += 5;
			cr_ptr->dis_to_a += 5;
		}

		/* Normal weapons */
		if (o_ptr->k_idx && !cr_ptr->heavy_wield[i])
		{
			int str_index, dex_index;

			int num = 0, wgt = 0, mul = 0, div = 0;

			/* Analyze the class */
			switch (cr_ptr->cls_idx)
			{
				/* Warrior */
				case CLASS_WARRIOR:
					num = 6; wgt = 70; mul = 5; break;

				/* Berserker */
				case CLASS_BERSERKER:
					num = 6; wgt = 70; mul = 7; break;

				/* Mage */
				case CLASS_MAGE:
				case CLASS_HIGH_MAGE:
				case CLASS_BLUE_MAGE:
					num = 3; wgt = 100; mul = 2; break;

				/* Priest, Mindcrafter, Magic-Eater */
				case CLASS_PRIEST:
				case CLASS_MAGIC_EATER:
				case CLASS_MINDCRAFTER:
					num = 5; wgt = 100; mul = 3; break;

				/* Rogue */
				case CLASS_ROGUE:
					num = 5; wgt = 40; mul = 3; break;

				/* Ranger */
				case CLASS_RANGER:
					num = 5; wgt = 70; mul = 4; break;

				/* Paladin */
				case CLASS_PALADIN:
				case CLASS_SAMURAI:
					num = 5; wgt = 70; mul = 4; break;

				/* Weaponsmith */
				case CLASS_SMITH:
					num = 5; wgt = 150; mul = 5; break;

				/* Warrior-Mage */
				case CLASS_WARRIOR_MAGE:
				case CLASS_RED_MAGE:
					num = 5; wgt = 70; mul = 3; break;

				/* Chaos Warrior */
				case CLASS_CHAOS_WARRIOR:
					num = 5; wgt = 70; mul = 4; break;

				/* Monk */
				case CLASS_MONK:
					num = 5; wgt = 60; mul = 3; break;

				/* Tourist */
				case CLASS_TOURIST:
					num = 4; wgt = 100; mul = 3; break;

				/* Imitator */
				case CLASS_IMITATOR:
					num = 5; wgt = 70; mul = 4; break;

				/* Beastmaster */
				case CLASS_BEASTMASTER:
					num = 5; wgt = 70; mul = 3; break;

				/* Cavalry */
				case CLASS_CAVALRY:
					if ((cr_ptr->riding) && (have_flag(flgs, TR_RIDING))) {num = 5; wgt = 70; mul = 4;}
					else {num = 5; wgt = 100; mul = 3;}
					break;

				/* Sorcerer */
				case CLASS_SORCERER:
					num = 1; wgt = 1; mul = 1; break;

				/* Archer, Bard */
				case CLASS_ARCHER:
				case CLASS_BARD:
					num = 4; wgt = 70; mul = 2; break;

				/* ForceTrainer */
				case CLASS_FORCETRAINER:
					num = 4; wgt = 60; mul = 2; break;

				/* Mirror Master, Sniper */
				case CLASS_MIRROR_MASTER:
				case CLASS_SNIPER:
					num = 3; wgt = 100; mul = 3; break;

				/* Ninja */
				case CLASS_NINJA:
					num = 4; wgt = 20; mul = 1; break;

				/* None */
				default:
					num = 4; wgt = 100; mul = 3; break;

			}

			/* Hex - extra mights gives +1 bonus to max blows */
			if (hex_spelling(cr_ptr, HEX_XTRA_MIGHT) || hex_spelling(cr_ptr, HEX_BUILDING)) { num++; wgt /= 2; mul += 2; }

			/* Enforce a minimum "weight" (tenth pounds) */
			div = ((o_ptr->weight < wgt) ? wgt : o_ptr->weight);

			/* Access the strength vs weight */
			str_index = (adj_str_blow[cr_ptr->stat_ind[STAT_STR]] * mul / div);

			if (cr_ptr->ryoute && !omoi) str_index++;
			if (cr_ptr->cls_idx == CLASS_NINJA) str_index = MAX(0, str_index-1);

			/* Maximal value */
			if (str_index > 11) str_index = 11;

			/* Index by dexterity */
			dex_index = (adj_dex_blow[cr_ptr->stat_ind[STAT_DEX]]);

			/* Maximal value */
			if (dex_index > 11) dex_index = 11;

			/* Use the blows table */
			cr_ptr->num_blow[i] = blows_table[str_index][dex_index];

			/* Maximal value */
			if (cr_ptr->num_blow[i] > num) cr_ptr->num_blow[i] = num;

			/* Add in the "bonus blows" */
			cr_ptr->num_blow[i] += extra_blows[i];


			if (cr_ptr->cls_idx == CLASS_WARRIOR) cr_ptr->num_blow[i] += (cr_ptr->lev / 40);
			else if (cr_ptr->cls_idx == CLASS_BERSERKER)
			{
				cr_ptr->num_blow[i] += (cr_ptr->lev / 23);
			}
			else if ((cr_ptr->cls_idx == CLASS_ROGUE) && (o_ptr->weight < 50) && (cr_ptr->stat_ind[STAT_DEX] >= 30)) cr_ptr->num_blow[i] ++;

			if (cr_ptr->special_defense & KATA_FUUJIN) cr_ptr->num_blow[i] -= 1;

			if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) cr_ptr->num_blow[i] = 1;


			/* Require at least one blow */
			if (cr_ptr->num_blow[i] < 1) cr_ptr->num_blow[i] = 1;

			/* Boost digging skill by weapon weight */
			cr_ptr->skill_dig += (o_ptr->weight / 10);
		}

		/* Assume okay */
		/* Priest weapon penalty for non-blessed edged weapons */
		if ((cr_ptr->cls_idx == CLASS_PRIEST) && (!(have_flag(flgs, TR_BLESSED))) &&
		    ((o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM)))
		{
			/* Reduce the real bonuses */
			cr_ptr->to_h[i] -= 2;
			cr_ptr->to_d[i] -= 2;

			/* Reduce the mental bonuses */
			cr_ptr->dis_to_h[i] -= 2;
			cr_ptr->dis_to_d[i] -= 2;

			/* Icky weapon */
			cr_ptr->icky_wield[i] = TRUE;
		}
		else if (cr_ptr->cls_idx == CLASS_BERSERKER)
		{
			cr_ptr->to_h[i] += cr_ptr->lev/5;
			cr_ptr->to_d[i] += cr_ptr->lev/6;
			cr_ptr->dis_to_h[i] += cr_ptr->lev/5;
			cr_ptr->dis_to_d[i] += cr_ptr->lev/6;
			if (((i == 0) && !cr_ptr->hidarite) || cr_ptr->ryoute)
			{
				cr_ptr->to_h[i] += cr_ptr->lev/5;
				cr_ptr->to_d[i] += cr_ptr->lev/6;
				cr_ptr->dis_to_h[i] += cr_ptr->lev/5;
				cr_ptr->dis_to_d[i] += cr_ptr->lev/6;
			}
		}
		else if (cr_ptr->cls_idx == CLASS_SORCERER)
		{
			if (!((o_ptr->tval == TV_HAFTED) && ((o_ptr->sval == SV_WIZSTAFF) || (o_ptr->sval == SV_NAMAKE_HAMMER))))
			{
				/* Reduce the real bonuses */
				cr_ptr->to_h[i] -= 200;
				cr_ptr->to_d[i] -= 200;

				/* Reduce the mental bonuses */
				cr_ptr->dis_to_h[i] -= 200;
				cr_ptr->dis_to_d[i] -= 200;

				/* Icky weapon */
				cr_ptr->icky_wield[i] = TRUE;
			}
			else
			{
				/* Reduce the real bonuses */
				cr_ptr->to_h[i] -= 30;
				cr_ptr->to_d[i] -= 10;

				/* Reduce the mental bonuses */
				cr_ptr->dis_to_h[i] -= 30;
				cr_ptr->dis_to_d[i] -= 10;
			}
		}
		/* Hex bonuses */
		if (cr_ptr->realm1 == REALM_HEX)
		{
			if (object_is_cursed(o_ptr))
			{
				if (o_ptr->curse_flags & (TRC_CURSED)) { cr_ptr->to_h[i] += 5; cr_ptr->dis_to_h[i] += 5; }
				if (o_ptr->curse_flags & (TRC_HEAVY_CURSE)) { cr_ptr->to_h[i] += 7; cr_ptr->dis_to_h[i] += 7; }
				if (o_ptr->curse_flags & (TRC_DIVINE_CURSE)) { cr_ptr->to_h[i] += 13; cr_ptr->dis_to_h[i] += 13; }
				if (o_ptr->curse_flags & (TRC_TY_CURSE)) { cr_ptr->to_h[i] += 5; cr_ptr->dis_to_h[i] += 5; }
				if (hex_spelling(cr_ptr, HEX_RUNESWORD))
				{
					if (o_ptr->curse_flags & (TRC_CURSED)) { cr_ptr->to_d[i] += 5; cr_ptr->dis_to_d[i] += 5; }
					if (o_ptr->curse_flags & (TRC_HEAVY_CURSE)) { cr_ptr->to_d[i] += 7; cr_ptr->dis_to_d[i] += 7; }
					if (o_ptr->curse_flags & (TRC_DIVINE_CURSE)) { cr_ptr->to_d[i] += 13; cr_ptr->dis_to_d[i] += 13; }
				}
			}
		}
		if (cr_ptr->riding)
		{
			if ((o_ptr->tval == TV_POLEARM) && ((o_ptr->sval == SV_LANCE) || (o_ptr->sval == SV_HEAVY_LANCE)))
			{
				cr_ptr->to_h[i] +=15;
				cr_ptr->dis_to_h[i] +=15;
				cr_ptr->to_dd[i] += 2;
			}
			else if (!(have_flag(flgs, TR_RIDING)))
			{
				int penalty;
				if ((cr_ptr->cls_idx == CLASS_BEASTMASTER) || (cr_ptr->cls_idx == CLASS_CAVALRY))
				{
					penalty = 5;
				}
				else
				{
					penalty = species_info[creature_list[cr_ptr->riding].species_idx].level - cr_ptr->skill_exp[GINOU_RIDING] / 80;
					penalty += 30;
					if (penalty < 30) penalty = 30;
				}
				cr_ptr->to_h[i] -= penalty;
				cr_ptr->dis_to_h[i] -= penalty;

				/* Riding weapon */
				cr_ptr->riding_wield[i] = TRUE;
			}
		}
	}

	if (cr_ptr->riding)
	{
		int penalty = 0;

		cr_ptr->riding_ryoute = FALSE;

		if (cr_ptr->ryoute || (empty_hands(cr_ptr, FALSE) == EMPTY_HAND_NONE)) cr_ptr->riding_ryoute = TRUE;
		else if (cr_ptr->pet_extra_flags & PF_RYOUTE)
		{
			switch (cr_ptr->cls_idx)
			{
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
			case CLASS_BERSERKER:
				if ((empty_hands(cr_ptr, FALSE) != EMPTY_HAND_NONE) && !have_weapon(cr_ptr, INVEN_1STARM) && !have_weapon(cr_ptr, INVEN_2NDARM))
					cr_ptr->riding_ryoute = TRUE;
				break;
			}
		}

		if ((cr_ptr->cls_idx == CLASS_BEASTMASTER) || (cr_ptr->cls_idx == CLASS_CAVALRY))
		{
			if (cr_ptr->tval_ammo != TV_ARROW) penalty = 5;
		}
		else
		{
			penalty = species_info[creature_list[cr_ptr->riding].species_idx].level - cr_ptr->skill_exp[GINOU_RIDING] / 80;
			penalty += 30;
			if (penalty < 30) penalty = 30;
		}
		if (cr_ptr->tval_ammo == TV_BOLT) penalty *= 2;
		cr_ptr->to_h_b -= penalty;
		cr_ptr->dis_to_h_b -= penalty;
	}

	/* Different calculation for monks with empty hands */
	if (((cr_ptr->cls_idx == CLASS_MONK) || (cr_ptr->cls_idx == CLASS_FORCETRAINER) || (cr_ptr->cls_idx == CLASS_BERSERKER)) &&
		(empty_hands_status & EMPTY_HAND_RARM) && !cr_ptr->hidarite)
	{
		int blow_base = cr_ptr->lev + adj_dex_blow[cr_ptr->stat_ind[STAT_DEX]];
		cr_ptr->num_blow[0] = 0;

		if (cr_ptr->cls_idx == CLASS_FORCETRAINER)
		{
			if (blow_base > 18) cr_ptr->num_blow[0]++;
			if (blow_base > 31) cr_ptr->num_blow[0]++;
			if (blow_base > 44) cr_ptr->num_blow[0]++;
			if (blow_base > 58) cr_ptr->num_blow[0]++;
			if (cr_ptr->magic_num1[0])
			{
				cr_ptr->to_d[0] += (s16b)(cr_ptr->magic_num1[0]/5);
				cr_ptr->dis_to_d[0] += (s16b)(cr_ptr->magic_num1[0]/5);
			}
		}
		else
		{
			if (blow_base > 12) cr_ptr->num_blow[0]++;
			if (blow_base > 22) cr_ptr->num_blow[0]++;
			if (blow_base > 31) cr_ptr->num_blow[0]++;
			if (blow_base > 39) cr_ptr->num_blow[0]++;
			if (blow_base > 46) cr_ptr->num_blow[0]++;
			if (blow_base > 53) cr_ptr->num_blow[0]++;
			if (blow_base > 59) cr_ptr->num_blow[0]++;
		}

		if (heavy_armor(cr_ptr) && (cr_ptr->cls_idx != CLASS_BERSERKER))
			cr_ptr->num_blow[0] /= 2;
		else
		{
			cr_ptr->to_h[0] += (cr_ptr->lev / 3);
			cr_ptr->dis_to_h[0] += (cr_ptr->lev / 3);

			cr_ptr->to_d[0] += (cr_ptr->lev / 6);
			cr_ptr->dis_to_d[0] += (cr_ptr->lev / 6);
		}

		if (cr_ptr->special_defense & KAMAE_BYAKKO)
		{
			cr_ptr->to_a -= 40;
			cr_ptr->dis_to_a -= 40;
			
		}
		else if (cr_ptr->special_defense & KAMAE_SEIRYU)
		{
			cr_ptr->to_a -= 50;
			cr_ptr->dis_to_a -= 50;
			cr_ptr->resist_acid = TRUE;
			cr_ptr->resist_fire = TRUE;
			cr_ptr->resist_elec = TRUE;
			cr_ptr->resist_cold = TRUE;
			cr_ptr->resist_pois = TRUE;
			cr_ptr->sh_fire = TRUE;
			cr_ptr->sh_elec = TRUE;
			cr_ptr->sh_cold = TRUE;
			cr_ptr->levitation = TRUE;
		}
		else if (cr_ptr->special_defense & KAMAE_GENBU)
		{
			cr_ptr->to_a += (cr_ptr->lev*cr_ptr->lev)/50;
			cr_ptr->dis_to_a += (cr_ptr->lev*cr_ptr->lev)/50;
			cr_ptr->reflect = TRUE;
			cr_ptr->num_blow[0] -= 2;
			if ((cr_ptr->cls_idx == CLASS_MONK) && (cr_ptr->lev > 42)) cr_ptr->num_blow[0]--;
			if (cr_ptr->num_blow[0] < 0) cr_ptr->num_blow[0] = 0;
		}
		else if (cr_ptr->special_defense & KAMAE_SUZAKU)
		{
			cr_ptr->to_h[0] -= (cr_ptr->lev / 3);
			cr_ptr->to_d[0] -= (cr_ptr->lev / 6);

			cr_ptr->dis_to_h[0] -= (cr_ptr->lev / 3);
			cr_ptr->dis_to_d[0] -= (cr_ptr->lev / 6);
			cr_ptr->num_blow[0] /= 2;
			cr_ptr->levitation = TRUE;
		}

		cr_ptr->num_blow[0] += 1+extra_blows[0];
	}

	if (cr_ptr->riding) cr_ptr->levitation = riding_levitation;

	monk_armour_aux = FALSE;

	if (heavy_armor(cr_ptr))
	{
		monk_armour_aux = TRUE;
	}

	for (i = 0; i < 2; i++)
	{
		if (have_weapon(cr_ptr, INVEN_1STARM+i))
		{
			int tval = cr_ptr->inventory[INVEN_1STARM+i].tval - TV_WEAPON_BEGIN;
			int sval = cr_ptr->inventory[INVEN_1STARM+i].sval;
			int boost = (cr_ptr->weapon_exp[tval][sval] - WEAPON_EXP_BEGINNER) / 200;

			cr_ptr->to_h[i] += boost > 0 ? boost : 0;
			cr_ptr->dis_to_h[i] += boost > 0 ? boost : 0;

			if ((cr_ptr->cls_idx == CLASS_MONK) || (cr_ptr->cls_idx == CLASS_FORCETRAINER))
			{
				if (!s_info[cr_ptr->cls_idx].w_max[tval][sval])
				{
					cr_ptr->to_h[i] -= 40;
					cr_ptr->dis_to_h[i] -= 40;
					cr_ptr->icky_wield[i] = TRUE;
				}
			}
			else if (cr_ptr->cls_idx == CLASS_NINJA)
			{
				if ((s_info[CLASS_NINJA].w_max[tval][sval] <= WEAPON_EXP_BEGINNER) || (cr_ptr->inventory[INVEN_2NDARM-i].tval == TV_SHIELD))
				{
					cr_ptr->to_h[i] -= 40;
					cr_ptr->dis_to_h[i] -= 40;
					cr_ptr->icky_wield[i] = TRUE;
					cr_ptr->num_blow[i] /= 2;
					if (cr_ptr->num_blow[i] < 1) cr_ptr->num_blow[i] = 1;
				}
			}

			if (cr_ptr->inventory[INVEN_1STARM + i].name1 == ART_IRON_BALL) cr_ptr->good -= 1000;
		}
	}

	/* Maximum speed is (+99). (internally it's 110 + 99) */
	/* Temporary lightspeed forces to be maximum speed */
	if ((cr_ptr->lightspeed && !cr_ptr->riding) || (new_speed > 209))
	{
		new_speed = 209;
	}

	/* Minimum speed is (-99). (internally it's 110 - 99) */
	if (new_speed < 11) new_speed = 11;

	/* Display the speed (if needed) */
	if (cr_ptr->speed != (byte)new_speed)
	{
		cr_ptr->speed = (byte)new_speed;
		play_redraw |= (PR_SPEED);
	}

	if (yoiyami)
	{
		if (cr_ptr->to_a > (0 - cr_ptr->ac))
			cr_ptr->to_a = 0 - cr_ptr->ac;
		if (cr_ptr->dis_to_a > (0 - cr_ptr->dis_ac))
			cr_ptr->dis_to_a = 0 - cr_ptr->dis_ac;
	}

	/* Redraw armor (if needed) */
	if ((cr_ptr->dis_ac != old_dis_ac) || (cr_ptr->dis_to_a != old_dis_to_a))
	{
		/* Redraw */
		play_redraw |= (PR_ARMOR);

		/* Window stuff */
		play_window |= (PW_PLAYER);
	}


	if (cr_ptr->ryoute && !omoi)
	{
		int bonus_to_h=0, bonus_to_d=0;
		bonus_to_d = ((int)(adj_str_td[cr_ptr->stat_ind[STAT_STR]]) - 128)/2;
		bonus_to_h = ((int)(adj_str_th[cr_ptr->stat_ind[STAT_STR]]) - 128) + ((int)(adj_dex_th[cr_ptr->stat_ind[STAT_DEX]]) - 128);

		cr_ptr->to_h[default_hand] += MAX(bonus_to_h,1);
		cr_ptr->dis_to_h[default_hand] += MAX(bonus_to_h,1);
		cr_ptr->to_d[default_hand] += MAX(bonus_to_d,1);
		cr_ptr->dis_to_d[default_hand] += MAX(bonus_to_d,1);
	}

	if (((cr_ptr->cls_idx == CLASS_MONK) || (cr_ptr->cls_idx == CLASS_FORCETRAINER) || (cr_ptr->cls_idx == CLASS_BERSERKER)) && (empty_hands(cr_ptr, FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))) cr_ptr->ryoute = FALSE;

	/* Affect Skill -- stealth (bonus one) */
	cr_ptr->skill_stl += 1;

	if (IS_TIM_STEALTH(cr_ptr)) cr_ptr->skill_stl += 99;

	/* Affect Skill -- disarming (DEX and INT) */
	cr_ptr->skill_dis += adj_dex_dis[cr_ptr->stat_ind[STAT_DEX]];
	cr_ptr->skill_dis += adj_int_dis[cr_ptr->stat_ind[STAT_INT]];

	/* Affect Skill -- magic devices (INT) */
	cr_ptr->skill_dev += adj_int_dev[cr_ptr->stat_ind[STAT_INT]];

	/* Affect Skill -- saving throw (WIS) */
	cr_ptr->skill_rob += (adj_sav[cr_ptr->stat_ind[STAT_STR]] + adj_sav[cr_ptr->stat_ind[STAT_CON]]);
	cr_ptr->skill_eva += (adj_sav[cr_ptr->stat_ind[STAT_INT]] + adj_sav[cr_ptr->stat_ind[STAT_DEX]]);
	cr_ptr->skill_vol += (adj_sav[cr_ptr->stat_ind[STAT_WIS]] + adj_sav[cr_ptr->stat_ind[STAT_CHR]]);

	/* Affect Skill -- digging (STR) */
	cr_ptr->skill_dig += adj_str_dig[cr_ptr->stat_ind[STAT_STR]];

	/* Affect Skill -- disarming (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_dis += (class_info[cr_ptr->cls_idx].x_dis * cr_ptr->lev / 10);
	else cr_ptr->skill_dis += (class_info[CLASS_TOURIST].x_dis * cr_ptr->lev / 10);

	if(cr_ptr->chara_idx != CHARA_NONE) cr_ptr->skill_dis += (chara_info[cr_ptr->chara_idx].a_dis * cr_ptr->lev / 50);

	/* Affect Skill -- magic devices (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_dev += (class_info[cr_ptr->cls_idx].x_dev * cr_ptr->lev / 10);
	else cr_ptr->skill_dev += (class_info[CLASS_TOURIST].x_dev * cr_ptr->lev / 10);

	if(cr_ptr->chara_idx != CHARA_NONE) cr_ptr->skill_dev += (chara_info[cr_ptr->chara_idx].a_dev * cr_ptr->lev / 50);

	/* Affect Skill -- saving throw (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_rob += (class_info[cr_ptr->cls_idx].x_sav * cr_ptr->lev / 10);
	else cr_ptr->skill_rob += (class_info[CLASS_TOURIST].x_sav * cr_ptr->lev / 10);
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_eva += (class_info[cr_ptr->cls_idx].x_sav * cr_ptr->lev / 10);
	else cr_ptr->skill_rob += (class_info[CLASS_TOURIST].x_sav * cr_ptr->lev / 10);
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_vol += (class_info[cr_ptr->cls_idx].x_sav * cr_ptr->lev / 10);
	else cr_ptr->skill_rob += (class_info[CLASS_TOURIST].x_sav * cr_ptr->lev / 10);

	if(cr_ptr->chara_idx != CHARA_NONE) cr_ptr->skill_rob += (chara_info[cr_ptr->chara_idx].a_sav * cr_ptr->lev / 50);
	if(cr_ptr->chara_idx != CHARA_NONE) cr_ptr->skill_eva += (chara_info[cr_ptr->chara_idx].a_sav * cr_ptr->lev / 50);
	if(cr_ptr->chara_idx != CHARA_NONE) cr_ptr->skill_vol += (chara_info[cr_ptr->chara_idx].a_sav * cr_ptr->lev / 50);

	/* Affect Skill -- stealth (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_stl += (class_info[cr_ptr->cls_idx].x_stl * cr_ptr->lev / 10);
	else cr_ptr->skill_stl += (class_info[CLASS_TOURIST].x_stl * cr_ptr->lev / 10);

	/* Affect Skill -- search ability (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_srh += (class_info[cr_ptr->cls_idx].x_srh * cr_ptr->lev / 10);
	else cr_ptr->skill_srh += (class_info[CLASS_TOURIST].x_srh * cr_ptr->lev / 10);

	/* Affect Skill -- search frequency (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_fos += (class_info[cr_ptr->cls_idx].x_fos * cr_ptr->lev / 10);
	else  cr_ptr->skill_fos += (class_info[CLASS_TOURIST].x_fos * cr_ptr->lev / 10);

	/* Affect Skill -- combat (normal) (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_thn += (class_info[cr_ptr->cls_idx].x_thn * cr_ptr->lev / 10);
	else cr_ptr->skill_thn += (class_info[CLASS_TOURIST].x_thn * cr_ptr->lev / 10);
	if(cr_ptr->chara_idx != CHARA_NONE) cr_ptr->skill_thn += (chara_info[cr_ptr->chara_idx].a_thn * cr_ptr->lev / 50);

	/* Affect Skill -- combat (shooting) (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_thb += (class_info[cr_ptr->cls_idx].x_thb * cr_ptr->lev / 10);
	else cr_ptr->skill_thb += (class_info[CLASS_TOURIST].x_thb * cr_ptr->lev / 10);
	if(cr_ptr->chara_idx != CHARA_NONE) cr_ptr->skill_thb += (chara_info[cr_ptr->chara_idx].a_thb * cr_ptr->lev / 50);

	/* Affect Skill -- combat (throwing) (Level, by Class) */
	if(cr_ptr->cls_idx != CLASS_NONE) cr_ptr->skill_tht += (class_info[cr_ptr->cls_idx].x_thb * cr_ptr->lev / 10);
	else cr_ptr->skill_tht += (class_info[CLASS_TOURIST].x_thb * cr_ptr->lev / 10);
	if(cr_ptr->chara_idx != CHARA_NONE) cr_ptr->skill_tht += (chara_info[cr_ptr->chara_idx].a_thb * cr_ptr->lev / 50);


	if ((race_is_(cr_ptr, RACE_S_FAIRY)) && (cr_ptr->chara_idx != CHARA_SEXY) && (cr_ptr->cursed & TRC_AGGRAVATE))
	{
		cr_ptr->cursed &= ~(TRC_AGGRAVATE);
		cr_ptr->skill_stl = MIN(cr_ptr->skill_stl - 3, (cr_ptr->skill_stl + 2) / 2);
	}

	/* Limit Skill -- stealth from 0 to 30 */
	if (cr_ptr->skill_stl > 30) cr_ptr->skill_stl = 30;
	if (cr_ptr->skill_stl < 0) cr_ptr->skill_stl = 0;

	/* Limit Skill -- digging from 1 up */
	if (cr_ptr->skill_dig < 1) cr_ptr->skill_dig = 1;

	if (cr_ptr->anti_magic && (cr_ptr->skill_rob < (90 + cr_ptr->lev))) cr_ptr->skill_rob = 90 + cr_ptr->lev;
	if (cr_ptr->anti_magic && (cr_ptr->skill_eva < (90 + cr_ptr->lev))) cr_ptr->skill_eva = 90 + cr_ptr->lev;
	if (cr_ptr->anti_magic && (cr_ptr->skill_vol < (90 + cr_ptr->lev))) cr_ptr->skill_vol = 90 + cr_ptr->lev;

	if (cr_ptr->tsubureru) cr_ptr->skill_rob = 10;
	if (cr_ptr->tsubureru) cr_ptr->skill_eva = 10;
	if (cr_ptr->tsubureru) cr_ptr->skill_vol = 10;

	if ((cr_ptr->ult_res || cr_ptr->resist_magic || cr_ptr->magicdef) && (cr_ptr->skill_rob < (95 + cr_ptr->lev))) cr_ptr->skill_rob = 95 + cr_ptr->lev;
	if ((cr_ptr->ult_res || cr_ptr->resist_magic || cr_ptr->magicdef) && (cr_ptr->skill_eva < (95 + cr_ptr->lev))) cr_ptr->skill_eva = 95 + cr_ptr->lev;
	if ((cr_ptr->ult_res || cr_ptr->resist_magic || cr_ptr->magicdef) && (cr_ptr->skill_vol < (95 + cr_ptr->lev))) cr_ptr->skill_vol = 95 + cr_ptr->lev;

	if (down_saving) cr_ptr->skill_rob /= 2;
	if (down_saving) cr_ptr->skill_eva /= 2;
	if (down_saving) cr_ptr->skill_vol /= 2;

	/* Hack -- Each elemental immunity includes resistance */
	if (cr_ptr->immune_acid) cr_ptr->resist_acid = TRUE;
	if (cr_ptr->immune_elec) cr_ptr->resist_elec = TRUE;
	if (cr_ptr->immune_fire) cr_ptr->resist_fire = TRUE;
	if (cr_ptr->immune_cold) cr_ptr->resist_cold = TRUE;

	/* Set Possible Equipment*/
	if(cr_ptr->race_idx1 != RACE_NONE)
		cr_ptr->possible_equipment = race_info[cr_ptr->race_idx1].possible_equipment;
	else
		cr_ptr->possible_equipment = 0;



	/* calc karmas and bonuses */
	for(i = 0; i < MAX_KARMA; i++)
		cr_ptr->karmas_cur[i] = cr_ptr->karmas_cur[i];

	for(i = 0; i < MAX_KARMA; i++)
		cr_ptr->karmas_rank[i] = calc_rank(cr_ptr->karmas_cur[i]);


	/* calc alignments and bonuses */
	if(cr_ptr->race_idx1 != RACE_NONE)
	{
		cr_ptr->good    = tmp_rcr_ptr->good;
		cr_ptr->evil    = tmp_rcr_ptr->evil;
		cr_ptr->order   = tmp_rcr_ptr->order;
		cr_ptr->chaos   = tmp_rcr_ptr->chaos;
		cr_ptr->balance = tmp_rcr_ptr->balance;
	}
	else
	{
		cr_ptr->good    = 0;
		cr_ptr->evil    = 0;
		cr_ptr->order   = 0;
		cr_ptr->chaos   = 0;
		cr_ptr->balance = 0;
	}

	for(i = 0; i < MAX_KARMA; i++)
	{
		cr_ptr->good += cr_ptr->karmas[i] * karma[i].good_adj;
		cr_ptr->evil += cr_ptr->karmas[i] * karma[i].evil_adj;
		cr_ptr->order += cr_ptr->karmas[i] * karma[i].order_adj;
		cr_ptr->chaos += cr_ptr->karmas[i] * karma[i].chaos_adj;
	}

	cr_ptr->good_rank = calc_rank(cr_ptr->good);
	cr_ptr->evil_rank = calc_rank(cr_ptr->evil);
	cr_ptr->order_rank = calc_rank(cr_ptr->order);
	cr_ptr->chaos_rank = calc_rank(cr_ptr->chaos);

	cr_ptr->balance -= ((cr_ptr->good_rank - cr_ptr->evil_rank) * (cr_ptr->good_rank - cr_ptr->evil_rank) - 100) * 10;
	cr_ptr->balance -= ((cr_ptr->order_rank - cr_ptr->chaos_rank) * (cr_ptr->order_rank - cr_ptr->chaos_rank) - 100) * 10;

	cr_ptr->balance_rank = calc_rank(cr_ptr->balance);




	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	/* Take note when "heavy bow" changes */
	if (cr_ptr->old_heavy_shoot != cr_ptr->heavy_shoot)
	{
		/* Message */
		if (cr_ptr->heavy_shoot)
		{
#ifdef JP
			if(message) msg_print("こんな重い弓を装備しているのは大変だ。");
#else
			if(message) msg_print("You have trouble wielding such a heavy bow.");
#endif

		}
		else if (cr_ptr->inventory[INVEN_BOW].k_idx)
		{
#ifdef JP
			if(message) msg_print("この弓なら装備していても辛くない。");
#else
			if(message) msg_print("You have no trouble wielding your bow.");
#endif

		}
		else
		{
#ifdef JP
			if(message) msg_print("重い弓を装備からはずして体が楽になった。");
#else
			if(message) msg_print("You feel relieved to put down your heavy bow.");
#endif

		}

		/* Save it */
		cr_ptr->old_heavy_shoot = cr_ptr->heavy_shoot;
	}

	for (i = 0 ; i < 2 ; i++)
	{
		/* Take note when "heavy weapon" changes */
		if (cr_ptr->old_heavy_wield[i] != cr_ptr->heavy_wield[i])
		{
			/* Message */
			if (cr_ptr->heavy_wield[i])
			{
#ifdef JP
				if(message) msg_print("こんな重い武器を装備しているのは大変だ。");
#else
				if(message) msg_print("You have trouble wielding such a heavy weapon.");
#endif

			}
			else if (have_weapon(cr_ptr, INVEN_1STARM+i))
			{
#ifdef JP
				if(message) msg_print("これなら装備していても辛くない。");
#else
				if(message) msg_print("You have no trouble wielding your weapon.");
#endif

			}
			else if (cr_ptr->heavy_wield[1-i])
			{
#ifdef JP
				if(message) msg_print("まだ武器が重い。");
#else
				if(message) msg_print("You have still trouble wielding a heavy weapon.");
#endif

			}
			else
			{
#ifdef JP
				if(message) msg_print("重い武器を装備からはずして体が楽になった。");
#else
				if(message) msg_print("You feel relieved to put down your heavy weapon.");
#endif

			}

			/* Save it */
			cr_ptr->old_heavy_wield[i] = cr_ptr->heavy_wield[i];
		}

		/* Take note when "heavy weapon" changes */
		if (cr_ptr->old_riding_wield[i] != cr_ptr->riding_wield[i])
		{
			/* Message */
			if (cr_ptr->riding_wield[i])
			{
#ifdef JP
				if(message) msg_print("この武器は乗馬中に使うにはむかないようだ。");
#else
				if(message) msg_print("This weapon is not suitable for use while riding.");
#endif

			}
			else if (!cr_ptr->riding)
			{
#ifdef JP
				if(message) msg_print("この武器は徒歩で使いやすい。");
#else
				if(message) msg_print("This weapon was not suitable for use while riding.");
#endif

			}
			else if (have_weapon(cr_ptr, INVEN_1STARM+i))
			{
#ifdef JP
				if(message) msg_print("これなら乗馬中にぴったりだ。");
#else
				if(message) msg_print("This weapon is suitable for use while riding.");
#endif

			}
			/* Save it */
			cr_ptr->old_riding_wield[i] = cr_ptr->riding_wield[i];
		}

		/* Take note when "illegal weapon" changes */
		if (cr_ptr->old_icky_wield[i] != cr_ptr->icky_wield[i])
		{
			/* Message */
			if (cr_ptr->icky_wield[i])
			{
#ifdef JP
				if(message) msg_print("今の装備はどうも自分にふさわしくない気がする。");
#else
				if(message) msg_print("You do not feel comfortable with your weapon.");
#endif
			}
			else if (have_weapon(cr_ptr, INVEN_1STARM+i))
			{
#ifdef JP
				if(message) msg_print("今の装備は自分にふさわしい気がする。");
#else
				if(message) msg_print("You feel comfortable with your weapon.");
#endif

			}
			else
			{
#ifdef JP
				if(message) msg_print("装備をはずしたら随分と気が楽になった。");
#else
				if(message) msg_print("You feel more comfortable after removing your weapon.");
#endif

			}

			/* Save it */
			cr_ptr->old_icky_wield[i] = cr_ptr->icky_wield[i];
		}
	}

	if (cr_ptr->riding && (cr_ptr->old_riding_ryoute != cr_ptr->riding_ryoute))
	{
		/* Message */
		if (cr_ptr->riding_ryoute)
		{
#ifdef JP
			if(message) msg_format("%s馬を操れない。", (empty_hands(cr_ptr, FALSE) == EMPTY_HAND_NONE) ? "両手がふさがっていて" : "");
#else
			if(message) msg_print("You are using both hand for fighting, and you can't control a riding pet.");
#endif
		}
		else
		{
#ifdef JP
			if(message) msg_format("%s馬を操れるようになった。", (empty_hands(cr_ptr, FALSE) == EMPTY_HAND_NONE) ? "手が空いて" : "");
#else
			if(message) msg_print("You began to control riding pet with one hand.");
#endif
		}

		cr_ptr->old_riding_ryoute = cr_ptr->riding_ryoute;
	}

	if (((cr_ptr->cls_idx == CLASS_MONK) || (cr_ptr->cls_idx == CLASS_FORCETRAINER) || (cr_ptr->cls_idx == CLASS_NINJA)) && (monk_armour_aux != monk_notify_aux))
	{
		if (heavy_armor(cr_ptr))
		{
#ifdef JP
            if(message) msg_print("装備が重くてバランスを取れない。");
#else
			if(message) msg_print("The weight of your armor disrupts your balance.");
#endif
		}
		else
#ifdef JP
            if(message) msg_print("バランスがとれるようになった。");
#else
			if(message) msg_print("You regain your balance.");
#endif

		monk_notify_aux = monk_armour_aux;
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
#if 0
		if ((cr_ptr->inventory[i].tval == TV_SORCERY_BOOK) && (cr_ptr->inventory[i].sval == 2)) have_dd_s = TRUE;
		if ((cr_ptr->inventory[i].tval == TV_TRUMP_BOOK) && (cr_ptr->inventory[i].sval == 1)) have_dd_t = TRUE;
#endif
		if ((cr_ptr->inventory[i].tval == TV_NATURE_BOOK) && (cr_ptr->inventory[i].sval == 2)) have_sw = TRUE;
		if ((cr_ptr->inventory[i].tval == TV_CRAFT_BOOK) && (cr_ptr->inventory[i].sval == 2)) have_kabe = TRUE;
	}
	for (this_o_idx = cave[cr_ptr->fy][cr_ptr->fx].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

#if 0
		if ((o_ptr->tval == TV_SORCERY_BOOK) && (o_ptr->sval == 3)) have_dd_s = TRUE;
		if ((o_ptr->tval == TV_TRUMP_BOOK) && (o_ptr->sval == 1)) have_dd_t = TRUE;
#endif
		if ((o_ptr->tval == TV_NATURE_BOOK) && (o_ptr->sval == 2)) have_sw = TRUE;
		if ((o_ptr->tval == TV_CRAFT_BOOK) && (o_ptr->sval == 2)) have_kabe = TRUE;
	}

	if (cr_ptr->pass_wall && !cr_ptr->kill_wall) cr_ptr->no_flowed = TRUE;
#if 0
	if (have_dd_s && ((cr_ptr->realm1 == REALM_SORCERY) || (cr_ptr->realm2 == REALM_SORCERY) || (cr_ptr->cls_idx == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &m_info[cr_ptr->sex].info[REALM_SORCERY-1][SPELL_DD_S];
		if (cr_ptr->lev >= s_ptr->slevel) cr_ptr->no_flowed = TRUE;
	}

	if (have_dd_t && ((cr_ptr->realm1 == REALM_TRUMP) || (cr_ptr->realm2 == REALM_TRUMP) || (cr_ptr->cls_idx == CLASS_SORCERER) || (cr_ptr->cls_idx == CLASS_RED_MAGE)))
	{
		magic_type *s_ptr = &m_info[cr_ptr->sex].info[REALM_TRUMP-1][SPELL_DD_T];
		if (cr_ptr->lev >= s_ptr->slevel) cr_ptr->no_flowed = TRUE;
	}
#endif
	if (have_sw && ((cr_ptr->realm1 == REALM_NATURE) || (cr_ptr->realm2 == REALM_NATURE) || (cr_ptr->cls_idx == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &m_info[cr_ptr->sex].info[REALM_NATURE-1][SPELL_SW];
		if (cr_ptr->lev >= s_ptr->slevel) cr_ptr->no_flowed = TRUE;
	}

	if (have_kabe && ((cr_ptr->realm1 == REALM_CRAFT) || (cr_ptr->realm2 == REALM_CRAFT) || (cr_ptr->cls_idx == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &m_info[cr_ptr->sex].info[REALM_CRAFT-1][SPELL_KABE];
		if (cr_ptr->lev >= s_ptr->slevel) cr_ptr->no_flowed = TRUE;
	}

}



/*
 * Handle "cr_ptr->notice"
 */
void notice_stuff(creature_type *cr_ptr)
{
	/* Notice stuff */
	if (!cr_ptr->notice) return;


	/* Actually do auto-destroy */
	if (cr_ptr->notice & (PN_AUTODESTROY))
	{
		cr_ptr->notice &= ~(PN_AUTODESTROY);
		autopick_delayed_alter(cr_ptr);
	}

	/* Combine the pack */
	if (cr_ptr->notice & (PN_COMBINE))
	{
		cr_ptr->notice &= ~(PN_COMBINE);
		combine_pack();
	}

	/* Reorder the pack */
	if (cr_ptr->notice & (PN_REORDER))
	{
		cr_ptr->notice &= ~(PN_REORDER);
		reorder_pack();
	}
}


/*
 * Handle "cr_ptr->update"
 */
void update_stuff(creature_type *cr_ptr, bool message)
{
	/* Update stuff */
	if (!cr_ptr->update) return;

	if (cr_ptr->update & (PU_BONUS))
	{
		cr_ptr->update &= ~(PU_BONUS);
		calc_bonuses(cr_ptr, message);
	}

	if (cr_ptr->update & (PU_TORCH))
	{
		cr_ptr->update &= ~(PU_TORCH);
		calc_torch(cr_ptr);
	}

	if (cr_ptr->update & (PU_HP))
	{
		cr_ptr->update &= ~(PU_HP);
		calc_hitpoints(cr_ptr, message);
	}

	if (cr_ptr->update & (PU_MANA))
	{
		cr_ptr->update &= ~(PU_MANA);
		calc_mana(cr_ptr, message);
	}

	if (cr_ptr->update & (PU_SPELLS))
	{
		cr_ptr->update &= ~(PU_SPELLS);
		calc_spells(cr_ptr, message);
	}


	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;


	/* Character is in "icky" mode, no screen updates */
	if (character_icky) return;


	if (cr_ptr->update & (PU_UN_LITE))
	{
		cr_ptr->update &= ~(PU_UN_LITE);
		forget_lite();
	}

	if (cr_ptr->update & (PU_UN_VIEW))
	{
		cr_ptr->update &= ~(PU_UN_VIEW);
		forget_view();
	}

	if (cr_ptr->update & (PU_VIEW))
	{
		cr_ptr->update &= ~(PU_VIEW);
		update_view(cr_ptr);
	}

	if (cr_ptr->update & (PU_LITE))
	{
		cr_ptr->update &= ~(PU_LITE);
		update_lite(cr_ptr);
	}


	if (cr_ptr->update & (PU_FLOW))
	{
		cr_ptr->update &= ~(PU_FLOW);
		update_flow();
	}

	if (cr_ptr->update & (PU_DISTANCE))
	{
		cr_ptr->update &= ~(PU_DISTANCE);

		/* Still need to call update_monsters(FALSE) after update_mon_lite() */ 
		/* cr_ptr->update &= ~(PU_MONSTERS); */

		update_monsters(TRUE);
	}

	if (cr_ptr->update & (PU_MON_LITE))
	{
		cr_ptr->update &= ~(PU_MON_LITE);
		update_mon_lite(cr_ptr);
	}

	/*
	 * Mega-Hack -- Delayed visual update
	 * Only used if update_view(), update_lite() or update_mon_lite() was called
	 */
	if (cr_ptr->update & (PU_DELAY_VIS))
	{
		cr_ptr->update &= ~(PU_DELAY_VIS);
		delayed_visual_update();
	}

	if (cr_ptr->update & (PU_MONSTERS))
	{
		cr_ptr->update &= ~(PU_MONSTERS);
		update_monsters(FALSE);
	}
}


/*
 * Handle "play_redraw"
 */
void redraw_stuff(creature_type *cr_ptr)
{
	/* Redraw stuff */
	if (!play_redraw) return;


	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;


	/* Character is in "icky" mode, no screen updates */
	if (character_icky) return;



	/* Hack -- clear the screen */
	if (play_redraw & (PR_WIPE))
	{
		play_redraw &= ~(PR_WIPE);
		msg_print(NULL);
		Term_clear();
	}


	if (play_redraw & (PR_MAP))
	{
		play_redraw &= ~(PR_MAP);
		prt_map(cr_ptr);
	}


	prt_frame_basic(cr_ptr);

	if (play_redraw & (PR_BASIC))
	{
		play_redraw &= ~(PR_BASIC);
		play_redraw &= ~(PR_MISC | PR_TITLE | PR_STATS);
		play_redraw &= ~(PR_LEV | PR_EXP | PR_GOLD);
		play_redraw &= ~(PR_ARMOR | PR_HP | PR_MANA);
		play_redraw &= ~(PR_DEPTH | PR_HEALTH | PR_UHEALTH);
		prt_time();
		prt_dungeon();
	}

	if (play_redraw & (PR_EQUIPPY))
	{
		play_redraw &= ~(PR_EQUIPPY);
		print_equippy(cr_ptr); /* To draw / delete equippy chars */
	}

	if (play_redraw & (PR_MISC))
	{
		play_redraw &= ~(PR_MISC);
//		prt_field(race_info[cr_ptr->race_idx1].title, ROW_RACE, COL_RACE);
/*		prt_field(class_info[cr_ptr->cls_idx].title, ROW_CLASS, COL_CLASS); */

	}

	if (play_redraw & (PR_LEV))
	{
		play_redraw &= ~(PR_LEV);
		prt_level(cr_ptr);
	}

	if (play_redraw & (PR_EXP))
	{
		play_redraw &= ~(PR_EXP);
		prt_exp(cr_ptr);
	}

	if (play_redraw & (PR_STATS))
	{
		play_redraw &= ~(PR_STATS);
		prt_stat(cr_ptr, STAT_STR);
		prt_stat(cr_ptr, STAT_INT);
		prt_stat(cr_ptr, STAT_WIS);
		prt_stat(cr_ptr, STAT_DEX);
		prt_stat(cr_ptr, STAT_CON);
		prt_stat(cr_ptr, STAT_CHR);
	}

	if (play_redraw & (PR_STATUS))
	{
		play_redraw &= ~(PR_STATUS);
		prt_status(cr_ptr);
	}

	if (play_redraw & (PR_ARMOR))
	{
		play_redraw &= ~(PR_ARMOR);
		prt_ac(cr_ptr);
	}

	if (play_redraw & (PR_HP))
	{
		play_redraw &= ~(PR_HP);
		prt_hp(cr_ptr);
	}

	if (play_redraw & (PR_MANA))
	{
		play_redraw &= ~(PR_MANA);
		prt_sp(cr_ptr);
	}

	if (play_redraw & (PR_GOLD))
	{
		play_redraw &= ~(PR_GOLD);
		prt_gold(cr_ptr);
	}

	if (play_redraw & (PR_DEPTH))
	{
		play_redraw &= ~(PR_DEPTH);
		prt_depth(cr_ptr);
	}

	if (play_redraw & (PR_HEALTH))
	{
		play_redraw &= ~(PR_HEALTH);
		health_redraw(cr_ptr, FALSE);
	}

	if (play_redraw & (PR_UHEALTH))
	{
		play_redraw &= ~(PR_UHEALTH);
		health_redraw(cr_ptr, TRUE);
	}


	if (play_redraw & (PR_EXTRA))
	{
		play_redraw &= ~(PR_EXTRA);
		play_redraw &= ~(PR_CUT | PR_STUN);
		play_redraw &= ~(PR_HUNGER);
		play_redraw &= ~(PR_STATE | PR_SPEED | PR_STUDY | PR_IMITATION | PR_STATUS);
		prt_frame_extra(cr_ptr);
	}

	if (play_redraw & (PR_CUT))
	{
		play_redraw &= ~(PR_CUT);
		prt_cut(cr_ptr);
	}

	if (play_redraw & (PR_STUN))
	{
		play_redraw &= ~(PR_STUN);
		prt_stun(cr_ptr);
	}

	if (play_redraw & (PR_HUNGER))
	{
		play_redraw &= ~(PR_HUNGER);
		prt_hunger(cr_ptr);
	}

	if (play_redraw & (PR_STATE))
	{
		play_redraw &= ~(PR_STATE);
		prt_state(cr_ptr);
	}

	if (play_redraw & (PR_SPEED))
	{
		play_redraw &= ~(PR_SPEED);
		prt_speed(cr_ptr);
	}

	if (cr_ptr->cls_idx == CLASS_IMITATOR)
	{
		if (play_redraw & (PR_IMITATION))
		{
			play_redraw &= ~(PR_IMITATION);
			prt_imitation(cr_ptr);
		}
	}
	else if (play_redraw & (PR_STUDY))
	{
		play_redraw &= ~(PR_STUDY);
		prt_study(cr_ptr);
	}
}


/*
 * Handle "play_window"
 */
void window_stuff(void)
{
	int j;

	u32b mask = 0L;


	/* Nothing to do */
	if (!play_window) return;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		/* Save usable flags */
		if (angband_term[j]) mask |= window_flag[j];
	}

	/* Apply usable flags */
	play_window &= mask;

	/* Nothing to do */
	if (!play_window) return;


	/* Display inventory */
	if (play_window & (PW_INVEN))
	{
		play_window &= ~(PW_INVEN);
		fix_inven(p_ptr);
	}

	/* Display equipment */
	if (play_window & (PW_EQUIP))
	{
		play_window &= ~(PW_EQUIP);
		fix_equip();
	}

	/* Display spell list */
	if (play_window & (PW_SPELL))
	{
		play_window &= ~(PW_SPELL);
		fix_spell();
	}

	/* Display player */
	if (play_window & (PW_PLAYER))
	{
		play_window &= ~(PW_PLAYER);
		fix_player();
	}

	/* Display overhead view */
	if (play_window & (PW_MESSAGE))
	{
		play_window &= ~(PW_MESSAGE);
		fix_message();
	}

	/* Display overhead view */
	if (play_window & (PW_OVERHEAD))
	{
		play_window &= ~(PW_OVERHEAD);
		fix_overhead();
	}

	/* Display overhead view */
	if (play_window & (PW_DUNGEON))
	{
		play_window &= ~(PW_DUNGEON);
		fix_dungeon();
	}

	/* Display monster recall */
	if (play_window & (PW_MONSTER))
	{
		play_window &= ~(PW_MONSTER);
		fix_monster(p_ptr);
	}

	/* Display object recall */
	if (play_window & (PW_OBJECT))
	{
		play_window &= ~(PW_OBJECT);
		fix_object();
	}
}


/*
 * Handle "update" and "play_redraw" and "play_window"
 */
void handle_stuff(creature_type *cr_ptr)
{
	/* Update stuff */
	if (cr_ptr->update) update_stuff(cr_ptr, is_player(cr_ptr));

	/* Redraw stuff */
	if (play_redraw) redraw_stuff(player_ptr);

	/* Window stuff */
	if (play_window) window_stuff();
}


s16b empty_hands(creature_type *cr_ptr, bool riding_control)
{
	s16b status = EMPTY_HAND_NONE;

	if (!cr_ptr->inventory[INVEN_1STARM].k_idx) status |= EMPTY_HAND_RARM;
	if (!cr_ptr->inventory[INVEN_2NDARM].k_idx) status |= EMPTY_HAND_LARM;

	if (riding_control && (status != EMPTY_HAND_NONE) && cr_ptr->riding && !(cr_ptr->pet_extra_flags & PF_RYOUTE))
	{
		if (status & EMPTY_HAND_LARM) status &= ~(EMPTY_HAND_LARM);
		else if (status & EMPTY_HAND_RARM) status &= ~(EMPTY_HAND_RARM);
	}

	return status;
}


bool heavy_armor(creature_type *cr_ptr)
{
	u16b monk_arm_wgt = 0;

	if ((cr_ptr->cls_idx != CLASS_MONK) && (cr_ptr->cls_idx != CLASS_FORCETRAINER) && (cr_ptr->cls_idx != CLASS_NINJA)) return FALSE;

	/* Weight the armor */
	if(cr_ptr->inventory[INVEN_1STARM].tval > TV_SWORD) monk_arm_wgt += cr_ptr->inventory[INVEN_1STARM].weight;
	if(cr_ptr->inventory[INVEN_2NDARM].tval > TV_SWORD) monk_arm_wgt += cr_ptr->inventory[INVEN_2NDARM].weight;
	monk_arm_wgt += cr_ptr->inventory[INVEN_BODY].weight;
	monk_arm_wgt += cr_ptr->inventory[INVEN_1STHEAD].weight;
	monk_arm_wgt += cr_ptr->inventory[INVEN_OUTER].weight;
	monk_arm_wgt += cr_ptr->inventory[INVEN_1STHANDS].weight;
	monk_arm_wgt += cr_ptr->inventory[INVEN_FEET].weight;

	return (monk_arm_wgt > (100 + (cr_ptr->lev * 4)));
}


/* Calculate alignment */
s16b calc_rank(s32b align){
	s32b t = 0, n; 
	if(align < 0) n = -align;
	else          n = align;
	while(t * t < n / 10) t++;
	if(align < 0) return (s16b)-t;
	else          return (s16b)t;
}

/* get subrace */
bool get_subrace(creature_type *cr_ptr, int n)
{
	if(cr_ptr->sub_race[n / 32] & (0x01 << (n % 32))) return TRUE;
	return FALSE;
}

/* set subrace */
void set_subrace(creature_type *cr_ptr, int n, bool b)
{
	unsigned int a = n;
	if(b) cr_ptr->sub_race[a / 32] = cr_ptr->sub_race[a / 32] | (0x01 << (a % 32));
	else cr_ptr->sub_race[a / 32] = cr_ptr->sub_race[a / 32] & !(0x01 << (a % 32));
}



/* Return Race Name */
cptr desc_race_name(creature_type *cr_ptr){
	char name[80];
	race_type *rcr_ptr = &race_info[cr_ptr->race_idx1];
	name[0] = '\0';

	if(cr_ptr->race_idx1 == RACE_NONE) return format("");
	else if(cr_ptr->race_idx2 == RACE_NONE) return format("%s", race_info[cr_ptr->race_idx1].title);

	if(!IS_PURE(cr_ptr))
		strcat(name, "ハーフ");

	if(IS_RACE(cr_ptr, RACE_HUMAN) || IS_PURE(cr_ptr))
		strcat(name, race_info[cr_ptr->race_idx1].title);

	if(!IS_PURE(cr_ptr))
		strcat(name, race_info[cr_ptr->race_idx2].title);

	if(cr_ptr->sex != SEX_UNDEFINED)
	{
		strcat(name, "の");
		strcat(name, sex_info[cr_ptr->sex].title);
	}

	return format("%s", name);
}


cptr get_class_desc(creature_type *cr_ptr){
	char name[80];
	name[0] = '\0';

	strcat(name, class_info[cr_ptr->cls_idx].title);
	if(cr_ptr->realm1 != REALM_NONE && cr_ptr->realm1 < MAX_REALM)
	{
		strcat(name, " (");
		strcat(name, realm_names[cr_ptr->realm1]);
		if(cr_ptr->realm2 != REALM_NONE && cr_ptr->realm2 < MAX_REALM)
		{
			strcat(name, ", ");
			strcat(name, realm_names[cr_ptr->realm2]);
		}
		strcat(name, ")");
	}

	return format("%s", name);
}

