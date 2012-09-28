/* File: wizard1.c */

/*
 * Copyright (c) 1997 Ben Harrison, and others
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Spoiler generation -BEN- */

#include "angband.h"


#ifdef ALLOW_SPOILERS


/*
 * The spoiler file being created
 */
static FILE *fff = NULL;



/*
 * Extract a textual representation of an attribute
 */
static cptr attr_to_text(species_type *species_ptr)
{
#ifdef JP000
	if(has_trait_species(species_ptr, TRAIT_ATTR_CLEAR))	return "������";
	if(has_trait_species(species_ptr, TRAIT_ATTR_MULTI))	return "���F��";
	if(has_trait_species(species_ptr, TRAIT_ATTR_SEMIRAND))	return "�������_����";
#else
	if(has_trait_species(species_ptr, TRAIT_ATTR_CLEAR))	return "Clear";
	if(has_trait_species(species_ptr, TRAIT_ATTR_MULTI))	return "Multi";
	if(has_trait_species(species_ptr, TRAIT_ATTR_SEMIRAND))	return "S.Rand";
#endif

	switch (species_ptr->d_attr)
	{
#ifdef JP000
	case TERM_DARK:    return "XXX";
	case TERM_WHITE:   return "����";
	case TERM_SLATE:   return "�D�F��";
	case TERM_ORANGE:  return "�I�����W��";
	case TERM_RED:     return "�Ԃ�";
	case TERM_GREEN:   return "�΂�";
	case TERM_BLUE:    return "��";
	case TERM_UMBER:   return "���ߐF��";
	case TERM_L_DARK:  return "�D�F��";
	case TERM_L_WHITE: return "���D�F��";
	case TERM_VIOLET:  return "����";
	case TERM_YELLOW:  return "���F��";
	case TERM_L_RED:   return "�����Ԃ�";
	case TERM_L_GREEN: return "�����΂�";
	case TERM_L_BLUE:  return "������";
	case TERM_L_UMBER: return "�������ߐF��";
#else
	case TERM_DARK:    return "xxx";
	case TERM_WHITE:   return "White";
	case TERM_SLATE:   return "Slate";
	case TERM_ORANGE:  return "Orange";
	case TERM_RED:     return "Red";
	case TERM_GREEN:   return "Green";
	case TERM_BLUE:    return "Blue";
	case TERM_UMBER:   return "Umber";
	case TERM_L_DARK:  return "L.Dark";
	case TERM_L_WHITE: return "L.Slate";
	case TERM_VIOLET:  return "Violet";
	case TERM_YELLOW:  return "Yellow";
	case TERM_L_RED:   return "L.Red";
	case TERM_L_GREEN: return "L.Green";
	case TERM_L_BLUE:  return "L.Blue";
	case TERM_L_UMBER: return "L.Umber";
#endif
	}

	/* Oops */
#ifdef JP000
	return "�ς�";
#else
	return "Icky";
#endif
}



/*
 * A tval grouper
 */
typedef struct
{
	byte tval;
	cptr name;
} grouper;



/*
 * Item Spoilers by: benh@phial.com (Ben Harrison)
 */


/*
 * The basic items categorized by type
 */
static grouper group_item[] =
{
#ifdef JP
	{ TV_SHOT, "�ˌ���" },
#else
	{ TV_SHOT, "Ammo" },
#endif

	{ TV_ARROW, NULL },
	{ TV_BOLT, NULL },

#ifdef JP
	{ TV_BOW, "�|" },
#else
	{ TV_BOW, "Bows" },
#endif

#ifdef JP
	{ TV_DIGGING, "����" },
#else
	{ TV_DIGGING, "Weapons" },
#endif

	{ TV_POLEARM, NULL },
	{ TV_HAFTED, NULL },
	{ TV_SWORD, NULL },

#ifdef JP
	{ TV_SOFT_ARMOR, "�h�� (��)" },
#else
	{ TV_SOFT_ARMOR, "Armour (Body)" },
#endif

	{ TV_HARD_ARMOR, NULL },
	{ TV_DRAG_ARMOR, NULL },

#ifdef JP
	{ TV_BOOTS, "�h�� (���̑�)" },
#else
	{ TV_BOOTS, "Armour (Misc)" },
#endif

	{ TV_GLOVES, NULL },
	{ TV_HELM, NULL },
	{ TV_CROWN, NULL },
	{ TV_SHIELD, NULL },
	{ TV_CLOAK, NULL },

#ifdef JP
	{ TV_LITE, "����" },
	{ TV_AMULET, "�A�~�����b�g" },
	{ TV_RING, "�w��" },
#else
	{ TV_LITE, "Light Sources" },
	{ TV_AMULET, "Amulets" },
	{ TV_RING, "Rings" },
#endif

#ifdef JP
	{ TV_STAFF, "��" },
	{ TV_WAND, "���@�_" },
	{ TV_ROD, "���b�h" },
#else
	{ TV_STAFF, "Staffs" },
	{ TV_WAND, "Wands" },
	{ TV_ROD, "Rods" },
#endif

#ifdef JP
	{ TV_SCROLL, "����" },
	{ TV_POTION, "��" },
	{ TV_FOOD, "�H��" },
#else
	{ TV_SCROLL, "Scrolls" },
	{ TV_POTION, "Potions" },
	{ TV_FOOD, "Food" },
#endif

#ifdef JP
	{ TV_LIFE_BOOK, "���@�� (����)" },
	{ TV_SORCERY_BOOK, "���@�� (��p)" },
	{ TV_NATURE_BOOK, "���@�� (���R)" },
	{ TV_CHAOS_BOOK, "���@�� (�J�I�X)" },
	{ TV_DEATH_BOOK, "���@�� (�Í�)" },
	{ TV_TRUMP_BOOK, "���@�� (�g�����v)" },
	{ TV_ARCANE_BOOK, "���@�� (��p)" },
	{ TV_CRAFT_BOOK, "���@�� (��)" },
	{ TV_DAEMON_BOOK, "���@�� (����)" },
	{ TV_CRUSADE_BOOK, "���@�� (�j��)" },
	{ TV_MUSIC_BOOK, "�̏W" },
	{ TV_HISSATSU_BOOK, "���|�̏�" },
	{ TV_HEX_BOOK, "���@�� (��p)" },
#else
	{ TV_LIFE_BOOK, "Books (Life)" },
	{ TV_SORCERY_BOOK, "Books (Sorcery)" },
	{ TV_NATURE_BOOK, "Books (Nature)" },
	{ TV_CHAOS_BOOK, "Books (Chaos)" },
	{ TV_DEATH_BOOK, "Books (Death)" },
	{ TV_TRUMP_BOOK, "Books (Trump)" },
	{ TV_ARCANE_BOOK, "Books (Arcane)" },
	{ TV_CRAFT_BOOK, "Books (Craft)" },
	{ TV_DAEMON_BOOK, "Books (Daemon)" },
	{ TV_CRUSADE_BOOK, "Books (Crusade)" },
	{ TV_MUSIC_BOOK, "Song Books" },
	{ TV_HISSATSU_BOOK, "Books (Kendo)" },
	{ TV_HEX_BOOK, "Books (Hex)" },
#endif

#ifdef JP
	{ TV_WHISTLE, "�J" },
	{ TV_CAPTURE, "�L���v�`���[�E�{�[��" },
	{ TV_CARD, "�G�N�X�v���X�J�[�h" },
	{ TV_WHEEL, "�ԗ�" },

#else
	{ TV_WHISTLE, "Whistle" },
	{ TV_CAPTURE, "Capture Ball" },
	{ TV_CARD, "Express Card" },
	{ TV_WHEEL, "Wheel" },
#endif

#ifdef JP
	{ TV_CHEST, "��" },
#else
	{ TV_CHEST, "Chests" },
#endif

#ifdef JP
	{ TV_FIGURINE, "�l�`" },
	{ TV_STATUE, "��" },
	{ TV_CORPSE, "����" },
#else
	{ TV_FIGURINE, "Magical Figurines" },
	{ TV_STATUE, "Statues" },
	{ TV_CORPSE, "Corpses" },
#endif

#ifdef JP
	{ TV_SKELETON, "���̑�" },
#else
	{ TV_SKELETON, "Misc" },
#endif

	{ TV_BOTTLE, NULL },
	{ TV_JUNK, NULL },
	{ TV_SPIKE, NULL },
	{ TV_FLASK, NULL },
	{ TV_PARCHMENT, NULL },

	{ 0, "" }
};


/*
 * Describe the kind
 */
static void kind_info(char *buf, char *dam, char *wgt, int *lev, s32b *val, int k)
{
	object_type forge;
	object_type *quest_ptr;


	/* Get local object */
	quest_ptr = &forge;

	/* Prepare a fake item */
	object_prep(quest_ptr, k, ITEM_FREE_SIZE);

	/* It is known */
	quest_ptr->ident |= (IDENT_KNOWN);

	/* Cancel bonuses */
	quest_ptr->pval = 0;
	quest_ptr->to_ac = 0;
	quest_ptr->to_hit = 0;
	quest_ptr->to_damage = 0;


	/* Level */
	(*lev) = object_kind_info[quest_ptr->k_idx].level;

	/* Value */
	(*val) = object_value(quest_ptr);


	/* Hack */
	if(!buf || !dam || !wgt) return;


	/* Description (too brief) */
	object_desc(buf, quest_ptr, (OD_NAME_ONLY | OD_STORE));


	/* Misc info */
	strcpy(dam, "");

	/* Damage */
	switch (quest_ptr->tval)
	{
		/* Bows */
		case TV_BOW:
		{
			break;
		}

		/* Ammo */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		{
			sprintf(dam, "%dd%d", quest_ptr->dd, quest_ptr->ds);
			break;
		}

		/* Weapons */
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			sprintf(dam, "%dd%d", quest_ptr->dd, quest_ptr->ds);
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
		{
			sprintf(dam, "%d", quest_ptr->ac);
			break;
		}
	}


	/* Weight */
	sprintf(wgt, "%3d.%d", quest_ptr->weight / 10, quest_ptr->weight % 10);
}


/*
 * Create a spoiler file for items
 */
