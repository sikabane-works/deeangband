
/* File: misc.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

// 2012 Deskull modified.

/* Purpose: misc code */

#include "angband.h"



// Converts stat num into a six-char (right justified) string
void cnv_stat(int val, char *out_val)
{
	sprintf(out_val, " %3d.%01d", val / STAT_FRACTION, val % STAT_FRACTION);
}

// Print character info at given row, column in a 13 char field
static void prt_field(cptr info, int row, int col)
{
	c_put_str(TERM_WHITE, "             ", row, col); // Dump 13 spaces to clear
	c_put_str(TERM_L_BLUE, info, row, col); // Dump the info itself
}

//  Whether daytime or not
bool is_daytime(void)
{
	s32b len = TURNS_PER_TICK * TOWN_DAWN;
	if((turn % len) < (len / 2)) return TRUE;
	else return FALSE;
}

/*
 * Extract day, hour, min
 */
void extract_day_hour_min(int *day, int *hour, int *min)
{
	s32b turn_in_today = (turn + A_DAY / 4 + start_hour * 1440 + start_min * 60) % A_DAY;
	*day = (turn + A_DAY / 4) / A_DAY + 1;
	*hour = (24 * turn_in_today / A_DAY) % 24;
	*min = (1440 * turn_in_today / A_DAY) % 60;
}

/*
 * Print time
 */
void prt_time(creature_type *player_ptr)
{
	int day, hour, min;

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DAY, COL_DAY);

	extract_day_hour_min(&day, &hour, &min);

	/* Dump the info itself */
#ifdef JP
	if(day < 1000) c_put_str(TERM_WHITE, format("%2d日目", day), ROW_DAY, COL_DAY);
	else c_put_str(TERM_WHITE, "***日目", ROW_DAY, COL_DAY);
#else
	if(day < 1000) c_put_str(TERM_WHITE, format("Day%3d", day), ROW_DAY, COL_DAY);
	else c_put_str(TERM_WHITE, "Day***", ROW_DAY, COL_DAY);
#endif

	c_put_str(TERM_WHITE, format("%2d:%02d", hour, min), ROW_DAY, COL_DAY+7);
}

void prt_wiz_pos(creature_type *player_ptr)
{
	c_put_str(TERM_WHITE, format("W(%03d,%03d) F(%03d,%03d)", player_ptr->wx, player_ptr->wy, player_ptr->fx, player_ptr->fy), ROW_DAY + 5, COL_DAY);
}


cptr map_name(floor_type *floor_ptr)
{
	if(floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest) && (quest[floor_ptr->quest].flags & QUEST_FLAG_PRESET))
#ifdef JP
		return "クエスト";
#else
		return "Quest";
#endif
	else if(floor_ptr->wild_mode)
#ifdef JP
		return "地上";
#else
		return "Surface";
#endif
	else if(floor_ptr->fight_arena_mode)
#ifdef JP
		return "アリーナ";
#else
		return "Arena";
#endif
	else if(floor_ptr->gamble_arena_mode)
#ifdef JP
		return "闘技場";
#else
		return "Creature Arena";
#endif
	else if(!floor_ptr->floor_level && floor_ptr->town_num)
		return town[floor_ptr->town_num].name;
	else
		return dungeon_name + dungeon_info[floor_ptr->dun_type].name;
}

/*
 * Print dungeon
 */
static void prt_dungeon(void)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	cptr dungeon_name;
	int col;

	// Dump 13 spaces to clear
	c_put_str(TERM_WHITE, "             ", ROW_DUNGEON, COL_DUNGEON);

	dungeon_name = map_name(floor_ptr);

	col = COL_DUNGEON + 6 - strlen(dungeon_name) / 2;
	if(col < 0) col = 0;

	/* Dump the info itself */
	c_put_str(TERM_L_UMBER, format("%s",dungeon_name), ROW_DUNGEON, col);
}




/*
 * Print character stat in given row, column
 */
static void prt_stat(creature_type *creature_ptr, int stat)
{
	char tmp[32];

	if(!has_status(creature_ptr, stat))
	{
		put_str(stat_names[stat], ROW_STAT + stat, 0);
		c_put_str(TERM_L_DARK, "------", ROW_STAT + stat, COL_STAT + 6);
	}
	else if(creature_ptr->stat_cur[stat] < creature_ptr->stat_max[stat]) // Display "injured" stat
	{
		put_str(stat_names_reduced[stat], ROW_STAT + stat, 0);
		cnv_stat(creature_ptr->stat_use[stat], tmp);
		c_put_str(TERM_YELLOW, tmp, ROW_STAT + stat, COL_STAT + 6);
	}
	else // Display "healthy" stat
	{
		put_str(stat_names[stat], ROW_STAT + stat, 0);
		cnv_stat(creature_ptr->stat_use[stat], tmp);
		c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
	}

	if(creature_ptr->stat_max[stat] == creature_ptr->stat_mod_max_max[stat]) put_str("!", ROW_STAT + stat, 5);
}


//  Data structure for status bar
enum STATUS_BAR
{
	BAR_TSUYOSHI,
	BAR_HALLUCINATION,
	BAR_BLINDNESS,
	BAR_PARALYZE,
	BAR_CONFUSE,
	BAR_POISONED,
	BAR_AFRAID,
	BAR_LEVITATE,
	BAR_REFLECTION,
	BAR_PASSWALL,
	BAR_WRAITH,
	BAR_PROTEVIL,
	BAR_KAWARIMI,
	BAR_MAGICDEFENSE,
	BAR_EXPAND,
	BAR_STONESKIN,
	BAR_MULTISHADOW,
	BAR_REGMAGIC,
	BAR_ULTIMATE,
	BAR_INVULN,
	BAR_IMMACID,
	BAR_RESACID,
	BAR_IMMELEC,
	BAR_RESELEC,
	BAR_IMMFIRE,
	BAR_RESFIRE,
	BAR_IMMCOLD,
	BAR_RESCOLD,
	BAR_RESPOIS,
	BAR_RESNETH,
	BAR_RESTIME,
	BAR_DUSTROBE,
	BAR_SHFIRE,
	BAR_TOUKI,
	BAR_SHHOLY,
	BAR_EYEEYE,
	BAR_BLESSED,
	BAR_HEROISM,
	BAR_BERSERK,
	BAR_ATTKFIRE,
	BAR_ATTKCOLD,
	BAR_ATTKELEC,
	BAR_ATTKACID,
	BAR_ATTKPOIS,
	BAR_ATTKCONF,
	BAR_SENSEUNSEEN,
	BAR_TELEPATHY,
	BAR_REGENERATION,
	BAR_INFRAVISION,
	BAR_STEALTH,
	BAR_SUPERSTEALTH,
	BAR_RECALL,
	BAR_ALTER,
	BAR_SHCOLD,
	BAR_SHELEC,
	BAR_SHSHADOW,
	BAR_MIGHT,
	BAR_BUILD,
	BAR_ANTIMULTI,
	BAR_ANTITELE,
	BAR_ANTIMAGIC,
	BAR_PATIENCE,
	BAR_REVENGE,
	BAR_RUNESWORD,
	BAR_VAMPILIC,
	BAR_CURE,
	BAR_ESP_EVIL,
};

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
static void prt_status(creature_type *creature_ptr)
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

	if(creature_ptr->timed_trait[TRAIT_TSUYOSHI]) ADD_FLG(BAR_TSUYOSHI);		// Tsuyoshi
	if(has_trait(creature_ptr, TRAIT_HALLUCINATION)) ADD_FLG(BAR_HALLUCINATION);		// Hallucinating
	if(has_trait(creature_ptr, TRAIT_BLIND)) ADD_FLG(BAR_BLINDNESS);			// Blindness
	if(creature_ptr->timed_trait[TRAIT_PARALYZED]) ADD_FLG(BAR_PARALYZE);		// Paralysis
	if(has_trait(creature_ptr, TRAIT_CONFUSED)) ADD_FLG(BAR_CONFUSE);			// Confusion
	if(has_trait(creature_ptr, TRAIT_POISONED)) ADD_FLG(BAR_POISONED);		// Posioned
	if(creature_ptr->timed_trait[TRAIT_SEE_INVISIBLE]) ADD_FLG(BAR_SENSEUNSEEN);	// Times see-invisible
	if(IS_TIM_ESP(creature_ptr)) ADD_FLG(BAR_TELEPATHY);		// Timed esp
	if(creature_ptr->timed_trait[TRAIT_REGENERATE]) ADD_FLG(BAR_REGENERATION);	// Timed regenerate
	if(creature_ptr->timed_trait[TRAIT_SEE_INFRA]) ADD_FLG(BAR_INFRAVISION);	// Timed infra-vision
	if(creature_ptr->timed_trait[TRAIT_PROT_EVIL]) ADD_FLG(BAR_PROTEVIL);		// Protection from evil
	if(IS_INVULN(creature_ptr)) ADD_FLG(BAR_INVULN);			// Invulnerability
	if(has_trait(creature_ptr, TRAIT_WRAITH_FORM)) ADD_FLG(BAR_WRAITH);		// Wraith form
	if(creature_ptr->timed_trait[TRAIT_PASS_WALL]) ADD_FLG(BAR_PASSWALL);		// Pass wall
	if(creature_ptr->timed_trait[TRAIT_REFLECTING]) ADD_FLG(BAR_REFLECTION);
	if(IS_HERO(creature_ptr)) ADD_FLG(BAR_HEROISM);			// Heroism
	if(creature_ptr->timed_trait[TRAIT_S_HERO]) ADD_FLG(BAR_BERSERK);			// Super Heroism / berserk
	if(IS_BLESSED(creature_ptr)) ADD_FLG(BAR_BLESSED);		// Blessed
	if(creature_ptr->timed_trait[TRAIT_MAGIC_DEF]) ADD_FLG(BAR_MAGICDEFENSE);	// Shield
	if(creature_ptr->timed_trait[TRAIT_TSUBURERU]) ADD_FLG(BAR_EXPAND);
	if(creature_ptr->timed_trait[TRAIT_SHIELD]) ADD_FLG(BAR_STONESKIN);
	if(creature_ptr->posture & NINJA_KAWARIMI) ADD_FLG(BAR_KAWARIMI);

	if(IS_OPPOSE_ACID(creature_ptr)) ADD_FLG(BAR_RESACID);
	if(IS_OPPOSE_ELEC(creature_ptr)) ADD_FLG(BAR_RESELEC);
	if(IS_OPPOSE_FIRE(creature_ptr)) ADD_FLG(BAR_RESFIRE);
	if(IS_OPPOSE_COLD(creature_ptr)) ADD_FLG(BAR_RESCOLD);
	if(IS_OPPOSE_POIS(creature_ptr)) ADD_FLG(BAR_RESPOIS);
	if(creature_ptr->timed_trait[TRAIT_WORD_RECALL]) ADD_FLG(BAR_RECALL); // Word of Recall
	if(creature_ptr->timed_trait[TRAIT_ALTER_REALITY]) ADD_FLG(BAR_ALTER); // Alter realiry
	if(has_trait(creature_ptr, TRAIT_AFRAID)) ADD_FLG(BAR_AFRAID); // Afraid
	if(creature_ptr->timed_trait[TRAIT_RES_TIME]) ADD_FLG(BAR_RESTIME); // Resist time
	if(creature_ptr->timed_trait[TRAIT_MULTI_SHADOW]) ADD_FLG(BAR_MULTISHADOW);
	if(creature_ptr->timed_trait[TRAIT_CONFUSING_MELEE]) ADD_FLG(BAR_ATTKCONF); // Confusing Hands
	if(creature_ptr->timed_trait[TRAIT_RESIST_MAGIC]) ADD_FLG(BAR_REGMAGIC);
	if(creature_ptr->timed_trait[TRAIT_ULTRA_RES]) ADD_FLG(BAR_ULTIMATE); // Ultimate-resistance
	if(creature_ptr->timed_trait[TRAIT_LEVITATION]) ADD_FLG(BAR_LEVITATE); // tim levitation
	if(creature_ptr->timed_trait[TRAIT_RES_NETH]) ADD_FLG(BAR_RESNETH);
	if(creature_ptr->timed_trait[TRAIT_DUST_ROBE]) ADD_FLG(BAR_DUSTROBE);

	if(creature_ptr->timed_trait[TRAIT_FIRE_BRAND]) ADD_FLG(BAR_ATTKFIRE);
	if(creature_ptr->timed_trait[TRAIT_COLD_BRAND]) ADD_FLG(BAR_ATTKCOLD);
	if(creature_ptr->timed_trait[TRAIT_ELEC_BRAND]) ADD_FLG(BAR_ATTKELEC);
	if(creature_ptr->timed_trait[TRAIT_ACID_BRAND]) ADD_FLG(BAR_ATTKACID);
	if(creature_ptr->timed_trait[TRAIT_POIS_BRAND]) ADD_FLG(BAR_ATTKPOIS);
	if(creature_ptr->posture & NINJA_S_STEALTH) ADD_FLG(BAR_SUPERSTEALTH);

	if(creature_ptr->timed_trait[TRAIT_AURA_FIRE]) ADD_FLG(BAR_SHFIRE);
	
	if(IS_TIM_STEALTH(creature_ptr)) ADD_FLG(BAR_STEALTH); // tim stealth

	if(creature_ptr->timed_trait[TRAIT_AURA_MANA]) ADD_FLG(BAR_TOUKI);
	if(creature_ptr->timed_trait[TRAIT_HOLY_AURA]) ADD_FLG(BAR_SHHOLY);
	if(creature_ptr->timed_trait[TRAIT_EYE_EYE]) ADD_FLG(BAR_EYEEYE);

	/* Hex spells */
	if(creature_ptr->realm1 == REALM_HEX)
	{
		if(HEX_SPELLING(creature_ptr, HEX_BLESS)) ADD_FLG(BAR_BLESSED);
		if(HEX_SPELLING(creature_ptr, HEX_DEMON_AURA)) { ADD_FLG(BAR_SHFIRE); ADD_FLG(BAR_REGENERATION); }
		if(HEX_SPELLING(creature_ptr, HEX_XTRA_MIGHT)) ADD_FLG(BAR_MIGHT);
		if(HEX_SPELLING(creature_ptr, HEX_DETECT_EVIL)) ADD_FLG(BAR_ESP_EVIL);
		if(HEX_SPELLING(creature_ptr, HEX_ICE_ARMOR)) ADD_FLG(BAR_SHCOLD);
		if(HEX_SPELLING(creature_ptr, HEX_RUNESWORD)) ADD_FLG(BAR_RUNESWORD);
		if(HEX_SPELLING(creature_ptr, HEX_BUILDING)) ADD_FLG(BAR_BUILD);
		if(HEX_SPELLING(creature_ptr, HEX_ANTI_TELE)) ADD_FLG(BAR_ANTITELE);
		if(HEX_SPELLING(creature_ptr, HEX_SHOCK_CLOAK)) ADD_FLG(BAR_SHELEC);
		if(HEX_SPELLING(creature_ptr, HEX_SHADOW_CLOAK)) ADD_FLG(BAR_SHSHADOW);
		if(HEX_SPELLING(creature_ptr, HEX_CONFUSION)) ADD_FLG(BAR_ATTKCONF);
		if(HEX_SPELLING(creature_ptr, HEX_EYE_FOR_EYE)) ADD_FLG(BAR_EYEEYE);
		if(HEX_SPELLING(creature_ptr, HEX_ANTI_MULTI)) ADD_FLG(BAR_ANTIMULTI);
		if(HEX_SPELLING(creature_ptr, HEX_VAMP_BLADE)) ADD_FLG(BAR_VAMPILIC);
		if(HEX_SPELLING(creature_ptr, HEX_ANTI_MAGIC)) ADD_FLG(BAR_ANTIMAGIC);
		if(HEX_SPELLING(creature_ptr, HEX_CURE_LIGHT) ||
			HEX_SPELLING(creature_ptr, HEX_CURE_SERIOUS) ||
			HEX_SPELLING(creature_ptr, HEX_CURE_CRITICAL)) ADD_FLG(BAR_CURE);

		if(creature_ptr->class_skills.old_skills.magic_num2[2])
		{
			if(creature_ptr->class_skills.old_skills.magic_num2[1] == 1) ADD_FLG(BAR_PATIENCE);
			if(creature_ptr->class_skills.old_skills.magic_num2[1] == 2) ADD_FLG(BAR_REVENGE);
		}
	}

	/* Calcurate length */
	for (i = 0; bar[i].sstr; i++)
	{
		if(IS_FLG(i))
		{
			col += strlen(bar[i].lstr) + 1;
			num++;
		}
	}

	/* If there are not excess spaces for long strings, use short one */
	if(col - 1 > max_col_statbar)
	{
		space = 0;
		col = 0;

		for (i = 0; bar[i].sstr; i++)
		{
			if(IS_FLG(i))
			{
				col += strlen(bar[i].sstr);
			}
		}

		/* If there are excess spaces for short string, use more */
		if(col - 1 <= max_col_statbar - (num-1))
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
		if(IS_FLG(i))
		{
			cptr str;
			if(space == 2) str = bar[i].lstr;
			else str = bar[i].sstr;

			c_put_str(bar[i].attr, str, row_statbar, col);
			col += strlen(str);
			if(space > 0) col++;
			if(col > max_col_statbar) break;
		}
	}
}


/*
 * Prints level
 */
static void prt_level(creature_type *creature_ptr)
{
	char tmp[32];

	sprintf(tmp, "%2d/%2d", creature_ptr->lev, creature_ptr->max_lev);

	if(creature_ptr->lev >= creature_ptr->max_plv)
	{
		put_str("LEV", ROW_LEVEL, 0);				
		c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 3);
	}
	else
	{
		put_str("lev", ROW_LEVEL, 0);
		c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 3);
	}
}


/*
 * Display the experience
 */
static void prt_exp(creature_type *creature_ptr)
{
	char out_val[32];

	if((!exp_need)||(has_trait(creature_ptr, TRAIT_ANDROID)))
	{
		(void)sprintf(out_val, "%9ld", (long)creature_ptr->exp);
	}
	else
	{
		if(creature_ptr->lev >= creature_ptr->max_lev)
		{
			(void)sprintf(out_val, "*********");
		}
		else
		{
			(void)sprintf(out_val, "%9ld", (long)(creature_exp[creature_ptr->lev - 1] * creature_ptr->expfact / 100L) - creature_ptr->exp);
		}
	}

	if(creature_ptr->exp >= creature_ptr->max_exp)
	{
		if(has_trait(creature_ptr, TRAIT_ANDROID)) put_str("Cst ", ROW_EXP, 0);
		else put_str("EXP", ROW_EXP, COL_EXP);
		c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 3);

	}
	else
	{
		put_str("Exp", ROW_EXP, COL_EXP);
		c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 3);
	}
}

/*
 * Prints current gold
 */
static void prt_gold(creature_type *creature_ptr)
{
	char tmp[32];

#ifdef JP
	put_str("＄ ", ROW_GOLD, COL_GOLD);
#else
	put_str("AU ", ROW_GOLD, COL_GOLD);
#endif

	sprintf(tmp, "%9ld", (long)creature_ptr->au);
	c_put_str(TERM_L_GREEN, tmp, ROW_GOLD, COL_GOLD + 3);
}


// Prints current AC and EV
static void prt_ac_ev(creature_type *creature_ptr)
{
	char tmp[32];
	put_str(" AC(     )  EV(     )", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", creature_ptr->dis_ac + creature_ptr->dis_to_ac);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 4);
	sprintf(tmp, "%5d", creature_ptr->dis_ev + creature_ptr->dis_to_ev);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 15);
}

/*
 * Prints Cur/Max hit points
 */
