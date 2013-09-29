/* Purpose: create a player character */

/* 2011 Deskull modified. */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "autopick.h"
#include "birth.h"
#include "command.h"
#include "creature_inventory.h"
#include "diary.h"
#include "files.h"

/*
 * How often the autoroller will update the display and pause
 * to check for user interuptions.
 * Bigger values will make the autoroller faster, but slower
 * system may have problems because the user can't stop the
 * autoroller for this number of rolls.
 */
#define AUTOROLLER_STEP 5431L

static cptr starting_point_jouhou[STARTING_MAX] =
{
"イェンダーは〈烙印者〉が強く惹きつけられずには居られない宿命の地です。『運命の大迷宮』の奥には明確な*勝利*条件達成の手段である『イェンダーの魔除け』が眠っています。最も速やかな*勝利*を得るにはこの土地から始めるのが良いでしょう。",
"イェーキンは迫害の種族イーク達が、不毛の地に多くの犠牲を経て築いた約束の地です。しかし、ごく最近になって指導者である大王ボルドールとその一族の周りに奇妙で恐ろしい出来事が起きています。この地にはイーク以外も彼等の協力者たる異種族の要人も何人かおり、最低限の施設が揃っています。",
"ドゥードリンには古くからこの土地に住んでいた呪術の信奉者である原住民が住んでいます。彼等はねじれた時空の南方よりやってきた小人達の侵略を受けており、抵抗を重ねていますが現在は北の僻地に追いやられ、彼等の築き上げた洞窟の砦に深い怨嗟の念を注いでいます。",
"ウルウリストはオークの部族を蹴散らした西方の民が作り上げた城塞都市です。ですが南東には元の民であったオーク達すら近づかなかったという恐るべき病の気に満ちた谷が災いを撒き散らしています。城塞の民はこれもまたモルゴスやサウロンがもたらしたものと主張していますが、明確な証拠はないようです。",

"森深くにあるアングウィルはその名や建築の様式の通り、本来上古のエルフが作り上げた都市です。その第一統領である指輪の乙女コルマンウェンもテレリ族の王族に近しい齢数千歳のエルフです。しかし彼女を頭とし栄華を掴み取った同市の富裕民達は西方の「自由の民」を称する者達を明らかに軽蔑しています。一方、彼等は東方の列強、特にウルクやアヴァリの諸国に対しては非常に友好的で、彼等へ良質の金属を輸出することで栄えています。",
"砂漠の奥地シラは、かつては大きな都であった痕跡を残していますが現在は零細な牧畜の光景が見られるばかりの寂れた地です。集落の北には最も巨大な遺跡である地下王墓が広がっています。その中に眠る遺産を求める山師や冒険者は後を絶ちませんが、大抵は生きて帰らず、得る物を得た者はあまりにも少ないようです。",
"キセダラバジは非常に古くから人が住んでいる土地と思われるのですが、住民達も含めてその由来が一切分かりません。本来不倶戴天の敵同士とも言える種族達が平然と婚姻を交わして生まれた、複雑な混血児達が人口の大多数を占めています。どことなく茫洋とした態度の住民達は、あらゆる者達に対して友好的ではありますが、流石に北東に忽然と現れた色の無い霧の城からもたらされる厄介事と、デーモンの群れには憮然とせずにいられないようです。",
"端海は海岸に接した土地ですが、専ら陸上の交易を生業とする者達のオアシスとして認知されています。北西の崑崙山には峰の一つに拠点を持つ巨大な盗賊団がいるようです。また、この土地にいる鍛冶の一族は無名ながらも、滅多に見られない強力な武器を鍛造する術を持っています。",

"TODO:ズル",
"ヴィーヤウトゥムノには混沌の宮廷の貴族となったアルダ上古のアイヌア、ヴァララウコであったロムンクァリィ女伯が君臨しています。かつてトゥルカスに破壊されたウトゥムノの跡地そのままを復興して作り上げました。地獄界の貴族階級と多元宇宙の市民階級が併存するこの地は、富国強兵を巧みに体現しています。",
"TODO:モリバンド",
"TODO:テルモラ",

"TODO:テキソラミ",
"TODO:ルグブルズ",
"TODO:ルルイエ",

};

// Autoroll limit
static s16b stat_limit[STAT_MAX];

static struct {
	s16b agemin, agemax;
	s16b htmin, htmax;
	s16b wtmin, wtmax;
	s16b scmin, scmax;
} chara_limit;

// Autoroll matches
static s32b stat_match[STAT_MAX];

/*
 * Autoroll round
 */
static s32b auto_round;


static void birth_quit(void)
{
	remove_loc();
	quit(NULL);
}

/*
 *  Put initial status
 */
static void put_initial_status(creature_type *creature_ptr)
{
	cptr race, cls;

	put_str("      :                                                                         ", 1, 1);
	put_str("      :                                                                         ", 2, 1);
	put_str("      :                                                                         ", 3, 1);
	put_str(get_keyword("KW_RACE"), 1, 1);
	put_str(get_keyword("KW_CLASS"), 2, 1);
	put_str(get_keyword("KW_PATRON"), 3, 1);

	if(creature_ptr->race_idx1 != INDEX_NONE)
	{
		race = desc_race_name(creature_ptr, CD_SEX | CD_PURE_RACE);
		c_put_str(TERM_L_BLUE, race, 1, 9);
	}

	if(creature_ptr->class_idx != INDEX_NONE)
	{
		cls = desc_class_name(creature_ptr, CD_SUB_CLASS | CD_REALM);
		c_put_str(TERM_L_BLUE, cls, 2, 9);
	}

	if(creature_ptr->patron_idx != INDEX_NONE)
	{
		c_put_str(TERM_L_BLUE, species_name + species_info[creature_ptr->patron_idx].name, 3, 9);
	}


}


/*
 *  Show specific help file
 */
static void show_help(cptr helpfile)
{
	screen_save();
	(void)show_file(TRUE, helpfile, NULL, 0, 0);
	screen_load();
}