static void spoil_obj_desc(cptr fname)
{
	int i, k, s, t, n = 0, group_start = 0;

	u16b who[200];

	char buf[1024];

	char wgt[80];
	char dam[80];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}


	/* Header */
	fprintf(fff, "Spoiler File -- Basic Items (D\'angband %d.%d.%d)\n\n\n",
		VER_MAJOR, VER_MINOR, VER_PATCH);

	/* More Header */
	fprintf(fff, "%-45s     %8s%7s%5s%9s\n",
		"Description", "Dam/AC", "Wgt", "Lev", "Cost");
	fprintf(fff, "%-45s     %8s%7s%5s%9s\n",
		"----------------------------------------",
		"------", "---", "---", "----");

	/* List the groups */
	for (i = 0; TRUE; i++)
	{
		/* Write out the group title */
		if(group_item[i].name)
		{
			if(n)
			{
				/* Hack -- bubble-sort by cost and then level */
				for (s = 0; s < n - 1; s++)
				{
					for (t = 0; t < n - 1; t++)
					{
						int i1 = t;
						int i2 = t + 1;

						int e1;
						int e2;

						s32b t1;
						s32b t2;

						kind_info(NULL, NULL, NULL, &e1, &t1, who[i1]);
						kind_info(NULL, NULL, NULL, &e2, &t2, who[i2]);

						if((t1 > t2) || ((t1 == t2) && (e1 > e2)))
						{
							int tmp = who[i1];
							who[i1] = who[i2];
							who[i2] = tmp;
						}
					}
				}

				fprintf(fff, "\n\n%s\n\n", group_item[group_start].name);

				/* Spoil each item */
				for (s = 0; s < n; s++)
				{
					int e;
					s32b v;

					/* Describe the kind */
					kind_info(buf, dam, wgt, &e, &v, who[s]);

					/* Dump it */
					fprintf(fff, "     %-45s%8s%7s%5d%9ld\n",
						buf, dam, wgt, e, (long)(v));
				}

				/* Start a new set */
				n = 0;
			}

			/* Notice the end */
			if(!group_item[i].tval) break;

			/* Start a new set */
			group_start = i;
		}

		/* Acquire legal item types */
		for (k = 1; k < max_object_kind_idx; k++)
		{
			object_kind *k_ptr = &object_kind_info[k];

			/* Skip wrong tval's */
			if(k_ptr->tval != group_item[i].tval) continue;

			/* Hack -- Skip instant-artifacts */
			if(have_flag(k_ptr->flags, TRAIT_INSTA_ART)) continue;

			/* Save the index */
			who[n++] = k;
		}
	}


	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Message */
	msg_print("Successfully created a spoiler file.");
}


/*
 * Artifact Spoilers by: randy@PICARD.tamu.edu (Randy Hutson)
 */


/*
 * Returns a "+" string if a number is non-negative and an empty
 * string if negative
 */
#define POSITIZE(v) (((v) >= 0) ? "+" : "")

/*
 * These are used to format the artifact spoiler file. INDENT1 is used
 * to indent all but the first line of an artifact spoiler. INDENT2 is
 * used when a line "wraps". (Bladeturner's resistances cause this.)
 */
#define INDENT1 "    "
#define INDENT2 "      "

/*
 * MAX_LINE_LEN specifies when a line should wrap.
 */
#define MAX_LINE_LEN 75

/*
 * Given an array, determine how many elements are in the array
 */
#define N_ELEMENTS(a) (sizeof (a) / sizeof ((a)[0]))

/*
 * The artifacts categorized by type
 */
static grouper group_artifact[] =
{
#ifdef JP
	{ TV_SWORD, "����" },
	{ TV_POLEARM, "��/��" },
	{ TV_HAFTED, "�݊�" },
	{ TV_DIGGING, "�V���x��/��͂�" },
	{ TV_BOW, "��ѓ���" },
	{ TV_ARROW, "��" },

	{ TV_SOFT_ARMOR, "�Z" },
	{ TV_HARD_ARMOR, NULL },
	{ TV_DRAG_ARMOR, NULL },

	{ TV_CLOAK, "�N���[�N" },
	{ TV_SHIELD, "��" },
	{ TV_CARD, NULL },
	{ TV_WHEEL, NULL },
	{ TV_HELM, "��/��" },
	{ TV_CROWN, NULL },
	{ TV_GLOVES, "�Ď�" },
	{ TV_BOOTS, "�C" },

	{ TV_LITE, "����" },
	{ TV_AMULET, "�A�~�����b�g" },
	{ TV_RING, "�w��" },
#else
	{ TV_SWORD, "Edged Weapons" },
	{ TV_POLEARM, "Polearms" },
	{ TV_HAFTED, "Hafted Weapons" },
	{ TV_DIGGING, "Shovels/Picks" },
	{ TV_BOW, "Bows" },
	{ TV_ARROW, "Ammo" },

	{ TV_SOFT_ARMOR, "Body Armor" },
	{ TV_HARD_ARMOR, NULL },
	{ TV_DRAG_ARMOR, NULL },

	{ TV_CLOAK, "Cloaks" },
	{ TV_SHIELD, "Shields" },
	{ TV_CARD, NULL },
	{ TV_WHEEL, NULL },
	{ TV_HELM, "Helms/Crowns" },
	{ TV_CROWN, NULL },
	{ TV_GLOVES, "Gloves" },
	{ TV_BOOTS, "Boots" },

	{ TV_LITE, "Light Sources" },
	{ TV_AMULET, "Amulets" },
	{ TV_RING, "Rings" },
#endif

	{ 0, NULL }
};



/*
 * Pair together a constant flag with a textual description.
 *
 * Used by both "init.c" and "wiz-spo.c".
 *
 * Note that it sometimes more efficient to actually make an array
 * of textual names, where entry 'N' is assumed to be paired with
 * the flag whose value is "1L << N", but that requires hard-coding.
 */

typedef struct flag_desc flag_desc;

struct flag_desc
{
	const int flag;
	const char *const desc;
};



/*
 * These are used for "+3 to STR, DEX", etc. These are separate from
 * the other pval affected traits_precondition to simplify the case where an object
 * affects all stats.  In this case, "All stats" is used instead of
 * listing each stat individually.
 */