static void prt_hp(creature_type *creature_ptr)
{
	char tmp[32]; 
	byte color;

	put_str("HP", ROW_CURHP, COL_CURHP);
	sprintf(tmp, "%5ld", creature_ptr->chp);

	if(creature_ptr->chp >= creature_ptr->mhp) color = TERM_L_GREEN;
	else if(creature_ptr->chp > (creature_ptr->mhp * hitpoint_warn) / 10) color = TERM_YELLOW;
	else color = TERM_RED;

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP + 2);

	put_str("/", ROW_CURHP, COL_CURHP + 7);
	sprintf(tmp, "%5ld", creature_ptr->mhp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP + 8);
}


/*
 * Prints players max/cur spell points
 */
static void prt_sp(creature_type *creature_ptr)
{
	char tmp[32];
	byte color;

#ifdef JP
	put_str("MP", ROW_CURSP, COL_CURSP);
#else
	put_str("SP", ROW_CURSP, COL_CURSP);
#endif

	sprintf(tmp, "%5ld", creature_ptr->csp);

	if(creature_ptr->csp >= creature_ptr->msp) color = TERM_L_GREEN;
	else if(creature_ptr->csp > (creature_ptr->msp * mana_warn) / 10) color = TERM_YELLOW;
	else color = TERM_RED;
	c_put_str(color, tmp, ROW_CURSP, COL_CURSP + 2);

	put_str( "/", ROW_CURSP, COL_CURSP + 7);

	sprintf(tmp, "%5d", creature_ptr->msp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP + 8);
}



// Prints depth in stat area
static void prt_depth(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	char depths[32];
	int wid, hgt, row_depth, col_depth;
	byte attr = TERM_WHITE;

	Term_get_size(&wid, &hgt);
	col_depth = wid + COL_DEPTH;
	row_depth = hgt + ROW_DEPTH;

	if(!floor_ptr->floor_level)
	{
#ifdef JP
		strcpy(depths, "地上");
#else
		strcpy(depths, "Surf.");
#endif
	}
	else if(floor_ptr->quest && !floor_ptr->dun_type)
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
		if(depth_in_feet) (void)sprintf(depths, "%d ft", floor_ptr->floor_level * 50);
		else (void)sprintf(depths, "%d 階", floor_ptr->floor_level);
#else
		if(depth_in_feet) (void)sprintf(depths, "%d ft", floor_ptr->floor_level * 50);
		else (void)sprintf(depths, "Lev %d", floor_ptr->floor_level);
#endif


		/* Get color of level based on feeling  -JSV- */
		switch (creature_ptr->floor_feeling)
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
	if(wizard)
	{
		c_prt(attr, format("%7s F:%3d", depths, player_ptr->floor_id), row_depth, col_depth - 6);
	}
	else
	{
		c_prt(attr, format("%7s", depths), row_depth, col_depth);
	}
}


/*
 * Prints status of hunger
 */
