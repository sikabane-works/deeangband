/* File: files.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: code dealing with files (and death) */

#include "angband.h"
#include "autopick.h"
#include "birth.h"
#include "cave.h"
#include "command.h"
#include "creature_const.h"
#include "diary.h"
#include "files.h"
#include "init.h"
#include "karma.h"
#include "mutation.h"
#include "object.h"
#include "scores.h"

/*
 * You may or may not want to use the following "#undef".
 */
/* #undef _POSIX_SAVED_IDS */


/*
 * Hack -- drop permissions
 */
void safe_setuid_drop(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if(setuid(getuid()) != 0) quit(MES_SYS_ERROR_SETUID);
	if(setgid(getgid()) != 0) quit(MES_SYS_ERROR_SETGID);

#  else

	if(setreuid(geteuid(), getuid()) != 0) quit(MES_SYS_ERROR_SETEUID);
	if(setregid(getegid(), getgid()) != 0) quit(MES_SYS_ERROR_SETEGID);

#  endif

# endif

#endif

}


/*
 * Hack -- grab permissions
 */
void safe_setuid_grab(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if(setuid(player_euid) != 0) quit(MES_SYS_ERROR_SETUID);
	if(setgid(player_egid) != 0) quit(MES_SYS_ERROR_SETGID);

#  else

	if(setreuid(geteuid(), getuid()) != 0) quit(MES_SYS_ERROR_SETEUID);
	if(setregid(getegid(), getgid()) != 0) quit(MES_SYS_ERROR_SETGUID);

#  endif /* SAFE_SETUID_POSIX */

# endif /* SAFE_SETUID */

#endif /* SET_UID */

}


/*
 * Extract the first few "tokens" from a buffer
 *
 * This function uses "colon" and "slash" as the delimeter characters.
 *
 * We never extract more than "num" tokens.  The "last" token may include
 * "delimeter" characters, allowing the buffer to include a "string" token.
 *
 * We save pointers to the tokens in "tokens", and return the number found.
 *
 * Hack -- Attempt to handle the 'c' character formalism
 * Hack -- An empty buffer, or a final delimeter, yields an "empty" token.
 * Hack -- We will always extract at least one token
 */
int tokenize(char *buf, s16b num, char **tokens, int mode)
{
	int i = 0;
	char *s = buf;

	while (i < num - 1)
	{
		char *t;

		/* Scan the string */
		for (t = s; *t; t++)
		{
			/* Found a delimiter */
			if((*t == ':') || (*t == '/')) break;

			/* Handle single quotes */
			if((mode & TOKENIZE_CHECKQUOTE) && (*t == '\''))
			{
				t++; /* Advance */
				if(*t == '\\') t++; /* Handle backslash */
				if(!*t) break; /* Require a character */
				t++; /* Advance */
				if(*t != '\'') *t = '\''; /* Hack -- Require a close quote */
			}

			/* Handle back-slash */
			if(*t == '\\') t++;
		}

		if(!*t) break; /* Nothing left */
		*t++ = '\0'; /* Nuke and advance */
		tokens[i++] = s; /* Save the token */
		s = t; /* Advance */
	}

	tokens[i++] = s; /* Save the token */
	return (i); /* Number found */
}


/* Index of spell type names */
effect_type effect_desc[] =
{
	{"ELEC", DO_EFFECT_ELEC},
	{"POIS", DO_EFFECT_POIS},
	{"ACID", DO_EFFECT_ACID},
	{"COLD", DO_EFFECT_COLD},
	{"FIRE", DO_EFFECT_FIRE},
	{"PSY_SPEAR", DO_EFFECT_PSY_SPEAR},
	{"MISSILE", DO_EFFECT_MISSILE},
	{"ARROW", DO_EFFECT_ARROW},
	{"PLASMA", DO_EFFECT_PLASMA},
	{"WATER", DO_EFFECT_WATER},
	{"LITE", DO_EFFECT_LITE},
	{"DARK", DO_EFFECT_DARK},
	{"LITE_WEAK", DO_EFFECT_LITE_WEAK},
	{"DARK_WEAK", DO_EFFECT_DARK_WEAK},
	{"SHARDS", DO_EFFECT_SHARDS},
	{"SOUND", DO_EFFECT_SOUND},
	{"CONFUSION", DO_EFFECT_CONFUSION},
	{"FORCE", DO_EFFECT_FORCE},
	{"INERTIA", DO_EFFECT_INERTIA},
	{"MANA", DO_EFFECT_MANA},
	{"METEOR", DO_EFFECT_METEOR},
	{"ICE", DO_EFFECT_ICE},
	{"CHAOS", DO_EFFECT_CHAOS},
	{"NETHER", DO_EFFECT_NETHER},
	{"DISENCHANT", DO_EFFECT_DISENCHANT},
	{"NEXUS", DO_EFFECT_NEXUS},
	{"TIME", DO_EFFECT_TIME},
	{"GRAVITY", DO_EFFECT_GRAVITY},
	{"KILL_WALL", DO_EFFECT_KILL_WALL},
	{"KILL_DOOR", DO_EFFECT_KILL_DOOR},
	{"KILL_TRAP", DO_EFFECT_KILL_TRAP},
	{"MAKE_WALL", DO_EFFECT_MAKE_WALL},
	{"MAKE_DOOR", DO_EFFECT_MAKE_DOOR},
	{"MAKE_TRAP", DO_EFFECT_MAKE_TRAP},
	{"MAKE_TREE", DO_EFFECT_MAKE_TREE},
	{"CLONE", DO_EFFECT_CLONE},
	{"POLY", DO_EFFECT_POLY},
	{"HEAL_HP", DO_EFFECT_HEAL_HP},
	{"HEAL_MP", DO_EFFECT_HEAL_MP},
	{"HASTE", DO_EFFECT_HASTE},
	{"SLOW", DO_EFFECT_SLOW},
	{"SLEEP", DO_EFFECT_SLEEP},
	{"DRAIN", DO_EFFECT_DRAIN},
	{"AWAY_UNDEAD", DO_EFFECT_AWAY_UNDEAD},
	{"AWAY_EVIL", DO_EFFECT_AWAY_EVIL},
	{"AWAY_ALL", DO_EFFECT_AWAY_ALL},
	{"TURN_UNDEAD", DO_EFFECT_TURN_UNDEAD},
	{"TURN_EVIL", DO_EFFECT_TURN_EVIL},
	{"TURN_ALL", DO_EFFECT_TURN_ALL},
	{"DISP_UNDEAD", DO_EFFECT_DISP_UNDEAD},
	{"DISP_EVIL", DO_EFFECT_DISP_EVIL},
	{"DISP_ALL", DO_EFFECT_DISP_ALL},
	{"DISP_DEMON", DO_EFFECT_DISP_DEMON},
	{"DISP_LIVING", DO_EFFECT_DISP_LIVING},
	{"ROCKET", DO_EFFECT_ROCKET},
	{"NUKE", DO_EFFECT_NUKE},
	{"MAKE_GLYPH", DO_EFFECT_MAKE_GLYPH},
	{"HOLD", DO_EFFECT_HOLD},
	{"STONE_WALL", DO_EFFECT_STONE_WALL},
	{"DEATH_RAY", DO_EFFECT_DEATH_RAY},
	{"STUN", DO_EFFECT_STUN},
	{"HOLY_FIRE", DO_EFFECT_HOLY_FIRE},
	{"HELL_FIRE", DO_EFFECT_HELL_FIRE},
	{"DISINTEGRATE", DO_EFFECT_DISINTEGRATE},
	{"CHARM", DO_EFFECT_CHARM},
	{"CONTROL_UNDEAD", DO_EFFECT_CONTROL_UNDEAD},
	{"CONTROL_ANIMAL", DO_EFFECT_CONTROL_ANIMAL},
	{"PSI", DO_EFFECT_PSI},
	{"PSI_DRAIN", DO_EFFECT_PSI_DRAIN},
	{"TELEKINESIS", DO_EFFECT_TELEKINESIS},
	{"JAM_DOOR", DO_EFFECT_JAM_DOOR},
	{"DOMINATION", DO_EFFECT_DOMINATION},
	{"DISP_GOOD", DO_EFFECT_DISP_GOOD},
	{"DRAIN_MANA", DO_EFFECT_DRAIN_MANA},
	{"MIND_BLAST", DO_EFFECT_MIND_BLAST},
	{"BRAIN_SMASH", DO_EFFECT_BRAIN_SMASH},
	{"CURSE", DO_EFFECT_CURSE},
	{"KEIRAKU", DO_EFFECT_KEIRAKU},
	{"HAND_DOOM", DO_EFFECT_HAND_DOOM},
	{"CAPTURE", DO_EFFECT_CAPTURE},
	{"ANIM_DEAD", DO_EFFECT_ANIM_DEAD},
	{"CONTROL_LIVING", DO_EFFECT_CONTROL_LIVING},
	{"IDENTIFY", DO_EFFECT_IDENTIFY},
	{"ATTACK", DO_EFFECT_ATTACK},
	{"ENGETSU", DO_EFFECT_ENGETSU},
	{"GENOCIDE", DO_EFFECT_GENOCIDE},
	{"PHOTO", DO_EFFECT_PHOTO},
	{"CONTROL_DEMON", DO_EFFECT_CONTROL_DEMON},
	{"LAVA_FLOW", DO_EFFECT_LAVA_FLOW},
	{"BLOOD_CURSE", DO_EFFECT_BLOOD_CURSE},
	{"SEEKER", DO_EFFECT_SEEKER},
	{"SUPER_RAY", DO_EFFECT_SUPER_RAY},
	{"STAR_HEAL", DO_EFFECT_STAR_HEAL},
	{"WATER_FLOW", DO_EFFECT_WATER_FLOW},
	{"CRUSADE", DO_EFFECT_CRUSADE},
	{"HOLD_EVIL", DO_EFFECT_HOLD_EVIL},
	{"WOUNDS", DO_EFFECT_WOUNDS},
	{"ACID_FLOW", DO_EFFECT_ACID_FLOW},
	{"POISON_FLOW", DO_EFFECT_POISON_FLOW},
	{"WANDER", DO_EFFECT_WANDER},
	{"INVOKE_SPIRITS", DO_EFFECT_INVOKE_SPIRITS},
	{"RUMOR", DO_EFFECT_RUMOR},
	{"TIMESTOP", DO_EFFECT_TIMESTOP},
	{"DISPEL", DO_EFFECT_DISPEL},
	{"TRY_LIVINGTRUMP", DO_EFFECT_TRY_LIVINGTRUMP},
	{"VOMITING", DO_EFFECT_VOMITING},
	{"REMOVE_CURSE_1", DO_EFFECT_REMOVE_CURSE_1},
	{"REMOVE_CURSE_2", DO_EFFECT_REMOVE_CURSE_2},
	{"SATIATE", DO_EFFECT_SATIATE},
	{"AGGRAVATE", DO_EFFECT_AGGRAVATE},
	{"MIRROR_SET", DO_EFFECT_MIRROR_SET},
	{"IMPROVE_FORCE", DO_EFFECT_IMPROVE_FORCE},
	{"GAIN_EXP", DO_EFFECT_GAIN_EXP},
	{"RESTORE_EXP", DO_EFFECT_RESTORE_EXP},
	{"DEC_STR", DO_EFFECT_DEC_STR},
	{"DEC_INT", DO_EFFECT_DEC_INT},
	{"DEC_WIS", DO_EFFECT_DEC_WIS},
	{"DEC_DEX", DO_EFFECT_DEC_DEX},
	{"DEC_CON", DO_EFFECT_DEC_CON},
	{"DEC_CHA", DO_EFFECT_DEC_CHA},
	{"RES_STR", DO_EFFECT_RES_STR},
	{"RES_INT", DO_EFFECT_RES_INT},
	{"RES_WIS", DO_EFFECT_RES_WIS},
	{"RES_DEX", DO_EFFECT_RES_DEX},
	{"RES_CON", DO_EFFECT_RES_CON},
	{"RES_CHA", DO_EFFECT_RES_CHA},
	{"INC_STR", DO_EFFECT_INC_STR},
	{"INC_INT", DO_EFFECT_INC_INT},
	{"INC_WIS", DO_EFFECT_INC_WIS},
	{"INC_DEX", DO_EFFECT_INC_DEX},
	{"INC_CON", DO_EFFECT_INC_CON},
	{"INC_CHA", DO_EFFECT_INC_CHA},
	{"INC_WIZLITE", DO_EFFECT_WIZLITE},
	{"SHUFFLE", DO_EFFECT_SHUFFLE},
	{"RING_POWER", DO_EFFECT_RING_POWER},
	{"CALL_CHAOS", DO_EFFECT_CALL_CHAOS},
	{"TY_CURSE", DO_EFFECT_TY_CURSE},
	{NULL, 0}
};


/*
* Parse a sub-file of the "extra info" (format shown below)
*
* Each "action" line has an "action symbol" in the first column,
* followed by a colon, followed by some command specific info,
* usually in the form of "tokens" separated by colons or slashes.
*
* Blank lines, lines starting with white space, and lines starting
* with pound signs ("#") are ignored (as comments).
*
* Note the use of "tokenize()" to allow the use of both colons and
* slashes as delimeters, while still allowing final tokens which
* may contain any characters including "delimiters".
*
* Note the use of "strtol()" to allow all "integers" to be encoded
* in decimal, hexidecimal, or octal form.
*
* Note that "creature zero" is used for the "player" attr/char, "object
* zero" will be used for the "stack" attr/char, and "feature zero" is
* used for the "nothing" attr/char.
*
* Parse another file recursively, see below for details
*   %:<filename>
*
* Specify the attr/char values for "creatures" by race index
*   R:<num>:<a>:<c>
*
* Specify the attr/char values for "objects" by kind index
*   K:<num>:<a>:<c>
*
* Specify the attr/char values for "features" by feature index
*   F:<num>:<a>:<c>
*
* Specify the attr/char values for unaware "objects" by kind tval
*   U:<tv>:<a>:<c>
*
* Specify the attr/char values for player_ptr->inventory "objects" by kind tval
*   E:<tv>:<a>:<c>
*
* Define a macro action, given an encoded macro action
*   A:<str>
*
* Create a normal macro, given an encoded macro trigger
*   P:<str>
*
* Create a command macro, given an encoded macro trigger
*   C:<str>
*
* Create a keyset mapping
*   S:<key>:<key>:<dir>
*
* Turn an option off, given its name
*   X:<str>
*
* Turn an option on, given its name
*   Y:<str>
*
* Specify visual information, given an index, and some data
*   V:<num>:<kv>:<rv>:<gv>:<bv>
*
* Specify the set of colors to use when drawing a zapped spell
*   Z:<type>:<str>
*
* Specify a macro trigger template and macro trigger names.
*   T:<template>:<modifier chr>:<modifier art_id>:<modifier ego_id>:...
*   T:<trigger>:<keycode>:<shift-keycode>
*
*/

errr process_pref_file_command(char *buf)
{
	int i, j;
	COLOR_ID n1, n2;
	char *zz[16];

	/* Require "?:*" format */
	if(buf[1] != ':') return FAILURE;

	switch (buf[0])
	{
		/* Mega-Hack -- read external player's history file */
		/* Process "H:<history>" */
	case 'H':
		add_history_from_pref_line(buf + 2);
		return SUCCESS;

		/* Process "R:<num>:<a>/<c>" -- attr/char for creature races */
	case 'R':
		if(tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			species_type *species_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = (COLOR_ID)strtol(zz[1], NULL, 0);
			n2 = (COLOR_ID)strtol(zz[2], NULL, 0);
			if(i >= max_species_idx) return FAILURE;
			species_ptr = &species_info[i];
			if(n1 || (!(n2 & 0x80) && n2)) species_ptr->x_attr = n1; /* Allow TERM_DARK text */
			if(n2) species_ptr->x_char = n2;
			return SUCCESS;
		}
		break;

		/* Process "K:<num>:<a>/<c>"  -- attr/char for object kinds */
	case 'K':
		if(tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			object_kind *object_kind_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = (COLOR_ID)strtol(zz[1], NULL, 0);
			n2 = (COLOR_ID)strtol(zz[2], NULL, 0);
			if(i >= max_object_kind_idx) return FAILURE;
			object_kind_ptr = &object_kind_info[i];
			if(n1 || (!(n2 & 0x80) && n2)) object_kind_ptr->x_attr = n1; /* Allow TERM_DARK text */
			if(n2) object_kind_ptr->x_char = n2;
			return SUCCESS;
		}
		break;

		/* Process "F:<num>:<a>/<c>" -- attr/char for terrain features */
		/* "F:<num>:<a>/<c>" */
		/* "F:<num>:<a>/<c>:LIT" */
		/* "F:<num>:<a>/<c>:<la>/<lc>:<da>/<dc>" */
	case 'F':
		{
			feature_type *f_ptr;
			int num = tokenize(buf + 2, F_LIT_MAX * 2 + 1, zz, TOKENIZE_CHECKQUOTE);

			if((num != 3) && (num != 4) && (num != F_LIT_MAX * 2 + 1)) return FAILURE;
			else if((num == 4) && !streq(zz[3], "LIT")) return FAILURE;

			i = (huge)strtol(zz[0], NULL, 0);
			if(i >= max_feature_idx) return FAILURE;
			f_ptr = &feature_info[i];

			n1 = (COLOR_ID)strtol(zz[1], NULL, 0);
			n2 = (COLOR_ID)strtol(zz[2], NULL, 0);
			if(n1 || (!(n2 & 0x80) && n2)) f_ptr->x_attr[F_LIT_STANDARD] = n1; /* Allow TERM_DARK text */
			if(n2) f_ptr->x_char[F_LIT_STANDARD] = n2;

			/* Mega-hack -- feat supports lighting */
			switch (num)
			{
				/* No lighting support */
			case 3:
				n1 = f_ptr->x_attr[F_LIT_STANDARD];
				n2 = f_ptr->x_char[F_LIT_STANDARD];
				for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
				{
					f_ptr->x_attr[j] = n1;
					f_ptr->x_char[j] = n2;
				}
				break;

				/* Use default lighting */
			case 4:
				apply_default_feat_lighting(f_ptr->x_attr, f_ptr->x_char);
				break;

				/* Use desired lighting */
			case F_LIT_MAX * 2 + 1:
				for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
				{
					n1 = (COLOR_ID)strtol(zz[j * 2 + 1], NULL, 0);
					n2 = (COLOR_ID)strtol(zz[j * 2 + 2], NULL, 0);
					if(n1 || (!(n2 & 0x80) && n2)) f_ptr->x_attr[j] = n1; /* Allow TERM_DARK text */
					if(n2) f_ptr->x_char[j] = n2;
				}
				break;
			}
		}
		return SUCCESS;

		/* Process "S:<num>:<a>/<c>" -- attr/char for special things */
	case 'S':
		if(tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (byte)strtol(zz[0], NULL, 0);
			n1 = (COLOR_ID)strtol(zz[1], NULL, 0);
			n2 = (COLOR_ID)strtol(zz[2], NULL, 0);
			misc_to_acttr[j] = n1;
			misc_to_char[j] = n2;
			return SUCCESS;
		}
		break;

		/* Process "U:<tv>:<a>/<c>" -- attr/char for unaware items */
	case 'U':
		if(tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (huge)strtol(zz[0], NULL, 0);
			n1 = (COLOR_ID)strtol(zz[1], NULL, 0);
			n2 = (COLOR_ID)strtol(zz[2], NULL, 0);
			for (i = 1; i < max_object_kind_idx; i++)
			{
				object_kind *object_kind_ptr = &object_kind_info[i];
				if(object_kind_ptr->tval == j)
				{
					if(n1) object_kind_ptr->d_attr = n1;
					if(n2) object_kind_ptr->d_char = n2;
				}
			}
			return SUCCESS;
		}
		break;

		/* Process "E:<tv>:<a>" -- attribute for inventory objects */
	case 'E':
		if(tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) == 2)
		{
			j = (byte)strtol(zz[0], NULL, 0) % 128;
			n1 = (COLOR_ID)strtol(zz[1], NULL, 0);
			if(n1) tval_to_acttr[j] = n1;
			return SUCCESS;
		}
		break;

		/* Process "A:<str>" -- save an "action" for later */
	case 'A':
		text_to_acscii(macro__buf, buf+2);
		return SUCCESS;

		/* Process "P:<str>" -- normal macro */
	case 'P':
		{
			char tmp[1024];

			text_to_acscii(tmp, buf+2);
			macro_add(tmp, macro__buf);
			return SUCCESS;
		}

		/* Process "C:<str>" -- create keymap */
	case 'C':
		{
			int mode;
			char tmp[1024];

			if(tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) != 2) return FAILURE;

			mode = strtol(zz[0], NULL, 0);
			if((mode < 0) || (mode >= KEYMAP_MODES)) return FAILURE;

			text_to_acscii(tmp, zz[1]);
			if(!tmp[0] || tmp[1]) return FAILURE;
			i = (byte)(tmp[0]);

			string_free(keymap_act[mode][i]);

			keymap_act[mode][i] = string_make(macro__buf);

			return SUCCESS;
		}

		/* Process "V:<num>:<kv>:<rv>:<gv>:<bv>" -- visual info */
	case 'V':
		if(tokenize(buf+2, 5, zz, TOKENIZE_CHECKQUOTE) == 5)
		{
			i = (byte)strtol(zz[0], NULL, 0);
			angband_color_table[i][0] = (byte)strtol(zz[1], NULL, 0);
			angband_color_table[i][1] = (byte)strtol(zz[2], NULL, 0);
			angband_color_table[i][2] = (byte)strtol(zz[3], NULL, 0);
			angband_color_table[i][3] = (byte)strtol(zz[4], NULL, 0);
			return SUCCESS;
		}
		break;

		/* Process "X:<str>" -- turn option off */
		/* Process "Y:<str>" -- turn option on */
	case 'X':
	case 'Y':
		for (i = 0; option_info[i].o_desc; i++)
		{
			if(option_info[i].o_var && option_info[i].o_text && streq(option_info[i].o_text, buf + 2))
			{
				int os = option_info[i].o_set;
				int ob = option_info[i].o_bit;

				if((playing) && (OPT_PAGE_BIRTH == option_info[i].o_page) && !wizard)
				{
					msg_format(MES_PLEFFILE_INIT_OPTION(buf));
					msg_print(NULL);
					return SUCCESS;
				}

				if(buf[0] == 'X') /* Clear */
				{
					option_flag[os] &= ~(1L << ob);
					(*option_info[i].o_var) = FALSE;
				}
				else /* Set */
				{
					option_flag[os] |= (1L << ob);
					(*option_info[i].o_var) = TRUE;
				}
				return SUCCESS;
			}
		}

		msg_format(MES_PLEFFILE_INVALID_OPTION(buf));
		msg_print(NULL);
		return SUCCESS;

		/* Process "Z:<type>:<str>" -- set spell color */
	case 'Z':
		{
			/* Find the colon */
			char *t = my_strchr(buf + 2, ':');

			if(!t) return FAILURE;

			/* Nuke the colon */
			*(t++) = '\0';

			for (i = 0; effect_desc[i].name; i++)
			{
				/* Match this type */
				if(streq(effect_desc[i].name, buf + 2))
				{
					/* Remember this color set */
					gf_color[effect_desc[i].num] = (s16b)quark_add(t);

					return SUCCESS;
				}
			}

			break;
		}

		/* Initialize macro trigger names and a template */
		/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
		/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
	case 'T':
		{
			int tok = tokenize(buf+2, 2+MAX_MACRO_MOD, zz, 0);

			/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
			if(tok >= 4)
			{
				int i;
				int num;

				if(macro_template != NULL)
				{
					num = strlen(macro_modifier_chr);

					/* Kill the template string */
					string_free(macro_template);
					macro_template = NULL;

					/* Kill flag characters of modifier keys */
					string_free(macro_modifier_chr);

					/* Kill corresponding modifier names */
					for (i = 0; i < num; i++)
					{
						string_free(macro_modifiespecies_name[i]);
					}

					/* Kill trigger name strings */
					for (i = 0; i < max_macrotrigger; i++)
					{
						string_free(macro_triggespecies_name[i]);
						string_free(macro_trigger_keycode[0][i]);
						string_free(macro_trigger_keycode[1][i]);
					}

					max_macrotrigger = 0;
				}

				if(*zz[0] == '\0') return SUCCESS; /* clear template */

				/* Number of modifier flags */
				num = strlen(zz[1]);

				/* Limit the number */
				num = MIN(MAX_MACRO_MOD, num);

				/* Stop if number of modifier is not correct */
				if(2 + num != tok) return FAILURE;

				/* Get a template string */
				macro_template = string_make(zz[0]);

				/* Get flag characters of modifier keys */
				macro_modifier_chr = string_make(zz[1]);

				/* Get corresponding modifier names */
				for (i = 0; i < num; i++)
				{
					macro_modifiespecies_name[i] = string_make(zz[2+i]);
				}
			}

			/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
			else if(tok >= 2)
			{
				char buf[1024];
				int m;
				char *t, *s;
				if(max_macrotrigger >= MAX_MACRO_TRIG)
				{
					msg_print(MES_PLEFFILE_TOO_MACRO);
					return FAILURE;
				}
				m = max_macrotrigger;
				max_macrotrigger++;

				/* Take into account the escape character  */
				t = buf;
				s = zz[0];
				while (*s)
				{
					if('\\' == *s) s++;
					*t++ = *s++;
				}
				*t = '\0';

				/* Get a trigger name */
				macro_triggespecies_name[m] = string_make(buf);

				/* Get the corresponding key code */
				macro_trigger_keycode[0][m] = string_make(zz[1]);

				if(tok == 3) macro_trigger_keycode[1][m] = string_make(zz[2]);
				else macro_trigger_keycode[1][m] = string_make(zz[1]);
			}
			return SUCCESS;
		}
	}

	return FAILURE;
}