// Choose from one of the available magical realms
static s16b choose_realm(s32b choices, bool npc)
{
	int i;
	selection_table re[MAX_REALMS + 3];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = realm_detail;
	se_info.default_se = 0;
	se_info.y = 5;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 20;
	se_info.num = 0;

	/* Count the choices */
	if(choices & CH_LIFE)
	{
		re[se_info.num].cap = realm_names[REALM_LIFE];
		re[se_info.num].code = REALM_LIFE;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_SORCERY)
	{
		re[se_info.num].cap = realm_names[REALM_SORCERY];
		re[se_info.num].code = REALM_SORCERY;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_NATURE)
	{
		re[se_info.num].cap = realm_names[REALM_NATURE];
		re[se_info.num].code = REALM_NATURE;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_CHAOS)
	{
		re[se_info.num].cap = realm_names[REALM_CHAOS];
		re[se_info.num].code = REALM_CHAOS;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_DEATH)
	{
		re[se_info.num].cap = realm_names[REALM_DEATH];
		re[se_info.num].code = REALM_DEATH;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_TRUMP)
	{
		re[se_info.num].cap = realm_names[REALM_TRUMP];
		re[se_info.num].code = REALM_TRUMP;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_ARCANE)
	{
		re[se_info.num].cap = realm_names[REALM_ARCANE];
		re[se_info.num].code = REALM_ARCANE;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_ENCHANT)
	{
		re[se_info.num].cap = realm_names[REALM_CRAFT];
		re[se_info.num].code = REALM_CRAFT;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_DAEMON)
	{
		re[se_info.num].cap = realm_names[REALM_DAEMON];
		re[se_info.num].code = REALM_DAEMON;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_CRUSADE)
	{
		re[se_info.num].cap = realm_names[REALM_CRUSADE];
		re[se_info.num].code = REALM_CRUSADE;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_MUSIC)
	{
		re[se_info.num].cap = realm_names[REALM_MUSIC];
		re[se_info.num].code = REALM_MUSIC;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_HISSATSU)
	{
		re[se_info.num].cap = realm_names[REALM_HISSATSU];
		re[se_info.num].code = REALM_HISSATSU;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	if(choices & CH_HEX)
	{
		re[se_info.num].cap = realm_names[REALM_HEX];
		re[se_info.num].code = REALM_HEX;
		re[se_info.num].key = '\0';
		re[se_info.num].d_color = TERM_L_DARK;
		re[se_info.num].l_color = TERM_WHITE;
		se_info.num++;
	}
	
	if(se_info.num <= 0) return REALM_NONE;

	re[se_info.num].cap = KW_RANDOM;
	re[se_info.num].code = BIRTH_SELECT_RANDOM;
	re[se_info.num].key = '*';
	re[se_info.num].d_color = TERM_UMBER;
	re[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	re[se_info.num].cap = KW_BACK_TO_START;
	re[se_info.num].code = BIRTH_SELECT_RETURN;
	re[se_info.num].key = 'S';
	re[se_info.num].d_color = TERM_UMBER;
	re[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	re[se_info.num].cap = KW_QUIT_GAME;
	re[se_info.num].code = BIRTH_SELECT_QUIT;
	re[se_info.num].key = 'Q';
	re[se_info.num].d_color = TERM_UMBER;
	re[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	if(!npc) i = get_selection(&se_info, re);
	else return (s16b)re[randint0(se_info.num - 3)].code;

	if(i >= 0) return (s16b)i;
	else if(i == BIRTH_SELECT_RANDOM) return (s16b)re[randint0(se_info.num - 3)].code;
	else return (s16b)i;

}


/*
 * Choose the magical realms
 */
static bool get_creature_realms(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	s16b i;

	if(!npc)
	{
		clear_from(0);
		put_initial_status(creature_ptr);
	}

	if(!npc)
	{
		put_str(MES_BIRTH_REALM, 0, 0);
		put_initial_status(creature_ptr);
	}

	/* Select the first realm */

	if(species_ptr->realm1 != INDEX_VARIABLE)
	{
		creature_ptr->realm1 = species_ptr->realm1;
	}
	else
	{
		creature_ptr->realm1 = REALM_NONE;
		creature_ptr->realm2 = 255;
		i = choose_realm(realm_choices1[creature_ptr->class_idx], npc);

		if(i == BIRTH_SELECT_RETURN) return BIRTH_SELECT_RETURN;
		else if(i == BIRTH_SELECT_QUIT) return BIRTH_SELECT_QUIT;
		else creature_ptr->realm1 = i;
	}

	if(!npc)
	{
		put_str(MES_BIRTH_REALM, 0, 0);
		put_initial_status(creature_ptr);
	}

	if(species_ptr->realm2 != INDEX_VARIABLE) creature_ptr->realm2 = species_ptr->realm2;
	else
	{
		
		/* Select the second realm */
		creature_ptr->realm2 = REALM_NONE;
		i = choose_realm(realm_choices2[creature_ptr->class_idx] & ~(1 << (creature_ptr->realm1 - 1)), npc);
		if(i == BIRTH_SELECT_RETURN)
			return BIRTH_SELECT_RETURN;
		else if(i == BIRTH_SELECT_QUIT)
			return BIRTH_SELECT_QUIT;
		else
			creature_ptr->realm2 = i;
	}

	return TRUE;
}

void get_max_stats(creature_type *creature_ptr)
{
	int i, j;
	int dice[STAT_MAX];

	/* Roll and verify some stats */
	while (TRUE)
	{
		/* Roll some dice */
		for (j = i = 0; i < STAT_MAX; i++)
		{
			/* Roll the dice */
			dice[i] = randint1(7);

			/* Collect the maximum */
			j += dice[i];
		}

		/* Verify totals */
		if(j == 24) break;
	}

	/* Acquire the stats */
	for (i = 0; i < STAT_MAX; i++)
	{
		j = STAT_VALUE_BASE_MAX_MAX + dice[i] * 10;
		// Save that value
		creature_ptr->stat_max_max[i] = (s16b)j;
		if(creature_ptr->stat_max[i] > j) creature_ptr->stat_max[i] = (s16b)j;
		if(creature_ptr->stat_cur[i] > j) creature_ptr->stat_cur[i] = (s16b)j;
	}
	creature_ptr->knowledge &= ~(KNOW_STAT);

	/* Redisplay the stats later */
	prepare_redraw(PR_STATS);
}


/*
 * Roll for some info that the auto-roller ignores
 */
static void set_extra(creature_type *creature_ptr, bool roll_hitdice)
{
	set_expfact(creature_ptr);

	// Reset record of realm changes
	creature_ptr->old_realm = 0;

	initialize_skill(creature_ptr);

	set_bodysize(creature_ptr);
	set_hitdice(creature_ptr);

	/* Roll for hit point unless quick-start */
	if(roll_hitdice) set_base_hp(creature_ptr);
}


/*
 * Get the racial history, and social class, using the "history charts".
 */
static void set_history(creature_type *creature_ptr)
{
	int i, j, n;
	char *s, *t;

	char buf[240];
	species_type *species_ptr = &species_info[creature_ptr->species_idx];
	char *history = (species_text + species_ptr->text);
	char temp[HISTORY_ROW * HISTORY_COL];


	/* Clear the previous history strings */
	for (i = 0; i < HISTORY_ROW; i++) creature_ptr->history[i][0] = '\0';

	/* Clear the history text */
	buf[0] = '\0';

	/* Skip leading spaces */
	for (s = buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	if(history)
	{
		/* Clear the previous history strings */
		for (i = 0; i < HISTORY_ROW; i++) creature_ptr->history[i][0] = '\0';

		/* Skip leading spaces */
		for (s = history; *s == ' '; s++) /* loop */;

		/* Get apparent length */
		n = strlen(s);

		/* Kill trailing spaces */
		while ((n > 0) && (s[n - 1] == ' ')) s[--n] = '\0';

		roff_to_buf(s, HISTORY_COL, temp, sizeof(temp));
		t = temp;
		for (i = 0; i < HISTORY_ROW; i++)
		{
			if(t[0] == 0) break;
			else
			{
				strcpy(creature_ptr->history[i], t);
				t += strlen(t) + 1;
			}
		}

		/* Fill the remaining spaces */
		for (i = 0; i < HISTORY_ROW; i++)
		{
			for (j = 0; creature_ptr->history[i][j]; j++) /* loop */;
			for (; j < HISTORY_COL - 1; j++) creature_ptr->history[i][j] = ' ';
			creature_ptr->history[i][HISTORY_COL - 1] = '\0';
		}

	}

	if(species_ptr->sc) creature_ptr->sc = species_info->sc;
	else
	{
		creature_ptr->sc = diceroll(4, 20);
		if(one_in_(10)) creature_ptr->sc += diceroll(1, 60);
	}

}


static void set_exp(creature_type *creature_ptr, species_type *species_ptr)
{
	s32b exp1, rate1;
	u32b exp2, rate2;

	exp1 = 0;
	exp2 = species_ptr->exp;
	rate1 = 0;
	rate2 = randnor(10000, EXP_STAND_RATE);

	s64b_mul(&exp1, &exp2, rate1, rate2);
	s64b_div(&exp1, &exp2, 0, 10000);

	exp2 = (exp2 >= 0) ? exp2 : 0;

	creature_ptr->exp = creature_ptr->max_exp = creature_ptr->max_max_exp = exp2;
}


// Computes character's age
// by henkma (get_ahw)
// Modified by deskull in D'angband.
static void set_age(creature_type *creature_ptr)
{
	species_type *species_ptr = &species_info[creature_ptr->species_idx];

	if(species_ptr->age <= 0)
	{
		creature_ptr->age = race_info[creature_ptr->race_idx1].b_age + race_info[creature_ptr->race_idx2].b_age;
		creature_ptr->age += randint0((race_info[creature_ptr->race_idx1].m_age + race_info[creature_ptr->race_idx2].m_age)/2);
	}
	else
	{
		creature_ptr->age = species_ptr->age;
	}
}


// Get the player's starting money
static void set_money(creature_type *creature_ptr)
{
	int gold;

	// Starting gold
	gold = randint1(100) + 300;
	if(creature_ptr->class_idx == CLASS_TOURIST) gold += 2000;
	if(gold < 100) gold = 100;

	if(creature_ptr->chara_idx == CHARA_NAMAKE) gold /= 2;
	else if(creature_ptr->chara_idx == CHARA_MUNCHKIN) gold = 10000000;
	if(has_trait(creature_ptr, TRAIT_ANDROID)) gold /= 5;

	gold += creature_ptr->sc * (creature_ptr->lev * creature_ptr->lev + 5) / 2; // Level and social class calc

	// Save the gold
	creature_ptr->au = gold;
}



/*
 * Clear all the global "character" data
 */
void creature_wipe(creature_type *creature_ptr)
{
	int i;

	/* Hack -- free the "last message" string */
	if(creature_ptr->last_message) string_free(creature_ptr->last_message);

	/* Hack -- zero the struct */
	(void)WIPE(creature_ptr, creature_type);

	/* Wipe the history */
	for (i = 0; i < HISTORY_ROW; i++) strcpy(creature_ptr->history[i], "");

	/* No weight */
	creature_ptr->carrying_weight = 0;
	creature_ptr->equipping_weight = 0;

	/* No items */
	creature_ptr->inven_cnt = 0;
	creature_ptr->equip_cnt = 0;

	/* Clear the creature_ptr->inventory */
	for (i = 0; i < INVEN_TOTAL; i++) object_wipe(&creature_ptr->inventory[i]);

	/* Hack -- Well fed player */
	creature_ptr->food = CREATURE_FOOD_FULL - 1;

	/* Wipe the spells */
	if(creature_ptr->class_idx == CLASS_SORCERER)
	{
		creature_ptr->spell_learned1 = creature_ptr->spell_learned2 = 0xffffffffL;
		creature_ptr->spell_worked1 = creature_ptr->spell_worked2 = 0xffffffffL;
	}
	else
	{
		creature_ptr->spell_learned1 = creature_ptr->spell_learned2 = 0L;
		creature_ptr->spell_worked1 = creature_ptr->spell_worked2 = 0L;
	}
	creature_ptr->spell_forgotten1 = creature_ptr->spell_forgotten2 = 0L;
	for (i = 0; i < (REALM_MAGIC_NUMBER * 2); i++) creature_ptr->spell_order[i] = 99;
	creature_ptr->learned_spells = 0;
	creature_ptr->add_spells = 0;
	creature_ptr->knowledge = 0;

	/* Clean the mutation count */
	creature_ptr->regenerate_mod = 100;

	/* Default pet command settings */
	creature_ptr->pet_follow_distance = PET_FOLLOW_DIST;
	creature_ptr->pet_extra_flags = (PF_TELEPORT | PF_ATTACK_SPELL | PF_TRAIT_S_SPELL);

	/* Wipe the recall depths */
	for (i = 0; i < max_dungeon_idx; i++) max_dlv[i] = 0;

	creature_ptr->visit = 1;

	/* Level one */
	creature_ptr->max_plv = creature_ptr->lev = 1;

	/* Initialize arena and rewards information -KMW- */
	arena_number = 0;

	for (i = 0; i < MAX_MANE; i++)
	{
		creature_ptr->mane_spell[i] = -1;
		creature_ptr->mane_dam[i] = 0;
	}
	creature_ptr->mane_num = 0;

	/* Reset karmas*/
	for (i = 0; i < MAX_KARMA; i++) creature_ptr->karmas[i] = 0;

	creature_ptr->recall_dungeon = DUNGEON_GALGALS;

	creature_ptr->camp_idx = INDEX_NONE;
	creature_ptr->race_idx1 = INDEX_NONE;
	creature_ptr->race_idx2 = INDEX_NONE;
	creature_ptr->mimic_race_idx = INDEX_NONE;
	creature_ptr->sex = SEX_NONE;
	creature_ptr->class_idx = INDEX_NONE;
	creature_ptr->chara_idx = INDEX_NONE;
	creature_ptr->patron_idx = INDEX_NONE;
	creature_ptr->realm1 = REALM_NONE;
	creature_ptr->realm2 = REALM_NONE;

	for(i = 0; i < RACE_FLAG_MAX; i++) creature_ptr->sub_race[i] = 0x0;

	creature_ptr->total_winner = FALSE;

}

/*
 * Determine the random quest uniques
 */
void determine_random_questor(quest_type *quest_ptr)
{
	PROB *prob_list;

	alloc_species_list(&prob_list);
	set_species_list_bias_random_questor(&prob_list, quest_ptr->level);
	quest_ptr->species_idx = species_rand(prob_list);
	free_species_list(&prob_list);
}


/*
 *  Initialize random quests and final quests
 */
void init_dungeon_quests(void)
{
	int number_of_quests = MAX_RANDOM_QUEST - MIN_RANDOM_QUEST + 1;
	int i;

	// Init the random quests
	process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_ASSIGN, MIN_RANDOM_QUEST);

	// Generate quests
	for (i = MIN_RANDOM_QUEST + number_of_quests - 1; i >= MIN_RANDOM_QUEST; i--)
	{
		quest_type      *quest_ptr = &quest[i];
		species_type    *quest_r_ptr;

		quest_ptr->status = QUEST_STATUS_TAKEN;
		determine_random_questor(quest_ptr);

		/* Mark uniques */
		quest_r_ptr = &species_info[quest_ptr->species_idx];
		//TODO quest_r_ptr->flags1 |= RF1_QUESTOR;

		quest_ptr->max_num = 1;
	}

	// Init the two main quests (AOY)
	process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_ASSIGN, QUEST_AOY);
	quest[QUEST_AOY].status = QUEST_STATUS_TAKEN;
}

/*
 * Reset turn
 */
static void init_turn(creature_type *creature_ptr)
{
	/* Undead start just after midnight */
	if(has_trait(creature_ptr, TRAIT_UNDEAD))
	{
		game_turn = (TURNS_PER_TICK * 3 * TOWN_DAWN) / 4 + 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * MAX_DAYS + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
	else
	{
		game_turn = 1;
		turn_limit = TURNS_PER_TICK * TOWN_DAWN * (MAX_DAYS - 1) + TURNS_PER_TICK * TOWN_DAWN * 3 / 4;
	}
}

static int wield_one(creature_type *creature_ptr, OBJECT_ID item, FLAGS_32 flags)
{
	object_type *object_ptr; 
	object_type *i_ptr; 
	object_type object_type_body; 
 
	SLOT_ID slot; 

	object_ptr = &creature_ptr->inventory[item]; 

	// temporary
	if(!flags) return FALSE;

	// Skip non-objects
	if(!is_valid_object(object_ptr)) return -1; 
 
	// Make sure we can wield it and that there's nothing else in that slot
	slot = WIELD_SLOT(object_ptr);

	if(slot == SLOT_ID_INVENTORY) return -1; 
	if(creature_ptr->inventory[slot].k_idx) return -1; 
 
	i_ptr = &object_type_body; 
	object_copy(i_ptr, object_ptr); 
 
	/* Modify quantity */ 
	i_ptr->number = 1; 
 
	increase_item(creature_ptr, item, -1, FALSE);
 
	/* Get the wield slot */ 
	object_ptr = &creature_ptr->inventory[slot]; 
 
	/* Wear the new stuff */ 
	object_copy(object_ptr, i_ptr); 
 
	set_inventory_weight(creature_ptr);
 
	/* Increment the equip counter by hand */ 
	creature_ptr->equip_cnt++;

	return slot; 
}


/* 
 * Try to wield everything wieldable in the inventory. 
 * Code taken from Angband 3.1.0 under Angband license
 */ 
static void wield_all(creature_type *creature_ptr, FLAGS_32 flags) 
{ 
	OBJECT_ID item;
	/* Scan through the slots backwards */ 
	for (item = INVEN_TOTAL - 1; item >= 0; item--) wield_one(creature_ptr, item, flags);
	return; 
} 


// Add an outfit object
void add_item_to_creature(creature_type *creature_ptr, object_type *object_ptr, FLAGS_32 flags)
{
	SLOT_ID slot;

	if(is_player(creature_ptr) && (flags & ADD_OUTFIT_EQUIP))
	{
		object_aware(object_ptr);
		object_known(object_ptr);
	}
	slot = inven_carry(creature_ptr, object_ptr);

	/* Auto-inscription */
	autopick_alter_item(creature_ptr, slot, FALSE);

	prepare_update(creature_ptr, CRU_COMBINE | CRU_REORDER);
	notice_stuff(creature_ptr);

	if(!(flags & ADD_OUTFIT_MULTIPLE_FENCING) && object_is_weapon(object_ptr)) return;

	/* Now try wielding everything */ 
	if(flags & ADD_OUTFIT_EQUIP) wield_all(creature_ptr, flags); 

}


void race_detail(int code)
{
	bool e;
	int base = 5;
	int i;
	char buf[100], temp[58*18];
	cptr t;

	for (i = 0; i < 22; i++)
		prt("                                                                       ", base + i, 24);
	if(code < 0) return;

	c_put_str(TERM_L_BLUE, race_info[code].title, base, 24);
	put_str(MES_BIRTH_MAIN_RACE_MODIFY, base, 24+strlen(race_info[code].title));
	put_str(MES_BIRTH_BASE_LEVEL_DR, base, 53);
	put_str(format("%-8s%-8s%-8s%-8s%-8s%-8s%-8s",
		stat_names[STAT_STR], stat_names[STAT_INT], stat_names[STAT_WIS],
		stat_names[STAT_DEX], stat_names[STAT_CON], stat_names[STAT_CHA], get_keyword("KW_EXP")), base+1, 24);

	sprintf(buf, "%+3d>%+3d %+3d>%+3d %+3d>%+3d %+3d>%+3d %+3d>%+3d %+3d>%+3d  %+4d%% ",
		race_info[code].r_adj[0] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_STR), race_info[code].r_adj[0],
		race_info[code].r_adj[1] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_INT), race_info[code].r_adj[1],
		race_info[code].r_adj[2] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_WIS), race_info[code].r_adj[2],
		race_info[code].r_adj[3] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_DEX), race_info[code].r_adj[3],
		race_info[code].r_adj[4] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_CON), race_info[code].r_adj[4],
		race_info[code].r_adj[5] - calc_unreached_race_level_penalty(race_info[code].lev - 1, STAT_CHA), race_info[code].r_adj[5],
		(race_info[code].r_exp - 100));
	c_put_str(TERM_L_BLUE, buf, base+2, 23);
	c_put_str(TERM_L_WHITE, ">", base+2, 26);
	c_put_str(TERM_L_WHITE, ">", base+2, 34);
	c_put_str(TERM_L_WHITE, ">", base+2, 42);
	c_put_str(TERM_L_WHITE, ">", base+2, 50);
	c_put_str(TERM_L_WHITE, ">", base+2, 58);
	c_put_str(TERM_L_WHITE, ">", base+2, 66);

	sprintf(buf, "%2d", race_info[code].lev);
	c_put_str(TERM_L_BLUE, buf, base, 64);

	if(race_info[code].dr >= 0)
	{
		sprintf(buf, "%2d", race_info[code].dr);
		c_put_str(TERM_L_BLUE, buf, base, 76);
	}
	else
		c_put_str(TERM_L_DARK, "--", base, 76);

	roff_to_buf(race_text + race_info[code].text, 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e)
			if(t[0] == 0) e = TRUE;

		if(e) prt("                                                                       ", base+4 + i, 24);
		else
		{
			prt(t, base+4 + i, 24);
			t += strlen(t) + 1;
		}
	}
}