static void prt_hunger(creature_type *creature_ptr)
{
	byte color;
	char *expression;
	char tmp[30];

	if(creature_ptr->food < CREATURE_FOOD_FAINT)
	{
		color = TERM_RED;
#ifdef JP
		expression = "衰弱  ";
		c_put_str(TERM_RED, "衰弱  ", ROW_HUNGRY, COL_HUNGRY);
#else
		expression = "Weak  ";
		c_put_str(TERM_RED, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}
	else if(creature_ptr->food < CREATURE_FOOD_WEAK)
	{
		color = TERM_ORANGE;
#ifdef JP
		expression = "衰弱  ";
		c_put_str(TERM_ORANGE, "衰弱  ", ROW_HUNGRY, COL_HUNGRY);
#else
		expression = "衰弱  ";
		c_put_str(TERM_ORANGE, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}
	else if(creature_ptr->food < CREATURE_FOOD_ALERT)
	{
		color = TERM_YELLOW;
#ifdef JP
		expression = "空腹  ";
		c_put_str(TERM_YELLOW, "空腹  ", ROW_HUNGRY, COL_HUNGRY);
#else
		expression = "Hungry";
		c_put_str(TERM_YELLOW, "Hungry", ROW_HUNGRY, COL_HUNGRY);
#endif
	}
	else if(creature_ptr->food < CREATURE_FOOD_FULL)
	{
		color = TERM_L_GREEN;
		expression = "      ";
		c_put_str(TERM_L_GREEN, "      ", ROW_HUNGRY, COL_HUNGRY);
	}
	else if(creature_ptr->food < CREATURE_FOOD_MAX)
	{
		color = TERM_L_GREEN;
#ifdef JP
		expression = "満腹  ";
#else
		expression = "Full  ";
#endif

	}
	else
	{
		color = TERM_GREEN;
#ifdef JP
		expression = "食過ぎ";
#else
		expression = "Gorged";
#endif

	}

	if(wizard) sprintf(tmp, "%6s(%d)", expression, creature_ptr->food);
	else sprintf(tmp, "%6s", expression);

	c_put_str(color, tmp, ROW_HUNGRY, COL_HUNGRY);
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status
 * Display is always exactly 10 characters wide (see below)
 *
 * This function was a major bottleneck when resting, so a lot of
 * the text formatting code was optimized in place below.
 */
static void prt_state(creature_type *creature_ptr)
{
	byte attr = TERM_WHITE;

	char text[5];

	/* Repeating */
	if(command_rep)
	{
		if(command_rep > 999)
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
		switch(creature_ptr->action)
		{
			case ACTION_REST:
			{
				int i;
				strcpy(text, "    ");

				/* Extensive (timed) rest */
				if(creature_ptr->resting >= 1000)
				{
					i = creature_ptr->resting / 100;
					text[3] = '0';
					text[2] = '0';
					text[1] = '0' + (i % 10);
					text[0] = '0' + (i / 10);
				}

				/* Long (timed) rest */
				else if(creature_ptr->resting >= 100)
				{
					i = creature_ptr->resting;
					text[3] = '0' + (i % 10);
					i = i / 10;
					text[2] = '0' + (i % 10);
					text[1] = '0' + (i / 10);
				}

				/* Medium (timed) rest */
				else if(creature_ptr->resting >= 10)
				{
					i = creature_ptr->resting;
					text[3] = '0' + (i % 10);
					text[2] = '0' + (i / 10);
				}

				/* Short (timed) rest */
				else if(creature_ptr->resting > 0)
				{
					i = creature_ptr->resting;
					text[3] = '0' + (i);
				}

				/* Rest until healed */
				else if(creature_ptr->resting == -1)
				{
					text[0] = text[1] = text[2] = text[3] = '*';
				}

				/* Rest until done */
				else if(creature_ptr->resting == -2)
				{
					text[0] = text[1] = text[2] = text[3] = '&';
				}
				break;
			}
			
#ifdef JP
			case ACTION_SEARCH: strcpy(text, "探索"); break;
			case ACTION_LEARN: strcpy(text, "学習"); break;
			case ACTION_FISH: strcpy(text, "釣り"); break;
			case ACTION_SING: strcpy(text, "歌  "); break;
			case ACTION_HAYAGAKE: strcpy(text, "速駆"); break;
			case ACTION_SPELL: strcpy(text, "詠唱"); break;
#else
			case ACTION_SEARCH: strcpy(text, "Sear"); break;
			case ACTION_LEARN: strcpy(text, "lear"); break;
			case ACTION_FISH: strcpy(text, "fish"); break;
			case ACTION_SING: strcpy(text, "Sing"); break;
			case ACTION_HAYAGAKE: strcpy(text, "Fast"); break;
			case ACTION_SPELL: strcpy(text, "Spel"); break;
#endif

			case ACTION_KAMAE:
			{
				int i;
				for (i = 0; i < MAX_KAMAE; i++)
					if(creature_ptr->posture & (KAMAE_GENBU << i)) break;
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
					if(creature_ptr->posture & (KATA_IAI << i)) break;
				strcpy(text, kata_shurui[i].desc);
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
static void prt_speed(creature_type *creature_ptr)
{
	int i = creature_ptr->speed;
	bool is_fast = IS_FAST(creature_ptr);

	byte attr = TERM_WHITE;
	char buf[32] = "";
	int wid, hgt, row_speed, col_speed;

	Term_get_size(&wid, &hgt);
	col_speed = wid + COL_SPEED;
	row_speed = hgt + ROW_SPEED;

	/* Hack -- Visually "undo" the Search Mode Slowdown */
	if(creature_ptr->action == ACTION_SEARCH && !creature_ptr->timed_trait[TRAIT_LIGHT_SPEED]) i += 10;

	/* Fast */
	if(i > 0)
	{
		if(creature_ptr->riding)
		{
			creature_type *m_ptr = &creature_list[creature_ptr->riding];
			if(has_trait(m_ptr, TRAIT_FAST) && !has_trait(m_ptr, TRAIT_SLOW)) attr = TERM_L_BLUE;
			else if(has_trait(m_ptr, TRAIT_SLOW) && !has_trait(m_ptr, TRAIT_FAST)) attr = TERM_VIOLET;
			else attr = TERM_GREEN;
		}
		else if((is_fast && !has_trait(creature_ptr, TRAIT_SLOW)) || has_trait(creature_ptr, TRAIT_LIGHT_SPEED)) attr = TERM_YELLOW;
		else if(has_trait(creature_ptr, TRAIT_SLOW) && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_GREEN;
#ifdef JP
		sprintf(buf, "%s(%+d)", (creature_ptr->riding ? "乗馬" : "加速"), i);
#else
		sprintf(buf, "Fast(%+d)", i);
#endif

	}

	/* Slow */
	else if(i < 0)
	{
		if(creature_ptr->riding)
		{
			creature_type *m_ptr = &creature_list[creature_ptr->riding];
			if(m_ptr->timed_trait[TRAIT_FAST] && !m_ptr->timed_trait[TRAIT_SLOW]) attr = TERM_L_BLUE;
			else if(m_ptr->timed_trait[TRAIT_SLOW] && !m_ptr->timed_trait[TRAIT_FAST]) attr = TERM_VIOLET;
			else attr = TERM_RED;
		}
		else if(is_fast && !creature_ptr->timed_trait[TRAIT_SLOW]) attr = TERM_YELLOW;
		else if(creature_ptr->timed_trait[TRAIT_SLOW] && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_UMBER;
#ifdef JP
		sprintf(buf, "%s(%+d)", (creature_ptr->riding ? "乗馬" : "減速"), i);
#else
		sprintf(buf, "Slow(%+d)", i);
#endif
	}

	else if(creature_ptr->riding)
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


static void prt_study(creature_type *creature_ptr)
{
	int wid, hgt, row_study, col_study;

	Term_get_size(&wid, &hgt);
	col_study = wid + COL_STUDY;
	row_study = hgt + ROW_STUDY;

	if(creature_ptr->new_spells)
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


static void prt_imitation(creature_type *creature_ptr)
{
	int wid, hgt, row_study, col_study;

	Term_get_size(&wid, &hgt);
	col_study = wid + COL_STUDY;
	row_study = hgt + ROW_STUDY;

	if(creature_ptr->class_idx == CLASS_IMITATOR)
	{
		if(creature_ptr->mane_num)
		{
			byte attr;
			if(creature_ptr->new_mane) attr = TERM_L_RED;
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


static void prt_cut(creature_type *creature_ptr)
{
	int c = creature_ptr->timed_trait[TRAIT_CUT];

	if(c > 1000)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "致命傷      ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_L_RED, "Mortal wound", ROW_CUT, COL_CUT);
#endif

	}
	else if(c > 200)
	{
#ifdef JP
		c_put_str(TERM_RED, "ひどい深手  ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Deep gash   ", ROW_CUT, COL_CUT);
#endif

	}
	else if(c > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "重傷        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Severe cut  ", ROW_CUT, COL_CUT);
#endif

	}
	else if(c > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "大変な傷    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Nasty cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if(c > 25)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "ひどい傷    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Bad cut     ", ROW_CUT, COL_CUT);
#endif

	}
	else if(c > 10)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "軽傷        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Light cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if(c)
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



static void prt_stun(creature_type *creature_ptr)
{
	int s = creature_ptr->timed_trait[TRAIT_STUN];

	if(s > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "意識不明瞭  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_RED, "Knocked out ", ROW_STUN, COL_STUN);
#endif

	}
	else if(s > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "ひどく朦朧  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Heavy stun  ", ROW_STUN, COL_STUN);
#endif

	}
	else if(s)
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
 * Redraw the "creature health bar"	-DRS-
 * Rather extensive modifications by	-BEN-
 *
 * The "creature health bar" provides visual feedback on the "health"
 * of the creature currently being "tracked".  There are several ways
 * to "track" a creature, including targetting it, attacking it, and
 * affecting it (and nobody else) with a ranged attack.
 *
 * Display the creature health bar (affectionately known as the
 * "health-o-meter").  Clear health bar if nothing is being tracked.
 * Auto-track current target creature when bored.  Note that the
 * health-bar stops tracking any creature that "disappears".
 */
static void health_redraw(creature_type *creature_ptr, bool riding)
{
	int row, col;
	creature_type *target_ptr;
	species_type *species_ptr;
	char k[2];

	if(riding)
	{
		npc_status_id = creature_ptr->riding;
		row = ROW_RIDING_INFO;
		col = COL_RIDING_INFO;
	}
	else
	{
		row = ROW_INFO;
		col = COL_INFO;
	}

	target_ptr = &creature_list[npc_status_id];
	species_ptr = &species_info[target_ptr->species_idx];

	if(is_player(target_ptr)) return;
	if(!npc_status_id) Term_erase(col, row, 20);

	if(!target_ptr->see_others || has_trait(creature_ptr, TRAIT_HALLUCINATION) || target_ptr->chp < 0)
		Term_putstr(col, row, 16, TERM_WHITE, "  HP[----------]");
	else
	{
		// Extract the "percent" of health
		int pct = 100L * target_ptr->chp / target_ptr->mhp;
		int pct2 = 100L * target_ptr->chp / target_ptr->mmhp;

		// Convert percent into "health"
		int len = (pct2 < 10) ? 1 : (pct2 < 90) ? (pct2 / 10 + 1) : 10;

		// Health
		byte attr = TERM_RED;

		if(has_trait(target_ptr, TRAIT_INVULNERABLE)) attr = TERM_WHITE;
		else if(has_trait(target_ptr, TRAIT_PARALYZED)) attr = TERM_BLUE;
		else if(has_trait(target_ptr, TRAIT_SLEPT)) attr = TERM_BLUE;
		else if(has_trait(target_ptr, TRAIT_AFRAID)) attr = TERM_VIOLET;

		else if(pct >= 100) attr = TERM_L_GREEN;
		else if(pct >= 60) attr = TERM_YELLOW;
		else if(pct >= 25) attr = TERM_ORANGE;
		else if(pct >= 10) attr = TERM_L_RED;
		Term_putstr(col, row, 16, TERM_WHITE, "  HP[----------]");

		Term_putstr(col + 5, row, len, attr, "**********");
		if(wizard) Term_putstr(col + 16, row, 8, TERM_L_GREEN, format("E:%d", creature_ptr->energy_need));

		if(npc_status_id)
		{
			k[0] = species_ptr->x_char;
			k[1] = 0;
			Term_putstr(col, row, 1, species_ptr->x_attr, k);
		}
	}
}


// Display basic info (mostly left of map)
static void prt_frame_basic(creature_type *creature_ptr)
{
	int i;
	prt_level(creature_ptr);
	prt_exp(creature_ptr);

	for (i = 0; i < STAT_MAX; i++) prt_stat(creature_ptr, i);
	prt_ac_ev(creature_ptr);
	prt_hp(creature_ptr);
	prt_sp(creature_ptr);
	prt_gold(creature_ptr);
	prt_depth(creature_ptr);

	//TODO	//health_redraw(creature_ptr, FALSE);
}


/*
 * Display extra info (mostly below map)
 */
static void prt_frame_extra(creature_type *creature_ptr)
{
	/* Cut/Stun */
	prt_cut(creature_ptr);
	prt_stun(creature_ptr);

	/* Food */
	prt_hunger(creature_ptr);

	/* State */
	prt_state(creature_ptr);

	/* Speed */
	prt_speed(creature_ptr);

	/* Study spells */
	prt_study(creature_ptr);

	prt_imitation(creature_ptr);

	prt_status(creature_ptr);
}


/*
 * Hack -- display inventory in sub-windows
 */
static void fix_inven(creature_type *creature_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if(!angband_term[j]) continue;

		/* No relevant flags */
		if(!(window_flag[j] & (PW_INVEN))) continue;

		Term_activate(angband_term[j]);

		/* Display inventory */
		display_inven(creature_ptr);

		Term_fresh();
		Term_activate(old);
	}
}



/*
 * Hack -- display equipment in sub-windows
 */
static void fix_equip(creature_type *player_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;
		if(!angband_term[j]) continue;	// No window
		if(!(window_flag[j] & (PW_EQUIP))) continue;	// No relevant flags

		Term_activate(angband_term[j]);	// Activate
		display_equip(player_ptr);		// Display equipment
		Term_fresh();					// Fresh
		Term_activate(old);				// Restore
	}
}


/*
 * Hack -- display equipment in sub-windows
 */
static void fix_spell(creature_type *creature_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if(!angband_term[j]) continue;

		/* No relevant flags */
		if(!(window_flag[j] & (PW_SPELL))) continue;

		Term_activate(angband_term[j]);

		/* Display spell list */
		display_spell_list(creature_ptr);

		Term_fresh();
		Term_activate(old);
	}
}


/*
 * Hack -- display character in sub-windows
 */
static void fix_player(creature_type *player_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if(!angband_term[j]) continue;

		/* No relevant flags */
		if(!(window_flag[j] & (PW_PLAYER))) continue;

		Term_activate(angband_term[j]);

		update_play_time();

		/* Display player */
		display_creature_status(0, player_ptr);

		Term_fresh();

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
		if(!angband_term[j]) continue;

		/* No relevant flags */
		if(!(window_flag[j] & (PW_MESSAGE))) continue;

		Term_activate(angband_term[j]);
		Term_get_size(&w, &h);

		/* Dump messages */
		for (i = 0; i < h; i++)
		{
			/* Dump the message on the appropriate line */
			Term_putstr(0, (h - 1) - i, -1, (byte)((i < now_message) ? TERM_WHITE : TERM_SLATE), message_str((s16b)i));
			Term_locate(&x, &y);

			/* Clear to end of line */
			Term_erase(x, y, 255);
		}

		Term_fresh();
		Term_activate(old);
	}
}


/*
 * Hack -- display overhead view in sub-windows
 *
 * Note that the "player" symbol does NOT appear on the map.
 */
static void fix_overhead(creature_type *watcher_ptr)
{
	int j;

	int cy, cx;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;
		int wid, hgt;

		/* No window */
		if(!angband_term[j]) continue;

		/* No relevant flags */
		if(!(window_flag[j] & (PW_OVERHEAD))) continue;

		Term_activate(angband_term[j]);

		/* Full map in too small window is useless  */
		Term_get_size(&wid, &hgt);
		if(wid > COL_MAP + 2 && hgt > ROW_MAP + 2)
		{
			display_map(watcher_ptr, &cy, &cx);
			Term_fresh();
		}
		Term_activate(old);
	}
}


/*
 * Hack -- display dungeon view in sub-windows
 */
static void fix_dungeon(creature_type *creature_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if(!angband_term[j]) continue;

		/* No relevant flags */
		if(!(window_flag[j] & (PW_DUNGEON))) continue;

		Term_activate(angband_term[j]);
		display_dungeon(creature_ptr);
		Term_fresh();
		Term_activate(old);
	}
}


/*
 * Hack -- display creature recall in sub-windows
 */
static void fix_creature(creature_type *creature_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if(!angband_term[j]) continue;

		/* No relevant flags */
		if(!(window_flag[j] & (PW_MONSTER))) continue;
		Term_activate(angband_term[j]);

		/* Display creature race info */
		if(species_window_idx) display_roff(species_window_idx);

		Term_fresh();
		Term_activate(old);
	}
}


/*
 * Hack -- display object recall in sub-windows
 */
static void fix_object(creature_type *creature_ptr)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if(!angband_term[j]) continue;

		/* No relevant flags */
		if(!(window_flag[j] & (PW_OBJECT))) continue;

		Term_activate(angband_term[j]);

		/* Display creature race info */
		if(play_window_object_kind_idx) display_koff(creature_ptr, play_window_object_kind_idx);

		Term_fresh();
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
static void calc_spells(creature_type *creature_ptr, bool message)
{
	int i, j, k, levels;
	int num_allowed;
	int num_boukyaku = 0;
	magic_type		*s_ptr;
	int which;
	int bonus = 0;
	cptr p;

	/* Hack -- must be literate */
	if(!magic_info[creature_ptr->class_idx].spell_book) return;

	/* Hack -- wait for creation */
	if(!character_generated) return;

	if((creature_ptr->class_idx == CLASS_SORCERER) || (creature_ptr->class_idx == CLASS_RED_MAGE))
	{
		creature_ptr->new_spells = 0;
		return;
	}

	p = spell_category_name(magic_info[creature_ptr->class_idx].spell_book);

	/* Determine the number of spells allowed */
	levels = creature_ptr->lev - magic_info[creature_ptr->class_idx].spell_first + 1;

	/* Hack -- no negative spells */
	if(levels < 0) levels = 0;

	/* Extract total allowed spells */
	num_allowed = (adj_mag_study[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] * levels / 2);

	if((creature_ptr->class_idx != CLASS_SAMURAI) && (magic_info[creature_ptr->class_idx].spell_book != TV_LIFE_BOOK))
		bonus = 4;
	if(creature_ptr->class_idx == CLASS_SAMURAI)
		num_allowed = 32;
	else if(creature_ptr->realm2 == REALM_NONE)
	{
		num_allowed = (num_allowed+1)/2;
		if(num_allowed>(32+bonus)) num_allowed = 32+bonus;
	}
	else if((creature_ptr->class_idx == CLASS_MAGE) || (creature_ptr->class_idx == CLASS_PRIEST))
	{
		if(num_allowed>(96+bonus)) num_allowed = 96+bonus;
	}
	else
	{
		if(num_allowed>(80+bonus)) num_allowed = 80+bonus;
	}

	/* Count the number of spells we know */
	for (j = 0; j < (REALM_MAGIC_NUMBER * 2); j++)
	{
		/* Count known spells */
		if((j < 32) ? (creature_ptr->spell_forgotten1 & (1L << j)) : (creature_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			num_boukyaku++;
		}
	}

	/* See how many spells we must forget or may learn */
	creature_ptr->new_spells = num_allowed + creature_ptr->add_spells + num_boukyaku - creature_ptr->learned_spells;

	/* Forget spells which are too hard */

	for (i = 63; i >= 0; i--)
	{
		/* Efficiency -- all done */
		if(!creature_ptr->spell_learned1 && !creature_ptr->spell_learned2) break;

		/* Access the spell */
		j = creature_ptr->spell_order[i];

		/* Skip non-spells */
		if(j >= 99) continue;

		/* Get the spell */
		if(!is_magic((j < 32) ? creature_ptr->realm1 : creature_ptr->realm2))
		{
			if(j < 32)
				s_ptr = &technic_info[creature_ptr->realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[creature_ptr->realm2 - MIN_TECHNIC][j%32];
		}
		else if(j < 32)
			s_ptr = &magic_info[creature_ptr->class_idx].info[creature_ptr->realm1-1][j];
		else
			s_ptr = &magic_info[creature_ptr->class_idx].info[creature_ptr->realm2-1][j%32];

		/* Skip spells we are allowed to know */
		if(s_ptr->slevel <= creature_ptr->lev) continue;

		/* Is it known? */
		if((j < 32) ?
		    (creature_ptr->spell_learned1 & (1L << j)) :
		    (creature_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if(j < 32)
			{
				creature_ptr->spell_forgotten1 |= (1L << j);
				which = creature_ptr->realm1;
			}
			else
			{
				creature_ptr->spell_forgotten2 |= (1L << (j - 32));
				which = creature_ptr->realm2;
			}

			/* No longer known */
			if(j < 32)
			{
				creature_ptr->spell_learned1 &= ~(1L << j);
				which = creature_ptr->realm1;
			}
			else
			{
				creature_ptr->spell_learned2 &= ~(1L << (j - 32));
				which = creature_ptr->realm2;
			}

#ifdef JP
			if(message) msg_format("%sの%sを忘れてしまった。", do_spell(creature_ptr, which, j%32, SPELL_NAME), p );
#else
			if(message) msg_format("You have forgotten the %s of %s.", p, do_spell(creature_ptr, which, j%32, SPELL_NAME));
#endif


			/* One more can be learned */
			creature_ptr->new_spells++;
		}
	}


	/* Forget spells if we know too many spells */
	for (i = 63; i >= 0; i--)
	{
		/* Stop when possible */
		if(creature_ptr->new_spells >= 0) break;

		/* Efficiency -- all done */
		if(!creature_ptr->spell_learned1 && !creature_ptr->spell_learned2) break;

		/* Get the (i+1)th spell learned */
		j = creature_ptr->spell_order[i];

		/* Skip unknown spells */
		if(j >= 99) continue;

		/* Forget it (if learned) */
		if((j < 32) ?
		    (creature_ptr->spell_learned1 & (1L << j)) :
		    (creature_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if(j < 32)
			{
				creature_ptr->spell_forgotten1 |= (1L << j);
				which = creature_ptr->realm1;
			}
			else
			{
				creature_ptr->spell_forgotten2 |= (1L << (j - 32));
				which = creature_ptr->realm2;
			}

			/* No longer known */
			if(j < 32)
			{
				creature_ptr->spell_learned1 &= ~(1L << j);
				which = creature_ptr->realm1;
			}
			else
			{
				creature_ptr->spell_learned2 &= ~(1L << (j - 32));
				which = creature_ptr->realm2;
			}

#ifdef JP
			if(message) msg_format("%sの%sを忘れてしまった。", do_spell(creature_ptr, which, j%32, SPELL_NAME), p );
#else
			if(message) msg_format("You have forgotten the %s of %s.", p, do_spell(creature_ptr, which, j%32, SPELL_NAME));
#endif

			/* One more can be learned */
			creature_ptr->new_spells++;
		}
	}

	/* Check for spells to remember */
	for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++)
	{
		/* None left to remember */
		if(creature_ptr->new_spells <= 0) break;

		/* Efficiency -- all done */
		if(!creature_ptr->spell_forgotten1 && !creature_ptr->spell_forgotten2) break;

		/* Get the next spell we learned */
		j = creature_ptr->spell_order[i];

		/* Skip unknown spells */
		if(j >= 99) break;

		/* Access the spell */
		if(!is_magic((j < 32) ? creature_ptr->realm1 : creature_ptr->realm2))
		{
			if(j < 32) s_ptr = &technic_info[creature_ptr->realm1 - MIN_TECHNIC][j];
			else s_ptr = &technic_info[creature_ptr->realm2 - MIN_TECHNIC][j%32];
		}
		else if(j < 32)
			s_ptr = &magic_info[creature_ptr->class_idx].info[creature_ptr->realm1-1][j];
		else
			s_ptr = &magic_info[creature_ptr->class_idx].info[creature_ptr->realm2-1][j%32];

		/* Skip spells we cannot remember */
		if(s_ptr->slevel > creature_ptr->lev) continue;

		/* First set of spells */
		if((j < 32) ? (creature_ptr->spell_forgotten1 & (1L << j)) :
		    (creature_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			/* No longer forgotten */
			if(j < 32)
			{
				creature_ptr->spell_forgotten1 &= ~(1L << j);
				which = creature_ptr->realm1;
			}
			else
			{
				creature_ptr->spell_forgotten2 &= ~(1L << (j - 32));
				which = creature_ptr->realm2;
			}

			/* Known once more */
			if(j < 32)
			{
				creature_ptr->spell_learned1 |= (1L << j);
				which = creature_ptr->realm1;
			}
			else
			{
				creature_ptr->spell_learned2 |= (1L << (j - 32));
				which = creature_ptr->realm2;
			}

#ifdef JP
			if(message) msg_format("%sの%sを思い出した。", do_spell(creature_ptr, which, j%32, SPELL_NAME), p );
#else
			if(message) msg_format("You have remembered the %s of %s.", p, do_spell(creature_ptr, which, j%32, SPELL_NAME));
#endif


			/* One less can be learned */
			creature_ptr->new_spells--;
		}
	}

	k = 0;

	if(creature_ptr->realm2 == REALM_NONE)
	{
		/* Count spells that can be learned */
		for (j = 0; j < 32; j++)
		{
			if(!is_magic(creature_ptr->realm1)) s_ptr = &technic_info[creature_ptr->realm1-MIN_TECHNIC][j];
			else s_ptr = &magic_info[creature_ptr->class_idx].info[creature_ptr->realm1-1][j];

			/* Skip spells we cannot remember */
			if(s_ptr->slevel > creature_ptr->lev) continue;

			/* Skip spells we already know */
			if(creature_ptr->spell_learned1 & (1L << j)) continue;

			/* Count it */
			k++;
		}
		if(k > 32) k = 32;
		if((creature_ptr->new_spells > k) && ((magic_info[creature_ptr->class_idx].spell_book == TV_LIFE_BOOK) || (magic_info[creature_ptr->class_idx].spell_book == TV_HISSATSU_BOOK))) creature_ptr->new_spells = k;
	}

	if(creature_ptr->new_spells < 0) creature_ptr->new_spells = 0;

	/* Spell count changed */
	if(creature_ptr->old_spells != creature_ptr->new_spells)
	{
		/* Message if needed */
		if(creature_ptr->new_spells)
		{
#ifdef JP
			if( creature_ptr->new_spells < 10 )
				if(message) msg_format("あと %d つの%sを学べる。", creature_ptr->new_spells, p);
			else
				if(message) msg_format("あと %d 個の%sを学べる。", creature_ptr->new_spells, p);
#else
			if(message) msg_format("You can learn %d more %s%s.", creature_ptr->new_spells, p, (creature_ptr->new_spells != 1) ? "s" : "");
#endif
		}

		/* Save the new_spells value */
		creature_ptr->old_spells = creature_ptr->new_spells;

		play_redraw |= (PR_STUDY);
		play_window |= (PW_OBJECT);
	}
}


/*
 * Calculate maximum mana.  You do not need to know any spells.
 * Note that mana is lowered by heavy (or inappropriate) armor.
 *
 * This function induces status messages.
 */
static void calc_mana(creature_type *creature_ptr, bool message)
{
	int		msp, levels, cur_wgt, max_wgt;

	object_type	*object_ptr;

	levels = creature_ptr->lev;

	switch(creature_ptr->class_idx)
	{
		case CLASS_SAMURAI:
			msp = (adj_mag_mana[creature_ptr->stat_ind[magic_info[creature_ptr->class_idx].spell_stat]] + 10) * 2;
			if(msp) msp += (msp * race_info[creature_ptr->race_idx1].r_adj[magic_info[creature_ptr->class_idx].spell_stat] / 20);
			break;

		default:
			/* Extract total mana */
			msp = adj_mag_mana[creature_ptr->stat_ind[STAT_INT]] * (levels + 3) / 4;

			/* Hack -- usually add one mana */
			msp++;

			if(msp) msp += (msp * race_info[creature_ptr->race_idx1].r_adj[magic_info[creature_ptr->class_idx].spell_stat] / 20);

			if(msp && (creature_ptr->chara_idx == CHARA_MUNCHKIN)) msp += msp / 2;

			/* Hack: High mages have a 25% mana bonus */
			if(msp && (creature_ptr->class_idx == CLASS_MAGE)) msp += msp / 6;
			if(msp && (creature_ptr->class_idx == CLASS_HIGH_MAGE)) msp += msp / 4;
	}

	/* Only mages are affected */
	if(magic_info[creature_ptr->class_idx].spell_xtra & MAGIC_GLOVE_REDUCE_MANA)
	{
		u32b flgs[TRAIT_FLAG_MAX];

		/* Assume player is not encumbered by gloves */
		creature_ptr->cumber_glove = FALSE;

		/* Get the gloves */
		object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_ARMS, 0);

		/* Examine the gloves */
		object_flags(object_ptr, flgs);

		/* Normal gloves hurt mage-type spells */
		if(object_ptr->k_idx &&
		    !(have_flag(flgs, TRAIT_FREE_ACTION)) &&
		    !(have_flag(flgs, TRAIT_MAGIC_MASTERY)) &&
		    !((have_flag(flgs, STAT_DEX)) && (object_ptr->pval > 0)))
		{
			/* Encumbered */
			creature_ptr->cumber_glove = TRUE;

			/* Reduce mana */
			msp = (3 * msp) / 4;
		}
	}


	/* Assume player not encumbered by armor */
	creature_ptr->cumber_armor = FALSE;

	/* Weigh the armor */
	cur_wgt = 0;


	/*TODO:ADJUST
	if(creature_ptr->inventory[].tval> TV_SWORD) cur_wgt += creature_ptr->inventory[].weight;
	if(creature_ptr->inventory[].tval> TV_SWORD) cur_wgt += creature_ptr->inventory[].weight;
	cur_wgt += creature_ptr->inventory[].weight;
	cur_wgt += creature_ptr->inventory[].weight;
	//TODO cur_wgt += creature_ptr->inventory[].weight;
	cur_wgt += creature_ptr->inventory[].weight;
	cur_wgt += creature_ptr->inventory[].weight;

	// Subtract a percentage of maximum mana.
	switch (creature_ptr->class_idx)
	{
		// For these classes, mana is halved if armour 
		// is 30 pounds over their weight limit.
		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_BLUE_MAGE:
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		case CLASS_SORCERER:
		{
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight;
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight;
			break;
		}

		// Mana halved if armour is 40 pounds over weight limit.
		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_TOURIST:
		{
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight*2/3;
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight*2/3;
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_BEASTMASTER:
		case CLASS_MIRROR_MASTER:
		{
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight/2;
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight/2;
			break;
		}

		// Mana halved if armour is 50 pounds over weight limit.
		case CLASS_ROGUE:
		case CLASS_RANGER:
		case CLASS_RED_MAGE:
		case CLASS_WARRIOR_MAGE:
		{
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight/3;
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight/3;
			break;
		}

		// Mana halved if armour is 60 pounds over weight limit.
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		{
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight/5;
			if(creature_ptr->inventory[].tval <= TV_SWORD) cur_wgt += creature_ptr->inventory[].weight/5;
			break;
		}

		// For new classes created, but not yet added to this formula.
		default:
		{
			break;
		}
	}
	*/

	// Determine the weight allowance
	max_wgt = magic_info[creature_ptr->class_idx].spell_weight;

	// Heavy armor penalizes mana by a percentage.  -LM-
	if((cur_wgt - max_wgt) > 0)
	{
		creature_ptr->cumber_armor = TRUE; // Encumbered
		msp -= msp * (cur_wgt - max_wgt) / 800; // Subtract a percentage of maximum mana.
	}

	if(msp < 0) msp = 0; // Mana can never be negative

	
	if(creature_ptr->msp != msp) // Maximum mana has changed
	{
		// Enforce maximum
		if((creature_ptr->csp >= msp) && (creature_ptr->class_idx != CLASS_SAMURAI))
		{
			creature_ptr->csp = msp;
			creature_ptr->csp_frac = 0;
		}

#ifdef JP
		if(creature_ptr->level_up && (msp > creature_ptr->msp))
			if(message) msg_format("最大マジック・ポイントが %d 増加した！", (msp - creature_ptr->msp));
#endif
		
		creature_ptr->msp = msp; // Save new mana
		play_redraw |= (PR_MANA | PW_PLAYER | PW_SPELL); // Display mana later
	}

	/* Take note when "glove state" changes */
	if(creature_ptr->old_cumber_glove != creature_ptr->cumber_glove)
	{
		if(creature_ptr->cumber_glove)
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
		creature_ptr->old_cumber_glove = creature_ptr->cumber_glove;
	}


	/* Take note when "armor state" changes */
	if(creature_ptr->old_cumber_armor != creature_ptr->cumber_armor)
	{
		if(creature_ptr->cumber_armor)
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
		creature_ptr->old_cumber_armor = creature_ptr->cumber_armor;
	}
}



/*
 * Calculate the players (maximal) hit points
 * Adjust current hitpoints if necessary
 */
static void calc_hitpoints(creature_type *creature_ptr, bool message)
{
	int bonus, mhp;

	/* Un-inflate "half-hitpoint bonus per level" value */
	bonus = ((int)(adj_con_mhp[creature_ptr->stat_ind[STAT_CON]])) * creature_ptr->lev / 4;

	// Divine Bonuses
	if(creature_ptr->dr >= 0) bonus += adj_dr_mhp[creature_ptr->dr] * creature_ptr->hitdice;

	/* Calculate hitpoints */
	mhp = creature_ptr->base_hp[creature_ptr->lev - 1];

	// TODO MIMIC FORM
	if(has_trait(creature_ptr, TRAIT_COMPENSATION_OF_MAGI))
	{
		if(creature_ptr->lev < 30) mhp = (mhp * (45 + creature_ptr->lev) / 100);
		else mhp = (mhp * 75 / 100);
		bonus = (bonus * 65 / 100);
	}

	mhp += bonus;

	if(creature_ptr->class_idx == CLASS_BERSERKER)
		mhp = mhp * (110+ (((creature_ptr->lev + 40) * (creature_ptr->lev + 40) - 1550) / 110)) / 100;

	/* Always have at least one hitpoint per level */
	if(mhp < creature_ptr->lev + 1) mhp = creature_ptr->lev + 1;

	/* Factor in the hero / superhero settings */
	if(IS_HERO(creature_ptr)) mhp += 10;
	if(creature_ptr->timed_trait[TRAIT_S_HERO] && (creature_ptr->class_idx != CLASS_BERSERKER)) mhp += 30;
	if(creature_ptr->timed_trait[TRAIT_TSUYOSHI]) mhp += 50;

	/* Factor in the hex spell settings */
	if(HEX_SPELLING(creature_ptr, HEX_XTRA_MIGHT)) mhp += 15;
	if(HEX_SPELLING(creature_ptr, HEX_BUILDING)) mhp += 60;

	/* New maximum hitpoints */
	if(creature_ptr->mhp != mhp)
	{
		/* Enforce maximum */
		if(creature_ptr->chp >= mhp)
		{
			creature_ptr->chp = mhp;
			creature_ptr->chp_frac = 0;
		}

#ifdef JP
		if(creature_ptr->level_up && (mhp > creature_ptr->mhp)) 
			if(message) msg_format("最大ヒット・ポイントが %d 増加した！", (mhp - creature_ptr->mhp) );
#endif
		/* Save the new max-hitpoints */
		creature_ptr->mhp = mhp;
		creature_ptr->mmhp = mhp;

		/* Display hitpoints (later) */
		play_redraw |= (PR_HP);

		play_window |= (PW_PLAYER);
	}
}



/*
 * Extract and set the current "lite radius"
 *
 * SWD: Experimental modification: multiple light sources have additive effect.
 *
 */
static void calc_lite(creature_type *creature_ptr)
{
	int i;
	object_type *object_ptr;
	u32b flgs[TRAIT_FLAG_MAX];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Assume no light
	creature_ptr->cur_lite = 0;

	// Loop through all wielded items
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		// Examine actual lites
		if(GET_INVEN_SLOT_TYPE(creature_ptr, i) == INVEN_SLOT_LITE && (object_ptr->k_idx) && (object_ptr->tval == TV_LITE))
		{
			if(object_ptr->name2 == EGO_LITE_DARKNESS)
			{
				if(object_ptr->sval == SV_LITE_TORCH) creature_ptr->cur_lite -= 2;
				else if(object_ptr->sval == SV_LITE_LANTERN) creature_ptr->cur_lite -= 2;
				else if(object_ptr->sval == SV_LITE_FEANOR) creature_ptr->cur_lite -= 3;
			}
			/* Torches (with fuel) provide some lite */
			else if((object_ptr->sval == SV_LITE_TORCH) && (object_ptr->xtra4 > 0))
				creature_ptr->cur_lite += 2;

			/* Lanterns (with fuel) provide more lite */
			else if((object_ptr->sval == SV_LITE_LANTERN) && (object_ptr->xtra4 > 0))
				creature_ptr->cur_lite += 2;

			else if(object_ptr->sval == SV_LITE_FEANOR || object_ptr->sval == SV_LITE_UDUN)
				creature_ptr->cur_lite += 3;

			/* Artifact Lites provide permanent, bright, lite */
			else if(object_is_fixed_artifact(object_ptr))
				creature_ptr->cur_lite += 4;

			if(object_ptr->name2 == EGO_LITE_SHINE) creature_ptr->cur_lite++;
		}
		else
		{
			/* Skip empty slots */
			if(!is_valid_object(object_ptr)) continue;

			/* Extract the flags */
			object_flags(object_ptr, flgs);

			// does this item glow?
			if(have_flag(flgs, TRAIT_LITE))
			{
				//TODO if((object_ptr->name2 == EGO_DARK) || (object_ptr->name1 == ART_NIGHT)) creature_ptr->cur_lite--;
				creature_ptr->cur_lite++;
			}
		}

	}

	/* max radius is 14 (was 5) without rewriting other code -- */
	/* see cave.c:update_lite() and defines.h:LITE_MAX */
	if(dungeon_info[floor_ptr->dun_type].flags1 & DF1_DARKNESS && creature_ptr->cur_lite > 1)
		creature_ptr->cur_lite = 1;

	/*
	 * check if the player doesn't have light radius, 
	 * but does weakly glow as an intrinsic.
	 */
	//TODO if(creature_ptr->cur_lite <= 0 && creature_ptr->lite) creature_ptr->cur_lite++;

	if(creature_ptr->cur_lite > 14) creature_ptr->cur_lite = 14;
	if(creature_ptr->cur_lite < 0) creature_ptr->cur_lite = 0;

	/* end experimental mods */

	/* Notice changes in the "lite radius" */
	if(creature_ptr->old_lite != creature_ptr->cur_lite)
	{
		/* Hack -- PU_SPECIES_LITE for creatures' darkness */
		creature_ptr->creature_update |= (PU_LITE | PU_SPECIES_LITE | PU_CREATURES);

		/* Remember the old lite */
		creature_ptr->old_lite = creature_ptr->cur_lite;

		if((creature_ptr->cur_lite > 0) && (creature_ptr->posture & NINJA_S_STEALTH)) set_superstealth(creature_ptr, FALSE);
	}
}


static void set_race_bonuses(creature_type *creature_ptr)
{
	int i;

	species_type *species_ptr = &species_info[creature_ptr->species_idx];
	race_type *race1_ptr  = &race_info[creature_ptr->race_idx1];
	race_type *race2_ptr = &race_info[creature_ptr->race_idx2];
	race_type *mimic_ptr = &race_info[creature_ptr->mimic_race_idx];

	for (i = 0; i < STAT_MAX; i++)
	{
		if(IS_MIMICED(creature_ptr))
			creature_ptr->stat_add[i] += mimic_ptr->r_adj[i] * STAT_FRACTION;
		else if(IS_PURE(creature_ptr))
			creature_ptr->stat_add[i] += race1_ptr->r_adj[i] * STAT_FRACTION;
		else
		{
			creature_ptr->stat_add[i] += race1_ptr->r_s_adj[i] * STAT_FRACTION;
			creature_ptr->stat_add[i] += race1_ptr->r_s_adj[i] * STAT_FRACTION;
		}
	}

	if(IS_MIMICED(creature_ptr))
	{
		creature_ptr->skill_dis += mimic_ptr->r_dis;
		creature_ptr->skill_dev += mimic_ptr->r_dev;
		creature_ptr->skill_stl += mimic_ptr->r_stl;
		creature_ptr->skill_srh += mimic_ptr->r_srh;
		creature_ptr->skill_fos += mimic_ptr->r_fos;
		creature_ptr->skill_thn += mimic_ptr->r_thn;
		creature_ptr->skill_thb += mimic_ptr->r_thb;
		creature_ptr->skill_tht += mimic_ptr->r_thb;

		creature_ptr->to_ac += mimic_ptr->ac_base + (mimic_ptr->ac_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_ac += mimic_ptr->ac_base + (mimic_ptr->ac_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->to_ev += mimic_ptr->ev_base + (mimic_ptr->ev_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_ev += mimic_ptr->ev_base + (mimic_ptr->ev_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->to_vo += mimic_ptr->ev_base + (mimic_ptr->vo_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_vo += mimic_ptr->ev_base + (mimic_ptr->vo_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
	}
	else if(IS_PURE(creature_ptr))
	{
		creature_ptr->skill_dis += race1_ptr->r_dis;
		creature_ptr->skill_dev += race1_ptr->r_dev;
		creature_ptr->skill_stl += race1_ptr->r_stl;
		creature_ptr->skill_srh += race1_ptr->r_srh;
		creature_ptr->skill_fos += race1_ptr->r_fos;
		creature_ptr->skill_thn += race1_ptr->r_thn;
		creature_ptr->skill_thb += race1_ptr->r_thb;
		creature_ptr->skill_tht += race1_ptr->r_thb;

		creature_ptr->to_ac += race1_ptr->ac_base + (race1_ptr->ac_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_ac += race1_ptr->ac_base + (race1_ptr->ac_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->to_ev += race1_ptr->ev_base + (race1_ptr->ev_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_ev += race1_ptr->ev_base + (race1_ptr->ev_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->to_vo += race1_ptr->vo_base + (race1_ptr->vo_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_vo += race1_ptr->vo_base + (race1_ptr->vo_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);

	}
	else
	{
		creature_ptr->skill_dis += race1_ptr->r_s_dis + race2_ptr->r_s_dis;
		creature_ptr->skill_dev += race1_ptr->r_s_dev + race2_ptr->r_s_dev;
		creature_ptr->skill_stl += race1_ptr->r_s_stl + race2_ptr->r_s_stl;
		creature_ptr->skill_srh += race1_ptr->r_s_srh + race2_ptr->r_s_srh;
		creature_ptr->skill_fos += race1_ptr->r_s_fos + race2_ptr->r_s_fos;
		creature_ptr->skill_thn += race1_ptr->r_s_thn + race2_ptr->r_s_thn;
		creature_ptr->skill_thb += race1_ptr->r_s_thb + race2_ptr->r_s_thb;
		creature_ptr->skill_tht += race1_ptr->r_s_thb + race2_ptr->r_s_thb;

		creature_ptr->to_ac += race1_ptr->ac_s_base + (race1_ptr->ac_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_ac += race1_ptr->ac_s_base + (race1_ptr->ac_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->to_ac += race2_ptr->ac_s_base + (race2_ptr->ac_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_ac += race2_ptr->ac_s_base + (race2_ptr->ac_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);

		creature_ptr->to_ev += race1_ptr->ev_s_base + (race1_ptr->ev_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_ev += race1_ptr->ev_s_base + (race1_ptr->ev_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->to_ev += race2_ptr->ev_s_base + (race2_ptr->ev_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_ev += race2_ptr->ev_s_base + (race2_ptr->ev_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);

		creature_ptr->to_vo += race1_ptr->vo_s_base + (race1_ptr->vo_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_vo += race1_ptr->vo_s_base + (race1_ptr->vo_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->to_vo += race2_ptr->vo_s_base + (race2_ptr->vo_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);
		creature_ptr->dis_to_vo += race2_ptr->vo_s_base + (race2_ptr->vo_s_plus * (creature_ptr->lev < 30 ? creature_ptr->lev : 30 ) / 30);

	}

	// Species
	creature_ptr->ac += species_ptr->ac;
	creature_ptr->dis_ac += species_ptr->ac;
	creature_ptr->ev += species_ptr->ev;
	creature_ptr->dis_ev += species_ptr->ev;
	creature_ptr->vo += species_ptr->vo;
	creature_ptr->dis_vo += species_ptr->vo;
	creature_ptr->speed += species_ptr->speed;

	creature_ptr->see_infra += MAX(race1_ptr->infra, race2_ptr->infra); // Base infravision (purely racial)
}


static void set_class_bonuses(creature_type *creature_ptr)
{
	int i;
	class_type *class_ptr = &class_info[creature_ptr->class_idx];

	for (i = 0; i < STAT_MAX; i++)
	{
		creature_ptr->stat_add[i] += class_ptr->c_adj[i] * STAT_FRACTION;
		if(creature_ptr->cls_bonus) creature_ptr->stat_add[i] += class_ptr->c_adj_b[i] * STAT_FRACTION;
	}

	creature_ptr->skill_dis += class_ptr->c_dis;
	creature_ptr->skill_dev += class_ptr->c_dev;
	creature_ptr->skill_stl += class_ptr->c_stl;
	creature_ptr->skill_srh += class_ptr->c_srh;
	creature_ptr->skill_fos += class_ptr->c_fos;
	creature_ptr->skill_thn += class_ptr->c_thn;
	creature_ptr->skill_thb += class_ptr->c_thb;
	creature_ptr->skill_tht += class_ptr->c_thb;

	if(has_trait(creature_ptr, TRAIT_ORDINARILY_BERSERK))
	{
		creature_ptr->timed_trait[TRAIT_S_HERO] = 1;
		//TODO has_trait(creature_ptr, TRAIT_FREE_ACTION) = TRUE;
		creature_ptr->speed += 2;

		if(creature_ptr->lev > 29) creature_ptr->speed++;
		if(creature_ptr->lev > 39) creature_ptr->speed++;
		if(creature_ptr->lev > 44) creature_ptr->speed++;
		if(creature_ptr->lev > 49) creature_ptr->speed++;

		creature_ptr->to_ac += 10 + creature_ptr->lev / 2;
		creature_ptr->dis_to_ac += 10 + creature_ptr->lev / 2;
		creature_ptr->skill_dig += (100 + creature_ptr->lev * 8);

		play_redraw |= PR_STATUS;
	}

	switch (creature_ptr->class_idx)
	{
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
			if(!(heavy_armor(creature_ptr))) // Unencumbered Monks become faster every 10 levels
			{
				if(!(has_trait(creature_ptr, TRAIT_AGILE_RACE)))
					creature_ptr->speed += (creature_ptr->lev) / 10;
				//TODO if  (creature_ptr->lev > 24) has_trait(creature_ptr, TRAIT_FREE_ACTION) = TRUE; // Free action if unencumbered at level 25
			}
			break;

		case CLASS_NINJA:
			// Unencumbered Ninjas become faster every 10 levels
			if(heavy_armor(creature_ptr))
			{
				creature_ptr->speed -= (creature_ptr->lev) / 10;
				creature_ptr->skill_stl -= (creature_ptr->lev)/10;
			}
			else if((!get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 0)->k_idx || creature_ptr->can_melee[0]) &&
			         (!get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 1)->k_idx || creature_ptr->can_melee[1]))
			{
				creature_ptr->speed += 3;
				if(!has_trait(creature_ptr, TRAIT_AGILE_RACE)) creature_ptr->speed += (creature_ptr->lev) / 10;
				creature_ptr->skill_stl += (creature_ptr->lev)/10;
			}
			if((!get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 0)->k_idx || creature_ptr->can_melee[0]) &&
			    (!get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 1)->k_idx || creature_ptr->can_melee[1]))
			{
				creature_ptr->to_ac += creature_ptr->lev / 2 + 5;
				creature_ptr->dis_to_ac += creature_ptr->lev / 2 + 5;
			}

			break;
	}

	// Affect Skill (Level, by Class)
	creature_ptr->skill_dis += (class_ptr->x_dis * creature_ptr->lev / 10);
	creature_ptr->skill_dev += (class_ptr->x_dev * creature_ptr->lev / 10);
	creature_ptr->skill_stl += (class_ptr->x_stl * creature_ptr->lev / 10);
	creature_ptr->skill_srh += (class_ptr->x_srh * creature_ptr->lev / 10);
	creature_ptr->skill_fos += (class_ptr->x_fos * creature_ptr->lev / 10);
	creature_ptr->skill_thn += (class_ptr->x_thn * creature_ptr->lev / 10);
	creature_ptr->skill_thb += (class_ptr->x_thb * creature_ptr->lev / 10);
	creature_ptr->skill_tht += (class_ptr->x_thb * creature_ptr->lev / 10);
}

static void set_character_bonuses(creature_type *creature_ptr)
{
	int i;
	chara_type *chara_ptr = &chara_info[creature_ptr->chara_idx];

	for (i = 0; i < STAT_MAX; i++) creature_ptr->stat_add[i] += chara_ptr->a_adj[i] * STAT_FRACTION;

	creature_ptr->skill_dis += chara_ptr->a_dis;
	creature_ptr->skill_dev += chara_ptr->a_dev;
	creature_ptr->skill_stl += chara_ptr->a_stl;
	creature_ptr->skill_srh += chara_ptr->a_srh;
	creature_ptr->skill_fos += chara_ptr->a_fos;
	creature_ptr->skill_thn += chara_ptr->a_thn;
	creature_ptr->skill_thb += chara_ptr->a_thb;
	creature_ptr->skill_tht += chara_ptr->a_thb;
	creature_ptr->to_m_chance += chara_ptr->m_pena;

	if(creature_ptr->chara_idx == CHARA_MUNCHKIN)
	{
		//TODO if(creature_ptr->class_idx != CLASS_NINJA) creature_ptr->lite = TRUE;
		creature_ptr->speed += (creature_ptr->lev) / 8 + 5; // Munchkin become faster
	}

	creature_ptr->skill_dis += (chara_ptr->a_dis * creature_ptr->lev / 50);
	creature_ptr->skill_dev += (chara_ptr->a_dev * creature_ptr->lev / 50);
	creature_ptr->skill_thn += (chara_ptr->a_thn * creature_ptr->lev / 50);
	creature_ptr->skill_thb += (chara_ptr->a_thb * creature_ptr->lev / 50);
	creature_ptr->skill_tht += (chara_ptr->a_thb * creature_ptr->lev / 50);
}

static void set_posture_bonuses(creature_type *creature_ptr)
{
	u32b limit;
	int empty_hands_status = empty_hands(creature_ptr, TRUE);

	if(creature_ptr->posture & KAMAE_GENBU | KAMAE_BYAKKO | KAMAE_SEIRYU | KAMAE_SUZAKU)
	{
		if(!(empty_hands_status & EMPTY_HAND_RARM))
			set_action(creature_ptr, ACTION_NONE);
	}

	if(has_trait(creature_ptr, TRAIT_WANT_LIGHT_WEIGHT) && !heavy_armor(creature_ptr))
	{
		/* TODO Monks get extra ac for armour _not worn_
		if(!(creature_ptr->inventory[INVEN_SLOT_BODY].k_idx))
		{
			creature_ptr->to_ac += (creature_ptr->lev * 3) / 2;
			creature_ptr->dis_to_ac += (creature_ptr->lev * 3) / 2;
		}
		if(!(creature_ptr->inventory[INVEN_SLOT_OUTER].k_idx) && (creature_ptr->lev > 15))
		{
			creature_ptr->to_ac += ((creature_ptr->lev - 13) / 3);
			creature_ptr->dis_to_ac += ((creature_ptr->lev - 13) / 3);
		}
		if(!(creature_ptr->inventory[INVEN_SLOT_2NDARM].k_idx) && (creature_ptr->lev > 10))
		{
			creature_ptr->to_ac += ((creature_ptr->lev - 8) / 3);
			creature_ptr->dis_to_ac += ((creature_ptr->lev - 8) / 3);
		}
		if(!(creature_ptr->inventory[INVEN_SLOT_1STHEAD].k_idx) && (creature_ptr->lev > 4))
		{
			creature_ptr->to_ac += (creature_ptr->lev - 2) / 3;
			creature_ptr->dis_to_ac += (creature_ptr->lev -2) / 3;
		}
		if(!(creature_ptr->inventory[INVEN_SLOT_1STHANDS].k_idx))
		{
			creature_ptr->to_ac += (creature_ptr->lev / 2);
			creature_ptr->dis_to_ac += (creature_ptr->lev / 2);
		}
		if(!(creature_ptr->inventory[INVEN_SLOT_FEET].k_idx))
		{
			creature_ptr->to_ac += (creature_ptr->lev / 3);
			creature_ptr->dis_to_ac += (creature_ptr->lev / 3);
		}
		*/
	}

	limit = calc_carrying_weight_limit(creature_ptr);
	if(creature_ptr->carrying_weight > limit) creature_ptr->speed -= (s16b)((creature_ptr->carrying_weight - limit) / (limit / 5 + 1));
	if(creature_ptr->action == ACTION_SEARCH) creature_ptr->speed -= 10; // Searching slows the player down
}

static void set_status_table_indexes(creature_type *creature_ptr)
{
	int i;

	// Calculate stats
	for (i = 0; i < STAT_MAX; i++)
	{
		int top, use, ind;

		// Extract the new "stat_use" value for the stat
		top = creature_ptr->stat_max[i] + creature_ptr->stat_add[i];
		if(creature_ptr->stat_top[i] != top) // Notice changes
		{
			creature_ptr->stat_top[i] = top; // Save the new value
			play_redraw |= (PR_STATS); // Redisplay the stats later
			play_window |= (PW_PLAYER); // Window stuff
		}

		// Extract the new "stat_use" value for the stat
		use = creature_ptr->stat_cur[i] + creature_ptr->stat_add[i];
		if((i == STAT_CHA) && (has_trait(creature_ptr, TRAIT_ILL_NORM)))
		{
			// 10.0 to 34.0 charisma, guaranteed, based on level
			if(use < 100 + 4 * creature_ptr->lev) use = 100 + 4 * creature_ptr->lev;
		}

		// Notice changes
		if(creature_ptr->stat_use[i] != use)
		{
			creature_ptr->stat_use[i] = use; // Save the new value
			play_redraw |= (PR_STATS);  // Redisplay the stats later
			play_window |= (PW_PLAYER); // Window stuff
		}

		ind = use / 10;

		// Notice changes
		if(creature_ptr->stat_ind[i] != ind)
		{
			creature_ptr->stat_ind[i] = ind; // Save the new index
			if(i == STAT_CON) creature_ptr->creature_update |= (CRU_HP); // Change in CON affects Hitpoints

			else if(i == STAT_INT) // Change in INT may affect Mana/Spells
			{
				if(magic_info[creature_ptr->class_idx].spell_stat == STAT_INT)
					creature_ptr->creature_update |= (CRU_MANA | CRU_SPELLS);
			}
			else if(i == STAT_WIS) // Change in WIS may affect Mana/Spells
			{
				if(magic_info[creature_ptr->class_idx].spell_stat == STAT_WIS)
					creature_ptr->creature_update |= (CRU_MANA | CRU_SPELLS);
			}
			else if(i == STAT_CHA) // Change in WIS may affect Mana/Spells
			{
				if(magic_info[creature_ptr->class_idx].spell_stat == STAT_CHA)
					creature_ptr->creature_update |= (CRU_MANA | CRU_SPELLS);
			}
			play_window |= (PW_PLAYER); // Window stuff
		}
	}

	/* Affect Skill -- disarming (DEX and INT) */
	creature_ptr->skill_dis += adj_dex_dis[creature_ptr->stat_ind[STAT_DEX]];
	creature_ptr->skill_dis += adj_int_dis[creature_ptr->stat_ind[STAT_INT]];

	/* Affect Skill -- magic devices (INT) */
	creature_ptr->skill_dev += adj_int_dev[creature_ptr->stat_ind[STAT_INT]];

	/* Affect Skill -- saving throw (WIS) */
	//TODO creature_ptr->skill_rob += (adj_sav[creature_ptr->stat_ind[STAT_STR]] + adj_sav[creature_ptr->stat_ind[STAT_CON]]);
	//TODO creature_ptr->skill_eva += (adj_sav[creature_ptr->stat_ind[STAT_INT]] + adj_sav[creature_ptr->stat_ind[STAT_DEX]]);
	//TODO creature_ptr->skill_vol += (adj_sav[creature_ptr->stat_ind[STAT_WIS]] + adj_sav[creature_ptr->stat_ind[STAT_CHA]]);

	/* Affect Skill -- digging (STR) */
	creature_ptr->skill_dig += adj_str_dig[creature_ptr->stat_ind[STAT_STR]];
}

static void set_state_bonuses(creature_type *creature_ptr)
{
	int i;
	object_type *object_ptr;

	if(creature_ptr->food >= CREATURE_FOOD_MAX) creature_ptr->speed -= 10; // Bloating slows the player down

	// Hex bonuses
	if(creature_ptr->realm1 == REALM_HEX)
	{
		if(HEX_SPELLING_ANY(creature_ptr)) creature_ptr->skill_stl -= (1 + creature_ptr->class_skills.old_skills.magic_num2[0]);
		if(HEX_SPELLING(creature_ptr, HEX_DETECT_EVIL));//TODO creature_ptr->esp_evil = TRUE;
		if(HEX_SPELLING(creature_ptr, HEX_XTRA_MIGHT)) creature_ptr->stat_add[STAT_STR] += 40;
		if(HEX_SPELLING(creature_ptr, HEX_BUILDING))
		{
			creature_ptr->stat_add[STAT_STR] += 40;
			creature_ptr->stat_add[STAT_DEX] += 40;
			creature_ptr->stat_add[STAT_CON] += 40;
		}
		if(HEX_SPELLING(creature_ptr, HEX_DEMON_AURA))
		{
			//TODO creature_ptr->sh_fire = TRUE;
			//TODO creature_ptr->regenerate = TRUE;
		}
		if(HEX_SPELLING(creature_ptr, HEX_ICE_ARMOR))
		{
			//creature_ptr->sh_cold = TRUE; 
			creature_ptr->to_ac += 30;
			creature_ptr->dis_to_ac += 30;
		}
		if(HEX_SPELLING(creature_ptr, HEX_SHOCK_CLOAK))
		{
			//TODO creature_ptr->sh_elec = TRUE;
			creature_ptr->speed += 3;
		}

		for (i = 0; i <= INVEN_TOTAL; i++)
		{
			int ac = 0;
			object_ptr = &creature_ptr->inventory[i];
			if(IS_EQUIPPED(object_ptr)) continue; 
			if(!is_valid_object(object_ptr)) continue;
			if(!object_is_armour(object_ptr)) continue;
			if(!object_is_cursed(object_ptr)) continue;
			ac += 5;
			if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE)) ac += 7;
			if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE)) ac += 13;
			creature_ptr->to_ac += ac;
			creature_ptr->dis_to_ac += ac;
		}
	}

	// Apply temporary "stun"
	if(creature_ptr->timed_trait[TRAIT_STUN] > 50)
	{
		creature_ptr->to_hit[0] -= 20;
		creature_ptr->to_hit[1] -= 20;
		creature_ptr->to_hit_b  -= 20;
		creature_ptr->to_hit_m  -= 20;
		creature_ptr->dis_to_hit[0] -= 20;
		creature_ptr->dis_to_hit[1] -= 20;
		creature_ptr->dis_to_hit_b  -= 20;
		creature_ptr->to_damage[0] -= 20;
		creature_ptr->to_damage[1] -= 20;
		creature_ptr->to_damage_m -= 20;
		creature_ptr->dis_to_damage[0] -= 20;
		creature_ptr->dis_to_damage[1] -= 20;
	}
	else if(has_trait(creature_ptr, TRAIT_STUN))
	{
		creature_ptr->to_hit[0] -= 5;
		creature_ptr->to_hit[1] -= 5;
		creature_ptr->to_hit_b -= 5;
		creature_ptr->to_hit_m -= 5;
		creature_ptr->dis_to_hit[0] -= 5;
		creature_ptr->dis_to_hit[1] -= 5;
		creature_ptr->dis_to_hit_b -= 5;
		creature_ptr->to_damage[0] -= 5;
		creature_ptr->to_damage[1] -= 5;
		creature_ptr->to_damage_m -= 5;
		creature_ptr->dis_to_damage[0] -= 5;
		creature_ptr->dis_to_damage[1] -= 5;
	}

	// Wraith form
	if(has_trait(creature_ptr, TRAIT_WRAITH_FORM))
	{
		//TODO has_trait(creature_ptr, TRAIT_REFLECTING) = TRUE;
	}

	if(has_trait(creature_ptr, TRAIT_MAGIC_DEF))
	{
		//has_trait(creature_ptr, TRAIT_NO_BLIND) = TRUE;
		//TODO creature_ptr->resist_conf = TRUE;
		//TODO has_trait(creature_ptr, TRAIT_REFLECTING) = TRUE;
		//TODO has_trait(creature_ptr, TRAIT_FREE_ACTION) = TRUE;
		//creature_ptr->levitation = TRUE;
	}

	/* Temporary "Beserk" */
	if(has_trait(creature_ptr, TRAIT_S_HERO))
	{
		creature_ptr->to_hit[0] += 12;
		creature_ptr->to_hit[1] += 12;
		creature_ptr->to_hit_b  -= 12;
		creature_ptr->to_hit_m  += 12;
		creature_ptr->to_damage[0] += 3+(creature_ptr->lev/5);
		creature_ptr->to_damage[1] += 3+(creature_ptr->lev/5);
		creature_ptr->to_damage_m  += 3+(creature_ptr->lev/5);
		creature_ptr->dis_to_hit[0] += 12;
		creature_ptr->dis_to_hit[1] += 12;
		creature_ptr->dis_to_hit_b  -= 12;
		creature_ptr->dis_to_damage[0] += 3+(creature_ptr->lev/5);
		creature_ptr->dis_to_damage[1] += 3+(creature_ptr->lev/5);
		creature_ptr->skill_dig += 30;
	}

	/* Hack -- Hero/Shero -> Res fear */
	if(has_trait(creature_ptr, TRAIT_HERO) || has_trait(creature_ptr, TRAIT_S_HERO))
	{
		//TODO creature_ptr->resist_fear = TRUE;
	}

	if(has_trait(creature_ptr, TRAIT_ULTRA_RES) || has_trait(creature_ptr, TRAIT_POSTURE_MUSOU))
	{
		//creature_ptr->see_inv = TRUE;
		//TODO has_trait(creature_ptr, TRAIT_FREE_ACTION) = TRUE;
		//creature_ptr->timed_trait[TRAIT_SLOW]_digest = TRUE;
		//creature_ptr->regenerate = TRUE;
		//creature_ptr->levitation = TRUE;
		//creature_ptr->hold_life = TRUE;
		//TODO creature_ptr->telepathy = TRUE;
		//creature_ptr->lite = TRUE;
		//has_trait(creature_ptr, TRAIT_SUSTAIN_STR) = TRUE;
		//has_trait(creature_ptr, TRAIT_SUSTAIN_INT) = TRUE;
		//has_trait(creature_ptr, TRAIT_SUSTAIN_WIS) = TRUE;
		//has_trait(creature_ptr, TRAIT_SUSTAIN_CON) = TRUE;
		//has_trait(creature_ptr, TRAIT_SUSTAIN_DEX) = TRUE;
		//has_trait(creature_ptr, TRAIT_SUSTAIN_CHR) = TRUE;
		//TODO creature_ptr->resist_acid = TRUE;
		//TODO creature_ptr->resist_elec = TRUE;
		//TODO creature_ptr->resist_fire = TRUE;
		//TODO creature_ptr->resist_cold = TRUE;
		//TODO creature_ptr->resist_pois = TRUE;
		//TODO creature_ptr->resist_conf = TRUE;
		//TODO creature_ptr->resist_sound = TRUE;
		//TODO creature_ptr->resist_lite = TRUE;
		//TODO creature_ptr->resist_dark = TRUE;
		//TODO creature_ptr->resist_chaos = TRUE;
		//TODO creature_ptr->resist_disen = TRUE;
		//TODO creature_ptr->resist_shard = TRUE;
		//has_trait(creature_ptr, TRAIT_NO_BLIND) = TRUE;
		//TODO creature_ptr->resist_neth = TRUE;
		//TODO creature_ptr->resist_fear = TRUE;
		//TODO has_trait(creature_ptr, TRAIT_REFLECTING) = TRUE;
		//TODO creature_ptr->sh_fire = TRUE;
		//TODO creature_ptr->sh_elec = TRUE;
		//TODO creature_ptr->sh_cold = TRUE;
	}

	if(MUSIC_SINGING(creature_ptr, MUSIC_WALL))
	{
		//TODO creature_ptr->kill_wall = TRUE;
	}

	if(IS_TIM_STEALTH(creature_ptr)) creature_ptr->skill_stl += 99;
}


static void set_inventory_bonuses(creature_type *creature_ptr)
{
	int i;
	object_type *object_ptr;
	int bonus_to_hit, bonus_to_damage, slot;
	u32b flgs[TRAIT_FLAG_MAX];
	int default_hand = 1;

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		slot = GET_INVEN_SLOT_TYPE(creature_ptr, i);
		object_ptr = &creature_ptr->inventory[i];

		if(!IS_EQUIPPED(object_ptr)) continue; // Skip no equip
		if(!is_valid_object(object_ptr)) continue; // Skip non-objects

		/* Extract the item flags */
		object_flags(object_ptr, flgs);

		creature_ptr->cursed |= (object_ptr->curse_flags[0] & (0xFFFFFFF0L));

		// Affect stats
		creature_ptr->stat_add[STAT_STR] += object_ptr->stat_val[STAT_STR];
		creature_ptr->stat_add[STAT_INT] += object_ptr->stat_val[STAT_INT];
		creature_ptr->stat_add[STAT_WIS] += object_ptr->stat_val[STAT_WIS];
		creature_ptr->stat_add[STAT_DEX] += object_ptr->stat_val[STAT_DEX];
		creature_ptr->stat_add[STAT_CON] += object_ptr->stat_val[STAT_CON];
		creature_ptr->stat_add[STAT_CHA] += object_ptr->stat_val[STAT_CHA];

		if(has_trait_object(object_ptr, TRAIT_SEXY_EQUIPMENT) && has_trait(creature_ptr, TRAIT_SEXY_BONUS))
		{
			creature_ptr->stat_add[STAT_STR] += 3 * STAT_FRACTION;
			creature_ptr->stat_add[STAT_INT] += 3 * STAT_FRACTION;
			creature_ptr->stat_add[STAT_WIS] += 3 * STAT_FRACTION;
			creature_ptr->stat_add[STAT_DEX] += 3 * STAT_FRACTION;
			creature_ptr->stat_add[STAT_CON] += 3 * STAT_FRACTION;
			creature_ptr->stat_add[STAT_CHA] += 3 * STAT_FRACTION;
		}

		if(have_flag(flgs, TRAIT_MAGIC_MASTERY))    creature_ptr->skill_dev += 8 * object_ptr->pval;

		creature_ptr->skill_stl += object_ptr->pval;

		/* Affect searching ability (factor of five) */
		if(have_flag(flgs, TRAIT_SEARCH)) creature_ptr->skill_srh += (object_ptr->pval * 5);

		/* Affect searching frequency (factor of five) */
		if(have_flag(flgs, TRAIT_SEARCH)) creature_ptr->skill_fos += (object_ptr->pval * 5);

		/* Affect infravision */
		if(have_flag(flgs, TRAIT_INFRA)) creature_ptr->see_infra += object_ptr->pval;

		/* Affect digging (factor of 20) */
		if(have_flag(flgs, TRAIT_TUNNEL)) creature_ptr->skill_dig += (object_ptr->pval * 20);

		if(have_flag(flgs, TRAIT_SPEED)) creature_ptr->speed += object_ptr->pval;

		/* Affect blows */
		if(has_trait_object(object_ptr, TRAIT_BLOWS))
		{
			//TODO adjust
		}

		/* Various flags */
		//TODO if(have_flag(flgs, TRAIT_EXTRA_ATTACK_MIGHT))  creature_ptr->xtra_might = TRUE;

		if(have_flag(flgs, TRAIT_PASSIVE_TELEPORT))
		{
			if(object_is_cursed(object_ptr)) add_flag(object_ptr->curse_flags, TRAIT_RANDOM_TELEPORT);
			else
			{
				cptr insc = quark_str(object_ptr->inscription);

				if(object_ptr->inscription && my_strchr(insc, '.'))
				{
					/*
					 * {.} will stop random teleportation.
					 */
				}
				else
				{
					/* Controlled random teleportation */
					//TODO creature_ptr->cursed |= TRAIT_ACTIVE_TELEPORT_SELF;
				}
			}
		}

		if(object_ptr->tval == TV_CAPTURE) continue;

		// AC/EV/VO bonuses
		creature_ptr->ac += object_ptr->ac;
		creature_ptr->dis_ac += object_ptr->ac;
		creature_ptr->to_ac += object_ptr->to_ac;
		creature_ptr->ev += object_ptr->ev;
		creature_ptr->dis_ev += object_ptr->ev;
		creature_ptr->to_ev += object_ptr->to_ev;
		creature_ptr->vo += object_ptr->vo;
		creature_ptr->dis_vo += object_ptr->vo;
		creature_ptr->to_vo += object_ptr->to_vo;

		if(object_is_known(object_ptr)) creature_ptr->dis_to_ac += object_ptr->to_ac;

		if(has_trait(creature_ptr, TRAIT_LOW_MELEE))
		{
			int slot = i - 0; //TODO 
			if(slot < 2)
			{
				if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))
				{
					creature_ptr->to_hit[slot] -= 15;
					if(object_ptr->ident & IDENT_MENTAL) creature_ptr->dis_to_hit[slot] -= 15;
				}
				else
				{
					creature_ptr->to_hit[slot] -= 5;
					if(object_ptr->ident & IDENT_MENTAL) creature_ptr->dis_to_hit[slot] -= 5;
				}
			}
			else
			{
				if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))
				{
					creature_ptr->to_hit_b -= 15;
					if(object_ptr->ident & IDENT_MENTAL) creature_ptr->dis_to_hit_b -= 15;
				}
				else
				{
					creature_ptr->to_hit_b -= 5;
					if(object_ptr->ident & IDENT_MENTAL) creature_ptr->dis_to_hit_b -= 5;
				}
			}
		}

		if(has_trait(creature_ptr, TRAIT_LOW_AC))
		{
			if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))
			{
				creature_ptr->to_ac -= 30;
				if(object_ptr->ident & IDENT_MENTAL) creature_ptr->dis_to_ac -= 30;
			}
			else
			{
				creature_ptr->to_ac -= 10;
				if(object_ptr->ident & IDENT_MENTAL) creature_ptr->dis_to_ac -= 10;
			}
		}

		/* Hack -- do not apply "weapon" bonuses */
		if(slot == INVEN_SLOT_HAND && get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > i) continue;

		/* Hack -- do not apply "bow" bonuses */
		if(slot == INVEN_SLOT_BOW) continue;

		bonus_to_hit = object_ptr->to_hit;
		bonus_to_damage = object_ptr->to_damage;

		if(creature_ptr->class_idx == CLASS_NINJA)
		{
			if(object_ptr->to_hit > 0) bonus_to_hit = (object_ptr->to_hit+1)/2;
			if(object_ptr->to_damage > 0) bonus_to_damage = (object_ptr->to_damage+1)/2;
		}

		/* To Bow and Natural attack */

		/* Apply the bonuses to hit/damage */
		creature_ptr->to_hit_b += bonus_to_hit;
		creature_ptr->to_hit_m += bonus_to_hit;
		creature_ptr->to_damage_m += bonus_to_damage;

		/* Apply the mental bonuses tp hit/damage, if known */
		if(object_is_known(object_ptr)) creature_ptr->dis_to_hit_b += bonus_to_hit;

		/* To Melee */
		if(GET_INVEN_SLOT_TYPE(creature_ptr, i) == INVEN_SLOT_RING && !creature_ptr->two_handed)
		{
			/* Apply the bonuses to hit/damage */
			creature_ptr->to_hit[get_equipped_slot_idx(creature_ptr, INVEN_SLOT_RING, i)] += bonus_to_hit;
			creature_ptr->to_damage[get_equipped_slot_idx(creature_ptr, INVEN_SLOT_RING, i)] += bonus_to_damage;

			/* Apply the mental bonuses tp hit/damage, if known */
			if(object_is_known(object_ptr))
			{
				creature_ptr->dis_to_hit[get_equipped_slot_idx(creature_ptr, INVEN_SLOT_RING, i)] += bonus_to_hit;
				creature_ptr->dis_to_damage[get_equipped_slot_idx(creature_ptr, INVEN_SLOT_RING, i)] += bonus_to_damage;
			}
		}
		else if(creature_ptr->can_melee[0] && creature_ptr->can_melee[1])
		{
			/* Apply the bonuses to hit/damage */
			creature_ptr->to_hit[0] += (bonus_to_hit > 0) ? (bonus_to_hit+1)/2 : bonus_to_hit;
			creature_ptr->to_hit[1] += (bonus_to_hit > 0) ? bonus_to_hit/2 : bonus_to_hit;
			creature_ptr->to_damage[0] += (bonus_to_damage > 0) ? (bonus_to_damage+1)/2 : bonus_to_damage;
			creature_ptr->to_damage[1] += (bonus_to_damage > 0) ? bonus_to_damage/2 : bonus_to_damage;

			/* Apply the mental bonuses tp hit/damage, if known */
			if(object_is_known(object_ptr))
			{
				creature_ptr->dis_to_hit[0] += (bonus_to_hit > 0) ? (bonus_to_hit+1)/2 : bonus_to_hit;
				creature_ptr->dis_to_hit[1] += (bonus_to_hit > 0) ? bonus_to_hit/2 : bonus_to_hit;
				creature_ptr->dis_to_damage[0] += (bonus_to_damage > 0) ? (bonus_to_damage+1)/2 : bonus_to_damage;
				creature_ptr->dis_to_damage[1] += (bonus_to_damage > 0) ? bonus_to_damage/2 : bonus_to_damage;
			}
		}
		else
		{
			/* Apply the bonuses to hit/damage */
			creature_ptr->to_hit[default_hand] += bonus_to_hit;
			creature_ptr->to_damage[default_hand] += bonus_to_damage;

			/* Apply the mental bonuses to hit/damage, if known */
			if(object_is_known(object_ptr))
			{
				creature_ptr->dis_to_hit[default_hand] += bonus_to_hit;
				creature_ptr->dis_to_damage[default_hand] += bonus_to_damage;
			}
		}
	}

}

static void wipe_creature_calculation_status(creature_type *creature_ptr)
{
	int i;

	for(i = 0; i < STAT_MAX; i++) creature_ptr->stat_add[i] = 0; // Clear the stat modifiers

	// Clear the Displayed/Real armor class and evasion
	creature_ptr->dis_ac = creature_ptr->ac = 0;
	creature_ptr->dis_ev = creature_ptr->ev = 0;
	creature_ptr->dis_vo = creature_ptr->vo = 0;

	creature_ptr->dis_to_hit_b = creature_ptr->to_hit_b = 0;
	creature_ptr->dis_to_ac = creature_ptr->to_ac = 0;
	creature_ptr->dis_to_ev = creature_ptr->to_ev = 0;
	creature_ptr->to_hit_m = 0;
	creature_ptr->to_damage_m = 0;
	creature_ptr->to_m_chance = 0;

	for(i = 0; i < MAX_WEAPONS; i++)
	{
		creature_ptr->dis_to_hit[i] = creature_ptr->to_hit[i] = 0;
		creature_ptr->dis_to_damage[i] = creature_ptr->to_damage[i] = 0;
		creature_ptr->to_damaged[i] = creature_ptr->to_damages[i] = 0;
		creature_ptr->can_melee[i] = FALSE;
	}

	creature_ptr->speed = 0;		// Start with "normal" speed
	creature_ptr->num_fire = 100;	// Start with a single shot per turn
	creature_ptr->tval_xtra = 0;	// Reset the "xtra" tval
	creature_ptr->tval_ammo = 0;	// Reset the "ammo" tval

	creature_ptr->skill_dis = 0;
	creature_ptr->skill_dev = 0;
	creature_ptr->skill_stl = 0;
	creature_ptr->skill_srh = 0;
	creature_ptr->skill_fos = 0;
	creature_ptr->skill_thn = 0;
	creature_ptr->skill_thb = 0;
	creature_ptr->skill_tht = 0;
	creature_ptr->skill_dig = 0;
	creature_ptr->see_infra = 0;

	// Clear all the flags
	creature_ptr->cursed = 0L;

	for(i = 0; i < INVEN_TOTAL; i++) creature_ptr->two_handed[i] = -1;
	for(i = 0; i < STAT_MAX; i++) creature_ptr->stat_mod_max_max[i] = creature_ptr->stat_max_max[i];

	creature_ptr->no_flowed = FALSE;
}

static void set_karma_bonuses(creature_type *creature_ptr)
{
	int i;

	race_type *tmp_race_ptr, *tmp_race_ptr2;

	tmp_race_ptr = &race_info[creature_ptr->race_idx1];
	tmp_race_ptr2 = &race_info[creature_ptr->race_idx2];

	// calc karmas and bonuses
	for(i = 0; i < MAX_KARMA; i++) creature_ptr->karmas_cur[i] = creature_ptr->karmas_cur[i];
	for(i = 0; i < MAX_KARMA; i++) creature_ptr->karmas_rank[i] = calc_rank(creature_ptr->karmas_cur[i]);

	// calc alignments and bonuses
	if(creature_ptr->race_idx1 != INDEX_NONE)
	{
		creature_ptr->good_exp    = tmp_race_ptr->good;
		creature_ptr->evil_exp    = tmp_race_ptr->evil;
		creature_ptr->order_exp   = tmp_race_ptr->order;
		creature_ptr->chaos_exp   = tmp_race_ptr->chaos;
		creature_ptr->balance_exp = tmp_race_ptr->balance;
	}
	else
	{
		creature_ptr->good_exp    = 0;
		creature_ptr->evil_exp    = 0;
		creature_ptr->order_exp   = 0;
		creature_ptr->chaos_exp   = 0;
		creature_ptr->balance_exp = 0;
	}

	for(i = 0; i < MAX_KARMA; i++)
	{
		creature_ptr->good_exp  += creature_ptr->karmas[i] * karma[i].good_adj;
		creature_ptr->evil_exp  += creature_ptr->karmas[i] * karma[i].evil_adj;
		creature_ptr->order_exp += creature_ptr->karmas[i] * karma[i].order_adj;
		creature_ptr->chaos_exp += creature_ptr->karmas[i] * karma[i].chaos_adj;
	}

	if(get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, i)->name1 == ART_IRON_BALL) creature_ptr->good_rank -= 300;

	creature_ptr->good_rank = calc_rank(creature_ptr->good_exp);
	creature_ptr->evil_rank = calc_rank(creature_ptr->evil_exp);
	creature_ptr->order_rank = calc_rank(creature_ptr->order_exp);
	creature_ptr->chaos_rank = calc_rank(creature_ptr->chaos_exp);

	creature_ptr->balance_exp -= ((creature_ptr->good_rank - creature_ptr->evil_rank) * (creature_ptr->good_rank - creature_ptr->evil_rank) - 100) * 10;
	creature_ptr->balance_exp -= ((creature_ptr->order_rank - creature_ptr->chaos_rank) * (creature_ptr->order_rank - creature_ptr->chaos_rank) - 100) * 10;
	creature_ptr->balance_rank = calc_rank(creature_ptr->balance_exp);
}

static void creature_bonuses_message(creature_type *creature_ptr)
{
	int i;

	// TODO : Hack, monk armour
	bool monk_armour_aux = FALSE;
	bool monk_notify_aux = FALSE;

	/* Take note when "heavy bow" changes */
	if(creature_ptr->old_heavy_shoot != creature_ptr->heavy_shoot)
	{
		if(creature_ptr->heavy_shoot)
		{
#ifdef JP
			msg_print("こんな重い弓を装備しているのは大変だ。");
#else
			msg_print("You have trouble wielding such a heavy bow.");
#endif

		}
		else if(get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 0)->k_idx)
		{
#ifdef JP
			msg_print("この弓なら装備していても辛くない。");
#else
			msg_print("You have no trouble wielding your bow.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("重い弓を装備からはずして体が楽になった。");
#else
			msg_print("You feel relieved to put down your heavy bow.");
#endif
		}

		/* Save it */
		creature_ptr->old_heavy_shoot = creature_ptr->heavy_shoot;
	}

	for (i = 0 ; i < MAX_WEAPONS ; i++)
	{
		/* Take note when "heavy weapon" changes */
		if(creature_ptr->old_heavy_wield[i] != creature_ptr->heavy_wield[i])
		{
			if(creature_ptr->heavy_wield[i])
			{
#ifdef JP
				msg_print("こんな重い武器を装備しているのは大変だ。");
#else
				msg_print("You have trouble wielding such a heavy weapon.");
#endif
			}
			else if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0)
			{
#ifdef JP
				msg_print("これなら装備していても辛くない。");
#else
				msg_print("You have no trouble wielding your weapon.");
#endif
			}
			else if(creature_ptr->heavy_wield[1-i])
			{
#ifdef JP
				msg_print("まだ武器が重い。");
#else
				msg_print("You have still trouble wielding a heavy weapon.");
#endif
			}
			else
			{
#ifdef JP
				msg_print("重い武器を装備からはずして体が楽になった。");
#else
				msg_print("You feel relieved to put down your heavy weapon.");
#endif

			}

			/* Save it */
			creature_ptr->old_heavy_wield[i] = creature_ptr->heavy_wield[i];
		}

		/* Take note when "heavy weapon" changes */
		if(creature_ptr->old_riding_wield[i] != creature_ptr->riding_wield[i])
		{
			if(creature_ptr->riding_wield[i])
			{
#ifdef JP
				msg_print("この武器は乗馬中に使うにはむかないようだ。");
#else
				msg_print("This weapon is not suitable for use while riding.");
#endif

			}
			else if(!creature_ptr->riding)
			{
#ifdef JP
				msg_print("この武器は徒歩で使いやすい。");
#else
				msg_print("This weapon was not suitable for use while riding.");
#endif

			}
			else if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0)
			{
#ifdef JP
				msg_print("これなら乗馬中にぴったりだ。");
#else
				msg_print("This weapon is suitable for use while riding.");
#endif

			}
			/* Save it */
			creature_ptr->old_riding_wield[i] = creature_ptr->riding_wield[i];
		}

	}

	if(creature_ptr->riding && (creature_ptr->old_riding_two_handed != creature_ptr->riding_two_handed))
	{
		if(creature_ptr->riding_two_handed)
		{
#ifdef JP
			msg_format("%s馬を操れない。", (empty_hands(creature_ptr, FALSE) == EMPTY_HAND_NONE) ? "両手がふさがっていて" : "");
#else
			msg_print("You are using both hand for fighting, and you can't control a riding pet.");
#endif
		}
		else
		{
#ifdef JP
			msg_format("%s馬を操れるようになった。", (empty_hands(creature_ptr, FALSE) == EMPTY_HAND_NONE) ? "手が空いて" : "");
#else
			msg_print("You began to control riding pet with one hand.");
#endif
		}

		creature_ptr->old_riding_two_handed = creature_ptr->riding_two_handed;
	}

	if(has_trait(creature_ptr, TRAIT_WANT_LIGHT_WEIGHT) && (monk_armour_aux != monk_notify_aux))
	{
		if(heavy_armor(creature_ptr))
#ifdef JP
            msg_print("装備が重くてバランスを取れない。");
#else
			msg_print("The weight of your armor disrupts your balance.");
#endif
		else
#ifdef JP
            msg_print("バランスがとれるようになった。");
#else
			msg_print("You regain your balance.");
#endif

		monk_notify_aux = monk_armour_aux;
	}

}


static void set_trait_bonuses(creature_type *creature_ptr)
{
	int i;

	for(i = 0; i < max_trait_idx; i++)
	{
		if(has_trait(creature_ptr, i))
		{
			creature_ptr->stat_add[STAT_STR] += trait_info[i].adj[STAT_STR] * 10;
			creature_ptr->stat_add[STAT_INT] += trait_info[i].adj[STAT_INT] * 10;
			creature_ptr->stat_add[STAT_WIS] += trait_info[i].adj[STAT_WIS] * 10;
			creature_ptr->stat_add[STAT_DEX] += trait_info[i].adj[STAT_DEX] * 10;
			creature_ptr->stat_add[STAT_CON] += trait_info[i].adj[STAT_CON] * 10;
			creature_ptr->stat_add[STAT_CHA] += trait_info[i].adj[STAT_CHA] * 10;

			creature_ptr->skill_dis += trait_info[i].dis;
			creature_ptr->skill_dev += trait_info[i].dev;
			creature_ptr->skill_stl += trait_info[i].stl;
			creature_ptr->skill_srh += trait_info[i].srh;
			creature_ptr->skill_dig += trait_info[i].dig;
			creature_ptr->skill_thb += trait_info[i].thb;
			creature_ptr->skill_thn += trait_info[i].thn;
			creature_ptr->skill_tht += trait_info[i].tht;

			creature_ptr->see_infra += trait_info[i].infra;

			creature_ptr->ac += trait_info[i].ac;
			creature_ptr->ev += trait_info[i].ev;
			creature_ptr->vo += trait_info[i].vo;
			creature_ptr->dis_ac += trait_info[i].ac;
			creature_ptr->dis_ev += trait_info[i].ev;
			creature_ptr->dis_vo += trait_info[i].vo;
		}
	}

		if(has_trait(creature_ptr, TRAIT_FIRE_BODY))
		{
			//TODO creature_ptr->sh_fire = TRUE;
			//TODO creature_ptr->lite = TRUE;
		}

		if(has_trait(creature_ptr, TRAIT_WINGS))
		{
			//TODO creature_ptr->levitation = TRUE;
		}

		if(has_trait(creature_ptr, TRAIT_MOTION))
		{
			//TODO has_trait(creature_ptr, TRAIT_FREE_ACTION) = TRUE;
		}

		if(has_trait(creature_ptr, TRAIT_AGILE_RACE)) creature_ptr->speed += creature_ptr->lev / 8;

		if(has_trait(creature_ptr, TRAIT_ILL_NORM)) creature_ptr->stat_add[STAT_CHA] = 0;
		if(has_trait(creature_ptr, TRAIT_LOW_MAGIC)) creature_ptr->to_m_chance += 5;

}


static void set_melee_status(creature_type *creature_ptr)
{
	int i, hold;
	object_type *bow_ptr, *weapon_ptr;
	u32b flgs[TRAIT_FLAG_MAX];
	bool omoi;
	int default_hand = 1;
	int empty_hands_status = empty_hands(creature_ptr, TRUE);
	species_type *species_ptr = &species_info[creature_ptr->species_idx];

	hold = calc_equipping_weight_limit(creature_ptr); // Obtain the equipment value

	// Examine the "current bow"
	bow_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_BOW, 0);

	// Set Species Blow.
	for(i = 0; i < MAX_SPECIAL_BLOWS; i++)
	{
		if(species_ptr->blow[i].method) creature_ptr->blow[i] = species_ptr->blow[i];
		else creature_ptr->blow[i] = race_info[creature_ptr->race_idx1].blow[i];
	}

	if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0) creature_ptr->can_melee[0] = TRUE;
	if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1)
	{
		creature_ptr->can_melee[1] = TRUE;
		if(!creature_ptr->can_melee[0]) default_hand = 1;
	}

	if(CAN_TWO_HANDS_WIELDING(creature_ptr))
	{
		if(creature_ptr->can_melee[0] && (empty_hands(creature_ptr, FALSE) == EMPTY_HAND_LARM) &&
			object_allow_two_hands_wielding(creature_ptr, get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 0)))
		{
			//TODO creature_ptr->two_handed = TRUE;
		}
		else if(creature_ptr->can_melee[1] && (empty_hands(creature_ptr, FALSE) == EMPTY_HAND_RARM) &&
			object_allow_two_hands_wielding(creature_ptr, get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, 1)))
		{
			//TODO creature_ptr->two_handed = TRUE;
		}
		else
		{
			switch (creature_ptr->class_idx)
			{
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
			case CLASS_BERSERKER:
				if(empty_hands(creature_ptr, FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))
				{
					creature_ptr->can_melee[0] = TRUE;
					// TODO creature_ptr->two_handed = TRUE;
				}
				break;
			}
		}
	}

	if(bow_ptr->k_idx)
	{
		/* Assume not heavy */
		creature_ptr->heavy_shoot = FALSE;

		/* It is hard to carholdry a heavy bow */
		if(hold < bow_ptr->weight / 10)
		{
			/* Hard to wield a heavy bow */
			creature_ptr->to_hit_b  += 2 * (hold - bow_ptr->weight / 10);
			creature_ptr->dis_to_hit_b  += 2 * (hold - bow_ptr->weight / 10);

			/* Heavy Bow */
			creature_ptr->heavy_shoot = TRUE;
		}

		/* Compute "extra shots" if needed */
		if(bow_ptr->k_idx)
		{
			/* Analyze the launcher */
			switch (bow_ptr->sval)
			{
				case SV_SLING:
				{
					creature_ptr->tval_ammo = TV_SHOT;
					break;
				}

				case SV_SHORT_BOW:
				case SV_LONG_BOW:
				case SV_NAMAKE_BOW:
				{
					creature_ptr->tval_ammo = TV_ARROW;
					break;
				}

				case SV_LIGHT_XBOW:
				case SV_HEAVY_XBOW:
				{
					creature_ptr->tval_ammo = TV_BOLT;
					break;
				}
				case SV_CRIMSON:
				{
					creature_ptr->tval_ammo = TV_NO_AMMO;
					break;
				}
			}

			/* Apply special flags */
			if(bow_ptr->k_idx && !creature_ptr->heavy_shoot)
			{
				// TODO Extra shots

				/* Hack -- Rangers love Bows */
				if((creature_ptr->class_idx == CLASS_RANGER) && (creature_ptr->tval_ammo == TV_ARROW))
				{
					creature_ptr->num_fire += (creature_ptr->lev * 4);
				}

				if((creature_ptr->class_idx == CLASS_CAVALRY) && (creature_ptr->tval_ammo == TV_ARROW))
				{
					creature_ptr->num_fire += (creature_ptr->lev * 3);
				}

				if(creature_ptr->class_idx == CLASS_ARCHER)
				{
					if(creature_ptr->tval_ammo == TV_ARROW)
						creature_ptr->num_fire += ((creature_ptr->lev * 5)+50);
					else if((creature_ptr->tval_ammo == TV_BOLT) || (creature_ptr->tval_ammo == TV_SHOT))
						creature_ptr->num_fire += (creature_ptr->lev * 4);
				}

				/*
				 * Addendum -- also "Reward" high level warriors,
				 * with _any_ missile weapon -- TY
				 */
				if(creature_ptr->class_idx == CLASS_WARRIOR && (creature_ptr->tval_ammo <= TV_BOLT) && (creature_ptr->tval_ammo >= TV_SHOT))
					creature_ptr->num_fire += (creature_ptr->lev * 2);
				if((creature_ptr->class_idx == CLASS_ROGUE) && (creature_ptr->tval_ammo == TV_SHOT))
					creature_ptr->num_fire += (creature_ptr->lev * 4);

				/* Snipers love Cross bows */
				if((creature_ptr->class_idx == CLASS_SNIPER) && (creature_ptr->tval_ammo == TV_BOLT))
				{
					creature_ptr->to_hit_b += (10 + (creature_ptr->lev / 5));
					creature_ptr->dis_to_hit_b += (10 + (creature_ptr->lev / 5));
				}
			}
		}
	}

	for(i = 0; i < MAX_WEAPONS; i++)
	{
	//TODO: adjust
	/*
	if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 1)
	{
		int penalty1, penalty2;
		penalty1 = ((100 - creature_ptr->skill_exp[SKILL_MULTI_WEAPON] / 160) - (130 - creature_ptr->inventory[].weight) / 8);
		penalty2 = ((100 - creature_ptr->skill_exp[SKILL_MULTI_WEAPON] / 160) - (130 - creature_ptr->inventory[].weight) / 8);
		if((creature_ptr->inventory[].name1 == ART_QUICKTHORN) && (creature_ptr->inventory[].name1 == ART_TINYTHORN))
		{
			penalty1 = penalty1 / 2 - 5;
			penalty2 = penalty2 / 2 - 5;
			creature_ptr->speed += 7;
			creature_ptr->to_ac += 10;
			creature_ptr->dis_to_ac += 10;
		}
		if(has_trait(creature_ptr, TRAIT_EASY_MULTI_WEAPON)
		{
			if(penalty1 > 0) penalty1 /= 2;
			if(penalty2 > 0) penalty2 /= 2;
		}
		else if((creature_ptr->inventory[].tval == TV_SWORD) && ((creature_ptr->inventory[].sval == SV_MAIN_GAUCHE) || (creature_ptr->inventory[].sval == SV_WAKIZASHI)))
		{
			penalty1 = MAX(0, penalty1 - 10);
			penalty2 = MAX(0, penalty2 - 10);
		}
		if((creature_ptr->inventory[].name1 == ART_MUSASI_KATANA) && (creature_ptr->inventory[].name1 == ART_MUSASI_WAKIZASI))
		{
			penalty1 = MIN(0, penalty1);
			penalty2 = MIN(0, penalty2);
			creature_ptr->to_ac += 10;
			creature_ptr->dis_to_ac += 10;
		}
		else
		{
			if((creature_ptr->inventory[].name1 == ART_MUSASI_KATANA) && (penalty1 > 0))
				penalty1 /= 2;
			if((creature_ptr->inventory[].name1 == ART_MUSASI_WAKIZASI) && (penalty2 > 0))
				penalty2 /= 2;
		}
		if(creature_ptr->inventory[].tval == TV_POLEARM) penalty1 += 10;
		if(creature_ptr->inventory[].tval == TV_POLEARM) penalty2 += 10;
		creature_ptr->to_hit[0] -= penalty1;
		creature_ptr->to_hit[1] -= penalty2;
		creature_ptr->dis_to_hit[0] -= penalty1;
		creature_ptr->dis_to_hit[1] -= penalty2;
	}
	*/

		/* Examine the "main weapon" */
		bow_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, i);

		object_flags(bow_ptr, flgs);

		/* Assume not heavy */
		creature_ptr->heavy_wield[i] = FALSE;
		creature_ptr->riding_wield[i] = FALSE;

		/* It is hard to hold a heavy weapon */
		if(hold < bow_ptr->weight / 10)
		{
			/* Hard to wield a heavy weapon */
			creature_ptr->to_hit[i] += 2 * (hold - bow_ptr->weight / 10);
			creature_ptr->dis_to_hit[i] += 2 * (hold - bow_ptr->weight / 10);

			/* Heavy weapon */
			creature_ptr->heavy_wield[i] = TRUE;
		}
		else if(creature_ptr->two_handed && (hold < bow_ptr->weight/5)) omoi = TRUE;

		if((i == 1) && (bow_ptr->tval == TV_SWORD) && ((bow_ptr->sval == SV_MAIN_GAUCHE) || (bow_ptr->sval == SV_WAKIZASHI)))
		{
			creature_ptr->to_ac += 5;
			creature_ptr->dis_to_ac += 5;
		}

		if(bow_ptr->k_idx && !creature_ptr->heavy_wield[i])
			creature_ptr->skill_dig += (bow_ptr->weight / 10); // Boost digging skill by weapon weight

		// Assume okay
		// Priest weapon penalty for non-blessed edged weapons
		if((creature_ptr->class_idx == CLASS_PRIEST) && (!(have_flag(flgs, TRAIT_BLESSED_BRAND))) && ((bow_ptr->tval == TV_SWORD) || (bow_ptr->tval == TV_POLEARM)))
		{
			/* Reduce the real bonuses */
			creature_ptr->to_hit[i] -= 2;
			creature_ptr->to_damage[i] -= 2;

			/* Reduce the mental bonuses */
			creature_ptr->dis_to_hit[i] -= 2;
			creature_ptr->dis_to_damage[i] -= 2;
		}

		// Hex bonuses
		if(creature_ptr->realm1 == REALM_HEX)
		{
			if(object_is_cursed(bow_ptr))
			{
				if(have_flag(bow_ptr->curse_flags, TRAIT_CURSED)) { creature_ptr->to_hit[i] += 5; creature_ptr->dis_to_hit[i] += 5; }
				if(have_flag(bow_ptr->curse_flags, TRAIT_HEAVY_CURSE)) { creature_ptr->to_hit[i] += 7; creature_ptr->dis_to_hit[i] += 7; }
				if(have_flag(bow_ptr->curse_flags, TRAIT_DIVINE_CURSE)) { creature_ptr->to_hit[i] += 13; creature_ptr->dis_to_hit[i] += 13; }
				if(have_flag(bow_ptr->curse_flags, TRAIT_TY_CURSE)) { creature_ptr->to_hit[i] += 5; creature_ptr->dis_to_hit[i] += 5; }
				if(HEX_SPELLING(creature_ptr, HEX_RUNESWORD))
				{
					if(have_flag(bow_ptr->curse_flags, TRAIT_CURSED)) { creature_ptr->to_damage[i] += 5; creature_ptr->dis_to_damage[i] += 5; }
					if(have_flag(bow_ptr->curse_flags, TRAIT_HEAVY_CURSE)) { creature_ptr->to_damage[i] += 7; creature_ptr->dis_to_damage[i] += 7; }
					if(have_flag(bow_ptr->curse_flags, TRAIT_DIVINE_CURSE)) { creature_ptr->to_damage[i] += 13; creature_ptr->dis_to_damage[i] += 13; }
				}
			}
		}
		if(creature_ptr->riding)
		{
			if((bow_ptr->tval == TV_POLEARM) && ((bow_ptr->sval == SV_LANCE) || (bow_ptr->sval == SV_HEAVY_LANCE)))
			{
				creature_ptr->to_hit[i] +=15;
				creature_ptr->dis_to_hit[i] +=15;
				creature_ptr->to_damaged[i] += 2;
			}
			else if(!(have_flag(flgs, TRAIT_RIDING)))
			{
				int penalty;
				if(has_trait(creature_ptr, TRAIT_RODEO))
				{
					penalty = 5;
				}
				else
				{
					penalty = species_info[creature_list[creature_ptr->riding].species_idx].level - creature_ptr->skill_exp[SKILL_RIDING] / 80;
					penalty += 30;
					if(penalty < 30) penalty = 30;
				}
				creature_ptr->to_hit[i] -= penalty;
				creature_ptr->dis_to_hit[i] -= penalty;

				/* Riding weapon */
				creature_ptr->riding_wield[i] = TRUE;
			}
		}
	}

	if((creature_ptr->can_melee[0] && (empty_hands_status & EMPTY_HAND_RARM)) ||
	    (creature_ptr->can_melee[1] && (empty_hands_status & EMPTY_HAND_LARM)))
	{
		creature_ptr->to_hit[default_hand] += (creature_ptr->skill_exp[SKILL_MARTIAL_ARTS]) / 200;
		creature_ptr->dis_to_hit[default_hand] += (creature_ptr->skill_exp[SKILL_MARTIAL_ARTS]) / 200;
		creature_ptr->to_damage[default_hand] += creature_ptr->size * (10 + (creature_ptr->skill_exp[SKILL_MARTIAL_ARTS]) / 200) / 100;
		creature_ptr->dis_to_damage[default_hand] += creature_ptr->size * (10 + (creature_ptr->skill_exp[SKILL_MARTIAL_ARTS]) / 200) / 100;
	}

	for (i = 0; i < MAX_WEAPONS; i++)
	{
		weapon_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, i); 

		if(get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND) > 0)
		{
			int tval = weapon_ptr->tval - TV_WEAPON_BEGIN;
			int sval = weapon_ptr->sval;
			int boost = 0; //(creature_ptr->weapon_exp[tval][sval] - WEAPON_EXP_BEGINNER) / 200;

			creature_ptr->to_hit[i] += boost > 0 ? boost : 0;
			creature_ptr->dis_to_hit[i] += boost > 0 ? boost : 0;
			creature_ptr->action_cost[MELLE_WEAPON_INDEX + i] = calc_weapon_melee_cost(creature_ptr, weapon_ptr);
			creature_ptr->action_priority[MELLE_WEAPON_INDEX + i] = calc_weapon_melee_priority(creature_ptr, weapon_ptr);

		}
	}

	// Actual Modifier Bonuses (Un-inflate stat bonuses)
	for(i = 0; i < MAX_WEAPONS; i++)
	{
		creature_ptr->to_damage[i] += ((int)(adj_str_to_damage[creature_ptr->stat_ind[STAT_STR]]));
		creature_ptr->to_hit[i] += ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]));
		creature_ptr->to_hit[i] += ((int)(adj_str_to_hit[creature_ptr->stat_ind[STAT_STR]]));

		creature_ptr->dis_to_damage[i] += ((int)(adj_str_to_damage[creature_ptr->stat_ind[STAT_STR]]));
		creature_ptr->dis_to_hit[i] += ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]));
		creature_ptr->dis_to_hit[i] += ((int)(adj_str_to_hit[creature_ptr->stat_ind[STAT_STR]]));
	}

	creature_ptr->to_ac += ((int)(adj_dex_to_ac[creature_ptr->stat_ind[STAT_DEX]]));
	creature_ptr->to_damage_m  += ((int)(adj_str_to_damage[creature_ptr->stat_ind[STAT_STR]]));
	creature_ptr->to_hit_b  += ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]));
	creature_ptr->to_hit_m  += ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]));
	creature_ptr->to_hit_b  += ((int)(adj_str_to_hit[creature_ptr->stat_ind[STAT_STR]]));
	creature_ptr->to_hit_m  += ((int)(adj_str_to_hit[creature_ptr->stat_ind[STAT_STR]]));

	// Displayed Modifier Bonuses (Un-inflate stat bonuses)
	creature_ptr->dis_to_ac += ((int)(adj_dex_to_ac[creature_ptr->stat_ind[STAT_DEX]]));
	creature_ptr->dis_to_hit_b  += ((int)(adj_dex_to_hit[creature_ptr->stat_ind[STAT_DEX]]));
	creature_ptr->dis_to_hit_b  += ((int)(adj_str_to_hit[creature_ptr->stat_ind[STAT_STR]]));

	// Different calculation for monks with empty hands
	if((empty_hands_status & EMPTY_HAND_RARM) && !creature_ptr->can_melee[1])
	{
		int blow_base = creature_ptr->lev + adj_dex_blow[creature_ptr->stat_ind[STAT_DEX]];

		if(creature_ptr->class_idx == CLASS_FORCETRAINER)
		{
			if(creature_ptr->class_skills.old_skills.magic_num1[0])
			{
				creature_ptr->to_damage[0] += (s16b)(creature_ptr->class_skills.old_skills.magic_num1[0]/5);
				creature_ptr->dis_to_damage[0] += (s16b)(creature_ptr->class_skills.old_skills.magic_num1[0]/5);
			}
		}

		if(heavy_armor(creature_ptr) && (creature_ptr->class_idx != CLASS_BERSERKER));
		else
		{
			creature_ptr->to_hit[0] += (creature_ptr->lev / 3);
			creature_ptr->dis_to_hit[0] += (creature_ptr->lev / 3);

			creature_ptr->to_damage[0] += (creature_ptr->lev / 6);
			creature_ptr->dis_to_damage[0] += (creature_ptr->lev / 6);
		}

		if(creature_ptr->posture & KAMAE_SEIRYU)
		{
			//TODO creature_ptr->resist_acid = TRUE;
			//TODO creature_ptr->resist_fire = TRUE;
			//TODO creature_ptr->resist_elec = TRUE;
			//TODO creature_ptr->resist_cold = TRUE;
			//TODO creature_ptr->resist_pois = TRUE;
			//TODO creature_ptr->sh_fire = TRUE;
			//TODO creature_ptr->sh_elec = TRUE;
			//TODO creature_ptr->sh_cold = TRUE;
			//TODO creature_ptr->levitation = TRUE;
		}
		else if(creature_ptr->posture & KAMAE_GENBU)
		{
			//TODO has_trait(creature_ptr, TRAIT_REFLECTING) = TRUE;
		}
		else if(creature_ptr->posture & KAMAE_SUZAKU)
		{
			creature_ptr->to_hit[0] -= (creature_ptr->lev / 3);
			creature_ptr->to_damage[0] -= (creature_ptr->lev / 6);

			creature_ptr->dis_to_hit[0] -= (creature_ptr->lev / 3);
			creature_ptr->dis_to_damage[0] -= (creature_ptr->lev / 6);
			//TODO creature_ptr->levitation = TRUE;
		}

	}

}