/*
 * Helper function for "process_pref_file()"
 *
 * Input:
 *   v: output buffer array
 *   f: final character
 *
 * Output:
 *   result
 */
cptr process_pref_file_expr(char **sp, char *fp, creature_type *creature_ptr)
{
	cptr v;

	char *b;
	char *s;

	char b1 = '[';
	char b2 = ']';

	char f = ' ';
	static char tmp[10];

	/* Initial */
	s = (*sp);

	/* Skip spaces */
	while (isspace(*s)) s++;

	/* Save start */
	b = s;

	v = "?o?o?";

	/* Analyze */
	if(*s == b1)
	{
		const char *p;
		const char *t;

		/* Skip b1 */
		s++;

		/* First */
		t = process_pref_file_expr(&s, &f, creature_ptr);

		if(!*t)
		{
			/* Nothing */
		}

		/* Function: IOR */
		else if(streq(t, "IOR"))
		{
			v = "0";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f, creature_ptr);
				if(*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		else if(streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f, creature_ptr);
				if(*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		else if(streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f, creature_ptr);
				if(*t && streq(t, "1")) v = "0";
			}
		}

		/* Function: EQU */
		else if(streq(t, "EQU"))
		{
			v = "0";
			if(*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f, creature_ptr);
			}
			while (*s && (f != b2))
			{
				p = process_pref_file_expr(&s, &f, creature_ptr);
				if(streq(t, p)) v = "1";
			}
		}

		/* Function: LEQ */
		else if(streq(t, "LEQ"))
		{
			v = "1";
			if(*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f, creature_ptr);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f, creature_ptr);
				if(*t && strtol(p, NULL, 10) > strtol(t, NULL, 10)) v = "0";
			}
		}

		/* Function: GEQ */
		else if(streq(t, "GEQ"))
		{
			v = "1";
			if(*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f, creature_ptr);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f, creature_ptr);

				/* Compare two numbers instead of string */
				if(*t && strtol(p, NULL, 10) < strtol(t, NULL, 10)) v = "0";
			}
		}

		else
		{
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f, creature_ptr);
			}
		}

		/* Verify ending */
		if(f != b2) v = "?x?x?";

		/* Extract final and Terminate */
		if((f = *s) != '\0') *s++ = '\0';
	}

	/* Other */
	else
	{
		/* Accept all printables except spaces and brackets */
#ifdef JP
		while (is_mbyte(*s) || (isprint(*s) && !my_strchr(" []", *s)))
		{
			if(is_mbyte(*s)) s++;
			s++;
		}
#else
		while (isprint(*s) && !my_strchr(" []", *s)) ++s;
#endif

		/* Extract final and Terminate */
		if((f = *s) != '\0') *s++ = '\0';

		/* Variable */
		if(*b == '$')
		{
			/* System */
			if(streq(b+1, "SYS")) v = ANGBAND_SYS;
			else if(streq(b+1, "KEYBOARD")) v = ANGBAND_KEYBOARD;
			/* Graphics */
			else if(streq(b+1, "GRAF")) v = ANGBAND_GRAF;
			/* Monochrome mode */
			else if(streq(b+1, "MONOCHROME"))
			{
				if(arg_monochrome) v = "ON";
				else v = "OFF";
			}

#ifdef JP
			else if(streq(b+1, "RACE1")) v = race_info[creature_ptr->race_idx1].E_title;
			else if(streq(b+1, "RACE2")) v = race_info[creature_ptr->race_idx2].E_title;
			else if(streq(b+1, "CLASS")) v = class_info[creature_ptr->class_idx].E_title;
			else if(streq(b+1, "REALM1")) v = E_realm_names[creature_ptr->realm1];
			else if(streq(b+1, "REALM2")) v = E_realm_names[creature_ptr->realm2];
#else
			else if(streq(b+1, "RACE1")) v = race_info[creature_ptr->race_idx1].title;
			else if(streq(b+1, "RACE2")) v = race_info[creature_ptr->race_idx2].title;
			else if(streq(b+1, "CLASS")) v = class_info[creature_ptr->class_idx].title;
			else if(streq(b+1, "REALM1")) v = realm_names[creature_ptr->realm1];
			else if(streq(b+1, "REALM2")) v = realm_names[creature_ptr->realm2];
#endif
			else if(streq(b+1, "PLAYER"))
			{
				static char tmp_playespecies_name[128];
				char *pn, *tpn;
				for (pn = creature_ptr->name, tpn = tmp_playespecies_name; *pn; pn++, tpn++)
				{
#ifdef JP
					if(is_mbyte(*pn))
					{
						*(tpn++) = *(pn++);
						*tpn = *pn;
						continue;
					}
#endif
					*tpn = my_strchr(" []", *pn) ? '_' : *pn;
				}
				*tpn = '\0';
				v = tmp_playespecies_name;
			}


			else if(streq(b+1, "LEVEL"))
			{
				sprintf(tmp, "%02d", creature_ptr->lev);
				v = tmp;
			}

			else if(streq(b+1, "AUTOREGISTER"))
			{
				if(creature_ptr->autopick_autoregister) v = "1";
				else v = "0";
			}

			else if(streq(b+1, "MONEY"))
			{
				sprintf(tmp, "%09d", creature_ptr->au);
				v = tmp;
			}
		}

		/* Constant */
		else v = b;
	}

	(*fp) = f;
	(*sp) = s;
	return (v);
}


#define PREF_TYPE_NORMAL   0
#define PREF_TYPE_AUTOPICK 1
#define PREF_TYPE_HISTPREF 2

/*
 * Open the "user pref file" and parse it.
 */
static errr process_pref_file_aux(cptr name, int preftype)
{
	FILE *fp;
	char buf[1024];
	char old[1024];
	int line = -1;
	errr err = 0;
	bool bypass = FALSE;

	fp = my_fopen(name, "r");

	/* No such file */
	if(!fp) return (-1);

	/* Process the file */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* Count lines */
		line++;

		/* Skip "empty" lines */
		if(!buf[0]) continue;

		/* Skip "blank" lines */
#ifdef JP
		if(!is_mbyte(buf[0]))
#endif
		if(isspace(buf[0])) continue;

		/* Skip comments */
		if(buf[0] == '#') continue;


		/* Save a copy */
		strcpy(old, buf);


		/* Process "?:<expr>" */
		if((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			cptr v;
			char *s;

			/* Start */
			s = buf + 2;

			/* Parse the expr */
			v = process_pref_file_expr(&s, &f, player_ptr);

			/* Set flag */
			bypass = (streq(v, "0") ? TRUE : FALSE);

			continue;
		}

		/* Apply conditionals */
		if(bypass) continue;


		/* Process "%:<file>" */
		if(buf[0] == '%')
		{
			static FLOOR_LEV depth_count = 0;

			/* Ignore if deeper than 20 level */
			if(depth_count > 20) continue;

			/* Count depth level */
			depth_count++;

			/* Process that file if allowed */
			switch (preftype)
			{
			case PREF_TYPE_AUTOPICK:
				(void)process_autopick_file(buf + 2);
				break;
			case PREF_TYPE_HISTPREF:
				(void)process_histpref_file(buf + 2);
				break;
			default:
				(void)process_pref_file(buf + 2);
				break;
			}

			/* Set back depth level */
			depth_count--;

			continue;
		}


		/* Process the line */
		err = process_pref_file_command(buf);

		/* This is not original pref line... */
		if(err)
		{
			if(preftype != PREF_TYPE_AUTOPICK)
				break;
			err = process_autopick_file_command(buf);
		}
	}

	if(err)
	{
		/* Print error message */
		/* ToDo: Add better error messages */
		msg_format(MES_SYS_PARSE_ERROR(name, line, err));
		msg_format(MES_SYS_PARSING(old));
		msg_print(NULL);
	}

	my_fclose(fp);

	return (err);
}



/*
 * Process the "user pref file" with the given name
 *
 * See the functions above for a list of legal "commands".
 *
 * We also accept the special "?" and "%" directives, which
 * allow conditional evaluation and filename inclusion.
 */
errr process_pref_file(cptr name)
{
	char buf[1024];
	errr err1, err2;

	path_build(buf, sizeof(buf), ANGBAND_DIR_PREF, name);

	/* Process the system pref file */
	err1 = process_pref_file_aux(buf, PREF_TYPE_NORMAL);

	/* Stop at parser errors, but not at non-existing file */
	if(err1 > 0) return err1;


	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);
	
	/* Process the user pref file */
	err2 = process_pref_file_aux(buf, PREF_TYPE_NORMAL);


	/* User file does not exist, but read system pref file */
	if(err2 < 0 && !err1) return -2;

	/* Result of user file processing */
	return err2;
}


#ifdef CHECK_LOAD

#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN  64
#endif

typedef struct statstime statstime;

struct statstime
{
	int                 cp_time[4];
	int                 dk_xfer[4];
	unsigned int        v_pgpgin;
	unsigned int        v_pgpgout;
	unsigned int        v_pswpin;
	unsigned int        v_pswpout;
	unsigned int        v_intr;
	int                 if_ipackets;
	int                 if_ierrors;
	int                 if_opackets;
	int                 if_oerrors;
	int                 if_collisions;
	unsigned int        v_swtch;
	long                avenrun[3];
	struct timeval      boottime;
	struct timeval      curtime;
};

/*
 * Maximal load (if any).
 */
static int check_load_value = 0;

#endif


/*
 * Handle CHECK_LOAD
 */
errr check_load(void)
{

#ifdef CHECK_LOAD

	struct statstime    st;

	/* Success if not checking */
	if(!check_load_value) return SUCCESS;

	/* Check the load */
	if(0 == rstat("localhost", &st))
	{
		long val1 = (long)(st.avenrun[2]);
		long val2 = (long)(check_load_value) * FSCALE;

		/* Check for violation */
		if(val1 >= val2) return FAILURE;
	}

#endif

	return SUCCESS;
}


/*
 * Initialize CHECK_LOAD
 */
errr check_load_init(void)
{

#ifdef CHECK_LOAD

	FILE        *fp;

	char	buf[1024];

	char	temphost[MAXHOSTNAMELEN+1];
	char	thishost[MAXHOSTNAMELEN+1];


	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "load.txt");

	/* Open the "load" file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if(!fp) return SUCCESS;

	/* Default load */
	check_load_value = 100;

	/* Get the host name */
	(void)gethostname(thishost, (sizeof thishost) - 1);

	/* Parse it */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		int value;

		/* Skip comments and blank lines */
		if(!buf[0] || (buf[0] == '#')) continue;

		/* Parse, or ignore */
		if(sscanf(buf, "%s%d", temphost, &value) != 2) continue;

		/* Skip other hosts */
		if(!streq(temphost, thishost) &&
		    !streq(temphost, "localhost")) continue;

		/* Use that value */
		check_load_value = value;

		break;
	}

	my_fclose(fp);

#endif

	return SUCCESS;
}


#define ENTRY_CARRYING_WEIGHT 0
#define ENTRY_EQUIPPING_WEIGHT 1
#define ENTRY_LEFT_HAND1 2
#define ENTRY_LEFT_HAND2 3
#define ENTRY_RIGHT_HAND2 4
#define ENTRY_POSTURE 5
#define ENTRY_SHOOT 6
#define ENTRY_THROW 7
#define ENTRY_SPEED 8
//#define ENTRY_ACTION_POWER 9
#define ENTRY_BASE_AC 10
#define ENTRY_BASE_EV 11
#define ENTRY_LEVEL 12
#define ENTRY_CUR_EXP 13
#define ENTRY_MAX_EXP 14
#define ENTRY_EXP_TO_ADV 15
#define ENTRY_GOLD 16
#define ENTRY_DAY 17
#define ENTRY_HP 18
#define ENTRY_SP 19
#define ENTRY_PLAY_TIME 20
#define ENTRY_SKILL_FIGHT 21
#define ENTRY_SKILL_SHOOT 22
#define ENTRY_SKILL_ROBUSTNESS 23
#define ENTRY_SKILL_EVALITY 24
#define ENTRY_SKILL_VOLITION 25
#define ENTRY_SKILL_STEALTH 26
#define ENTRY_SKILL_PERCEP 27
#define ENTRY_SKILL_SEARCH 28
#define ENTRY_SKILL_DISARM 29
#define ENTRY_SKILL_DEVICE 30
#define ENTRY_BLOWS 31
#define ENTRY_SHOTS 32
#define ENTRY_AVG_DMG 33
#define ENTRY_INFRA 34

#define ENTRY_NAME 35
#define ENTRY_SEX 36
#define ENTRY_RACE 37
#define ENTRY_CLASS 38
#define ENTRY_REALM 39
#define ENTRY_PATRON 40
#define ENTRY_AGE 41
#define ENTRY_HEIGHT 42
#define ENTRY_WEIGHT 43
#define ENTRY_SOCIAL 44
#define ENTRY_ALIGN 45

#define ENTRY_EXP_ANDR 46
#define ENTRY_EXP_TO_ADV_ANDR 47
#define ENTRY_SIZE 48
#define ENTRY_DIVINE_RANK 49
#define ENTRY_SKILL_DIGGING 50

#define ENTRY_GOOD 51
#define ENTRY_EVIL 52
#define ENTRY_ORDER 53
#define ENTRY_CHAOS 54
#define ENTRY_BALANCE 55

#define ENTRY_AUTH 56


static struct
{
	int col;
	int row;
	int len;
	int difficulty1;
	int difficulty2;
	int difficulty3;
	char header[20];
} disp_creature_line[]
#ifdef JP
= {
	{ 1, 13, 30, 0, 0, 0, "KW_CARRYING_WEIGHT"},
	{ 1, 12, 30, 0, 0, 0, "KW_EQUIPMENT_WEIGHT"},
	{ 1, 11, 30, 0, 0, 0, ""},
	{ 1, 12, 30, 0, 0, 0, ""},
	{ 1, 12, 30, 0, 0, 0, ""},
	{ 1, 13, 30, 0, 0, 0, ""},
	{ 1, 15, 30, 0, 0, 0, "射撃"},
	{ 1, 16, 30, 0, 0, 0, "投擲"},
	{16, 10, 14, 0, 0, 0, "速度"},
	{16, 11, 14, 0, 0, 0, "行動値"},
	{ 1, 10, 14, 0, 0, 0, "ＡＣ"},
	{ 1, 11, 14, 0, 0, 0, "ＥＶ"},
	{ 1, 6, 13, 0, 0, 0, "レベル"},
	{33, 10, 21, 0, 0, 0, "経験値"},
	{33, 11, 21, 0, 0, 0, "最大経験"},
	{33, 12, 21, 0, 0, 0, "次レベル"},
	{33, 13, 21, 0, 0, 0, "所持金"},
	{57, 10, 21, 0, 0, 0, "日付"},
	{1, 7, 17, 0, 0, 0, ""}, //HP
	{19, 7, 13, 0, 0, 0, ""}, //MP
	{57, 11, 21, 0, 0, 0, "プレイ時間"},
	{33, 15, -1, 0, 0, 0, "打撃攻撃  :"},
	{33, 16, -1, 0, 0, 0, "射撃攻撃  :"},
	{33, 17, -1, 0, 0, 0, "頑健度    :"},
	{33, 18, -1, 0, 0, 0, "回避能力  :"},
	{33, 19, -1, 0, 0, 0, "意志力    :"},
	{33, 20, -1, 0, 0, 0, "隠密能力  :"},
	{57, 15, -1, 0, 0, 0, "知覚力    :"},
	{57, 16, -1, 0, 0, 0, "観察力    :"},
	{57, 17, -1, 0, 0, 0, "解除能力　:"},
	{57, 18, -1, 0, 0, 0, "魔法道具  :"},
	{ 1, 12, 25, 0, 0, 0, "打撃回数"},
	{ 1, 17, 25, 0, 0, 0, "射撃回数"},
	{ 1, 13, 25, 0, 0, 0, "平均ダメージ"},
	{57, 20, -1, 0, 0, 0, "赤外線視力:"},
	{ 1, 1, -1, 0, 0, 0, "名前: "},
	{ 1, 3, -1, 0, 0, 0, ""},
	{ 1, 2, -1, 0, 0, 0, "種族: "},
	{ 1, 3, -1, 0, 0, 0, "職業: "},
	{ 1, 5, -1, 0, 0, 0, ""},
	{ 1, 4, -1, 0, 0, 0, "主神: "},
	{43, 6, 14, 0, 0, 0, "年齢"},
	{58, 1, 21, 0, 0, 0, ""},
	{58, 2, 15, 0, 0, 0, ""},
	{34, 6, 8, 0, 0, 0, "身分"},
	{ 1, 8, 51, 0, 0, 0, "属性"},
	{29, 14, 21, 0, 0, 0, "強化度"},
	{29, 16, 21, 0, 0, 0, "次レベル"},
	{34, 7, 23, 0, 0, 0, "体格"},
	{16, 6, 17, 0, 0, 0, "神格 "},
	{57, 19, -1, 0, 0, 0, "掘削能力  :"},
	{1, 8, 9, 0, 0, 0, "KW_GOOD"},
	{12, 8, 9, 0, 0, 0, "KW_EVIL"},
	{23, 8, 9, 0, 0, 0, "KW_LAW"},
	{34, 8, 9, 0, 0, 0, "KW_CHAOS"},
	{47, 8, 9, 0, 0, 0, "KW_BALANCE"},
	{1, 5, -1, 0, 0, 0, "権能: "},
};
#else
= {
	{ 1, 13, 30, 0, 0, 0, "Carrying"},
	{ 1, 12, 30, 0, 0, 0, "Equipping"},
	{ 1, 11, 30, 0, 0, 0, ""},
	{ 1, 12, 30, 0, 0, 0, ""},
	{ 1, 12, 30, 0, 0, 0, ""},
	{ 1, 13, 30, 0, 0, 0, ""},
	{ 1, 15, 30, 0, 0, 0, "Shoot"},
	{ 1, 16, 30, 0, 0, 0, "Throw"},
	{16, 10, 14, 0, 0, 0, "Speed"},
	{16, 11, 14, 0, 0, 0, "Action.P"},
	{ 1, 10, 14, 0, 0, 0, "AC    "},
	{ 1, 11, 14, 0, 0, 0, "EV    "},
	{ 1, 8, 12, 0, 0, 0, "Level "},
	{29, 10, 21, 0, 0, 0, "Experience"},
	{29, 11, 21, 0, 0, 0, "Max Exp"},
	{29, 12, 21, 0, 0, 0, "Exp to Adv"},
	{29, 13, 21, 0, 0, 0, "Gold"},
	{51, 10, 21, 0, 0, 0, "Time"},
	{40, 1, 15, 0, 0, 0, "H.P."},
	{58, 1, 15, 0, 0, 0, "S.P."},
	{51, 11, 21, 0, 0, 0, "Play time"},
	{33, 15, -1, 0, 0, 0, "Fighting   : "},
	{33, 16, -1, 0, 0, 0, "Bows/Throw : "},
	{33, 17, -1, 0, 0, 0, "Robustness :"},
	{33, 18, -1, 0, 0, 0, "Evasion    :"},
	{33, 19, -1, 0, 0, 0, "Volition   :"},
	{33, 20, -1, 0, 0, 0, "Stealth    : "},
	{57, 15, -1, 0, 0, 0, "Scouting   : "},
	{57, 16, -1, 0, 0, 0, "Discerning : "},
	{57, 17, -1, 0, 0, 0, "Disarming  : "},
	{57, 18, -1, 0, 0, 0, "MagicDevice: "},
	{ 1, 12, 25, 0, 0, 0, "Blows/Round"},
	{ 1, 17, 25, 0, 0, 0, "Shots/Round"},
	{ 1, 13, 25, 0, 0, 0, "AverageDmg/Rnd"},
	{53, 20, -1, 0, 0, 0, "Infra-Vision: "},
	{ 1, 0, -1, 0, 0, 0, "[Name   ]"},
	{ 1, 3, -1, 0, 0, 0, ""},
	{ 1, 1, -1, 0, 0, 0, "[Race   ]"},
	{ 1, 2, -1, 0, 0, 0, "[Class  ]"},
	{ 1, 5, -1, 0, 0, 0, ""},
	{ 1, 3, -1, 0, 0, 0, "[Patron ]"},
	{29, 3, 14, 0, 0, 0, "Age"},
	{58, 1, 14, 0, 0, 0, "Hgt"},
	{58, 2, 14, 0, 0, 0, "Wgt"},
	{29, 7, 21, 0, 0, 0, "Social Class"},
	{ 1, 8, 51, 0, 0, 0, get_keyword("KW_ALIGNMENT")},
	{29, 14, 21, 0, 0, 0, "Construction"},
	{29, 16, 21, 0, 0, 0, "Const to Adv"},
	{29, 6, 21, 0, 0, 0, "Size"},
	{14, 9, 15, 0, 0, 0, "Diety"},
	{53, 19, -1, 0, 0, 0, "Digging    : "},
	{ 1, 8, 4, 0, 0, 0, "Good"},
	{ 9, 8, 4, 0, 0, 0, "Evil"},
	{ 17, 8, 4, 0, 0, 0, "Order"},
	{ 25, 8, 4, 0, 0, 0, "Chaos"},
	{ 33, 8, 4, 0, 0, 0, "Balance"},
	{ 1, 4, -1, 0, 0, 0, "[Right  ]"},
};
#endif

static void display_player_one_line(int entry, cptr val, byte attr)
{
	char buf[60];
	int row = disp_creature_line[entry].row;
	int col = disp_creature_line[entry].col;
	int len = disp_creature_line[entry].len;
	cptr head = disp_creature_line[entry].header;
	int head_len = strlen(head);

	Term_putstr(col, row, -1, TERM_WHITE, head);
	if(!val) return;
	if(len > 0)
	{
		int val_len = len - head_len;
		sprintf(buf, "%*.*s", val_len, val_len, val);
		Term_putstr(col + head_len, row, -1, attr, buf);
	}
	else Term_putstr(col + head_len, row, -1, attr, val);

	return;
}