static flag_desc stat_flags_desc[] =
{
#ifdef JP
	{ STAT_STR, "�r��" },
	{ STAT_INT, "�m�\" },
	{ STAT_WIS, "����" },
	{ STAT_DEX, "��p��" },
	{ STAT_CON, "�ϋv��" },
	{ STAT_CHA, "����" }
#else
	{ STAT_STR, "STR" },
	{ STAT_INT, "INT" },
	{ STAT_WIS, "WIS" },
	{ STAT_DEX, "DEX" },
	{ STAT_CON, "CON" },
	{ STAT_CHA, "CHR" }
#endif
};

/*
 * Besides stats, these are the other player traits_precondition
 * which may be affected by an object's pval
 */

static flag_desc pval_flags1_desc[] =
{
#ifdef JP
	{ TRAIT_MAGIC_MASTERY, "���@����g�p�\��" },
	{ TRAIT_STEALTH, "�B��" },
	{ TRAIT_SEARCH, "�T��" },
	{ TRAIT_INFRA, "�ԊO������" },
	{ TRAIT_TUNNEL, "�̌@" },
	{ TRAIT_BLOWS, "�U����" },
	{ TRAIT_SPEED, "�X�s�[�h" }
#else
	{ TRAIT_STEALTH, "Stealth" },
	{ TRAIT_SEARCH, "Searching" },
	{ TRAIT_INFRA, "Infravision" },
	{ TRAIT_TUNNEL, "Tunneling" },
	{ TRAIT_BLOWS, "Attacks" },
	{ TRAIT_SPEED, "Speed" }
#endif
};

/*
 * Slaying preferences for weapons
 */

static flag_desc slay_flags_desc[] =
{
#ifdef JP
	{ TRAIT_SLAY_ANIMAL, "����" },
	{ TRAIT_KILL_ANIMAL, "*����*" },
	{ TRAIT_SLAY_EVIL, "�׈�" },
	{ TRAIT_KILL_EVIL, "*�׈�*" },
	{ TRAIT_SLAY_GOOD, "�P��" },
	{ TRAIT_KILL_GOOD, "*�P��*" },
	{ TRAIT_SLAY_HUMAN, "�l��" },
	{ TRAIT_KILL_HUMAN, "*�l��*" },
	{ TRAIT_SLAY_UNDEAD, "�A���f�b�h" },
	{ TRAIT_KILL_UNDEAD, "*�A���f�b�h*" },
	{ TRAIT_SLAY_DEMON, "����" },
	{ TRAIT_KILL_DEMON, "*����*" },
	{ TRAIT_SLAY_ORC, "�I�[�N" },
	{ TRAIT_KILL_ORC, "*�I�[�N*" },
	{ TRAIT_SLAY_TROLL, "�g����" },
	{ TRAIT_KILL_TROLL, "*�g����*" },
	{ TRAIT_SLAY_GIANT, "���l" },
	{ TRAIT_KILL_GIANT, "*���l*" },
	{ TRAIT_SLAY_DRAGON, "�h���S��" },
	{ TRAIT_KILL_DRAGON, "*�h���S��*" },
#else
	{ TRAIT_SLAY_ANIMAL, "Animal" },
	{ TRAIT_KILL_ANIMAL, "XAnimal" },
	{ TRAIT_SLAY_EVIL, "Evil" },
	{ TRAIT_KILL_EVIL, "XEvil" },
	{ TRAIT_SLAY_GOOD, "Good" },
	{ TRAIT_KILL_GOOD, "XGood" },
	{ TRAIT_SLAY_HUMAN, "Human" },
	{ TRAIT_KILL_HUMAN, "XHuman" },
	{ TRAIT_SLAY_UNDEAD, "Undead" },
	{ TRAIT_KILL_UNDEAD, "XUndead" },
	{ TRAIT_SLAY_DEMON, "Demon" },
	{ TRAIT_KILL_DEMON, "XDemon" },
	{ TRAIT_SLAY_ORC, "Orc" },
	{ TRAIT_KILL_ORC, "XOrc" },
	{ TRAIT_SLAY_TROLL, "Troll" },
	{ TRAIT_KILL_TROLL, "XTroll" },
	{ TRAIT_SLAY_GIANT, "Giant" },
	{ TRAIT_KILL_GIANT, "Xgiant" },
	{ TRAIT_SLAY_DRAGON, "Dragon" },
	{ TRAIT_KILL_DRAGON, "Xdragon" }
#endif
};

/*
 * Elemental brands for weapons
 *
 * Clearly, TR1_IMPACT is a bit out of place here. To simplify
 * coding, it has been included here along with the elemental
 * brands. It does seem to fit in with the brands and slaying
 * more than the miscellaneous section.
 */
static flag_desc brand_flags_desc[] =
{
#ifdef JP
	{ TRAIT_ACID_BRAND, "�n��" },
	{ TRAIT_ELEC_BRAND, "�d��" },
	{ TRAIT_FIRE_BRAND, "�Ċ�" },
	{ TRAIT_COLD_BRAND, "����" },
	{ TRAIT_POIS_BRAND, "�ŎE" },

	{ TRAIT_FORCE_WEAPON, "����" },
	{ TRAIT_CHAOTIC_BRAND, "����" },
	{ TRAIT_VAMPIRIC_BRAND, "�z��" },
	{ TRAIT_SHATTER, "�n�k" },
	{ TRAIT_VORPAL, "�؂ꖡ" },
#else
	{ TRAIT_ACID_BRAND, "Acid Brand" },
	{ TRAIT_ELEC_BRAND, "Lightning Brand" },
	{ TRAIT_FIRE_BRAND, "Flame Tongue" },
	{ TRAIT_COLD_BRAND, "Frost Brand" },
	{ TRAIT_POIS_BRAND, "Poisoned" },

	{ TRAIT_FORCE_WEAPON, "Force" },
	{ TRAIT_CHAOTIC_BRAND, "Mark of Chaos" },
	{ TRAIT_VAMPIRIC_BRAND, "Vampiric" },
	{ TRAIT_SHATTER, "Earthquake impact on hit" },
	{ TRAIT_VORPAL, "Very sharp" },
#endif
};


/*
 * The 15 resistables
 */
static const flag_desc resist_flags_desc[] =
{
#ifdef JP
	{ TRAIT_RES_ACID, "�_" },
	{ TRAIT_RES_ELEC, "�d��" },
	{ TRAIT_RES_FIRE, "�Ή�" },
	{ TRAIT_RES_COLD, "��C" },
	{ TRAIT_RES_POIS, "��" },
	{ TRAIT_FEARLESS, "���|"},
	{ TRAIT_RES_LITE, "�M��" },
	{ TRAIT_RES_DARK, "�Í�" },
	{ TRAIT_NO_BLIND, "�Ӗ�" },
	{ TRAIT_NO_CONF, "����" },
	{ TRAIT_RES_SOUN, "����" },
	{ TRAIT_RES_SHAR, "�j��" },
	{ TRAIT_RES_NETH, "�n��" },
	{ TRAIT_RES_NEXU, "���ʍ���" },
	{ TRAIT_RES_CHAO, "�J�I�X" },
	{ TRAIT_RES_DISE, "��" },
#else
	{ TRAIT_RES_ACID, "Acid" },
	{ TRAIT_RES_ELEC, "Lightning" },
	{ TRAIT_RES_FIRE, "Fire" },
	{ TRAIT_RES_COLD, "Cold" },
	{ TRAIT_RES_POIS, "Poison" },
	{ TRAIT_FEARLESS, "Fear"},
	{ TRAIT_RES_LITE, "Light" },
	{ TRAIT_RES_DARK, "Dark" },
	{ TRAIT_NO_BLIND, "Blindness" },
	{ TRAIT_NO_CONF, "Confusion" },
	{ TRAIT_RES_SOUN, "Sound" },
	{ TRAIT_RES_SHAR, "Shards" },
	{ TRAIT_RES_NETH, "Nether" },
	{ TRAIT_RES_NEXU, "Nexus" },
	{ TRAIT_RES_CHAO, "Chaos" },
	{ TRAIT_RES_DISE, "Disenchantment" },
#endif
};

/*
 * Elemental immunities (along with poison)
 */

static const flag_desc immune_flags_desc[] =
{
#ifdef JP
	{ TRAIT_IM_ACID, "�_" },
	{ TRAIT_IM_ELEC, "�d��" },
	{ TRAIT_IM_FIRE, "�Ή�" },
	{ TRAIT_IM_COLD, "��C" },
#else
	{ TRAIT_IM_ACID, "Acid" },
	{ TRAIT_IM_ELEC, "Lightning" },
	{ TRAIT_IM_FIRE, "Fire" },
	{ TRAIT_IM_COLD, "Cold" },
#endif
};

/*
 * Sustain stats -  these are given their "own" line in the
 * spoiler file, mainly for simplicity
 */
static const flag_desc sustain_flags_desc[] =
{
#ifdef JP
	{ TRAIT_SUSTAIN_STR, "�r��" },
	{ TRAIT_SUSTAIN_INT, "�m�\" },
	{ TRAIT_SUSTAIN_WIS, "����" },
	{ TRAIT_SUSTAIN_DEX, "��p��" },
	{ TRAIT_SUSTAIN_CON, "�ϋv��" },
	{ TRAIT_SUSTAIN_CHR, "����" },
#else
	{ TRAIT_SUSTAIN_STR, "STR" },
	{ TRAIT_SUSTAIN_INT, "INT" },
	{ TRAIT_SUSTAIN_WIS, "WIS" },
	{ TRAIT_SUSTAIN_DEX, "DEX" },
	{ TRAIT_SUSTAIN_CON, "CON" },
	{ TRAIT_SUSTAIN_CHR, "CHR" },
#endif
};

/*
 * Miscellaneous magic given by an object's "flags2" field
 */

static const flag_desc misc_flags2_desc[] =
{
#ifdef JP
	{ TRAIT_THROW_MIGHTY, "����" },
	{ TRAIT_REFLECTING, "����" },
	{ TRAIT_FREE_ACTION, "��გm�炸" },
	{ TRAIT_HOLD_LIFE, "�����͈ێ�" },
#else
	{ TRAIT_THROW_MIGHTY, "Throwing" },
	{ TRAIT_REFLECTING, "Reflection" },
	{ TRAIT_FREE_ACTION, "Free Action" },
	{ TRAIT_HOLD_LIFE, "Hold Life" },
#endif
};

/*
 * Miscellaneous magic given by an object's "flags3" field
 *
 * Note that cursed artifacts and objects with permanent light
 * are handled "directly" -- see analyze_misc_magic()
 */

static const flag_desc misc_flags3_desc[] =
{
#ifdef JP
	{ TRAIT_AURA_FIRE, "�Ή��I�[��" },
	{ TRAIT_AURA_ELEC, "�d���I�[��" },
	{ TRAIT_AURA_COLD, "��C�I�[��" },
	{ TRAIT_PREVENT_TELEPORT, "���e���|�[�g" },
	{ TRAIT_ANTI_MAGIC, "�����@" },
	{ TRAIT_LEVITATION, "���V" },
	{ TRAIT_SEE_INVISIBLE, "������" },
	{ TRAIT_ESP, "�e���p�V�[" },
	{ TRAIT_SENSE_ANIMAL, "�������m" },
	{ TRAIT_SENSE_UNDEAD, "�s�����m" },
	{ TRAIT_SENSE_DEMON, "�������m" },
	{ TRAIT_SENSE_ORC,  "�I�[�N���m" },
	{ TRAIT_SENSE_TROLL, "�g�������m" },
	{ TRAIT_SENSE_GIANT, "���l���m" },
	{ TRAIT_SENSE_DRAGON, "�h���S�����m" },
	{ TRAIT_SENSE_HUMAN, "�l�Ԋ��m" },
	{ TRAIT_SENSE_EVIL, "�׈����m" },
	{ TRAIT_SENSE_GOOD, "�P�Ǌ��m" },
	{ TRAIT_SENSE_NONLIVING, "���������m" },
	{ TRAIT_SENSE_UNIQUE, "���j�[�N���m" },
	{ TRAIT_SLOW_DIGEST, "�x����" },
	{ TRAIT_REGENERATE, "�}����" },
	{ TRAIT_WARNING, "�x��" },
	{ TRAIT_EXTRA_ATTACK_MIGHT, "���͎ˌ�" }, 
	{ TRAIT_EXTRA_ATTACK_SPEED, "�ǉ��ˌ�" }, /* always +1? */
	{ TRAIT_DRAIN_EXP, "�o���l�z��" },
	{ TRAIT_ANTIPATHY, "����" },
	{ TRAIT_BLESSED_BRAND, "�j��" },
	{ TRAIT_DEC_MANA, "����͌���" },
#else
	{ TRAIT_AURA_FIRE, "Fiery Aura" },
	{ TRAIT_AURA_ELEC, "Electric Aura" },
	{ TRAIT_AURA_COLD, "Coldly Aura" },
	{ TRAIT_PREVENT_TELEPORT, "Prevent Teleportation" },
	{ TRAIT_ANTI_MAGIC, "Anti-Magic" },
	{ TRAIT_LEVITATION, "Levitation" },
	{ TRAIT_SEE_INVISIBLE, "See Invisible" },
	{ TRAIT_ESP, "ESP" },
	{ TRAIT_SLOW_DIGEST, "Slow Digestion" },
	{ TRAIT_REGENERATE, "Regeneration" },
	{ TRAIT_WARNING, "Warning" },
/*	{ TRAIT_EXTRA_ATTACK_MIGHT, "Extra Might" }, */
	{ TRAIT_EXTRA_ATTACK_SPEED, "+1 Extra Shot" }, /* always +1? */
	{ TRAIT_DRAIN_EXP, "Drains Experience" },
	{ TRAIT_ANTIPATHY, "Aggravates" },
	{ TRAIT_BLESSED_BRAND, "Blessed Blade" },
	{ TRAIT_DEC_MANA, "Decrease Mana Consumption Rate" },
#endif
};


/*
 * A special type used just for deailing with pvals
 */
typedef struct
{
	/*
	 * This will contain a string such as "+2", "-10", etc.
	 */
	char pval_desc[12];

	/*
	 * A list of various player traits_precondition affected by an object's pval such
	 * as stats, speed, stealth, etc.  "Extra attacks" is NOT included in
	 * this list since it will probably be desirable to format its
	 * description differently.
	 *
	 * Note that room need only be reserved for the number of stats - 1
	 * since the description "All stats" is used if an object affects all
	 * all stats. Also, room must be reserved for a sentinel NULL pointer.
	 *
	 * This will be a list such as ["STR", "DEX", "Stealth", NULL] etc.
	 *
	 * This list includes extra attacks, for simplicity.
	 */
	cptr pval_affects[N_ELEMENTS(stat_flags_desc) - 1 +
			  N_ELEMENTS(pval_flags1_desc) + 1];

} pval_info_type;


/*
 * An "object analysis structure"
 *
 * It will be filled with descriptive strings detailing an object's
 * various magical powers. The "ignore X" traits_precondition are not noted since
 * all artifacts ignore "normal" destruction.
 */

typedef struct
{
	/* "The Longsword Dragonsmiter (6d4) (+20, +25)" */
	char description[MAX_NLEN];

	/* Description of what is affected by an object's pval */
	pval_info_type pval_info;

	/* A list of an object's slaying preferences */
	cptr slays[N_ELEMENTS(slay_flags_desc) + 1];

	/* A list if an object's elemental brands */
	cptr brands[N_ELEMENTS(brand_flags_desc) + 1];

	/* A list of immunities granted by an object */
	cptr immunities[N_ELEMENTS(immune_flags_desc) + 1];

	/* A list of resistances granted by an object */
	cptr resistances[N_ELEMENTS(resist_flags_desc) + 1];

	/* A list of stats sustained by an object */
	cptr sustains[N_ELEMENTS(sustain_flags_desc)  - 1 + 1];

	/* A list of various magical qualities an object may have */
	cptr misc_magic[N_ELEMENTS(misc_flags2_desc) + N_ELEMENTS(misc_flags3_desc)
			+ 1       /* Permanent Light */
			+ 1       /* TY curse */
			+ 1       /* type of curse */
			+ 1];     /* sentinel NULL */

	/* Additional ability or resistance */
	char addition[80];

	/* A string describing an artifact's activation */
	cptr activation;

	/* "Level 20, Rarity 30, 3.0 lbs, 20000 Gold" */
	char misc_desc[80];
} obj_desc_list;


/*
 * Write out `n' of the character `c' to the spoiler file
 */
static void spoiler_out_n_chars(int n, char c)
{
	while (--n >= 0) fputc(c, fff);
}


/*
 * Write out `n' blank lines to the spoiler file
 */
static void spoiler_blanklines(int n)
{
	spoiler_out_n_chars(n, '\n');
}


/*
 * Write a line to the spoiler file and then "underline" it with hypens
 */
static void spoiler_underline(cptr str)
{
	fprintf(fff, "%s\n", str);
	spoiler_out_n_chars(strlen(str), '-');
	fprintf(fff, "\n");
}



/*
 * This function does most of the actual "analysis". Given a set of bit flags
 * (which will be from one of the flags fields from the object in question),
 * a "flag description structure", a "description list", and the number of
 * elements in the "flag description structure", this function sets the
 * "description list" members to the appropriate descriptions contained in
 * the "flag description structure".
 *
 * The possibly updated description pointer is returned.
 */
static cptr *spoiler_flag_aux(const u32b trait_flags[TRAIT_FLAG_MAX],
			      const flag_desc *flag_ptr,
			      cptr *desc_ptr, const int n_elmnts)
{
	int i;

	for (i = 0; i < n_elmnts; ++i)
	{
		if(have_flag(trait_flags, flag_ptr[i].flag))
		{
			*desc_ptr++ = flag_ptr[i].desc;
		}
	}

	return desc_ptr;
}


/*
 * Acquire a "basic" description "The Cloak of Death [1,+10]"
 */
static void analyze_general(object_type *object_ptr, char *desc_ptr)
{
	/* Get a "useful" description of the object */
	object_desc(desc_ptr, object_ptr, (OD_NAME_AND_ENCHANT | OD_STORE));
}


/*
 * List "player traits_precondition" altered by an artifact's pval. These include stats,
 * speed, infravision, tunneling, stealth, searching, and extra attacks.
 */
static void analyze_pval(object_type *object_ptr, pval_info_type *pval_ptr)
{
	u32b flgs[TRAIT_FLAG_MAX];

	cptr *affects_list;

	/* If pval == 0, there is nothing to do. */
	if(!object_ptr->pval)
	{
		/* An "empty" pval description indicates that pval == 0 */
		pval_ptr->pval_desc[0] = '\0';
		return;
	}

	/* Extract the flags */
	object_flags(object_ptr, flgs);

	affects_list = pval_ptr->pval_affects;

	/* Create the "+N" string */
	sprintf(pval_ptr->pval_desc, "%s%d", POSITIZE(object_ptr->pval), object_ptr->pval);

	/* First, check to see if the pval affects all stats */
	if(have_flag(flgs, STAT_STR) && have_flag(flgs, STAT_INT) &&
	    have_flag(flgs, STAT_WIS) && have_flag(flgs, STAT_DEX) &&
	    have_flag(flgs, STAT_CON) && have_flag(flgs, STAT_CHA))
	{
#ifdef JP
		*affects_list++ = "�S�\��";
#else
		*affects_list++ = "All stats";
#endif
	}

	/* Are any stats affected? */
	else if(have_flag(flgs, STAT_STR) || have_flag(flgs, STAT_INT) ||
		 have_flag(flgs, STAT_WIS) || have_flag(flgs, STAT_DEX) ||
		 have_flag(flgs, STAT_CON) || have_flag(flgs, STAT_CHA))
	{
		affects_list = spoiler_flag_aux(flgs, stat_flags_desc,
						affects_list,
						N_ELEMENTS(stat_flags_desc));
	}

	/* And now the "rest" */
	affects_list = spoiler_flag_aux(flgs, pval_flags1_desc,
					affects_list,
					N_ELEMENTS(pval_flags1_desc));

	/* Terminate the description list */
	*affects_list = NULL;
}


/* Note the slaying specialties of a weapon */
static void analyze_slay(object_type *object_ptr, cptr *slay_list)
{
	u32b flgs[TRAIT_FLAG_MAX];

	object_flags(object_ptr, flgs);

	slay_list = spoiler_flag_aux(flgs, slay_flags_desc, slay_list,
				     N_ELEMENTS(slay_flags_desc));

	/* Terminate the description list */
	*slay_list = NULL;
}

/* Note an object's elemental brands */
static void analyze_brand(object_type *object_ptr, cptr *brand_list)
{
	u32b flgs[TRAIT_FLAG_MAX];

	object_flags(object_ptr, flgs);

	brand_list = spoiler_flag_aux(flgs, brand_flags_desc, brand_list,
				      N_ELEMENTS(brand_flags_desc));

	/* Terminate the description list */
	*brand_list = NULL;
}


/* Note the resistances granted by an object */
static void analyze_resist(object_type *object_ptr, cptr *resist_list)
{
	u32b flgs[TRAIT_FLAG_MAX];

	object_flags(object_ptr, flgs);

	resist_list = spoiler_flag_aux(flgs, resist_flags_desc,
				       resist_list, N_ELEMENTS(resist_flags_desc));

	/* Terminate the description list */
	*resist_list = NULL;
}


/* Note the immunities granted by an object */
static void analyze_immune(object_type *object_ptr, cptr *immune_list)
{
	u32b flgs[TRAIT_FLAG_MAX];

	object_flags(object_ptr, flgs);

	immune_list = spoiler_flag_aux(flgs, immune_flags_desc,
				       immune_list, N_ELEMENTS(immune_flags_desc));

	/* Terminate the description list */
	*immune_list = NULL;
}


/* Note which stats an object sustains */
static void analyze_sustains(object_type *object_ptr, cptr *sustain_list)
{
	u32b flgs[TRAIT_FLAG_MAX];

	object_flags(object_ptr, flgs);

	/* Simplify things if an item sustains all stats */
	if(have_flag(flgs, TRAIT_SUSTAIN_STR) && have_flag(flgs, TRAIT_SUSTAIN_INT) &&
	    have_flag(flgs, TRAIT_SUSTAIN_WIS) && have_flag(flgs, TRAIT_SUSTAIN_DEX) &&
	    have_flag(flgs, TRAIT_SUSTAIN_CON) && have_flag(flgs, TRAIT_SUSTAIN_CHR))
	{
#ifdef JP
		*sustain_list++ = "�S�\��";
#else
		*sustain_list++ = "All stats";
#endif
	}

	/* Should we bother? */
	else if(have_flag(flgs, TRAIT_SUSTAIN_STR) || have_flag(flgs, TRAIT_SUSTAIN_INT) ||
		 have_flag(flgs, TRAIT_SUSTAIN_WIS) || have_flag(flgs, TRAIT_SUSTAIN_DEX) ||
		 have_flag(flgs, TRAIT_SUSTAIN_CON) || have_flag(flgs, TRAIT_SUSTAIN_CHR))
	{
		sustain_list = spoiler_flag_aux(flgs, sustain_flags_desc,
						sustain_list,
						N_ELEMENTS(sustain_flags_desc));
	}

	/* Terminate the description list */
	*sustain_list = NULL;
}


/*
 * Note miscellaneous powers bestowed by an artifact such as see invisible,
 * free action, permanent light, etc.
 */
static void analyze_misc_magic(object_type *object_ptr, cptr *misc_list)
{
	u32b flgs[TRAIT_FLAG_MAX];

	object_flags(object_ptr, flgs);

	misc_list = spoiler_flag_aux(flgs, misc_flags2_desc, misc_list,
				     N_ELEMENTS(misc_flags2_desc));

	misc_list = spoiler_flag_aux(flgs, misc_flags3_desc, misc_list,
				     N_ELEMENTS(misc_flags3_desc));

	/*
	 * Artifact lights -- large radius light.
	 */
	if((object_ptr->tval == TV_LITE) && object_is_fixed_artifact(object_ptr))
	{
#ifdef JP
		*misc_list++ = "�i�v����(���a3)";
#else
		*misc_list++ = "Permanent Light(3)";
#endif
	}

	/*
	 * Glowing artifacts -- small radius light.
	 */
	if(have_flag(flgs, TRAIT_LITE))
	{
#ifdef JP
		*misc_list++ = "�i�v����(���a1)";
#else
		*misc_list++ = "Permanent Light(1)";
#endif
	}

	/*
	 * Handle cursed objects here to avoid redundancies such as noting
	 * that a permanently cursed object is heavily cursed as well as
	 * being "lightly cursed".
	 */

/*	if(object_is_cursed(object_ptr)) */
	{
		if(have_flag(flgs, TRAIT_TY_CURSE))
		{
#ifdef JP
			*misc_list++ = "���Ẩ��O";
#else
			*misc_list++ = "Ancient Curse";
#endif
		}
		if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE))
		{
#ifdef JP
			*misc_list++ = "�_��̎�";
#else
			*misc_list++ = "Divinely Cursed";
#endif
		}
		else if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))
		{
#ifdef JP
			*misc_list++ = "���͂Ȏ�";
#else
			*misc_list++ = "Heavily Cursed";
#endif
		}
