/* File: avatar.c */

/*
 * Purpose: Enable an Ultima IV style "avatar" game where you try to
 * achieve perfection in various virtues.
 *
 * Topi Ylinen 1998
 * f1toyl@uta.fi
 * topi.ylinen@noodi.fi
 *
 */

/*
 * Copyright (c) 1989 James E. Wilson, Christopher J. Stuart
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/* The names of the virtues */

cptr virtue[MAX_VIRTUE] =
{
#ifdef JP
	"èÓ",
	"ó_",
	"ê≥",
	"ã]",
	"éØ",
	"êΩ",
	"å[",
	"îÈ",
	"â^",
	"ëR",
	"í≤",
	"äà",
	"éÄ",
	"îE",
	"êﬂ",
	"ãŒ",
	"óE",
	"å¬",
#else
	"Compassion",
	"Honour",
	"Justice",
	"Sacrifice",
	"Knowledge",
	"Faith",
	"Enlightenment",
	"Mysticism",
	"Chance",
	"Nature",
	"Harmony",
	"Vitality",
	"Unlife",
	"Patience",
	"Temperance",
	"Diligence",
	"Valour",
	"Individualism",
#endif
};


bool compare_virtue(creature_type *cr_ptr, int type, int num, int tekitou)
{
	int vir;
	if (virtue_number(p_ptr, type))
		vir = cr_ptr->virtues[virtue_number(p_ptr, type) - 1];
	else
		vir = 0;

	switch (tekitou)
	{
	case VIRTUE_LARGE:
		if (vir > num) return TRUE;
		else return FALSE;
	case VIRTUE_SMALL:
		if (vir < num) return TRUE;
		else return FALSE;
	}

	return FALSE;
}


/* Aux function */

int virtue_number(creature_type *cr_ptr, int type)
{
	int i;

	/* Search */
	for (i = 0; i < 8; i++)
	{
		if (p_ptr->vir_types[i] == type) return i + 1;
	}

	/* No match */
	return 0;
}

/* Aux function */

static void get_random_virtue(creature_type *cr_ptr, int which)
{
	int type = 0;

	/* Randomly choose a type */
	while (!(type) || virtue_number(cr_ptr, type))
	{
		switch (randint1(29))
		{
		case 1: case 2: case 3:
			type = V_SACRIFICE;
			break;
		case 4: case 5: case 6:
			type = V_COMPASSION;
			break;
		case 7: case 8: case 9: case 10: case 11: case 12:
			type = V_VALOUR;
			break;
		case 13: case 14: case 15: case 16: case 17:
			type = V_HONOUR;
			break;
		case 18: case 19: case 20: case 21:
			type = V_JUSTICE;
			break;
		case 22: case 23:
			type = V_TEMPERANCE;
			break;
		case 24: case 25:
			type = V_HARMONY;
			break;
		case 26: case 27: case 28:
			type = V_PATIENCE;
			break;
		default:
			type = V_DILIGENCE;
			break;
		}
	}

	/* Chosen */
	cr_ptr->vir_types[which] = type;
}

static s16b get_realm_virtues(creature_type *cr_ptr, byte realm)
{
	switch (realm)
	{
	case REALM_LIFE:
		if (virtue_number(cr_ptr, V_VITALITY)) return V_TEMPERANCE;
		else return V_VITALITY;
	case REALM_SORCERY:
		if (virtue_number(cr_ptr, V_KNOWLEDGE)) return V_ENCHANT;
		else return V_KNOWLEDGE;
	case REALM_NATURE:
		if (virtue_number(cr_ptr, V_NATURE)) return V_HARMONY;
		else return V_NATURE;
	case REALM_CHAOS:
		if (virtue_number(cr_ptr, V_CHANCE)) return V_INDIVIDUALISM;
		else return V_CHANCE;
	case REALM_DEATH:
		return V_UNLIFE;
	case REALM_TRUMP:
		return V_KNOWLEDGE;
	case REALM_ARCANE:
		return 0;
	case REALM_CRAFT:
		if (virtue_number(cr_ptr, V_ENCHANT)) return V_INDIVIDUALISM;
		else return V_ENCHANT;
	case REALM_DAEMON:
		if (virtue_number(cr_ptr, V_JUSTICE)) return V_FAITH;
		else return V_JUSTICE;
	case REALM_CRUSADE:
		if (virtue_number(cr_ptr, V_JUSTICE)) return V_HONOUR;
		else return V_JUSTICE;
	case REALM_HEX:
		if (virtue_number(cr_ptr, V_COMPASSION)) return V_JUSTICE;
		else return V_COMPASSION;
	};

	return 0;
}

/* Select virtues & reset values for a new character */