void subrace_detail(int code)
{
	bool e;
	int base = 5;
	int i;
	char buf[100], temp[58*18];
	cptr t;

	if(code >= 0)
	{
		put_str("                                                  " , base, 24);
		put_str("                                                  " , base+1, 24);
		put_str("                                                  " , base+2, 24);
		c_put_str(TERM_L_BLUE, race_info[code].title, base, 24);
		put_str(MES_BIRTH_SUB_RACE_MODIFY, base, 24+strlen(race_info[code].title));
		put_str(format("%-8s%-8s%-8s%-8s%-8s%-8s%8s",
			stat_names[STAT_STR], stat_names[STAT_INT], stat_names[STAT_WIS],
			stat_names[STAT_DEX], stat_names[STAT_CON], stat_names[STAT_CHA], get_keyword("KW_EXP")), base+1, 24);

		sprintf(buf, "%+2d      %+2d      %+2d      %+2d      %+2d      %+2d      %+4d%% ",
			race_info[code].r_s_adj[STAT_STR],
			race_info[code].r_s_adj[STAT_INT],
			race_info[code].r_s_adj[STAT_WIS],
			race_info[code].r_s_adj[STAT_DEX],
			race_info[code].r_s_adj[STAT_CON],
			race_info[code].r_s_adj[STAT_CHA],
			(race_info[code].r_s_exp - 100));
		c_put_str(TERM_L_BLUE, buf, base+2, 24);

		roff_to_buf(race_text + race_info[code].text, 56, temp, sizeof(temp));
		t = temp;
		e = FALSE;
		for (i = 0; i < 18; i++)
		{
			if(!e) if(t[0] == 0) e = TRUE;

			if(e) prt("                                                                       ", base+4 + i, 24);
			else
			{
				prt(t, base+4 + i, 24);
				t += strlen(t) + 1;
			}
		}
	}
	else
	{
		put_str("                                                      " , base, 24);
		put_str("                                                      " , base+1, 24);
		put_str("                                                      " , base+2, 24);
		for (i = 0; i < 18; i++)
			prt("                                                                       ", base+4 + i, 24);
	}


}

void class_detail(int code)
{
	bool e;
	int base = 5;
	int i;
	char buf[100], temp[58*18];
	cptr t;
	put_str("                                                      " , base, 24);
	put_str("                                                      " , base+1, 24);
	put_str("                                                      " , base+2, 24);

	for (i = 0; i < 18; i++) prt("                                                                       ", base+4 + i, 24);
	if(code < 0) return;

	c_put_str(TERM_L_BLUE, class_info[code].title, base, 24);
	put_str(MES_BIRTH_MAIN_RACE_MODIFY, base, 24+strlen(class_info[code].title));
	put_str(format("%-8s%-8s%-8s%-8s%-8s%-8s%8s",
		stat_names[STAT_STR], stat_names[STAT_INT], stat_names[STAT_WIS],
		stat_names[STAT_DEX], stat_names[STAT_CON], stat_names[STAT_CHA], get_keyword("KW_EXP")), base+1, 24);

	sprintf(buf, "%+2d      %+2d      %+2d      %+2d      %+2d      %+2d     %+4d%% ",
		class_info[code].c_adj[STAT_STR],
		class_info[code].c_adj[STAT_INT],
		class_info[code].c_adj[STAT_WIS],
		class_info[code].c_adj[STAT_DEX],
		class_info[code].c_adj[STAT_CON],
		class_info[code].c_adj[STAT_CHA],
		class_info[code].c_exp);
	c_put_str(TERM_L_BLUE, buf, base+2, 24);

	roff_to_buf(class_text + class_info[code].text, 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e) if(t[0] == 0) e = TRUE;
		else
		{
			prt(t, base+4 + i, 24);
			t += strlen(t) + 1;
		}
	}
}