/*		else */
		else if(have_flag(object_ptr->curse_flags, TRAIT_CURSED))
		{
#ifdef JP
			*misc_list++ = "��";
#else
			*misc_list++ = "Cursed";
#endif
		}
	}

	/* Terminate the description list */
	*misc_list = NULL;
}


/*
 * Note additional ability and/or resistance of fixed artifacts
 */
static void analyze_addition(object_type *object_ptr, char *addition)
{
	artifact_type *a_ptr = &artifact_info[object_ptr->name1];

	/* Init */
	strcpy(addition, "");

#ifdef JP
	if((have_flag(a_ptr->flags, TRAIT_XTRA_POWER)) && (have_flag(a_ptr->flags, TRAIT_XTRA_H_RES))) strcat(addition, "�\��and�ϐ�");
	else if(have_flag(a_ptr->flags, TRAIT_XTRA_POWER))
	{
		strcat(addition, "�\��");
		if(have_flag(a_ptr->flags, TRAIT_XTRA_RES_OR_POWER)) strcat(addition, "(1/2��and�ϐ�)");
	}
	else if(have_flag(a_ptr->flags, TRAIT_XTRA_H_RES))
	{
		strcat(addition, "�ϐ�");
		if(have_flag(a_ptr->flags, TRAIT_XTRA_RES_OR_POWER)) strcat(addition, "(1/2��and�\��)");
	}
	else if(have_flag(a_ptr->flags, TRAIT_XTRA_RES_OR_POWER)) strcat(addition, "�\��or�ϐ�");
#else
	if((have_flag(a_ptr->flag, TRAIT_XTRA_POWER)) && (have_flag(a_ptr->flags, TRAIT_XTRA_H_RES))) strcat(addition, "Ability and Resistance");
	else if(have_flag(a_ptr->flag, TRAIT_XTRA_POWER))
	{
		strcat(addition, "Ability");
		if(have_flag(a_ptr->flags, TRAIT_XTRA_RES_OR_POWER)) strcat(addition, "(plus Resistance about 1/2)");
	}
	else if(have_flag(a_ptr->flags, TRAIT_XTRA_H_RES))
	{
		strcat(addition, "Resistance");
		if(have_flag(a_ptr->flags, TRAIT_XTRA_RES_OR_POWER)) strcat(addition, "(plus Ability about 1/2)");
	}
	else if(have_flag(a_ptr->flags, TRAIT_XTRA_RES_OR_POWER)) strcat(addition, "Ability or Resistance");
#endif
}


/*
 * Determine the minimum depth an artifact can appear, its rarity, its weight,
 * and its value in gold pieces
 */
