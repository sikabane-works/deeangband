/* Purpose: Object flavor code */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * Certain items, if aware, are known instantly
 * This function is used only by "flavor_init()"
 */
static bool object_easy_know(int i)
{
	object_kind *k_ptr = &object_kind_info[i];

	/* Analyze the "tval" */
	switch (k_ptr->tval)
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
			return (TRUE);
		}

		/* Simple items */
		case TV_FLASK:
		case TV_JUNK:
		case TV_BOTTLE:
		case TV_SKELETON:
		case TV_SPIKE:
		case TV_WHISTLE:
		{
			return (TRUE);
		}

		/* All Food, Potions, Scrolls, Rods */
		case TV_FOOD:
		case TV_POTION:
		case TV_SCROLL:
		case TV_ROD:
		{
			return (TRUE);
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * Create a name from random parts.
 */
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

	if (randint1(3) == 2)
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
			(void)get_rnd_line("elvish.txt", 0, Syllable);
			strcat(out_string, Syllable);
		}
	}

	out_string[0] = toupper(out_string[1]);

	out_string[16] = '\0';
#endif
}


/*
 * Create a name from random parts with quotes.
 */
void get_table_name(char *out_string)
{
	char buff[80];
	get_table_name_aux(buff);

#ifdef JP
	sprintf(out_string, "�w%s�x", buff);
#else
	sprintf(out_string, "'%s'", buff);
#endif
}


/*
 * Make random Sindarin name
 */
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
	sprintf(out_string, "�w%s�x", buff);
#else
	sprintf(out_string, "'%s'", buff);
#endif
}


/*
 * Shuffle flavor indices of a group of objects with given tval
 */