// Prints the following information on the screen.
static void display_player_middle(creature_type *creature_ptr)
{
	char buf[160], buf1[30], buf2[30];
	byte attr;

	int melee_num = 0;
	int percentage;

	// Base skill
	int show_tohit = creature_ptr->dis_to_hit_b;
	int show_todam = 0;

	// Range weapon
	object_type *bow_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_BOW, 0);

	int tmul = 0;
	int i, j, k, e;
	c_put_str(TERM_WHITE, MES_INTERFACE_MELEE_LIST, 14, 1);

	for(i = 0; i < MAX_WEAPONS; i++)
	{
		if(creature_ptr->can_melee[i])
		{
			object_type *weapon_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_HAND, i);
			j = calc_weapon_melee_cost(creature_ptr, weapon_ptr);
			k = calc_weapon_melee_priority(creature_ptr, weapon_ptr);
			c_put_str(TERM_L_BLUE, mention_use_ptr(creature_ptr, weapon_ptr), 15 + melee_num, 1);
			c_put_str(TERM_YELLOW, format("%+4d", creature_ptr->to_hit[i]), 15 + melee_num, 9);
			c_put_str(TERM_YELLOW, format("%dd%d%+d", weapon_ptr->dd, weapon_ptr->ds, creature_ptr->to_damage[i]), 15 + melee_num, 14);
			c_put_str(TERM_YELLOW, format("%3d", j), 15 + melee_num, 22);
			c_put_str(TERM_YELLOW, format("%4d", k), 15 + melee_num, 27);
			melee_num++;
		}
	}

	/* Apply weapon bonuses */
	if(bow_ptr->k_idx)
	{
		if(object_is_known(bow_ptr)) show_tohit += bow_ptr->to_hit;
		if(object_is_known(bow_ptr)) show_todam += bow_ptr->to_damage;

		/*
		if((bow_ptr->sval == SV_LIGHT_XBOW) || (bow_ptr->sval == SV_HEAVY_XBOW))
			show_tohit += creature_ptr->weapon_exp[0][bow_ptr->sval] / 400;
		else
			show_tohit += (creature_ptr->weapon_exp[0][bow_ptr->sval] - (WEAPON_EXP_MASTER / 2)) / 200;
		*/

		/* Range attacks */
		display_player_one_line(ENTRY_SHOOT, format("(%+4d,%+4d)x%2d.%02d:%4d", show_tohit, show_todam, tmul/100, tmul%100, 0), TERM_L_BLUE);
		display_player_one_line(ENTRY_THROW, format("(%+4d,%+4d)x%2d.%02d:----", show_tohit, show_todam, tmul/100, tmul%100), TERM_L_BLUE);

		if(bow_ptr->k_idx)
		{
			tmul = bow_ptr->bow_mul;

			/* Get extra "power" from "extra might" */
			//TODO if(creature_ptr->xtra_might) tmul++;

			tmul = tmul * (100 + (int)(adj_str_to_damage[creature_ptr->stat_ind[STAT_STR]]) - 128);
		}
	}

	// Dump the armor class and evasion
	display_player_one_line(ENTRY_BASE_AC, format("[%d,%+d]", creature_ptr->dis_ac, creature_ptr->dis_to_ac), TERM_L_BLUE);
	display_player_one_line(ENTRY_BASE_EV, format("[%d,%+d]", creature_ptr->dis_ev, creature_ptr->dis_to_ev), TERM_L_BLUE);

	percentage = creature_ptr->carrying_weight * 100 / calc_carrying_weight_limit(creature_ptr);
	if(percentage <= WEIGHT_LIMIT_LIGHT) attr = TERM_L_BLUE;
	else if(percentage <= WEIGHT_LIMIT_MIDDLE) attr = TERM_L_GREEN;
	else if(percentage <= WEIGHT_LIMIT_HEAVY) attr = TERM_YELLOW;
	else attr = TERM_L_RED;
	format_weight(buf1, creature_ptr->carrying_weight);
	format_weight(buf2, calc_carrying_weight_limit(creature_ptr));
	display_player_one_line(ENTRY_CARRYING_WEIGHT, format("%s/%s(%3d%%)", buf1, buf2, percentage), attr);

	percentage = creature_ptr->equipping_weight * 100 / calc_equipping_weight_limit(creature_ptr);
	if(percentage <= WEIGHT_LIMIT_LIGHT) attr = TERM_L_BLUE;
	else if(percentage <= WEIGHT_LIMIT_MIDDLE) attr = TERM_L_GREEN;
	else if(percentage <= WEIGHT_LIMIT_HEAVY) attr = TERM_YELLOW;
	else attr = TERM_L_RED;
	format_weight(buf1, creature_ptr->equipping_weight);
	format_weight(buf2, calc_equipping_weight_limit(creature_ptr));
	display_player_one_line(ENTRY_EQUIPPING_WEIGHT, format("%s/%s(%3d%%)", buf1, buf2, percentage), attr);

	// Dump speed
	{
		int tmp_speed = 0;
		byte attr;
		int i;

		i = creature_ptr->speed;

		if(creature_ptr->action == ACTION_SEARCH) i += 10; // Hack -- Visually "undo" the Search Mode Slowdown

		if(i > 0)
		{
			if(!creature_ptr->riding) attr = TERM_L_GREEN;
			else attr = TERM_GREEN;
		}
		else if(i == 0)
		{
			if(!creature_ptr->riding) attr = TERM_L_BLUE;
			else attr = TERM_GREEN;
		}
		else
		{
			if(!creature_ptr->riding) attr = TERM_L_UMBER;
			else attr = TERM_RED;
		}

		if(!creature_ptr->riding)
		{
			if(has_trait(creature_ptr, TRAIT_FAST)) tmp_speed += 10;
			if(has_trait(creature_ptr, TRAIT_SLOW)) tmp_speed -= 10;
			if(has_trait(creature_ptr, TRAIT_LIGHT_SPEED)) tmp_speed = 99;
		}
		else
		{
			if(has_trait(&creature_list[creature_ptr->riding], TRAIT_FAST)) tmp_speed += 10;
			if(has_trait(&creature_list[creature_ptr->riding], TRAIT_SLOW)) tmp_speed -= 10;
			if(has_trait(&creature_list[creature_ptr->riding], TRAIT_LIGHT_SPEED)) tmp_speed = 99;
		}

		if(tmp_speed)
		{
			if(!creature_ptr->riding)
				sprintf(buf, "(%+d%+d)", i-tmp_speed, tmp_speed);
			else
#ifdef JP
				sprintf(buf, "騎乗中 (%+d%+d)", i-tmp_speed, tmp_speed);
#else
				sprintf(buf, "Riding (%+d%+d)", i-tmp_speed, tmp_speed);
#endif

			if(tmp_speed > 0)
				attr = TERM_YELLOW;
			else
				attr = TERM_VIOLET;
		}
		else
		{
			if(!creature_ptr->riding)
				sprintf(buf, "(%+d)", i);
			else
#ifdef JP
				sprintf(buf, "騎乗中 (%+d)", i);
#else
				sprintf(buf, "Riding (%+d)", i);
#endif
		}
	
		display_player_one_line(ENTRY_SPEED, buf, attr);
	}

	/* Dump experience */
	if(has_trait(creature_ptr, TRAIT_ANDROID)) e = ENTRY_EXP_ANDR;
	else e = ENTRY_CUR_EXP;

	if(creature_ptr->exp >= creature_ptr->max_exp)
		display_player_one_line(e, format("%ld", creature_ptr->exp), TERM_L_GREEN);
	else
		display_player_one_line(e, format("%ld", creature_ptr->exp), TERM_YELLOW);

	/* Dump max experience */
	if(has_trait(creature_ptr, TRAIT_ANDROID))
		/* Nothing */;
	else
		display_player_one_line(ENTRY_MAX_EXP, format("%ld", creature_ptr->max_exp), TERM_L_GREEN);

	/* Dump exp to advance */
	if(has_trait(creature_ptr, TRAIT_ANDROID)) e = ENTRY_EXP_TO_ADV_ANDR;
	else e = ENTRY_EXP_TO_ADV;

	if(creature_ptr->lev >= creature_ptr->max_lev)
		display_player_one_line(e, "*****", TERM_L_GREEN);
	else if(has_trait(creature_ptr, TRAIT_ANDROID))
		display_player_one_line(e, format("%ld", (s32b)(creature_exp_a[creature_ptr->lev - 1] * creature_ptr->expfact / 100L)), TERM_L_GREEN);
	else
		display_player_one_line(e, format("%ld", (s32b)(creature_exp[creature_ptr->lev - 1] * creature_ptr->expfact / 100L)), TERM_L_GREEN);

	/* Dump gold */
	display_player_one_line(ENTRY_GOLD, format("%ld", creature_ptr->au), TERM_L_GREEN);

	// Dump Day(TODO)
	//display_player_one_line(ENTRY_DAY, buf, TERM_L_GREEN);

	/* Dump play time */
	display_player_one_line(ENTRY_PLAY_TIME, format("%.2lu:%.2lu:%.2lu", play_time / (60 * 60), (play_time / 60) % 60, play_time % 60), TERM_L_GREEN);
}

// Returns a "rating" of x depending on y
static COLOR_ID likert(char *buf, int x, int y)
{
	COLOR_ID id;

	static COLOR_ID rank_color[] =
	{
		TERM_L_DARK,
		TERM_RED,
		TERM_L_RED,
		TERM_ORANGE,
		TERM_ORANGE,
		TERM_YELLOW,
		TERM_YELLOW,
		TERM_L_GREEN,
		TERM_L_GREEN,
		TERM_GREEN,
		TERM_GREEN,
		TERM_GREEN,
		TERM_GREEN,
		TERM_BLUE,
		TERM_BLUE,
		TERM_BLUE,
		TERM_BLUE,
		TERM_VIOLET,
		TERM_VIOLET,
		TERM_VIOLET,
		TERM_VIOLET,
		TERM_WHITE,
	};

	static cptr rank_desc[] =
	{
#ifdef JP
		"無能",
		"劣悪",
		"苦手",
		"凡庸",
		"凡庸",
		"得意",
		"熟練",
		"卓越",
		"卓越",
		"超越",
		"超越",
		"超越",
		"超越",
		"英雄的",
		"英雄的",
		"英雄的",
		"英雄的",
		"伝説的",
		"伝説的",
		"伝説的",
		"伝説的",
		"神域",
#else
		"Very Bad",
		"Bad",
		"Poor",
		"Fair",
		"Fair",
		"Good",
		"Very Good",
		"Excellent",
		"Excellent",
		"Surerb",
		"Surerb",
		"Surerb",
		"Surerb",
		"Heroic",
		"Heroic",
		"Heroic",
		"Heroic",
		"Legendary",
		"Legendary",
		"Legendary",
		"Legendary",
		"Devine",
#endif
	};

	if(y <= 0) y = 1;
	id = (x / y) > 21 ? 21 : (COLOR_ID)(x / y);

	// Negative value
	if(x < 0)
	{
		sprintf(buf, "   0[%s]", rank_desc[0]);
		return TERM_L_DARK;
	}

	// Analyze the value
	sprintf(buf, "%4d[%s]", (int)(x), rank_desc[(x / y) > 21 ? 21 : (x / y)]);
	return rank_color[id];
}


/*
 * Prints ratings on certain abilities
 * This code is "imitated" elsewhere to "dump" a character sheet.
 */
static void display_player_various(creature_type * creature_ptr)
{
	int			xthn, xthb, xfos, xsrh;
	int			xdis, xdev, xstl; // xrob, xagi, xvol
	int			xdig;
	char		desc[40];
	int         muta_att = 0;
	int		shots, shot_frac;
	COLOR_ID col;

	object_type		*object_ptr;

	if(has_trait(creature_ptr, TRAIT_HORNS))     muta_att++;
	if(has_trait(creature_ptr, TRAIT_SCOR_TAIL)) muta_att++;
	if(has_trait(creature_ptr, TRAIT_BEAK))      muta_att++;
	if(has_trait(creature_ptr, TRAIT_TRUNK))     muta_att++;
	if(has_trait(creature_ptr, TRAIT_TENTACLES)) muta_att++;

	/* Shooting Skill (with current bow and normal missile) */
	object_ptr = get_equipped_slot_ptr(creature_ptr, SLOT_ID_BOW, 0);

	/* If the player is wielding one? */
	if(is_valid_object(object_ptr))
	{
		s16b energy_fire = object_ptr->bow_energy;

		/* Calculate shots per round */
		shots = creature_ptr->num_fire * 100;
		shot_frac = (shots * 100 / energy_fire) % 100;
		shots = shots / energy_fire;

		/* TODO
		if(object_ptr->art_id == ART_CRIMSON)
		{
			shots = 1;
			shot_frac = 0;
			if(creature_ptr->class_idx == CLASS_ARCHER)
			{
				if(creature_ptr->lev >= 10) shots++;
				if(creature_ptr->lev >= 30) shots++;
				if(creature_ptr->lev >= 45) shots++;
			}
		}
		*/
	}
	else
	{
		shots = 0;
		shot_frac = 0;
	}

	// Basic abilities
	xthn = creature_ptr->skill_melee;
	xthb = creature_ptr->skill_shooting;
	xdis = creature_ptr->skill_disarm;
	xdev = creature_ptr->skill_device;
	xstl = creature_ptr->skill_stealth;
	xsrh = creature_ptr->skill_perception;
	xfos = creature_ptr->skill_penetration;
	xdig = creature_ptr->skill_digging;

	col = likert(desc, xthn, 10);
	display_player_one_line(ENTRY_SKILL_FIGHT, desc, col);

	col = likert(desc, xthb, 10);
	display_player_one_line(ENTRY_SKILL_SHOOT, desc, col);

	/*TODO
	col = likert(desc, xrob, 5);
	display_player_one_line(ENTRY_SKILL_ROBUSTNESS, desc, col);

	col = likert(desc, xagi, 5);
	display_player_one_line(ENTRY_SKILL_EVALITY, desc, col);

	col = likert(desc, xvol, 5);
	display_player_one_line(ENTRY_SKILL_VOLITION, desc, col);
	*/

	/* Hack -- 0 is "minimum stealth value", so print "Very Bad" */
	col = likert(desc, (xstl > 0) ? xstl : -1, 1);
	display_player_one_line(ENTRY_SKILL_STEALTH, desc, col);

	col = likert(desc, xfos, 5);
	display_player_one_line(ENTRY_SKILL_PERCEP, desc, col);

	col = likert(desc, xsrh, 5);
	display_player_one_line(ENTRY_SKILL_SEARCH, desc, col);

	col = likert(desc, xdis, 5);
	display_player_one_line(ENTRY_SKILL_DISARM, desc, col);

	col = likert(desc, xdev, 5);
	display_player_one_line(ENTRY_SKILL_DEVICE, desc, col);

	col = likert(desc, xdig, 5);
	display_player_one_line(ENTRY_SKILL_DIGGING, desc, col);

/*
	if(!muta_att)
		display_player_one_line(ENTRY_BLOWS, format("%d+%d", blows1, blows2), TERM_L_BLUE);
	else
		display_player_one_line(ENTRY_BLOWS, format("%d+%d+%d", blows1, blows2, muta_att), TERM_L_BLUE);

	display_player_one_line(ENTRY_SHOTS, format("%d.%02d", shots, shot_frac), TERM_L_BLUE);

	desc = format("%d+%d", blows1 * damage[0] / 100, blows2 * damage[1] / 100);
	display_player_one_line(ENTRY_AVG_DMG, desc, TERM_L_BLUE);
*/

	//display_player_one_line(ENTRY_INFRA, format("%d feet", creature_ptr->see_infra * 10), TERM_WHITE);
}

/* Mode flags for displaying player flags */
#define DP_CURSE   0x01
#define DP_IMM     0x02
#define DP_TWO_LINES 0x04
#define DP_WP      0x08

// Equippy chars
static void display_player_equippy(int y, int x, u16b mode, creature_type *creature_ptr)
{
	int i, j;
	byte a;
	char c;

	object_type *object_ptr;

	/* Dump equippy chars */
	for (i = 0, j = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i]; // Object

		if(!IS_EQUIPPED(object_ptr)) continue;
		if((mode & DP_WP) && GET_SLOT_ID_TYPE(creature_ptr, i) != SLOT_ID_HAND) continue;

		a = object_attr(object_ptr);
		c = object_char(object_ptr);

		/* Clear the part of the screen */
		if(!equippy_chars || !object_ptr->k_idx)
		{
			c = 'x';
			a = TERM_L_DARK;

		}

		Term_putch(x + (j++), y, a, c);
		
	}
}

static void known_obj_immunity(u32b flgs[MAX_TRAITS_FLAG], creature_type *creature_ptr)
{
	int i;
	for (i = 0; i < MAX_TRAITS_FLAG; i++) flgs[i] = 0L;

	/* Check equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		u32b o_flgs[MAX_TRAITS_FLAG];

		object_type *object_ptr;

		object_ptr = &creature_ptr->inventory[i]; // object

		if(!IS_EQUIPPED(object_ptr)) continue;
		if(!is_valid_object(object_ptr)) continue;

		/* Known flags */
		object_flags_known(object_ptr, o_flgs);

		if(have_flag(o_flgs, TRAIT_IM_ACID)) add_flag(flgs, TRAIT_RES_ACID);
		if(have_flag(o_flgs, TRAIT_IM_ELEC)) add_flag(flgs, TRAIT_RES_ELEC);
		if(have_flag(o_flgs, TRAIT_IM_FIRE)) add_flag(flgs, TRAIT_RES_FIRE);
		if(have_flag(o_flgs, TRAIT_IM_COLD)) add_flag(flgs, TRAIT_RES_COLD);
	}
}

static void tim_player_immunity(u32b flgs[MAX_TRAITS_FLAG], creature_type *creature_ptr)
{
	int i;

	// Clear
	for (i = 0; i < MAX_TRAITS_FLAG; i++) flgs[i] = 0L;
	if(has_trait(creature_ptr, TRAIT_WRAITH_FORM)) add_flag(flgs, TRAIT_RES_DARK);
}

static void player_vuln_flags(u32b flgs[MAX_TRAITS_FLAG], creature_type *creature_ptr)
{
	int i;
	for (i = 0; i < MAX_TRAITS_FLAG; i++) flgs[i] = 0L;

	if(has_trait(creature_ptr, TRAIT_VULN_ELEM) || (has_trait(creature_ptr, TRAIT_POSTURE_KOUKIJIN)))
	{
		add_flag(flgs, TRAIT_RES_ACID);
		add_flag(flgs, TRAIT_RES_ELEC);
		add_flag(flgs, TRAIT_RES_FIRE);
		add_flag(flgs, TRAIT_RES_COLD);
	}
	if(has_trait(creature_ptr, TRAIT_ANDROID)) add_flag(flgs, TRAIT_RES_ELEC);
	if(IS_RACE(creature_ptr, RACE_ENT)) add_flag(flgs, TRAIT_RES_FIRE);

	/*TODO
	if(IS_RACE(creature_ptr, RACE_VAMPIRE) || IS_RACE(creature_ptr, RACE_S_FAIRY) ||
	    (creature_ptr->mimic_race_idx == MIMIC_VAMPIRE))
		add_flag(flgs, TRAIT_RES_LITE);
	*/
}


/*
 * A struct for storing misc. flags
 */
typedef struct {
	u32b player_flags[MAX_TRAITS_FLAG];
	u32b tim_player_flags[MAX_TRAITS_FLAG];
	u32b player_imm[MAX_TRAITS_FLAG];
	u32b tim_player_imm[MAX_TRAITS_FLAG];
	u32b player_vuln[MAX_TRAITS_FLAG];
	u32b known_obj_imm[MAX_TRAITS_FLAG];
} all_player_flags;


/*
 * Helper function, see below
 */
static void display_flag_aux(int row, int col, cptr header, int flag1, all_player_flags *f, u16b mode, creature_type *creature_ptr)
{
	int i;
	bool vuln = FALSE;

	if(have_flag(f->player_vuln, flag1) && !(have_flag(f->known_obj_imm, flag1) || have_flag(f->player_imm, flag1) || have_flag(f->tim_player_imm, flag1)))
		vuln = TRUE;

	
	if(!(mode & DP_IMM)) c_put_str(TERM_WHITE, header, row, col); /* Header */

	/* Advance */
	col += strlen(header) + 1;

	/* Check equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		u32b flgs[MAX_TRAITS_FLAG];
		object_type *object_ptr;

		/* Object */
		object_ptr = &creature_ptr->inventory[i];

		if(!IS_EQUIPPED(object_ptr)) continue;
		if((mode & DP_WP) && WIELD_SLOT(object_ptr) != SLOT_ID_HAND) continue;

		/* Known flags */
		object_flags_known(object_ptr, flgs);

		if(!(mode & DP_IMM))
			c_put_str((byte)(vuln ? TERM_RED : TERM_SLATE), ".", row, col);

		/* Check flags */
		if(mode & DP_CURSE)
		{
			if((mode & DP_CURSE) && have_flag(object_ptr->curse_flags, TRAIT_CURSED) && have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))
				c_put_str(TERM_WHITE, "+", row, col);
			if((mode & DP_CURSE) && have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE))
				c_put_str(TERM_WHITE, "*", row, col);
		}
		else
		{
			if(have_flag(flgs, flag1))
				c_put_str((byte)(vuln ? TERM_L_RED : TERM_WHITE),
					  (mode & DP_IMM) ? "*" : "+", row, col);
		}

		/* Advance */
		col++;
	}

	/* Assume that player flag is already written */
	if(mode & DP_IMM) return;

	c_put_str((byte)(vuln ? TERM_RED : TERM_SLATE), ".", row, col);

	/* Player flags */
	if(have_flag(f->player_flags, flag1)) c_put_str((byte)(vuln ? TERM_L_RED : TERM_WHITE), "+", row, col);

	/* Timed player flags */
	if(have_flag(f->tim_player_flags, flag1)) c_put_str((byte)(vuln ? TERM_ORANGE : TERM_YELLOW), "#", row, col);

	/* Immunity */
	if(have_flag(f->tim_player_imm, flag1)) c_put_str(TERM_YELLOW, "*", row, col);
	if(have_flag(f->player_imm, flag1)) c_put_str(TERM_WHITE, "*", row, col);

	/* Vulnerability */
	if(vuln) c_put_str(TERM_RED, "v", row, col + 1);

}

static cptr get_equipped_flag_label(creature_type *creature_ptr, u16b mode)
{
	int i, n;
	cptr list = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static char res[INVEN_TOTAL] = "";

	for(i = 0, n = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];
		if((mode & DP_WP) && WIELD_SLOT(object_ptr) != SLOT_ID_HAND) continue;
		if(IS_EQUIPPED(object_ptr))
		{
			res[n] = list[i];
			n++;
		}
	}
	res[n++] = '@';
	res[n++] = '\0';

	return res;
}

/*
 * Special display, part 1
 */
static void display_creature_flag_info1(creature_type *creature_ptr)
{
	int row;
	int col;
	char buf[80];
	int rate;

	all_player_flags f;

	/* Extract flags and store */
	//TODO player_immunity(f.player_imm, creature_ptr);
	//TODO tim_player_immunity(f.tim_player_imm, creature_ptr);
	known_obj_immunity(f.known_obj_imm, creature_ptr);
	player_vuln_flags(f.player_vuln, creature_ptr);

	/*** Set 1 ***/

	row = 12;
	col = 1;

	display_player_equippy(row-2, col+8, 0, creature_ptr);

#ifdef JP
	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, 0), row-1, col+8);
	display_flag_aux(row+0, col, "耐酸  :", TRAIT_RES_ACID, &f, 0, creature_ptr);
	display_flag_aux(row+0, col, "耐酸  :", TRAIT_IM_ACID, &f, DP_IMM, creature_ptr);
	display_flag_aux(row+1, col, "耐電撃:", TRAIT_RES_ELEC, &f, 0, creature_ptr);
	display_flag_aux(row+1, col, "耐電撃:", TRAIT_IM_ELEC, &f, DP_IMM, creature_ptr);
	display_flag_aux(row+2, col, "耐火炎:", TRAIT_RES_FIRE, &f, 0, creature_ptr);
	display_flag_aux(row+2, col, "耐火炎:", TRAIT_IM_FIRE, &f, DP_IMM, creature_ptr);
	display_flag_aux(row+3, col, "耐冷気:", TRAIT_RES_COLD, &f, 0, creature_ptr);
	display_flag_aux(row+3, col, "耐冷気:", TRAIT_IM_COLD, &f, DP_IMM, creature_ptr);
	display_flag_aux(row+4, col, "耐毒  :", TRAIT_RES_POIS, &f, 0, creature_ptr);
	display_flag_aux(row+5, col, "耐閃光:", TRAIT_RES_LITE, &f, 0, creature_ptr);
	display_flag_aux(row+6, col, "耐暗黒:", TRAIT_RES_DARK, &f, 0, creature_ptr);
	display_flag_aux(row+7, col, "耐破片:", TRAIT_RES_SHAR, &f, 0, creature_ptr);
	display_flag_aux(row+8, col, "耐盲目:", TRAIT_NO_BLIND, &f, 0, creature_ptr);
	display_flag_aux(row+9, col, "耐混乱:", TRAIT_NO_CONF, &f, 0, creature_ptr);
#else
	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, 0), row-1, col+8);
	display_flag_aux(row+0, col, "Acid  :", TRAIT_RES_ACID, &f, 0, creature_ptr);
	display_flag_aux(row+0, col, "Acid  :", TRAIT_IM_ACID, &f, DP_IMM, creature_ptr);
	display_flag_aux(row+1, col, "Elec  :", TRAIT_RES_ELEC, &f, 0, creature_ptr);
	display_flag_aux(row+1, col, "Elec  :", TRAIT_IM_ELEC, &f, DP_IMM, creature_ptr);
	display_flag_aux(row+2, col, "Fire  :", TRAIT_RES_FIRE, &f, 0, creature_ptr);
	display_flag_aux(row+2, col, "Fire  :", TRAIT_IM_FIRE, &f, DP_IMM, creature_ptr);
	display_flag_aux(row+3, col, "Cold  :", TRAIT_RES_COLD, &f, 0, creature_ptr);
	display_flag_aux(row+3, col, "Cold  :", TRAIT_IM_COLD, &f, DP_IMM, creature_ptr);
	display_flag_aux(row+4, col, "Poison:", TRAIT_RES_POIS, &f, 0, creature_ptr);
	display_flag_aux(row+5, col, "Light :", TRAIT_RES_LITE, &f, 0, creature_ptr);
	display_flag_aux(row+6, col, "Dark  :", TRAIT_RES_DARK, &f, 0, creature_ptr);
	display_flag_aux(row+7, col, "Shard :", TRAIT_RES_SHAR, &f, 0, creature_ptr);
	display_flag_aux(row+8, col, "Blind :", TRAIT_NO_BLIND, &f, 0, creature_ptr);
	display_flag_aux(row+9, col, "Conf  :", TRAIT_NO_CONF, &f, 0, creature_ptr);
