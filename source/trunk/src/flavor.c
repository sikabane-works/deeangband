/* Purpose: Object flavor code */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "files.h"

/*
 * Certain items, if aware, are known instantly
 * This function is used only by "flavor_init()"
 */
static bool object_easy_know(int i)
{
	object_kind *object_kind_ptr = &object_kind_info[i];

	/* Analyze the "tval" */
	switch (object_kind_ptr->tval)
	{
		/* Spellbooks */
		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		case TV_CRAFT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_CRUSADE_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		case TV_HEX_BOOK:
		{
			return TRUE;
		}

		/* Simple items */
		case TV_FLASK:
		case TV_JUNK:
		case TV_BOTTLE:
		case TV_SKELETON:
		case TV_SPIKE:
		case TV_WHISTLE:
		{
			return TRUE;
		}

		/* All Food, Potions, Scrolls, Rods */
		case TV_FOOD:
		case TV_POTION:
		case TV_SCROLL:
		case TV_ROD:
		{
			return TRUE;
		}
	}

	return FALSE;
}

// Create a name from random parts.
void get_table_name_aux(char *out_string)
{
#ifdef JP
	char Syllable[80];
	get_rnd_line("aname_j.txt", 1, Syllable);
	strcpy(out_string, Syllable);
	get_rnd_line("aname_j.txt", 2, Syllable);
	strcat(out_string, Syllable);
#else
#define MAX_SYLLABLES 164       /* Used with scrolls (see below) */

	static cptr syllables[MAX_SYLLABLES] = {
		"a", "ab", "ag", "aks", "ala", "an", "ankh", "app",
		"arg", "arze", "ash", "aus", "ban", "bar", "bat", "bek",
		"bie", "bin", "bit", "bjor", "blu", "bot", "bu",
		"byt", "comp", "con", "cos", "cre", "dalf", "dan",
		"den", "der", "doe", "dok", "eep", "el", "eng", "er", "ere", "erk",
		"esh", "evs", "fa", "fid", "flit", "for", "fri", "fu", "gan",
		"gar", "glen", "gop", "gre", "ha", "he", "hyd", "i",
		"ing", "ion", "ip", "ish", "it", "ite", "iv", "jo",
		"kho", "kli", "klis", "la", "lech", "man", "mar",
		"me", "mi", "mic", "mik", "mon", "mung", "mur", "nag", "nej",
		"nelg", "nep", "ner", "nes", "nis", "nih", "nin", "o",
		"od", "ood", "org", "orn", "ox", "oxy", "pay", "pet",
		"ple", "plu", "po", "pot", "prok", "re", "rea", "rhov",
		"ri", "ro", "rog", "rok", "rol", "sa", "san", "sat",
		"see", "sef", "seh", "shu", "ski", "sna", "sne", "snik",
		"sno", "so", "sol", "sri", "sta", "sun", "ta", "tab",
		"tem", "ther", "ti", "tox", "trol", "tue", "turs", "u",
		"ulk", "um", "un", "uni", "ur", "val", "viv", "vly",
		"vom", "wah", "wed", "werg", "wex", "whon", "wun", "x",
		"yerg", "yp", "zun", "tri", "blaa", "jah", "bul", "on",
		"foo", "ju", "xuxu"
	};

	int testcounter = randint1(3) + 1;

	strcpy(out_string, "");

	if(randint1(3) == 2)
	{
		while (testcounter--)
			strcat(out_string, syllables[randint0(MAX_SYLLABLES)]);
	}
	else
	{
		char Syllable[80];
		testcounter = randint1(2) + 1;
		while (testcounter--)
		{
			(void)get_rnd_line(TEXT_FILES_ELVISH, 0, Syllable);
			strcat(out_string, Syllable);
		}
	}

	out_string[0] = toupper(out_string[1]);

	out_string[16] = '\0';
#endif
}

// Create a name from random parts with quotes.
void get_table_name(char *out_string)
{
	char buff[80];
	get_table_name_aux(buff);
	sprintf(out_string, BRACKET_ARTIFACT, buff);
}

// Make random Sindarin name
void get_table_sindarin_aux(char *out_string)
{
	char Syllable[80];
#ifdef JP
	char tmp[80];
#endif

	get_rnd_line("sname.txt", 1, Syllable);
#ifdef JP
	strcpy(tmp, Syllable);
#else
	strcpy(out_string, Syllable);
#endif

	get_rnd_line("sname.txt", 2, Syllable);
#ifdef JP
	strcat(tmp, Syllable);
	sindarin_to_kana(out_string, tmp);
#else
	strcat(out_string, Syllable);
#endif
}


/*
 * Make random Sindarin name with quotes
 */
void get_table_sindarin(char *out_string)
{
	char buff[80];
	get_table_sindarin_aux(buff);

#ifdef JP
	sprintf(out_string, "『%s』", buff);
#else
	sprintf(out_string, "'%s'", buff);
#endif
}


/*
 * Shuffle flavor indices of a group of objects with given tval
 */
static void shuffle_flavors(byte tval)
{
	OBJECT_KIND_ID *k_idx_list;
	OBJECT_KIND_ID k_idx_list_num = 0;
	int i;

	/* Allocate an array for a list of k_idx */
	C_MAKE(k_idx_list, max_object_kind_idx, OBJECT_KIND_ID);

	/* Search objects with given tval for shuffle */
	for (i = 0; i < max_object_kind_idx; i++)
	{
		object_kind *object_kind_ptr = &object_kind_info[i];

		/* Skip non-Rings */
		if(object_kind_ptr->tval != tval) continue;


		if(!object_kind_ptr->flavor) continue;

		/* Skip objects with a fixed flavor name */
		if(have_flag(object_kind_ptr->flags, TRAIT_FIXED_FLAVOR)) continue;

		/* Remember k_idx */
		k_idx_list[k_idx_list_num] = i;

		/* Increase number of remembered indices */
		k_idx_list_num++;
	}

	/* Shuffle flavors */
	for (i = 0; i < k_idx_list_num; i++)
	{
		object_kind *k1_ptr = &object_kind_info[k_idx_list[i]];
		object_kind *k2_ptr = &object_kind_info[k_idx_list[randint0(k_idx_list_num)]];

		/* Swap flavors of this pair */
		s16b tmp = k1_ptr->flavor;
		k1_ptr->flavor = k2_ptr->flavor;
		k2_ptr->flavor = tmp;
	}

	/* Free an array for a list of k_idx */
	C_KILL(k_idx_list, max_object_kind_idx, OBJECT_KIND_ID);
}

/*
 * Prepare the "variable" part of the "object_kind_info" array.
 *
 * The "color"/"metal"/"type" of an item is its "flavor".
 * For the most part, flavors are assigned randomly each game.
 *
 * Initialize descriptions for the "colored" objects, including:
 * Rings, Amulets, Staffs, Wands, Rods, Food, Potions, Scrolls.
 *
 * The first 4 entries for potions are fixed (Water, Apple Juice,
 * Slime Mold Juice, Unused Potion).
 *
 * Scroll titles are always between 6 and 14 letters long.  This is
 * ensured because every title is composed of whole words, where every
 * word is from 1 to 8 letters long (one or two syllables of 1 to 4
 * letters each), and that no scroll is finished until it attempts to
 * grow beyond 15 letters.  The first time this can happen is when the
 * current title has 6 letters and the new word has 8 letters, which
 * would result in a 6 letter scroll title.
 *
 * Duplicate titles are avoided by requiring that no two scrolls share
 * the same first four letters (not the most efficient method, and not
 * the least efficient method, but it will always work).
 *
 * Hack -- make sure everything stays the same for each saved game
 * This is accomplished by the use of a saved "random seed", as in
 * "town_gen()".  Since no other functions are called while the special
 * seed is in effect, so this function is pretty "safe".
 *
 * Note that the "hacked seed" may provide an RNG with alternating parity!
 */
void flavor_init(void)
{
	int i;

	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant flavors */
	Rand_value = seed_flavor;


	/* Initialize flavor index of each object by itself */
	for (i = 0; i < max_object_kind_idx; i++)
	{
		object_kind *object_kind_ptr = &object_kind_info[i];

		/* Skip objects without flavor name */
		if(!object_kind_ptr->flavor_bane) continue;

		/*
		 * Initialize flavor index to itself
		 *  -> Shuffle it later
		 */
		object_kind_ptr->flavor = (s16b)i;
	}

	/* Shuffle Rings */
	shuffle_flavors(TV_RING);

	/* Shuffle Amulets */
	shuffle_flavors(TV_AMULET);

	/* Shuffle Staves */
	shuffle_flavors(TV_STAFF);

	/* Shuffle Wands */
	shuffle_flavors(TV_WAND);

	/* Shuffle Rods */
	shuffle_flavors(TV_ROD);

	/* Shuffle Mushrooms */
	shuffle_flavors(TV_FOOD);

	/* Shuffle Potions */
	shuffle_flavors(TV_POTION);

	/* Shuffle Scrolls */
	shuffle_flavors(TV_SCROLL);


	/* Hack -- Use the "complex" RNG */
	Rand_quick = FALSE;

	/* Analyze every object */
	for (i = 1; i < max_object_kind_idx; i++)
	{
		object_kind *object_kind_ptr = &object_kind_info[i];

		/* Skip "empty" objects */
		if(!object_kind_ptr->name) continue;

		/* No flavor yields aware */
		if(!object_kind_ptr->flavor) object_kind_ptr->aware = TRUE;

		/* Check for "easily known" */
		object_kind_ptr->easy_know = object_easy_know(i);
	}
}