void chara_detail(int code)
{
	bool e;
	int base = 5;
	int i;
	char buf[100], temp[58*18];
	cptr t;

	for (i = 0; i < 22; i++) prt("                                                                       ", base + i, 24);
	if(code < 0) return;

	c_put_str(TERM_L_BLUE, chara_info[code].title, base, 24);
	put_str(MES_BIRTH_MAIN_RACE_MODIFY, base, 24+strlen(chara_info[code].title));
	put_str(format("%-8s%-8s%-8s%-8s%-8s%-8s%8s",
		stat_names[STAT_STR], stat_names[STAT_INT], stat_names[STAT_WIS],
		stat_names[STAT_DEX], stat_names[STAT_CON], stat_names[STAT_CHA], get_keyword("KW_EXP")), base+1, 24);

	sprintf(buf, "%+2d      %+2d      %+2d      %+2d      %+2d      %+2d     %+4d%% ",
		chara_info[code].a_adj[STAT_STR], chara_info[code].a_adj[STAT_INT],
		chara_info[code].a_adj[STAT_WIS], chara_info[code].a_adj[STAT_DEX],
		chara_info[code].a_adj[STAT_CON], chara_info[code].a_adj[STAT_CHA], 0);
	c_put_str(TERM_L_BLUE, buf, base+2, 24);

	roff_to_buf(chara_text + chara_info[code].text, 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e)
			if(t[0] == 0) e = TRUE;
		else
		{
			prt(t, base+4 + i, 24);
			t += strlen(t) + 1;
		}
	}
}


void realm_detail(int code)
{
	bool e;
	int base = 5;
	int i;
	char temp[58*18];
	cptr t;

	for (i = 0; i < 22; i++)
		prt("                                                                       ", base + i, 24);

	if(code < 0) return;

	roff_to_buf(realm_jouhou[technic2magic(code) - 1], 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e) if(t[0] == 0) e = TRUE;
		else
		{
			prt(t, base + i, 24);
			t += strlen(t) + 1;
		}
	}
}

void starting_point_detail(int code)
{
	bool e;
	int base = 5;
	int i;
	char temp[58*18];
	cptr t;

	for (i = 0; i < 22; i++) prt("                                                                       ", base + i, 24);

	if(code < 0) return;

	roff_to_buf(starting_point_jouhou[code], 56, temp, sizeof(temp));
	t = temp;
	e = FALSE;
	for (i = 0; i < 18; i++)
	{
		if(!e) if(t[0] == 0) e = TRUE;
		else
		{
			prt(t, base + i, 24);
			t += strlen(t) + 1;
		}
	}
}

// Creature first race
static int get_creature_first_race(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i, lev, weight[MAX_RACES], id[MAX_RACES];
	selection_table se[MAX_RACES + 3];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = race_detail;
	se_info.default_se = 0;
	se_info.y = 5;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 20;
	se_info.num = 0;

	if(species_ptr->race_idx1 != INDEX_VARIABLE)
	{
		creature_ptr->race_idx1 = species_ptr->race_idx1;
		return 0;
	}

	for (i = 0, se_info.num = 0; i < MAX_RACES; i++)
	{
		if(race_info[i].race_category < RACE_RARITY_UNSELECTABLE)
		{
			se[se_info.num].cap = race_name + race_info[i].name;
			se[se_info.num].code = i;
			se[se_info.num].key = '\0';
			se[se_info.num].d_color = TERM_L_DARK;
			se[se_info.num].l_color = TERM_WHITE;
			id[se_info.num] = i;
			weight[se_info.num] = 100000;
			if(race_info[i].race_category == RACE_RARITY_UNCOMMON) weight[se_info.num] /= 5;
			if(race_info[i].race_category == RACE_RARITY_RARE) weight[se_info.num] /= 40;
			if(race_info[i].race_category == RACE_RARITY_LEGENDARY) weight[se_info.num] /= 200;

			for(lev = 0; lev < CREATURE_MAX_LEVEL || creature_exp[lev] * 2 <= species_ptr->exp; lev++);
			if(lev < race_info[i].lev) weight[se_info.num] /= ((race_info[i].lev - lev) * (race_info[i].lev - lev));  

			if(species_ptr->dr < race_info[i].dr) weight[se_info.num] /= ((race_info[i].dr - species_ptr->dr) * (race_info[i].dr - species_ptr->dr));
			if(weight[se_info.num] <= 0) weight[se_info.num] = 1;
			se_info.num++;
		}
	}

	if(npc)
	{
		creature_ptr->race_idx1 = (RACE_ID)pick_rand(id, weight, se_info.num);
		return 0;
	}

	se[se_info.num].cap = KW_RANDOM;
	se[se_info.num].code = BIRTH_SELECT_RANDOM;
	se[se_info.num].key = '*';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	se[se_info.num].cap = KW_BACK_TO_START;
	se[se_info.num].code = BIRTH_SELECT_RETURN;
	se[se_info.num].key = 'S';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	se[se_info.num].cap = KW_QUIT_GAME;
	se[se_info.num].code = BIRTH_SELECT_QUIT;
	se[se_info.num].key = 'Q';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	put_str(MES_BIRTH_RACE, 0, 0);

	i = get_selection(&se_info, se);

	if(i >= 0)
	{
		creature_ptr->race_idx1 = (s16b)i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		creature_ptr->race_idx1 = (s16b)se[randint0(se_info.num - 3)].code;
		return 0;
	}
	else return i;
}

// Creature sub-race
static int get_creature_second_race(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i;
	selection_table se[MAX_RACES + 3];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = race_detail;
	se_info.default_se = 0;
	se_info.y = 5;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 20;
	se_info.num = 0;

	if(species_ptr->race_idx2 != INDEX_VARIABLE)
	{
		creature_ptr->race_idx2 = species_ptr->race_idx2;
		return 0;
	}

	se[se_info.num].cap = get_keyword("KW_PURE_BLOOD");
	se[se_info.num].code = creature_ptr->race_idx1;
	se[se_info.num].key = '\0';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	for (i = 0; i < MAX_RACES; i++)
	{
		if(race_crossing[creature_ptr->race_idx1][i] > 0 && creature_ptr->race_idx1 != i)
		{
			se[se_info.num].cap = race_info[i].title;
			se[se_info.num].code = i;
			se[se_info.num].key = '\0';
			se[se_info.num].d_color = TERM_L_DARK;
			se[se_info.num].l_color = TERM_WHITE;
			se_info.num++;
		}
	}

	se[se_info.num].cap = KW_RANDOM;
	se[se_info.num].code = BIRTH_SELECT_RANDOM;
	se[se_info.num].key = '*';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	se[se_info.num].cap = KW_BACK_TO_START;
	se[se_info.num].code = BIRTH_SELECT_RETURN;
	se[se_info.num].key = 'S';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	se[se_info.num].cap = KW_QUIT_GAME;
	se[se_info.num].code = BIRTH_SELECT_QUIT;
	se[se_info.num].key = 'Q';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	if(npc)
	{
		if(one_in_(RATE_OF_HALF_RACE))
		{
			creature_ptr->race_idx2 = (s16b)se[randint0(se_info.num-3)].code;
			return 0;
		}
		else
		{
			creature_ptr->race_idx2 = creature_ptr->race_idx1;
			return 0;
		}
	}

	put_str(MES_BIRTH_SECOND_RACE, 0, 0);
	i = get_selection(&se_info, se);

	if(i >= 0)
	{
		creature_ptr->race_idx2 = (s16b)i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		int t = randint0(se_info.num-3);
		creature_ptr->race_idx2 = (s16b)se[t].code;
		return 0;
	}
	else return i;
}

// Player SubRace(Eldar)
static bool get_creature_subrace_eldar(creature_type *creature_ptr, bool npc)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(npc) return TRUE;
	return FALSE;

/*
	int i, n = 0;
	selection_table se[3 + 3];

	strcpy(se[n].cap, race_info[RACE_TELERI_LINEAGE].title);
	se[n].code = RACE_TELERI_LINEAGE;
	se[n].key = '\0';
	se[n].d_color = TERM_L_DARK;
	se[n].l_color = TERM_WHITE;
	n++;

	strcpy(se[n].cap, race_info[RACE_NOLDOR_LINEAGE].title);
	se[n].code = RACE_NOLDOR_LINEAGE;
	se[n].key = '\0';
	se[n].d_color = TERM_L_DARK;
	se[n].l_color = TERM_WHITE;
	n++;

	strcpy(se[n].cap, race_info[RACE_VANYAR_LINEAGE].title);
	se[n].code = RACE_VANYAR_LINEAGE;
	se[n].key = '\0';
	se[n].d_color = TERM_L_DARK;
	se[n].l_color = TERM_WHITE;
	n++;

	strcpy(se[n].cap, KW_RANDOM);
	se[n].code = BIRTH_SELECT_RANDOM;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	strcpy(se[n].cap, KW_BACK_TO_START);
	se[n].code = BIRTH_SELECT_RETURN;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	strcpy(se[n].cap, KW_QUIT_GAME);
	se[n].code = BIRTH_SELECT_QUIT;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(!npc)
	{
		put_str(MES_BIRTH_ELDAR, 0, 0);
		i = get_selection(NULL, se, n, 5, 2, 18, 20, subrace_detail, 0);
	}
	else
	{
		set_subrace(creature_ptr, se[randint0(3)].code, TRUE);
		return 0;
	}

	if(i >= 0)
	{
		set_subrace(creature_ptr, i, TRUE);
		return 0;
	}
	if(i == BIRTH_SELECT_RANDOM)
	{
		set_subrace(creature_ptr, se[randint0(3)].code, TRUE);
		return 0;
	}
	else
	{
		return i;
	}

*/
	return 0;

}