static void set_divine_bonuses(creature_type *creature_ptr)
{
	int i, j;

	for(i = 0; i < STAT_MAX; i++)
	{
		for(j = 0; j < max_authorities_idx; j++)
			if(HAS_AUTHORITY(creature_ptr, j))
				creature_ptr->stat_add[i] += authority_info[j].a_adj[i] * 10;

		if(creature_ptr->dr < 0 && creature_ptr->patron_idx != INDEX_NONE && creature_ptr->patron_idx != creature_ptr->species_idx)
		{
			creature_type *patron_ptr = find_unique_instance(creature_ptr->patron_idx);
			if(patron_ptr)
			{
				for(j = 0; j < max_authorities_idx; j++)
					if(HAS_AUTHORITY(patron_ptr, j))
						creature_ptr->stat_add[i] += authority_info[j].a_adj[i] * 10;
			}
		}
	}

	// Plus AC on Divine Rank
	if(creature_ptr->dr >= 0){
		creature_ptr->dis_to_ac += adj_dr_ac[creature_ptr->dr];
		creature_ptr->to_ac += adj_dr_ac[creature_ptr->dr];
	}

	for(i = 0; i < max_authorities_idx; i++)
	{
		if(HAS_AUTHORITY(creature_ptr, i))
		{
			creature_ptr->skill_dis += authority_info[i].a_dis;
			creature_ptr->skill_dev += authority_info[i].a_dev;
			creature_ptr->skill_stl += authority_info[i].a_stl;
			creature_ptr->skill_srh += authority_info[i].a_srh;
			creature_ptr->skill_fos += authority_info[i].a_fos;
			creature_ptr->skill_thn += authority_info[i].a_thn;
			creature_ptr->skill_thb += authority_info[i].a_thb;
			creature_ptr->skill_tht += authority_info[i].a_thb;
		}
	}

	if(creature_ptr->dr < 0 && creature_ptr->patron_idx != INDEX_NONE && creature_ptr->patron_idx != creature_ptr->species_idx)
	{
		creature_type *patron_ptr = find_unique_instance(creature_ptr->patron_idx);
		if(patron_ptr)
		{
			for(i = 0; i < max_authorities_idx; i++)
			{
				if(HAS_AUTHORITY(patron_ptr, i))
				{
					creature_ptr->skill_dis += authority_info[i].w_dis;
					creature_ptr->skill_dev += authority_info[i].w_dev;
					creature_ptr->skill_stl += authority_info[i].w_stl;
					creature_ptr->skill_srh += authority_info[i].w_srh;
					creature_ptr->skill_fos += authority_info[i].w_fos;
					creature_ptr->skill_thn += authority_info[i].w_thn;
					creature_ptr->skill_thb += authority_info[i].w_thb;
					creature_ptr->skill_tht += authority_info[i].w_thb;
				}
			}
		}
	}

	if(creature_ptr->dr >= 0)
		for(i = 0; i < STAT_MAX; i++) creature_ptr->stat_mod_max_max[i] += creature_ptr->dr / 4 * 10;

	if(creature_ptr->dr >= 0) creature_ptr->speed += adj_dr_speed[creature_ptr->dr];
}