void get_virtues(creature_type *cr_ptr)
{
	int i = 0, j = 0;
	s16b tmp_vir;

	/* Reset */
	for (i = 0; i < 8; i++)
	{
		cr_ptr->virtues[i] = 0;
		cr_ptr->vir_types[i] = 0;
	}

	i = 0;

	/* Get pre-defined types */
	/* 1 or more virtues based on class */
	switch (cr_ptr->cls_idx)
	{
	case CLASS_WARRIOR:
	case CLASS_SAMURAI:
		cr_ptr->vir_types[i++] = V_VALOUR;
		cr_ptr->vir_types[i++] = V_HONOUR;
		break;
	case CLASS_MAGE:
		cr_ptr->vir_types[i++] = V_KNOWLEDGE;
		cr_ptr->vir_types[i++] = V_ENCHANT;
		break;
	case CLASS_PRIEST:
		cr_ptr->vir_types[i++] = V_FAITH;
		cr_ptr->vir_types[i++] = V_TEMPERANCE;
		break;
	case CLASS_ROGUE:
	case CLASS_SNIPER:
		cr_ptr->vir_types[i++] = V_HONOUR;
		break;
	case CLASS_RANGER:
	case CLASS_ARCHER:
		cr_ptr->vir_types[i++] = V_NATURE;
		cr_ptr->vir_types[i++] = V_TEMPERANCE;
		break;
	case CLASS_PALADIN:
		cr_ptr->vir_types[i++] = V_JUSTICE;
		cr_ptr->vir_types[i++] = V_VALOUR;
		cr_ptr->vir_types[i++] = V_HONOUR;
		cr_ptr->vir_types[i++] = V_FAITH;
		break;
	case CLASS_WARRIOR_MAGE:
	case CLASS_RED_MAGE:
		cr_ptr->vir_types[i++] = V_ENCHANT;
		cr_ptr->vir_types[i++] = V_VALOUR;
		break;
	case CLASS_CHAOS_WARRIOR:
		cr_ptr->vir_types[i++] = V_CHANCE;
		cr_ptr->vir_types[i++] = V_INDIVIDUALISM;
		break;
	case CLASS_MONK:
	case CLASS_FORCETRAINER:
		cr_ptr->vir_types[i++] = V_FAITH;
		cr_ptr->vir_types[i++] = V_HARMONY;
		cr_ptr->vir_types[i++] = V_TEMPERANCE;
		cr_ptr->vir_types[i++] = V_PATIENCE;
		break;
	case CLASS_MINDCRAFTER:
	case CLASS_MIRROR_MASTER:
		cr_ptr->vir_types[i++] = V_HARMONY;
		cr_ptr->vir_types[i++] = V_ENLIGHTEN;
		cr_ptr->vir_types[i++] = V_PATIENCE;
		break;
	case CLASS_HIGH_MAGE:
	case CLASS_SORCERER:
		cr_ptr->vir_types[i++] = V_ENLIGHTEN;
		cr_ptr->vir_types[i++] = V_ENCHANT;
		cr_ptr->vir_types[i++] = V_KNOWLEDGE;
		break;
	case CLASS_TOURIST:
		cr_ptr->vir_types[i++] = V_ENLIGHTEN;
		cr_ptr->vir_types[i++] = V_CHANCE;
		break;
	case CLASS_IMITATOR:
		cr_ptr->vir_types[i++] = V_CHANCE;
		break;
	case CLASS_BLUE_MAGE:
		cr_ptr->vir_types[i++] = V_CHANCE;
		cr_ptr->vir_types[i++] = V_KNOWLEDGE;
		break;
	case CLASS_BEASTMASTER:
		cr_ptr->vir_types[i++] = V_NATURE;
		cr_ptr->vir_types[i++] = V_CHANCE;
		cr_ptr->vir_types[i++] = V_VITALITY;
		break;
	case CLASS_MAGIC_EATER:
		cr_ptr->vir_types[i++] = V_ENCHANT;
		cr_ptr->vir_types[i++] = V_KNOWLEDGE;
		break;
	case CLASS_BARD:
		cr_ptr->vir_types[i++] = V_HARMONY;
		cr_ptr->vir_types[i++] = V_COMPASSION;
		break;
	case CLASS_CAVALRY:
		cr_ptr->vir_types[i++] = V_VALOUR;
		cr_ptr->vir_types[i++] = V_HARMONY;
		break;
	case CLASS_BERSERKER:
		cr_ptr->vir_types[i++] = V_VALOUR;
		cr_ptr->vir_types[i++] = V_INDIVIDUALISM;
		break;
	case CLASS_SMITH:
		cr_ptr->vir_types[i++] = V_HONOUR;
		cr_ptr->vir_types[i++] = V_KNOWLEDGE;
		break;
	case CLASS_NINJA:
		cr_ptr->vir_types[i++] = V_PATIENCE;
		cr_ptr->vir_types[i++] = V_KNOWLEDGE;
		cr_ptr->vir_types[i++] = V_FAITH;
		cr_ptr->vir_types[i++] = V_UNLIFE;
		break;
	};

	/* Get one virtue based on race */
	switch (cr_ptr->irace_idx)
	{
	case RACE_HUMAN: case RACE_ELF: case RACE_DUNADAN:
		cr_ptr->vir_types[i++] = V_INDIVIDUALISM;
		break;
	case RACE_SPRITE: case RACE_ENT:
		cr_ptr->vir_types[i++] = V_NATURE;
		break;
	case RACE_HOBBIT: case RACE_OGRE:
		cr_ptr->vir_types[i++] = V_TEMPERANCE;
		break;
	case RACE_DWARF: case RACE_KLACKON: case RACE_ANDROID:
		cr_ptr->vir_types[i++] = V_DILIGENCE;
		break;
	case RACE_GNOME: case RACE_CYCLOPS:
		cr_ptr->vir_types[i++] = V_KNOWLEDGE;
		break;
	case RACE_ORC: case RACE_AMBERITE: case RACE_KOBOLD:
		cr_ptr->vir_types[i++] = V_HONOUR;
		break;
	case RACE_TROLL: case RACE_BARBARIAN:
		cr_ptr->vir_types[i++] = V_VALOUR;
		break;
	case RACE_ELDAR: case RACE_KUTAR:
		cr_ptr->vir_types[i++] = V_VITALITY;
		break;
	case RACE_GIANT: case RACE_GOLEM: case RACE_ANGEL: case RACE_DEMON:
		cr_ptr->vir_types[i++] = V_JUSTICE;
		break;
	case RACE_TITAN:
		cr_ptr->vir_types[i++] = V_HARMONY;
		break;
	case RACE_YEEK:
		cr_ptr->vir_types[i++] = V_SACRIFICE;
		break;
	case RACE_MIND_FLAYER:
		cr_ptr->vir_types[i++] = V_ENLIGHTEN;
		break;
	case RACE_DARK_ELF: case RACE_DRACONIAN: case RACE_S_FAIRY:
		cr_ptr->vir_types[i++] = V_ENCHANT;
		break;
	case RACE_NIBELUNG:
		cr_ptr->vir_types[i++] = V_PATIENCE;
		break;
	case RACE_IMP:
		cr_ptr->vir_types[i++] = V_FAITH;
		break;
	case RACE_ZOMBIE: case RACE_SKELETON:
	case RACE_VAMPIRE: case RACE_LICH:
		cr_ptr->vir_types[i++] = V_UNLIFE;
		break;
	case RACE_BEASTMAN:
		cr_ptr->vir_types[i++] = V_CHANCE;
		break;
	}

	/* Get a virtue for realms */
	if (cr_ptr->realm1)
	{
		tmp_vir = get_realm_virtues(cr_ptr, cr_ptr->realm1);
		if (tmp_vir) cr_ptr->vir_types[i++] = tmp_vir;
	}
	if (cr_ptr->realm2)
	{
		tmp_vir = get_realm_virtues(cr_ptr, cr_ptr->realm2);
		if (tmp_vir) cr_ptr->vir_types[i++] = tmp_vir;
	}

	/* Eliminate doubles */
	for (i = 0; i < 8; i++)
	{
		for (j = i + 1; j < 8; j++)
		{
			if ((cr_ptr->vir_types[j] != 0) && (cr_ptr->vir_types[j] == cr_ptr->vir_types[i]))
				cr_ptr->vir_types[j] = 0;
		}
	}

	/* Fill in the blanks */
	for (i = 0; i < 8; i++)
	{
		if (cr_ptr->vir_types[i] == 0) get_random_virtue(cr_ptr, i);
	}
}