#endif

	//acid
	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_ACID, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_ACID))
		c_put_str(TERM_WHITE, "+", row + 0, col + 31);
	if(has_trait(creature_ptr, TRAIT_OPP_ACID))
		c_put_str(TERM_YELLOW, "#", row + 0, col + 31);
	if(has_trait(creature_ptr, TRAIT_IM_ACID))
		c_put_str(TERM_YELLOW, "*", row + 0, col * 31);
	c_put_str(TERM_YELLOW, buf, row+0, col + 33);

	//elec
	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_ELEC, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_ELEC))
		c_put_str((byte)(has_trait(creature_ptr, TRAIT_HURT_ELEC) ? TERM_L_RED : TERM_WHITE), "+", row + 1, col + 31);
	if(has_trait(creature_ptr, TRAIT_OPP_ELEC))
		c_put_str((byte)(has_trait(creature_ptr, TRAIT_HURT_ELEC) ? TERM_ORANGE : TERM_YELLOW), "#", row + 1, col + 31);
	if(has_trait(creature_ptr, TRAIT_IM_ELEC))
		c_put_str(TERM_YELLOW, "*", row + 1, col * 31);

	if(has_trait(creature_ptr, TRAIT_HURT_ELEC))
		c_put_str(TERM_RED, "v", row + 1, col + 31);

	c_put_str(TERM_YELLOW, buf, row+1, col + 33);

	//fire
	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_FIRE, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_FIRE))
		c_put_str((byte)(has_trait(creature_ptr, TRAIT_HURT_FIRE) ? TERM_L_RED : TERM_WHITE), "+", row + 2, col + 31);
	if(has_trait(creature_ptr, TRAIT_OPP_FIRE))
		c_put_str((byte)(has_trait(creature_ptr, TRAIT_HURT_FIRE) ? TERM_ORANGE : TERM_YELLOW), "#", row + 2, col + 31);
	if(has_trait(creature_ptr, TRAIT_IM_FIRE))
		c_put_str(TERM_YELLOW, "*", row + 2, col * 31);
	if(has_trait(creature_ptr, TRAIT_HURT_FIRE))
		c_put_str(TERM_RED, "v", row + 2, col + 31);
	c_put_str(TERM_YELLOW, buf, row + 2, col + 33);

	//cold


	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_COLD, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_COLD))
		c_put_str((byte)(has_trait(creature_ptr, TRAIT_HURT_COLD) ? TERM_L_RED : TERM_WHITE), "+", row + 3, col + 31);
	if(has_trait(creature_ptr, TRAIT_OPP_COLD))
		c_put_str((byte)(has_trait(creature_ptr, TRAIT_HURT_COLD) ? TERM_ORANGE : TERM_YELLOW), "#", row + 3, col + 31);
	if(has_trait(creature_ptr, TRAIT_IM_COLD))
		c_put_str(TERM_YELLOW, "*", row + 3, col * 31);
	if(has_trait(creature_ptr, TRAIT_HURT_COLD))
		c_put_str(TERM_RED, "v", row + 3, col + 31);
	c_put_str(TERM_YELLOW, buf, row + 3, col + 33);

	//pois
	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_POIS, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_POIS))
		c_put_str(TERM_WHITE, "+", row + 4, col + 31);
	if(has_trait(creature_ptr, TRAIT_OPP_POIS))
		c_put_str(TERM_YELLOW, "#", row + 4, col + 31);
	if(has_trait(creature_ptr, TRAIT_IM_POIS))
		c_put_str(TERM_YELLOW, "*", row + 4, col * 31);
	c_put_str(TERM_YELLOW, buf, row+4, col + 33);

	//lite
	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_LITE, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_LITE))
		c_put_str(TERM_WHITE, "+", row + 5, col + 31);
	c_put_str(TERM_YELLOW, buf, row+5, col + 33);

	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_DARK, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_DARK))
		c_put_str(TERM_WHITE, "+", row + 6, col + 31);
	c_put_str(TERM_YELLOW, buf, row+6, col + 33);

	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_SHARDS, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_SHAR))
		c_put_str(TERM_WHITE, "+", row + 5, col + 31);
	c_put_str(TERM_YELLOW, buf, row+7, col + 33);


	/*** Set 2 ***/

	row = 12;
	col = 41;

	display_player_equippy(row-2, col+8, 0, creature_ptr);


#ifdef JP
	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, 0), row-1, col+8);
	display_flag_aux(row+0, col, "耐轟音:", TRAIT_RES_SOUN, &f, 0, creature_ptr);
	display_flag_aux(row+1, col, "耐地獄:", TRAIT_RES_NETH, &f, 0, creature_ptr);
	display_flag_aux(row+2, col, "耐因混:", TRAIT_RES_NEXU, &f, 0, creature_ptr);
	display_flag_aux(row+3, col, "耐カオ:", TRAIT_RES_CHAO, &f, 0, creature_ptr);
	display_flag_aux(row+4, col, "耐劣化:", TRAIT_RES_DISE, &f, 0, creature_ptr);
	display_flag_aux(row+5, col, "耐恐怖:", TRAIT_FEARLESS, &f, 0, creature_ptr);
#else
	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, 0), row-1, col+8);
	display_flag_aux(row+0, col, "Sound :", TRAIT_RES_SOUN, &f, 0, creature_ptr);
	display_flag_aux(row+1, col, "Nether:", TRAIT_RES_NETH, &f, 0, creature_ptr);
	display_flag_aux(row+2, col, "Nexus :", TRAIT_RES_NEXU, &f, 0, creature_ptr);
	display_flag_aux(row+3, col, "Chaos :", TRAIT_RES_CHAO, &f, 0, creature_ptr);
	display_flag_aux(row+4, col, "Disnch:", TRAIT_RES_DISE, &f, 0, creature_ptr);
	display_flag_aux(row+5, col, "Fear  :", TRAIT_FEARLESS, &f, 0, creature_ptr);
#endif

	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_SOUND, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_SOUN))
		c_put_str(TERM_WHITE, "+", row + 0, col + 31);
	c_put_str(TERM_YELLOW, buf, row+0, col + 33);

	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_NETHER, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_NETH))
		c_put_str(TERM_WHITE, "+", row + 1, col + 31);
	c_put_str(TERM_YELLOW, buf, row+1, col + 33);

	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_NEXUS, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_NEXU))
		c_put_str(TERM_WHITE, "+", row + 2, col + 31);
	c_put_str(TERM_YELLOW, buf, row+2, col + 33);

	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_CHAOS, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_CHAO))
		c_put_str(TERM_WHITE, "+", row + 3, col + 31);
	c_put_str(TERM_YELLOW, buf, row+3, col + 33);

	rate = calc_damage(NULL, creature_ptr, 100, DO_EFFECT_DISENCHANT, FALSE, TRUE);
	sprintf(buf, "x%1d.%02d", rate / 100, rate % 100);
	if(has_trait(creature_ptr, TRAIT_RES_DISE))
		c_put_str(TERM_WHITE, "+", row + 4, col + 31);
	c_put_str(TERM_YELLOW, buf, row+4, col + 33);

}

// Special display, part 2
static void display_creature_flag_info2(creature_type *creature_ptr)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	//TODO player_immunity(f.player_imm, creature_ptr);
	//TODO tim_player_immunity(f.tim_player_imm, creature_ptr);
	known_obj_immunity(f.known_obj_imm, creature_ptr);
	player_vuln_flags(f.player_vuln, creature_ptr);

	/*** Set 3 ***/

	row = 3;
	col = 1;

	display_player_equippy(row-2, col+12, 0, creature_ptr);

	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, 0), row-1, col+12);

/*
#ifdef JP
	display_flag_aux(row+0, col, "反射      :", TRAIT_REFLECTING, &f, 0, creature_ptr);
	display_flag_aux(row+1, col, "火炎オーラ:", TRAIT_AURA_FIRE, &f, 0, creature_ptr);
	display_flag_aux(row+2, col, "電気オーラ:", TRAIT_AURA_ELEC, &f, 0, creature_ptr);
	display_flag_aux(row+3, col, "冷気オーラ:", TRAIT_AURA_COLD, &f, 0, creature_ptr);
	display_flag_aux(row+4, col, "加速      :", TRAIT_SPEED, &f, 0, creature_ptr);
	display_flag_aux(row+5, col, "耐麻痺    :", TRAIT_FREE_ACTION, &f, 0, creature_ptr);
	display_flag_aux(row+6, col, "透明体視認:", TRAIT_SEE_INVISIBLE, &f, 0, creature_ptr);
	display_flag_aux(row+7, col, "経験値保持:", TRAIT_HOLD_LIFE, &f, 0, creature_ptr);
	display_flag_aux(row+8, col, "警告      :", TRAIT_WARNING, &f, 0, creature_ptr);
	display_flag_aux(row+9, col, "遅消化    :", TRAIT_SLOW_DIGEST, &f, 0, creature_ptr);
	display_flag_aux(row+10, col, "急回復    :", TRAIT_REGENERATE, &f, 0, creature_ptr);
	display_flag_aux(row+11, col, "浮遊      :", TRAIT_LEVITATION, &f, 0, creature_ptr);
	display_flag_aux(row+12, col, "永遠光源  :", TRAIT_LITE, &f, 0, creature_ptr);
	display_flag_aux(row+13, col, "呪い      :", 0, &f, DP_CURSE, creature_ptr);
#else
	display_flag_aux(row+0, col, "Reflct    :", TRAIT_REFLECTING, &f, 0, creature_ptr);
	display_flag_aux(row+1, col, "AuraFire  :", TRAIT_AURA_FIRE, &f, 0, creature_ptr);
	display_flag_aux(row+2, col, "AuraElec  :", TRAIT_AURA_ELEC, &f, 0, creature_ptr);
	display_flag_aux(row+3, col, "AuraCold  :", TRAIT_AURA_COLD, &f, 0, creature_ptr);
	display_flag_aux(row+4, col, "Speed     :", TRAIT_SPEED, &f, 0, creature_ptr);
	display_flag_aux(row+5, col, "FreeAction:", TRAIT_FREE_ACTION, &f, 0, creature_ptr);
	display_flag_aux(row+6, col, "SeeInvisi.:", TRAIT_SEE_INVISIBLE, &f, 0, creature_ptr);
	display_flag_aux(row+7, col, "Hold Life :", TRAIT_HOLD_LIFE, &f, 0, creature_ptr);
	display_flag_aux(row+8, col, "Warning   :", TRAIT_WARNING, &f, 0, creature_ptr);
	display_flag_aux(row+9, col, "SlowDigest:", TRAIT_SLOW_DIGEST, &f, 0, creature_ptr);
	display_flag_aux(row+10, col, "Regene.   :", TRAIT_REGENERATE, &f, 0, creature_ptr);
	display_flag_aux(row+11, col, "Levitation:", TRAIT_LEVITATION, &f, 0, creature_ptr);
	display_flag_aux(row+12, col, "Perm Lite :", TRAIT_LITE, &f, 0, creature_ptr);
	display_flag_aux(row+13, col, "Cursed    :", 0, &f, DP_CURSE, creature_ptr);
#endif
*/
}

// Special display, part 3
static void display_creature_flag_info3(creature_type *creature_ptr)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	//TODO player_immunity(f.player_imm, creature_ptr);
	//TODO tim_player_immunity(f.tim_player_imm, creature_ptr);
	known_obj_immunity(f.known_obj_imm, creature_ptr);
	player_vuln_flags(f.player_vuln, creature_ptr);

	/*** Set 1 ***/

	row = 3;
	col = 1;

	display_player_equippy(row-2, col+12, DP_WP, creature_ptr);

	/*** Set 2 ***/

	row = 3;
	col = 1;

	display_player_equippy(row-2, col+12, 0, creature_ptr);
	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, 0), row-1, col+12);

/*
#ifdef JP
	display_flag_aux(row+ 0, col, "テレパシー:", TRAIT_ESP, &f, 0, creature_ptr);
	display_flag_aux(row+ 1, col, "邪悪ESP   :", TRAIT_SENSE_EVIL, &f, 0, creature_ptr);
	display_flag_aux(row+ 2, col, "無生物ESP :", TRAIT_SENSE_NONLIVING, &f, 0, creature_ptr);
	display_flag_aux(row+ 3, col, "善良ESP   :", TRAIT_SENSE_GOOD, &f, 0, creature_ptr);
	display_flag_aux(row+ 4, col, "不死ESP   :", TRAIT_SENSE_UNDEAD, &f, 0, creature_ptr);
	display_flag_aux(row+ 5, col, "悪魔ESP   :", TRAIT_SENSE_DEMON, &f, 0, creature_ptr);
	display_flag_aux(row+ 6, col, "龍ESP     :", TRAIT_SENSE_DRAGON, &f, 0, creature_ptr);
	display_flag_aux(row+ 7, col, "人間ESP   :", TRAIT_SENSE_HUMAN, &f, 0, creature_ptr);
	display_flag_aux(row+ 8, col, "動物ESP   :", TRAIT_SENSE_ANIMAL, &f, 0, creature_ptr);
	display_flag_aux(row+ 9, col, "オークESP :", TRAIT_SENSE_ORC, &f, 0, creature_ptr);
	display_flag_aux(row+10, col, "トロルESP :", TRAIT_SENSE_TROLL, &f, 0, creature_ptr);
	display_flag_aux(row+11, col, "巨人ESP   :", TRAIT_SENSE_GIANT, &f, 0, creature_ptr);

	display_flag_aux(row+13, col, "腕力維持  :", TRAIT_SUSTAIN_STR, &f, 0, creature_ptr);
	display_flag_aux(row+14, col, "知力維持  :", TRAIT_SUSTAIN_INT, &f, 0, creature_ptr);
	display_flag_aux(row+15, col, "賢さ維持  :", TRAIT_SUSTAIN_WIS, &f, 0, creature_ptr);
	display_flag_aux(row+16, col, "器用維持  :", TRAIT_SUSTAIN_DEX, &f, 0, creature_ptr);
	display_flag_aux(row+17, col, "耐久維持  :", TRAIT_SUSTAIN_CON, &f, 0, creature_ptr);
	display_flag_aux(row+18, col, "魅力維持  :", TRAIT_SUSTAIN_CHR, &f, 0, creature_ptr);
#else
	display_flag_aux(row+ 0, col, "Telepathy :", TRAIT_ESP, &f, 0, creature_ptr);
	display_flag_aux(row+ 1, col, "ESP Evil  :", TRAIT_SENSE_EVIL, &f, 0, creature_ptr);
	display_flag_aux(row+ 2, col, "ESP Noliv.:", TRAIT_SENSE_NONLIVING, &f, 0, creature_ptr);
	display_flag_aux(row+ 3, col, "ESP Good  :", TRAIT_SENSE_GOOD, &f, 0, creature_ptr);
	display_flag_aux(row+ 4, col, "ESP Undead:", TRAIT_SENSE_UNDEAD, &f, 0, creature_ptr);
	display_flag_aux(row+ 5, col, "ESP Demon :", TRAIT_SENSE_DEMON, &f, 0, creature_ptr);
	display_flag_aux(row+ 6, col, "ESP Dragon:", TRAIT_SENSE_DRAGON, &f, 0, creature_ptr);
	display_flag_aux(row+ 7, col, "ESP Human :", TRAIT_SENSE_HUMAN, &f, 0, creature_ptr);
	display_flag_aux(row+ 8, col, "ESP Animal:", TRAIT_SENSE_ANIMAL, &f, 0, creature_ptr);
	display_flag_aux(row+ 9, col, "ESP Orc   :", TRAIT_SENSE_ORC, &f, 0, creature_ptr);
	display_flag_aux(row+10, col, "ESP Troll :", TRAIT_SENSE_TROLL, &f, 0, creature_ptr);
	display_flag_aux(row+11, col, "ESP Giant :", TRAIT_SENSE_GIANT, &f, 0, creature_ptr);

	display_flag_aux(row+13, col, "Sust Str  :", TRAIT_SUSTAIN_STR, &f, 0, creature_ptr);
	display_flag_aux(row+14, col, "Sust Int  :", TRAIT_SUSTAIN_INT, &f, 0, creature_ptr);
	display_flag_aux(row+15, col, "Sust Wis  :", TRAIT_SUSTAIN_WIS, &f, 0, creature_ptr);
	display_flag_aux(row+16, col, "Sust Dex  :", TRAIT_SUSTAIN_DEX, &f, 0, creature_ptr);
	display_flag_aux(row+17, col, "Sust Con  :", TRAIT_SUSTAIN_CON, &f, 0, creature_ptr);
	display_flag_aux(row+18, col, "Sust Chr  :", TRAIT_SUSTAIN_CHR, &f, 0, creature_ptr);
#endif
*/

	/*** Set 3 ***/

	row = 3;
	col = 41;

	display_player_equippy(row-2, col+14, 0, creature_ptr);

	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, 0), row-1, col+14);

/*
#ifdef JP
	display_flag_aux(row+ 0, col, "追加攻撃    :", TRAIT_BLOWS, &f, 0, creature_ptr);
	display_flag_aux(row+ 1, col, "採掘        :", TRAIT_TUNNEL, &f, 0, creature_ptr);
	display_flag_aux(row+ 2, col, "赤外線視力  :", TRAIT_INFRA, &f, 0, creature_ptr);
	display_flag_aux(row+ 3, col, "魔法道具支配:", TRAIT_MAGIC_MASTERY, &f, 0, creature_ptr);
	display_flag_aux(row+ 4, col, "隠密        :", TRAIT_STEALTH, &f, 0, creature_ptr);
	display_flag_aux(row+ 5, col, "探索        :", TRAIT_SEARCH, &f, 0, creature_ptr);

	display_flag_aux(row+ 7, col, "乗馬        :", TRAIT_RIDING, &f, 0, creature_ptr);
	display_flag_aux(row+ 8, col, "投擲        :", TRAIT_THROW_MIGHTY, &f, 0, creature_ptr);
	display_flag_aux(row+ 9, col, "祝福        :", TRAIT_BLESSED_BRAND, &f, 0, creature_ptr);
	display_flag_aux(row+10, col, "反テレポート:", TRAIT_PREVENT_TELEPORT, &f, 0, creature_ptr);
	display_flag_aux(row+11, col, "反魔法      :", TRAIT_ANTI_MAGIC, &f, 0, creature_ptr);
	display_flag_aux(row+12, col, "消費魔力減少:", TRAIT_DEC_MANA, &f, 0, creature_ptr);

	display_flag_aux(row+14, col, "経験値減少  :", TRAIT_DRAIN_EXP, &f, 0, creature_ptr);
	display_flag_aux(row+15, col, "乱テレポート:", TRAIT_PASSIVE_TELEPORT, &f, 0, creature_ptr);
	display_flag_aux(row+16, col, "反感        :", TRAIT_ANTIPATHY, &f, 0, creature_ptr);
	display_flag_aux(row+17, col, "太古の怨念  :", TRAIT_TY_CURSE, &f, 0, creature_ptr);
#else
	display_flag_aux(row+ 0, col, "Add Blows   :", TRAIT_BLOWS, &f, 0, creature_ptr);
	display_flag_aux(row+ 1, col, "Add Tunnel  :", TRAIT_TUNNEL, &f, 0, creature_ptr);
	display_flag_aux(row+ 2, col, "Add Infra   :", TRAIT_INFRA, &f, 0, creature_ptr);
	display_flag_aux(row+ 3, col, "Add Device  :", TRAIT_MAGIC_MASTERY, &f, 0, creature_ptr);
	display_flag_aux(row+ 4, col, "Add Stealth :", TRAIT_STEALTH, &f, 0, creature_ptr);
	display_flag_aux(row+ 5, col, "Add Search  :", TRAIT_SEARCH, &f, 0, creature_ptr);

	display_flag_aux(row+ 7, col, "Riding      :", TRAIT_RIDING, &f, 0, creature_ptr);
	display_flag_aux(row+ 8, col, "Throw       :", TRAIT_THROW_MIGHTY, &f, 0, creature_ptr);
	display_flag_aux(row+ 9, col, "Blessed     :", TRAIT_BLESSED_BRAND, &f, 0, creature_ptr);
	display_flag_aux(row+10, col, "No Teleport :", TRAIT_PREVENT_TELEPORT, &f, 0, creature_ptr);
	display_flag_aux(row+11, col, "Anti Magic  :", TRAIT_ANTI_MAGIC, &f, 0, creature_ptr);
	display_flag_aux(row+12, col, "Econom. Mana:", TRAIT_DEC_MANA, &f, 0, creature_ptr);

	display_flag_aux(row+14, col, "Drain Exp   :", TRAIT_DRAIN_EXP, &f, 0, creature_ptr);
	display_flag_aux(row+15, col, "Rnd.Teleport:", TRAIT_PASSIVE_TELEPORT, &f, 0, creature_ptr);
	display_flag_aux(row+16, col, "Aggravate   :", TRAIT_ANTIPATHY, &f, 0, creature_ptr);
	display_flag_aux(row+17, col, "TY Curse    :", TRAIT_TY_CURSE, &f, 0, creature_ptr);
#endif
*/
}

static void display_creature_flag_info4(creature_type *creature_ptr)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	//TODO player_immunity(f.player_imm, creature_ptr);
	//TODO tim_player_immunity(f.tim_player_imm, creature_ptr);
	known_obj_immunity(f.known_obj_imm, creature_ptr);
	player_vuln_flags(f.player_vuln, creature_ptr);

	/*** Set 1 ***/

	row = 3;
	col = 1;

	display_player_equippy(row-2, col+12, DP_WP, creature_ptr);
	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, DP_WP), row-1, col+12);
/*
#ifdef JP
	display_flag_aux(row+ 0, col, "邪悪 倍打 :", TRAIT_SLAY_EVIL, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 0, col, "邪悪 倍打 :", TRAIT_KILL_EVIL, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 1, col, "善良 倍打 :", TRAIT_SLAY_GOOD, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 1, col, "善良 倍打 :", TRAIT_KILL_GOOD, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 2, col, "不死 倍打 :", TRAIT_SLAY_UNDEAD, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 2, col, "不死 倍打 :", TRAIT_KILL_UNDEAD, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 3, col, "悪魔 倍打 :", TRAIT_SLAY_DEMON, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 3, col, "悪魔 倍打 :", TRAIT_KILL_DEMON, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 4, col, "龍 倍打   :", TRAIT_SLAY_DRAGON, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 4, col, "龍 倍打   :", TRAIT_KILL_DRAGON, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 5, col, "人間 倍打 :", TRAIT_SLAY_HUMAN, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 5, col, "人間 倍打 :", TRAIT_KILL_HUMAN, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 6, col, "動物 倍打 :", TRAIT_SLAY_ANIMAL, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 6, col, "動物 倍打 :", TRAIT_KILL_ANIMAL, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 7, col, "オーク倍打:", TRAIT_SLAY_ORC, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 7, col, "オーク倍打:", TRAIT_KILL_ORC, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 8, col, "トロル倍打:", TRAIT_SLAY_TROLL, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 8, col, "トロル倍打:", TRAIT_KILL_TROLL, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 9, col, "巨人 倍打 :", TRAIT_SLAY_GIANT, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 9, col, "巨人 倍打 :", TRAIT_KILL_GIANT, &f, (DP_WP|DP_IMM), creature_ptr);
#else
	display_flag_aux(row+ 0, col, "Slay Evil :", TRAIT_SLAY_EVIL, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 0, col, "Slay Evil :", TRAIT_KILL_EVIL, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 1, col, "Slay Good :", TRAIT_SLAY_GOOD, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 1, col, "Slay Good :", TRAIT_KILL_GOOD, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 2, col, "Slay Und. :", TRAIT_SLAY_UNDEAD, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 2, col, "Slay Und. :", TRAIT_KILL_UNDEAD, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 3, col, "Slay Demon:", TRAIT_SLAY_DEMON, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 3, col, "Slay Demon:", TRAIT_KILL_DEMON, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 4, col, "Slay Drag.:", TRAIT_SLAY_DRAGON, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 4, col, "Slay Drag.:", TRAIT_KILL_DRAGON, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 5, col, "Slay Human:", TRAIT_SLAY_HUMAN, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 5, col, "Slay Human:", TRAIT_KILL_HUMAN, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 6, col, "Slay Anim.:", TRAIT_SLAY_ANIMAL, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 6, col, "Slay Anim.:", TRAIT_KILL_ANIMAL, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 7, col, "Slay Orc  :", TRAIT_SLAY_ORC, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 7, col, "Slay Orc  :", TRAIT_KILL_ORC, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 8, col, "Slay Troll:", TRAIT_SLAY_TROLL, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 8, col, "Slay Troll:", TRAIT_KILL_TROLL, &f, (DP_WP|DP_IMM), creature_ptr);
	display_flag_aux(row+ 9, col, "Slay Giant:", TRAIT_SLAY_GIANT, &f, DP_WP, creature_ptr);
	display_flag_aux(row+ 9, col, "Slay Giant:", TRAIT_KILL_GIANT, &f, (DP_WP|DP_IMM), creature_ptr);
#endif
*/

	/*** Set 2 ***/

	row = 3;
	col = 25;

	display_player_equippy(row-2, col+12, DP_WP, creature_ptr);
	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, DP_WP), row-1, col+12);

/*
#ifdef JP
	display_flag_aux(row, col, "溶解      :", TRAIT_ACID_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+1, col, "電撃      :", TRAIT_ELEC_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+2, col, "焼棄      :", TRAIT_FIRE_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+3, col, "凍結      :", TRAIT_COLD_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+4, col, "毒殺      :", TRAIT_POIS_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+5, col, "切れ味    :", TRAIT_VORPAL, &f, DP_WP, creature_ptr);
	display_flag_aux(row+6, col, "地震      :", TRAIT_SHATTER, &f, DP_WP, creature_ptr);
	display_flag_aux(row+7, col, "吸血      :", TRAIT_VAMPIRIC_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+8, col, "カオス効果:", TRAIT_CHAOTIC_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+9, col, "理力      :", TRAIT_FORCE_WEAPON, &f, DP_WP, creature_ptr);
#else
	display_flag_aux(row, col, "Acid Brand:", TRAIT_ACID_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+1, col, "Elec Brand:", TRAIT_ELEC_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+2, col, "Fire Brand:", TRAIT_FIRE_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+3, col, "Cold Brand:", TRAIT_COLD_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+4, col, "Poison Brd:", TRAIT_POIS_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+5, col, "Sharpness :", TRAIT_VORPAL, &f, DP_WP, creature_ptr);
	display_flag_aux(row+6, col, "Quake     :", TRAIT_SHATTER, &f, DP_WP, creature_ptr);
	display_flag_aux(row+7, col, "Vampiric  :", TRAIT_VAMPIRIC_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+8, col, "Chaotic   :", TRAIT_CHAOTIC_BRAND, &f, DP_WP, creature_ptr);
	display_flag_aux(row+9, col, "Force Wep.:", TRAIT_FORCE_WEAPON, &f, DP_WP, creature_ptr);
#endif
*/
}