static void set_riding_bonuses(creature_type *creature_ptr)
{
	int i, j;
	creature_type *steed_ptr = &creature_list[creature_ptr->riding];
	species_type *riding_r_ptr = &species_info[steed_ptr->species_idx];
	int speed = steed_ptr->speed;
	int penalty = 0;

	j = creature_ptr->carrying_weight;

	if(steed_ptr->speed > 0)
	{
		creature_ptr->speed = (s16b)(speed * (creature_ptr->skill_exp[SKILL_RIDING] * 3 + creature_ptr->lev * 160L - 10000L) / (22000L));
		if(creature_ptr->speed < 0) creature_ptr->speed = 0;
	}
	else creature_ptr->speed = speed;

	creature_ptr->speed += (creature_ptr->skill_exp[SKILL_RIDING] + creature_ptr->lev *160L) / 3200;

	//TODO if(is_kill_wall_species(riding_r_ptr)) creature_ptr->kill_wall = TRUE;

	if(creature_ptr->skill_exp[SKILL_RIDING] < RIDING_EXP_SKILLED) j += (creature_ptr->wt * 3 * (RIDING_EXP_SKILLED - creature_ptr->skill_exp[SKILL_RIDING])) / RIDING_EXP_SKILLED;

	i = 1500 + riding_r_ptr->level * 25; // Extract the "weight limit"

	creature_ptr->riding_two_handed = FALSE;

	if(creature_ptr->two_handed || (empty_hands(creature_ptr, FALSE) == EMPTY_HAND_NONE)) creature_ptr->riding_two_handed = TRUE;

	if(creature_ptr->pet_extra_flags & PF_RYOUTE)
	{
		switch (creature_ptr->class_idx)
		{
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		case CLASS_BERSERKER:
			if((empty_hands(creature_ptr, FALSE) != EMPTY_HAND_NONE) && !get_equipped_slot_num(creature_ptr, INVEN_SLOT_HAND))
				creature_ptr->riding_two_handed = TRUE;
			break;
		}
	}

	if((creature_ptr->class_idx == CLASS_BEASTMASTER) || (creature_ptr->class_idx == CLASS_CAVALRY))
	{
		if(creature_ptr->tval_ammo != TV_ARROW) penalty = 5;
	}
	else
	{
		penalty = species_info[creature_list[creature_ptr->riding].species_idx].level - creature_ptr->skill_exp[SKILL_RIDING] / 80;
		penalty += 30;
		if(penalty < 30) penalty = 30;
	}
	if(creature_ptr->tval_ammo == TV_BOLT) penalty *= 2;
	creature_ptr->to_hit_b -= penalty;
	creature_ptr->dis_to_hit_b -= penalty;
}