static void analyze_misc(object_type *object_ptr, char *misc_desc)
{
	char buf[80];
	artifact_type *a_ptr = &artifact_info[object_ptr->name1];

	format_weight(buf, object_ptr->weight);

#ifdef JP
	sprintf(misc_desc, "���x�� %u, �󏭓x %u, %s, ��%ld",
		a_ptr->level, a_ptr->rarity, buf, a_ptr->cost);
#else
	sprintf(misc_desc, "Level %u, Rarity %u, %s, %ld Gold",
		a_ptr->level, a_ptr->rarity, buf, a_ptr->cost);
#endif
}


/*
 * Fill in an object description structure for a given object
 */
static void object_analyze(object_type *object_ptr, obj_desc_list *desc_ptr)
{
	analyze_general(object_ptr, desc_ptr->description);
	analyze_pval(object_ptr, &desc_ptr->pval_info);
	analyze_brand(object_ptr, desc_ptr->brands);
	analyze_slay(object_ptr, desc_ptr->slays);
	analyze_immune(object_ptr, desc_ptr->immunities);
	analyze_resist(object_ptr, desc_ptr->resistances);
	analyze_sustains(object_ptr, desc_ptr->sustains);
	analyze_misc_magic(object_ptr, desc_ptr->misc_magic);
	analyze_addition(object_ptr, desc_ptr->addition);
	analyze_misc(object_ptr, desc_ptr->misc_desc);
	desc_ptr->activation = item_activation(object_ptr);
}


static void print_header(void)
{
	char buf[80];

	sprintf(buf, "Artifact Spoilers for D\'angband Version %d.%d.%d",
		VER_MAJOR, VER_MINOR, VER_PATCH);
	spoiler_underline(buf);
}

/*
 * This is somewhat ugly.
 *
 * Given a header ("Resist", e.g.), a list ("Fire", "Cold", Acid", e.g.),
 * and a separator character (',', e.g.), write the list to the spoiler file
 * in a "nice" format, such as:
 *
 *      Resist Fire, Cold, Acid
 *
 * That was a simple example, but when the list is long, a line wrap
 * should occur, and this should induce a new level of indention if
 * a list is being spread across lines. So for example, Bladeturner's
 * list of resistances should look something like this
 *
 *     Resist Acid, Lightning, Fire, Cold, Poison, Light, Dark, Blindness,
 *       Confusion, Sound, Shards, Nether, Nexus, Chaos, Disenchantment
 *
 * However, the code distinguishes between a single list of many items vs.
 * many lists. (The separator is used to make this determination.) A single
 * list of many items will not cause line wrapping (since there is no
 * apparent reason to do so). So the lists of Ulmo's miscellaneous traits_precondition
 * might look like this:
 *
 *     Free Action; Hold Life; See Invisible; Slow Digestion; Regeneration
 *     Blessed Blade
 *
 * So comparing the two, "Regeneration" has no trailing separator and
 * "Blessed Blade" was not indented. (Also, Ulmo's lists have no headers,
 * but that's not relevant to line wrapping and indention.)
 */

/* ITEM_SEP separates items within a list */
#define ITEM_SEP ','


/* LIST_SEP separates lists */
#ifdef JP
#define LIST_SEP ','
#else
#define LIST_SEP ';'
#endif

static void spoiler_outlist(cptr header, cptr *list, char separator)
{
	int line_len, buf_len;
	char line[MAX_LINE_LEN+1], buf[80];

	/* Ignore an empty list */
	if(*list == NULL) return;

	/* This function always indents */
	strcpy(line, INDENT1);

	/* Create header (if one was given) */
	if(header && (header[0]))
	{
		strcat(line, header);
		strcat(line, " ");
	}

	line_len = strlen(line);

	/* Now begin the tedious task */
	while (1)
	{
		/* Copy the current item to a buffer */
		strcpy(buf, *list);

		/* Note the buffer's length */
		buf_len = strlen(buf);

		/*
		 * If there is an item following this one, pad with separator and
		 * a space and adjust the buffer length
		 */

		if(list[1])
		{
			sprintf(buf + buf_len, "%c ", separator);
			buf_len += 2;
		}

		/*
		 * If the buffer will fit on the current line, just append the
		 * buffer to the line and adjust the line length accordingly.
		 */

		if(line_len + buf_len <= MAX_LINE_LEN)
		{
			strcat(line, buf);
			line_len += buf_len;
		}

		/* Apply line wrapping and indention semantics described above */
		else
		{
			/*
			 * Don't print a trailing list separator but do print a trailing
			 * item separator.
			 */
			if(line_len > 1 && line[line_len - 1] == ' '
			    && line[line_len - 2] == LIST_SEP)
			{
				/* Ignore space and separator */
				line[line_len - 2] = '\0';

				/* Write to spoiler file */
				fprintf(fff, "%s\n", line);

				/* Begin new line at primary indention level */
				sprintf(line, "%s%s", INDENT1, buf);
			}

			else
			{
				/* Write to spoiler file */
				fprintf(fff, "%s\n", line);

				/* Begin new line at secondary indention level */
				sprintf(line, "%s%s", INDENT2, buf);
			}

			line_len = strlen(line);
		}

		/* Advance, with break */
		if(!*++list) break;
	}

	/* Write what's left to the spoiler file */
	fprintf(fff, "%s\n", line);
}


/* Create a spoiler file entry for an artifact */

static void spoiler_print_art(obj_desc_list *art_ptr)
{
	pval_info_type *pval_ptr = &art_ptr->pval_info;

	char buf[80];

	/* Don't indent the first line */
	fprintf(fff, "%s\n", art_ptr->description);

	/* An "empty" pval description indicates that the pval affects nothing */
	if(pval_ptr->pval_desc[0])
	{
		/* Mention the effects of pval */
#ifdef JP
		sprintf(buf, "%s�̏C��:", pval_ptr->pval_desc);
#else
		sprintf(buf, "%s to", pval_ptr->pval_desc);
#endif
		spoiler_outlist(buf, pval_ptr->pval_affects, ITEM_SEP);
	}

	/* Now deal with the description lists */

#ifdef JP
	spoiler_outlist("��:", art_ptr->slays, ITEM_SEP);
	spoiler_outlist("���푮��:", art_ptr->brands, LIST_SEP);
	spoiler_outlist("�Ɖu:", art_ptr->immunities, ITEM_SEP);
	spoiler_outlist("�ϐ�:", art_ptr->resistances, ITEM_SEP);
	spoiler_outlist("�ێ�:", art_ptr->sustains, ITEM_SEP);
#else
	spoiler_outlist("Slay", art_ptr->slays, ITEM_SEP);
	spoiler_outlist("", art_ptr->brands, LIST_SEP);
	spoiler_outlist("Immunity to", art_ptr->immunities, ITEM_SEP);
	spoiler_outlist("Resist", art_ptr->resistances, ITEM_SEP);
	spoiler_outlist("Sustain", art_ptr->sustains, ITEM_SEP);
#endif
	spoiler_outlist("", art_ptr->misc_magic, LIST_SEP);

	if(art_ptr->addition[0])
	{
#ifdef JP
		fprintf(fff, "%s�ǉ�: %s\n", INDENT1, art_ptr->addition);
#else
		fprintf(fff, "%sAdditional %s\n", INDENT1, art_ptr->addition);
#endif
	}

	/* Write out the possible activation at the primary indention level */
	if(art_ptr->activation)
	{
#ifdef JP
		fprintf(fff, "%s����: %s\n", INDENT1, art_ptr->activation);
#else
		fprintf(fff, "%sActivates for %s\n", INDENT1, art_ptr->activation);
#endif
	}

	/* End with the miscellaneous facts */
	fprintf(fff, "%s%s\n\n", INDENT1, art_ptr->misc_desc);
}


/*
 * Hack -- Create a "forged" artifact
 */
static bool make_fake_artifact(object_type *object_ptr, int name1)
{
	int i;

	artifact_type *a_ptr = &artifact_info[name1];


	/* Ignore "empty" artifacts */
	if(!a_ptr->name) return FALSE;

	/* Acquire the "kind" index */
	i = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	if(!i) return (FALSE);

	/* Create the artifact */
	object_prep(object_ptr, i, ITEM_FREE_SIZE);

	/* Save the name */
	object_ptr->name1 = name1;

	/* Extract the fields */
	object_ptr->pval = a_ptr->pval;
	object_ptr->ac = a_ptr->ac;
	object_ptr->ev = a_ptr->ev;
	object_ptr->dd = a_ptr->dd;
	object_ptr->ds = a_ptr->ds;
	object_ptr->to_ac = a_ptr->to_ac;
	object_ptr->to_ev = a_ptr->to_ev;
	object_ptr->to_hit = a_ptr->to_hit;
	object_ptr->to_damage = a_ptr->to_damage;
	object_ptr->weight = a_ptr->weight;
	object_ptr->charge_const = a_ptr->charge_const;
	object_ptr->charge_dice = a_ptr->charge_dice;

	/* Success */
	return (TRUE);
}


/*
 * Create a spoiler file for artifacts
 */
static void spoil_artifact(cptr fname)
{
	int i, j;

	object_type forge;
	object_type *quest_ptr;

	obj_desc_list artifact;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}

	/* Dump the header */
	print_header();

	/* List the artifacts by tval */
	for (i = 0; group_artifact[i].tval; i++)
	{
		/* Write out the group title */
		if(group_artifact[i].name)
		{
			spoiler_blanklines(2);
			spoiler_underline(group_artifact[i].name);
			spoiler_blanklines(1);
		}

		/* Now search through all of the artifacts */
		for (j = 1; j < max_artifact_idx; ++j)
		{
			artifact_type *a_ptr = &artifact_info[j];

			/* We only want objects in the current group */
			if(a_ptr->tval != group_artifact[i].tval) continue;

			/* Get local object */
			quest_ptr = &forge;

			/* Wipe the object */
			object_wipe(quest_ptr);

			/* Attempt to "forge" the artifact */
			if(!make_fake_artifact(quest_ptr, j)) continue;

			/* Analyze the artifact */
			object_analyze(quest_ptr, &artifact);

			/* Write out the artifact description to the spoiler file */
			spoiler_print_art(&artifact);
		}
	}

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Message */
	msg_print("Successfully created a spoiler file.");
}





/*
 * Create a spoiler file for creatures   -BEN-
 */