/*
 * Print a char "c" into a string "t", as if by sprintf(t, "%c", c),
 * and return a pointer to the terminator (t + 1).
 */
static char *object_desc_chr(char *t, char c)
{
	/* Copy the char */
	*t++ = c;

	*t = '\0';

	return (t);
}


/*
 * Print a string "s" into a string "t", as if by strcpy(t, s),
 * and return a pointer to the terminator.
 */
static char *object_desc_str(char *t, cptr s)
{
	/* Copy the string */
	while (*s) *t++ = *s++;

	*t = '\0';

	return (t);
}



/*
 * Print an unsigned number "n" into a string "t", as if by
 * sprintf(t, "%u", n), and return a pointer to the terminator.
 */
static char *object_desc_num(char *t, uint n)
{
	uint p;

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + (char)(n / p);

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	*t = '\0';

	return (t);
}




#ifdef JP
/*
 * 日本語の個数表示ルーチン
 *（cmd1.c で流用するために object_desc_japanese から移動した。）
 */

char *object_desc_kosuu(char *t, object_type *object_ptr)
{
    t = object_desc_num(t, object_ptr->number);

    switch (object_ptr->tval)
    {
      case TV_BOLT:
      case TV_ARROW:
      case TV_POLEARM:
      case TV_STAFF:
      case TV_WAND:
      case TV_ROD:
      case TV_DIGGING:
      {
	  t = object_desc_str(t, "本");
	  break;
      }
      case TV_SCROLL:
      {
	  t = object_desc_str(t, "巻");
	  break;
      }
      case TV_POTION:
      {
	  t = object_desc_str(t, "服");
	  break;
      }
      case  TV_LIFE_BOOK:
      case  TV_SORCERY_BOOK:
      case  TV_NATURE_BOOK:
      case  TV_CHAOS_BOOK:
      case  TV_DEATH_BOOK:
      case  TV_TRUMP_BOOK:
      case  TV_ARCANE_BOOK:
      case  TV_CRAFT_BOOK:
      case  TV_DAEMON_BOOK:
      case  TV_CRUSADE_BOOK:
      case  TV_MUSIC_BOOK:
      case  TV_HISSATSU_BOOK:
	  case TV_HEX_BOOK:
      {
	  t = object_desc_str(t, "冊");
	  break;
      }
      case TV_SOFT_ARMOR:
      case TV_HARD_ARMOR:
      case TV_DRAG_ARMOR:
      case TV_CLOAK:
      {
	  t = object_desc_str(t, "着");
	  break;
      }
      case TV_SWORD:
      case TV_HAFTED:
      case TV_BOW:
      {
	  t = object_desc_str(t, "振");
	  break;
      }
      case TV_BOOTS:
      {
	  t = object_desc_str(t, "足");
	  break;
      }
      case TV_CARD:
      {
	  t = object_desc_str(t, "枚");
	  break;
      }
	    /* 食べもの by ita */
      case TV_FOOD:
      {
	  if(object_ptr->sval == SV_FOOD_JERKY)
	  {
	      t = object_desc_str(t, "切れ");
	      break;
	  }
      }
      default:
      {
	  if(object_ptr->number < 10)
	  {
	      t = object_desc_str(t, "つ");
	  }
	  else
	  {
	      t = object_desc_str(t, "個");
	  }
	  break;
      }
  }
  return (t);		       
}
#endif

/*
 * Print an signed number "v" into a string "t", as if by
 * sprintf(t, "%+d", n), and return a pointer to the terminator.
 * Note that we always print a sign, either "+" or "-".
 */
static char *object_desc_int(char *t, sint v)
{
	uint p, n;

	/* Negative */
	if(v < 0)
	{
		/* Take the absolute value */
		n = 0 - v;

		/* Use a "minus" sign */
		*t++ = '-';
	}

	/* Positive (or zero) */
	else
	{
		/* Use the actual number */
		n = v;

		/* Use a "plus" sign */
		*t++ = '+';
	}

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + (char)(n / p);

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	*t = '\0';

	return (t);
}


/*
 * Structs and tables for Auto Inscription for flags
 */

typedef struct flag_insc_table
{
#ifdef JP
	cptr japanese;
#endif
	cptr english;
	int flag;
	int except_flag;
} flag_insc_table;