static void shuffle_flavors(byte tval)
{
	s16b *k_idx_list;
	int k_idx_list_num = 0;
	int i;

	/* Allocate an array for a list of k_idx */
	C_MAKE(k_idx_list, max_object_kind_idx, s16b);

	/* Search objects with given tval for shuffle */
	for (i = 0; i < max_object_kind_idx; i++)
	{
		object_kind *k_ptr = &object_kind_info[i];

		/* Skip non-Rings */
		if (k_ptr->tval != tval) continue;

		/* Paranoia -- Skip objects without flavor */
		if (!k_ptr->flavor) continue;

		/* Skip objects with a fixed flavor name */
		if (have_flag(k_ptr->flags, TR_FIXED_FLAVOR)) continue;

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
	C_KILL(k_idx_list, max_object_kind_idx, s16b);
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
		object_kind *k_ptr = &object_kind_info[i];

		/* Skip objects without flavor name */
		if (!k_ptr->flavospecies_name) continue;

		/*
		 * Initialize flavor index to itself
		 *  -> Shuffle it later
		 */
		k_ptr->flavor = i;
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
		object_kind *k_ptr = &object_kind_info[i];

		/* Skip "empty" objects */
		if (!k_ptr->name) continue;

		/* No flavor yields aware */
		if (!k_ptr->flavor) k_ptr->aware = TRUE;

		/* Check for "easily known" */
		k_ptr->easy_know = object_easy_know(i);
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

	/* Terminate */
	*t = '\0';

	/* Result */
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

	/* Terminate */
	*t = '\0';

	/* Result */
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
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}




#ifdef JP
/*
 * ���{��̌��\�����[�`��
 *�icmd1.c �ŗ��p���邽�߂� object_desc_japanese ����ړ������B�j
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
	  t = object_desc_str(t, "�{");
	  break;
      }
      case TV_SCROLL:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_POTION:
      {
	  t = object_desc_str(t, "��");
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
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_SOFT_ARMOR:
      case TV_HARD_ARMOR:
      case TV_DRAG_ARMOR:
      case TV_CLOAK:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_SWORD:
      case TV_HAFTED:
      case TV_BOW:
      {
	  t = object_desc_str(t, "�U");
	  break;
      }
      case TV_BOOTS:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_CARD:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
	    /* �H�ׂ��� by ita */
      case TV_FOOD:
      {
	  if(object_ptr->sval == SV_FOOD_JERKY)
	  {
	      t = object_desc_str(t, "�؂�");
	      break;
	  }
      }
      default:
      {
	  if (object_ptr->number < 10)
	  {
	      t = object_desc_str(t, "��");
	  }
	  else
	  {
	      t = object_desc_str(t, "��");
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
	if (v < 0)
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
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
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
	{ "�U", "At", TR_BLOWS, -1 },
	{ "��", "Sp", TR_SPEED, -1 },
	{ "�r", "St", TR_STR, -1 },
	{ "�m", "In", TR_INT, -1 },
	{ "��", "Wi", TR_WIS, -1 },
	{ "��", "Dx", TR_DEX, -1 },
	{ "��", "Cn", TR_CON, -1 },
	{ "��", "Ch", TR_CHR, -1 },
	{ "��", "Md", TR_MAGIC_MASTERY, -1 },
	{ "�B", "Sl", TR_STEALTH, -1 },
	{ "�T", "Sr", TR_SEARCH, -1 },
	{ "��", "If", TR_INFRA, -1 },
	{ "�@", "Dg", TR_TUNNEL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
	{ "�_", "Ac", TR_IM_ACID, -1 },
	{ "�d", "El", TR_IM_ELEC, -1 },
	{ "��", "Fi", TR_IM_FIRE, -1 },
	{ "��", "Co", TR_IM_COLD, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_resistance[] =
{
	{ "�_", "Ac", TR_RES_ACID, TR_IM_ACID },
	{ "�d", "El", TR_RES_ELEC, TR_IM_ELEC },
	{ "��", "Fi", TR_RES_FIRE, TR_IM_FIRE },
	{ "��", "Co", TR_RES_COLD, TR_IM_COLD },
	{ "��", "Po", TR_RES_POIS, -1 },
	{ "�M", "Li", TR_RES_LITE, -1 },
	{ "��", "Dk", TR_RES_DARK, -1 },
	{ "�j", "Sh", TR_RES_SHARDS, -1 },
	{ "��", "Bl", TR_RES_BLIND, -1 },
	{ "��", "Cf", TR_RES_CONF, -1 },
	{ "��", "So", TRAIT_RES_SOUN, -1 },
	{ "��", "Nt", TRAIT_RES_NETH, -1 },
	{ "��", "Nx", TRAIT_RES_NEXU, -1 },
	{ "��", "Ca", TRAIT_RES_CHAO, -1 },
	{ "��", "Di", TRAIT_RES_DISE, -1 },
	{ "��", "Fe", TR_RES_FEAR, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_misc[] =
{
	{ "��", "Es", TRAIT_EASY_SPELL, -1 },
	{ "��", "Dm", TR_DEC_MANA, -1 },
	{ "��", "Th", TR_THROW, -1 },
	{ "��", "Rf", TRAIT_REFLECTING, -1 },
	{ "��", "Fa", TR_FREE_ACT, -1 },
	{ "��", "Si", TR_SEE_INVIS, -1 },
	{ "�o", "Hl", TR_HOLD_LIFE, -1 },
	{ "�x", "Sd", TR_SLOW_DIGEST, -1 },
	{ "��", "Rg", TR_REGEN, -1 },
	{ "��", "Lv", TR_LEVITATION, -1 },
	{ "��", "Lu", TR_LITE, -1 },
	{ "�x", "Wr", TR_WARNING, -1 },
	{ "�{", "Xm", TR_XTRA_MIGHT, -1 },
	{ "��", "Xs", TR_XTRA_SHOTS, -1 },
	{ "�u", "Te", TR_TELEPORT, -1 },
	{ "�{", "Ag", TR_AGGRAVATE, -1 },
	{ "�j", "Bs", TR_BLESSED, -1 },
	{ "��", "Ty", TR_TY_CURSE, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
	{ "��", "F", TR_SH_FIRE, -1 },
	{ "�d", "E", TR_SH_ELEC, -1 },
	{ "��", "C", TR_SH_COLD, -1 },
	{ "��", "M", TR_NO_MAGIC, -1 },
	{ "�u", "T", TR_NO_TELE, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_brand[] =
{
	{ "�_", "A", TR_BRAND_ACID, -1 },
	{ "�d", "E", TR_BRAND_ELEC, -1 },
	{ "��", "F", TR_BRAND_FIRE, -1 },
	{ "��", "Co", TR_BRAND_COLD, -1 },
	{ "��", "P", TR_BRAND_POIS, -1 },
	{ "��", "Ca", TR_CHAOTIC, -1 },
	{ "�z", "V", TR_VAMPIRIC, -1 },
	{ "�k", "Q", TR_IMPACT, -1 },
	{ "��", "S", TR_VORPAL, -1 },
	{ "��", "M", TR_FORCE_WEAPON, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
	{ "��", "*", TR_KILL_EVIL, -1 },
	{ "�P", "#", TR_KILL_GOOD, -1 },
	{ "�l", "p", TR_KILL_HUMAN, -1 },
	{ "��", "D", TR_KILL_DRAGON, -1 },
	{ "�I", "o", TR_KILL_ORC, -1 },
	{ "�g", "T", TR_KILL_TROLL, -1 },
	{ "��", "P", TR_KILL_GIANT, -1 },
	{ "�f", "U", TR_KILL_DEMON, -1 },
	{ "��", "L", TR_KILL_UNDEAD, -1 },
	{ "��", "Z", TR_KILL_ANIMAL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
	{ "��", "*", TR_SLAY_EVIL, TR_KILL_EVIL },
	{ "��", "#", TR_SLAY_EVIL, TR_KILL_GOOD },
	{ "�l", "p", TR_SLAY_HUMAN, TR_KILL_HUMAN },
	{ "��", "D", TR_SLAY_DRAGON, TR_KILL_DRAGON },
	{ "�I", "o", TR_SLAY_ORC, TR_KILL_ORC },
	{ "�g", "T", TR_SLAY_TROLL, TR_KILL_TROLL },
	{ "��", "P", TR_SLAY_GIANT, TR_KILL_GIANT },
	{ "�f", "U", TR_SLAY_DEMON, TR_KILL_DEMON },
	{ "��", "L", TR_SLAY_UNDEAD, TR_KILL_UNDEAD },
	{ "��", "Z", TR_SLAY_ANIMAL, TR_KILL_ANIMAL },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
	{ "��", "Tele", TR_TELEPATHY, -1 },
	{ "��", "Evil", TR_ESP_EVIL, -1 },
	{ "�P", "Good", TR_ESP_GOOD, -1 },
	{ "��", "Nolv", TR_ESP_NONLIVING, -1 },
	{ "��", "Uniq", TR_ESP_UNIQUE, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
	{ "�l", "p", TR_ESP_HUMAN, -1 },
	{ "��", "D", TR_ESP_DRAGON, -1 },
	{ "�I", "o", TR_ESP_ORC, -1 },
	{ "�g", "T", TR_ESP_TROLL, -1 },
	{ "��", "P", TR_ESP_GIANT, -1 },
	{ "�f", "U", TR_ESP_DEMON, -1 },
	{ "��", "L", TR_ESP_UNDEAD, -1 },
	{ "��", "Z", TR_ESP_ANIMAL, -1 },
	{ NULL, NULL, 0, -1 }
};

static flag_insc_table flag_insc_sust[] =
{
	{ "�r", "St", TRAIT_SUSTAIN_STR, -1 },
	{ "�m", "In", TRAIT_SUSTAIN_INT, -1 },
	{ "��", "Wi", TRAIT_SUSTAIN_WIS, -1 },
	{ "��", "Dx", TRAIT_SUSTAIN_DEX, -1 },
	{ "��", "Cn", TRAIT_SUSTAIN_CON, -1 },
	{ "��", "Ch", TRAIT_SUSTAIN_CHR, -1 },
	{ NULL, NULL, 0, -1 }
};

#else
static flag_insc_table flag_insc_plus[] =
{
	{ "At", TR_BLOWS, -1 },
	{ "Sp", TR_SPEED, -1 },
	{ "St", TR_STR, -1 },
	{ "In", TR_INT, -1 },
	{ "Wi", TR_WIS, -1 },
	{ "Dx", TR_DEX, -1 },
	{ "Cn", TR_CON, -1 },
	{ "Ch", TR_CHR, -1 },
	{ "Md", TR_MAGIC_MASTERY, -1 },
	{ "Sl", TR_STEALTH, -1 },
	{ "Sr", TR_SEARCH, -1 },
	{ "If", TR_INFRA, -1 },
	{ "Dg", TR_TUNNEL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
	{ "Ac", TR_IM_ACID, -1 },
	{ "El", TR_IM_ELEC, -1 },
	{ "Fi", TR_IM_FIRE, -1 },
	{ "Co", TR_IM_COLD, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_resistance[] =
{
	{ "Ac", TR_RES_ACID, TR_IM_ACID },
	{ "El", TR_RES_ELEC, TR_IM_ELEC },
	{ "Fi", TR_RES_FIRE, TR_IM_FIRE },
	{ "Co", TR_RES_COLD, TR_IM_COLD },
	{ "Po", TR_RES_POIS, -1 },
	{ "Li", TR_RES_LITE, -1 },
	{ "Dk", TR_RES_DARK, -1 },
	{ "Sh", TR_RES_SHARDS, -1 },
	{ "Bl", TR_RES_BLIND, -1 },
	{ "Cf", TR_RES_CONF, -1 },
	{ "So", TRAIT_RES_SOUN, -1 },
	{ "Nt", TRAIT_RES_NETH, -1 },
	{ "Nx", TRAIT_RES_NEXU, -1 },
	{ "Ca", TRAIT_RES_CHAO, -1 },
	{ "Di", TRAIT_RES_DISE, -1 },
	{ "Fe", TR_RES_FEAR, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_misc[] =
{
	{ "Es", TRAIT_EASY_SPELL, -1 },
	{ "Dm", TR_DEC_MANA, -1 },
	{ "Th", TR_THROW, -1 },
	{ "Rf", TRAIT_REFLECTING, -1 },
	{ "Fa", TR_FREE_ACT, -1 },
	{ "Si", TR_SEE_INVIS, -1 },
	{ "Hl", TR_HOLD_LIFE, -1 },
	{ "Sd", TR_SLOW_DIGEST, -1 },
	{ "Rg", TR_REGEN, -1 },
	{ "Lv", TR_LEVITATION, -1 },
	{ "Lu", TR_LITE, -1 },
	{ "Wr", TR_WARNING, -1 },
	{ "Xm", TR_XTRA_MIGHT, -1 },
	{ "Xs", TR_XTRA_SHOTS, -1 },
	{ "Te", TR_TELEPORT, -1 },
	{ "Ag", TR_AGGRAVATE, -1 },
	{ "Bs", TR_BLESSED, -1 },
	{ "Ty", TR_TY_CURSE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
	{ "F", TR_SH_FIRE, -1 },
	{ "E", TR_SH_ELEC, -1 },
	{ "C", TR_SH_COLD, -1 },
	{ "M", TR_NO_MAGIC, -1 },
	{ "T", TR_NO_TELE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_brand[] =
{
	{ "A", TR_BRAND_ACID, -1 },
	{ "E", TR_BRAND_ELEC, -1 },
	{ "F", TR_BRAND_FIRE, -1 },
	{ "Co", TR_BRAND_COLD, -1 },
	{ "P", TR_BRAND_POIS, -1 },
	{ "Ca", TR_CHAOTIC, -1 },
	{ "V", TR_VAMPIRIC, -1 },
	{ "Q", TR_IMPACT, -1 },
	{ "S", TR_VORPAL, -1 },
	{ "M", TR_FORCE_WEAPON, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
	{ "*", TR_KILL_EVIL, -1 },
	{ "#", TR_KILL_GOOD, -1 },
	{ "p", TR_KILL_HUMAN, -1 },
	{ "D", TR_KILL_DRAGON, -1 },
	{ "o", TR_KILL_ORC, -1 },
	{ "T", TR_KILL_TROLL, -1 },
	{ "P", TR_KILL_GIANT, -1 },
	{ "U", TR_KILL_DEMON, -1 },
	{ "L", TR_KILL_UNDEAD, -1 },
	{ "Z", TR_KILL_ANIMAL, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
	{ "*", TR_SLAY_EVIL, TR_KILL_EVIL },
	{ "#", TR_SLAY_GOOD, TR_KILL_GOOD },
	{ "p", TR_SLAY_HUMAN, TR_KILL_HUMAN },
	{ "D", TR_SLAY_DRAGON, TR_KILL_DRAGON },
	{ "o", TR_SLAY_ORC, TR_KILL_ORC },
	{ "T", TR_SLAY_TROLL, TR_KILL_TROLL },
	{ "P", TR_SLAY_GIANT, TR_KILL_GIANT },
	{ "U", TR_SLAY_DEMON, TR_KILL_DEMON },
	{ "L", TR_SLAY_UNDEAD, TR_KILL_UNDEAD },
	{ "Z", TR_SLAY_ANIMAL, TR_KILL_ANIMAL },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
	{ "Tele", TR_TELEPATHY, -1 },
	{ "Evil", TR_ESP_EVIL, -1 },
	{ "Good", TR_ESP_GOOD, -1 },
	{ "Nolv", TR_ESP_NONLIVING, -1 },
	{ "Uniq", TR_ESP_UNIQUE, -1 },
	{ NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
	{ "p", TR_ESP_HUMAN, -1 },
	{ "D", TR_ESP_DRAGON, -1 },
	{ "o", TR_ESP_ORC, -1 },
	{ "T", TR_ESP_TROLL, -1 },
	{ "P", TR_ESP_GIANT, -1 },
	{ "U", TR_ESP_DEMON, -1 },
	{ "L", TR_ESP_UNDEAD, -1 },
	{ "Z", TR_ESP_ANIMAL, -1 },
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
static char *inscribe_flags_aux(flag_insc_table *fi_ptr, u32b flgs[TR_FLAG_SIZE], bool kanji, char *ptr)
{
#ifndef JP
	(void)kanji;
#endif

	while (fi_ptr->english)
	{
		if (have_flag(flgs, fi_ptr->flag) &&
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
static bool have_flag_of(flag_insc_table *fi_ptr, u32b flgs[TR_FLAG_SIZE])
{
	while (fi_ptr->english)
	{
		if (have_flag(flgs, fi_ptr->flag) &&
		   (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
			return (TRUE);
		fi_ptr++;
	}

	return (FALSE);
}

static char *get_ability_abbreviation(char *ptr, object_type *object_ptr, bool kanji, bool all)
{
	char *prev_ptr = ptr;
	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(object_ptr, flgs);


	/* Remove obvious flags */
	if (!all)
	{
		object_kind *k_ptr = &object_kind_info[object_ptr->k_idx];
		int j;
				
		/* Base object */
		for (j = 0; j < TR_FLAG_SIZE; j++)
			flgs[j] &= ~k_ptr->flags[j];

		if (object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];
					
			for (j = 0; j < TR_FLAG_SIZE; j++)
				flgs[j] &= ~a_ptr->flags[j];
		}

		if (object_is_ego(object_ptr))
		{
			ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];
					
			for (j = 0; j < TR_FLAG_SIZE; j++)
				flgs[j] &= ~e_ptr->flags[j];
		}
	}


	/* Plusses */
	if (have_flag_of(flag_insc_plus, flgs))
	{
		if (kanji)
			ADD_INSC("+");
	}
	ptr = inscribe_flags_aux(flag_insc_plus, flgs, kanji, ptr);

	/* Immunity */
	if (have_flag_of(flag_insc_immune, flgs))
	{
		if (!kanji && ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
		ADD_INSC("*");
	}
	ptr = inscribe_flags_aux(flag_insc_immune, flgs, kanji, ptr);

	/* Resistance */
	if (have_flag_of(flag_insc_resistance, flgs))
	{
		if (kanji)
			ADD_INSC("r");
		else if (ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
	}
	ptr = inscribe_flags_aux(flag_insc_resistance, flgs, kanji, ptr);

	/* Misc Ability */
	if (have_flag_of(flag_insc_misc, flgs))
	{
		if (ptr != prev_ptr)
		{
			ADD_INSC(";");
			prev_ptr = ptr;
		}
	}
	ptr = inscribe_flags_aux(flag_insc_misc, flgs, kanji, ptr);

	/* Aura */
	if (have_flag_of(flag_insc_aura, flgs))
	{
		ADD_INSC("[");
	}
	ptr = inscribe_flags_aux(flag_insc_aura, flgs, kanji, ptr);

	/* Brand Weapon */
	if (have_flag_of(flag_insc_brand, flgs))
		ADD_INSC("|");
	ptr = inscribe_flags_aux(flag_insc_brand, flgs, kanji, ptr);

	/* Kill Weapon */
	if (have_flag_of(flag_insc_kill, flgs))
		ADD_INSC("/X");
	ptr = inscribe_flags_aux(flag_insc_kill, flgs, kanji, ptr);

	/* Slay Weapon */
	if (have_flag_of(flag_insc_slay, flgs))
		ADD_INSC("/");
	ptr = inscribe_flags_aux(flag_insc_slay, flgs, kanji, ptr);

	/* Esp */
	if (kanji)
	{
		if (have_flag_of(flag_insc_esp1, flgs) ||
		    have_flag_of(flag_insc_esp2, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp1, flgs, kanji, ptr);
		ptr = inscribe_flags_aux(flag_insc_esp2, flgs, kanji, ptr);
	}
	else
	{
		if (have_flag_of(flag_insc_esp1, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp1, flgs, kanji, ptr);
		if (have_flag_of(flag_insc_esp2, flgs))
			ADD_INSC("~");
		ptr = inscribe_flags_aux(flag_insc_esp2, flgs, kanji, ptr);
	}

	/* sustain */
	if (have_flag_of(flag_insc_sust, flgs))
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
	if (!(object_ptr->ident & IDENT_MENTAL))
	{
		/* Copy until end of line or '#' */
		while (*insc)
		{
			if (*insc == '#') break;
#ifdef JP
			if (iskanji(*insc)) *buff++ = *insc++;
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
		if (*insc == '#') break;

		/* {%} will be automatically converted */
		else if ('%' == *insc)
		{
			bool kanji = FALSE;
			bool all;
			cptr start = ptr;

			/* check for too long inscription */
			if (ptr >= buff + MAX_NLEN) continue;

#ifdef JP
			if ('%' == insc[1])
			{
				insc++;
				kanji = FALSE;
			}
			else
			{
				kanji = TRUE;
			}
#endif
			if ('a' == insc[1] && 'l' == insc[2] && 'l' == insc[3])
			{
				all = TRUE;
				insc += 3;
			}
			else
			{
				all = FALSE;
			}

			ptr = get_ability_abbreviation(ptr, object_ptr, kanji, all);

			if (ptr == start)
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
void object_desc(char *buf, object_type *object_ptr, u32b mode)
{
	/* Extract object kind name */
	cptr            kindname = object_kind_name + object_kind_info[object_ptr->k_idx].name;

	/* Extract default "base" string */
	cptr            basenm = kindname;

	/* Assume no "modifier" string */
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

	u32b flgs[TR_FLAG_SIZE];

	//object_type *bow_ptr;

	object_kind *k_ptr = &object_kind_info[object_ptr->k_idx];
	object_kind *flavor_k_ptr = &object_kind_info[k_ptr->flavor];

	/* Extract some flags */
	object_flags(object_ptr, flgs);

	/* See if the object is "aware" */
	if (object_is_aware(object_ptr)) aware = TRUE;

	/* See if the object is "known" */
	if (object_is_known(object_ptr)) known = TRUE;

	/* Allow flavors to be hidden when aware */
	if (aware && ((mode & OD_NO_FLAVOR) || plain_descriptions)) flavor = FALSE;

	/* Object is in the inventory of a store or spoiler */
	if ((mode & OD_STORE) || (object_ptr->ident & IDENT_STORE))
	{
		/* Don't show flavors */
		flavor = FALSE;

		/* Pretend known and aware */
		aware = TRUE;
		known = TRUE;
	}

	/* Force to be flavor name only */
	if (mode & OD_FORCE_FLAVOR)
	{
		aware = FALSE;
		flavor = TRUE;
		known = FALSE;

		/* Cancel shuffling */
		flavor_k_ptr = k_ptr;
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
			species_type *r_ptr = &species_info[object_ptr->pval];

			if (known)
			{
				if (!object_ptr->pval)
				{
#ifdef JP
					modstr = " (��)";
#else
					modstr = " (empty)";
#endif
				}
				else
				{
#ifdef JP
					sprintf(tmp_val2, " (%s)",species_name + r_ptr->name);
					modstr = tmp_val2;
#else
					cptr t = species_name + r_ptr->name;

					if (!is_unique_species(r_ptr))
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
			species_type *r_ptr = &species_info[object_ptr->pval];

#ifdef JP
			modstr = species_name + r_ptr->name;
#else
			cptr t = species_name + r_ptr->name;

			if (!is_unique_species(r_ptr))
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
			species_type *r_ptr = &species_info[object_ptr->pval];

			modstr = species_name + r_ptr->name;

#ifdef JP
			basenm = "#%";
#else
			if (is_unique_species(r_ptr))
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
			if (aware)
			{
				if (object_is_fixed_artifact(object_ptr)) break;
				if (k_ptr->gen_flags & TRG_INSTA_ART) break;
			}

			/* Color the object */
			modstr = object_kind_name + flavor_k_ptr->flavospecies_name;

#ifdef JP
			if (!flavor)    basenm = "%�̃A�~�����b�g";
			else if (aware) basenm = "%��#�A�~�����b�g";
			else            basenm = "#�A�~�����b�g";
#else
			if (!flavor)    basenm = "& Amulet~ of %";
			else if (aware) basenm = "& # Amulet~ of %";
			else            basenm = "& # Amulet~";
#endif

			break;
		}

		/* Rings (including a few "Specials") */
		case TV_RING:
		{
			/* Known artifacts */
			if (aware)
			{
				if (object_is_fixed_artifact(object_ptr)) break;
				if (k_ptr->gen_flags & TRG_INSTA_ART) break;
			}

			/* Color the object */
			modstr = object_kind_name + flavor_k_ptr->flavospecies_name;

#ifdef JP
			if (!flavor)    basenm = "%�̎w��";
			else if (aware) basenm = "%��#�w��";
			else            basenm = "#�w��";
#else
			if (!flavor)    basenm = "& Ring~ of %";
			else if (aware) basenm = "& # Ring~ of %";
			else            basenm = "& # Ring~";
#endif

			if (!k_ptr->to_hit && !k_ptr->to_damage && (object_ptr->to_hit || object_ptr->to_damage)) show_weapon = TRUE;

			break;
		}

		case TV_CARD:
		{
			break;
		}

		case TV_STAFF:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_k_ptr->flavospecies_name;

#ifdef JP
			if (!flavor)    basenm = "%�̏�";
			else if (aware) basenm = "%��#��";
			else            basenm = "#��";
#else
			if (!flavor)    basenm = "& Staff~ of %";
			else if (aware) basenm = "& # Staff~ of %";
			else            basenm = "& # Staff~";
#endif

			break;
		}

		case TV_WAND:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_k_ptr->flavospecies_name;

#ifdef JP
			if (!flavor)    basenm = "%�̖��@�_";
			else if (aware) basenm = "%��#���@�_";
			else            basenm = "#���@�_";
#else
			if (!flavor)    basenm = "& Wand~ of %";
			else if (aware) basenm = "& # Wand~ of %";
			else            basenm = "& # Wand~";
#endif

			break;
		}

		case TV_ROD:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_k_ptr->flavospecies_name;

#ifdef JP
			if (!flavor)    basenm = "%�̃��b�h";
			else if (aware) basenm = "%��#���b�h";
			else            basenm = "#���b�h";
#else
			if (!flavor)    basenm = "& Rod~ of %";
			else if (aware) basenm = "& # Rod~ of %";
			else            basenm = "& # Rod~";
#endif

			break;
		}

		case TV_SCROLL:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_k_ptr->flavospecies_name;

#ifdef JP
			if (!flavor)    basenm = "%�̊���";
			else if (aware) basenm = "�u#�v�Ə����ꂽ%�̊���";
			else            basenm = "�u#�v�Ə����ꂽ����";
#else
			if (!flavor)    basenm = "& Scroll~ of %";
			else if (aware) basenm = "& Scroll~ titled \"#\" of %";
			else            basenm = "& Scroll~ titled \"#\"";
#endif

			break;
		}

		case TV_POTION:
		{
			/* Color the object */
			modstr = object_kind_name + flavor_k_ptr->flavospecies_name;

#ifdef JP
			if (!flavor)    basenm = "%�̖�";
			else if (aware) basenm = "%��#��";
			else            basenm = "#��";
#else
			if (!flavor)    basenm = "& Potion~ of %";
			else if (aware) basenm = "& # Potion~ of %";
			else            basenm = "& # Potion~";
#endif

			break;
		}

		case TV_FOOD:
		{
			/* Ordinary food is "boring" */
			if (!k_ptr->flavospecies_name) break;

			/* Color the object */
			modstr = object_kind_name + flavor_k_ptr->flavospecies_name;

#ifdef JP
			if (!flavor)    basenm = "%�̃L�m�R";
			else if (aware) basenm = "%��#�L�m�R";
			else            basenm = "#�L�m�R";
#else
			if (!flavor)    basenm = "& Mushroom~ of %";
			else if (aware) basenm = "& # Mushroom~ of %";
			else            basenm = "& # Mushroom~";
#endif

			break;
		}

		case TV_PARCHMENT:
		{
#ifdef JP
			basenm = "�r�玆 - %";
#else
			basenm = "& Parchment~ - %";
#endif
			break;
		}

		/* Magic Books */
		case TV_LIFE_BOOK:
		{
#ifdef JP
			basenm = "�����̖��@��%";
#else
			basenm = "& Life Spellbook~ %";
#endif

			break;
		}

		case TV_SORCERY_BOOK:
		{
#ifdef JP
			basenm = "��p�̖��@��%";
#else
			basenm = "& Sorcery Spellbook~ %";
#endif

			break;
		}

		case TV_NATURE_BOOK:
		{
#ifdef JP
			basenm = "���R�̖��@��%";
#else
			basenm = "& Nature Spellbook~ %";
#endif

			break;
		}

		case TV_CHAOS_BOOK:
		{
#ifdef JP
			basenm = "�J�I�X�̖��@��%";
#else
			basenm = "& Chaos Spellbook~ %";
#endif

			break;
		}

		case TV_DEATH_BOOK:
		{
#ifdef JP
			basenm = "�Í��̖��@��%";
#else
			basenm = "& Death Spellbook~ %";
#endif

			break;
		}

		case TV_TRUMP_BOOK:
		{
#ifdef JP
			basenm = "�g�����v�̖��@��%";
#else
			basenm = "& Trump Spellbook~ %";
#endif

			break;
		}

		case TV_ARCANE_BOOK:
		{
#ifdef JP
			basenm = "��p�̖��@��%";
#else
			basenm = "& Arcane Spellbook~ %";
#endif

			break;
		}

		case TV_CRAFT_BOOK:
		{
#ifdef JP
			basenm = "���̖��@��%";
#else
			basenm = "& Craft Spellbook~ %";
#endif

			break;
		}

		case TV_DAEMON_BOOK:
		{
#ifdef JP
			basenm = "�����̖��@��%";
#else
			basenm = "& Daemon Spellbook~ %";
#endif

			break;
		}

		case TV_CRUSADE_BOOK:
		{
#ifdef JP
			basenm = "�j�ׂ̖��@��%";
#else
			basenm = "& Crusade Spellbook~ %";
#endif

			break;
		}

		case TV_MUSIC_BOOK:
		{
#ifdef JP
			basenm = "�̏W%";
#else
			basenm = "& Song Book~ %";
#endif

			break;
		}

		case TV_HISSATSU_BOOK:
		{
#ifdef JP
			basenm = "& ���|�̏�%";
#else
			basenm = "Book~ of Kendo %";
#endif

			break;
		}

		case TV_HEX_BOOK:
		{
#ifdef JP
			basenm = "��p�̖��@��%";
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
			strcpy(buf, "(�Ȃ�)");
#else
			strcpy(buf, "(nothing)");
#endif

			return;
		}
	}

	/* Use full name from object_kind_info or artifact_info */
	if (aware && have_flag(flgs, TR_FULL_NAME))
	{
		if (known && object_ptr->name1) basenm = artifact_name + artifact_info[object_ptr->name1].name;
		else basenm = kindname;
	}

	/* Start dumping the result */
	t = tmp_val;

#ifdef JP
	if (basenm[0] == '&')
		s = basenm + 2;
	else
		s = basenm;

	/* No prefix */
	if (mode & OD_OMIT_PREFIX)
	{
		/* Nothing */
	}
	else if (object_ptr->number > 1)
	{
		t = object_desc_kosuu(t, object_ptr);
		t = object_desc_str(t, "�� ");
	}

	/* �p��̏ꍇ�A�[�e�B�t�@�N�g�� The ���t���̂ŕ����邪
	 * ���{��ł͕�����Ȃ��̂Ń}�[�N������ 
	 */
	if (known)
	{
		if (object_is_fixed_artifact(object_ptr)) t = object_desc_str(t, "��");
		else if (object_ptr->art_name) t = object_desc_str(t, "��");
	}

#else

	/* The object "expects" a "number" */
	if (basenm[0] == '&')
	{
		/* Skip the ampersand (and space) */
		s = basenm + 2;

		/* No prefix */
		if (mode & OD_OMIT_PREFIX)
		{
			/* Nothing */
		}

		/* Hack -- None left */
		else if (object_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Extract the number */
		else if (object_ptr->number > 1)
		{
			t = object_desc_num(t, object_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if ((known && object_is_artifact(object_ptr)) ||
		         ((object_ptr->tval == TV_CORPSE) &&
		          (is_unique_species(species_info[object_ptr->pval]))))
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

			if (vowel)
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
		if (mode & OD_OMIT_PREFIX)
		{
			/* Nothing */
		}

		/* Hack -- all gone */
		else if (object_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Prefix a number if required */
		else if (object_ptr->number > 1)
		{
			t = object_desc_num(t, object_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && object_is_artifact(object_ptr))
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

	/* Paranoia -- skip illegal tildes */
	/* while (*s == '~') s++; */

#ifdef JP
	if (object_is_smith(object_ptr))
	{
		//TODO
		t = object_desc_str(t, format("%s��", species_name + species_info[object_ptr->creater_idx].name));
	}

	/* �`���̃A�C�e���A���̂���A�C�e���̖��O��t������ */
	if (known)
	{
		/* �����_���E�A�[�e�B�t�@�N�g */
		if (object_ptr->art_name)
		{
			cptr temp = quark_str(object_ptr->art_name);

			/* '�w' ����n�܂�Ȃ��`���̃A�C�e���̖��O�͍ŏ��ɕt������ */
			/* �p��ł̃Z�[�u�t�@�C�����痈�� 'of XXX' ��,�uXXX�́v�ƕ\������ */
			if (strncmp(temp, "of ", 3) == 0)
			{
				t = object_desc_str(t, &temp[3]);
				t = object_desc_str(t, "��");
			}
			else if ((strncmp(temp, "�w", 2) != 0) &&
				 (strncmp(temp, "�s", 2) != 0) &&
				 (temp[0] != '\''))
				t = object_desc_str(t, temp);
		}
		/* �`���̃A�C�e�� */
		else if (object_ptr->name1 && !have_flag(flgs, TR_FULL_NAME))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];
			/* '�w' ����n�܂�Ȃ��`���̃A�C�e���̖��O�͍ŏ��ɕt������ */
			if (strncmp(artifact_name + a_ptr->name, "�w", 2) != 0)
			{
				t = object_desc_str(t, artifact_name + a_ptr->name);
			}
		}
		/* ���̂���A�C�e�� */
		else if (object_is_ego(object_ptr))
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
		if (!*s)
		{
			s = s0 + 1;
			s0 = NULL;
		}

		/* Begin to append the modifier (flavor) */
		else if ((*s == '#') && !s0)
		{
			s0 = s;
			s = modstr;

			/* Paranoia -- Never append multiple modstrs */
			modstr = "";
		}

		/* Begin to append the kind name */
		else if ((*s == '%') && !s0)
		{
			s0 = s;
			s = kindname;

			/* Paranoia -- Never append multiple kindnames */
			kindname = "";
		}

#ifndef JP
		/* Pluralizer */
		else if (*s == '~')
		{
			/* Add a plural if needed */
			if (!(mode & OD_NO_PLURAL) && (object_ptr->number != 1))
			{
				char k = t[-1];

				/* XXX XXX XXX Mega-Hack */

				/* Hack -- "Cutlass-es" and "Torch-es" */
				if ((k == 's') || (k == 'h')) *t++ = 'e';

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

	/* Terminate */
	*t = '\0';


#ifdef JP
	/* '�w'����n�܂�`���̃A�C�e���̖��O�͍Ō�ɕt������ */
	if (known)
	{
		/* �����_���A�[�e�B�t�@�N�g�̖��O�̓Z�[�u�t�@�C���ɋL�^
		   �����̂ŁA�p��ł̖��O������炵���ϊ����� */
		if (object_ptr->art_name)
		{
			char temp[256];
			int itemp;
			strcpy(temp, quark_str(object_ptr->art_name));
			/* MEGA HACK by ita */
			if (strncmp(temp, "�w", 2) == 0 ||
			    strncmp(temp, "�s", 2) == 0)
				t = object_desc_str(t, temp);
			else if (temp[0] == '\'')
			{
				itemp = strlen(temp);
				temp[itemp - 1] = 0;
				t = object_desc_str(t, "�w");
				t = object_desc_str(t, &temp[1]);
				t = object_desc_str(t, "�x");
			}
		}
		else if (object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];
			if (strncmp(artifact_name + a_ptr->name, "�w", 2) == 0)
			{
				t = object_desc_str(t, artifact_name + a_ptr->name);
			}
		}
		else if (object_ptr->inscription)
		{
			cptr str = quark_str(object_ptr->inscription);

			while(*str)
			{
				if (iskanji(*str))
				{
					str += 2;
					continue;
				}
				if (*str == '#') break;
				str++;
			}
			if (*str)
			{
				/* Find the '#' */
				cptr str = my_strchr(quark_str(object_ptr->inscription), '#');

				/* Add the false name */
				t = object_desc_str(t,"�w");
				t = object_desc_str(t, &str[1]);
				t = object_desc_str(t,"�x");
			}
		}
	}
#else
	if (object_is_smith(owner_ptr, object_ptr))
	{
		if(!object_ptr->creater_idx)
			t = object_desc_str(t,format(" of %s the Smith", owner_ptr->name));
		else
			t = object_desc_str(t, format(" of %s", species_name + species_info[object_ptr->creater_idx].name));
	}

	/* Hack -- Append "Artifact" or "Special" names */
	if (known && !have_flag(flgs, TR_FULL_NAME))
	{
		/* Is it a new random artifact ? */
		if (object_ptr->art_name)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, quark_str(object_ptr->art_name));
		}

		/* Grab any artifact name */
		else if (object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, artifact_name + a_ptr->name);
		}

		/* Grab any ego-item name */
		else
		{
			if (object_is_ego(object_ptr))
			{
				ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];

				t = object_desc_chr(t, ' ');
				t = object_desc_str(t, object_egobject_name + e_ptr->name);
			}

			if (object_ptr->inscription && my_strchr(quark_str(object_ptr->inscription), '#'))
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
	if (mode & OD_NAME_ONLY) goto object_desc_done;

	/* Hack -- Chests must be described in detail */
	if (object_ptr->tval == TV_CHEST)
	{
		/* Not searched yet */
		if (!known)
		{
			/* Nothing */
		}

		/* May be "empty" */
		else if (!object_ptr->pval)
		{
#ifdef JP
			t = object_desc_str(t, "(��)");
#else
			t = object_desc_str(t, " (empty)");
#endif
		}

		/* May be "disarmed" */
		else if (object_ptr->pval < 0)
		{
			if (chest_traps[0 - object_ptr->pval])
			{
#ifdef JP
				t = object_desc_str(t, "(������)");
#else
				t = object_desc_str(t, " (disarmed)");
#endif
			}
			else
			{
#ifdef JP
				t = object_desc_str(t, "(��{��)");
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
					t = object_desc_str(t, "(�{��)");
#else
					t = object_desc_str(t, " (Locked)");
#endif
					break;
				}
				case CHEST_LOSE_STR:
				{
#ifdef JP
					t = object_desc_str(t, "(�Őj)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif
					break;
				}
				case CHEST_LOSE_CON:
				{
#ifdef JP
					t = object_desc_str(t, "(�Őj)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif
					break;
				}
				case CHEST_POISON:
				{
#ifdef JP
					t = object_desc_str(t, "(�K�X�E�g���b�v)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif
					break;
				}
				case CHEST_PARALYZE:
				{
#ifdef JP
					t = object_desc_str(t, "(�K�X�E�g���b�v)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif
					break;
				}
				case CHEST_EXPLODE:
				{
#ifdef JP
					t = object_desc_str(t, "(�������u)");
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
					t = object_desc_str(t, "(�����̃��[��)");
#else
					t = object_desc_str(t, " (Summoning Runes)");
#endif
					break;
				}
				case CHEST_RUNES_OF_EVIL:
				{
#ifdef JP
					t = object_desc_str(t, "(�׈��ȃ��[��)");
#else
					t = object_desc_str(t, " (Gleaming Black Runes)");
#endif
					break;
				}
				case CHEST_ALARM:
				{
#ifdef JP
					t = object_desc_str(t, "(�x�񑕒u)");
#else
					t = object_desc_str(t, " (Alarm)");
#endif
					break;
				}
				default:
				{
#ifdef JP
					t = object_desc_str(t, "(�}���`�E�g���b�v)");
#else
					t = object_desc_str(t, " (Multiple Traps)");
#endif
					break;
				}
			}
		}
	}


	/* Display the item like a weapon */
	if (have_flag(flgs, TR_SHOW_MODS)) show_weapon = TRUE;

	/* Display the item like a weapon */
	if (object_is_smith(object_ptr) && (object_ptr->xtra3 == 1 + ESSENCE_SLAY_GLOVE))
		show_weapon = TRUE;

	/* Display the item like a weapon */
	if (object_ptr->to_hit && object_ptr->to_damage) show_weapon = TRUE;

	/* Display the item like armour */
	if (object_ptr->ac) show_armour = TRUE;

	/* Fitting Size */
	if (object_ptr->size_lower && object_ptr->size_upper)
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
		power = bow_tmul(object_ptr->sval);

		/* Apply the "Extra Might" flag */
		if (have_flag(flgs, TR_XTRA_MIGHT)) power++;

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
	if (known)
	{
		/* Show the tohit/todam on request */
		if (show_weapon)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, object_ptr->to_hit);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, object_ptr->to_damage);
			t = object_desc_chr(t, p2);
		}

		/* Show the tohit if needed */
		else if (object_ptr->to_hit)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, object_ptr->to_hit);
			t = object_desc_chr(t, p2);
		}

		/* Show the todam if needed */
		else if (object_ptr->to_damage)
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
	if (bow_ptr->k_idx && (object_ptr->tval == owner_ptr->tval_ammo))
	{
		int avgdam = object_ptr->dd * (object_ptr->ds + 1) * 10 / 2;
		int tmul = bow_tmul(bow_ptr->sval);
		s16b energy_fire = bow_energy(bow_ptr->sval);

		// See if the bow is "known" - then set damage bonus
		if (object_is_known(bow_ptr)) avgdam += (bow_ptr->to_damage * 10);

		// Effect of ammo
		if (known) avgdam += (object_ptr->to_damage * 10);

		// Get extra "power" from "extra might"
		if (owner_ptr->xtra_might) tmul++;

		tmul = tmul * (100 + (int)(adj_str_to_damage[owner_ptr->stat_ind[STAT_STR]]) - 128);

		// Launcher multiplier
		avgdam *= tmul;
		avgdam /= (100 * 10);

		// Get extra damage from concentration
		
		if (owner_ptr->concent) avgdam = boost_concentration_damage(owner_ptr, avgdam);

		if (avgdam < 0) avgdam = 0;

		// Display (shot damage/ avg damage)
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, '/');

		if (owner_ptr->num_fire == 0)
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
	else if ((owner_ptr->class_idx == CLASS_NINJA) && (object_ptr->tval == TV_SPIKE))
	{
		int avgdam = has_trait(owner_ptr, TRAIT_MIGHTY_THROW) ? (1 + 3) : 1;
		s16b energy_fire = 100 - owner_ptr->lev;

		avgdam += ((owner_ptr->lev + 30) * (owner_ptr->lev + 30) - 900) / 55;

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
	if (known)
	{
		/* Show the armor class info */
		if (show_armour)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_num(t, object_ptr->ac);
			t = object_desc_int(t, object_ptr->to_ac);
			t = object_desc_chr(t, ',');
			t = object_desc_num(t, object_ptr->ev);
			t = object_desc_int(t, object_ptr->to_ev);
			t = object_desc_chr(t, b2);
		}

		/* No base armor, but does increase armor */
		else if (object_ptr->to_ac)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_int(t, object_ptr->to_ac);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, object_ptr->to_ev);
			t = object_desc_chr(t, b2);
		}
	}

	/* Hack -- always show base armor */
	else if (show_armour)
	{
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, b1);
		t = object_desc_num(t, object_ptr->ac);
			t = object_desc_chr(t, ',');
		t = object_desc_num(t, object_ptr->ev);
		t = object_desc_chr(t, b2);
	}


	/* No more details wanted */
	if (mode & OD_NAME_AND_ENCHANT) goto object_desc_done;


	if (known) /* Known item only */
	{
		/*
		 * Hack -- Wands and Staffs have charges.  Make certain how many charges
		 * a stack of staffs really has is clear. -LM-
		 */
		if (((object_ptr->tval == TV_STAFF) || (object_ptr->tval == TV_WAND)))
		{
			/* Dump " (N charges)" */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Clear explaination for staffs. */
			if ((object_ptr->tval == TV_STAFF) && (object_ptr->number > 1))
			{
				t = object_desc_num(t, object_ptr->number);
				t = object_desc_str(t, "x ");
			}
			t = object_desc_num(t, object_ptr->pval);
#ifdef JP
			t = object_desc_str(t, "��");
#else
			t = object_desc_str(t, " charge");
			if (object_ptr->pval != 1) t = object_desc_chr(t, 's');
#endif

			t = object_desc_chr(t, p2);
		}
		/* Hack -- Rods have a "charging" indicator.  Now that stacks of rods may
		 * be in any state of charge or discharge, this now includes a number. -LM-
		 */
		else if (object_ptr->tval == TV_ROD)
		{
			/* Hack -- Dump " (# charging)" if relevant */
			if (object_ptr->timeout)
			{
				/* Stacks of rods display an exact count of charging rods. */
				if (object_ptr->number > 1)
				{
					if (k_ptr->pval == 0) k_ptr->pval = 1; // Paranoia.

					/* Find out how many rods are charging, by dividing
					 * current timeout by each rod's maximum timeout.
					 * Ensure that any remainder is rounded up.  Display
					 * very discharged stacks as merely fully discharged.
					 */
					power = (object_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
					if (power > object_ptr->number) power = object_ptr->number;

					/* Display prettily. */
					t = object_desc_str(t, " (");
					t = object_desc_num(t, power);
#ifdef JP
					t = object_desc_str(t, "�{ �[�U��)");
#else
					t = object_desc_str(t, " charging)");
#endif
				}

				/* "one Rod of Perception (1 charging)" would look tacky. */
				else
				{
#ifdef JP
					t = object_desc_str(t, "(�[�U��)");
#else
					t = object_desc_str(t, " (charging)");
#endif
				}
			}
		}

		/* Dump "pval" flags for wearable items */
		if (have_pval_flags(flgs))
		{
			/* Start the display */
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);

			/* Dump the "pval" itself */
			t = object_desc_int(t, object_ptr->pval);

			/* Do not display the "pval" flags */
			if (have_flag(flgs, TR_HIDE_TYPE))
			{
				/* Nothing */
			}

			/* Speed */
			else if (have_flag(flgs, TR_SPEED))
			{
				/* Dump " to speed" */
#ifdef JP
				t = object_desc_str(t, "����");
#else
				t = object_desc_str(t, " to speed");
#endif
			}

			/* Attack speed */
			else if (have_flag(flgs, TR_BLOWS))
			{
				/* Add " attack" */
#ifdef JP
				t = object_desc_str(t, "�U��");
#else
				t = object_desc_str(t, " attack");

				/* Add "attacks" */
				if (ABS(object_ptr->pval) != 1) t = object_desc_chr(t, 's');
#endif
			}

			/* Stealth */
			else if (have_flag(flgs, TR_STEALTH))
			{
				/* Dump " to stealth" */
#ifdef JP
				t = object_desc_str(t, "�B��");
#else
				t = object_desc_str(t, " to stealth");
#endif
			}

			/* Search */
			else if (have_flag(flgs, TR_SEARCH))
			{
				/* Dump " to searching" */
#ifdef JP
				t = object_desc_str(t, "�T��");
#else
				t = object_desc_str(t, " to searching");
#endif
			}

			/* Infravision */
			else if (have_flag(flgs, TR_INFRA))
			{
				/* Dump " to infravision" */
#ifdef JP
				t = object_desc_str(t, "�ԊO������");
#else
				t = object_desc_str(t, " to infravision");
#endif
			}

			/* Finish the display */
			t = object_desc_chr(t, p2);
		}

		/* Hack -- Process Lanterns/Torches */
		if (object_ptr->tval == TV_LITE && !have_flag(k_ptr->flags, TR_NO_LIMIT_LITE))
		{
			/* Hack -- Turns of light for normal lites */
#ifdef JP
			t = object_desc_chr(t, '(');
#else
			t = object_desc_str(t, " (with ");
#endif

			if (object_ptr->name2 == EGO_LITE_LONG) t = object_desc_num(t, object_ptr->xtra4 * 2);
			else t = object_desc_num(t, object_ptr->xtra4);
#ifdef JP
			t = object_desc_str(t, "�^�[���̎���)");
#else
			t = object_desc_str(t, " turns of light)");
#endif
		}

		/* Indicate charging objects, but not rods. */
		if (object_ptr->timeout && (object_ptr->tval != TV_ROD))
		{
			/* Hack -- Dump " (charging)" if relevant */
#ifdef JP
			t = object_desc_str(t, "(�[�U��)");
#else
			t = object_desc_str(t, " (charging)");
#endif
		}
	}


	/* No more details wanted */
	if (mode & OD_OMIT_INSCRIPTION) goto object_desc_done;


	/* Prepare real inscriptions in a buffer */
	tmp_val2[0] = '\0';

	/* Auto abbreviation inscribe */
	if ((abbrev_extra || abbrev_all) && (object_ptr->ident & IDENT_MENTAL))
	{
		if (!object_ptr->inscription || !my_strchr(quark_str(object_ptr->inscription), '%'))
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
	if (object_ptr->inscription)
	{
		char buff[1024];

		if (tmp_val2[0]) strcat(tmp_val2, ", ");

		/* Get inscription and convert {%} */
		get_inscription(buff, object_ptr);

		/* strcat with correct treating of kanji */
		my_strcat(tmp_val2, buff, sizeof(tmp_val2));
	}


	/* No fake inscription yet */
	fake_insc_buf[0] = '\0';

	/* Use the game-generated "feeling" otherwise, if available */
	if (object_ptr->feeling)
	{
		strcpy(fake_insc_buf, game_inscriptions[object_ptr->feeling]);
	}

	/* Note "cursed" if the item is known to be cursed */
	else if (object_is_cursed(object_ptr) && (known || (object_ptr->ident & IDENT_SENSE)))
	{
#ifdef JP
		strcpy(fake_insc_buf, "����Ă���");
#else
		strcpy(fake_insc_buf, "cursed");
#endif
	}

	/* Note "unidentified" if the item is unidentified */
	else if (((object_ptr->tval == TV_RING) || (object_ptr->tval == TV_AMULET)
		   || (object_ptr->tval == TV_LITE) || (object_ptr->tval == TV_FIGURINE))
		 && aware && !known
		 && !(object_ptr->ident & IDENT_SENSE))
	{
#ifdef JP
		strcpy(fake_insc_buf, "���Ӓ�");
#else
		strcpy(fake_insc_buf, "unidentified");
#endif
	}

	/* Mega-Hack -- note empty wands/staffs */
	else if (!known && (object_ptr->ident & IDENT_EMPTY))
	{
#ifdef JP
		strcpy(fake_insc_buf, "��");
#else
		strcpy(fake_insc_buf, "empty");
#endif
	}

	/* Note "tried" if the object has been tested unsuccessfully */
	else if (!aware && object_is_tried(object_ptr))
	{
#ifdef JP
		strcpy(fake_insc_buf, "������");
#else
		strcpy(fake_insc_buf, "tried");
#endif
	}

	/* Note the discount, if any */
	if (object_ptr->discount)
	{
		/* Hidden by real inscription unless in a store */
		if (!tmp_val2[0] || (object_ptr->ident & IDENT_STORE))
		{
			char discount_num_buf[4];

			/* Append to other fake inscriptions if any */
			if (fake_insc_buf[0]) strcat(fake_insc_buf, ", ");

			(void)object_desc_num(discount_num_buf, object_ptr->discount);
			strcat(fake_insc_buf, discount_num_buf);
#ifdef JP
			strcat(fake_insc_buf, "%����");
#else
			strcat(fake_insc_buf, "% off");
#endif
		}
	}


	/* Append the inscription, if any */
	if (fake_insc_buf[0] || tmp_val2[0])
	{
		/* Append the inscription */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, c1);

		/* Append fake inscriptions */
		if (fake_insc_buf[0])
		{
			t = object_desc_str(t, fake_insc_buf);
		}

		/* Append a separater */
		if (fake_insc_buf[0] && tmp_val2[0])
		{
			t = object_desc_chr(t, ',');
			t = object_desc_chr(t, ' ');
		}

		/* Append real inscriptions */
		if (tmp_val2[0])
		{
			t = object_desc_str(t, tmp_val2);
		}

		t = object_desc_chr(t, c2);
	}

object_desc_done:
	my_strcpy(buf, tmp_val, MAX_NLEN);
}