static void spoil_species_desc(cptr fname)
{
	int i, j, n = 0;
	int tmpht, tmpwt, tmpsize;
	int hpdata[4];
	int stat[6];

	u16b why = 2;
	s16b *who;

	char buf[1024];

	char id[10];
	char nam[80];
	char lev[80];
	char rar[80];
	char spd[80];
	char ac[80];
	char hp[80];
	char hp_desc[80];
	char exp[80];
	char ht[80];
	char wt[80];
	char size[80];
	char div[80];
	char trait[160];

	char sa[10], ia[10], wa[10], da[10], ca[10], cha[10];

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}

	/* Allocate the "who" array */
	C_MAKE(who, max_species_idx, s16b);

	/* Dump the header */
	fprintf(fff, "Creature Spoilers for D\'angband Version %d.%d.%d\n",
		VER_MAJOR, VER_MINOR, VER_PATCH);
	fprintf(fff, "---------------------------------------------\n\n");

	/* Dump the header */
	fprintf(fff, "%5s     %-38.38s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%21s%6s%7s%7s%6s %-11.11s %-64s\n",
		"ID", "Name", "Lev", "Div", "Rar", "Spd", "ST", "IN", "WI", "DE", "CO", "CH",
		"Hp", "Ac", "Ht", "Wt", "Siz", "Visual Info", "Trait");
	fprintf(fff, "%5s %-42.42s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%21s%6s%7s%7s%6s %11.11s %-64s\n",
		"-----","--------", "-------", "---", "---", "---", "--", "--", "--", "--", "--", "--",
		"--------------------", "--", "--", "--", "-----", "-----------",
		"-----");


	/* Scan the creatures */
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];

		/* Use that creature */
		if(species_ptr->name) who[n++] = i;
	}

	/* Sort the array by dungeon depth of creatures */
	ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);

	/* Scan again */
	for (i = 0; i < n; i++)
	{
		species_type *species_ptr = &species_info[who[i]];

		cptr name = (species_name + species_ptr->name);

		sprintf(id, "%5d ", who[i]);

		//TODOif(is_shadow_species(species_ptr)) continue;
		/* Get the "name" */
		/*
		else if(has_trait_species(species_ptr, TRAIT_QUESTOR))
		{
			sprintf(nam, "[Q] %s", name);
		}
		*/
		if(has_trait_species(species_ptr, TRAIT_UNIQUE))
		{
			sprintf(nam, "[U] %s", name);
		}
		else
		{
#ifdef JP
			sprintf(nam, "    %s", name);
#else
			sprintf(nam, "The %s", name);
#endif
		}

		/* Divine Rank */
		if(species_ptr->dr >= 0)
			sprintf(div, "%d", species_ptr->dr);
		else
			sprintf(div, "Nr");
		

		/* Rarity */
		sprintf(rar, "%d", species_ptr->rarity);

		/* Speed */
		if(species_ptr->speed >= 0)
		{
			sprintf(spd, "+%d", (species_ptr->speed));
		}
		else
		{
			sprintf(spd, "-%d", (species_ptr->speed));
		}
		

		/* Base Status */
		for(j = STAT_STR; j <= STAT_CHA; j++)
		{
			stat[j] = species_ptr->stat_max[j] / 10;
			if(species_ptr->race_idx1 != INDEX_NONE)
				stat[j] += race_info[species_ptr->race_idx1].r_adj[j];
			if(species_ptr->class_idx != INDEX_NONE)
				stat[j] += class_info[species_ptr->class_idx].c_adj[j];
			/* TODO
			if(species_ptr->patron_idx != INDEX_NONE)
				stat[j] += player_patrons[species_ptr->class_idx].p_adj[j];*/
			if(species_ptr->chara_idx != INDEX_NONE)
				stat[j] += chara_info[species_ptr->chara_idx].a_adj[j];
		}

		if(has_trait_species(species_ptr, TRAIT_ELDRITCH_HORROR)){
			stat[STAT_CHA] += 5;		
		}

		if(has_trait_species(species_ptr, TRAIT_POWERFUL)){
			for(j = STAT_STR; j <= STAT_CHA; j++)
			{
				stat[j] += 2;
			}
		}

		for(j = STAT_STR; j <= STAT_CHA; j++)
		{
			if(stat[j] < 3) stat[j] = 3;
		}

		if((species_ptr->blow[0].method == RBM_NONE || species_ptr->blow[0].method >= RBM_NONDEX_ATTACK) && has_trait_species(species_ptr, TRAIT_NEVER_MOVE))
			sprintf(sa, "--");
		else
			sprintf(sa, "%2d", stat[0]);

		if(has_trait_species(species_ptr, TRAIT_EMPTY_MIND))
			sprintf(ia, "--");
		else
			sprintf(ia, "%2d", stat[1]);

		if(has_trait_species(species_ptr, TRAIT_EMPTY_MIND) || has_trait_species(species_ptr, TRAIT_WEIRD_MIND))
			sprintf(wa, "--");
		else
			sprintf(wa, "%2d", stat[2]);

		if((species_ptr->blow[0].method == RBM_NONE || species_ptr->blow[0].method >= RBM_NONDEX_ATTACK) && has_trait_species(species_ptr, TRAIT_NEVER_MOVE))
			sprintf(da, "--");
		else
			sprintf(da, "%2d", stat[3]);

		sprintf(ca, "%2d", stat[4]);

		if(has_trait_species(species_ptr, TRAIT_WEIRD_MIND) || has_trait_species(species_ptr, TRAIT_WEIRD_MIND))
			sprintf(cha, "--");
		else
			sprintf(cha, "%2d", stat[5]);


		/* Armor Class */
		sprintf(ac, "%d", species_ptr->ac);

		/* Hitpoints */
		//TODO:: EstimateHP
		estimate_enemy_hp(species_ptr, hpdata);
		sprintf(hp, "%5d", hpdata[0]);
		if(species_ptr->dr >= 1)
		{
			sprintf(hp_desc, "(%dx%d%+d)", hpdata[1], hpdata[2], hpdata[3]);
		}
		else
		{
			sprintf(hp_desc, "(%dd%d%+d)", hpdata[1], hpdata[2], hpdata[3]);
		}

		/* height & weight & size */

		if(IS_MALE(species_ptr))
		{
			tmpht = species_ptr->m_b_ht;
			tmpwt = species_ptr->m_b_wt;
		}
		else if(IS_FEMALE(species_ptr))
		{
			tmpht = species_ptr->f_b_ht;
			tmpwt = species_ptr->f_b_wt;
		}
		else
		{
			tmpht = species_ptr->m_b_ht;
			tmpwt = species_ptr->m_b_wt;
		}

		//TODOif(is_variable_race_species(species_ptr) | is_variable_size_species(species_ptr)) sprintf(ht, "----");
		if(tmpht < 1000) sprintf(ht, "%dcm", tmpht);
		else if(tmpht < 1000000) sprintf(ht, "%dm ", tmpht / 100);
		else sprintf(ht, "%dkm", tmpht / 100000);

		//TODOif(is_variable_race_species(species_ptr) | is_variable_size_species(species_ptr)) sprintf(wt, "----");
		if(tmpwt < 10000) sprintf(wt, "%dkg", tmpwt);
		else if(tmpwt < 10000000) sprintf(wt, "%dt ", tmpwt / 1000);
		else sprintf(wt, "%dKt", tmpwt / 1000000);

		tmpsize = calc_bodysize(tmpht, tmpwt);

		if(is_variable_race_species(species_ptr)) sprintf(size, "B%+2d ", tmpsize - 10);
		//TODO else if(is_variable_size_species(species_ptr)) sprintf(size, "%3d+", tmpsize);
		else  sprintf(size, "%4d ", tmpsize);

		/* Experience */
		sprintf(exp, "%ld", (long)(species_ptr->exp));

		/* Hack -- use visual instead */
		if(is_variable_race_species(species_ptr))
		{
			sprintf(exp, "%s ---", attr_to_text(species_ptr));
		}
		else
		{
			sprintf(exp, "%s [%c]", attr_to_text(species_ptr), species_ptr->d_char);
		}

		/* Trait */
		trait[0] = '\0';

		if(is_variable_race_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "�푰��");
#else
			strcat(trait, "VariableRace");
#endif

		}
		else if(species_ptr->race_idx1 < MAX_RACES)
		{
			strcat(trait, "/�푰:");
			strcat(trait, race_info[species_ptr->race_idx1].title);
		}

		if(is_variable_class_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "�E�Ɖ�");
#else
			strcat(trait, "VariableClass");
#endif

		}
		else if(species_ptr->class_idx < MAX_CLASS)
		{
			strcat(trait, "/�E��:");
			strcat(trait, class_info[species_ptr->class_idx].title);
		}

		if(is_variable_chara_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "���i��");
#else
			strcat(trait, "VariableCharacter");
#endif

		}
		else if(species_ptr->chara_idx < MAX_CHARA)
		{
			strcat(trait, "/���i:");
			strcat(trait, chara_info[species_ptr->chara_idx].title);
		}

		if(is_variable_patron_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "��_��");
#else
			strcat(trait, "VariableCharacter");
#endif

		}
/*TODO
		if(is_force_lesser_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "�򓙃G�S");
#else
			strcat(trait, "Lesser");
#endif
		}
*/
/*
		if(is_variable_size_species(species_ptr)) 
		{
			strcat(trait, "/");
#ifdef JP
			strcat(trait, "�T�C�Y��");
#else
			strcat(trait, "VariableSize");
#endif
		}
*/

		if(strlen(trait) == 0)
			trait[1] = '\0';

/* Dump the info */
		fprintf(fff, "%6s%-42.42s%8s%4s%4s%4s%3s%3s%3s%3s%3s%3s%6s%-15s%6s%7s%7s%6s %11.11s %s\n",
			id, nam, lev, div, rar, spd, sa, ia, wa, da, ca, cha, hp, hp_desc, ac, ht, wt, size, exp, &trait[1]);
	}

	/* End it */
	fprintf(fff, "\n");


	/* Free the "who" array */
	C_KILL(who, max_species_idx, s16b);

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Worked */
	msg_print("Successfully created a spoiler file.");
}




/*
 * Creature spoilers by: smchorse@ringer.cs.utsa.edu (Shawn McHorse)
 *
 * Primarily based on code already in mon-desc.c, mostly by -BEN-
 */



/*
 * Buffer text to the given file. (-SHAWN-)
 * This is basically c_roff() from mon-desc.c with a few changes.
 */