static void set_size_bonuses(creature_type *creature_ptr)
{
	// Base skills
	creature_ptr->size = calc_bodysize(creature_ptr->ht, creature_ptr->wt);
}

static void fix_creature_status(creature_type *creature_ptr)
{
	if(creature_ptr->speed > MAX_SPEED) creature_ptr->speed = MAX_SPEED;
	if(creature_ptr->speed < MIN_SPEED) creature_ptr->speed = MIN_SPEED;

	if(creature_ptr->skill_stl > MAX_STEALTH) creature_ptr->skill_stl = MAX_STEALTH;
	if(creature_ptr->skill_stl < MIN_STEALTH) creature_ptr->skill_stl = MIN_STEALTH;

	if(creature_ptr->skill_dig < 1) creature_ptr->skill_dig = 1;

	// Hack -- Each elemental immunity includes resistance
	//TODO if(creature_ptr->immune_acid) creature_ptr->resist_acid = TRUE;
	//TODO if(creature_ptr->immune_elec) creature_ptr->resist_elec = TRUE;
	//TODO if(creature_ptr->immune_fire) creature_ptr->resist_fire = TRUE;
	//TODO if(creature_ptr->immune_cold) creature_ptr->resist_cold = TRUE;

	 // Hack -- aura of fire also provides light
	//TODO if(creature_ptr->sh_fire) creature_ptr->lite = TRUE;

	if(has_trait(creature_ptr, TRAIT_PASSIVE_TELEPORT)) creature_ptr->cursed &= ~(TRAIT_PASSIVE_TELEPORT);

	if((IS_RACE(creature_ptr, RACE_S_FAIRY)) && has_trait(creature_ptr, TRAIT_ANTIPATHY))
	{
		//TODO creature_ptr->cursed &= ~(TRC_AGGRAVATE);
		creature_ptr->skill_stl = MIN(creature_ptr->skill_stl - 3, (creature_ptr->skill_stl + 2) / 2);
	}

	if(has_trait(creature_ptr, TRAIT_DUSK_ENCHANT))
	{
		if(creature_ptr->to_ac > (0 - creature_ptr->ac))         creature_ptr->to_ac     = 0 - creature_ptr->ac;
		if(creature_ptr->dis_to_ac > (0 - creature_ptr->dis_ac)) creature_ptr->dis_to_ac = 0 - creature_ptr->dis_ac;
	}
}