static void display_creature_trait(creature_type *creature_ptr)
{
	int i = 0, n = 0;

	for(i = 0; i < MAX_TRAITS; i++)
	{
		if(has_trait(creature_ptr, i) && trait_info[i].display_priority > 0)
		{
			c_put_str(TERM_WHITE, format("・%s", trait_info[i].title), n + 2, 1);
			if(wizard)
			{
				if(!creature_ptr->timed_trait[i]) c_put_str(TERM_WHITE, "[CONST]", n + 2, 30);
				else c_put_str(TERM_L_GREEN, format("[%5d]", creature_ptr->timed_trait[i]), n + 2, 30);
			}
			n++;
		}
		if(n > 10) break;
	}

	if(n) c_put_str(TERM_YELLOW, format(MES_SYS_TRAITS(creature_ptr)), 1, 1);
	else c_put_str(TERM_YELLOW, MES_SYS_YOUR_NO_TRAIT, 1, 1);
}

static void display_creature_underings(creature_type *creature_ptr)
{
	int i, type_num = 0, total = 0;
	char buf[100];
	char master_name[100];

	creature_desc(master_name, creature_ptr, 0);

	for(i = 0; i < MAX_UNDERLINGS; i++)
	{
		if(!creature_ptr->underling_num[i]) break;
		sprintf(buf, "%-40s x%d", species_name + species_info[creature_ptr->underling_id[i]].name, creature_ptr->underling_num[i]);
		c_put_str(TERM_WHITE, buf, 2 + type_num, 6);
		type_num++;
		total += creature_ptr->underling_num[i];
	}

	if(!type_num) c_put_str(TERM_YELLOW, format(MES_SYS_NO_SERVANT, master_name), 1, 1);
	else c_put_str(TERM_YELLOW, format(MES_SYS_SERVANTS, master_name, type_num, total), 1, 1);
}

/*
 * Special display, part 2b
 *
 * How to print out the modifications and sustains.
 * Positive mods with no sustain will be light green.
 * Positive mods with a sustain will be dark green.
 * Sustains (with no modification) will be a dark green 's'.
 * Negative mods (from a curse) will be red.
 * Huge mods (>9), like from MIC, will be a '*'
 * No mod, no sustain, will be a slate '.'
 */
static void display_player_stat_info(creature_type *creature_ptr)
{
	int i, j, e_adj;
	STAT_ID stat_col, stat;
	int row, col;
	object_type *object_ptr;
	u32b flgs[MAX_TRAITS_FLAG];
	byte a;
	char c;
	char buf[80];
	stat_col = 0;
	row = 2;

	/* Print out the labels for the columns */
#ifdef JP
	c_put_str(TERM_WHITE, "能力", row, stat_col+1);
	c_put_str(TERM_WHITE, "  基本", row, stat_col+7);
	c_put_str(TERM_WHITE, "  限界", row, stat_col+14);
	c_put_str(TERM_WHITE, " 種 職 神 性 装 ", row, stat_col+20);
	c_put_str(TERM_L_GREEN, "合計", row, stat_col+38);
	c_put_str(TERM_YELLOW, "現在", row, stat_col+44);
#else
	c_put_str(TERM_WHITE, "Stat", row, stat_col+1);
	c_put_str(TERM_WHITE, "  Base", row, stat_col+7);
	c_put_str(TERM_WHITE, "Limit", row, stat_col+14);
	c_put_str(TERM_WHITE, "RacClaDivPerMod", row, stat_col+20);
	c_put_str(TERM_L_GREEN, "Actual", row, stat_col+34);
	c_put_str(TERM_YELLOW, "Current", row, stat_col+39);
#endif

	// Display the stats
	for (i = 0; i < STAT_MAX; i++)
	{
		int r_adj, cl_adj, p_adj = 0;

		if(IS_MIMICED(creature_ptr)) r_adj = race_info[creature_ptr->mimic_race_idx].r_adj[i];
		else if(IS_PURE(creature_ptr)) r_adj = race_info[creature_ptr->race_idx1].r_adj[i];
		else r_adj = race_info[creature_ptr->race_idx1].r_s_adj[i] + race_info[creature_ptr->race_idx2].r_s_adj[i];

		for(j = 0; j < max_trait_idx; j++) if(has_trait(creature_ptr, j)) r_adj += trait_info[j].adj[i];

		e_adj = 0; // Calculate equipment adjustment

		for (j = 0; j < INVEN_TOTAL; j++)
		{
			object_ptr = &creature_ptr->inventory[j];
			if(!IS_EQUIPPED(object_ptr)) continue;
			object_flags_known(object_ptr, flgs);
			if(have_flag(flgs, i)) e_adj += object_ptr->pval;
		}

		r_adj -= calc_unreached_race_level_penalty(calc_base_level(creature_ptr) - creature_ptr->lev, i);
		
		if(creature_ptr->stat_cur[i] < creature_ptr->stat_max[i] && has_status(creature_ptr, i))
			c_put_str(TERM_WHITE, stat_names_reduced[i], row + i+1, stat_col+1); // Reduced name of stat
		else
			c_put_str(TERM_WHITE, stat_names[i], row + i+1, stat_col+1);

		// Internal "natural" max value.  Maxes at 18/100 This is useful to see if you are maxed out
		if(has_status(creature_ptr, i))
		{
			cnv_stat(creature_ptr->stat_max[i], buf);
			if(creature_ptr->stat_max[i] == creature_ptr->stat_mod_max_max[i])
				c_put_str(TERM_WHITE, "!", row + i + 1, stat_col + 5);
			c_put_str(TERM_WHITE, buf, row + i+1, stat_col + 13 - strlen(buf));
		}
		else c_put_str(TERM_L_DARK, "------", row + i+1, stat_col + 7);

		if(has_status(creature_ptr, i))
		{
			if(creature_ptr->knowledge & KNOW_HPRATE) cnv_stat(creature_ptr->stat_mod_max_max[i], buf);
			else strcpy(buf, "????");
			c_put_str(TERM_WHITE, buf, row + i+1, stat_col + 20 - strlen(buf));
		}
		else c_put_str(TERM_L_DARK, "------", row + i+1, stat_col + 14);

		/* Race, class, and equipment modifiers */
		if(creature_ptr->race_idx1 != INDEX_NONE && has_status(creature_ptr, i))
		{
			(void)sprintf(buf, "%+3d", r_adj);
			if(r_adj > 0) c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 20);
			else if(r_adj < 0) c_put_str(TERM_L_RED, buf, row + i+1, stat_col + 20);
			else c_put_str(TERM_L_DARK, buf, row + i+1, stat_col + 20);

		}
		else c_put_str(TERM_L_DARK, " --", row + i+1, stat_col + 20);

		if(creature_ptr->class_idx != INDEX_NONE && has_status(creature_ptr, i))
		{
			cl_adj = (int)class_info[creature_ptr->class_idx].c_adj[i];
			if(creature_ptr->cls_bonus) cl_adj += class_info[creature_ptr->class_idx].c_adj_b[i];

			(void)sprintf(buf, "%+3d", cl_adj);
			if(class_info[creature_ptr->class_idx].c_adj[i] > 0) c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 23);
			else if(class_info[creature_ptr->class_idx].c_adj[i] < 0) c_put_str(TERM_L_RED, buf, row + i+1, stat_col + 23);
			else c_put_str(TERM_L_DARK, buf, row + i+1, stat_col + 23);
		}
		else c_put_str(TERM_L_DARK, " --", row + i+1, stat_col + 23);

		//authority bonus
		if((creature_ptr->patron_idx != INDEX_NONE || creature_ptr->dr >= 0) && has_status(creature_ptr, i))
		{
			for(j = 0; j < max_authorities_idx; j++)
				if(HAS_AUTHORITY(creature_ptr, j)) p_adj += authority_info[j].a_adj[i];

			//worshipment bonus
			if(creature_ptr->dr < 0 && creature_ptr->patron_idx != INDEX_NONE && creature_ptr->patron_idx != creature_ptr->species_idx)
			{
				creature_type *patron_ptr = find_unique_instance(creature_ptr->patron_idx);
				if(patron_ptr)
				{
					for(j = 0; j < max_authorities_idx; j++)
						if(HAS_AUTHORITY(patron_ptr, j)) p_adj += authority_info[j].w_adj[i];
				}
			}


			(void)sprintf(buf, "%+3d", p_adj);
			if(p_adj > 0) c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 26);
			else if(p_adj < 0) c_put_str(TERM_L_RED, buf, row + i+1, stat_col + 26);
			else c_put_str(TERM_L_DARK, buf, row + i+1, stat_col + 26);
		}
		else c_put_str(TERM_L_DARK, " --", row + i+1, stat_col + 26);

		if(creature_ptr->chara_idx != INDEX_NONE && has_status(creature_ptr, i))
		{
			(void)sprintf(buf, "%+3d", (int)chara_info[creature_ptr->chara_idx].a_adj[i]);
			if(chara_info[creature_ptr->chara_idx].a_adj[i] > 0) c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 29);
			else if(chara_info[creature_ptr->chara_idx].a_adj[i] < 0) c_put_str(TERM_L_RED, buf, row + i+1, stat_col + 29);
			else c_put_str(TERM_L_DARK, buf, row + i+1, stat_col + 29);
		}
		else c_put_str(TERM_L_DARK, " --", row + i+1, stat_col + 29);

		if(has_status(creature_ptr, i))
		{
			(void)sprintf(buf, "%+3d", (int)e_adj);
			if(e_adj > 0) c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 32);
			else if(e_adj < 0) c_put_str(TERM_L_RED, buf, row + i+1, stat_col + 32);
			else c_put_str(TERM_L_DARK, buf, row + i+1, stat_col + 32);
		}
		else c_put_str(TERM_L_DARK, " --", row + i+1, stat_col + 32);

		if(has_status(creature_ptr, i))
		{
			// Actual maximal modified value
			cnv_stat(creature_ptr->stat_top[i], buf);
			c_put_str(TERM_L_GREEN, buf, row + i+1, stat_col + 36);

			// Only display stat_use if not maximal
			if(creature_ptr->stat_use[i] < creature_ptr->stat_top[i])
			{
				cnv_stat(creature_ptr->stat_use[i], buf);
				c_put_str(TERM_YELLOW, buf, row + i+1, stat_col + 42);
			}
		}
		else
		{
			c_put_str(TERM_L_DARK, "------", row + i+1, stat_col + 36);
		}

	}

	col = stat_col + 49; // Column

	/* Header and Footer */
	c_put_str(TERM_WHITE, get_equipped_flag_label(creature_ptr, 0), row, col);
	c_put_str(TERM_L_GREEN, MES_SYS_MODIFICATION, row - 1, col);

	/* Process equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];
		if(!IS_EQUIPPED(object_ptr)) continue;

		/* Acquire "known" flags */
		object_flags_known(object_ptr, flgs);

		/* Initialize color based of sign of pval. */
		for (stat = 0; stat < STAT_MAX; stat++)
		{
			a = TERM_SLATE;
			c = '.';

			/* Boost */
			if(have_flag(flgs, stat))
			{
				c = '*';

				// Good
				if(object_ptr->pval > 0)
				{
					a = TERM_L_GREEN;
					if(object_ptr->pval < 10) c = '0' + (char)object_ptr->pval;
				}

				if(have_flag(flgs, stat + TRAIT_SUSTAIN_STR))
				{
					/* Dark green for sustained stats */
					a = TERM_GREEN;
				}

				// Bad
				if(object_ptr->pval < 0)
				{
					a = TERM_RED;

					/* Label boost */
					if(object_ptr->pval > -10) c = '0' - (char)object_ptr->pval;
				}
			}

			/* Sustain */
			else if(have_flag(flgs, stat + TRAIT_SUSTAIN_STR))
			{
				/* Dark green "s" */
				a = TERM_GREEN;
				c = 's';
			}

			/* Dump proper character */
			Term_putch(col, row + stat+1, a, c);
		}

		/* Advance */
		col++;
	}

	/* Check stats */
	for (stat = 0; stat < STAT_MAX; stat++)
	{
		a = TERM_SLATE;
		c = '.';

		/* Sustain */
		if(have_flag(flgs, stat + TRAIT_SUSTAIN_STR))
		{
			/* Dark green "s" */
			a = TERM_GREEN;
			c = 's';
		}

		/* Dump */
		Term_putch(col, row + stat+1, a, c);
	}
}


/*
 * Display the character on the screen (various modes)
 * The top one and bottom two lines are left blank.
 */
void display_creature_status(int mode, creature_type *creature_ptr)
{
	int i;

	char	buf1[20], buf2[20], out_val[60];
	char	buf[80];
	char	tmp[128];
	char	tmp2[128];
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	mode = (mode % DISPLAY_CR_STATUS_MAX);

	clear_from(0); // Erase screen

	// Standard
	if(mode == DISPLAY_CR_STATUS_STANDARD)
	{
		// Name, Sex, Race, Class
		if(creature_ptr->chara_idx != INDEX_NONE && creature_ptr->species_idx == SPECIES_STIGMATIC_ONE){ 
#ifdef JP
		if(!strcmp(creature_ptr->name, "烙印者"))
			sprintf(tmp, "%s%s〈%s〉", chara_info[creature_ptr->chara_idx].title, chara_info[creature_ptr->chara_idx].no == 1 ? "の":"", creature_ptr->name);
		else
			sprintf(tmp, "%s%s『%s』", chara_info[creature_ptr->chara_idx].title, chara_info[creature_ptr->chara_idx].no == 1 ? "の":"", creature_ptr->name);
#else
			sprintf(tmp, "%s %s", chara_info[creature_ptr->chara_idx].title, creature_ptr->name);
#endif
		}
		else sprintf(tmp, "%s", creature_ptr->name);

		if(wizard)
			sprintf(tmp, "[id:%d/f:%d/x:%d/y:%d/turn:%d/wx:%d/wy:%d]%s", creature_ptr->creature_idx, creature_ptr->floor_idx, creature_ptr->fx, creature_ptr->fy, creature_ptr->action_turn, creature_ptr->wx, creature_ptr->wy, creature_ptr->name);

		display_player_one_line(ENTRY_NAME, tmp, TERM_L_BLUE);

		if(creature_ptr->race_idx1 != INDEX_NONE) display_player_one_line(ENTRY_RACE, desc_race_name(creature_ptr, CD_SEX | CD_PURE_RACE), TERM_L_BLUE);
		else display_player_one_line(ENTRY_RACE, "--------", TERM_L_DARK);

		if(creature_ptr->class_idx != INDEX_NONE) display_player_one_line(ENTRY_CLASS, desc_class_name(creature_ptr, CD_SUB_CLASS | CD_REALM), TERM_L_BLUE);
		else display_player_one_line(ENTRY_CLASS, "--------", TERM_L_DARK);

		if(creature_ptr->patron_idx == INDEX_NONE)
			display_player_one_line(ENTRY_PATRON, "------", TERM_L_DARK);
		else
			display_player_one_line(ENTRY_PATRON, species_name + species_info[creature_ptr->patron_idx].name, TERM_L_BLUE);
		

		authority_desc(buf, creature_ptr);
		
		if(buf[0] == '\0') display_player_one_line(ENTRY_AUTH, "------", TERM_L_DARK);
		else display_player_one_line(ENTRY_AUTH, buf, TERM_L_BLUE);


		// Age, Height, Weight, Social
		// D'angband(mertle scale).
#ifdef JP

		if(creature_ptr->race_idx1 != INDEX_NONE)
			display_player_one_line(ENTRY_AGE, format("%u" ,creature_ptr->age), TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_AGE, "--------", TERM_L_DARK);

		if(creature_ptr->race_idx1 != INDEX_NONE)
			display_player_one_line(ENTRY_SOCIAL, format("%d" ,(int)creature_ptr->sc), TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_SOCIAL, "---", TERM_L_DARK);

		// Dump character level
		display_player_one_line(ENTRY_LEVEL, format("%d/%d", creature_ptr->lev, creature_ptr->max_lev), TERM_L_GREEN);

		// Dump hit point
		if(creature_ptr->chp >= creature_ptr->mhp) 
			display_player_one_line(ENTRY_HP, format("%6d/%6d", creature_ptr->chp , creature_ptr->mhp), TERM_L_GREEN);
		else if(creature_ptr->chp > (creature_ptr->mhp * hitpoint_warn) / 10) 
			display_player_one_line(ENTRY_HP, format("%6d/%6d", creature_ptr->chp , creature_ptr->mhp), TERM_YELLOW);
		else
			display_player_one_line(ENTRY_HP, format("%6d/%6d", creature_ptr->chp , creature_ptr->mhp), TERM_RED);

		// Dump mana power
		if(creature_ptr->csp >= creature_ptr->msp) 
			display_player_one_line(ENTRY_SP, format("%4d/%4d", creature_ptr->csp , creature_ptr->msp), TERM_L_GREEN);
		else if(creature_ptr->csp > (creature_ptr->msp * mana_warn) / 10) 
			display_player_one_line(ENTRY_SP, format("%4d/%4d", creature_ptr->csp , creature_ptr->msp), TERM_YELLOW);
		else
			display_player_one_line(ENTRY_SP, format("%4d/%4d", creature_ptr->csp , creature_ptr->msp), TERM_RED);


		if(creature_ptr->dr < 0)
			display_player_one_line(ENTRY_DIVINE_RANK, format("--[なし]"), TERM_L_DARK);
		else if(creature_ptr->dr >= 26)
			display_player_one_line(ENTRY_DIVINE_RANK, format("%2d[究極神]", creature_ptr->dr), TERM_WHITE);
		else if(creature_ptr->dr >= 21)
			display_player_one_line(ENTRY_DIVINE_RANK, format("%2d[超越神]", creature_ptr->dr), TERM_VIOLET);
		else if(creature_ptr->dr >= 16)
			display_player_one_line(ENTRY_DIVINE_RANK, format("%2d[上級神]", creature_ptr->dr), TERM_L_BLUE);
		else if(creature_ptr->dr >= 11)
			display_player_one_line(ENTRY_DIVINE_RANK, format("%2d[中級神]", creature_ptr->dr), TERM_L_BLUE);
		else if(creature_ptr->dr >= 6)
			display_player_one_line(ENTRY_DIVINE_RANK, format("%2d[下級神]", creature_ptr->dr), TERM_L_BLUE);
		else if(creature_ptr->dr >= 1)
			display_player_one_line(ENTRY_DIVINE_RANK, format("%2d[半神]", creature_ptr->dr), TERM_YELLOW);
		else
			display_player_one_line(ENTRY_DIVINE_RANK, format("%2d[英霊]", creature_ptr->dr), TERM_RED);

#else

		if(creature_ptr->race_idx1 != INDEX_NONE)
			display_player_one_line(ENTRY_AGE, format("%d" ,(unsigned int)creature_ptr->age), TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_AGE, "--------", TERM_L_DARK);

		display_player_one_line(ENTRY_HEIGHT, format("%d" ,(int)((creature_ptr->ht*100)/254)), TERM_L_BLUE);
		display_player_one_line(ENTRY_WEIGHT, format("%d" ,(int)((creature_ptr->wt*10000)/4536)), TERM_L_BLUE);

		if(creature_ptr->race_idx1 != INDEX_NONE){ 
			display_player_one_line(ENTRY_SOCIAL, format("%d" ,(int)creature_ptr->sc), TERM_L_BLUE);
		}
		else{
			display_player_one_line(ENTRY_SOCIAL, "---", TERM_L_DARK);
		}

		if(creature_ptr->dr < 0)
			display_player_one_line(ENTRY_DIVINE_RANK, format("--[None]"), TERM_L_DARK);
		else if(creature_ptr->dr >= 26)
			display_player_one_line(ENTRY_DIVINE_RANK, format("[%2d]Ultima", creature_ptr->dr), TERM_L_BLUE);
		else if(creature_ptr->dr >= 21)
			display_player_one_line(ENTRY_DIVINE_RANK, format("[%2d]Over", creature_ptr->dr), TERM_L_BLUE);
		else if(creature_ptr->dr >= 16)
			display_player_one_line(ENTRY_DIVINE_RANK, format("[%2d]Elder", creature_ptr->dr), TERM_L_BLUE);
		else if(creature_ptr->dr >= 11)
			display_player_one_line(ENTRY_DIVINE_RANK, format("[%2d]Middle", creature_ptr->dr), TERM_L_BLUE);
		else if(creature_ptr->dr >= 6)
			display_player_one_line(ENTRY_DIVINE_RANK, format("[%2d]Lesser", creature_ptr->dr), TERM_L_BLUE);
		else if(creature_ptr->dr >= 1)
			display_player_one_line(ENTRY_DIVINE_RANK, format("[%2d]Demi", creature_ptr->dr), TERM_YELLOW);
		else
			display_player_one_line(ENTRY_DIVINE_RANK, format("[%2d]Hero", creature_ptr->dr), TERM_RED);

#endif

		display_player_one_line(ENTRY_GOOD, format("%3d" ,creature_ptr->good_rank), TERM_YELLOW);
		display_player_one_line(ENTRY_EVIL, format("%3d" ,creature_ptr->evil_rank), TERM_RED);
		display_player_one_line(ENTRY_ORDER, format("%3d" ,creature_ptr->order_rank), TERM_L_BLUE);
		display_player_one_line(ENTRY_CHAOS, format("%3d" ,creature_ptr->chaos_rank), TERM_VIOLET);
		display_player_one_line(ENTRY_BALANCE,format("%3d" ,creature_ptr->balance_rank), TERM_L_GREEN);

		format_height(tmp, creature_ptr->ht);
		format_weight(tmp2, creature_ptr->wt);
		display_player_one_line(ENTRY_SIZE, format("%d(%s/%s)", creature_ptr->size, tmp, tmp2), TERM_L_BLUE);

		for (i = 0; i < STAT_MAX; i++) /* Display the stats */
		{
			if(!has_status(creature_ptr, i))
			{
				put_str(stat_names[i], 3 + i, 58); // Assume uppercase stat name
				c_put_str(TERM_L_DARK, "------", 3 + i, 63); // Display the current stat (void)
			}
			else if(creature_ptr->stat_cur[i] < creature_ptr->stat_max[i]) // Special treatment of "injured" stats
			{
				int value;				
				put_str(stat_names_reduced[i], 3 + i, 58); // Use lowercase stat name
				value = creature_ptr->stat_use[i]; // Get the current stat
				cnv_stat(value, buf); // Obtain the current stat (modified)
				c_put_str(TERM_YELLOW, buf, 3 + i, 63); // Display the current stat (modified)
				value = creature_ptr->stat_top[i]; // Acquire the max stat
				cnv_stat(value, buf); // Obtain the maximum stat (modified)
				c_put_str(TERM_L_GREEN, buf, 3 + i, 70); // Display the maximum stat (modified)
			}
			else // Normal treatment of "normal" stats
			{
				put_str(stat_names[i], 3 + i, 58); // Assume uppercase stat name
				cnv_stat(creature_ptr->stat_use[i], buf); // Obtain the current stat (modified)
				c_put_str(TERM_L_GREEN, buf, 3 + i, 63); // Display the current stat (modified)
			}

			if(creature_ptr->stat_max[i] == creature_ptr->stat_mod_max_max[i])
			{
#ifdef JP
				c_put_str(TERM_WHITE, "!", 3 + i, 62);
#else
				c_put_str(TERM_WHITE, "!", 3 + i, 60);
#endif
			}
		}

		/* Display "various" info */
		display_player_middle(creature_ptr);
		display_player_various(creature_ptr);

	}

	/* Display "history" info */
	else if(mode == DISPLAY_CR_STATUS_HISTORY)
	{
		char statmsg[10000];

		put_str(format("(%s)", get_keyword("KW_CREATURE_PROFILE")), 2, 25);

		for (i = 0; i < HISTORY_ROW; i++) put_str(creature_ptr->history[i], i + 4, 3);

		*statmsg = '\0';

		if(gameover)
		{
			if(creature_ptr->total_winner)
			{
#ifdef JP
				sprintf(statmsg, "…あなたは勝利の後%sした。", streq(gameover_from, COD_SEPPUKU) ? COD_SEPPUKU : COD_RETIRE);
#else
				sprintf(statmsg, "...You %s after the winning.", streq(gameover_from, COD_SEPPUKU) ? "did Seppuku" : "retired from the adventure");
#endif
			}
			else if(!floor_ptr->depth)
			{
#ifdef JP
				sprintf(statmsg, "…あなたは%sで%sに殺された。", get_floor_name(floor_ptr), gameover_from);
#else
				sprintf(statmsg, "...You were killed by %s in %s.", gameover_from, get_floor_name(floor_ptr));
#endif
			}
			else if(floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest))
			{
				// Get the quest text
				// Bewere that INIT_ASSIGN resets the cur_num.
				process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_ASSIGN, floor_ptr->quest);

#ifdef JP
				sprintf(statmsg, "…あなたは、クエスト「%s」で%sに殺された。", quest[floor_ptr->quest].name, gameover_from);
#else
				sprintf(statmsg, "...You were killed by %s in the quest '%s'.", gameover_from, quest[floor_ptr->quest].name);
#endif
			}
			else
			{
#ifdef JP
				sprintf(statmsg, "…あなたは、%sの%d階で%sに殺された。", get_floor_name(floor_ptr), floor_ptr->depth, gameover_from);
#else
				sprintf(statmsg, "...You were killed by %s on level %d of %s.", gameover_from, floor_ptr->depth, get_floor_name(floor_ptr));
#endif
			}
		}
		else
		{
			if(!floor_ptr->depth)
			{
#ifdef JP
				sprintf(statmsg, "…あなたは現在、 %s にいる。", get_floor_name(floor_ptr));
#else
				sprintf(statmsg, "...Now, you are in %s.", get_floor_name(floor_ptr));
#endif
			}
			else if(floor_ptr->quest && is_fixed_quest_idx(floor_ptr->quest))
			{
				/* Clear the text */
				/* Must be done before doing INIT_SHOW_TEXT */
				for (i = 0; i < 10; i++) questp_text[i][0] = '\0';
				questp_text_line = 0;

				// Get the quest text
				process_dungeon_file(NULL, QUEST_INFO_FILE, 0, 0, 0, 0, INIT_SHOW_TEXT, floor_ptr->quest);

#ifdef JP
				sprintf(statmsg, "…あなたは現在、 クエスト「%s」を遂行中だ。", quest[floor_ptr->quest].name);
#else
				sprintf(statmsg, "...Now, you are in the quest '%s'.", quest[floor_ptr->quest].name);
#endif
			}
			else
			{
#ifdef JP
				sprintf(statmsg, "…あなたは現在、 %s の %d 階で探索している。", get_floor_name(floor_ptr), floor_ptr->depth);
#else
				sprintf(statmsg, "...Now, you are exploring level %d of %s.", floor_ptr->depth, get_floor_name(floor_ptr));
#endif
			}
		}

		if(*statmsg)
		{
			char temp[75*2], *t;
			roff_to_buf(statmsg, 75, temp, sizeof(temp));
			t = temp;
			for(i=0 ; i<2 ; i++)
			{
				if(t[0]==0)
					break; 
				else
				{
					put_str(t, i + HISTORY_ROW + 5, 3);
					t += strlen(t)+1;
				}
			}
		}
	}

	else if(mode == DISPLAY_CR_STATUS_INVENTORY)
	{
		(void)show_item_list(0, creature_ptr, SHOW_ITEM_INVENTORY, NULL);
		format_weight(buf1, creature_ptr->carrying_weight);
		format_weight(buf2, calc_carrying_weight_limit(creature_ptr));

#ifdef JP
		sprintf(out_val, "%s: %s/%s (%ld%%)", buf1, buf2, get_keyword("KW_CARRYING_WEIGHT"),
		    (creature_ptr->carrying_weight * 100) / calc_carrying_weight_limit(creature_ptr));
#else
		sprintf(out_val, "%s: %s/%s (%ld%%). Command: ", buf1, buf2, get_keyword("KW_CARRYING_WEIGHT"),
		    (creature_ptr->carrying_weight * 100) / calc_carrying_weight_limit(creature_ptr));
#endif

		prt(out_val, 0, 0);
	}

	else if(mode == DISPLAY_CR_STATUS_EQUIPMENT)
	{
		(void)show_item_list(0, creature_ptr, SHOW_ITEM_EQUIPMENT, NULL);		
		format_weight(buf1, creature_ptr->equipping_weight);
		format_weight(buf2, calc_equipping_weight_limit(creature_ptr));
		sprintf(out_val, "%s: %s/%s (%ld%%)", get_keyword("KW_EQUIPMENT_WEIGHT"), buf1, buf2,
		    (creature_ptr->equipping_weight * 100) / calc_equipping_weight_limit(creature_ptr));
		prt(out_val, 0, 0);
	}

	else if(mode == DISPLAY_CR_STATUS_VARIOUS1)
	{
		display_player_stat_info(creature_ptr);
		display_creature_flag_info1(creature_ptr);
	}

	else if(mode == DISPLAY_CR_STATUS_VARIOUS2) display_creature_flag_info2(creature_ptr);
	else if(mode == DISPLAY_CR_STATUS_VARIOUS3) display_creature_flag_info3(creature_ptr);
	else if(mode == DISPLAY_CR_STATUS_VARIOUS4) display_creature_flag_info4(creature_ptr);
	else if(mode == DISPLAY_CR_STATUS_MUTATION) display_creature_trait(creature_ptr);
	else if(mode == DISPLAY_CR_STATUS_UNDERINGS) display_creature_underings(creature_ptr);

}