void chg_virtue(creature_type *cr_ptr, int virtue, int amount)
{
	int i = 0;

	for (i = 0; i < 8; i++)
	{
		if (cr_ptr->vir_types[i] == virtue)
		{
			if (amount > 0)
			{
				if ((amount + cr_ptr->virtues[i] > 50) && one_in_(2))
				{
					cr_ptr->virtues[i] = MAX(cr_ptr->virtues[i], 50);
					return;
				}
				if ((amount + cr_ptr->virtues[i] > 80) && one_in_(2))
				{
					cr_ptr->virtues[i] = MAX(cr_ptr->virtues[i], 80);
					return;
				}
				if ((amount + cr_ptr->virtues[i] > 100) && one_in_(2))
				{
					cr_ptr->virtues[i] = MAX(cr_ptr->virtues[i], 100);
					return;
				}
				if (amount + cr_ptr->virtues[i] > 125)
					cr_ptr->virtues[i] = 125;
				else
					cr_ptr->virtues[i] = cr_ptr->virtues[i] + amount;
			}
			else
			{
				if ((amount + cr_ptr->virtues[i] < -50) && one_in_(2))
				{
					cr_ptr->virtues[i] = MIN(cr_ptr->virtues[i], -50);
					return;
				}
				if ((amount + cr_ptr->virtues[i] < -80) && one_in_(2))
				{
					cr_ptr->virtues[i] = MIN(cr_ptr->virtues[i], -80);
					return;
				}
				if ((amount + cr_ptr->virtues[i] < -100) && one_in_(2))
				{
					cr_ptr->virtues[i] = MIN(cr_ptr->virtues[i], -100);
					return;
				}
				if (amount + cr_ptr->virtues[i] < -125)
					cr_ptr->virtues[i] = -125;
				else
					cr_ptr->virtues[i] = cr_ptr->virtues[i] + amount;
			}
			cr_ptr->update |= (PU_BONUS);
			return;
		}
	}
}