static void spoil_out(cptr str)
{
	cptr r;

	/* Line buffer */
	static char roff_buf[256];

	/* Delay buffer */
	static char roff_waiting_buf[256];

#ifdef JP
	bool iskanji_flag = FALSE;
#endif
	/* Current pointer into line roff_buf */
	static char *roff_p = roff_buf;

	/* Last space saved into roff_buf */
	static char *roff_s = NULL;

	/* Mega-Hack -- Delayed output */
	static bool waiting_output = FALSE;

	/* Special handling for "new sequence" */
	if(!str)
	{
		if(waiting_output)
		{
			fputs(roff_waiting_buf, fff);
			waiting_output = FALSE;
		}

		if(roff_p != roff_buf) roff_p--;
		while (*roff_p == ' ' && roff_p != roff_buf) roff_p--;

		if(roff_p == roff_buf) fprintf(fff, "\n");
		else
		{
			*(roff_p + 1) = '\0';
			fprintf(fff, "%s\n\n", roff_buf);
		}

		roff_p = roff_buf;
		roff_s = NULL;
		roff_buf[0] = '\0';
		return;
	}

	/* Scan the given string, character at a time */
	for (; *str; str++)
	{
#ifdef JP
		char cbak;
		bool k_flag = iskanji((unsigned char)(*str));
#endif
		char ch = *str;
		bool wrap = (ch == '\n');

#ifdef JP
		if(!isprint(ch) && !k_flag && !iskanji_flag) ch = ' ';
		iskanji_flag = k_flag && !iskanji_flag;
#else
		if(!isprint(ch)) ch = ' ';
#endif

		if(waiting_output)
		{
			fputs(roff_waiting_buf, fff);
			if(!wrap) fputc('\n', fff);
			waiting_output = FALSE;
		}

		if(!wrap)
		{
#ifdef JP
			if(roff_p >= roff_buf + (k_flag ? 74 : 75)) wrap = TRUE;
			else if((ch == ' ') && (roff_p >= roff_buf + (k_flag ? 72 : 73))) wrap = TRUE;
#else
			if(roff_p >= roff_buf + 75) wrap = TRUE;
			else if((ch == ' ') && (roff_p >= roff_buf + 73)) wrap = TRUE;
#endif

			if(wrap)
			{
#ifdef JP
				bool k_flag_local;
				bool iskanji_flag_local = FALSE;
				cptr tail = str + (k_flag ? 2 : 1);
#else
				cptr tail = str + 1;
#endif

				for (; *tail; tail++)
				{
					if(*tail == ' ') continue;

#ifdef JP
					k_flag_local = iskanji((unsigned char)(*tail));
					if(isprint(*tail) || k_flag_local || iskanji_flag_local) break;
					iskanji_flag_local = k_flag_local && !iskanji_flag_local;
#else
					if(isprint(*tail)) break;
#endif
				}

				if(!*tail) waiting_output = TRUE;
			}
		}

		/* Handle line-wrap */
		if(wrap)
		{
			*roff_p = '\0';
			r = roff_p;
#ifdef JP
			cbak = ' ';
#endif
			if(roff_s && (ch != ' '))
			{
#ifdef JP
				cbak = *roff_s;
#endif
				*roff_s = '\0';
				r = roff_s + 1;
			}
			if(!waiting_output) fprintf(fff, "%s\n", roff_buf);
			else strcpy(roff_waiting_buf, roff_buf);
			roff_s = NULL;
			roff_p = roff_buf;
#ifdef JP
			if(cbak != ' ') *roff_p++ = cbak;
#endif
			while (*r) *roff_p++ = *r++;
		}

		/* Save the char */
		if((roff_p > roff_buf) || (ch != ' '))
		{
#ifdef JP
			if(!k_flag)
			{
				if((ch == ' ') || (ch == '(')) roff_s = roff_p;
			}
			else
			{
				if(iskanji_flag &&
				    strncmp(str, "�B", 2) != 0 &&
				    strncmp(str, "�A", 2) != 0 &&
				    strncmp(str, "�B", 2) != 0 &&
				    strncmp(str, "�[", 2) != 0) roff_s = roff_p;
			}
#else
			if(ch == ' ') roff_s = roff_p;
#endif

			*roff_p++ = ch;
		}
	}
}



/*
 *  Hook function used in spoil_species_info()
 */
static void roff_func(byte attr, cptr str)
{
	/* Unused */
	(void)attr;

	spoil_out(str);
}


/*
 * Create a spoiler file for creatures (-SHAWN-)
 */
static void spoil_species_info(cptr fname)
{
	char buf[1024];
	int i, l, n = 0;
	int tmpht;
	int hpdata[4];

	u16b why = 2;
	s16b *who;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
		return;
	}


	/* Dump the header */
	sprintf(buf, "Creature Spoilers for D\'angband Version %d.%d.%d\n",
	     VER_MAJOR, VER_MINOR, VER_PATCH);

	spoil_out(buf);
	spoil_out("------------------------------------------\n\n");

	/* Allocate the "who" array */
	C_MAKE(who, max_species_idx, s16b);

	/* Scan the creatures */
	for (i = 1; i < max_species_idx; i++)
	{
		species_type *species_ptr = &species_info[i];

		/* Use that creature */
		if(species_ptr->name) who[n++] = i;
	}

	/* Sort the array by dungeon depth of creatures */
	ang_sort(who, &why, n, ang_sort_comp_hook, ang_sort_swap_hook);


	/*
	 * List all creatures in order
	 */
	for (l = 0; l < n; l++)
	{
		species_type *species_ptr = &species_info[who[l]];

		// TODO: Prefix
		if(has_trait_species(species_ptr, TRAIT_QUESTOR)) spoil_out("[Q] ");
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) spoil_out("[U] ");

		{
#ifndef JP
			spoil_out("The ");
#endif
		}

		/* Name */
#ifdef JP
		sprintf(buf, "%s/%s  (", (species_name + species_ptr->name),(species_name+species_ptr->E_name));  /* ---)--- */
#else
		sprintf(buf, "%s  (", (species_name + species_ptr->name));  /* ---)--- */
#endif

		spoil_out(buf);

		/* Color */
		spoil_out(attr_to_text(species_ptr));

		/* Symbol --(-- */
		sprintf(buf, " '%c')\n", species_ptr->d_char);
		spoil_out(buf);


		/* Indent */
		sprintf(buf, "=== ");
		spoil_out(buf);

		/* Number */
		sprintf(buf, "Num:%d  ", who[l]);
		spoil_out(buf);

		/* Level */
		sprintf(buf, "Lev:%d  ", species_ptr->level);
		spoil_out(buf);

		/* Rarity */
		sprintf(buf, "Rar:%d  ", species_ptr->rarity);
		spoil_out(buf);

		/* Rarity */
		sprintf(buf, "Div:%d  ", species_ptr->dr);
		spoil_out(buf);

		/* Speed */
		if(species_ptr->speed >= 0)
		{
			sprintf(buf, "Spd:+%d  ", (species_ptr->speed));
		}
		else
		{
			sprintf(buf, "Spd:-%d  ", (species_ptr->speed));
		}
		spoil_out(buf);

		/* Height and Weight */
		spoil_out("Height:");
		tmpht = (species_ptr->m_b_ht + species_ptr->f_b_ht) / 2;
		if(tmpht < 1000) sprintf(buf, "%dcm  ", tmpht);
		else if(tmpht < 1000000) sprintf(buf, "%dm   ", tmpht / 100);
		else sprintf(buf, "%dkm  ", tmpht / 100000);
		spoil_out(buf);

		spoil_out("Weight:");
		tmpht = (species_ptr->m_b_wt + species_ptr->f_b_wt) / 2;
		if(tmpht < 10000) sprintf(buf, "%dkg  ", tmpht);
		else if(tmpht < 10000000) sprintf(buf, "%dt   ", tmpht / 1000);
		else sprintf(buf, "%dKt  ", tmpht / 1000000);
		spoil_out(buf);

		/* Base Status */
		sprintf(buf, "\n=== STR:%2d INT:%2d WIS:%2d DEX:%2d CON:%2d CHR:%2d\n=== ",
		        species_ptr->stat_max[STAT_STR] / STAT_FRACTION, species_ptr->stat_max[STAT_INT] / STAT_FRACTION,
		        species_ptr->stat_max[STAT_WIS] / STAT_FRACTION, species_ptr->stat_max[STAT_DEX] / STAT_FRACTION,
		        species_ptr->stat_max[STAT_CON] / STAT_FRACTION, species_ptr->stat_max[STAT_CHA] / STAT_FRACTION);
		spoil_out(buf);


		/* Hitpoints */
		estimate_enemy_hp(species_ptr, hpdata);
		sprintf(buf, "Hp:%d (%dx%d+%d)", hpdata[0], hpdata[1], hpdata[2], hpdata[3]);

		spoil_out(buf);

		/* Armor Class */
		sprintf(buf, "Ac:%d  ", species_ptr->ac);
		spoil_out(buf);

		/* Experience */
		sprintf(buf, "Exp:%ld  ", (long)(species_ptr->exp));
		spoil_out(buf);

		/* Size */
		spoil_out("Size:");
		tmpht = calc_bodysize((species_ptr->m_b_ht + species_ptr->f_b_ht) / 2,
		                      (species_ptr->m_b_wt + species_ptr->f_b_wt) / 2);
		if(tmpht < 1000) sprintf(buf, "%d  ", tmpht);
		else sprintf(buf, "***  ", tmpht);
		spoil_out(buf);

		spoil_out("\n");

		/* Reuse the code of creature recall. */
		output_creature_spoiler(who[l], roff_func);

		spoil_out(NULL);
	}

	/* Free the "who" array */
	C_KILL(who, max_species_idx, s16b);

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	msg_print("Successfully created a spoiler file.");
}



#define MAX_EVOL_DEPTH 64


/*
 * Compare two int-type array like strncmp() and return TRUE if equals
 */
static bool int_n_cmp(int *a, int *b, int length)
{
	/* Null-string comparation is always TRUE */
	if(!length) return TRUE;

	do
	{
		if(*a != *(b++)) return FALSE;
		if(!(*(a++))) break;
	}
	while (--length);

	return TRUE;
}


/*
 * Returns TRUE if an evolution tree is "partial tree"
 */
static bool is_partial_tree(int *tree, int *partial_tree)
{
	int pt_head = *(partial_tree++);
	int pt_len = 0;

	while (partial_tree[pt_len]) pt_len++;

	while (*tree)
	{
		if(*(tree++) == pt_head)
		{
			if(int_n_cmp(tree, partial_tree, pt_len)) return TRUE;
		}
	}

	return FALSE;
}


/*
 * Sorting hook -- Comp function
 */
static bool ang_sort_comp_evol_tree(vptr u, vptr v, int a, int b)
{
	int **evol_tree = (int **)u;

	int w1 = evol_tree[a][0];
	int w2 = evol_tree[b][0];
	species_type *r1_ptr = &species_info[w1];
	species_type *r2_ptr = &species_info[w2];

	/* Unused */
	(void)v;

	/* Used tree first */
	if(w1 && !w2) return TRUE;
	if(!w1 && w2) return FALSE;

	/* Sort by creature level */
	if(r1_ptr->level < r2_ptr->level) return TRUE;
	if(r1_ptr->level > r2_ptr->level) return FALSE;

	/* Sort by creature experience */
	if(r1_ptr->exp < r2_ptr->exp) return TRUE;
	if(r1_ptr->exp > r2_ptr->exp) return FALSE;

	/* Compare indexes */
	return w1 <= w2;
}


/*
 * Sorting hook -- Swap function
 */
static void ang_sort_swap_evol_tree(vptr u, vptr v, int a, int b)
{
	int **evol_tree = (int **)u;
	int *holder;

	/* Unused */
	(void)v;

	/* Swap */
	holder = evol_tree[a];
	evol_tree[a] = evol_tree[b];
	evol_tree[b] = holder;
}


/*
 * Print creatures' evolution information to file
 */