/*
 *
 */
static void dump_aux_display_creature_status(creature_type *creature_ptr, FILE *fff)
{
	int x, y;
	byte a;
	char c;
	char buf[1024];

	/* Display player */
	display_creature_status(0, creature_ptr);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));
			buf[x] = c;
		}
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		fprintf(fff, "%s\n", buf);

	}

	/* Display history */
	display_creature_status(1, creature_ptr);

	/* Dump part of the screen */
	for (y = 10; y < 19; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");

	/* Display flags (part 1) */
	display_creature_status(2, creature_ptr);

	/* Dump part of the screen */
	for (y = 2; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if(a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");

	/* Display flags (part 2) */
	display_creature_status(3, creature_ptr);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if(a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	/* Display flags (part 3) */
	display_creature_status(4, creature_ptr);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if(a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	/* Display flags (part 4) */
	display_creature_status(5, creature_ptr);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if(a < 128) buf[x] = c;
			else buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");
}


/*
 *
 */
static void dump_aux_pet(creature_type *creature_ptr, FILE *fff)
{
	int i;
	bool pet = FALSE;
	bool pet_settings = FALSE;
	char pet_name[80];

	for (i = creature_max - 1; i >= 1; i--)
	{
		creature_type *m_ptr = &creature_list[i];

		if(!is_valid_creature(m_ptr)) continue;
		if(!is_pet(player_ptr, m_ptr)) continue;
		pet_settings = TRUE;
		if(!m_ptr->nickname && (creature_ptr->riding != i)) continue;
		if(!pet)
		{
			fprintf(fff, "\n\n  %s\n\n", MES_INFO_LEADING_SERVANT);
			pet = TRUE;
		}
		creature_desc(pet_name, m_ptr, CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);
		fprintf(fff, "%s\n", pet_name);
	}

	if(pet_settings)
	{
#ifdef JP
		fprintf(fff, "\n\n  [ペットへの命令]\n");
		fprintf(fff, "\n ドアを開ける:                       %s", (creature_ptr->pet_extra_flags & PF_OPEN_DOORS) ? "ON" : "OFF");
		fprintf(fff, "\n アイテムを拾う:                     %s", (creature_ptr->pet_extra_flags & PF_PICKUP_ITEMS) ? "ON" : "OFF");
		fprintf(fff, "\n テレポート系魔法を使う:             %s", (creature_ptr->pet_extra_flags & PF_TELEPORT) ? "ON" : "OFF");
		fprintf(fff, "\n 攻撃魔法を使う:                     %s", (creature_ptr->pet_extra_flags & PF_ATTACK_SPELL) ? "ON" : "OFF");
		fprintf(fff, "\n 召喚魔法を使う:                     %s", (creature_ptr->pet_extra_flags & PF_TRAIT_S_SPELL) ? "ON" : "OFF");
		fprintf(fff, "\n 主を巻き込む範囲魔法を使う: %s", (creature_ptr->pet_extra_flags & PF_BALL_SPELL) ? "ON" : "OFF");
#else
		fprintf(fff, "\n\n  [Command for Pets]\n");
		fprintf(fff, "\n Pets open doors:                    %s", (creature_ptr->pet_extra_flags & PF_OPEN_DOORS) ? "ON" : "OFF");
		fprintf(fff, "\n Pets pick up items:                 %s", (creature_ptr->pet_extra_flags & PF_PICKUP_ITEMS) ? "ON" : "OFF");
		fprintf(fff, "\n Allow teleport:                     %s", (creature_ptr->pet_extra_flags & PF_TELEPORT) ? "ON" : "OFF");
		fprintf(fff, "\n Allow cast attack spell:            %s", (creature_ptr->pet_extra_flags & PF_ATTACK_SPELL) ? "ON" : "OFF");
		fprintf(fff, "\n Allow cast summon spell:            %s", (creature_ptr->pet_extra_flags & PF_TRAIT_S_SPELL) ? "ON" : "OFF");
		fprintf(fff, "\n Allow involve master in area spell: %s", (creature_ptr->pet_extra_flags & PF_BALL_SPELL) ? "ON" : "OFF");
#endif
		fputc('\n', fff);
	}
}
static void dump_aux_class_special(creature_type *creature_ptr, FILE *fff)
{
	char p[60][80];
	int col = 0;

	if(creature_ptr->class_idx == CLASS_BLUE_MAGE)
	{
		strcat(p[col], MES_INTERFACE_LEARNED_BLUEMAGIC);
	}

	if(creature_ptr->class_idx == CLASS_MAGIC_EATER)
	{
		fprintf(fff, MES_INTERFACE_EATEN_MAGIC);
	}
}


/*
 *
 */
static void dump_aux_quest(FILE *fff)
{
	int i;
	int *quest_num;
	int dummy;

	fprintf(fff, MES_INTERFACE_QUEST_INFO);

	C_MAKE(quest_num, max_quests, int); /* Allocate Memory */

	/* Sort by compete level */
	for (i = 1; i < max_quests; i++) quest_num[i] = i;
	ang_sort(quest_num, &dummy, max_quests, ang_sort_comp_quest_num, ang_sort_swap_quest_num);

	/* Dump Quest Information */
	fputc('\n', fff);
	do_cmd_knowledge_quests_completed(fff, quest_num);
	fputc('\n', fff);
	do_cmd_knowledge_quests_failed(fff, quest_num);
	fputc('\n', fff);

	/* Free Memory */
	C_KILL(quest_num, max_quests, int);
}

static void dump_aux_last_message(creature_type *creature_ptr, FILE *fff)
{
	if(gameover)
	{
		if(!creature_ptr->total_winner)
		{
			int i;
			fprintf(fff, MES_INTERFACE_LAST_MESSAGE);
			for (i = MIN(message_num(), 30); i >= 0; i--) fprintf(fff,"> %s\n",message_str((s16b)i));
			fputc('\n', fff);
		}

		/* Hack -- *Winning* message */
		else if(creature_ptr->last_message)
		{
			fprintf(fff, MES_INTERFACE_WINNING_MESSAGE);
			fprintf(fff,"  %s\n", creature_ptr->last_message);
			fputc('\n', fff);
		}
	}
}

static void dump_aux_recall(FILE *fff)
{
	int y;

	fprintf(fff, MES_INTERFACE_RECALL_DEPTH);

	for (y = 1; y < max_dungeon_idx; y++)
	{
		bool seiha = FALSE;

		if(!dungeon_info[y].maxdepth) continue;
		if(!max_dlv[y]) continue;
		if(dungeon_info[y].final_guardian) if(!species_info[dungeon_info[y].final_guardian].max_num) seiha = TRUE;
		else if(max_dlv[y] == dungeon_info[y].maxdepth) seiha = TRUE;
		fprintf(fff, MES_INTERFACE_RECALL_DEPTH_LIST(seiha, dungeon_name + dungeon_info[y].name, max_dlv[y]));
	}
}

static void dump_aux_options(FILE *fff)
{

	fprintf(fff, "\n  %s\n", MES_INFO_LEADING_OPTIONS);
#ifdef JP
	if(ironman_small_levels) fprintf(fff, "\n 小さいダンジョン:   ALWAYS");
	else if(always_small_levels) fprintf(fff, "\n 小さいダンジョン:   ON");
	else fprintf(fff, "\n 小さいダンジョン:   OFF");

	if(ironman_shops) fprintf(fff, "\n 店なし:             ON");
	if(ironman_downward) fprintf(fff, "\n 階段を上がれない:   ON");
	if(ironman_rooms) fprintf(fff, "\n 普通でない部屋:     ON");

	if(ironman_empty_levels) fprintf(fff, "\n アリーナ:           ALWAYS");
	else if(empty_levels) fprintf(fff, "\n アリーナ:           ENABLED");
	else fprintf(fff, "\n アリーナ:           OFF");

	fputc('\n', fff);

	if(noscore) fprintf(fff, "\n 何か不正なことをしてしまっています。\n");

#else

	if(ironman_small_levels) fprintf(fff, "\n Small Levels:       ALWAYS");
	else if(always_small_levels) fprintf(fff, "\n Small Levels:       ON");
	else fprintf(fff, "\n Small Levels:       OFF");

	if(ironman_shops) fprintf(fff, "\n No Shops:           ON");
	if(ironman_downward) fprintf(fff, "\n Diving Only:        ON");
	if(ironman_rooms) fprintf(fff, "\n Unusual Rooms:      ON");
	if(ironman_empty_levels) fprintf(fff, "\n Arena Levels:       ALWAYS");
	else if(empty_levels) fprintf(fff, "\n Arena Levels:       ENABLED");
	else fprintf(fff, "\n Arena Levels:       OFF");

	fputc('\n', fff);
	if(noscore) fprintf(fff, "\n You have done something illegal.\n");
#endif

	fputc('\n', fff);
}

static void dump_aux_arena(FILE *fff)
{

#ifdef JP
	if(arena_number < 0) fprintf(fff, "\n 闘技場: %d回戦で%sの前に敗北\n", -arena_number,
			species_name + species_info[arena_info[-1 - arena_number].species_idx].name);
	else if(arena_number > MAX_ARENA_MONS + 2) fprintf(fff, "\n 闘技場: 真のチャンピオン\n");
	else if(arena_number > MAX_ARENA_MONS - 1) fprintf(fff, "\n 闘技場: チャンピオン\n");
	else fprintf(fff, "\n 闘技場: %2d勝\n", (arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : arena_number));
#else
	if(arena_number < 0) fprintf(fff, "\n Arena: Defeated by %s in the %d%s fight\n",
			species_name + species_info[arena_info[-1 - arena_number].species_idx].name,
			-arena_number, get_ordinal_number_suffix(-arena_number));
	else if(arena_number > MAX_ARENA_MONS + 2) fprintf(fff, "\n Arena: True Champion\n");
	else if(arena_number > MAX_ARENA_MONS - 1) fprintf(fff, "\n Arena: Champion\n");
	else fprintf(fff, "\n Arena: %2d Victor%s\n", (arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : arena_number), (arena_number > 1) ? "ies" : "y");
#endif

	fprintf(fff, "\n");
}


static void dump_aux_creatures(FILE *fff)
{
	SPECIES_ID k; /* Creatures slain */
	long uniq_total = 0;
	long norm_total = 0;
	SPECIES_ID *who;

	u16b why = 2; /* Sort by creature level */
	fprintf(fff, MES_INTERFACE_DEFEATED_CREATURE);

	C_MAKE(who, max_species_idx, SPECIES_ID); /* Allocate the "who" array */

	for (k = 1; k < max_species_idx; k++) /* Count creature kills */
	{
		species_type *species_ptr = &species_info[k];
		if(!species_ptr->name) continue; /* Ignore unused index */
		if(has_trait_species(species_ptr, TRAIT_UNIQUE)) /* Unique creatures */
		{
			bool dead = (species_ptr->max_num == 0);
			if(dead)
			{
				norm_total++;
				who[uniq_total++] = k; /* Add a unique creature to the list */
			}
		}
		else if(species_ptr->killed_by_player > 0) norm_total += species_ptr->killed_by_player; /* Normal creatures */
	}

	if(norm_total < 1) fprintf(fff, MES_KNOW_NO_KILLED); /* No creatures is defeated */
	else if(uniq_total == 0) fprintf(fff, MES_KNOW_KILLED(norm_total)); /* Defeated more than one normal creatures */
	else /* Defeated more than one unique creatures */
	{
		fprintf(fff, MES_INTERFACE_DEFEATED_SCORE(uniq_total, norm_total)); 
		ang_sort(who, &why, uniq_total, ang_sort_comp_hook, ang_sort_swap_hook); /* Sort the array by dungeon depth of creatures */
		fprintf(fff, MES_INTERFACE_DEFEATED_UNIQUE_TOP(MIN(uniq_total, 10)));

		for (k = uniq_total - 1; k >= 0 && k >= uniq_total - 10; k--)
		{
			species_type *species_ptr = &species_info[who[k]]; /* Print top 10 */
			fprintf(fff, "  %-40s (%s%3d)\n", (species_name + species_ptr->name), get_keyword("KW_LEVEL"), species_ptr->level); 
		}

	}
	C_KILL(who, max_species_idx, SPECIES_ID); /* Free the "who" array */
}

static void dump_aux_race_history(FILE *fff)
{
	fprintf(fff, MES_INTERFACE_FIRST_RACE(&player_prev));
	fputc('\n', fff);
}

static void dump_aux_realm_history(creature_type *creature_ptr, FILE *fff)
{
	if(creature_ptr->old_realm)
	{
		int i;

		fputc('\n', fff);
		for (i = 0; i < MAX_REALMS; i++)
		{
			if(!(creature_ptr->old_realm & 1L << i)) continue;
#ifdef JP
			fprintf(fff, "\n あなたはかつて%s魔法を使えた。", realm_names[i+1]);
#else
			fprintf(fff, "\n You were able to use %s magic before.", realm_names[i+1]);
#endif
		}
		fputc('\n', fff);
	}
}


/*
 *
 */
static void dump_aux_karmas(creature_type *creature_ptr, FILE *fff)
{
	char buf[100];
	show_alignment(buf, creature_ptr);

	fprintf(fff, "\n\n  [%s]\n\n", get_keyword("KW_KARMA"));
	fprintf(fff, "%s: %s\n", get_keyword("KW_ALIGNMENT"), buf);
	fprintf(fff, "\n");
	dump_karmas(creature_ptr, fff);
}

static void dump_aux_mutations(creature_type *creature_ptr, FILE *fff)
{
	fprintf(fff, MES_INTERFACE_MUTATION);
	dump_traits(creature_ptr, fff);
}

static void dump_aux_equipment_inventory(creature_type *creature_ptr, FILE *fff)
{
	int i;
	char object_name[MAX_NLEN];

	/* Dump the equipment */
	if(creature_ptr->equip_cnt)
	{
		fprintf(fff, "  [%s]\n\n", get_keyword("KW_CHARA_EQUIPMENT"));
		for (i = 0; i < INVEN_TOTAL; i++)
		{
			if(!IS_EQUIPPED(&creature_ptr->inventory[i])) continue;

			object_desc(object_name, &creature_ptr->inventory[i], 0);
			if((((i == get_equipped_slot_idx(creature_ptr, SLOT_ID_HAND, 0)) && creature_ptr->can_melee[0]) || ((i == get_equipped_slot_idx(creature_ptr, SLOT_ID_HAND, 2)) && creature_ptr->can_melee[0])) && creature_ptr->two_handed)
				strcpy(object_name, get_keyword("KW_WITH_THOHANDED"));

			fprintf(fff, "%c) %s\n", index_to_label(i), object_name);
		}
		fprintf(fff, "\n\n");
	}

	/* Dump the inventory */
	fprintf(fff, "  [%s]\n\n", get_keyword("KW_CHARA_INVENTORY"));

	for (i = 0; i < INVEN_TOTAL; i++)
	{
		/* Don't dump the empty slots */
		if(!creature_ptr->inventory[i].k_idx) break;

		/* Dump the inventory slots */
		object_desc(object_name, &creature_ptr->inventory[i], 0);
		fprintf(fff, "%c) %s\n", index_to_label(i), object_name);
	}

	/* Add an empty line */
	fprintf(fff, "\n\n");
}


/*
 *
 */
static void dump_aux_home_museum(FILE *fff)
{
	if(!fff) return;
//	char object_name[MAX_NLEN];
//	store_type  *st_ptr;

	/* Do we need it?? */
	/* process_dungeon_file(WORLD_INFO_FILE, 0, 0, max_wild_y, max_wild_x); */

	/* Print the home */
	//TODO
	
	/*
	st_ptr = &town[1].store[STORE_HOME];

	// Home -- if anything there
	if(st_ptr->stock_num)
	{
		int i;
		int x = 1;

		fprintf(fff, "  [%s]\n", MES_INFO_HOME);

		// Dump all available items
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			if((i % 12) == 0){
				fprintf(fff, "\n(");
				fprintf(fff, MES_STORE_PAGE(NUM));
				fprintf(fff, ")\n");
			}
			object_desc(object_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), object_name);
		}

		// Add an empty line
		fprintf(fff, "\n\n");
	}


	// Print the home
	st_ptr = &town[1].store[STORE_MUSEUM];

	// Home -- if anything there
	if(st_ptr->stock_num)
	{
		int i;
		int x = 1;
		fprintf(fff, "  [%s]\n", get_keyword("KW_MUSIUM_LIST);

		// Dump all available items
		for (i = 0; i < st_ptr->stock_num; i++)
		{
#ifdef JP
		if((i % 12) == 0) fprintf(fff, "\n ( %d ページ )\n", x++);
			object_desc(object_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), object_name);
#else
		if((i % 12) == 0) fprintf(fff, "\n ( page %d )\n", x++);
			object_desc(object_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c) %s\n", I2A(i%12), object_name);
#endif

		}

		// Add an empty line
		fprintf(fff, "\n\n");
	}
	*/
}


/*
 * Output the character dump to a file
 */
errr make_character_dump(creature_type *creature_ptr, FILE *fff)
{
	fprintf(fff, MES_INTERFACE_DUMP_VERSION(VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH));

	update_play_time();

	dump_aux_display_creature_status(creature_ptr, fff);
	dump_aux_last_message(creature_ptr, fff);
	dump_aux_options(fff);
	dump_aux_recall(fff);
	dump_aux_quest(fff);
	dump_aux_arena(fff);
	dump_aux_creatures(fff);
	dump_aux_karmas(creature_ptr, fff);
	dump_aux_race_history(fff);
	dump_aux_realm_history(creature_ptr, fff);
	dump_aux_class_special(creature_ptr, fff);
	dump_aux_mutations(creature_ptr, fff);
	dump_aux_pet(creature_ptr, fff);
	fputs("\n\n", fff);
	dump_aux_equipment_inventory(creature_ptr, fff);
	dump_aux_home_museum(fff);
	fprintf(fff, MES_INTERFACE_CHECKSUM(get_check_sum()));

	return SUCCESS;
}

/*
 * Hack -- Dump a character description file
 *
 *  Allow the "full" flag to dump additional info,
 * and trigger its usage from various places in the code.
 */
errr file_character(cptr name)
{
	int fd = -1;
	FILE *fff = NULL;
	char buf[1024];

	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	if(fd >= 0) /* Existing file */
	{
		char out_val[160];
		(void)fd_close(fd);
		(void)sprintf(out_val, MES_SYS_REPLACE_FILE, buf); /* Build query */
		if(get_check_strict(out_val, CHECK_NO_HISTORY)) fd = -1;
	}

	if(fd < 0) fff = my_fopen(buf, "w"); /* Open the non-existing file */
	if(!fff) /* Invalid file */
	{
		prt(format(MES_FILE_SAVED_FAIL(buf)), 0, 0);
		(void)inkey();
		return (-1);
	}
	(void)make_character_dump(player_ptr, fff);
	my_fclose(fff);

	msg_format(MES_FILE_SAVED(buf));
	msg_print(NULL);
	return SUCCESS;
}


/*
 * Display single line of on-line help file
 *
 * You can insert some special color tag to change text color.
 * Such as...
 * WHITETEXT [[[[y|SOME TEXT WHICH IS DISPLAYED IN YELLOW| WHITETEXT
 *
 * A colored segment is between "[[[[y|" and the last "|".
 * You can use any single character in place of the "|".
 */
static void show_file_aux_line(cptr str, int cy, cptr shower)
{
	static const char tag_str[] = "[[[[";
	byte color = TERM_WHITE;
	char in_tag = '\0';
	int cx = 0;
	int i;
	char lcstr[1024];

	if(shower) /* Make a lower case version of str for searching */
	{
		strcpy(lcstr, str);
		str_tolower(lcstr);
	}

	Term_gotoxy(cx, cy);
	for (i = 0; str[i];)
	{
		int len = strlen(&str[i]);
		int showercol = len + 1;
		int bracketcol = len + 1;
		int endcol = len;
		cptr ptr;

		/* Search for a shower string in the line */
		if(shower)
		{
			ptr = my_strstr(&lcstr[i], shower);
			if(ptr) showercol = ptr - &lcstr[i];
		}

		/* Search for a color segment tag */
		ptr = in_tag ? my_strchr(&str[i], in_tag) : my_strstr(&str[i], tag_str);
		if(ptr) bracketcol = ptr - &str[i];

		if(bracketcol < endcol) endcol = bracketcol; /* A color tag is found */
		if(showercol < endcol) endcol = showercol; /* The shower string is found before the color tag */

		/* Print a segment of the line */
		Term_addstr(endcol, color, &str[i]);
		cx += endcol;
		i += endcol;

		if(endcol == showercol) /* Shower string? */
		{
			int showerlen = strlen(shower);
			Term_addstr(showerlen, TERM_YELLOW, &str[i]); /* Print the shower string in yellow */
			cx += showerlen;
			i += showerlen;
		}
		else if(endcol == bracketcol) /* Colored segment? */
		{
			if(in_tag)
			{
				i++; /* Found the end of colored segment */
				in_tag = '\0'; /* Now looking for an another tag_str */
				color = TERM_WHITE; /* Set back to the default color */
			}
			else
			{
				i += sizeof(tag_str)-1; /* Found a tag_str, and get a tag color */
				color = color_char_to_acttr(str[i]); /* Get tag color */
				if(color == 255 || str[i+1] == '\0') /* Illegal color tag */
				{
					color = TERM_WHITE; /* Illegal color tag */
					Term_addstr(-1, TERM_WHITE, tag_str); /* Print the broken tag as a string */
					cx += sizeof(tag_str)-1;
				}
				else
				{
					i++; /* Skip the color tag */
					in_tag = str[i]; /* Now looking for a close tag */
					i++; /* Skip the close-tag-indicator */
				}
			}
		}

	} /* for (i = 0; str[i];) */
	Term_erase(cx, cy, 255); /* Clear rest of line */
}


/*
 * Recursive file perusal.
 *
 * Process various special text in the input file, including
 * the "menu" structures used by the "help file" system.
 *
 * Return FALSE on 'q' to exit from a deep, otherwise TRUE.
 */
bool show_file(bool show_version, cptr name, cptr what, int line, int mode)
{
	int i, n, skey;
	int next = 0;	// Number of "real" lines passed by
	int size = 0;	// Number of "real" lines in the file
	int back = 0;	// Backup value for "line"
	bool menu = FALSE;	// This screen has sub-screens

	FILE *fff = NULL;	// Current help file
	cptr find = NULL;	// Find this string (if any)
	cptr tag = NULL;	// Jump to this tag

	/* Hold strings to find/show */
	char finder_str[81];
	char shower_str[81];
	char back_str[81];

	cptr shower = NULL; /* String to show */

	char filename[1024];
	char caption[128];
	char path[1024]; /* Path buffer */
	char buf[1024]; /* General buffer */
	char hook[68][32]; /* Sub-menu information */

	bool reverse = (line < 0);

	int wid, hgt, rows;

	Term_get_size(&wid, &hgt);
	rows = hgt - 4;

	strcpy(finder_str, ""); /* Wipe finder */
	strcpy(shower_str, ""); /* Wipe shower */
	strcpy(caption, ""); /* Wipe caption */

	for (i = 0; i < 68; i++) hook[i][0] = '\0'; /* Wipe the hooks */
	strcpy(filename, name); /* Copy the filename */
	n = strlen(filename);

	for (i = 0; i < n; i++) /* Extract the tag from the filename */
	{
		if(filename[i] == '#')
		{
			filename[i] = '\0';
			tag = filename + i + 1;
			break;
		}
	}

	/* Redirect the name */
	name = filename;

	/* Hack  */
	if(what)
	{
		strcpy(caption, what);
		strcpy(path, name); /* Access the "file" */
		fff = my_fopen(path, "r"); /* Open */
	}

	if(!fff) /* Look in "help" */
	{
		sprintf(caption, "%s '%s'", get_keyword("KW_HELPFILE"), name);
		path_build(path, sizeof(path), ANGBAND_DIR_HELP, name);
		fff = my_fopen(path, "r");
	}

	if(!fff) /* Look in "info" */
	{
		sprintf(caption, MES_SYS_SPOILER_FILE(name));
		path_build(path, sizeof(path), ANGBAND_DIR_INFO, name);
		fff = my_fopen(path, "r");
	}

	if(!fff) /* Look in "info" */
	{
		path_build(path, sizeof(path), ANGBAND_DIR, name);
		for (i = 0; path[i]; i++) if('\\' == path[i]) path[i] = PATH_SEP[0];
		sprintf(caption, MES_SYS_SPOILER_FILE(name));

		fff = my_fopen(path, "r");
	}

	if(!fff)
	{
		msg_format(MES_FILE_LOADED_FAIL(name));
		msg_print(NULL);
		return TRUE;
	}

	while (TRUE) /* Pre-Parse the file */
	{
		char *str = buf;
		if(my_fgets(fff, buf, sizeof(buf))) break; /* Read a line or stop */
		if(prefix(str, "***** ")) /* XXX Parse "menu" items */
		{
			/* Notice "menu" requests */
			if((str[6] == '[') && isalpha(str[7]))
			{
				/* Extract the menu item */
				int k = str[7] - 'A';
				menu = TRUE; /* This is a menu file */

				if((str[8] == ']') && (str[9] == ' '))
				{
					strncpy(hook[k], str + 10, 31); /* Extract the menu item */
					hook[k][31] = '\0'; /* Make sure it's null-terminated */
				}
			}
			else if(str[6] == '<') /* Notice "tag" requests */
			{
				size_t len = strlen(str);

				if(str[len - 1] == '>')
				{
					str[len - 1] = '\0';
					if(tag && streq(str + 7, tag)) line = next;
				}
			}
			continue; /* Skip this */
		}
		next++; /* Count the "real" lines */
	}
	size = next; /* Save the number of "real" lines */
	if(line == -1) line = ((size-1)/rows)*rows; /* start from bottom when reverse mode */
	Term_clear();

	while (TRUE) /* Display the file */
	{
		/* Restart when necessary */
		if(line >= size - rows) line = size - rows;
		if(line < 0) line = 0;

		/* Re-open the file if needed */
		if(next > line)
		{
			my_fclose(fff);
			fff = my_fopen(path, "r"); /* Hack -- Re-Open the file */
			if(!fff) return FALSE;
			next = 0; /* File has been restarted */
		}

		while (next < line) /* Goto the selected line */
		{
			if(my_fgets(fff, buf, sizeof(buf))) break; /* Get a line */
			if(prefix(buf, "***** ")) continue; /* Skip tags/links */
			next++; /* Count the lines */
		}

		/* Dump the next 20, or rows, lines of the file */
		for (i = 0; i < rows; )
		{
			cptr str = buf;
			if(!i) line = next; /* Hack -- track the "first" line */
			if(my_fgets(fff, buf, sizeof(buf))) break; /* Get a line of the file or stop */
			if(prefix(buf, "***** ")) continue; /* Hack -- skip "special" lines */
			next++; /* Count the "real" lines */

			if(find && !i) /* Hack -- keep searching */
			{
				char lc_buf[1024];
				strcpy(lc_buf, str); /* Make a lower case version of str for searching */
				str_tolower(lc_buf);

				if(!my_strstr(lc_buf, find)) continue;
			}
			find = NULL; /* Hack -- stop searching */
			show_file_aux_line(str, i + 2, shower); /* Dump the line */
			i++; /* Count the printed lines */
		}

		while (i < rows)
		{
			Term_erase(0, i + 2, 255); /* Clear rest of line */
			i++;
		}
		if(find) /* Hack -- failed search */
		{
			bell();
			line = back;
			find = NULL;
			continue;
		}

		/* Show a general "title" */
		if(show_version) prt(format("[%s %d.%d.%d, %s, %d/%d]", VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH, caption, line, size), 0, 0);
		else prt(format("[%s, %d/%d]", caption, line, size), 0, 0);

		if(size <= rows) prt(MES_INTERFACE_SHOWFILE, hgt - 1, 0); /* Prompt -- small files */

		/* Prompt -- large files */
		else
		{
			if(reverse) prt(MES_INTERFACE_SHOWFILE_LONG2, hgt - 1, 0);
			else prt(MES_INTERFACE_SHOWFILE_LONG1, hgt - 1, 0);
		}

		/* Get a special key code */
		skey = inkey_special(TRUE);

		switch (skey)
		{
		/* Show the help for the help */
		case '?':
			if(strcmp(name, TEXT_FILES_HELPINFO) != 0) show_file(TRUE, TEXT_FILES_HELPINFO, NULL, 0, mode);
			break;

		/* Hack -- try showing */
		case '=':
			/* Get "shower" */
			prt(PROMPT_SHOW, hgt - 1, 0);
			strcpy(back_str, shower_str);
			if(askfor(shower_str, 80))
			{
				if(shower_str[0])
				{
					str_tolower(shower_str); /* Make it lowercase */
					shower = shower_str; /* Show it */
				}
				else shower = NULL; /* Stop showing */
			}
			else strcpy(shower_str, back_str);
			break;

		/* Hack -- try finding */
		case '/':
		case KTRL('s'):
			/* Get "finder" */
			prt(MES_SYS_FIND, hgt - 1, 0);

			strcpy(back_str, finder_str);
			if(askfor(finder_str, 80))
			{
				if(finder_str[0])
				{
					/* Find it */
					find = finder_str;
					back = line;
					line = line + 1;

					/* Make finder lowercase */
					str_tolower(finder_str);

					/* Show it */
					shower = finder_str;
				}
				else shower = NULL; /* Stop showing */
			}
			else strcpy(finder_str, back_str);
			break;

		/* Hack -- go to a specific line */
		case '#':
			{
				char tmp[81];
#ifdef JP
				prt("行: ", hgt - 1, 0);
#else
				prt("Goto Line: ", hgt - 1, 0);
#endif

				strcpy(tmp, "0");

				if(askfor(tmp, 80)) line = strtol(tmp, NULL, 10);
			}
			break;

		/* Hack -- go to the top line */
		case SKEY_TOP:
			line = 0;
			break;

		/* Hack -- go to the bottom line */
		case SKEY_BOTTOM:
			line = ((size - 1) / rows) * rows;
			break;

		/* Hack -- go to a specific file */
		case '%':
			{
				char tmp[81];
#ifdef JP
				prt("ファイル・ネーム: ", hgt - 1, 0);
#else
				prt("Goto File: ", hgt - 1, 0);
#endif
				strcpy(tmp, TEXT_FILES_HELP);
				if(askfor(tmp, 80) && !show_file(TRUE, tmp, NULL, 0, mode)) skey = 'q';
			}
			break;

		/* Allow backing up */
		case '-':
			line = line + (reverse ? rows : -rows);
			if(line < 0) line = 0;
			break;

		/* One page up */
		case SKEY_PGUP:
			line = line - rows;
			if(line < 0) line = 0;
			break;

		/* Advance a single line */
		case '\n':
		case '\r':
			line = line + (reverse ? -1 : 1);
			if(line < 0) line = 0;
			break;

		/* Move up / down */
		case '8':
		case SKEY_UP:
			line--;
			if(line < 0) line = 0;
			break;

		case '2':
		case SKEY_DOWN:
			line++;
			break;

		/* Advance one page */
		case ' ':
			line = line + (reverse ? -rows : rows);
			if(line < 0) line = 0;
			break;

		/* One page down */
		case SKEY_PGDOWN:
			line = line + rows;
			break;
		}

		/* Recurse on numbers */
		if(menu)
		{
			int key = -1;

			if(!(skey & SKEY_MASK) && isalpha(skey)) key = skey - 'A';

			if((key > -1) && hook[key][0])
			{
				/* Recurse on that file */
				if(!show_file(TRUE, hook[key], NULL, 0, mode)) skey = 'q';
			}
		}

		/* Hack, dump to file */
		if(skey == '|')
		{
			FILE *ffp;
			char buff[1024];
			char xtmp[82];

			strcpy (xtmp, "");
			if(!get_string(PROMPT_FILE, xtmp, 80)) continue;
			my_fclose(fff);

			path_build(buff, sizeof(buff), ANGBAND_DIR_USER, xtmp);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			ffp = my_fopen(buff, "w");

			if(!(fff && ffp))
			{
				msg_print(MES_SYS_FAILED_FILEOPEN);
				skey = ESCAPE;
				break;
			}

			sprintf(xtmp, "%s: %s", player_ptr->name, what ? what : caption);
			my_fputs(ffp, xtmp, 80);
			my_fputs(ffp, "\n", 80);

			while (!my_fgets(fff, buff, sizeof(buff))) my_fputs(ffp, buff, 80);
			my_fclose(fff);
			my_fclose(ffp);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");
		}

		if((skey == ESCAPE) || (skey == '<')) break; /* Return to last screen */
		if(skey == KTRL('q')) skey = 'q'; /* Exit on the ^q */
		if(skey == 'q') break; /* Exit on the q key */
	}

	my_fclose(fff);
	if(skey == 'q') return FALSE; /* Escape */
	return TRUE; /* Normal return */
}

/*
 * Process the player name.
 * Extract a clean "base name".
 * Build the savefile name if needed.
 */
void set_creature_name(bool sf, creature_type *creature_ptr)
{
	int i, k = 0;
	char old_player_base[32] = "";
	if(character_generated) strcpy(old_player_base, player_base);

	/* Cannot be too long */
#if defined(MACINTOSH) || defined(USE_EMX) || defined(VM)
	if(strlen(creature_ptr->name) > 15) quit_fmt(MES_SYS_TOO_LONG_NAME(creature_ptr->name));
#endif

	/* Cannot contain "icky" characters */
	for (i = 0; creature_ptr->name[i]; i++)
	{
		/* No control characters */
		if(is_mbyte(creature_ptr->name[i])){i++;continue;}
		if(iscntrl( (unsigned char)creature_ptr->name[i]))
		{
			quit_fmt(MES_SYS_CONTROL_CODE(creature_ptr->name)); /* Illegal characters */
		}
	}

#ifdef MACINTOSH
	/* Extract "useful" letters */
	for (i = 0; creature_ptr->name[i]; i++)
	{
#ifdef JP
		unsigned char c = creature_ptr->name[i];
#else
		char c = creature_ptr->name[i];
#endif


		/* Convert "dot" to "underscore" */
		if(c == '.') c = '_';

		/* Accept all the letters */
		player_base[k++] = c;
	}

#else

	/* Extract "useful" letters */
	for (i = 0; creature_ptr->name[i]; i++)
	{
		char c = creature_ptr->name[i];

		/* Accept some letters */
		if(is_mbyte(c)){
		  if(k + 2 >= sizeof(player_base) || !creature_ptr->name[i+1]) break;
		  player_base[k++] = c;
		  i++;
		  player_base[k++] = creature_ptr->name[i];
		}
#ifdef SJIS
		else if(iskana(c)) player_base[k++] = c;
#endif
		else
		/* Convert path separator to underscore */
		if(!strncmp(PATH_SEP, creature_ptr->name+i, strlen(PATH_SEP))){
			player_base[k++] = '_';
			i += strlen(PATH_SEP);
		}

		/* Convert some characters to underscore */
#if defined(WINDOWS)
		else if(my_strchr("\"*,/:;<>?\\|", c)) player_base[k++] = '_';
#endif
		else if(isprint(c)) player_base[k++] = c;
	}

#endif

	player_base[k] = '\0';

	/* Require a "base" name */
	if(!player_base[0]) strcpy(player_base, species_name + species_info[creature_ptr->species_idx].name);


#ifdef SAVEFILE_MUTABLE

	/* Accept */
	sf = TRUE;

#endif
	if(!savefile_base[0] && savefile[0])
	{
		cptr s;
		s = savefile;
		while(TRUE)
		{
			cptr t;
			t = my_strstr(s, PATH_SEP);
			if(!t)
				break;
			s = t+1;
		}
		strcpy(savefile_base, s);
	}

	if(!savefile_base[0] || !savefile[0])
		sf = TRUE;

	/* Change the savefile name */
	if(sf)
	{
		char temp[128];

		strcpy(savefile_base, player_base);

#ifdef SAVEFILE_USE_UID
		/* Rename the savefile, using the player_uid and player_base */
		(void)sprintf(temp, "%d.%s", player_uid, player_base);
#else
		/* Rename the savefile, using the player_base */
		(void)sprintf(temp, "%s", player_base);
#endif

		path_build(savefile, sizeof(savefile), ANGBAND_DIR_SAVE, temp);
	}

	/* Load an autopick preference file */
	if(character_generated)
	{
		if(!streq(old_player_base, player_base)) autopick_load_pref(FALSE);
	}
}


/*
 * Gets a name for the character, reacting to name changes.
 *
 * Assumes that "display_creature_status(creature_ptr)" has just been called
 *
 * Perhaps we should NOT ask for a name (at "birth()") on
 * Unix machines?
 *
 * What a horrible name for a global function.  
 */
void get_name(creature_type *creature_ptr)
{
	char tmp[80];

	/* Save the player name */
	strcpy(tmp, creature_ptr->name);

	/* Prompt for a new name */
	if(get_string(get_keyword("KW_NAME"), tmp, 70)) strcpy(creature_ptr->name, tmp);
	if(0 == strlen(creature_ptr->name)) strcpy(creature_ptr->name, species_name + species_info[creature_ptr->species_idx].name);

	strcpy(tmp,chara_info[creature_ptr->chara_idx].title);
#ifdef JP
	if(chara_info[creature_ptr->chara_idx].no) strcat(tmp,"の");
#else
	strcat(tmp, " ");
#endif
	strcat(tmp,creature_ptr->name);

	/* Re-Draw the name (in light blue) */
	Term_erase(34, 1, 255);
	c_put_str(TERM_L_BLUE, tmp, 1, 34);

	/* Erase the prompt, etc */
	clear_from(22);
}



/*
 * Hack -- commit suicide
 */
void do_cmd_suicide(creature_type *creature_ptr)
{
	int i;

	flush();
	if(creature_ptr->total_winner) if(!get_check_strict(MES_GET_CHECK_RETIRE, CHECK_NO_HISTORY)) return;
	else if(!get_check(MES_GET_CHECK_SUICIDE)) return;

	if(!noscore)
	{
		prt(MES_SYS_HIT_VERIFY_KEY, 0, 0);
		flush();
		i = inkey();
		prt("", 0, 0);
		if(i != '@') return;
	}

	/* Initialize "last message" buffer */
	if(creature_ptr->last_message) string_free(creature_ptr->last_message);
	creature_ptr->last_message = NULL;

	/* Hack -- Note *winning* message */
	if(creature_ptr->total_winner && last_words)
	{
		char buf[1024] = "";

		do
		{
			while (!get_string(get_keyword("KW_WINNING_MESSAGE"), buf, sizeof buf));
		}
		while (!get_check_strict(MES_SYS_ASK_SURE, CHECK_NO_HISTORY));

		if(buf[0])
		{
			creature_ptr->last_message = string_make(buf);
			msg_print(creature_ptr->last_message);
		}
	}

	/* Stop playing */
	playing = FALSE;

	/* Kill the player */
	gameover = TRUE;

	/* Leaving */
	subject_change_floor = TRUE;

	if(!creature_ptr->total_winner)
	{
		write_diary(DIARY_BUNSHOU, 0, DIARY_SUICIDE);
		write_diary(DIARY_GAMESTART, 1, DIARY_GAMEOVER);
		write_diary(DIARY_BUNSHOU, 1, "\n\n\n\n");
	}

	(void)strcpy(gameover_from, COD_QUITTING); /* Cause of death */
}


/*
 * Save the game
 */
void do_cmd_save_game(int is_autosave)
{
	// Autosaves do not disturb
	if(is_autosave) msg_print(MES_SYS_AUTO_SAVING);
	else disturb(player_ptr, 1, 0);

	/* Clear messages */
	msg_print(NULL);
	handle_stuff(player_ptr);

	prt(MES_SYS_GAME_SAVING, 0, 0);

	Term_fresh();
	(void)strcpy(gameover_from, COD_SAVE);

	/* Forbid suspend */
	signals_ignore_tstp();

	prt(MES_SYS_GAME_SAVING, 0, 0);
	if(save_player()) prt(get_keyword("KW_DONE"), 0, 40);
	else prt(get_keyword("KW_FAILED"), 0, 40);

	/* Allow suspend again */
	signals_handle_tstp();
	Term_fresh();
	(void)strcpy(gameover_from, COD_ALIVE_AND_WELL);

}


/*
 * Save the game and exit
 */
void do_cmd_save_and_exit(void)
{
	playing = FALSE;

	/* Leaving */
	subject_change_floor = TRUE;
	write_diary(DIARY_GAMESTART, 0, DIARY_SAVEQUIT);
}


/*
 * Hack -- Calculates the total number of points earned		-JWT-
 */
long total_points(creature_type *player_ptr)
{
	int i, mult = 100;
	FLOOR_LEV max_dl = 0;
	u32b point, point_h, point_l;
	int arena_win = MIN(arena_number, MAX_ARENA_MONS);

	if(!smart_learn) mult -= 20;
	if(smart_cheat) mult += 30;
	if(ironman_shops) mult += 50;
	if(ironman_small_levels) mult += 10;
	if(ironman_empty_levels) mult += 20;
	if(ironman_rooms) mult += 100;

	if(mult < 5) mult = 5;

	for (i = 0; i < max_dungeon_idx; i++)
		if(max_dlv[i] > max_dl) max_dl = max_dlv[i];

	point_l = (player_ptr->max_exp + (100 * max_dl));
	point_h = point_l / 0x10000L;
	point_l = point_l % 0x10000L;
	point_h *= mult;
	point_l *= mult;
	point_h += point_l / 0x10000L;
	point_l %= 0x10000L;

	point_l += ((point_h % 100) << 16);
	point_h /= 100;
	point_l /= 100;

	point = (point_h << 16) + (point_l);
	if(arena_number >= 0)
		point += (arena_win * arena_win * (arena_win > 29 ? 1000 : 100));

	if(ironman_downward) point *= 2;

	if((player_ptr->chara_idx == CHARA_MUNCHKIN) && point)
	{
		point = 1;
		if(player_ptr->total_winner) point = 2;
	}

	return point;
}


#define GRAVE_LINE_WIDTH 31

/*
 * Centers a string within a GRAVE_LINE_WIDTH character string		-JWT-
 */
static void center_string(char *buf, cptr str)
{
	int i, j;

	i = strlen(str); /* Total length */
	j = GRAVE_LINE_WIDTH / 2 - i / 2; /* Necessary border */

	(void)sprintf(buf, "%*s%s%*s", j, "", str, GRAVE_LINE_WIDTH - i - j, "");
}

/*
 * Redefinable "print_tombstone" action
 */
bool (*tombstone_aux)(void) = NULL;


/*
 * Display a "tomb-stone"
 */
static void print_tomb(creature_type *creature_ptr)
{
	bool done = FALSE;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Do we use a special tombstone ? */
	if(tombstone_aux)
	{
		/* Use tombstone hook */
		done = (*tombstone_aux)();
	}

	/* Print the text-tombstone */
	if(!done)
	{
		cptr   p;
		char   tmp[160];
		char   buf[1024];
		char   dummy[80];
		char   *t;
		FILE   *fp;
		time_t ct = time((time_t)0);
#ifdef JP
		int    extra_line = 0;
#endif

		Term_clear();

		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, TEXT_FILES_DEAD);

		/* Open the News file */
		fp = my_fopen(buf, "r");

		/* Dump */
		if(fp)
		{
			int i = 0;
			while (0 == my_fgets(fp, buf, sizeof(buf))) put_str(buf, i++, 0); /* Dump the file to the screen */
			my_fclose(fp);
		}

		if(creature_ptr->total_winner || (creature_ptr->lev > CREATURE_MAX_LEVEL)) p = get_keyword("KW_WINNER");
		else p = class_info[creature_ptr->class_idx].title;

		center_string(buf, creature_ptr->name);
		put_str(buf, 6, 11);

#ifndef JP
		center_string(buf, "the");
		put_str(buf, 7, 11);
#endif
		center_string(buf, p);
		put_str(buf, 8, 11);

		center_string(buf, class_info[creature_ptr->class_idx].title);
		put_str(buf, 10, 11);

		(void)sprintf(tmp, "%s: %d", get_keyword("KW_LEVEL"), (int)creature_ptr->lev);
		center_string(buf, tmp);
		put_str(buf, 11, 11);

		(void)sprintf(tmp, "%s: %ld", get_keyword("KW_EXP"), (long)creature_ptr->exp);
		center_string(buf, tmp);
		put_str(buf, 12, 11);

		(void)sprintf(tmp, "%s: %ld", MES_SYS_MONEY, (long)creature_ptr->au);
		center_string(buf, tmp);
		put_str(buf, 13, 11);

		if(streq(gameover_from, COD_QUITTING)) strcpy(tmp, get_keyword("KW_SUICIDE"));
#ifdef JP
		else if(streq(gameover_from, COD_RETIRE)) strcpy(tmp, "引退後に天寿を全う");
		else if(streq(gameover_from, COD_SEPPUKU)) strcpy(tmp, "勝利の後、切腹");
		else
		{
			roff_to_buf(gameover_from, GRAVE_LINE_WIDTH + 1, tmp, sizeof tmp);
			t = tmp + strlen(tmp) + 1;
			if(*t)
			{
				strcpy(dummy, t); /* 2nd line */
				if(*(t + strlen(t) + 1)) /* Does 3rd line exist? */
				{
					for (t = dummy + strlen(dummy) - 2; is_mbyte(*(t - 1)); t--) /* Loop */;
					strcpy(t, "…");
				}
				else if(my_strstr(tmp, "『") && suffix(dummy, "』"))
				{
					char dummy2[80];
					char *name_head = my_strstr(tmp, "『");
					sprintf(dummy2, "%s%s", name_head, dummy);
					if(strlen(dummy2) <= GRAVE_LINE_WIDTH)
					{
						strcpy(dummy, dummy2);
						*name_head = '\0';
					}
				}
				else if(my_strstr(tmp, "「") && suffix(dummy, "」"))
				{
					char dummy2[80];
					char *name_head = my_strstr(tmp, "「");
					sprintf(dummy2, "%s%s", name_head, dummy);
					if(strlen(dummy2) <= GRAVE_LINE_WIDTH)
					{
						strcpy(dummy, dummy2);
						*name_head = '\0';
					}
				}
				center_string(buf, dummy);
				put_str(buf, 15, 11);
				extra_line = 1;
			}
		}
		center_string(buf, tmp);
		put_str(buf, 14, 11);

		if(!streq(gameover_from, COD_RETIRE) && !streq(gameover_from, COD_SEPPUKU))
		{
			if(floor_ptr->depth == 0)
			{
				if(streq(gameover_from, COD_QUITTING)) sprintf(tmp, "%sで死んだ", get_keyword("KW_SURFACE"));
				else sprintf(tmp, "に%sで殺された", get_keyword("KW_SURFACE"));
			}
			else
			{
				if(streq(gameover_from, COD_QUITTING))
				{
					sprintf(tmp, "地下 %d 階で死んだ", floor_ptr->depth);
				}
				else
				{
					sprintf(tmp, "に地下 %d 階で殺された", floor_ptr->depth);
				}
			}
			center_string(buf, tmp);
			put_str(buf, 15 + extra_line, 11);
		}
#else
		(void)sprintf(tmp, "Killed on Level %d", floor_ptr->depth);
		center_string(buf, tmp);
		put_str(buf, 14, 11);

		roff_to_buf(format("by %s.", gameover_from), GRAVE_LINE_WIDTH + 1, tmp, sizeof tmp);
		center_string(buf, tmp);
		put_str(buf, 15, 11);
		t = tmp + strlen(tmp) + 1;
		if(*t)
		{
			strcpy(dummy, t); /* 2nd line */
			if(*(t + strlen(t) + 1)) /* Does 3rd line exist? */
			{
				int dummy_len = strlen(dummy);
				strcpy(dummy + MIN(dummy_len, GRAVE_LINE_WIDTH - 3), "...");
			}
			center_string(buf, dummy);
			put_str(buf, 16, 11);
		}
#endif

		(void)sprintf(tmp, "%-.24s", ctime(&ct));
		center_string(buf, tmp);
		put_str(buf, 17, 11);

		msg_format(MES_GOODBYE(creature_ptr->name));
	}
}


/*
 * Display some character info
 */
static void show_info(creature_type *creature_ptr)
{
	int i;
	object_type *object_ptr;

	/* Hack -- Know everything in the inven/equip */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		if(!is_valid_object(object_ptr)) continue;

		/* Aware and Known */
		object_aware(object_ptr);
		object_known(object_ptr);
	}

	/*
	for (i = 1; i < max_towns; i++)
	{
		st_ptr = &town[i].store[STORE_HOME];

		// Hack -- Know everything in the home
		for (j = 0; j < st_ptr->stock_num; j++)
		{
			object_ptr = &st_ptr->stock[j];

			// Skip non-objects
			if(!is_valid_object(object_ptr)) continue;

			// Aware and Known
			object_aware(object_ptr);
			object_known(object_ptr);
		}
	}
	*/

	/* Hack -- Recalculate bonuses */
	prepare_update(creature_ptr, CRU_BONUS);

	handle_stuff(creature_ptr);
	flush(); /* Flush all input keys */

	msg_print(NULL);
	prt(MES_SYS_DUMP_INFO1, 21, 0);
	prt(MES_SYS_DUMP_INFO2, 22, 0);

	while (TRUE) /* Dump character records as requested */
	{
		char out_val[160];
		put_str(PROMPT_FILE, 23, 0);
		strcpy(out_val, "");
		if(!askfor(out_val, 60)) return; /* Ask for filename (or abort) */
		if(!out_val[0]) break; /* Return means "show on screen" */

		screen_save();
		(void)file_character(out_val); /* Dump a character file */
		screen_load();
	}

	update_play_time();

	display_creature_status(0, creature_ptr); /* Display player */
	prt(MES_SYS_HIT_ANY_KEY_CAN_ESC, 23, 0); /* Prompt for inventory */

	/* Allow abort at this point */
	if(inkey() == ESCAPE) return;

	/* Show equipment and inventory */

	/* Equipment -- if any */
	if(creature_ptr->equip_cnt)
	{
		Term_clear();
		(void)show_item_list(0, creature_ptr, SHOW_ITEM_INVENTORY | SHOW_ITEM_FULL, NULL);
		prt(format("%s %s", get_keyword("KW_EQUIPMENT"), get_keyword("KW_MORE")), 0, 0);
		if(inkey() == ESCAPE) return;
	}

	if(creature_ptr->inven_cnt) /* inventory -- if any */
	{
		Term_clear();
		(void)show_item_list(0, creature_ptr, SHOW_ITEM_INVENTORY | SHOW_ITEM_FULL, NULL);
		prt(format("%s %s", get_keyword("KW_INVENTORY"), get_keyword("KW_MORE")), 0, 0);
		if(inkey() == ESCAPE) return;
	}

	/* Homes in the different towns */
	//TODO
	/*
	for (l = 1; l < max_towns; l++)
	{
		st_ptr = &town[l].store[STORE_HOME];

		// Home -- if anything there
		if(st_ptr->stock_num)
		{
			// Display contents of the home
			for (k = 0, i = 0; i < st_ptr->stock_num; k++)
			{
				// Clear screen
				Term_clear();

				// Show 12 items
				for (j = 0; (j < 12) && (i < st_ptr->stock_num); j++, i++)
				{
					char object_name[MAX_NLEN];
					char tmp_val[80];

					// Acquire item
					object_ptr = &st_ptr->stock[i];

					// Print header, clear line
					sprintf(tmp_val, "%c) ", I2A(j));
					prt(tmp_val, j+2, 4);

					// Display object description
					object_desc(object_name, object_ptr, 0);
					c_put_str(tval_to_acttr[object_ptr->tval], object_name, j+2, 7);
				}

				// Caption
				prt(format(MES_INTERFACE_HOME_ITEM(k+1)), 0, 0);

				// Wait for it
				if(inkey() == ESCAPE) return;
			}
		}
	}
	*/
}

static bool check_score(creature_type *player_ptr)
{
	Term_clear();

	if(highscore_fd < 0)
	{
		msg_print(MES_SYS_SCORE_FAILED);
		msg_print(NULL);
		return FALSE;
	}

	/* Wizard-mode pre-empts scoring */
	if(noscore & 0x000F)
	{
		msg_print(MES_SCORE_DISABLE_WIZARD);
		msg_print(NULL);
		return FALSE;
	}

	/* Unique mode pre-empts scoring */
	if(noscore & 0x0004)
	{
		msg_print(MES_SCORE_DISABLE_UNIQUE);
		msg_print(NULL);
		return FALSE;
	}

	/* Cheaters are not scored */
	if(noscore & 0xFF00)
	{
		msg_print(MES_SCORE_DISABLE_CHEAT);
		msg_print(NULL);
		return FALSE;
	}

	if(!player_ptr->total_winner && streq(gameover_from, COD_INTERRUPTING))
	{
		msg_print(MES_SCORE_DISABLE_INTERUPT);
		msg_print(NULL);
		return FALSE;
	}

	/* Quitter */
	if(!player_ptr->total_winner && streq(gameover_from, COD_QUITTING))
	{
		msg_print(MES_SCORE_DISABLE_QUIT);
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}

/*
 * Close up the current game (player may or may not be dead)
 * This function is called only from "main.c" and "signals.c".
 */
void close_game(void)
{
	char buf[1024];
	bool do_send = TRUE;

/*	cptr p = "[i:キャラクタの情報, f:ファイル書き出し, t:スコア, x:*鑑定*, ESC:ゲーム終了]"; */

	handle_stuff(player_ptr);

	/* Flush the messages */
	msg_print(NULL);

	/* Flush the input */
	flush();

	/* No suspending now */
	signals_ignore_tstp();

	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	/* Grab permissions */
	safe_setuid_grab();

	/* Open the high score file, for reading/writing */
	highscore_fd = fd_open(buf, O_RDWR);

	/* Drop permissions */
	safe_setuid_drop();

	/* Handle death */
	if(gameover)
	{
		if(player_ptr->total_winner) kingly(player_ptr); /* Handle retirement */
		if(!cheat_save || get_check(MES_SCORE_SAVE_DEATH)) if(!save_player()) msg_print(MES_SYS_SAVE_FAILED); /* Save memories */
		else do_send = FALSE;

		print_tomb(player_ptr); /* You are dead */
		flush();

		/* Show more info */
		show_info(player_ptr);
		Term_clear();

		if(check_score(player_ptr))
		{
			if((!send_world_score(do_send)))
			{
				if(get_check_strict(MES_SCORE_STAND_BY, (CHECK_NO_ESCAPE | CHECK_NO_HISTORY)))
				{
					wait_report_score = TRUE;
					gameover = FALSE;
					if(!save_player()) msg_print(MES_SYS_SAVE_FAILED);
				}
			}
			if(!wait_report_score)
				(void)top_twenty(player_ptr);
		}
		else if(highscore_fd >= 0) display_scores_aux(0, 10, -1, NULL);
	}

	/* Still alive */
	else
	{
		do_cmd_save_game(FALSE);
		prt(MES_SYS_QUITING_KEY, 0, 40);
		if(inkey() != ESCAPE) predict_score(player_ptr);
	}

	/* Shut the high score file */
	(void)fd_close(highscore_fd);

	/* Forget the high score fd */
	highscore_fd = -1;

	/* Allow suspending now */
	signals_handle_tstp();
}


/*
 * Handle abrupt death of the visual system
 *
 * This routine is called only in very rare situations, and only
 * by certain visual systems, when they experience fatal errors.
 *
 *  Hack -- clear the death flag when creating a HANGUP
 * save file so that player can see tombstone when restart.
 */
void exit_game_panic(creature_type *player_ptr)
{
	/* If nothing important has happened, just quit */
	if(!character_generated || character_saved) quit(MES_SYS_PANIC);

	/* Mega-Hack -- see "msg_print()" */
	msg_flag = FALSE;

	/* Clear the top line */
	prt("", 0, 0);

	/* Hack -- turn off some things */
	disturb(player_ptr, 1, 0);

	/* Mega-Hack -- Delay death */
	if(player_ptr->chp < 0) gameover = FALSE;

	/* Hardcode panic save */
	panic_save = TRUE;

	/* Forbid suspend */
	signals_ignore_tstp();

	/* Indicate panic save */
	(void)strcpy(gameover_from, COD_PANIC_SAVE);
	if(!save_player()) quit(MES_SYS_PANIC_SAVE_FAILED);
	quit(MES_SYS_PANIC_SAVE_SUCCEED);

}


/*
 * Get a random line from a file
 * Based on the creature speech patch by Matt Graham,
 */
errr get_rnd_line(cptr file_name, int entry, char *output)
{
	FILE    *fp;
	char    buf[1024];
	int     counter, test;
	int     line_num = 0;


	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, file_name);

	fp = my_fopen(buf, "r");

	if(!fp) return -1;

	/* Find the entry of the creature */
	while (TRUE)
	{
		/* Get a line from the file */
		if(my_fgets(fp, buf, sizeof(buf)) == 0)
		{
			line_num++; /* Count the lines */

			/* Look for lines starting with 'N:' */
			if((buf[0] == 'N') && (buf[1] == ':'))
			{
				/* Allow default lines */
				if(buf[2] == '*') break;
				else if(buf[2] == 'M') if(IS_MALE(&species_info[entry])) break;
				else if(buf[2] == 'F') if(IS_FEMALE(&species_info[entry])) break;
				/* Get the creature number */
				else if(sscanf(&(buf[2]), "%d", &test) != EOF) if(test == entry) break; /* Is it the right number? */
				else
				{
					/* Error while converting the number */
					msg_format("Error in line %d of %s!", line_num, file_name);
					my_fclose(fp);
					return -1;
				}
			}
		}
		else
		{
			/* Reached end of file */
			my_fclose(fp);
			return -1;
		}
	}

	/* Get the random line */
	for (counter = 0; ; counter++)
	{
		while (TRUE)
		{
			test = my_fgets(fp, buf, sizeof(buf));

			/* Count the lines */
			/* line_num++; No more needed */

			if(!test)
			{
				/* Ignore lines starting with 'N:' */
				if((buf[0] == 'N') && (buf[1] == ':')) continue;
				if(buf[0] != '#') break;
			}
			else break;
		}
		if(!buf[0]) break;
		if(one_in_(counter + 1)) strcpy(output, buf); /* Copy the line */
	}

	my_fclose(fp);

	return counter ? 0 : -1;
}


#ifdef JP
errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count)
{
	int  i, j, kanji;
	errr result = 1;

	for (i = 0; i < count; i++)
	{
		result = get_rnd_line(file_name, entry, output);
		if(result) break;
		kanji = 0;
		for (j = 0; output[j]; j++) kanji |= is_mbyte(output[j]);
		if(kanji) break;
	}
	return result;
}
#endif

/*
 * Process file for auto picker/destroyer.
 */
errr process_autopick_file(cptr name)
{
	char buf[1024];

	errr err = 0;

	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);
	err = process_pref_file_aux(buf, PREF_TYPE_AUTOPICK);
	return (err);
}