// Creature SubRace(Dragon & Draconian)
static bool get_creature_subrace_dragonbone(creature_type *creature_ptr, bool npc)
{
	if(!is_valid_creature(creature_ptr)) return FALSE;
	if(npc) return TRUE;
	return FALSE;

	/*
	int i, n = 0;
	selection_table se[15];

	strcpy(se[n].cap, race_info[RACE_RED_LINEAGE].title);
	se[n].code = RACE_RED_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_WHITE_LINEAGE].title);
	se[n].code = RACE_WHITE_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_BLUE_LINEAGE].title);
	se[n].code = RACE_BLUE_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_BLACK_LINEAGE].title);
	se[n].code = RACE_BLACK_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_GREEN_LINEAGE].title);
	se[n].code = RACE_GREEN_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_CHROMATIC_LINEAGE].title);
	se[n].code = RACE_CHROMATIC_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_BRONZE_LINEAGE].title);
	se[n].code = RACE_BRONZE_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_GOLD_LINEAGE].title);
	se[n].code = RACE_GOLD_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_CRYSTAL_LINEAGE].title);
	se[n].code = RACE_CRYSTAL_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_LAW_LINEAGE].title);
	se[n].code = RACE_LAW_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_BALANCE_LINEAGE].title);
	se[n].code = RACE_BALANCE_LINEAGE;
	n++;
	strcpy(se[n].cap, race_info[RACE_CHAOS_LINEAGE].title);
	n++;
	se[n].code = RACE_CHAOS_LINEAGE;

	for(i = 0; i < 12; i++)
	{
		se[i].d_color = TERM_L_DARK;
		se[i].l_color = TERM_WHITE;
		se[i].key = '\0';
	}

	strcpy(se[n].cap, KW_RANDOM);
	se[n].code = BIRTH_SELECT_RANDOM;
	se[n].key = '*';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	strcpy(se[n].cap, KW_BACK_TO_START);
	se[n].code = BIRTH_SELECT_RETURN;
	se[n].key = 'S';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	strcpy(se[n].cap, KW_QUIT_GAME);
	se[n].code = BIRTH_SELECT_QUIT;
	se[n].key = 'Q';
	se[n].d_color = TERM_UMBER;
	se[n].l_color = TERM_L_UMBER;
	n++;

	if(!npc)
	{
		put_str(MES_BIRTH_DRAGONBONE, 0, 0);
		i = get_selection(NULL, se, n, 5, 2, 18, 20, subrace_detail, 0);
	}
	else
	{
		set_subrace(creature_ptr, se[randint0(12)].code, TRUE);
		return 0;
	}

	if(i >= 0)
	{
		set_subrace(creature_ptr, i, TRUE);
		return 0;
	}
	if(i == BIRTH_SELECT_RANDOM)
	{
		set_subrace(creature_ptr, se[randint0(12)].code, TRUE);
		return 0;
	}
	else
	{
		return i;
	}

*/
	return TRUE;

}

// Creature sex
static int get_creature_sex(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i, category_num;
	selection_table se[MAX_SEXES + 3];
	int id[MAX_SEXES + 1], weight1[MAX_SEXES + 1], weight2[MAX_SEXES];
	int list[MAX_SEXES] = {SEX_MALE, SEX_FEMALE, SEX_INTERSEX, SEX_NONE};
	int trait_list[MAX_SEXES] = {TRAIT_MALE, TRAIT_FEMALE, TRAIT_INTERSEX, TRAIT_NOSEX};
	int left_per = 100;
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = NULL;
	se_info.default_se = 0;
	se_info.y = 5;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 20;
	se_info.num = 0;

	if(species_ptr->sex != INDEX_VARIABLE)
	{
		creature_ptr->sex = species_ptr->sex;
		return 0;
	}

	for (i = 0, se_info.num = 0, category_num = 0; i < MAX_SEXES; i++)
	{
		se[se_info.num].cap = sex_info[list[i]].title;
		se[se_info.num].code = list[i];
		se[se_info.num].key = '\0';
		id[se_info.num] = list[i];
		if(race_info[creature_ptr->race_idx1].sex_flag & (0x01 << list[i]) || race_info[creature_ptr->race_idx2].sex_flag & (0x01 << list[i]))
		{
			se[se_info.num].d_color = TERM_L_DARK;
			se[se_info.num].l_color = TERM_WHITE;
			if(has_trait_species(species_ptr, trait_list[i]))
			{
				weight1[se_info.num] = species_ptr->flags.probability[trait_list[i]];
				left_per -= weight1[se_info.num];
			}
			else weight1[se_info.num] = 0;
			weight2[se_info.num] = 100;
		}
		else
		{
			se[se_info.num].d_color = TERM_RED;
			se[se_info.num].l_color = TERM_L_RED;
			weight1[se_info.num] = 0;
			weight2[se_info.num] = 0;
		}
		se_info.num++;
		category_num++;
	}

	if(npc)
	{
		id[se_info.num] = MAX_SEXES;
		weight1[se_info.num] = left_per > 0 ? left_per: 1;
		se_info.num++;
		creature_ptr->sex = (SEX_ID)pick_rand(id, weight1, se_info.num);
		if(creature_ptr->sex == MAX_SEXES) creature_ptr->sex = (s16b)pick_rand(id, weight2, se_info.num-1);
		return 0;
	}

	se[se_info.num].cap = KW_RANDOM;
	se[se_info.num].code = BIRTH_SELECT_RANDOM;
	se[se_info.num].key = '*';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	se[se_info.num].cap = KW_BACK_TO_START;
	se[se_info.num].code = BIRTH_SELECT_RETURN;
	se[se_info.num].key = 'S';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	se[se_info.num].cap = KW_QUIT_GAME;
	se[se_info.num].code = BIRTH_SELECT_QUIT;
	se[se_info.num].key = 'Q';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	put_str(MES_BIRTH_SEX, 0, 0);
	i = get_selection(&se_info, se);

	if(i >= 0)
	{
		creature_ptr->sex = (s16b)i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		creature_ptr->sex = (s16b)se[randint0(category_num)].code;
		return 0;
	}
	else return i;
}