static void spoil_species_evol(cptr fname)
{
	char buf[1024];
	species_type *r_ptr;
	int **evol_tree, i, j, n, species_idx;
	int *evol_tree_zero; /* For C_KILL() */

	/* Build the filename */
	path_build(buf, sizeof buf, ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if(!fff)
	{
		msg_print("Cannot create spoiler file.");
	    return;
	}

	/* Dump the header */
	sprintf(buf, "Creature Spoilers for D\'angband Version %d.%d.%d\n",
	     VER_MAJOR, VER_MINOR, VER_PATCH);

	spoil_out(buf);
	spoil_out("------------------------------------------\n\n");

	/* Allocate the "evol_tree" array (2-dimension) */
	C_MAKE(evol_tree, max_species_idx, int *);
	C_MAKE(*evol_tree, max_species_idx * (MAX_EVOL_DEPTH + 1), int);
	for (i = 1; i < max_species_idx; i++) evol_tree[i] = *evol_tree + i * (MAX_EVOL_DEPTH + 1);
	evol_tree_zero = *evol_tree;

	/* Step 1: Build the evolution tree */
	for (i = 1; i < max_species_idx; i++)
	{
		r_ptr = &species_info[i];

		/* No evolution */
		if(!r_ptr->next_exp) continue;

		/* Trace evolution */
		n = 0;
		evol_tree[i][n++] = i;
		do
		{
			evol_tree[i][n++] = r_ptr->next_species_idx;
			r_ptr = &species_info[r_ptr->next_species_idx];
		}
		while (r_ptr->next_exp && (n < MAX_EVOL_DEPTH));
	}

	/* Step 2: Scan the evolution trees and remove "partial tree" */
	for (i = 1; i < max_species_idx; i++)
	{
		/* Not evolution tree */
		if(!evol_tree[i][0]) continue;

		for (j = 1; j < max_species_idx; j++)
		{
			/* Same tree */
			if(i == j) continue;

			/* Not evolution tree */
			if(!evol_tree[j][0]) continue;

			/* Is evolution tree[i] is part of [j]? */
			if(is_partial_tree(evol_tree[j], evol_tree[i]))
			{
				/* Remove this evolution tree */
				evol_tree[i][0] = 0;
				break;
			}
		}
	}

	/* Step 3: Sort the evolution trees */

	/* Sort the array */
	ang_sort(evol_tree, NULL, max_species_idx, ang_sort_comp_evol_tree, ang_sort_swap_evol_tree);

	/* Step 4: Print the evolution trees */
	for (i = 0; i < max_species_idx; i++)
	{
		species_idx = evol_tree[i][0];

		/* No evolution or removed evolution tree */
		if(!species_idx) continue;

		/* Trace the evolution tree */
		r_ptr = &species_info[species_idx];
#ifdef JP
		fprintf(fff, "[%d]: %s (���x��%d, '%c')\n", species_idx,
			species_name + r_ptr->name, r_ptr->level, r_ptr->d_char);
#else
		fprintf(fff, "[%d]: %s (Level %d, '%c')\n", species_idx,
			species_name + r_ptr->name, r_ptr->level, r_ptr->d_char);
#endif
		for (n = 1; r_ptr->next_exp; n++)
		{
			fprintf(fff, "%*s-(%ld)-> ", n * 2, "", r_ptr->next_exp);
			fprintf(fff, "[%d]: ", r_ptr->next_species_idx);
			r_ptr = &species_info[r_ptr->next_species_idx];
#ifdef JP
			fprintf(fff, "%s (���x��%d, '%c')\n",
				species_name + r_ptr->name, r_ptr->level, r_ptr->d_char);
#else
			fprintf(fff, "%s (Level %d, '%c')\n",
				species_name + r_ptr->name, r_ptr->level, r_ptr->d_char);
#endif
		}

		/* End of evolution tree */
		fputc('\n', fff);
	}

	/* Free the "evol_tree" array (2-dimension) */
	C_KILL(evol_tree_zero, max_species_idx * (MAX_EVOL_DEPTH + 1), int);
	C_KILL(evol_tree, max_species_idx, int *);

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close spoiler file.");
		return;
	}

	/* Message */
	msg_print("Successfully created a spoiler file.");
}



/*
 * Forward declare
 */
extern void do_cmd_spoilers(void);

/*
 * Create Spoiler files -BEN-
 */
void do_cmd_spoilers(void)
{
	/* Save the screen */
	screen_save();

	/* Interact */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Info */
		prt("Create a spoiler file.", 2, 0);

		/* Prompt for a file */
		prt("(1) Brief Object Info (obj-desc.spo)", 5, 5);
		prt("(2) Brief Artifact Info (artifact.spo)", 6, 5);
		prt("(3) Brief Creature Info (mon-desc.spo)", 7, 5);
		prt("(4) Full Creature Info (mon-info.spo)", 8, 5);
		prt("(5) Creature Evolution Info (mon-evol.spo)", 9, 5);

		/* Prompt */
#ifdef JP
		prt("�R�}���h:", 18, 0);
#else
		prt("Command: ", 12, 0);
#endif

		/* Get a choice */
		switch (inkey())
		{
		/* Escape */
		case ESCAPE:
			/* Restore the screen */
			screen_load();
			return;

		/* Option (1) */
		case '1':
			spoil_obj_desc("obj-desc.spo");
			break;

		/* Option (2) */
		case '2':
			spoil_artifact("artifact.spo");
			break;

		/* Option (3) */
		case '3':
			spoil_species_desc("mon-desc.spo");
			break;

		/* Option (4) */
		case '4':
			spoil_species_info("mon-info.spo");
			break;

		/* Option (5) */
		case '5':
			spoil_species_evol("mon-evol.spo");
			break;

		/* Oops */
		default:
			bell();
			break;
		}

		/* Flush messages */
		msg_print(NULL);
	}
}

/*
 * Fill in an object description structure for a given object
 */
static void random_artifact_analyze(object_type *object_ptr, obj_desc_list *desc_ptr)
{
	char buf[80];
	format_weight(buf, object_ptr->weight);

	analyze_general(object_ptr, desc_ptr->description);
	analyze_pval(object_ptr, &desc_ptr->pval_info);
	analyze_brand(object_ptr, desc_ptr->brands);
	analyze_slay(object_ptr, desc_ptr->slays);
	analyze_immune(object_ptr, desc_ptr->immunities);
	analyze_resist(object_ptr, desc_ptr->resistances);
	analyze_sustains(object_ptr, desc_ptr->sustains);
	analyze_misc_magic(object_ptr, desc_ptr->misc_magic);
	desc_ptr->activation = item_activation(object_ptr);
#ifdef JP
	sprintf(desc_ptr->misc_desc, "�d�� %s", buf);
#else
	sprintf(desc_ptr->misc_desc, "Weight %s", buf);
#endif
}

/* Create a spoiler file entry for an artifact */

static void spoiler_print_randart(object_type *object_ptr, obj_desc_list *art_ptr)
{
	pval_info_type *pval_ptr = &art_ptr->pval_info;

	char buf[80];

	/* Don't indent the first line */
	fprintf(fff, "%s\n", art_ptr->description);
	
	/* unidentified */
	if(!(object_ptr->ident & (IDENT_MENTAL)))
	{
#ifdef JP
		fprintf(fff, "%s�s��\n",INDENT1);
#else
		fprintf(fff, "%sUnknown\n",INDENT1);
#endif
	}
	else {
		/* An "empty" pval description indicates that the pval affects nothing */
		if(pval_ptr->pval_desc[0])
		{
			/* Mention the effects of pval */
#ifdef JP
			sprintf(buf, "%s�̏C��:", pval_ptr->pval_desc);
#else
			sprintf(buf, "%s to", pval_ptr->pval_desc);
#endif
			spoiler_outlist(buf, pval_ptr->pval_affects, ITEM_SEP);
		}

		/* Now deal with the description lists */

#ifdef JP
		spoiler_outlist("��:", art_ptr->slays, ITEM_SEP);
		spoiler_outlist("���푮��:", art_ptr->brands, LIST_SEP);
		spoiler_outlist("�Ɖu:", art_ptr->immunities, ITEM_SEP);
		spoiler_outlist("�ϐ�:", art_ptr->resistances, ITEM_SEP);
		spoiler_outlist("�ێ�:", art_ptr->sustains, ITEM_SEP);
#else
		spoiler_outlist("Slay", art_ptr->slays, ITEM_SEP);
		spoiler_outlist("", art_ptr->brands, LIST_SEP);
		spoiler_outlist("Immunity to", art_ptr->immunities, ITEM_SEP);
		spoiler_outlist("Resist", art_ptr->resistances, ITEM_SEP);
		spoiler_outlist("Sustain", art_ptr->sustains, ITEM_SEP);
#endif
		spoiler_outlist("", art_ptr->misc_magic, LIST_SEP);

		/* Write out the possible activation at the primary indention level */
		if(art_ptr->activation)
		{
#ifdef JP
			fprintf(fff, "%s����: %s\n", INDENT1, art_ptr->activation);
#else
			fprintf(fff, "%sActivates for %s\n", INDENT1, art_ptr->activation);
#endif
		}
	}
	/* End with the miscellaneous facts */
	fprintf(fff, "%s%s\n\n", INDENT1, art_ptr->misc_desc);
}

/* Create a part of file for random artifacts */

static void spoil_random_artifact_aux(object_type *object_ptr, int i)
{
	obj_desc_list artifact;

	if(!object_is_known(object_ptr) || !object_ptr->art_name
		|| object_ptr->tval != group_artifact[i].tval)
		return;

	/* Analyze the artifact */
	random_artifact_analyze(object_ptr, &artifact);

	/* Write out the artifact description to the spoiler file */
	spoiler_print_randart(object_ptr, &artifact);
}

/*
 * Create a list file for random artifacts
 */
void spoil_random_artifact(creature_type *creature_ptr, cptr fname)
{
	int i,j;

	//store_type  *st_ptr;
	object_type *quest_ptr;

	char buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Open the file */
	fff = my_fopen(buf, "w");

	/* Oops */
	if(!fff)
	{
		msg_print("Cannot create list file.");
		return;
	}

	/* Dump the header */
	sprintf(buf, "Random artifacts list.\r");
	spoiler_underline(buf);

	/* List the artifacts by tval */
	for (j = 0; group_artifact[j].tval; j++)
	{
		/* random artifacts wielding */
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			/* Skip no equip */
			quest_ptr = &creature_ptr->inventory[i];
			if(!IS_EQUIPPED(quest_ptr)) continue;
			spoil_random_artifact_aux(quest_ptr, j);
		}

		/* random artifacts in creature_ptr */
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			quest_ptr = &creature_ptr->inventory[i];
			spoil_random_artifact_aux(quest_ptr, j);
		}

		/* random artifacts in home */
		/*
		st_ptr = &town[1].store[STORE_HOME];
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			quest_ptr = &st_ptr->stock[i];
			spoil_random_artifact_aux(quest_ptr, j);
		}
		*/

		/* random artifacts in museum */
		/*
		st_ptr = &town[1].store[STORE_MUSEUM];
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			quest_ptr = &st_ptr->stock[i];
			spoil_random_artifact_aux(quest_ptr, j);
		}
		*/
	}

	/* Check for errors */
	if(ferror(fff) || my_fclose(fff))
	{
		msg_print("Cannot close list file.");
		return;
	}

	/* Message */
	msg_print("Successfully created a list file.");
}

#else

#ifdef MACINTOSH
static int i = 0;
#endif /* MACINTOSH */

#endif