/*
 * Process file for player's history editor.
 */
errr process_histpref_file(cptr name)
{
	char buf[1024];
	errr err = 0;
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);
	err = process_pref_file_aux(buf, PREF_TYPE_HISTPREF);
	return (err);
}


static errr counts_seek(int fd, u32b where, bool flag)
{
	huge seekpoint;
	char temp1[128], temp2[128];
	u32b zero_header[3] = {0L, 0L, 0L};
	int i;

#ifdef SAVEFILE_USE_UID
	(void)sprintf(temp1, "%d.%s.%d%d%d", player_uid, savefile_base, player_ptr->class_idx, player_ptr->chara_idx, player_ptr->age);
#else
	(void)sprintf(temp1, "%s.%d%d%d", savefile_base, player_ptr->class_idx, player_ptr->chara_idx, player_ptr->age);
#endif
	for (i = 0; temp1[i]; i++)
		temp1[i] ^= (i+1) * 63;

	seekpoint = 0;
	while(TRUE)
	{
		if(fd_seek(fd, seekpoint + 3 * sizeof(u32b)))
			return 1;
		if(fd_read(fd, (char*)(temp2), sizeof(temp2)))
		{
			if(!flag)
				return 1;
			/* add new name */
			fd_seek(fd, seekpoint);
			fd_write(fd, (char*)zero_header, 3*sizeof(u32b));
			fd_write(fd, (char*)(temp1), sizeof(temp1));
			break;
		}

		if(strcmp(temp1, temp2) == 0)
			break;

		seekpoint += 128 + 3 * sizeof(u32b);
	}

	return fd_seek(fd, seekpoint + where * sizeof(u32b));
}