static void set_flow_flag(creature_type *creature_ptr)
{
	int i;
	bool have_sw = FALSE, have_kabe = FALSE;
	s16b this_object_idx, next_object_idx = 0;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		if((creature_ptr->inventory[i].tval == TV_NATURE_BOOK) && (creature_ptr->inventory[i].sval == 2)) have_sw = TRUE;
		if((creature_ptr->inventory[i].tval == TV_CRAFT_BOOK) && (creature_ptr->inventory[i].sval == 2)) have_kabe = TRUE;
	}

	if(IS_IN_THIS_FLOOR(creature_ptr))
	{
		for (this_object_idx = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx; this_object_idx; this_object_idx = next_object_idx)
		{
			object_type *object_ptr;
			object_ptr = &object_list[this_object_idx]; // Acquire object
			next_object_idx = object_ptr->next_object_idx; // Acquire next object

			if((object_ptr->tval == TV_NATURE_BOOK) && (object_ptr->sval == 2)) have_sw = TRUE;
			if((object_ptr->tval == TV_CRAFT_BOOK)  && (object_ptr->sval == 2)) have_kabe = TRUE;
		}
	}

	if(has_trait(creature_ptr, TRAIT_PASS_WALL) && !has_trait(creature_ptr, TRAIT_KILL_WALL)) creature_ptr->no_flowed = TRUE;

	if(have_sw && ((creature_ptr->realm1 == REALM_NATURE) || (creature_ptr->realm2 == REALM_NATURE) || (creature_ptr->class_idx == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &magic_info[creature_ptr->class_idx].info[REALM_NATURE - 1][SPELL_SW];
		if(creature_ptr->lev >= s_ptr->slevel) creature_ptr->no_flowed = TRUE;
	}

	if(have_kabe && ((creature_ptr->realm1 == REALM_CRAFT) || (creature_ptr->realm2 == REALM_CRAFT) || (creature_ptr->class_idx == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &magic_info[creature_ptr->class_idx].info[REALM_CRAFT - 1][SPELL_KABE];
		if(creature_ptr->lev >= s_ptr->slevel) creature_ptr->no_flowed = TRUE;
	}

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
void set_creature_bonuses(creature_type *creature_ptr, bool message)
{
	bool old_dis_ac    = (bool)creature_ptr->dis_ac;
	bool old_dis_to_ac = (bool)creature_ptr->dis_to_ac;

	wipe_creature_calculation_status(creature_ptr);

	set_size_bonuses(creature_ptr);
	set_divine_bonuses(creature_ptr);

	set_race_bonuses(creature_ptr);
	set_unreached_race_level_penalty(creature_ptr);

	if(creature_ptr->class_idx != INDEX_NONE) set_class_bonuses(creature_ptr);
	if(creature_ptr->chara_idx != INDEX_NONE) set_character_bonuses(creature_ptr);

	set_trait_bonuses(creature_ptr);
	set_inventory_bonuses(creature_ptr); // Scan the usable inventory
	set_status_table_indexes(creature_ptr);
	set_state_bonuses(creature_ptr);

	if(creature_ptr->riding) set_riding_bonuses(creature_ptr);
	set_posture_bonuses(creature_ptr);

	set_melee_status(creature_ptr);

	set_karma_bonuses(creature_ptr);
	set_flow_flag(creature_ptr);
	fix_creature_status(creature_ptr);

	if(message) creature_bonuses_message(creature_ptr);

	// Hack -- See Invis Change
	//TODO creature_ptr->creature_update |= (PU_CREATURES);
	play_window |= PW_INVEN; // Redraw average damege display of Shuriken
	play_redraw |= (PR_SPEED); // TODO

	// Redraw armor (if needed)
	if((creature_ptr->dis_ac != old_dis_ac) || (creature_ptr->dis_to_ac != old_dis_to_ac))
	{
		play_redraw |= (PR_ARMOR);  // Redraw
		play_window |= (PW_PLAYER); // Window stuff
	}

}



/*
 * Handle "creature_ptr->creature_update"
 */
void notice_stuff(creature_type *creature_ptr)
{
	/* Notice stuff */
	if(!creature_ptr->creature_update) return;

	/* Actually do auto-destroy */
	if(creature_ptr->creature_update & (CRN_AUTODESTROY))
	{
		creature_ptr->creature_update &= ~(CRN_AUTODESTROY);
		autopick_delayed_alter(creature_ptr);
	}

	/* Combine the pack */
	if(creature_ptr->creature_update & (CRU_COMBINE))
	{
		creature_ptr->creature_update &= ~(CRU_COMBINE);
		combine_pack(creature_ptr);
	}

	/* Reorder the pack */
	if(creature_ptr->creature_update & (CRU_REORDER))
	{
		creature_ptr->creature_update &= ~(CRU_REORDER);
		reorder_pack(creature_ptr);
	}
}

// Handle "update"
void update_creature(creature_type *creature_ptr, bool message)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	// Update stuff
	if(!creature_ptr->creature_update) return;

	calculate_upkeep_servant(creature_ptr);
	set_creature_equip_slot_num(creature_ptr);

	if(creature_ptr->creature_update & (CRU_BONUS))
	{
		creature_ptr->creature_update &= ~(CRU_BONUS);
		set_creature_bonuses(creature_ptr, message);
	}

	if(creature_ptr->creature_update & (CRU_TORCH))
	{
		creature_ptr->creature_update &= ~(CRU_TORCH);
		calc_lite(creature_ptr);
	}

	if(creature_ptr->creature_update & (CRU_HP))
	{
		creature_ptr->creature_update &= ~(CRU_HP);
		calc_hitpoints(creature_ptr, message);
	}

	if(creature_ptr->creature_update & (CRU_MANA))
	{
		creature_ptr->creature_update &= ~(CRU_MANA);
		calc_mana(creature_ptr, message);
	}

	if(creature_ptr->creature_update & (CRU_SPELLS))
	{
		creature_ptr->creature_update &= ~(CRU_SPELLS);
		calc_spells(creature_ptr, message);
	}


	/* Character is not ready yet, no screen updates */
	if(!character_generated) return;

	if(creature_ptr->creature_update & (PU_UN_LITE))
	{
		creature_ptr->creature_update &= ~(PU_UN_LITE);
		forget_lite(floor_ptr);
	}

	if(creature_ptr->creature_update & (PU_UN_VIEW))
	{
		creature_ptr->creature_update &= ~(PU_UN_VIEW);
		forget_view(floor_ptr);
	}

	if(creature_ptr->creature_update & (PU_VIEW))
	{
		creature_ptr->creature_update &= ~(PU_VIEW);
		update_view(creature_ptr);
	}

	if(creature_ptr->creature_update & (PU_LITE))
	{
		creature_ptr->creature_update &= ~(PU_LITE);
		update_lite(creature_ptr);
	}


	if(creature_ptr->creature_update & (PU_FLOW))
	{
		creature_ptr->creature_update &= ~(PU_FLOW);
		update_flow(creature_ptr);
	}

	if(creature_ptr->creature_update & (PU_DISTANCE))
	{
		creature_ptr->creature_update &= ~(PU_DISTANCE);

		/* Still need to call update_creatures(FALSE) after update_creature_lite() */ 
		/* creature_ptr->creature_update &= ~(PU_CREATURES); */

		update_creatures(TRUE);
	}

	if(creature_ptr->creature_update & (PU_SPECIES_LITE))
	{
		creature_ptr->creature_update &= ~(PU_SPECIES_LITE);
		//update_creature_lite(floor_ptr);
	}

	/*
	 * Mega-Hack -- Delayed visual update
	 * Only used if update_view(), update_lite() or update_creature_lite() was called
	 */
	if(creature_ptr->creature_update & (PU_DELAY_VIS))
	{
		creature_ptr->creature_update &= ~(PU_DELAY_VIS);
		delayed_visual_update(floor_ptr);
	}

	if(creature_ptr->creature_update & (PU_CREATURES))
	{
		creature_ptr->creature_update &= ~(PU_CREATURES);
		update_creatures(FALSE);
	}
}


/*
 * Handle "play_redraw"
 */
void redraw_stuff(creature_type *subjectivity_ptr)
{
	if(!play_redraw) return;

	/* Character is not ready yet, no screen updates */
	if(!character_generated) return;

	/* Hack -- clear the screen */
	if(play_redraw & (PR_WIPE))
	{
		play_redraw &= ~(PR_WIPE);
		msg_print(NULL);
		Term_clear();
	}

	if(play_redraw & (PR_MAP))
	{
		play_redraw &= ~(PR_MAP);
		prt_map(subjectivity_ptr);
	}

	prt_frame_basic(subjectivity_ptr);

	if(play_redraw & (PR_BASIC))
	{
		play_redraw &= ~(PR_BASIC);
		play_redraw &= ~(PR_MISC | PR_TITLE | PR_STATS);
		play_redraw &= ~(PR_LEV | PR_EXP | PR_GOLD);
		play_redraw &= ~(PR_ARMOR | PR_HP | PR_MANA);
		play_redraw &= ~(PR_DEPTH | PR_HEALTH | PR_UHEALTH);
		prt_time(subjectivity_ptr);
		if(wizard) prt_wiz_pos(subjectivity_ptr);
		prt_dungeon();
	}

	if(play_redraw & (PR_MISC))
	{
		play_redraw &= ~(PR_MISC);
	}

	if(play_redraw & (PR_LEV))
	{
		play_redraw &= ~(PR_LEV);
		prt_level(subjectivity_ptr);
	}

	if(play_redraw & (PR_EXP))
	{
		play_redraw &= ~(PR_EXP);
		prt_exp(subjectivity_ptr);
	}

	if(play_redraw & (PR_STATS))
	{
		play_redraw &= ~(PR_STATS);
		prt_stat(subjectivity_ptr, STAT_STR);
		prt_stat(subjectivity_ptr, STAT_INT);
		prt_stat(subjectivity_ptr, STAT_WIS);
		prt_stat(subjectivity_ptr, STAT_DEX);
		prt_stat(subjectivity_ptr, STAT_CON);
		prt_stat(subjectivity_ptr, STAT_CHA);
	}

	if(play_redraw & (PR_STATUS))
	{
		play_redraw &= ~(PR_STATUS);
		prt_status(subjectivity_ptr);
	}

	if(play_redraw & (PR_ARMOR))
	{
		play_redraw &= ~(PR_ARMOR);
		prt_ac_ev(subjectivity_ptr);
	}

	if(play_redraw & (PR_HP))
	{
		play_redraw &= ~(PR_HP);
		prt_hp(subjectivity_ptr);
	}

	if(play_redraw & (PR_MANA))
	{
		play_redraw &= ~(PR_MANA);
		prt_sp(subjectivity_ptr);
	}

	if(play_redraw & (PR_GOLD))
	{
		play_redraw &= ~(PR_GOLD);
		prt_gold(subjectivity_ptr);
	}

	if(play_redraw & (PR_DEPTH))
	{
		play_redraw &= ~(PR_DEPTH);
		prt_depth(subjectivity_ptr);
	}

	if(play_redraw & (PR_HEALTH))
	{
		play_redraw &= ~(PR_HEALTH);
		health_redraw(subjectivity_ptr, FALSE);
	}

	if(play_redraw & (PR_UHEALTH))
	{
		play_redraw &= ~(PR_UHEALTH);
		health_redraw(subjectivity_ptr, TRUE);
	}


	if(play_redraw & (PR_EXTRA))
	{
		play_redraw &= ~(PR_EXTRA);
		play_redraw &= ~(PR_CUT | PR_STUN);
		play_redraw &= ~(PR_HUNGER);
		play_redraw &= ~(PR_STATE | PR_SPEED | PR_STUDY | PR_IMITATION | PR_STATUS);
		prt_frame_extra(subjectivity_ptr);
	}

	if(play_redraw & (PR_CUT))
	{
		play_redraw &= ~(PR_CUT);
		prt_cut(subjectivity_ptr);
	}

	if(play_redraw & (PR_STUN))
	{
		play_redraw &= ~(PR_STUN);
		prt_stun(subjectivity_ptr);
	}

	if(play_redraw & (PR_HUNGER))
	{
		play_redraw &= ~(PR_HUNGER);
		prt_hunger(subjectivity_ptr);
	}

	if(play_redraw & (PR_STATE))
	{
		play_redraw &= ~(PR_STATE);
		prt_state(subjectivity_ptr);
	}

	if(play_redraw & (PR_SPEED))
	{
		play_redraw &= ~(PR_SPEED);
		prt_speed(subjectivity_ptr);
	}

	if(subjectivity_ptr->class_idx == CLASS_IMITATOR)
	{
		if(play_redraw & (PR_IMITATION))
		{
			play_redraw &= ~(PR_IMITATION);
			prt_imitation(subjectivity_ptr);
		}
	}
	else if(play_redraw & (PR_STUDY))
	{
		play_redraw &= ~(PR_STUDY);
		prt_study(subjectivity_ptr);
	}
}


/*
 * Handle "play_window"
 */
void window_stuff(creature_type *subjectivity_ptr)
{
	int j;
	u32b mask = 0L;

	if(!play_window) return;	// Nothing to do

	for (j = 0; j < 8; j++) if(angband_term[j]) mask |= window_flag[j];	// Scan windows
	play_window &= mask;	// Apply usable flags
	if(!play_window) return;	// Nothing to do

	// Display inventory
	if(play_window & (PW_INVEN))
	{
		play_window &= ~(PW_INVEN);
		fix_inven(subjectivity_ptr);
	}

	// Display equipment
	if(play_window & (PW_EQUIP))
	{
		play_window &= ~(PW_EQUIP);
		fix_equip(subjectivity_ptr);
	}

	// Display spell list
	if(play_window & (PW_SPELL))
	{
		play_window &= ~(PW_SPELL);
		fix_spell(subjectivity_ptr);
	}

	// Display player
	if(play_window & (PW_PLAYER))
	{
		play_window &= ~(PW_PLAYER);
		fix_player(subjectivity_ptr);
	}

	// Display overhead view
	if(play_window & (PW_MESSAGE))
	{
		play_window &= ~(PW_MESSAGE);
		fix_message();
	}

	// Display overhead view
	if(play_window & (PW_OVERHEAD))
	{
		play_window &= ~(PW_OVERHEAD);
		fix_overhead(subjectivity_ptr);
	}

	// Display overhead view
	if(play_window & (PW_DUNGEON))
	{
		play_window &= ~(PW_DUNGEON);
		fix_dungeon(subjectivity_ptr);
	}

	// Display creature recall
	if(play_window & (PW_MONSTER))
	{
		play_window &= ~(PW_MONSTER);
		fix_creature(subjectivity_ptr);
	}

	// Display object recall
	if(play_window & (PW_OBJECT))
	{
		play_window &= ~(PW_OBJECT);
		fix_object(subjectivity_ptr);
	}
}

// Handle "update" and "play_redraw" and "play_window"
void handle_stuff(void)
{
	if(player_ptr->creature_update) update_creature(player_ptr, TRUE);
	if(play_redraw) redraw_stuff(player_ptr);
	if(play_window) window_stuff(player_ptr);
}

s16b empty_hands(creature_type *creature_ptr, bool riding_control)
{
	s16b status = 0x0000, least_zero_bit;
	int i;

	for(i = 0; i < MAX_WEAPONS; i++)
		if(!get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, i)->k_idx) status |= 0x0001 << i;

	if(riding_control && status && creature_ptr->riding && !(creature_ptr->pet_extra_flags & PF_RYOUTE))
	{
		least_zero_bit = ~status & -(~status);
		status &= ~least_zero_bit;
	}

	return status;
}


bool heavy_armor(creature_type *creature_ptr)
{
	u16b monk_arm_wgt = 0;

	if(has_trait(creature_ptr, TRAIT_WANT_LIGHT_WEIGHT)) return FALSE;

	/* Weight the armor */
	//TODO
	//if(creature_ptr->inventory[].tval > TV_SWORD) monk_arm_wgt += creature_ptr->inventory[].weight;
	//if(creature_ptr->inventory[].tval > TV_SWORD) monk_arm_wgt += creature_ptr->inventory[].weight;
	//monk_arm_wgt += creature_ptr->inventory[].weight;
	//monk_arm_wgt += creature_ptr->inventory[].weight;
	//monk_arm_wgt += creature_ptr->inventory[].weight;
	//monk_arm_wgt += creature_ptr->inventory[].weight;
	//monk_arm_wgt += creature_ptr->inventory[].weight;

	return (monk_arm_wgt > (100 + (creature_ptr->lev * 4)) * creature_ptr->size / 10 * creature_ptr->size / 10);
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

// get subrace
bool get_subrace(creature_type *creature_ptr, int n)
{
	if(creature_ptr->sub_race[n / 32] & (0x01 << (n % 32))) return TRUE;
	return FALSE;
}

// set subrace
void set_subrace(creature_type *creature_ptr, int n, bool b)
{
	unsigned int a = n;
	if(b) creature_ptr->sub_race[a / 32] = creature_ptr->sub_race[a / 32] | (0x01 << (a % 32));
	else creature_ptr->sub_race[a / 32] = creature_ptr->sub_race[a / 32] & !(0x01 << (a % 32));
}



// Return Race Name
cptr desc_race_name(creature_type *creature_ptr, u32b mode){
	char name[80];
	char subname[80];
	race_type *rcreature_ptr = &race_info[creature_ptr->race_idx1];
	name[0] = '\0';
	subname[0] = '\0';

	if(creature_ptr->race_idx1 == INDEX_NONE) return format("");
	else if(creature_ptr->race_idx2 == INDEX_NONE) return format("%s", race_info[creature_ptr->race_idx1].title);

	if(has_trait(creature_ptr, TRAIT_GOLEM))
	{
#if JP
		strcat(subname, "型ゴーレム");
#else
		strcat(subname, "-type Golem");
#endif
	}

	if(has_trait(creature_ptr, TRAIT_ANDROID))
	{
#if JP
		strcat(subname, "型アンドロイド");
#else
		strcat(subname, "-type Android");
#endif
	}


	if(has_trait(creature_ptr, TRAIT_VAMPIRE))
	{
#if JP
		strcat(subname, "の吸血鬼");
#else
		strcat(subname, " Vampire");
#endif
	}

	if(has_trait(creature_ptr, TRAIT_ZOMBIE))
	{
#if JP
		strcat(subname, "のゾンビ");
#else
		strcat(subname, " Zombie");
#endif
	}

	if(has_trait(creature_ptr, TRAIT_SKELETON))
	{
#if JP
		strcat(subname, "のスケルトン");
#else
		strcat(subname, " Skeleton");
#endif
	}

	if(has_trait(creature_ptr, TRAIT_LICH))
	{
#if JP
		strcat(subname, "のリッチ");
#else
		strcat(subname, " Lich");
#endif
	}

	if(has_trait(creature_ptr, TRAIT_NAZGUL))
	{
#if JP
		strcat(subname, "のナズグル");
#else
		strcat(subname, " Nazgul");
#endif
	}

	if(has_trait(creature_ptr, TRAIT_PUELLA_MAGI))
	{
#if JP
		strcat(subname, "の魔法少女");
#else
		strcat(subname, " Puella Magi");
#endif
	}


	if(subname[0])
	{
#if JP
		strcat(name, "元");
#else
		strcat(name, "Ex-");
#endif
	}

	if(mode & CD_PURE_RACE)
	if(IS_PURE(creature_ptr))
	{
		if(!name[0])
		{
#if JP

			if(mode & CD_PURE_RACE) strcat(name, "純血種の");
#else
			if(mode & CD_PURE_RACE) strcat(name, "Pure-blooded ");
			if(mode & CD_SEX)
			{
				strcat(name, sex_info[creature_ptr->sex].title);
				strcat(name, " ");
			}
#endif
		}
		strcat(name, race_info[creature_ptr->race_idx1].title);
	}
	else
	{
		int i;
		for(i = 0; i < MAX_HALF_RACE_DESCRIPTION; i++)
		{
			if((creature_ptr->race_idx1 == half_race_desc[i].race1 && creature_ptr->race_idx2 == half_race_desc[i].race2) ||
			   (creature_ptr->race_idx2 == half_race_desc[i].race1 && creature_ptr->race_idx1 == half_race_desc[i].race2))
			{
				strcat(name, half_race_desc[i].title);
				break;
			}
		}

		if(!name[0])
		{
#if !JP
			strcat(name, sex_info[creature_ptr->sex].title);
			strcat(name, "Mixed ");
#endif
			strcat(name, race_info[creature_ptr->race_idx1].title);
#if JP
			strcat(name, "と");
#else
			strcat(name, "and ");
#endif
			strcat(name, race_info[creature_ptr->race_idx2].title);
#if JP
			strcat(name, "の混血");
#endif
		}
	}


	strcat(name, subname);

	if(mode & CD_SEX)
	{
		strcat(name, "(");
		strcat(name, sex_info[creature_ptr->sex].title);
		strcat(name, ")");
	}

	return format("%s", name);
}


cptr desc_class_name(creature_type *creature_ptr, u32b mode){
	char name[80];
	name[0] = '\0';

	strcat(name, class_info[creature_ptr->class_idx].title);

	if(mode & CD_SUB_CLASS)
	{
		if(has_trait(creature_ptr, TRAIT_PUELLA_MAGI))
		{
#if JP
			strcat(name, "/魔法少女");
#else
			strcat(name, "/Puella-Magi");
#endif
		}
	}

	if(mode & CD_REALM)
	{
		if(creature_ptr->realm1 != REALM_NONE && creature_ptr->realm1 < MAX_REALM)
		{
			strcat(name, " (");
			strcat(name, realm_names[creature_ptr->realm1]);
			if(creature_ptr->realm2 != REALM_NONE && creature_ptr->realm2 < MAX_REALM)
			{
				strcat(name, ", ");
				strcat(name, realm_names[creature_ptr->realm2]);
			}
			strcat(name, ")");
		}
	}


	return format("%s", name);

}