// Player class
static int get_creature_class(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i, n, id[MAX_CLASS], weight[MAX_CLASS];
	selection_table ce[MAX_CLASS+3];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = class_detail;
	se_info.default_se = 0;
	se_info.y = 5;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 20;
	se_info.num = 0;

	if(species_ptr->class_idx != INDEX_VARIABLE)
	{
		creature_ptr->class_idx = species_ptr->class_idx;
		return 0;
	}

	clear_from(0);
	if(!npc) put_initial_status(creature_ptr);

	for (i = 0, n = 0; i < MAX_CLASS; i++)
	{
		if(class_info[i].selectable)
		{
			ce[se_info.num].cap = class_info[i].title;
			id[se_info.num] = i;
			if(class_info[i].rarity) weight[se_info.num] = 10000 / class_info[i].rarity;
			else weight[se_info.num] = 0;
			ce[se_info.num].code = i;
			ce[se_info.num].key = '\0';
			ce[se_info.num].d_color = TERM_L_DARK;
			ce[se_info.num].l_color = TERM_WHITE;
			if(SUITABLE_CLASS(creature_ptr, i))
			{
				ce[se_info.num].d_color = TERM_GREEN;
				ce[se_info.num].l_color = TERM_L_GREEN;
			}
			else
			{
				ce[se_info.num].d_color = TERM_L_DARK;
				ce[se_info.num].l_color = TERM_WHITE;
			}
			se_info.num++;
		}
	}

	if(npc)
	{
		if(n > 0) creature_ptr->class_idx = (CLASS_ID)pick_rand(id, weight, n);
		else creature_ptr->class_idx = CLASS_SOLDIER;
		return 0;
	}

	ce[se_info.num].cap = KW_RANDOM;
	ce[se_info.num].code = BIRTH_SELECT_RANDOM;
	ce[se_info.num].key = '*';
	ce[se_info.num].d_color = TERM_UMBER;
	ce[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	ce[se_info.num].cap = KW_BACK_TO_START;
	ce[se_info.num].code = BIRTH_SELECT_RETURN;
	ce[se_info.num].key = 'S';
	ce[se_info.num].d_color = TERM_UMBER;
	ce[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	ce[se_info.num].cap = KW_QUIT_GAME;
	ce[se_info.num].code = BIRTH_SELECT_QUIT;
	ce[se_info.num].key = 'Q';
	ce[se_info.num].d_color = TERM_UMBER;
	ce[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	put_str(MES_BIRTH_CLASS, 0, 0);

	if(!npc) put_initial_status(creature_ptr);
	i = get_selection(&se_info, ce);

	if(i >= 0)
	{
		creature_ptr->class_idx = (s16b)i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		creature_ptr->class_idx = (s16b)ce[randint0(se_info.num - 3)].code;
		return 0;
	}
	else return i;
}

// Player patron
static int get_creature_patron(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i, n = 0;
	selection_table pt[400+3];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = NULL;
	se_info.default_se = 0;
	se_info.y = 5;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 76;
	se_info.num = 0;

	if(species_ptr->patron_idx != INDEX_VARIABLE)
	{
		creature_ptr->patron_idx = species_ptr->patron_idx;
		return 0;
	}

	for (i = 0; i < max_species_idx; i++)
	{
		if(!has_trait_species(&species_info[i], TRAIT_UNIQUE)) continue;		
		if(species_info[i].dr < 5 || species_info[i].dr < calc_base_divine_rank(creature_ptr)) continue;
		
		if(IS_PURE_RACE(creature_ptr, RACE_MELNIBONE))
		{
			if(i != SPECIES_ARIOCH) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_AMBERITE))
		{
			if(i != SPECIES_UNICORN_ORD) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_CHAOSIAN))
		{
			if(i != SPECIES_SERPENT) continue;
		}

		if(IS_PURE_RACE(creature_ptr, RACE_ISTARI))
		{
			if(!has_trait_raw(&species_info[i].flags, TRAIT_AMAN)) continue;		
		}

		pt[se_info.num].cap = species_name + species_info[i].name;
		pt[se_info.num].code = i;
		pt[se_info.num].key = '\0';
		pt[se_info.num].d_color = TERM_L_DARK;
		pt[se_info.num].l_color = TERM_WHITE;
		se_info.num++; 
		
		if(n == 400) break;
	}

	pt[se_info.num].cap = KW_RANDOM;
	pt[se_info.num].code = BIRTH_SELECT_RANDOM;
	pt[se_info.num].key = '*';
	pt[se_info.num].d_color = TERM_UMBER;
	pt[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	pt[se_info.num].cap = KW_BACK_TO_START;
	pt[se_info.num].code = BIRTH_SELECT_RETURN;
	pt[se_info.num].key = 'S';
	pt[se_info.num].d_color = TERM_UMBER;
	pt[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	pt[se_info.num].cap = KW_QUIT_GAME;
	pt[se_info.num].code = BIRTH_SELECT_QUIT;
	pt[se_info.num].key = 'Q';
	pt[se_info.num].d_color = TERM_UMBER;
	pt[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	if(npc)
	{
		creature_ptr->patron_idx = (s16b)pt[randint0(se_info.num)].code;
		return 0;
	}

	put_str(MES_BIRTH_PATRON, 0, 0);
	if(!npc) put_initial_status(creature_ptr);
	if(se_info.num <= 0) return 0;
	i = get_selection(&se_info, pt);

	if(i >= 0)
	{
		creature_ptr->patron_idx = (SPECIES_ID)i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		creature_ptr->patron_idx = (s16b)pt[randint0(se_info.num - 3)].code;
		return 0;
	}
	else return i;

}

// Player Chara
static int get_creature_chara(creature_type *creature_ptr, species_type *species_ptr, bool npc)
{
	int i;
	selection_table ce[MAX_CHARA + 3];
	int id[MAX_CHARA];
	int weight[MAX_CHARA];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = chara_detail;
	se_info.default_se = 0;
	se_info.y = 5;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 20;
	se_info.num = 0;

	if(!npc)
	{
		clear_from(0);
		put_initial_status(creature_ptr);
	}

	if(creature_ptr->patron_idx == SPECIES_ILUVATAR)
	{
		creature_ptr->chara_idx = CHARA_MUNCHKIN;
		return 0;
	}

	if(species_ptr->chara_idx != INDEX_VARIABLE)
	{
		creature_ptr->chara_idx = species_ptr->chara_idx;
		return 0;
	}

	for (i = 0; i < MAX_CHARA; i++)
	{
		if((chara_info[i].sex & (0x01 << creature_ptr->sex)) && (!npc || chara_info[i].rarity < 100))
		{
			ce[se_info.num].cap = chara_info[i].title;
			ce[se_info.num].code = i;
			ce[se_info.num].key = '\0';
			ce[se_info.num].d_color = TERM_L_DARK;
			ce[se_info.num].l_color = TERM_WHITE;

			id[se_info.num] = i;
			weight[se_info.num] = (chara_info[i].rarity ? 10000 / chara_info[i].rarity : 10000);
			se_info.num++;
		}
	}

	if(npc)
	{
		creature_ptr->chara_idx = (CHARA_ID)pick_rand(id, weight, se_info.num);
		return 0;
	}

	ce[se_info.num].cap = KW_RANDOM;
	ce[se_info.num].code = BIRTH_SELECT_RANDOM;
	ce[se_info.num].key = '*';
	ce[se_info.num].d_color = TERM_UMBER;
	ce[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	ce[se_info.num].cap = KW_BACK_TO_START;
	ce[se_info.num].code = BIRTH_SELECT_RETURN;
	ce[se_info.num].key = 'S';
	ce[se_info.num].d_color = TERM_UMBER;
	ce[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	ce[se_info.num].cap = KW_QUIT_GAME;
	ce[se_info.num].code = BIRTH_SELECT_QUIT;
	ce[se_info.num].key = 'Q';
	ce[se_info.num].d_color = TERM_UMBER;
	ce[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	put_str(MES_BIRTH_PERSONALITY, 0, 0);

	if(!npc) put_initial_status(creature_ptr);
	i = get_selection(&se_info, ce);

	if(i >= 0)
	{
		creature_ptr->chara_idx = (s16b)i;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		creature_ptr->chara_idx = (CHARA_ID)pick_rand(id, weight, se_info.num - 3);
		return 0;
	}
	else return i;
}

// Player Starting Point
static int get_starting_point(creature_type *creature_ptr, bool npc)
{
	int i, j, n;
	selection_table se[STARTING_MAX + 3];
	selection_info se_info;

	se_info.mode = 0;
	se_info.caption = NULL;
	se_info.detail = starting_point_detail;
	se_info.default_se = 0;
	se_info.y = 5;
	se_info.x = 2;
	se_info.h = 18;
	se_info.w = 20;
	se_info.num = 0;

	clear_from(0);
	if(!npc) put_initial_status(creature_ptr);

	for (i = 0, n = 0; i < STARTING_MAX; i++)
	{
		if(starting_point[i].enable)
		{
			se[se_info.num].cap = starting_point[i].name;
			se[se_info.num].code = i;
			se[se_info.num].key = '\0';
			se[se_info.num].d_color = TERM_L_DARK;
			se[se_info.num].l_color = TERM_WHITE;
			se_info.num++;
		}
	}

	se[se_info.num].cap = KW_RANDOM;
	se[se_info.num].code = BIRTH_SELECT_RANDOM;
	se[se_info.num].key = '*';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	se[se_info.num].cap = KW_BACK_TO_START;
	se[se_info.num].code = BIRTH_SELECT_RETURN;
	se[se_info.num].key = 'S';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	se[se_info.num].cap = KW_QUIT_GAME;
	se[se_info.num].code = BIRTH_SELECT_QUIT;
	se[se_info.num].key = 'Q';
	se[se_info.num].d_color = TERM_UMBER;
	se[se_info.num].l_color = TERM_L_UMBER;
	se_info.num++;

	if(!npc)
	{
		put_str(MES_BIRTH_START_POINT, 0, 0);
		i = get_selection(&se_info, se);
	}
	else
	{
		j = se[randint0(n - 3)].code;
		creature_ptr->start_wy = starting_point[j].wy;
		creature_ptr->start_wx = starting_point[j].wx;
		return 0;
	}

	if(i >= 0)
	{
		creature_ptr->start_wy = starting_point[i].wy;
		creature_ptr->start_wx = starting_point[i].wx;
		return 0;
	}
	else if(i == BIRTH_SELECT_RANDOM)
	{
		j = se[randint0(se_info.num - 3)].code;
		creature_ptr->start_wy = starting_point[j].wy;
		creature_ptr->start_wx = starting_point[j].wx;
		return 0;
	}
	else return i;
}

static bool get_stat_limits(creature_type *creature_ptr)
{
	int i, j, m, cs, os;
	int mval[STAT_MAX];
	s16b cval[STAT_MAX];
	char c;
	char buf[80], cur[80];
	char inp[80];

	clear_from(0);

	/* Extra infomation */
#ifdef JP
	put_str("能力値を割り振ってください。", 10, 10);
	put_str("2/8で項目選択、4/6で値の増減、Enterで次へ", 11, 10);
	put_str("         基本値  種族 職業 性格     合計値  最大値", 13, 10);
#else
	put_str("Set stats.", 10, 10);
	put_str("2/8 for Select, 4/6 for Change value, Enter for Goto next", 11, 10);
	put_str("           Base   Rac  Cla  Per      Total  Maximum", 13, 10);
#endif

	/* Output the maximum stats */
	for (i = 0; i < STAT_MAX; i++)
	{
		/* Reset the "success" counter */
		stat_match[i] = 0;
		cval[i] = 3;

		/* Race/Class bonus */
		if(IS_PURE(creature_ptr))
			j = race_info[creature_ptr->race_idx1].r_adj[i];
		else
			j = race_info[creature_ptr->race_idx1].r_s_adj[i] + race_info[creature_ptr->race_idx2].r_s_adj[i];
		
		j += class_info[creature_ptr->class_idx].c_adj[i] + chara_info[creature_ptr->chara_idx].a_adj[i];

		/* Save the maximum */
		mval[i] = j;
		sprintf(cur, "%2d", j / 10);
		sprintf(inp, "%2d", j / 10);

		/* Prepare a prompt */
		//TODO
		sprintf(buf, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s  %6s",
			stat_names[i], cval[i], race_info[creature_ptr->race_idx1].r_adj[i], class_info[creature_ptr->class_idx].c_adj[i],
			chara_info[creature_ptr->chara_idx].a_adj[i], inp, cur);
		
		/* Dump the prompt */
		put_str(buf, 14 + i, 10);
	}
	
	/* Get a minimum stat */
	cs = 0;
	os = STAT_MAX;
	while (TRUE)
	{
		/* Move Cursol */
		if(cs != os)
		{
			
			if(os == STAT_MAX) c_put_str(TERM_WHITE, KW_ACCEPT, 21, 35);
			else if(os < STAT_MAX) c_put_str(TERM_WHITE, cur, 14 + os, 10);
			if(cs == STAT_MAX) c_put_str(TERM_YELLOW, KW_ACCEPT, 21, 35);
			else
			{
				/* Race/Class bonus */
				j = race_info[creature_ptr->race_idx1].r_adj[cs] + class_info[creature_ptr->class_idx].c_adj[cs] + chara_info[creature_ptr->chara_idx].a_adj[cs];

				/* Obtain the current stat */
				m = j;
				
				sprintf(inp, "%2d", m);
				
				/* Prepare a prompt */
				sprintf(cur, "%6s       %2d   %+3d  %+3d  %+3d  =  %6s",
					stat_names[cs], cval[cs], race_info[creature_ptr->race_idx1].r_adj[cs],
					class_info[creature_ptr->class_idx].c_adj[cs], chara_info[creature_ptr->chara_idx].a_adj[cs], inp);
				c_put_str(TERM_YELLOW, cur, 14 + cs, 10);
			}
			os = cs;
		}
		
		/* Prompt for the minimum stats */
		c = inkey();
		switch ( c ){
		case 'Q':
			birth_quit();
		case 'S':
			return FALSE;
		case ESCAPE:
			break;
		case ' ':
		case '\r':
		case '\n':
			if(cs == 6) break;
			cs++;
			c = '2';
			break;
		case '8':
		case 'k':
			if(cs > 0) cs--;
			break;
		case '2':
		case 'j':
			if(cs < STAT_MAX) cs++;
			break;
		case '4':
		case 'h':
			if(cs != STAT_MAX)
			{
				if(cval[cs] == 3)
				{
					cval[cs] = 17;
					os = 7;
				}
				else if(cval[cs] > 3)
				{
					cval[cs]--;
					os = 7;
				}
				else return FALSE;
			}
			break;
		case '6':
		case 'l':
			if(cs != 6)
			{
				if(cval[cs] == 17)
				{
					cval[cs] = 3;
					os = 7;
				}
				else if(cval[cs] < 17)
				{
					cval[cs]++;
					os = 7;
				}
				else return FALSE;
			}
			break;
		case 'm':
			if(cs != 6)
			{
				cval[cs] = 17;
				os = 7;
			}
			break;
		case 'n':
			if(cs != 6)
			{
				cval[cs] = 3;
				os = 7;
			}
			break;
		case '?':
			show_help(TEXT_FILES_BIRTH_AUTO_ROLLER);
			break;
		default:
			bell();
			break;
		}
		if(c == ESCAPE || ((c == ' ' || c == '\r' || c == '\n') && cs == 6))break;
	}

	/* Save the minimum stat */
	for (i = 0; i < STAT_MAX; i++) stat_limit[i] = cval[i];

	return TRUE;
}

#define HISTPREF_LIMIT 1024
static char *histpref_buf = NULL;

/*
 * Hook function for reading the histpref.prf file.
 */
void add_history_from_pref_line(cptr t)
{
	/* Do nothing if the buffer is not ready */
	if(!histpref_buf) return;

	my_strcat(histpref_buf, t, HISTPREF_LIMIT);
}


static bool do_cmd_histpref(creature_type *creature_ptr)
{
	char buf[80];
	errr err;
	int i, j, n;
	char *s, *t;
	char temp[HISTORY_ROW * HISTORY_COL];
	char histbuf[HISTPREF_LIMIT];
	if(!get_check(MES_BIRTH_PROFILE_LOAD_ASK)) return FALSE;

	/* Prepare the buffer */
	histbuf[0] = '\0';
	histpref_buf = histbuf;

	sprintf(buf, TEXT_FILES_HISTEDIT_N, player_base);
	err = process_histpref_file(buf);

	/* Process 'hist????.prf' if 'hist????-<name>.prf' doesn't exist */
	if(0 > err)
	{
		strcpy(buf, TEXT_FILES_HISTEDIT);
		err = process_histpref_file(buf);
	}

	if(err)
	{
		msg_print(MES_BIRTH_PROFILE_LOAD_FAILED);
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;
		return FALSE;
	}
	else if(!histpref_buf[0])
	{
		msg_print(MES_BIRTH_PROFILE_LOAD_NO_DATA);
		msg_print(NULL);

		/* Kill the buffer */
		histpref_buf = NULL;

		return FALSE;
	}

	/* Clear the previous history strings */
	for (i = 0; i < HISTORY_ROW; i++) creature_ptr->history[i][0] = '\0';

	/* Skip leading spaces */
	for (s = histpref_buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* Kill trailing spaces */
	while ((n > 0) && (s[n - 1] == ' ')) s[--n] = '\0';

	roff_to_buf(s, HISTORY_COL, temp, sizeof(temp));
	t = temp;
	for (i = 0; i < HISTORY_ROW; i++)
	{
		if(t[0] == 0) break;
		else
		{
			strcpy(creature_ptr->history[i], t);
			t += strlen(t) + 1;
		}
	}

	/* Fill the remaining spaces */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		for (j = 0; creature_ptr->history[i][j]; j++) /* loop */;

		for (; j < HISTORY_COL - 1; j++) creature_ptr->history[i][j] = ' ';
		creature_ptr->history[i][HISTORY_COL - 1] = '\0';
	}

	/* Kill the buffer */
	histpref_buf = NULL;

	return TRUE;
}

/*
 *  Character background edit-mode
 */
static void edit_history(creature_type *creature_ptr)
{
	char old_history[HISTORY_ROW][HISTORY_COL];
	int y = 0, x = 0;
	int i, j;

	/* Edit character background */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		sprintf(old_history[i], "%s", creature_ptr->history[i]);
	}
	/* Turn 0 to space */
	for (i = 0; i < HISTORY_ROW; i++)
	{
		for (j = 0; creature_ptr->history[i][j]; j++) /* loop */;

		for (; j < HISTORY_COL - 1; j++) creature_ptr->history[i][j] = ' ';
		creature_ptr->history[i][HISTORY_COL - 1] = '\0';
	}
	display_creature_status(1, creature_ptr);
	c_put_str(TERM_L_GREEN, get_keyword("KW_CREATURE_PROFILE_EDITING"), 2, 20);
	put_str(MES_INTERFACE_EDIT_HISTORY, 23, 10);

	while (TRUE)
	{
		int skey;
		char c;

		for (i = 0; i < HISTORY_ROW; i++) put_str(creature_ptr->history[i], i + 4, 3);
		if(is_mbyte2(creature_ptr->history[y], x)) c_put_str(TERM_L_BLUE, format("%c%c", creature_ptr->history[y][x],creature_ptr->history[y][x+1]), y + 4, x + 3);
		else c_put_str(TERM_L_BLUE, format("%c", creature_ptr->history[y][x]), y + 4, x + 3);

		Term_gotoxy(x + 3, y + 4); /* Place cursor just after cost of current stat */
		skey = inkey_special(TRUE); /* Get special key code */
		if(!(skey & SKEY_MASK)) c = (char)skey; /* Get a character code */
		else c = 0;

		if(skey == SKEY_UP || c == KTRL('p'))
		{
			y--;
			if(y < 0) y = HISTORY_ROW - 1;
			if((x > 0) && (is_mbyte2(creature_ptr->history[y], x-1))) x--;
		}
		else if(skey == SKEY_DOWN || c == KTRL('n'))
		{
			y++;
			if(y >= HISTORY_ROW) y = 0;
			if((x > 0) && (is_mbyte2(creature_ptr->history[y], x-1))) x--;
		}
		else if(skey == SKEY_RIGHT || c == KTRL('f'))
		{
			if(is_mbyte2(creature_ptr->history[y], x)) x++;
			x++;
			if(x > HISTORY_COL - 2)
			{
				x = 0;
				if(y < HISTORY_ROW - 1) y++;
			}
		}
		else if(skey == SKEY_LEFT || c == KTRL('b'))
		{
			x--;
			if(x < 0)
			{
				if(y)
				{
					y--;
					x = HISTORY_ROW - 2;
				}
				else x = 0;
			}

#ifdef JP
			if((x > 0) && (is_mbyte2(creature_ptr->history[y], x-1))) x--;
#endif
		}
		else if(c == '\r' || c == '\n')
		{
			Term_erase(0, 11, 255);
			Term_erase(0, 17, 255);
			put_str(get_keyword("KW_CREATURE_PROFILE_EDITED"), 2, 20);
			break;
		}
		else if(c == ESCAPE)
		{
			clear_from(2);
			put_str(format("(%s)", get_keyword("KW_CREATURE_PROFILE")), 2, 25);

			for (i = 0; i < HISTORY_ROW; i++)
			{
				sprintf(creature_ptr->history[i], "%s", old_history[i]);
				put_str(creature_ptr->history[i], i + 4, 3);
			}
			break;
		}
		else if(c == KTRL('A'))
		{
			if(do_cmd_histpref(creature_ptr))
			{
#ifdef JP
				if((x > 0) && (is_mbyte2(creature_ptr->history[y], x - 1))) x--;
#endif
			}
		}
		else if(c == '\010')
		{
			x--;
			if(x < 0)
			{
				if(y)
				{
					y--;
					x = HISTORY_ROW - 2;
				}
				else x = 0;
			}

			creature_ptr->history[y][x] = ' ';
#ifdef JP
			if((x > 0) && (is_mbyte2(creature_ptr->history[y], x - 1)))
			{
				x--;
				creature_ptr->history[y][x] = ' ';
			}
#endif
		}
#ifdef JP
		else if(is_mbyte(c) || isprint(c))
#else
		else if(isprint(c)) /* BUGFIX */
#endif
		{
#ifdef JP
			if(is_mbyte2(creature_ptr->history[y], x)) creature_ptr->history[y][x+1] = ' ';
			if(is_mbyte(c))
			{
				if(x > HISTORY_COL - 3)
				{
					x = 0;
					y++;
					if(y > HISTORY_ROW - 1) y = 0;
				}

				if(is_mbyte2(creature_ptr->history[y], x+1)) creature_ptr->history[y][x+2] = ' ';

				creature_ptr->history[y][x++] = c;

				c = inkey();
			}
#endif
			creature_ptr->history[y][x++] = c;
			if(x > HISTORY_COL - 2)
			{
				x = 0;
				y++;
				if(y >= HISTORY_ROW) y = 0;
			}
		}
	} /* while (TRUE) */

}


/*
 * Helper function for 'generate_creature'
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.  And it cuts down on player stupidity.
 */
static bool generate_creature_aux(creature_type *creature_ptr, SPECIES_ID species_idx, FLAGS_32 flags)
{
	int i;
	int mode = 0;

	bool prev = FALSE;
	bool player_generate = flags & GC_PLAYER;
	bool auto_generate = flags & GC_AUTO;

	char c;
	char b1 = '[';
	char b2 = ']';

	species_type *species_ptr = &species_info[species_idx];

	// Wipe the player_generate
	creature_wipe(creature_ptr);

	creature_ptr->creature_idx = ++creature_idx_latest;
	creature_ptr->species_idx = species_idx;
	creature_ptr->ap_species_idx = species_idx;
	creature_ptr->lev = 1;
	creature_ptr->exp = creature_ptr->max_exp = species_ptr->exp;
	creature_ptr->dr = species_ptr->dr;
	creature_ptr->alert_range = species_ptr->alert_range;
	creature_ptr->action_turn = 0;

	if(flags & GC_PLAYER) creature_ptr->player = TRUE;

	strcpy(creature_ptr->name, species_name + species_ptr->name);

	// Authority is statics
	for(i = 0; i < AUTHORITY_FLAG_MAX; i++) creature_ptr->authority[i] = species_ptr->authority[i];

	// Race Select
	if(species_ptr->race_idx1 == INDEX_VARIABLE)
	{
		if(!auto_generate)
		{
			Term_clear();
			clear_from(0);
			put_initial_status(creature_ptr);
		}
		i = get_creature_first_race(creature_ptr, species_ptr, auto_generate);
		if(i == BIRTH_SELECT_RETURN) return FALSE;
		if(i == BIRTH_SELECT_QUIT) birth_quit();
	}
	else creature_ptr->race_idx1 = species_ptr->race_idx1;

	if(species_ptr->race_idx2 == INDEX_VARIABLE)
	{
		if(!auto_generate) put_initial_status(creature_ptr);
		i = get_creature_second_race(creature_ptr, species_ptr, auto_generate);
		if(i == BIRTH_SELECT_RETURN) return FALSE;
		if(i == BIRTH_SELECT_QUIT) birth_quit();
	}
	else creature_ptr->race_idx2 = species_ptr->race_idx2;

	// race_idx swap
	if(creature_ptr->race_idx1 > creature_ptr->race_idx2)
	{
		creature_ptr->race_idx1 ^= creature_ptr->race_idx2;
		creature_ptr->race_idx2 ^= creature_ptr->race_idx1;
		creature_ptr->race_idx1 ^= creature_ptr->race_idx2;
	}

	// TODO Race Trait

	// Give beastman a mutation at character birth
	if(has_trait(creature_ptr, TRAIT_KALEIDOSCOPIC_RACE)) creature_ptr->hack_mutation = TRUE;
	else creature_ptr->hack_mutation = FALSE;

	// Sex Select
	if(species_ptr->sex == INDEX_VARIABLE)
	{
		if(!auto_generate)
		{
			clear_from(0);
			put_initial_status(creature_ptr);
		}
		i = get_creature_sex(creature_ptr, species_ptr, auto_generate);
		if(i == BIRTH_SELECT_RETURN) return FALSE;
		if(i == BIRTH_SELECT_QUIT) birth_quit();
	}
	else creature_ptr->sex = species_ptr->sex;

	// Class Select
	i = get_creature_class(creature_ptr, species_ptr, auto_generate);
	if(i == BIRTH_SELECT_RETURN) return FALSE;
	if(i == BIRTH_SELECT_QUIT) birth_quit();

	// Realm Select
	i = get_creature_realms(creature_ptr, species_ptr, auto_generate);
	if(i == BIRTH_SELECT_RETURN) return FALSE;
	if(i == BIRTH_SELECT_QUIT) birth_quit();

	// Patron Select
	i = get_creature_patron(creature_ptr, species_ptr, auto_generate);
	if(i == BIRTH_SELECT_RETURN) return FALSE;
	if(i == BIRTH_SELECT_QUIT) birth_quit();

	// Character Select
	i = get_creature_chara(creature_ptr, species_ptr, auto_generate);
	if(i == BIRTH_SELECT_RETURN) return FALSE;
	if(i == BIRTH_SELECT_QUIT) birth_quit();

	// Starting Point
	if(player_generate)
	{
		i = get_starting_point(creature_ptr, auto_generate);
		if(i == BIRTH_SELECT_RETURN) return FALSE;
		if(i == BIRTH_SELECT_QUIT) birth_quit();
	}

	creature_ptr->d_attr = species_ptr->d_attr;
	creature_ptr->d_char = species_ptr->d_char;
	creature_ptr->x_char = species_ptr->x_char;
	creature_ptr->x_attr = species_ptr->x_attr;

	if(player_generate)
	{
		clear_from(0);
		init_turn(creature_ptr); /* Reset turn; before auto-roll and after choosing race */
	}

	/*** Generate ***/
	while (TRUE)
	{
		for(i = 0; i < STAT_MAX; i++) creature_ptr->stat_max[i] = creature_ptr->stat_cur[i] = species_ptr->stat_max[i];
		set_age(creature_ptr);                  // Roll for age
		set_exp(creature_ptr, species_ptr);                  // Roll for exp
		set_height_weight(creature_ptr);        // Roll for height and weight
		set_underlings(creature_ptr, species_ptr);
		set_history(creature_ptr);              // Roll for social class

		flush();

		mode = 0; /*** Display ***/

		set_extra(creature_ptr, TRUE);
		set_creature_bonuses(creature_ptr, FALSE);
		set_experience(creature_ptr);
		deal_item(creature_ptr);
		set_money(creature_ptr);

		c = '\0';
		while (TRUE)
		{
			set_experience(creature_ptr);
			set_creature_bonuses(creature_ptr, FALSE);

			// Calculate the bonuses and hitpoints
			prepare_update(creature_ptr, CRU_BONUS | CRU_HP | CRU_MANA);
			update_creature(creature_ptr, FALSE);

			/* And start out fully healthy */
			if(creature_ptr->species_idx == SPECIES_WOUNDED_BEAR) set_creature_hp_percent(creature_ptr, 50);
			else set_creature_hp_percent(creature_ptr, 100);
			set_creature_sp_percent(creature_ptr, 100);

			// Sexy gal gets bonus to maximum weapon skill of whip
			// TODO if(creature_ptr->chara_idx == CHARA_SEXY) skill_info[player_generate_ptr->class_idx].w_max[TV_HAFTED - TV_WEAPON_BEGIN][SV_WHIP] = WEAPON_EXP_MASTER;

			if(auto_generate) break;
			display_creature_status(mode, creature_ptr);	// Display the player_generate

			// Prepare a prompt (must squeeze everything in)
			Term_gotoxy(2, 23);
			Term_addch(TERM_WHITE, b1);
			if(mode) Term_addstr(-1, TERM_WHITE, MES_INTERFACE_CREATURE_DUMP1);
			Term_addstr(-1, TERM_WHITE, MES_INTERFACE_CREATURE_DUMP2);
			Term_addch(TERM_WHITE, b2);

			c = inkey();	// Prompt and get a command

			if(c == 'Q') birth_quit();
			if(c == 'S') return FALSE;						// Start over
			if(c == '\r' || c == '\n' || c == ESCAPE) break;	// Escape accepts the roll
			if((c == 'H') || (c == 'h'))	// Toggle the display
			{
				((mode >= DISPLAY_CR_STATUS_MAX) ? mode = DISPLAY_CR_STATUS_STANDARD : mode++);
				continue;
			}
			if(c == '?')	// Help
			{
				show_help(TEXT_FILES_BIRTH_AUTO_ROLLER);
				continue;
			}

			bell();
		}

		// Are we done?
		if(auto_generate || c == '\r' || c == '\n' || c == ESCAPE) break;

		// Save this for the "previous" character
		if(player_generate)
		{
			player_prev = *creature_ptr;
			quick_ok = FALSE;
		}

		// Note that a previous roll exists
		prev = TRUE;
	}

	if(!auto_generate)
	{
		get_name(creature_ptr); // Get a name, recolor it, prepare savefile
		set_creature_name(creating_savefile, creature_ptr); // Process the player_generate name
		edit_history(creature_ptr); // Edit character background
	}

	get_max_stats(creature_ptr);
	if(auto_generate) return TRUE;
	prt(MES_INTERFACE_CREATURE_DUMP3, 23, 10);
	c = inkey();	// Get a key
	if(c == 'Q') birth_quit();
	if(c == 'S') return FALSE;	// Start over
	return TRUE;					// Accept
}

/*
 * Ask whether the player use Quick Start or not.
 */
bool ask_quick_start(creature_type *creature_ptr)
{

	/* Doesn't have previous data */
	if(!quick_ok) return FALSE;


	Term_clear();

	while(TRUE)
	{
		char c;
		put_str(MES_QUICKSTART_DESC, 11, 5);
		put_str(MES_QUICKSTART_ASK, 14, 10);
		c = inkey();

		if(c == 'Q') quit(NULL);
		else if(c == 'S') return FALSE;
		else if(c == '?')
			show_help(TEXT_FILES_BIRTH_AUTO_ROLLER);
		else if((c == 'y') || (c == 'Y'))
			break;
		else
			return FALSE;
	}

	*creature_ptr = player_prev;
	init_dungeon_quests();
	init_turn(creature_ptr);

	creature_ptr->wy = creature_ptr->start_wy;
	creature_ptr->wx = creature_ptr->start_wx;

	/* Calc hitdice, but don't roll */
	set_extra(creature_ptr, FALSE);

	// Update the bonuses and hitpoints
	prepare_update(creature_ptr, CRU_BONUS | CRU_HP);
	update_creature(creature_ptr, TRUE);

	creature_ptr->chp = creature_ptr->mhp;
	creature_ptr->csp = creature_ptr->msp;

	/* Process the player name */
	set_creature_name(FALSE, creature_ptr);

	return TRUE;
}

/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
creature_type* generate_creature(cave_type *c_ptr, SPECIES_ID species_idx, FLAGS_32 flags)
{
	char buf[80];
	CREATURE_ID id;
	creature_type *creature_ptr;

	// Make a new creature
	id = creature_pop();
	if(!id) return NULL;

	// Get a new creature record
	creature_ptr = &creature_list[id];
	if(c_ptr) c_ptr->creature_idx = id;

	// Create a new character
	while (!generate_creature_aux(creature_ptr, species_idx, flags));

	if(flags & GC_PLAYER)
	{
		message_add("====================");

		write_diary(DIARY_GAMESTART, 1, DIARY_CREATE_PLAYER);
		write_diary(DIARY_HIGAWARI, 0, NULL);
		sprintf(buf, DIARY_SELECTSEX(sex_info[creature_ptr->sex].title));
		write_diary(DIARY_BUNSHOU, 1, buf);

		if(IS_PURE(creature_ptr))
			sprintf(buf, DIARY_SELECT_PURERACE(race_info[creature_ptr->race_idx1].title));
		else
			sprintf(buf, DIARY_SELECT_HALFRACE(race_info[creature_ptr->race_idx1].title, race_info[creature_ptr->race_idx2].title));
		write_diary(DIARY_BUNSHOU, 1, buf);

		sprintf(buf, DIARY_SELECT_CLASS(class_info[creature_ptr->class_idx].title));
		write_diary(DIARY_BUNSHOU, 1, buf);

		if(creature_ptr->realm1)
		{
			sprintf(buf, DIARY_SELECT_REALM(realm_names[creature_ptr->realm1]));
			write_diary(DIARY_BUNSHOU, 1, buf);
		}

		sprintf(buf, DIARY_SELECT_CHALA(chara_info[creature_ptr->chara_idx].title));
		write_diary(DIARY_BUNSHOU, 1, buf);

	}

	return creature_ptr;
}