u32b counts_read(int where)
{
	int fd;
	u32b count = 0;
	char buf[1024];

	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, format(BINARY_FILES_RAW("z_info")));
	fd = fd_open(buf, O_RDONLY);
	if(counts_seek(fd, where, FALSE) || fd_read(fd, (char*)(&count), sizeof(u32b))) count = 0;
	(void)fd_close(fd);
	return count;
}

errr counts_write(int where, u32b count)
{
	int fd;
	char buf[1024];
	errr err;
	path_build(buf, sizeof(buf), ANGBAND_DIR_DATA, format(BINARY_FILES_RAW("z_info")));

	/* Grab permissions */
	safe_setuid_grab();

	fd = fd_open(buf, O_RDWR);

	/* Drop permissions */
	safe_setuid_drop();

	if(fd < 0)
	{
		/* File type is "DATA" */
		FILE_TYPE(FILE_TYPE_DATA);

		/* Grab permissions */
		safe_setuid_grab();

		/* Create a new high score file */
		fd = fd_make(buf, 0644);

		/* Drop permissions */
		safe_setuid_drop();
	}

	/* Grab permissions */
	safe_setuid_grab();

	err = fd_lock(fd, F_WRLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if(err) return 1;

	counts_seek(fd, where, TRUE);
	fd_write(fd, (char*)(&count), sizeof(u32b));

	/* Grab permissions */
	safe_setuid_grab();

	err = fd_lock(fd, F_UNLCK);

	/* Drop permissions */
	safe_setuid_drop();

	if(err) return 1;

	(void)fd_close(fd);

	return 0;
}


#ifdef HANDLE_SIGNALS


#include <signal.h>


/*
 * Handle signals -- suspend
 *
 * Actually suspend the game, and then resume cleanly
 */
static void handle_signal_suspend(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);

#ifdef SIGSTOP

	/* Flush output */
	Term_fresh();

	/* Suspend the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 0);

	/* Suspend ourself */
	(void)kill(0, SIGSTOP);

	/* Resume the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 1);

	Term_redraw();

	/* Flush the term */
	Term_fresh();

#endif

	/* Restore handler */
	(void)signal(sig, handle_signal_suspend);
}


/*
 * Handle signals -- simple (interrupt and quit)
 *
 * This function was causing a *huge* number of problems, so it has
 * been simplified greatly.  We keep a global variable which counts
 * the number of times the user attempts to kill the process, and
 * we commit suicide if the user does this a certain number of times.
 *
 * We attempt to give "feedback" to the user as he approaches the
 * suicide thresh-hold, but without penalizing accidental keypresses.
 *
 * To prevent messy accidents, we should reset this global variable
 * whenever the user enters a keypress, or something like that.
 */
static void handle_signal_simple(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if(!character_generated || character_saved) quit(NULL);


	/* Count the signals */
	signal_count++;


	/* Terminate dead characters */
	if(gameover)
	{
		/* Mark the savefile */
		(void)strcpy(gameover_from, COD_INTERRUPTING);

		forget_lite(CURRENT_FLOOR_PTR);
		forget_view(CURRENT_FLOOR_PTR);
		clear_creature_lite(CURRENT_FLOOR_PTR);
		close_game();
		quit(COD_INTERRUPTING);

	}

	/* Allow suicide (after 5) */
	else if(signal_count >= 5)
	{
		(void)strcpy(gameover_from, MES_SYS_INTERUPTING); /* Cause of "death" */

		forget_lite(CURRENT_FLOOR_PTR);
		forget_view(CURRENT_FLOOR_PTR);
		clear_creature_lite(CURRENT_FLOOR_PTR);
		playing = FALSE; /* Stop playing */
		gameover = TRUE;
		subject_change_floor = TRUE; /* Leaving */
		close_game();
		quit(COD_INTERRUPTING);
	}

	/* Give warning (after 4) */
	else if(signal_count >= 4)
	{
		Term_xtra(TERM_XTRA_NOISE, 0); /* Make a noise */
		Term_erase(0, 0, 255); /* Clear the top line */
		Term_putstr(0, 0, -1, TERM_WHITE, MES_SYS_ERROR_GAMEOVER); /* Display the cause */
		Term_fresh();
	}

	/* Give warning (after 2) */
	else if(signal_count >= 2)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);
	}

	/* Restore handler */
	(void)signal(sig, handle_signal_simple);
}


/*
 * Handle signal -- abort, kill, etc
 */
static void handle_signal_abort(int sig)
{
	int wid, hgt;

	Term_get_size(&wid, &hgt);

	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if(!character_generated || character_saved) quit(NULL);

	forget_lite(CURRENT_FLOOR_PTR);
	forget_view(CURRENT_FLOOR_PTR);
	clear_creature_lite(CURRENT_FLOOR_PTR);

	/* Clear the bottom line */
	Term_erase(0, hgt - 1, 255);

	Term_putstr(0, hgt - 1, -1, TERM_RED, MES_SYS_TERRIBLE_BUGS);
	Term_putstr(45, hgt - 1, -1, TERM_RED, MES_SYS_PANIC_SAVE);
	write_diary(DIARY_GAMESTART, 0, DIARY_GAMEABORT);

	/* Flush output */
	Term_fresh();

	/* Panic Save */
	panic_save = TRUE;

	/* Panic save */
	(void)strcpy(gameover_from, COD_PANIC_SAVE);

	/* Forbid suspend */
	signals_ignore_tstp();

	// Attempt to save
	if(save_player()) Term_putstr(45, hgt - 1, -1, TERM_RED, MES_SYS_PANIC_SAVE_SUCCEED);
	else Term_putstr(45, hgt - 1, -1, TERM_RED, MES_SYS_PANIC_SAVE_FAILED);

	/* Flush output */
	Term_fresh();

#ifdef JP
	quit("ソフトのバグ");
#else
	quit("software bug");
#endif

}




/*
 * Ignore SIGTSTP signals (keyboard suspend)
 */
void signals_ignore_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, SIG_IGN);
#endif

}

/*
 * Handle SIGTSTP signals (keyboard suspend)
 */
void signals_handle_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif

}


/*
 * Prepare to handle the relevant signals
 */
void signals_init(void)
{
#ifdef SIGHUP
	(void)signal(SIGHUP, SIG_IGN);
#endif

#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif

#ifdef SIGINT
	(void)signal(SIGINT, handle_signal_simple);
#endif

#ifdef SIGQUIT
	(void)signal(SIGQUIT, handle_signal_simple);
#endif

#ifdef SIGFPE
	(void)signal(SIGFPE, handle_signal_abort);
#endif

#ifdef SIGILL
	(void)signal(SIGILL, handle_signal_abort);
#endif

#ifdef SIGTRAP
	(void)signal(SIGTRAP, handle_signal_abort);
#endif

#ifdef SIGIOT
	(void)signal(SIGIOT, handle_signal_abort);
#endif

#ifdef SIGKILL
	(void)signal(SIGKILL, handle_signal_abort);
#endif

#ifdef SIGBUS
	(void)signal(SIGBUS, handle_signal_abort);
#endif

#ifdef SIGSEGV
	(void)signal(SIGSEGV, handle_signal_abort);
#endif

#ifdef SIGTERM
	(void)signal(SIGTERM, handle_signal_abort);
#endif

#ifdef SIGPIPE
	(void)signal(SIGPIPE, handle_signal_abort);
#endif

#ifdef SIGEMT
	(void)signal(SIGEMT, handle_signal_abort);
#endif

#ifdef SIGDANGER
	(void)signal(SIGDANGER, handle_signal_abort);
#endif

#ifdef SIGSYS
	(void)signal(SIGSYS, handle_signal_abort);
#endif

#ifdef SIGXCPU
	(void)signal(SIGXCPU, handle_signal_abort);
#endif

#ifdef SIGPWR
	(void)signal(SIGPWR, handle_signal_abort);
#endif

}

#else	/* HANDLE_SIGNALS */

/*
 * Do nothing
 */
void signals_ignore_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_handle_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_init(void)
{
}
#endif	/* HANDLE_SIGNALS */

void dump_yourself(creature_type *creature_ptr, FILE *fff)
{
	char temp[80*10];
	int i;
	cptr t;

	if(!fff) return;
	roff_to_buf(race_text + race_info[creature_ptr->race_idx1].text, 78, temp, sizeof(temp));
	fprintf(fff, "\n\n%s: %s\n", get_keyword("KW_RACE"), race_info[creature_ptr->race_idx1].title);

	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(class_text + class_info[creature_ptr->class_idx].text, 78, temp, sizeof(temp));
	fprintf(fff, "\n");
	fprintf(fff, "%s: %s\n", get_keyword("KW_CLASS"), class_info[creature_ptr->class_idx].title);
	t = temp;
	for (i = 0; i < 10; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	roff_to_buf(chara_text + chara_info[creature_ptr->chara_idx].text, 78, temp, sizeof(temp));
	fprintf(fff, "\n");
	fprintf(fff, "%s: %s\n", get_keyword("KW_CHARA"), chara_info[creature_ptr->chara_idx].title);
	t = temp;
	for (i = 0; i < STAT_MAX; i++)
	{
		if(t[0] == 0)
			break; 
		fprintf(fff, "%s\n",t);
		t += strlen(t) + 1;
	}
	fprintf(fff, "\n");
	if(creature_ptr->realm1)
	{
		roff_to_buf(realm_jouhou[technic2magic(creature_ptr->realm1)-1], 78, temp, sizeof(temp));
		fprintf(fff, "%s: %s\n", get_keyword("KW_REALM"), realm_names[creature_ptr->realm1]);
		t = temp;
		for (i = 0; i < STAT_MAX; i++)
		{
			if(t[0] == 0)
				break; 
			fprintf(fff, "%s\n",t);
			t += strlen(t) + 1;
		}
	}
	fprintf(fff, "\n");
	if(creature_ptr->realm2)
	{
		roff_to_buf(realm_jouhou[technic2magic(creature_ptr->realm2)-1], 78, temp, sizeof(temp));
		fprintf(fff, "%s: %s\n", get_keyword("KW_REALM"), realm_names[creature_ptr->realm2]);
		t = temp;
		for (i = 0; i < STAT_MAX; i++)
		{
			if(t[0] == 0)
				break; 
			fprintf(fff, "%s\n",t);
			t += strlen(t) + 1;
		}
	}
}