void set_virtue(creature_type *cr_ptr, int virtue, int amount)
{
	int i = 0;

	for (i = 0; i < 8; i++)
	{
		if (cr_ptr->vir_types[i] == virtue)
		{
			cr_ptr->virtues[i] = amount;
			return;
		}
	}
}

void dump_virtues(creature_type *cr_ptr, FILE *OutFile)
{
	int v_nr = 0;

	if (!OutFile) return;

	for (v_nr = 0; v_nr < 8; v_nr++)
	{
		char v_name [20];
		int tester = cr_ptr->virtues[v_nr];

		strcpy(v_name, virtue[(cr_ptr->vir_types[v_nr])-1]);

		if (cr_ptr->vir_types[v_nr] == 0 || cr_ptr->vir_types[v_nr] > MAX_VIRTUE)
#ifdef JP
			fprintf(OutFile, "Ç®Ç¡Ç∆ÅB%sÇÃèÓïÒÇ»ÇµÅB", v_name);
#else
			fprintf(OutFile, "Oops. No info about %s.", v_name);
#endif

		else if (tester < -100)
#ifdef JP
			fprintf(OutFile, "[%s]ÇÃëŒã…",
#else
			fprintf(OutFile, "You are the polar opposite of %s.",
#endif

			        v_name);
		else if (tester < -80)
#ifdef JP
			fprintf(OutFile, "[%s]ÇÃëÂìG",
#else
			fprintf(OutFile, "You are an arch-enemy of %s.",
#endif

			        v_name);
		else if (tester < -60)
#ifdef JP
			fprintf(OutFile, "[%s]ÇÃã≠ìG",
#else
			fprintf(OutFile, "You are a bitter enemy of %s.",
#endif

			        v_name);
		else if (tester < -40)
#ifdef JP
			fprintf(OutFile, "[%s]ÇÃìG",
#else
			fprintf(OutFile, "You are an enemy of %s.",
#endif

			        v_name);
		else if (tester < -20)
#ifdef JP
			fprintf(OutFile, "[%s]ÇÃçﬂé“",
#else
			fprintf(OutFile, "You have sinned against %s.",
#endif

			        v_name);
		else if (tester < 0)
#ifdef JP
			fprintf(OutFile, "[%s]ÇÃñ¿ìπé“",
#else
			fprintf(OutFile, "You have strayed from the path of %s.",
#endif

			        v_name);
		else if (tester == 0)
#ifdef JP
			fprintf(OutFile,"[%s]ÇÃíÜóßé“",
#else
			fprintf(OutFile,"You are neutral to %s.",
#endif

			        v_name);
		else if (tester < 20)
#ifdef JP
			fprintf(OutFile,"[%s]ÇÃè¨ìøé“",
#else
			fprintf(OutFile,"You are somewhat virtuous in %s.",
#endif

			        v_name);
		else if (tester < 40)
#ifdef JP
			fprintf(OutFile,"[%s]ÇÃíÜìøé“",
#else
			fprintf(OutFile,"You are virtuous in %s.",
#endif

			        v_name);
		else if (tester < 60)
#ifdef JP
			fprintf(OutFile,"[%s]ÇÃçÇìøé“",
#else
			fprintf(OutFile,"You are very virtuous in %s.",
#endif

			        v_name);
		else if (tester < 80)
#ifdef JP
			fprintf(OutFile,"[%s]ÇÃîeé“",
#else
			fprintf(OutFile,"You are a champion of %s.",
#endif

			        v_name);
		else if (tester < 100)
#ifdef JP
			fprintf(OutFile,"[%s]ÇÃàÃëÂÇ»îeé“",
#else
			fprintf(OutFile,"You are a great champion of %s.",
#endif

			        v_name);
		else
#ifdef JP
			fprintf(OutFile,"[%s]ÇÃãÔåªé“",
#else
			fprintf(OutFile,"You are the living embodiment of %s.",
#endif

			        v_name);

	    fprintf(OutFile, "\n");
	}
}