#ifdef JP
static flag_insc_table flag_insc_plus[] =
{
	{ "攻", "At", TRAIT_BLOWS, -1 },
	{ "速", "Sp", TRAIT_SPEED, -1 },
	{ "腕", "St", STAT_STR, -1 },
	{ "知", "In", STAT_INT, -1 },
	{ "賢", "Wi", STAT_WIS, -1 },
	{ "器", "Dx", STAT_DEX, -1 },
	{ "耐", "Cn", STAT_CON, -1 },
	{ "魅", "Ch", STAT_CHA, -1 },
	{ "道", "Md", TRAIT_MAGIC_MASTERY, -1 },
	{ "隠", "Sl", TRAIT_STEALTH, -1 },
	{ "探", "Sr", TRAIT_SEARCH, -1 },
	{ "赤", "If", TRAIT_INFRA, -1 },
	{ "掘", "Dg", TRAIT_TUNNEL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
	{ "酸", "Ac", TRAIT_IM_ACID, -1 },
	{ "電", "El", TRAIT_IM_ELEC, -1 },
	{ "火", "Fi", TRAIT_IM_FIRE, -1 },
	{ "冷", "Co", TRAIT_IM_COLD, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_resistance[] =
{
	{ "酸", "Ac", TRAIT_RES_ACID, TRAIT_IM_ACID },
	{ "電", "El", TRAIT_RES_ELEC, TRAIT_IM_ELEC },
	{ "火", "Fi", TRAIT_RES_FIRE, TRAIT_IM_FIRE },
	{ "冷", "Co", TRAIT_RES_COLD, TRAIT_IM_COLD },
	{ "毒", "Po", TRAIT_RES_POIS, -1 },
	{ "閃", "Li", TRAIT_RES_LITE, -1 },
	{ "暗", "Dk", TRAIT_RES_DARK, -1 },
	{ "破", "Sh", TRAIT_RES_SHAR, -1 },
	{ "盲", "Bl", TRAIT_NO_BLIND, -1 },
	{ "乱", "Cf", TRAIT_NO_CONF, -1 },
	{ "轟", "So", TRAIT_RES_SOUN, -1 },
	{ "獄", "Nt", TRAIT_RES_NETH, -1 },
	{ "因", "Nx", TRAIT_RES_NEXU, -1 },
	{ "沌", "Ca", TRAIT_RES_CHAO, -1 },
	{ "劣", "Di", TRAIT_RES_DISE, -1 },
	{ "恐", "Fe", TRAIT_FEARLESS, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_misc[] =
{
	{ "易", "Es", TRAIT_EASY_SPELL, -1 },
	{ "減", "Dm", TRAIT_DEC_MANA, -1 },
	{ "投", "Th", TRAIT_THROW_MIGHTY, -1 },
	{ "反", "Rf", TRAIT_REFLECTING, -1 },
	{ "麻", "Fa", TRAIT_FREE_ACTION, -1 },
	{ "視", "Si", TRAIT_SEE_INVISIBLE, -1 },
	{ "経", "Hl", TRAIT_HOLD_LIFE, -1 },
	{ "遅", "Sd", TRAIT_SLOW_DIGEST, -1 },
	{ "活", "Rg", TRAIT_REGENERATE, -1 },
	{ "浮", "Lv", TRAIT_LEVITATION, -1 },
	{ "明", "Lu", TRAIT_LITE, -1 },
	{ "警", "Wr", TRAIT_WARNING, -1 },
	{ "倍", "Xm", TRAIT_EXTRA_ATTACK_MIGHT, -1 },
	{ "射", "Xs", TRAIT_EXTRA_ATTACK_SPEED, -1 },
	{ "瞬", "Te", TRAIT_PASSIVE_TELEPORT, -1 },
	{ "怒", "Ag", TRAIT_ANTIPATHY, -1 },
	{ "祝", "Bs", TRAIT_BLESSED_BRAND, -1 },
	{ "忌", "Ty", TRAIT_TY_CURSE, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
	{ "炎", "F", TRAIT_AURA_FIRE, -1 },
	{ "電", "E", TRAIT_AURA_ELEC, -1 },
	{ "冷", "C", TRAIT_AURA_COLD, -1 },
	{ "魔", "M", TRAIT_ANTI_MAGIC, -1 },
	{ "瞬", "T", TRAIT_PREVENT_TELEPORT, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_brand[] =
{
	{ "酸", "A", TRAIT_ACID_BRAND, -1 },
	{ "電", "E", TRAIT_ELEC_BRAND, -1 },
	{ "焼", "F", TRAIT_FIRE_BRAND, -1 },
	{ "凍", "Co", TRAIT_COLD_BRAND, -1 },
	{ "毒", "P", TRAIT_POIS_BRAND, -1 },
	{ "沌", "Ca", TRAIT_CHAOTIC_BRAND, -1 },
	{ "吸", "V", TRAIT_VAMPIRIC_BRAND, -1 },
	{ "震", "Q", TRAIT_SHATTER, -1 },
	{ "切", "S", TRAIT_VORPAL, -1 },
	{ "理", "M", TRAIT_FORCE_WEAPON, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
	{ "邪", "*", TRAIT_KILL_EVIL, -1 },
	{ "善", "#", TRAIT_KILL_GOOD, -1 },
	{ "人", "p", TRAIT_KILL_HUMAN, -1 },
	{ "龍", "D", TRAIT_KILL_DRAGON, -1 },
	{ "オ", "o", TRAIT_KILL_ORC, -1 },
	{ "ト", "T", TRAIT_KILL_TROLL, -1 },
	{ "巨", "P", TRAIT_KILL_GIANT, -1 },
	{ "デ", "U", TRAIT_KILL_DEMON, -1 },
	{ "死", "L", TRAIT_KILL_UNDEAD, -1 },
	{ "動", "Z", TRAIT_KILL_ANIMAL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
	{ "邪", "*", TRAIT_SLAY_EVIL, TRAIT_KILL_EVIL },
	{ "邪", "#", TRAIT_SLAY_EVIL, TRAIT_KILL_GOOD },
	{ "人", "p", TRAIT_SLAY_HUMAN, TRAIT_KILL_HUMAN },
	{ "竜", "D", TRAIT_SLAY_DRAGON, TRAIT_KILL_DRAGON },
	{ "オ", "o", TRAIT_SLAY_ORC, TRAIT_KILL_ORC },
	{ "ト", "T", TRAIT_SLAY_TROLL, TRAIT_KILL_TROLL },
	{ "巨", "P", TRAIT_SLAY_GIANT, TRAIT_KILL_GIANT },
	{ "デ", "U", TRAIT_SLAY_DEMON, TRAIT_KILL_DEMON },
	{ "死", "L", TRAIT_SLAY_UNDEAD, TRAIT_KILL_UNDEAD },
	{ "動", "Z", TRAIT_SLAY_ANIMAL, TRAIT_KILL_ANIMAL },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
	{ "感", "Tele", TRAIT_ESP, -1 },
	{ "邪", "Evil", TRAIT_SENSE_EVIL, -1 },
	{ "善", "Good", TRAIT_SENSE_GOOD, -1 },
	{ "無", "Nolv", TRAIT_SENSE_NONLIVING, -1 },
	{ "個", "Uniq", TRAIT_SENSE_UNIQUE, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
	{ "人", "p", TRAIT_SENSE_HUMAN, -1 },
	{ "竜", "D", TRAIT_SENSE_DRAGON, -1 },
	{ "オ", "o", TRAIT_SENSE_ORC, -1 },
	{ "ト", "T", TRAIT_SENSE_TROLL, -1 },
	{ "巨", "P", TRAIT_SENSE_GIANT, -1 },
	{ "デ", "U", TRAIT_SENSE_DEMON, -1 },
	{ "死", "L", TRAIT_SENSE_UNDEAD, -1 },
	{ "動", "Z", TRAIT_SENSE_ANIMAL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_sust[] =
{
	{ "腕", "St", TRAIT_SUSTAIN_STR, -1 },
	{ "知", "In", TRAIT_SUSTAIN_INT, -1 },
	{ "賢", "Wi", TRAIT_SUSTAIN_WIS, -1 },
	{ "器", "Dx", TRAIT_SUSTAIN_DEX, -1 },
	{ "耐", "Cn", TRAIT_SUSTAIN_CON, -1 },
	{ "魅", "Ch", TRAIT_SUSTAIN_CHR, -1 },
	{ NULL, NULL, 0, -1 }
};

#else
static flag_insc_table flag_insc_plus[] =
{
	{ "At", TRAIT_BLOWS, -1 },
	{ "Sp", TRAIT_SPEED, -1 },
	{ "St", STAT_STR, -1 },
	{ "In", STAT_INT, -1 },
	{ "Wi", STAT_WIS, -1 },
	{ "Dx", STAT_DEX, -1 },
	{ "Cn", STAT_CON, -1 },
	{ "Ch", STAT_CHA, -1 },
	{ "Md", TRAIT_MAGIC_MASTERY, -1 },
	{ "Sl", TRAIT_STEALTH, -1 },
	{ "Sr", TRAIT_SEARCH, -1 },
	{ "If", TRAIT_INFRA, -1 },
	{ "Dg", TRAIT_TUNNEL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
	{ "Ac", TRAIT_IM_ACID, -1 },
	{ "El", TRAIT_IM_ELEC, -1 },
	{ "Fi", TRAIT_IM_FIRE, -1 },
	{ "Co", TRAIT_IM_COLD, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_resistance[] =
{
	{ "Ac", TRAIT_RES_ACID, TRAIT_IM_ACID },
	{ "El", TRAIT_RES_ELEC, TRAIT_IM_ELEC },
	{ "Fi", TRAIT_RES_FIRE, TRAIT_IM_FIRE },
	{ "Co", TRAIT_RES_COLD, TRAIT_IM_COLD },
	{ "Po", TRAIT_RES_POIS, -1 },
	{ "Li", TRAIT_RES_LITE, -1 },
	{ "Dk", TRAIT_RES_DARK, -1 },
	{ "Sh", TRAIT_RES_SHAR, -1 },
	{ "Bl", TRAIT_NO_BLIND, -1 },
	{ "Cf", TRAIT_NO_CONF, -1 },
	{ "So", TRAIT_RES_SOUN, -1 },
	{ "Nt", TRAIT_RES_NETH, -1 },
	{ "Nx", TRAIT_RES_NEXU, -1 },
	{ "Ca", TRAIT_RES_CHAO, -1 },
	{ "Di", TRAIT_RES_DISE, -1 },
	{ "Fe", TRAIT_FEARLESS, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_misc[] =
{
	{ "Es", TRAIT_EASY_SPELL, -1 },
	{ "Dm", TRAIT_DEC_MANA, -1 },
	{ "Th", TRAIT_THROW_MIGHTY, -1 },
	{ "Rf", TRAIT_REFLECTING, -1 },
	{ "Fa", TRAIT_FREE_ACTION, -1 },
	{ "Si", TRAIT_SEE_INVISIBLE, -1 },
	{ "Hl", TRAIT_HOLD_LIFE, -1 },
	{ "Sd", TRAIT_SLOW_DIGEST, -1 },
	{ "Rg", TRAIT_REGENERATE, -1 },
	{ "Lv", TRAIT_LEVITATION, -1 },
	{ "Lu", TRAIT_LITE, -1 },
	{ "Wr", TRAIT_WARNING, -1 },
	{ "Xm", TRAIT_EXTRA_ATTACK_MIGHT, -1 },
	{ "Xs", TRAIT_EXTRA_ATTACK_SPEED, -1 },
	{ "Te", TRAIT_PASSIVE_TELEPORT, -1 },
	{ "Ag", TRAIT_ANTIPATHY, -1 },
	{ "Bs", TRAIT_BLESSED_BRAND, -1 },
	{ "Ty", TRAIT_TY_CURSE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
	{ "F", TRAIT_AURA_FIRE, -1 },
	{ "E", TRAIT_AURA_ELEC, -1 },
	{ "C", TRAIT_AURA_COLD, -1 },
	{ "M", TRAIT_ANTI_MAGIC, -1 },
	{ "T", TRAIT_PREVENT_TELEPORT, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_brand[] =
{
	{ "A", TRAIT_ACID_BRAND, -1 },
	{ "E", TRAIT_ELEC_BRAND, -1 },
	{ "F", TRAIT_FIRE_BRAND, -1 },
	{ "Co", TRAIT_COLD_BRAND, -1 },
	{ "P", TRAIT_POIS_BRAND, -1 },
	{ "Ca", TRAIT_CHAOTIC_BRAND, -1 },
	{ "V", TRAIT_VAMPIRIC_BRAND, -1 },
	{ "Q", TRAIT_SHATTER, -1 },
	{ "S", TRAIT_VORPAL, -1 },
	{ "M", TRAIT_FORCE_WEAPON, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
	{ "*", TRAIT_KILL_EVIL, -1 },
	{ "#", TRAIT_KILL_GOOD, -1 },
	{ "p", TRAIT_KILL_HUMAN, -1 },
	{ "D", TRAIT_KILL_DRAGON, -1 },
	{ "o", TRAIT_KILL_ORC, -1 },
	{ "T", TRAIT_KILL_TROLL, -1 },
	{ "P", TRAIT_KILL_GIANT, -1 },
	{ "U", TRAIT_KILL_DEMON, -1 },
	{ "L", TRAIT_KILL_UNDEAD, -1 },
	{ "Z", TRAIT_KILL_ANIMAL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
	{ "*", TRAIT_SLAY_EVIL, TRAIT_KILL_EVIL },
	{ "#", TRAIT_SLAY_GOOD, TRAIT_KILL_GOOD },
	{ "p", TRAIT_SLAY_HUMAN, TRAIT_KILL_HUMAN },
	{ "D", TRAIT_SLAY_DRAGON, TRAIT_KILL_DRAGON },
	{ "o", TRAIT_SLAY_ORC, TRAIT_KILL_ORC },
	{ "T", TRAIT_SLAY_TROLL, TRAIT_KILL_TROLL },
	{ "P", TRAIT_SLAY_GIANT, TRAIT_KILL_GIANT },
	{ "U", TRAIT_SLAY_DEMON, TRAIT_KILL_DEMON },
	{ "L", TRAIT_SLAY_UNDEAD, TRAIT_KILL_UNDEAD },
	{ "Z", TRAIT_SLAY_ANIMAL, TRAIT_KILL_ANIMAL },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
	{ "Tele", TRAIT_ESP, -1 },
	{ "Evil", TRAIT_SENSE_EVIL, -1 },
	{ "Good", TRAIT_SENSE_GOOD, -1 },
	{ "Nolv", TRAIT_SENSE_NONLIVING, -1 },
	{ "Uniq", TRAIT_SENSE_UNIQUE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
	{ "p", TRAIT_SENSE_HUMAN, -1 },
	{ "D", TRAIT_SENSE_DRAGON, -1 },
	{ "o", TRAIT_SENSE_ORC, -1 },
	{ "T", TRAIT_SENSE_TROLL, -1 },
	{ "P", TRAIT_SENSE_GIANT, -1 },
	{ "U", TRAIT_SENSE_DEMON, -1 },
	{ "L", TRAIT_SENSE_UNDEAD, -1 },
	{ "Z", TRAIT_SENSE_ANIMAL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_sust[] =
{
	{ "St", TRAIT_SUSTAIN_STR, -1 },
	{ "In", TRAIT_SUSTAIN_INT, -1 },
	{ "Wi", TRAIT_SUSTAIN_WIS, -1 },
	{ "Dx", TRAIT_SUSTAIN_DEX, -1 },
	{ "Cn", TRAIT_SUSTAIN_CON, -1 },
	{ "Ch", TRAIT_SUSTAIN_CHR, -1 },
	{ NULL, 0, -1 }
};
#endif

/* Simple macro for get_inscription() */
#define ADD_INSC(STR) (void)(ptr = object_desc_str(ptr, (STR)))

/*
 *  Helper function for get_inscription()
 */
static char *inscribe_flags_aux(flag_insc_table *fi_ptr, u32b flgs[MAX_TRAITS_FLAG], bool kanji, char *ptr)
{
#ifndef JP
	(void)kanji;
#endif

	while (fi_ptr->english)
	{
		if(have_flag(flgs, fi_ptr->flag) &&
		    (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
#ifdef JP
			ADD_INSC(kanji ? fi_ptr->japanese : fi_ptr->english);
#else
			ADD_INSC(fi_ptr->english);
#endif
		fi_ptr++;
	}

	return ptr;
}


/*
 *  Special variation of have_flag for auto-inscription
 */
static bool have_flag_of(flag_insc_table *fi_ptr, u32b flgs[MAX_TRAITS_FLAG])
{
	while (fi_ptr->english)
	{
		if(have_flag(flgs, fi_ptr->flag) &&
		   (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
			return TRUE;
		fi_ptr++;
	}

	return FALSE;
}

static char *get_ability_abbreviation(char *ptr, object_type *object_ptr, bool kanji, bool all)
{
	char *prev_ptr = ptr;
	u32b flgs[MAX_TRAITS_FLAG];

	/* Extract the flags */
	object_flags(object_ptr, flgs);


	/* Remove obvious flags */
	if(!all)
	{
		object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];
		int j;
				
		/* Base object */
		for (j = 0; j < MAX_TRAITS_FLAG; j++)
			flgs[j] &= ~object_kind_ptr->flags[j];

		if(object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];
					
			for (j = 0; j < MAX_TRAITS_FLAG; j++)
				flgs[j] &= ~a_ptr->flags[j];
		}

		if(object_is_ego(object_ptr))
		{
			ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];
					
			for (j = 0; j < MAX_TRAITS_FLAG; j++)
				flgs[j] &= ~e_ptr->flags[j];
		}
	}


	/* Plusses */
	if(have_flag_of(flag_insc_plus, flgs))
	{
		if(kanji)
			ADD_INSC("+");
	}
	ptr = inscribe_flags_aux(flag_insc_plus, flgs, kanji, ptr);

	/* Immunity */
	if(have_flag_of(flag_insc_immune, flgs))
	{
		if(!kanji && ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
		ADD_INSC("*");
	}
	ptr = inscribe_flags_aux(flag_insc_immune, flgs, kanji, ptr);

	/* Resistance */
	if(have_flag_of(flag_insc_resistance, flgs))
	{
		if(kanji)
			ADD_INSC("r");
		else if(ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
	}
	ptr = inscribe_flags_aux(flag_insc_resistance, flgs, kanji, ptr);

	/* Misc Ability */
	if(have_flag_of(flag_insc_misc, flgs))
	{
		if(ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
	}
	ptr = inscribe_flags_aux(flag_insc_misc, flgs, kanji, ptr);

	/* Aura */
	if(have_flag_of(flag_insc_aura, flgs))
	{
		ADD_INSC("[");
	}
	ptr = inscribe_flags_aux(flag_insc_aura, flgs, kanji, ptr);

	/* Brand Weapon */
	if(have_flag_of(flag_insc_brand, flgs))
		ADD_INSC("|");
	ptr = inscribe_flags_aux(flag_insc_brand, flgs, kanji, ptr);

	/* Kill Weapon */
	if(have_flag_of(flag_insc_kill, flgs))
		ADD_INSC("/X");
	ptr = inscribe_flags_aux(flag_insc_kill, flgs, kanji, ptr);

	/* Slay Weapon */
	if(have_flag_of(flag_insc_slay, flgs))
		ADD_INSC("/");
	ptr = inscribe_flags_aux(flag_insc_slay, flgs, kanji, ptr);

	/* Esp */
	if(kanji)
	{
		if(have_flag_of(flag_insc_esp1, flgs) ||
		    have_flag_of(flag_insc_esp2, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp1, flgs, kanji, ptr);
		ptr = inscribe_flags_aux(flag_insc_esp2, flgs, kanji, ptr);
	}
	else
	{
		if(have_flag_of(flag_insc_esp1, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp1, flgs, kanji, ptr);
		if(have_flag_of(flag_insc_esp2, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp2, flgs, kanji, ptr);
	}

	/* sustain */
	if(have_flag_of(flag_insc_sust, flgs))
	{
		ADD_INSC("(");
	}
	ptr = inscribe_flags_aux(flag_insc_sust, flgs, kanji, ptr);

	*ptr = '\0';

	return ptr;
}


/*
 *  Get object inscription with auto inscription of object flags.
 */
static void get_inscription(char *buff, object_type *object_ptr)
{
	cptr insc = quark_str(object_ptr->inscription);
	char *ptr = buff;

	/* Not fully identified */
	if(!(object_ptr->ident & IDENT_MENTAL))
	{
		/* Copy until end of line or '#' */
		while (*insc)
		{
			if(*insc == '#') break;
#ifdef JP
			if(iskanji(*insc)) *buff++ = *insc++;
#endif
			*buff++ = *insc++;
		}

		*buff = '\0';
		return;
	}

	*buff = '\0';
	for (; *insc; insc++)
	{
		/* Ignore fake artifact inscription */
		if(*insc == '#') break;

		/* {%} will be automatically converted */
		else if('%' == *insc)
		{
			bool kanji = FALSE;
			bool all;
			cptr start = ptr;

			/* check for too long inscription */
			if(ptr >= buff + MAX_NLEN) continue;

#ifdef JP
			if('%' == insc[1])
			{
				insc++;
				kanji = FALSE;
			}
			else
			{
				kanji = TRUE;
			}
#endif
			if('a' == insc[1] && 'l' == insc[2] && 'l' == insc[3])
			{
				all = TRUE;
				insc += 3;
			}
			else
			{
				all = FALSE;
			}

			ptr = get_ability_abbreviation(ptr, object_ptr, kanji, all);

			if(ptr == start)
				ADD_INSC(" ");
		}
		else
		{
			*ptr++ = *insc;
		}
	}
	*ptr = '\0';
}

/*
 * Creates a description of the item "object_ptr", and stores it in "out_val".
 *
 * One can choose the "verbosity" of the description, including whether
 * or not the "number" of items should be described, and how much detail
 * should be used when describing the item.
 *
 * The given "buf" must be MAX_NLEN chars long to hold the longest possible
 * description, which can get pretty long, including incriptions, such as:
 * "no more Maces of Disruption (Defender) (+10,+10) [+5] (+3 to stealth)".
 * Note that the inscription will be clipped to keep the total description
 * under MAX_NLEN-1 chars (plus a terminator).
 *
 * Note the use of "object_desc_num()" and "object_desc_int()" as hyper-efficient,
 * portable, versions of some common "sprintf()" commands.
 *
 * Note that all ego-items (when known) append an "Ego-Item Name", unless
 * the item is also an artifact, which should NEVER happen.
 *
 * Note that all artifacts (when known) append an "Artifact Name", so we
 * have special processing for "Specials" (artifact Lites, Rings, Amulets).
 * The "Specials" never use "modifiers" if they are "known", since they
 * have special "descriptions", such as "The Necklace of the Dwarves".
 *
 * Special Lite's use the "object_kind_info" base-name (Phial, Star, or Arkenstone),
 * plus the artifact name, just like any other artifact, if known.
 *
 * Special Ring's and Amulet's, if not "aware", use the same code as normal
 * rings and amulets, and if "aware", use the "object_kind_info" base-name (Ring or
 * Amulet or Necklace).  They will NEVER "append" the "object_kind_info" name.  But,
 * they will append the artifact name, just like any artifact, if known.
 *
 * Hack -- Display "The One Ring" as "a Plain Gold Ring" until aware.
 *
 * Mode:
 *   OD_NAME_ONLY        : The Cloak of Death
 *   OD_NAME_AND_ENCHANT : The Cloak of Death [1,+3]
 *   OD_OMIT_INSCRIPTION : The Cloak of Death [1,+3] (+2 to Stealth)
 *   0                   : The Cloak of Death [1,+3] (+2 to Stealth) {nifty}
 *
 *   OD_OMIT_PREFIX      : Forbidden numeric prefix
 *   OD_NO_PLURAL        : Forbidden use of plural 
 *   OD_STORE            : Assume to be aware and known
 *   OD_NO_FLAVOR        : Allow to hidden flavor
 *   OD_FORCE_FLAVOR     : Get un-shuffled flavor name
 */
void object_desc(char *buf, object_type *object_ptr, FLAGS_32 mode)
{
	cptr            kindname = object_kind_name + object_kind_info[object_ptr->k_idx].name;
	cptr            basenm = kindname;
	cptr            modstr = "";

	int             power;

	bool            aware = FALSE;
	bool            known = FALSE;
	bool            flavor = TRUE;

	bool            show_weapon = FALSE;
	bool            show_armour = FALSE;

	cptr            s, s0;
	char            *t;

	char            p1 = '(', p2 = ')';
	char            b1 = '[', b2 = ']';
	char            c1 = '{', c2 = '}';
	char            f1 = '<', f2 = '>';

	char            tmp_val[MAX_NLEN+160];
	char            tmp_val2[MAX_NLEN+10];
	char            fake_insc_buf[30];

	u32b flgs[MAX_TRAITS_FLAG];
	//object_type *bow_ptr;

	object_kind *object_kind_ptr = &object_kind_info[object_ptr->k_idx];
	object_kind *flavor_object_kind_ptr = &object_kind_info[object_kind_ptr->flavor];

	object_flags(object_ptr, flgs);	// Extract some flags
	if(object_is_aware(object_ptr)) aware = TRUE;	// See if the object is "aware"
	if(object_is_known(object_ptr)) known = TRUE;	// See if the object is "known"
	if(aware && ((mode & OD_NO_FLAVOR) || plain_descriptions)) flavor = FALSE;	// Allow flavors to be hidden when aware

	/* Object is in the inventory of a store or spoiler */
	if((mode & OD_STORE) || (object_ptr->ident & IDENT_STORE))
	{
		/* Don't show flavors */
		flavor = FALSE;

		/* Pretend known and aware */
		aware = TRUE;
		known = TRUE;
	}

	/* Force to be flavor name only */
	if(mode & OD_FORCE_FLAVOR)
	{
		aware = FALSE;
		flavor = TRUE;
		known = FALSE;

		/* Cancel shuffling */
		flavor_object_kind_ptr = object_kind_ptr;
	}

	/* Analyze the object */
	switch (object_ptr->tval)
	{
		/* Some objects are easy to describe */
		case TV_SKELETON:
		case TV_BOTTLE:
		case TV_JUNK:
		case TV_SPIKE:
		case TV_FLASK:
		case TV_CHEST:
		case TV_WHISTLE:
		case TV_WHEEL:
		case TV_INSTRUMENT:
		{
			break;
		}

		case TV_CAPTURE:
		{
			species_type *species_ptr = &species_info[object_ptr->pval];

			if(known)
			{
				if(!object_ptr->pval)
				{
#ifdef JP
					modstr = " (空)";
#else
					modstr = " (empty)";
#endif
				}
				else
				{
#ifdef JP
					sprintf(tmp_val2, " (%s)",species_name + species_ptr->name);
					modstr = tmp_val2;
#else
					cptr t = species_name + species_ptr->name;

					if(!has_trait_species(species_ptr, TRAIT_UNIQUE))
					{
						sprintf(tmp_val2, " (%s%s)", (is_a_vowel(*t) ? "an " : "a "), t);

						modstr = tmp_val2;
					}
					else
					{
						sprintf(tmp_val2, "(%s)", t);

						modstr = t;
					}
#endif
				}
			}
			break;
		}

		/* Figurines/Statues */
		case TV_FIGURINE:
		case TV_STATUE:
		{
			species_type *species_ptr = &species_info[object_ptr->source_idx];

#ifdef JP
			modstr = species_name + species_ptr->name;
#else
			cptr t = species_name + species_ptr->name;

			if(!has_trait_species(species_ptr, TRAIT_UNIQUE))
			{
				sprintf(tmp_val2, "%s%s", (is_a_vowel(*t) ? "an " : "a "), t);

				modstr = tmp_val2;
			}
			else
			{
				modstr = t;
			}
#endif

			break;
		}

		/* Corpses */
		case TV_CORPSE:
		{
			species_type *species_ptr = &species_info[object_ptr->source_idx];

			modstr = species_name + species_ptr->name;

#ifdef JP
			basenm = "#%";
#else
			if(has_trait_species(species_ptr, TRAIT_UNIQUE))
				basenm = "& % of #";
			else
				basenm = "& # %";
#endif

			break;
		}

		/* Missiles/ Bows/ Weapons */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_BOW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			show_weapon = TRUE;
			break;
		}

		/* Armour */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_TAIL:
		{
			show_armour = TRUE;
			break;
		}

		/* Lites (including a few "Specials") */
		case TV_LITE:
		{
			break;
		}

		/* Amulets (including a few "Specials") */
		case TV_AMULET:
		{
			/* Known artifacts */
			if(aware)
			{
				if(object_is_fixed_artifact(object_ptr)) break;
				if(has_trait_object(object_ptr, TRAIT_INSTA_ART)) break;
			}

			/* Color the object */
			modstr = object_kind_name + flavor_object_kind_ptr->flavor_bane;

#ifdef JP
			if(!flavor)    basenm = "%のアミュレット";
			else if(aware) basenm = "%の#アミュレット";
			else            basenm = "#アミュレット";
#else
			if(!flavor)    basenm = "& Amulet~ of %";
			else if(aware) basenm = "& # Amulet~ of %";
			else            basenm = "& # Amulet~";
#endif

			break;
		}

		/* Rings (including a few "Specials") */
		case TV_RING:
		{
			/* Known artifacts */
			if(aware)
			{
				if(object_is_fixed_artifact(object_ptr)) break;
				if(has_trait_object(object_ptr, TRAIT_INSTA_ART)) break;
			}

			/* Color the object */
			modstr = object_kind_name + flavor_object_kind_ptr->flavor_bane;

#ifdef JP
			if(!flavor)    basenm = "%の指輪";
			else if(aware) basenm = "%の#指輪";
			else            basenm = "#指輪";
#else
			if(!flavor)    basenm = "& Ring~ of %";
			else if(aware) basenm = "& # Ring~ of %";
			else            basenm = "& # Ring~";
#endif

			if(!object_kind_ptr->to_hit && !object_kind_ptr->to_damage && (object_ptr->to_hit || object_ptr->to_damage)) show_weapon = TRUE;

			break;
		}

		case TV_CARD:
		{
			break;
		}

		case TV_STAFF:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_object_kind_ptr->flavor_bane;

#ifdef JP
			if(!flavor)    basenm = "%の杖";
			else if(aware) basenm = "%の#杖";
			else            basenm = "#杖";
#else
			if(!flavor)    basenm = "& Staff~ of %";
			else if(aware) basenm = "& # Staff~ of %";
			else            basenm = "& # Staff~";
#endif

			break;
		}

		case TV_WAND:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_object_kind_ptr->flavor_bane;

#ifdef JP
			if(!flavor)    basenm = "%の魔法棒";
			else if(aware) basenm = "%の#魔法棒";
			else            basenm = "#魔法棒";
#else
			if(!flavor)    basenm = "& Wand~ of %";
			else if(aware) basenm = "& # Wand~ of %";
			else            basenm = "& # Wand~";
#endif

			break;
		}

		case TV_ROD:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_object_kind_ptr->flavor_bane;

#ifdef JP
			if(!flavor)    basenm = "%のロッド";
			else if(aware) basenm = "%の#ロッド";
			else            basenm = "#ロッド";
#else
			if(!flavor)    basenm = "& Rod~ of %";
			else if(aware) basenm = "& # Rod~ of %";
			else            basenm = "& # Rod~";
#endif

			break;
		}

		case TV_SCROLL:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_object_kind_ptr->flavor_bane;

#ifdef JP
			if(!flavor)    basenm = "%の巻物";
			else if(aware) basenm = "「#」と書かれた%の巻物";
			else            basenm = "「#」と書かれた巻物";
#else
			if(!flavor)    basenm = "& Scroll~ of %";
			else if(aware) basenm = "& Scroll~ titled \"#\" of %";
			else            basenm = "& Scroll~ titled \"#\"";
#endif

			break;
		}

		case TV_POTION:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_object_kind_ptr->flavor_bane;

#ifdef JP
			if(!flavor)    basenm = "%の薬";
			else if(aware) basenm = "%の#薬";
			else            basenm = "#薬";
#else
			if(!flavor)    basenm = "& Potion~ of %";
			else if(aware) basenm = "& # Potion~ of %";
			else            basenm = "& # Potion~";
#endif

			break;
		}

		case TV_FOOD:
		{
			/* Ordinary food is "boring" */
			if(!object_kind_ptr->flavor_bane) break;

			/* Color the object */
			modstr = object_kind_name + flavor_object_kind_ptr->flavor_bane;

#ifdef JP
			if(!flavor)    basenm = "%のキノコ";
			else if(aware) basenm = "%の#キノコ";
			else            basenm = "#キノコ";
#else
			if(!flavor)    basenm = "& Mushroom~ of %";
			else if(aware) basenm = "& # Mushroom~ of %";
			else            basenm = "& # Mushroom~";
#endif

			break;
		}

		case TV_PARCHMENT:
		{
#ifdef JP
			basenm = "羊皮紙 - %";
#else
			basenm = "& Parchment~ - %";
#endif
			break;
		}

		/* Magic Books */
		case TV_LIFE_BOOK:
		{
#ifdef JP
			basenm = "生命の魔法書%";
#else
			basenm = "& Life Spellbook~ %";
#endif

			break;
		}

		case TV_SORCERY_BOOK:
		{
#ifdef JP
			basenm = "仙術の魔法書%";
#else
			basenm = "& Sorcery Spellbook~ %";
#endif

			break;
		}

		case TV_NATURE_BOOK:
		{
#ifdef JP
			basenm = "自然の魔法書%";
#else
			basenm = "& Nature Spellbook~ %";
#endif

			break;
		}

		case TV_CHAOS_BOOK:
		{
#ifdef JP
			basenm = "カオスの魔法書%";
#else
			basenm = "& Chaos Spellbook~ %";
#endif

			break;
		}

		case TV_DEATH_BOOK:
		{
#ifdef JP
			basenm = "暗黒の魔法書%";
#else
			basenm = "& Death Spellbook~ %";
#endif

			break;
		}

		case TV_TRUMP_BOOK:
		{
#ifdef JP
			basenm = "トランプの魔法書%";
#else
			basenm = "& Trump Spellbook~ %";
#endif

			break;
		}

		case TV_ARCANE_BOOK:
		{
#ifdef JP
			basenm = "秘術の魔法書%";
#else
			basenm = "& Arcane Spellbook~ %";
#endif

			break;
		}

		case TV_CRAFT_BOOK:
		{
#ifdef JP
			basenm = "匠の魔法書%";
#else
			basenm = "& Craft Spellbook~ %";
#endif

			break;
		}

		case TV_DAEMON_BOOK:
		{
#ifdef JP
			basenm = "悪魔の魔法書%";
#else
			basenm = "& Daemon Spellbook~ %";
#endif

			break;
		}

		case TV_CRUSADE_BOOK:
		{
#ifdef JP
			basenm = "破邪の魔法書%";
#else
			basenm = "& Crusade Spellbook~ %";
#endif

			break;
		}

		case TV_MUSIC_BOOK:
		{
#ifdef JP
			basenm = "歌集%";
#else
			basenm = "& Song Book~ %";
#endif

			break;
		}

		case TV_HISSATSU_BOOK:
		{
#ifdef JP
			basenm = "& 武芸の書%";
#else
			basenm = "Book~ of Kendo %";
#endif

			break;
		}

		case TV_HEX_BOOK:
		{
#ifdef JP
			basenm = "呪術の魔法書%";
#else
			basenm = "& Crusade Spellbook~ %";
#endif

			break;
		}

		/* Hack -- Gold/Gems */
		case TV_GOLD:
		{
			strcpy(buf, basenm);
			return;
		}

		/* Used in the "inventory" routine */
		default:
		{
#ifdef JP
			strcpy(buf, "(なし)");
#else
			strcpy(buf, "(nothing)");
#endif

			return;
		}
	}

	/* Use full name from object_kind_info or artifact_info */
	if(aware && have_flag(flgs, TRAIT_FULL_NAME))
	{
		if(known && object_ptr->name1) basenm = artifact_name + artifact_info[object_ptr->name1].name;
		else basenm = kindname;
	}

	/* Start dumping the result */
	t = tmp_val;

#ifdef JP
	if(basenm[0] == '&')
		s = basenm + 2;
	else
		s = basenm;

	/* No prefix */
	if(mode & OD_OMIT_PREFIX)
	{
		/* Nothing */
	}
	else if(object_ptr->number > 1)
	{
		t = object_desc_kosuu(t, object_ptr);
		t = object_desc_str(t, "の ");
	}

	/* 英語の場合アーティファクトは The が付くので分かるが
	 * 日本語では分からないのでマークをつける 
	 */
	if(known)
	{
		if(object_is_fixed_artifact(object_ptr)) t = object_desc_str(t, "★");
		else if(object_is_random_artifact(object_ptr)) t = object_desc_str(t, "☆");
	}

#else

	/* The object "expects" a "number" */
	if(basenm[0] == '&')
	{
		/* Skip the ampersand (and space) */
		s = basenm + 2;

		/* No prefix */
		if(mode & OD_OMIT_PREFIX)
		{
			/* Nothing */
		}

		/* Hack -- None left */
		else if(object_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Extract the number */
		else if(object_ptr->number > 1)
		{
			t = object_desc_num(t, object_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if((known && object_is_artifact(object_ptr)) ||
		         ((object_ptr->tval == TV_CORPSE) &&
		          (has_trait_species(species_info[object_ptr->pval], TRAIT_UNIQUE))))
		{
			t = object_desc_str(t, "The ");
		}

		/* A single one */
		else
		{
			bool vowel;

			switch (*s)
			{
			case '#': vowel = is_a_vowel(modstr[0]); break;
			case '%': vowel = is_a_vowel(*kindname); break;
			default:  vowel = is_a_vowel(*s); break;
			}

			if(vowel)
			{
				/* A single one, with a vowel */
				t = object_desc_str(t, "an ");
			}
			else
			{
				/* A single one, without a vowel */
				t = object_desc_str(t, "a ");
			}
		}
	}

	/* Hack -- objects that "never" take an article */
	else
	{
		/* No ampersand */
		s = basenm;

		/* No pref */
		if(mode & OD_OMIT_PREFIX)
		{
			/* Nothing */
		}

		/* Hack -- all gone */
		else if(object_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Prefix a number if required */
		else if(object_ptr->number > 1)
		{
			t = object_desc_num(t, object_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if(known && object_is_artifact(object_ptr))
		{
			t = object_desc_str(t, "The ");
		}

		/* Hack -- single items get no prefix */
		else
		{
			/* Nothing */
		}
	}
#endif


	/* while (*s == '~') s++; */

#ifdef JP
	if(object_is_smith(object_ptr))
	{
		//TODO
		t = object_desc_str(t, format("%sの", species_name + species_info[object_ptr->creator_idx].name));
	}

	/* 伝説のアイテム、名のあるアイテムの名前を付加する */
	if(known)
	{
		/* ランダム・アーティファクト */
		if(object_is_random_artifact(object_ptr))
		{
			cptr temp = quark_str(object_ptr->art_name);

			/* '『' から始まらない伝説のアイテムの名前は最初に付加する */
			/* 英語版のセーブファイルから来た 'of XXX' は,「XXXの」と表示する */
			if(strncmp(temp, "of ", 3) == 0)
			{
				t = object_desc_str(t, &temp[3]);
				t = object_desc_str(t, "の");
			}
			else if((strncmp(temp, "『", 2) != 0) &&
				 (strncmp(temp, "《", 2) != 0) &&
				 (temp[0] != '\''))
				t = object_desc_str(t, temp);
		}
		/* 伝説のアイテム */
		else if(object_ptr->name1 && !have_flag(flgs, TRAIT_FULL_NAME))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];
			/* '『' から始まらない伝説のアイテムの名前は最初に付加する */
			if(strncmp(artifact_name + a_ptr->name, "『", 2) != 0)
			{
				t = object_desc_str(t, artifact_name + a_ptr->name);
			}
		}
		/* 名のあるアイテム */
		else if(object_is_ego(object_ptr))
		{
			ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];
			t = object_desc_str(t, object_egobject_name + e_ptr->name);
		}
	}
#endif

	/* Copy the string */
	for (s0 = NULL; *s || s0; )
	{
		/* The end of the flavour/kind string. */
		if(!*s)
		{
			s = s0 + 1;
			s0 = NULL;
		}

		/* Begin to append the modifier (flavor) */
		else if((*s == '#') && !s0)
		{
			s0 = s;
			s = modstr;


			modstr = "";
		}

		/* Begin to append the kind name */
		else if((*s == '%') && !s0)
		{
			s0 = s;
			s = kindname;


			kindname = "";
		}

#ifndef JP
		/* Pluralizer */
		else if(*s == '~')
		{
			/* Add a plural if needed */
			if(!(mode & OD_NO_PLURAL) && (object_ptr->number != 1))
			{
				char k = t[-1];

				/*  Mega-Hack */

				/* Hack -- "Cutlass-es" and "Torch-es" */
				if((k == 's') || (k == 'h')) *t++ = 'e';

				/* Add an 's' */
				*t++ = 's';
			}
			s++;
		}
#endif

		/* Normal */
		else
		{
			/* Copy */
			*t++ = *s++;
		}
	}

	*t = '\0';


#ifdef JP
	/* '『'から始まる伝説のアイテムの名前は最後に付加する */
	if(known)
	{
		/* ランダムアーティファクトの名前はセーブファイルに記録
		   されるので、英語版の名前もそれらしく変換する */
		if(object_is_random_artifact(object_ptr))
		{
			char temp[256];
			int itemp;
			strcpy(temp, quark_str(object_ptr->art_name));
			/* MEGA HACK by ita */
			if(strncmp(temp, "『", 2) == 0 ||
			    strncmp(temp, "《", 2) == 0)
				t = object_desc_str(t, temp);
			else if(temp[0] == '\'')
			{
				itemp = strlen(temp);
				temp[itemp - 1] = 0;
				t = object_desc_str(t, "『");
				t = object_desc_str(t, &temp[1]);
				t = object_desc_str(t, "』");
			}
		}
		else if(object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];
			if(strncmp(artifact_name + a_ptr->name, "『", 2) == 0)
			{
				t = object_desc_str(t, artifact_name + a_ptr->name);
			}
		}
		else if(object_ptr->inscription)
		{
			cptr str = quark_str(object_ptr->inscription);

			while(*str)
			{
				if(iskanji(*str))
				{
					str += 2;
					continue;
				}
				if(*str == '#') break;
				str++;
			}
			if(*str)
			{
				/* Find the '#' */
				cptr str = my_strchr(quark_str(object_ptr->inscription), '#');

				/* Add the false name */
				t = object_desc_str(t,"『");
				t = object_desc_str(t, &str[1]);
				t = object_desc_str(t,"』");
			}
		}
	}
#else
	if(object_is_smith(owner_ptr, object_ptr))
	{
		if(!object_ptr->creator_idx)
			t = object_desc_str(t,format(" of %s the Smith", owner_ptr->name));
		else
			t = object_desc_str(t, format(" of %s", species_name + species_info[object_ptr->creator_idx].name));
	}

	/* Hack -- Append "Artifact" or "Special" names */
	if(known && !have_flag(flgs, TRAIT_FULL_NAME))
	{
		/* Is it a new random artifact ? */
		if(object_is_random_artifact(object_ptr))
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, quark_str(object_ptr->art_name));
		}

		/* Grab any artifact name */
		else if(object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, artifact_name + a_ptr->name);
		}

		/* Grab any ego-item name */
		else
		{
			if(object_is_ego(object_ptr))
			{
				ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];

				t = object_desc_chr(t, ' ');
				t = object_desc_str(t, object_egobject_name + e_ptr->name);
			}

			if(object_ptr->inscription && my_strchr(quark_str(object_ptr->inscription), '#'))
			{
				/* Find the '#' */
				cptr str = my_strchr(quark_str(object_ptr->inscription), '#');

				/* Add the false name */
				t = object_desc_chr(t, ' ');
				t = object_desc_str(t, &str[1]);
			}
		}
	}
#endif


	/* No more details wanted */
	if(mode & OD_NAME_ONLY) goto object_desc_done;

	/* Hack -- Chests must be described in detail */
	if(object_ptr->tval == TV_CHEST)
	{
		/* Not searched yet */
		if(!known)
		{
			/* Nothing */
		}

		/* May be "empty" */
		else if(!object_ptr->pval)
		{
#ifdef JP
			t = object_desc_str(t, "(空)");
#else
			t = object_desc_str(t, " (empty)");
#endif
		}

		/* May be "disarmed" */
		else if(object_ptr->pval < 0)
		{
			if(chest_traps[0 - object_ptr->pval])
			{
#ifdef JP
				t = object_desc_str(t, "(解除済)");
#else
				t = object_desc_str(t, " (disarmed)");
#endif
			}
			else
			{
#ifdef JP
				t = object_desc_str(t, "(非施錠)");
#else
				t = object_desc_str(t, " (unlocked)");
#endif
			}
		}

		/* Describe the traps, if any */
		else
		{
			/* Describe the traps */
			switch (chest_traps[object_ptr->pval])
			{
				case 0:
				{
#ifdef JP
					t = object_desc_str(t, "(施錠)");
#else
					t = object_desc_str(t, " (Locked)");
#endif
					break;
				}
				case CHEST_LOSE_STR:
				{
#ifdef JP
					t = object_desc_str(t, "(毒針)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif
					break;
				}
				case CHEST_LOSE_CON:
				{
#ifdef JP
					t = object_desc_str(t, "(毒針)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif
					break;
				}
				case CHEST_POISON:
				{
#ifdef JP
					t = object_desc_str(t, "(ガス・トラップ)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif
					break;
				}
				case CHEST_PARALYZE:
				{
#ifdef JP
					t = object_desc_str(t, "(ガス・トラップ)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif
					break;
				}
				case CHEST_EXPLODE:
				{
#ifdef JP
					t = object_desc_str(t, "(爆発装置)");
#else
					t = object_desc_str(t, " (Explosion Device)");
#endif
					break;
				}
				case CHEST_SUMMON:
				case CHEST_BIRD_STORM:
				case CHEST_E_SUMMON:
				case CHEST_H_SUMMON:
				{
#ifdef JP
					t = object_desc_str(t, "(召喚のルーン)");
#else
					t = object_desc_str(t, " (Summoning Runes)");
#endif
					break;
				}
				case CHEST_RUNES_OF_EVIL:
				{
#ifdef JP
					t = object_desc_str(t, "(邪悪なルーン)");
#else
					t = object_desc_str(t, " (Gleaming Black Runes)");
#endif
					break;
				}
				case CHEST_ALARM:
				{
#ifdef JP
					t = object_desc_str(t, "(警報装置)");
#else
					t = object_desc_str(t, " (Alarm)");
#endif
					break;
				}
				default:
				{
#ifdef JP
					t = object_desc_str(t, "(マルチ・トラップ)");
#else
					t = object_desc_str(t, " (Multiple Traps)");
#endif
					break;
				}
			}
		}
	}


	/* Display the item like a weapon */
	if(have_flag(flgs, TRAIT_SHOW_MODS)) show_weapon = TRUE;

	/* Display the item like a weapon */
	if(object_is_smith(object_ptr) && (object_ptr->forged_type == 1 + ESSENCE_SLAY_GLOVE))
		show_weapon = TRUE;

	/* Display the item like a weapon */
	if(object_ptr->to_hit && object_ptr->to_damage) show_weapon = TRUE;

	/* Display the item like armour */
	if(object_ptr->ac) show_armour = TRUE;

	/* Fitting Size */
	if(object_ptr->size_lower && object_ptr->size_upper)
	{
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, f1);
		t = object_desc_num(t, object_ptr->size_lower);
		t = object_desc_chr(t, ':');
		t = object_desc_int(t, object_ptr->size_upper);
		t = object_desc_chr(t, f2);
	}

	/* Dump base weapon info */
	switch (object_ptr->tval)
	{
		/* Missiles and Weapons */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:

		/* Append a "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, object_ptr->dd);
		t = object_desc_chr(t, 'd');
		t = object_desc_num(t, object_ptr->ds);
		t = object_desc_chr(t, p2);

		/* All done */
		break;


		/* Bows get a special "damage string" */
		case TV_BOW:

		/* Mega-Hack -- Extract the "base power" */
		power = object_ptr->bow_mul;

		/* Apply the "Extra Might" flag */
		if(have_flag(flgs, TRAIT_EXTRA_ATTACK_MIGHT)) power++;

		/* Append a special "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_chr(t, 'x');
		t = object_desc_num(t, power);
		t = object_desc_chr(t, p2);

		/* All done */
		break;
	}


	/* Add the weapon bonuses */
	if(known)
	{
		/* Show the tohit/todam on request */
		if(show_weapon)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, object_ptr->to_hit);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, object_ptr->to_damage);
			t = object_desc_chr(t, p2);
		}

		/* Show the tohit if needed */
		else if(object_ptr->to_hit)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, object_ptr->to_hit);
			t = object_desc_chr(t, p2);
		}

		/* Show the todam if needed */
		else if(object_ptr->to_damage)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, object_ptr->to_damage);
			t = object_desc_chr(t, p2);
		}
	}

	/*
	bow_ptr = &owner_ptr->inventory[BOW];

	// If have a firing weapon + ammo matches bow
	if(bow_ptr->k_idx && (object_ptr->tval == owner_ptr->tval_ammo))
	{
		int avgdam = object_ptr->dd * (object_ptr->ds + 1) * 10 / 2;
		int tmul = bow_ptr->bow_mul;
		s16b energy_fire = bow_ptr->bow_energy;

		// See if the bow is "known" - then set damage bonus
		if(object_is_known(bow_ptr)) avgdamage += (bow_ptr->to_damage * 10);

		// Effect of ammo
		if(known) avgdamage += (object_ptr->to_damage * 10);

		// Get extra "power" from "extra might"
		if(owner_ptr->xtra_might) tmul++;

		tmul = tmul * (100 + (int)(adj_str_to_damage[owner_ptr->stat_ind[STAT_STR]]) - 128);

		// Launcher multiplier
		avgdam *= tmul;
		avgdam /= (100 * 10);

		// Get extra damage from concentration
		
		if(owner_ptr->concent) avgdam = boost_concentration_damage(owner_ptr, avgdam);

		if(avgdam < 0) avgdam = 0;

		// Display (shot damage/ avg damage)
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, '/');

		if(owner_ptr->num_fire == 0)
		{
			t = object_desc_chr(t, '0');
		}
		else
		{
			// Calc effects of energy
			avgdam *= (owner_ptr->num_fire * 100);
			avgdam /= energy_fire;
			t = object_desc_num(t, avgdam);
		}

		t = object_desc_chr(t, p2);
	}
	else if((owner_ptr->class_idx == CLASS_NINJA) && (object_ptr->tval == TV_SPIKE))
	{
		int avgdam = has_trait(owner_ptr, TRAIT_THROW_MIGHTY) ? (1 + 3) : 1;
		s16b energy_fire = 100 - owner_ptr->lev;

		avgdamage += ((owner_ptr->lev + 30) * (owner_ptr->lev + 30) - 900) / 55;

		// Display (shot damage/ avg damage)
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, '/');

		// Calc effects of energy
		avgdam = 100 * avgdam / energy_fire;

		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, p2);
	}
*/

	/* Add the armor bonuses */
	if(known)
	{
		/* Show the armor class info */
		if(show_armour)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_num(t, object_ptr->ac);
			t = object_desc_int(t, object_ptr->to_ac);
			t = object_desc_chr(t, ',');
			t = object_desc_num(t, object_ptr->ev);
			t = object_desc_int(t, object_ptr->to_ev);
			t = object_desc_chr(t, ',');
			t = object_desc_num(t, object_ptr->vo);
			t = object_desc_int(t, object_ptr->to_vo);
			t = object_desc_chr(t, b2);
		}

		/* No base armor, but does increase armor */
		else if(object_ptr->to_ac)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_int(t, object_ptr->to_ac);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, object_ptr->to_ev);
			t = object_desc_chr(t, ',');
			t = object_desc_num(t, object_ptr->to_vo);
			t = object_desc_chr(t, b2);
		}
	}

	/* Hack -- always show base armor */
	else if(show_armour)
	{
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, b1);
		t = object_desc_num(t, object_ptr->ac);
		t = object_desc_chr(t, ',');
		t = object_desc_num(t, object_ptr->ev);
		t = object_desc_chr(t, ',');
		t = object_desc_num(t, object_ptr->vo);
		t = object_desc_chr(t, b2);
	}


	/* No more details wanted */
	if(mode & OD_NAME_AND_ENCHANT) goto object_desc_done;


	if(known) /* Known item only */
	{
		/*
		 * Hack -- Wands and Staffs have charges.  Make certain how many charges
		 * a stack of staffs really has is clear. -LM-
		 */
		if(((object_ptr->tval == TV_STAFF) || (object_ptr->tval == TV_WAND)))
		{
			/* Dump " (N charges)" */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Clear explaination for staffs. */
			if((object_ptr->tval == TV_STAFF) && (object_ptr->number > 1))
			{
				t = object_desc_num(t, object_ptr->number);
				t = object_desc_str(t, "x ");
			}
			t = object_desc_num(t, object_ptr->pval);
			t = object_desc_str(t, MES_SYS_CHARGE_NUM(object_ptr->number));
			t = object_desc_chr(t, p2);
		}
		/* Hack -- Rods have a "charging" indicator.  Now that stacks of rods may
		 * be in any state of charge or discharge, this now includes a number. -LM-
		 */
		else if(IS_ROD(object_ptr))
		{
			/* Hack -- Dump " (# charging)" if relevant */
			if(object_ptr->timeout)
			{
				/* Stacks of rods display an exact count of charging rods. */
				if(object_ptr->number > 1)
				{
					if(object_kind_ptr->pval == 0) object_kind_ptr->pval = 1;

					/* Find out how many rods are charging, by dividing
					 * current timeout by each rod's maximum timeout.
					 * Ensure that any remainder is rounded up.  Display
					 * very discharged stacks as merely fully discharged.
					 */
					power = (object_ptr->timeout + (object_kind_ptr->pval - 1)) / object_kind_ptr->pval;
					if(power > object_ptr->number) power = object_ptr->number;

					/* Display prettily. */
					t = object_desc_str(t, " (");
					t = object_desc_num(t, power);
#ifdef JP
					t = object_desc_str(t, "本 充填中)");
#else
					t = object_desc_str(t, " charging)");
#endif
				}

				/* "one Rod of Perception (1 charging)" would look tacky. */
				else
				{
#ifdef JP
					t = object_desc_str(t, "(充填中)");
#else
					t = object_desc_str(t, " (charging)");
#endif
				}
			}
		}

		/* Dump "pval" flags for wearable items */
		if(have_pval_flags(flgs))
		{
			/* Start the display */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Dump the "pval" itself */
			t = object_desc_int(t, object_ptr->pval);

			/* Do not display the "pval" flags */
			if(have_flag(flgs, TRAIT_HIDE_TYPE))
			{
				/* Nothing */
			}

			/* Speed */
			else if(have_flag(flgs, TRAIT_SPEED))
			{
				/* Dump " to speed" */
#ifdef JP
				t = object_desc_str(t, "加速");
#else
				t = object_desc_str(t, " to speed");
#endif
			}

			/* Attack speed */
			else if(has_trait_object(object_ptr, TRAIT_BLOWS))
			{
				/* Add " attack" */
#ifdef JP
				t = object_desc_str(t, "攻撃");
#else
				t = object_desc_str(t, " attack");

				/* Add "attacks" */
				if(ABS(object_ptr->pval) != 1) t = object_desc_chr(t, 's');
#endif
			}

			/* Stealth */
			else if(has_trait_object(object_ptr, TRAIT_STEALTH))
			{
				/* Dump " to stealth" */
#ifdef JP
				t = object_desc_str(t, "隠密");
#else
				t = object_desc_str(t, " to stealth");
#endif
			}

			else if(have_flag(flgs, TRAIT_SEARCH))
			{
				/* Dump " to searching" */
#ifdef JP
				t = object_desc_str(t, "探索");
#else
				t = object_desc_str(t, " to searching");
#endif
			}

			/* Infravision */
			else if(have_flag(flgs, TRAIT_INFRA))
			{
				/* Dump " to infravision" */
#ifdef JP
				t = object_desc_str(t, "赤外線視力");
#else
				t = object_desc_str(t, " to infravision");
#endif
			}

			/* Finish the display */
			t = object_desc_chr(t, p2);
		}

		/* Hack -- Process Lanterns/Torches */
		if(object_ptr->tval == TV_LITE && !have_flag(object_kind_ptr->flags, TRAIT_NO_LIMIT_LITE))
		{
			/* Hack -- Turns of light for normal lites */
#ifdef JP
			t = object_desc_chr(t, '(');
#else
			t = object_desc_str(t, " (with ");
#endif

			if(object_ptr->name2 == EGO_LITE_LONG) t = object_desc_num(t, object_ptr->fuel * 2);
			else t = object_desc_num(t, object_ptr->fuel);
#ifdef JP
			t = object_desc_str(t, "ターンの寿命)");
#else
			t = object_desc_str(t, " turns of light)");
#endif
		}

		/* Indicate charging objects, but not rods. */
		if(object_ptr->timeout && (object_ptr->tval != TV_ROD))
		{
			/* Hack -- Dump " (charging)" if relevant */
#ifdef JP
			t = object_desc_str(t, "(充填中)");
#else
			t = object_desc_str(t, " (charging)");
#endif
		}
	}


	/* No more details wanted */
	if(mode & OD_OMIT_INSCRIPTION) goto object_desc_done;


	/* Prepare real inscriptions in a buffer */
	tmp_val2[0] = '\0';

	/* Auto abbreviation inscribe */
	if((abbrev_extra || abbrev_all) && (object_ptr->ident & IDENT_MENTAL))
	{
		if(!object_ptr->inscription || !my_strchr(quark_str(object_ptr->inscription), '%'))
		{
			bool kanji, all;

#ifdef JP
			kanji = TRUE;
#else
			kanji = FALSE;
#endif
			all = abbrev_all;

			get_ability_abbreviation(tmp_val2, object_ptr, kanji, all);
		}
	}

	/* Use the standard inscription if available */
	if(object_ptr->inscription)
	{
		char buff[1024];

		if(tmp_val2[0]) strcat(tmp_val2, ", ");

		/* Get inscription and convert {%} */
		get_inscription(buff, object_ptr);

		/* strcat with correct treating of kanji */
		my_strcat(tmp_val2, buff, sizeof(tmp_val2));
	}


	/* No fake inscription yet */
	fake_insc_buf[0] = '\0';

	/* Use the game-generated "feeling" otherwise, if available */
	if(object_ptr->feeling)
	{
		strcpy(fake_insc_buf, game_inscriptions[object_ptr->feeling]);
	}

	else if(object_is_cursed(object_ptr) && (known || (object_ptr->ident & IDENT_SENSE)))
		strcpy(fake_insc_buf, KEYWORD_CURSED);

	else if(((object_ptr->tval == TV_RING) || (object_ptr->tval == TV_AMULET)
		   || (object_ptr->tval == TV_LITE) || (object_ptr->tval == TV_FIGURINE))
		 && aware && !known && !(object_ptr->ident & IDENT_SENSE))
		strcpy(fake_insc_buf, KW_UNIDENTIFIED);

	/* Mega-Hack -- note empty wands/staffs */
	else if(!known && (object_ptr->ident & IDENT_EMPTY))
		strcpy(fake_insc_buf, KW_EMPTY);

	else if(!aware && object_is_tried(object_ptr))
		strcpy(fake_insc_buf, KW_TRIED);

	if(object_ptr->discount)
	{
		/* Hidden by real inscription unless in a store */
		if(!tmp_val2[0] || (object_ptr->ident & IDENT_STORE))
		{
			char discount_num_buf[4];

			/* Append to other fake inscriptions if any */
			if(fake_insc_buf[0]) strcat(fake_insc_buf, ", ");
			(void)object_desc_num(discount_num_buf, object_ptr->discount);
			strcat(fake_insc_buf, discount_num_buf);
			strcat(fake_insc_buf, KW_PRICE_OFF);
		}
	}


	/* Append the inscription, if any */
	if(fake_insc_buf[0] || tmp_val2[0])
	{
		/* Append the inscription */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, c1);

		/* Append fake inscriptions */
		if(fake_insc_buf[0])
		{
			t = object_desc_str(t, fake_insc_buf);
		}

		/* Append a separater */
		if(fake_insc_buf[0] && tmp_val2[0])
		{
			t = object_desc_chr(t, ',');
			t = object_desc_chr(t, ' ');
		}

		/* Append real inscriptions */
		if(tmp_val2[0])
		{
			t = object_desc_str(t, tmp_val2);
		}

		t = object_desc_chr(t, c2);
	}

object_desc_done:
	my_strcpy(buf, tmp_val, MAX_NLEN);
}


