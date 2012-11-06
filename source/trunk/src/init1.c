/* File: init1.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Initialization (part 1) -BEN- */

#include "angband.h"


/*
 * This file is used to initialize various variables and arrays for the
 * Angband game.  Note the use of "fd_read()" and "fd_write()" to bypass
 * the common limitation of "read()" and "write()" to only 32767 bytes
 * at a time.
 *
 * Several of the arrays for Angband are built from "template" files in
 * the "lib/file" directory, from which quick-load binary "image" files
 * are constructed whenever they are not present in the "lib/data"
 * directory, or if those files become obsolete, if we are allowed.
 *
 * Warning -- the "ascii" file parsers use a minor hack to collect the
 * name and text information in a single pass.  Thus, the game will not
 * be able to load any template file with more than 20K of names or 60K
 * of text, even though technically, up to 64K should be legal.
 *
 * Note that if "ALLOW_TEMPLATES" is not defined, then a lot of the code
 * in this file is compiled out, and the game will not run unless valid
 * "binary template files" already exist in "lib/data".  Thus, one can
 * compile Angband with ALLOW_TEMPLATES defined, run once to create the
 * "*.raw" files in "lib/data", and then quit, and recompile without
 * defining ALLOW_TEMPLATES, which will both save 20K and prevent people
 * from changing the ascii template files in potentially dangerous ways.
 *
 * The code could actually be removed and placed into a "stand-alone"
 * program, but that feels a little silly, especially considering some
 * of the platforms that we currently support.
 */

#include "init.h"


static errr grab_one_trait(traits_precondition *cf_ptr, cptr what, byte add, byte remove, byte prob);


static void note(cptr str)
{
	Term_erase(0, 23, 255);
	Term_putstr(20, 23, -1, TERM_WHITE, str);
	Term_fresh();
}


/*** Creature CSV List ***/


// Race Flags
static cptr race_flags[MAX_RACES] =
{
	"HUMAN",
	"ELF",
	"DWARF",
	"HOBBIT",
	"GNOME",

	"BARBARIAN",
	"DUNADAN",
	"MELNIBONE",
	"AMBERITE",
	"CHAOSIAN",

	"DARK_ELF",
	"ELDAR",
	"AVARI",
	"KOBOLD",
	"NIBELUNG",

	"SPRITE",
	"S_FAIRY",
	"WAREFOLK",
	"SKAVEN",
	"LIZARDMAN",

	"BEASTMAN",
	"MINOTAUR",
	"NAGA",
	"MARFOLK",
	"CENTAUR",

	"ENT",
	"ORC",
	"URUK",
	"TROLL",
	"OLOG",

	"OGRE",
	"GIANT",
	"CYCLOPS",
	"TITAN",
	"YEEK",

	"KLACKON",
	"MIND_FLAYER",
	"DRACONIAN",
	"SAIYAN",
	"KUTAR",

	"JURAL",
	"EWOK",
	"ANGEL",
	"IMP",
	"DEMON",

	"MAIAR",
	"DEEP_ONE",
	"ELDER_ONE",
	"MI_GO",
	"FLYING_POLYP",
//50
	"SHOGGOTH",
	"YITH",
	"ISTARI",
	"BALROG",
	"DRAGON",

	"GIANT_APE",
	"SUPER_SAIYAN",
	"HORSE",
	"ANT",
	"BAT",
//60
	"CENTIPETE",
	"FLOATING_EYE",
	"FELINE",
	"ICKY",
	"JELLY",

	"AQUARIC_MONSTER",
	"MOLD",
	"QUADRUPED",
	"RODENT",
	"VORTEX",
//70
	"WORM",
	"MIMIC",
	"BIRD",
	"CANINE",
	"ELEMENTAL",

	"INSECT",
	"SNAKE",
	"SHADOW",
	"QUYLTHLUG",
	"SPIDER",
//80
	"CRUSTACEA",
	"WRAITH",
	"XORN",
	"YETI",
	"ZEPHYR_HOUND",

	"FUNGUS",
	"GREAT_OLD_ONE",
	"REPTILIA",
	"FIEND_LORD",
	"HYDRA",
};

static cptr class_flags[MAX_CLASS]=
{
	"NONE",
	"WARRIOR",
	"MAGE",
	"PRIEST",
	"ROGUE",
	"RANGER",
	"PALADIN",
	"WARRIOR_MAGE",
	"CHAOS_WARRIOR",
	"MONK",
	"MINDCRAFTER",
	"HIGH_MAGE",
	"TOURIST",
	"IMITATOR",
	"BEASTMASTER",
	"SORCERER",
	"ARCHER",
	"MAGIC_EATER",
	"BARD",
	"RED_MAGE",
	"SAMURAI",
	"FORCETRAINER",
	"BLUE_MAGE",
	"CAVALRY",
	"BERSERKER",
	"SMITH",
	"MIRROR_MASTER",
	"NINJA",
	"SNIPER",
	"COMMONER",
	"SOLDIER",
};

static cptr class_type_flags[]=
{
	"NONE",
	"WARRIOR",
	"MAGE",
	"PRIEST",
	"ROGUE",
	"RANGER",
	"PALADIN",
};

static cptr chara_flags[MAX_CHARA]=
{
	"ORDINARY",
	"MIGHTY",
	"SHREWD",
	"PIOUS",
	"NIMBLE",
	"FEARLESS",
	"COMBAT",
	"LAZY",
	"SEXY",
	"LUCKY",
	"PATIENT",
	"MUNCHIKIN",
	"CHARGEMAN",
	"POWERLESS",
	"IGNORANT",
	"IMBECILE",
	"SLOW-WITTED",
	"WEAK",
	"UGLY",
};

static cptr realm_flags[MAX_REALM]=
{
	"NONE",
	"LIFE",
	"SORCERY",
	"NATURE",
	"CHAOS",
	"DEATH",
	"TRUMP",
	"ARCANE",
	"CRAFT",
	"DAEMON",

	"CRUSADE",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"MUSIC",
	"HISSATSU",
	"HEX",
};

// Equip Slot Flags
static cptr equip_slot_flags[MAX_INVENTORY_SLOTS] =
{
	"INVENTORY",
	"HAND",
	"BOW",
	"AMMO",
	"RING",
	"AMULET",
	"BODY",
	"OUTER",
	"HEAD",
	"ARMS",
	"FEET",
	"INSTRUMENT",
	"LITE",
	"TAIL",
};

// Object Kind tval Flags
static cptr tval_flags[255] =
{
	"NONE",

	"SKELETON",
	"BOTTLE",
	"JUNK",
	"WHISTLE",
	"SPIKE",
	"XXX",
	"CHEST",
	"FIGURINE",
	"STATUE",
	"CORPSE",

	"CAPTURE",
	"XXX",
	"XXX",
	"XXX",
	"NO_AMMO",
	"SHOT",
	"ARROW",
	"BOLT",
	"BOW",
	"DIGGING",

	"HAFTED",
	"POLEARM",
	"SWORD",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"BOOTS",

	"GLOVES",
	"HELM",
	"CROWN",
	"SHIELD",
	"CLOAK",
	"SOFT_ARMOR",
	"HARD_ARMOR",
	"DRAG_ARMOR",
	"LITE",
	"AMULET",

	"TAIL",
	"INSTRUMENT",
	"XXX",
	"XXX",
	"RING",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"CARD",

	"WHEEL",
	"XXX",
	"XXX",
	"XXX",
	"STAFF",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",

	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"WAND",
	"ROD",
	"XXX",
	"XXX",
	"PARCHMENT",
	"SCROLL",

	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"POTION",
	"XXX",
	"FLASK",
	"XXX",
	"XXX",
	"FOOD",

	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"LIFE_BOOK",

	"SORCERY_BOOK",
	"NATURE_BOOK",
	"CHAOS_BOOK",
	"DEATH_BOOK",
	"TRUMP_BOOK",
	"ARCANE_BOOK",
	"CRAFT_BOOK",
	"DAEMON_BOOK",
	"CRUSADE_BOOK",
	"XXX",

	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"MUSIC_BOOK",
	"HISSATSU_BOOK",
	"HEX_BOOK",
	"XXX",
	"XXX",
	"XXX",

	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",

	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"GOLD",

};

// Trait type
static cptr trait_info_type[] =
{
	"CONSTANT",
	"SELF",
	"TARGET",
	"ADJACENCY",
	"ADJACENCY2",
	"POSTURE",
	"MEAT",
	NULL
};

/*
 * Character Blow Methods
 */
static cptr species_info_blow_method[] =
{
	"",
	"HIT",
	"TOUCH",
	"PUNCH",
	"KICK",
	"CLAW",
	"BITE",
	"STING",
	"SLASH",
	"BUTT",
	"CRUSH",
	"CHARGE",
	"CRAWL",
	"DROOL",
	"SPIT",
	"XXX4",
	"BEG",
	"INSULT",
	"MOAN",
	"SHOW",
	"SHOOT",
	"ENGULF",
	"EXPLODE",
	"GAZE",
	"WAIL",
	"SPORE",
	NULL
};


/*
 * Creature Blow Effects
 */
static cptr species_info_blow_effect[] =
{
	"",
	"HURT",
	"POISON",
	"UN_BONUS",
	"UN_POWER",
	"EAT_GOLD",
	"EAT_ITEM",
	"EAT_FOOD",
	"EAT_LITE",
	"ACID",
	"ELEC",
	"FIRE",
	"COLD",
	"BLIND",
	"CONFUSE",
	"TERRIFY",
	"PARALYZE",
	"LOSE_STR",
	"LOSE_INT",
	"LOSE_WIS",
	"LOSE_DEX",
	"LOSE_CON",
	"LOSE_CHR",
	"LOSE_ALL",
	"SHATTER",
	"EXP_10",
	"EXP_20",
	"EXP_40",
	"EXP_80",
	"DISEASE",
	"TIME",
	"EXP_VAMP",
	"DR_MANA",
	"SUPERHURT",
	NULL
};


/*
 * Feature info flags
 */
static cptr feature_info_flags[] =
{
	"LOS",
	"PROJECT",
	"MOVE",
	"PLACE",
	"DROP",
	"SECRET",
	"NOTICE",
	"REMEMBER",
	"OPEN",
	"CLOSE",
	"BASH",
	"SPIKE",
	"DISARM",
	"STORE",
	"TUNNEL",
	"MAY_HAVE_GOLD",
	"HAS_GOLD",
	"HAS_ITEM",
	"DOOR",
	"TRAP",
	"STAIRS",
	"GLYPH",
	"LESS",
	"MORE",
	"AVOID_RUN",
	"FLOOR",
	"WALL",
	"PERMANENT",
	"XXX00",
	"XXX01",
	"XXX02",
	"HIT_TRAP",

	"BRIDGE",
	"RIVER",
	"LAKE",
	"BRIDGED",
	"COVERED",
	"GLOW",
	"ENSECRET",
	"WATER",
	"LAVA",
	"SHALLOW",
	"DEEP",
	"FILLED",
	"HURT_ROCK",
	"HURT_FIRE",
	"HURT_COLD",
	"HURT_ACID",
	"ICE",
	"ACID",
	"OIL",
	"XXX04",
	"CAN_CLIMB",
	"CAN_FLY",
	"CAN_SWIM",
	"CAN_PASS",
	"CAN_OOZE",
	"CAN_DIG",
	"HIDE_ITEM",
	"HIDE_SNEAK",
	"HIDE_SWIM",
	"HIDE_DIG",
	"KILL_HUGE",
	"KILL_MOVE",

	"PICK_TRAP",
	"PICK_DOOR",
	"ALLOC",
	"CHEST",
	"DROP_1D2",
	"DROP_2D2",
	"DROP_GOOD",
	"DROP_GREAT",
	"HURT_POIS",
	"HURT_ELEC",
	"HURT_WATER",
	"HURT_BWATER",
	"USE_FEAT",
	"GET_FEAT",
	"GROUND",
	"OUTSIDE",
	"EASY_HIDE",
	"EASY_CLIMB",
	"MUST_CLIMB",
	"TREE",
	"NEED_TREE",
	"BLOOD",
	"DUST",
	"SLIME",
	"PLANT",
	"XXX2",
	"INSTANT",
	"EXPLODE",
	"TIMED",
	"ERUPT",
	"STRIKE",
	"SPREAD",

	"SPECIAL",
	"HURT_DISI",
	"QUEST_ENTER",
	"QUEST_EXIT",
	"QUEST",
	"SHAFT",
	"MOUNTAIN",
	"BLDG",
	"MINOR_GLYPH",
	"PATTERN",
	"TOWN",
	"ENTRANCE",
	"MIRROR",
	"UNPERM",
	"TELEPORTABLE",
	"CONVERT",
	"GLASS",
	
	"CHAOS_TAINTED",
	"POISON_SWAMP",
	"ACID_SWAMP",
};


/*
 * Creature authority flags
 */
static cptr authority_str_id;

/*
 * Dungeon flags
 */
static cptr dungeon_info_flags1[] =
{
	"WINNER",
	"MAZE",
	"SMALLEST",
	"BEGINNER",
	"BIG",
	"NO_DOORS",
	"WATER_RIVER",
	"LAVA_RIVER",
	"CURTAIN",
	"GLASS_DOOR",
	"CAVE",
	"CAVERN",
	"POISON_RIVER",
	"ACID_RIVER",
	"XXX",
	"XXX",
	"FORGET",
	"LAKE_WATER",
	"LAKE_LAVA",
	"LAKE_RUBBLE",
	"LAKE_TREE",
	"NO_VAULT",
	"ARENA",
	"DESTROY",
	"GLASS_ROOM",
	"NO_CAVE",
	"NO_MAGIC",
	"NO_MELEE",
	"CHAMELEON",
	"DARKNESS",
	"LAKE_POISON",
	"LAKE_ACID"
};


/*
 *  Store Flags
 */
static cptr store_pre_info_flags[] =
{
	"YOUR_HOME",
	"MUSEUM",
	"BLACK_MARKET",
	"GENERAL",
	"SCULPTURE",
	"PAINT",
	"ARMOURY",
	"WEAPON",

	"ALCHEMY",
	"MAGIC_ITEM",
	"ORIENTAL_WEAPON",
	"BOOK",
	"TEMPLE",
	"XXX",
	"XXX",
	"XXX",

	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"XXX",

	"XXX",
	"XXX",
	"XXX",
	"XXX",
	"OTAKU",
	"MORGUE",
	"XXX",
	"XXX",
};


static parse_special_melee(special_blow_type *blow_ptr, char *tmp)
{
	int offset = 0;
	int num = strlen(tmp);
	int k = 0, am, ae, n1, n2;
	char *s;
	char atk_method[30], atk_effect[30];

	while(offset < num)
	{
		if(k == MAX_SPECIAL_BLOWS) return PARSE_ERROR_GENERIC;

		for(s = &tmp[offset]; *s != '\n' && *s != '\0'; s++);
		*s = '\0';

		n1 = n2 = 0;
		atk_method[0] = '\0';
		atk_effect[0] = '\0';

		if(sscanf(tmp + offset, "%[^:]:%[^:]:%dd%d", atk_method, atk_effect, &n1, &n2) != 4) {
			if(sscanf(tmp + offset, "%[^:\n]:%[^:]", atk_method, atk_effect) != 2) {
				if(sscanf(tmp + offset, "%[^:]", atk_method) != 1)
					return PARSE_ERROR_GENERIC;
			}
		}

		/* Analyze the method */
		for (am = 0; species_info_blow_method[am]; am++) if(streq(atk_method, species_info_blow_method[am])) break;
		if(!species_info_blow_method[am])
			return PARSE_ERROR_GENERIC;

		/* Analyze effect */
		for (ae = 0; species_info_blow_effect[ae]; ae++) if(streq(atk_effect, species_info_blow_effect[ae])) break;
		if(!species_info_blow_effect[ae])
			return PARSE_ERROR_GENERIC;

		/* Save the method */
		blow_ptr[k].method = am;

		/* Save the effect */
		blow_ptr[k].effect = ae;

		/* Extract the damage dice and sides */
		blow_ptr[k].d_dice = n1;
		blow_ptr[k].d_side = n2;

		k++;

		while(tmp[offset]) offset++;
		offset++;
	}

	return PARSE_ERROR_NONE;
}

/*
 * Add a text to the text-storage and store offset to it.
 *
 * Returns FALSE when there isn't enough space available to store
 * the text.
 */
static bool add_text(u32b *offset, header *head, cptr buf, bool normal_text)
{
	/* Hack -- Verify space */
	if(head->text_size + strlen(buf) + 8 > FAKE_TEXT_SIZE)
		return (FALSE);

	/* New text? */
	if(*offset == 0)
	{
		/* Advance and save the text index */
		*offset = ++head->text_size;
	}

	/* Additional text */
	else if(normal_text)
	{
		/*
		 * If neither the end of the last line nor
		 * the beginning of current line is not a space,
		 * fill up a space as a correct separator of two words.
		 */
		if(head->text_size > 0 &&
#ifdef JP
		    (*(head->text_ptr + head->text_size - 1) != ' ') &&
		    ((head->text_size == 1) || !iskanji(*(head->text_ptr + head->text_size - 2))) && 
		    (buf[0] != ' ') && !iskanji(buf[0])
#else
		    (*(head->text_ptr + head->text_size - 1) != ' ') &&
		    (buf[0] != ' ')
#endif
		    )
		{
			/* Append a space */
			*(head->text_ptr + head->text_size) = ' ';

			/* Advance the index */
			head->text_size++;
		}
	}

	/* Append chars to the text */
	strcpy(head->text_ptr + head->text_size, buf);

	/* Advance the index */
	head->text_size += strlen(buf);

	/* Success */
	return (TRUE);
}


/*
 * Add a name to the name-storage and return an offset to it.
 *
 * Returns FALSE when there isn't enough space available to store
 * the name.
 */
static bool add_name(u32b *offset, header *head, cptr buf)
{
	/* Hack -- Verify space */
	if(head->name_size + strlen(buf) + 8 > FAKE_NAME_SIZE)
		return (FALSE);

	/* New name? */
	if(*offset == 0)
	{
		/* Advance and save the name index */
		*offset = ++head->name_size;
	}

	/* Append chars to the names */
	strcpy(head->name_ptr + head->name_size, buf);

	/* Advance the index */
	head->name_size += strlen(buf);

	/* Success */
	return (TRUE);
}

// Deskull
// Add temporary buffer for reprocess
static bool add_tmp(u32b *offset, header *head, cptr buf)
{
	/* Hack -- Verify space */
	if(head->tmp_size + strlen(buf) + 8 > FAKE_TMP_BUFFER_SIZE)
		return (FALSE);

	/* New name? */
	if(*offset == 0)
	{
		/* Advance and save the name index */
		*offset = ++head->tmp_size;
	}

	/* Append chars to the names */
	strcpy(head->tmp_ptr + head->tmp_size, buf);

	/* Advance the index */
	head->tmp_size += strlen(buf);

	/* Success */
	return (TRUE);
}



/*
 * Add a tag to the tag-storage and return an offset to it.
 *
 * Returns FALSE when there isn't enough space available to store
 * the name.
 */
static bool add_tag(s16b *offset, header *head, cptr buf)
{
	u32b i;

	/* Search for an existing (fake) tag */
	for (i = 1; i < head->tag_size; i += strlen(&head->tag_ptr[i]) + 1)
	{
		/* Found it */
		if(streq(&head->tag_ptr[i], buf)) break;
	}

	/* There was no existing tag */
	if(i >= head->tag_size)
	{
		/* Hack -- Verify space */
		if(head->tag_size + strlen(buf) + 8 > FAKE_TAG_SIZE)
			return FALSE;

		/* Append chars to the tags */
		strcpy(head->tag_ptr + head->tag_size, buf);

		/* Point the new tag */
		i = head->tag_size;

		/* Advance the index */
		head->tag_size += strlen(buf) + 1;
	}

	/* Return offset of the tag */
	*offset = (s16b)i;

	/* Success */
	return TRUE;
}


/*
 * Convert a "color letter" into an "actual" color
 * The colors are: dwsorgbuDWvyRGBU, as shown below
 */
byte color_char_to_acttr(char c)
{
	switch (c)
	{
		case 'd': return (TERM_DARK);
		case 'w': return (TERM_WHITE);
		case 's': return (TERM_SLATE);
		case 'o': return (TERM_ORANGE);
		case 'r': return (TERM_RED);
		case 'g': return (TERM_GREEN);
		case 'b': return (TERM_BLUE);
		case 'u': return (TERM_UMBER);

		case 'D': return (TERM_L_DARK);
		case 'W': return (TERM_L_WHITE);
		case 'v': return (TERM_VIOLET);
		case 'y': return (TERM_YELLOW);
		case 'R': return (TERM_L_RED);
		case 'G': return (TERM_L_GREEN);
		case 'B': return (TERM_L_BLUE);
		case 'U': return (TERM_L_UMBER);
	}

	return (255);
}



/*** Initialize from ascii template files ***/


/*
 * Initialize an "*_info" array, by parsing an ascii "template" file
 */
errr init_info_txt(FILE *fp, char *buf, header *head, parse_info_txt_func parse_info_txt_line)
{
	errr err;

	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = 0;


	/* Prepare the "fake" stuff */
	head->name_size = 0;
	head->text_size = 0;
	head->tag_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if(!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if(buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if(buf[0] == 'V')
		{
			/* ignore */
			continue;
		}

		/* Mega Hack -- Calculate Check Sum */
		if(buf[0] != 'N' && buf[0] != 'D')
		{
			int i;
			for (i = 0; buf[i]; i++)
			{
				head->v_extra += (byte)buf[i];
				head->v_extra ^= (1 << (i % 8));
			}
		}

		/* Parse the line */
		if((err = (*parse_info_txt_line)(buf, head)) != 0)
			return (err);
	}


	/* Complete the "name" and "text" sizes */
	if(head->name_size) head->name_size++;
	if(head->text_size) head->text_size++;

	/* Success */
	return PARSE_ERROR_NONE;
}



/*
 * Initialize an "*_info" array, by parsing an ascii "template" file
 */
errr init_info_csv(FILE *fp, char *buf, header *head, parse_info_txt_func parse_info_txt_line)
{
	errr err;
	char nt[80];
	int i = 0;

	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = 0;

	/* Prepare the "fake" stuff */
	head->name_size = 0;
	head->text_size = 0;
	head->tag_size = 0;

	/* Parse */
	while (0 == my_fgets_csv(fp, buf, 65536, '"'))
	{
		// Skip comments and blank lines
		if(!buf[0] || (buf[0] == '#')) continue;

		i++;

		// Parse the line
		if((err = (*parse_info_txt_line)(buf, head)) != 0)
			return (err);
	}

	sprintf(nt, "CSV Line:%d", i);
	note(nt);
	/* Complete the "name" and "text" sizes */
	if(head->name_size) head->name_size++;
	if(head->text_size) head->text_size++;

	/* Success */
	return PARSE_ERROR_NONE;
}




/*
 * Initialize the "vault_info" array, by parsing an ascii "template" file
 */
errr parse_vault_info(char *buf, header *head)
{
	int i;
	char *s;

	/* Current entry */
	static vault_type *v_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if(buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

		/* Verify that colon */
		if(!s) return PARSE_ERROR_GENERIC;

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if(!*s) return PARSE_ERROR_GENERIC;

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if(i <= error_idx) return (4);

		/* Verify information */
		if(i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		v_ptr = &vault_info[i];

		/* Store the name */
		if(!add_name(&v_ptr->name, head, s)) return PARSE_ERROR_OUT_OF_MEMORY;
	}

	/* There better be a current v_ptr */
	else if(!v_ptr) return (3);

	/* Process 'D' for "Description" */
	else if(buf[0] == 'D')
	{
		/* Acquire the text */
		s = buf+2;

		/* Store the text */
		if(!add_text(&v_ptr->text, head, s, FALSE)) return PARSE_ERROR_OUT_OF_MEMORY;
	}

	/* Process 'X' for "Extra info" (one line only) */
	else if(buf[0] == 'X')
	{
		int typ, rat, hgt, wid;

		/* Scan for the values */
		if(4 != sscanf(buf+2, "%d:%d:%d:%d",
			&typ, &rat, &hgt, &wid)) return PARSE_ERROR_GENERIC;

		/* Save the values */
		v_ptr->typ = typ;
		v_ptr->rat = rat;
		v_ptr->hgt = hgt;
		v_ptr->wid = wid;
	}

	/* Oops */
	else	return (6);

	/* Success */
	return PARSE_ERROR_NONE;
}



/*
 * Initialize the "skill_info" array, by parsing an ascii "template" file
 */
errr parse_skill_info(char *buf, header *head)
{
	int i;

	/* Current entry */
	static skill_table *s_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if(buf[0] == 'N')
	{
		/* Get the index */
		i = atoi(buf+2);

			/* Verify information */
		if(i <= error_idx) return (4);

		/* Verify information */
		if(i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		s_ptr = &skill_info[i];
	}

	/* There better be a current s_ptr */
	else if(!s_ptr) return (3);

	/* Process 'W' for "Weapon exp" */
	else if(buf[0] == 'W')
	{
		int tval, sval, start, max;
		const s16b exp_conv_table[] =
		{
			WEAPON_EXP_UNSKILLED, WEAPON_EXP_BEGINNER, WEAPON_EXP_SKILLED,
			WEAPON_EXP_EXPERT, WEAPON_EXP_MASTER
		};

		/* Scan for the values */
		if(4 != sscanf(buf+2, "%d:%d:%d:%d",
				&tval, &sval, &start, &max)) return PARSE_ERROR_GENERIC;

		if(start < EXP_LEVEL_UNSKILLED || start > EXP_LEVEL_MASTER
			|| max < EXP_LEVEL_UNSKILLED || max > EXP_LEVEL_MASTER) return (8);

		/* Save the values */
		//s_ptr->w_start[tval][sval] = exp_conv_table[start];
		//s_ptr->w_max[tval][sval] = exp_conv_table[max];
	}

	/* Process 'S' for "Skill exp" */
	else if(buf[0] == 'S')
	{
		int num, start, max;

		/* Scan for the values */
		if(3 != sscanf(buf+2, "%d:%d:%d",
				&num, &start, &max)) return PARSE_ERROR_GENERIC;

		if(start < WEAPON_EXP_UNSKILLED || start > WEAPON_EXP_MASTER
			|| max < WEAPON_EXP_UNSKILLED || max > WEAPON_EXP_MASTER) return (8);

		/* Save the values */
		s_ptr->s_start[num] = start;
		s_ptr->s_max[num] = max;
	}


	/* Oops */
	else return (6);

	/* Success */
	return PARSE_ERROR_NONE;
}


/*
 * Initialize the "magic_info" array, by parsing an ascii "template" file
 */
errr parse_magic_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static magic_table_type *m_ptr = NULL;

	/* ---Hack--- */
	static int realm, magic_idx = 0, readable = 0;


	/* Process 'N' for "New/Number/Name" */
	if(buf[0] == 'N')
	{
		/* Get the index */
		i = atoi(buf+2);

			/* Verify information */
		if(i <= error_idx) return (4);

		/* Verify information */
		if(i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		m_ptr = &magic_info[i];
	}

	/* There better be a current m_ptr */
	else if(!m_ptr) return (3);

	/* Process 'I' for "Info" (one line only) */
	else if(buf[0] == 'I')
	{
		char *book, *stat;
		int xtra, type, first, weight;

		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

		/* Verify that colon */
		if(!s) return PARSE_ERROR_GENERIC;

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		book = buf+2;

		if(streq(book, "SORCERY")) m_ptr->spell_book = TV_SORCERY_BOOK;
		else if(streq(book, "LIFE")) m_ptr->spell_book = TV_LIFE_BOOK;
		else if(streq(book, "MUSIC")) m_ptr->spell_book = TV_MUSIC_BOOK;
		else if(streq(book, "HISSATSU")) m_ptr->spell_book = TV_HISSATSU_BOOK;
		else if(streq(book, "NONE")) m_ptr->spell_book = 0;
		else return PARSE_ERROR_INVALID_FLAG;

		stat = s;

		/* Find the colon before the name */
		s = my_strchr(s, ':');

		/* Verify that colon */
		if(!s) return PARSE_ERROR_GENERIC;

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		if(streq(stat, "STR")) m_ptr->spell_stat = STAT_STR;
		else if(streq(stat, "INT")) m_ptr->spell_stat = STAT_INT;
		else if(streq(stat, "WIS")) m_ptr->spell_stat = STAT_WIS;
		else if(streq(stat, "DEX")) m_ptr->spell_stat = STAT_DEX;
		else if(streq(stat, "CON")) m_ptr->spell_stat = STAT_CON;
		else if(streq(stat, "CHR")) m_ptr->spell_stat = STAT_CHA;
		else return PARSE_ERROR_INVALID_FLAG;


		/* Scan for the values */
		if(4 != sscanf(s, "%x:%d:%d:%d",
				(uint *)&xtra, &type, &first, &weight))	return PARSE_ERROR_GENERIC;

		m_ptr->spell_xtra = xtra;
		m_ptr->spell_type = type;
		m_ptr->spell_first = first;
		m_ptr->spell_weight = weight;
	}


	/* Process 'R' for "Realm" (one line only) */
	else if(buf[0] == 'R')
	{
		/* Scan for the values */
		if(2 != sscanf(buf+2, "%d:%d",
				&realm, &readable)) return PARSE_ERROR_GENERIC;

		magic_idx = 0;
	}

	else if(buf[0] == 'T')
	{
		int level, mana, fail, exp;

		if(!readable) return PARSE_ERROR_GENERIC;
		/* Scan for the values */
		if(4 != sscanf(buf+2, "%d:%d:%d:%d",
				&level, &mana, &fail, &exp)) return PARSE_ERROR_GENERIC;

		m_ptr->info[realm][magic_idx].slevel = level;
		m_ptr->info[realm][magic_idx].smana = mana;
		m_ptr->info[realm][magic_idx].sfail = fail;
		m_ptr->info[realm][magic_idx].sexp = exp;
		magic_idx ++;
	}


	/* Oops */
	else return (6);

	/* Success */
	return PARSE_ERROR_NONE;
}


/*
 * Grab one flag from a textual string
 */
static errr grab_one_flag(u32b *flags, cptr names[], cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < 32; i++)
	{
		if(streq(what, names[i]))
		{
			*flags |= (1L << i);
			return PARSE_ERROR_NONE;
		}
	}

	return -1;
}

/*
 * Grab index from a textual string
 */
static errr grab_one_index(int *n, cptr names[], cptr what, bool common_none)
{
	int i = 0;

	if(streq(what, "VARIABLE"))
	{
		*n = INDEX_VARIABLE;
		return PARSE_ERROR_NONE;
	}
	else if(streq(what, "NONE") && common_none)
	{
		*n = INDEX_NONE;
		return PARSE_ERROR_NONE;
	}

	if(names == NULL) return -1;

	/* Check flags */
	for (; names[i]; i++)
	{
		if(streq(what, names[i]))
		{
			*n = i;
			return PARSE_ERROR_NONE;
		}
	}

	return -1;
}

static errr traits_precondition_splits(traits_precondition *flags_pre_ptr, char *tmp)
{
	char flagname[80];
	char flag_aux[80];
	char *s, *t;
	int b, c, prob;


	for (s = tmp; *s; )
	{

		for (t = s; *t && (*t != ' ') && (*t != '\n') && (*t != '|'); ++t);

		/* Nuke and skip any dividers */
		if(*t)
		{
			*t++ = '\0';
			while (*t == ' ' || *t == '|' || *t == '\n') t++;
		}

		if(sscanf(s, "%[^:]:%d:%d", &flag_aux, &b, &c) != 3)
		{
			if(sscanf(s, "%[^:]:%d", &flag_aux, &b) == 2)
			{
				c = PY_MAX_LEVEL + 1;
			}
			else if(sscanf(s, "%s", &flag_aux) == 1)
			{
				b = 1;
				c = PY_MAX_LEVEL + 1;
			}
			else
				return PARSE_ERROR_GENERIC;
		}

		if(sscanf(flag_aux, "%[^[][%d%]", &flagname, &prob) != 2)
		{
			if(sscanf(flag_aux, "%s", &flagname) != 1)
				return PARSE_ERROR_GENERIC;
			prob = 100;
		}

		/* Parse this entry */
		if(grab_one_trait(flags_pre_ptr, flagname, (byte)b, (byte)c, (byte)prob) != 0)
			return (PARSE_ERROR_INVALID_FLAG);

		/* Start the next entry */
		s = t;
	}

	return PARSE_ERROR_NONE;
}

static errr grab_one_trait(traits_precondition *cf_ptr, cptr what, byte add, byte remove, byte prob)
{
	int i;

	/* Check flags */
	for (i = 0; i < MAX_TRAITS; i++)
	{
		if(streq(what, trait_info[i].id2))
		{
			cf_ptr->add_lev[i] = add;
			cf_ptr->remove_lev[i] = remove;
			cf_ptr->probability[i] = prob;
			return PARSE_ERROR_NONE;
		}
	}

	return -1;
}

/*
 * Grab one flag in an feature_type from a textual string
 */
static errr grab_one_feat_flag(feature_type *f_ptr, cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < FF_FLAG_MAX; i++)
	{
		if(streq(what, feature_info_flags[i]))
		{
			add_flag(f_ptr->flags, i);
			return PARSE_ERROR_NONE;
		}
	}

	/* Oops */
#ifdef JP
	msg_format("未知の地形フラグ '%s'。", what);
#else
	msg_format("Unknown feature flag '%s'.", what);
#endif

	/* Error */
	return PARSE_ERROR_GENERIC;
}



/*
 * Grab an action in an feature_type from a textual string
 */
static errr grab_one_feat_action(feature_type *f_ptr, cptr what, int count)
{
	int i;

	/* Check flags */
	for (i = 0; i < FF_FLAG_MAX; i++)
	{
		if(streq(what, feature_info_flags[i]))
		{
			f_ptr->state[count].action = i;
			return PARSE_ERROR_NONE;
		}
	}

	/* Oops */
#ifdef JP
	msg_format("未知の地形アクション '%s'。", what);
#else
	msg_format("Unknown feature action '%s'.", what);
#endif

	/* Error */
	return PARSE_ERROR_GENERIC;
}


/*
 * Initialize the "feature_info" array, by parsing an ascii "template" file
 */
errr parse_feature_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static feature_type *f_ptr = NULL;


	/* Process 'N' for "New/Number/Name" */
	if(buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

		if(s)
		{
			/* Nuke the colon, advance to the name */
			*s++ = '\0';
		}

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if(i <= error_idx) return (4);

		/* Verify information */
		if(i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		f_ptr = &feature_info[i];

		/* Tag name is given */
		if(s)
		{
			/* Store the tag */
			if(!add_tag(&f_ptr->tag, head, s)) return PARSE_ERROR_OUT_OF_MEMORY;
		}

		/* Default "mimic" */
		f_ptr->mimic = i;

		/* Default "destroyed state" -- if not specified */
		f_ptr->destroyed = i;

		/* Default "states" */
		for (i = 0; i < MAX_FEAT_STATES; i++) f_ptr->state[i].action = FF_FLAG_MAX;
	}

	/* There better be a current f_ptr */
	else if(!f_ptr) return (3);

#ifdef JP
	else if(buf[0] == 'J')
	{
		/* Store the name */
		if(!add_name(&f_ptr->name, head, buf+2)) return PARSE_ERROR_OUT_OF_MEMORY;
	}

	else if(buf[0] == 'E')
	{
		/* Ignore english name */
	}
#else
	else if(buf[0] == 'J')
	{
		/* Ignore Japanese name */
	}

	else if(buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if(!add_name(&f_ptr->name, head, s)) return PARSE_ERROR_OUT_OF_MEMORY;
	}
#endif


	/* Process 'M' for "Mimic" (one line only) */
	else if(buf[0] == 'M')
	{
		s16b offset;

		if(!add_tag(&offset, head, buf + 2)) return PARSE_ERROR_OUT_OF_MEMORY;

		/* Record a fake tag index */
		f_ptr->mimic = -offset;
	}


	/* Process 'G' for "Graphics" (one line only) */
	else if(buf[0] == 'G')
	{
		int j;
		byte s_attr;
		char char_tmp[F_LIT_MAX];

		/* Paranoia */
		if(buf[1] != ':') return PARSE_ERROR_GENERIC;
		if(!buf[2]) return PARSE_ERROR_GENERIC;
		if(buf[3] != ':') return PARSE_ERROR_GENERIC;
		if(!buf[4]) return PARSE_ERROR_GENERIC;

		/* Extract the char */
		char_tmp[F_LIT_STANDARD] = buf[2];

		/* Extract the color */
		s_attr = color_char_to_acttr(buf[4]);

		/* Paranoia */
		if(s_attr > 127) return PARSE_ERROR_GENERIC;

		/* Save the standard values */
		f_ptr->d_attr[F_LIT_STANDARD] = s_attr;
		f_ptr->d_char[F_LIT_STANDARD] = char_tmp[F_LIT_STANDARD];

		/* Is this feature supports lighting? */
		if(buf[5] == ':')
		{
			/* G:c:a:LIT (default) */
			apply_default_feat_lighting(f_ptr->d_attr, f_ptr->d_char);

			/* G:c:a:lc:la:dc:da */
			if(!streq(buf + 6, "LIT"))
			{
				char attr_lite_tmp[F_LIT_MAX - F_LIT_NS_BEGIN];

				if((F_LIT_MAX - F_LIT_NS_BEGIN) * 2 != sscanf(buf + 6, "%c:%c:%c:%c",
					&char_tmp[F_LIT_LITE], &attr_lite_tmp[F_LIT_LITE - F_LIT_NS_BEGIN],
					&char_tmp[F_LIT_DARK], &attr_lite_tmp[F_LIT_DARK - F_LIT_NS_BEGIN])) return 1;
				if(buf[F_LIT_MAX * 4 + 1]) return 1;

				for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
				{
					switch (attr_lite_tmp[j - F_LIT_NS_BEGIN])
					{
					case '*':
						/* Use default lighting */
						break;
					case '-':
						/* No lighting support */
						f_ptr->d_attr[j] = f_ptr->d_attr[F_LIT_STANDARD];
						break;
					default:
						/* Extract the color */
						f_ptr->d_attr[j] = color_char_to_acttr(attr_lite_tmp[j - F_LIT_NS_BEGIN]);
						if(f_ptr->d_attr[j] > 127) return 1;
						break;
					}
					f_ptr->d_char[j] = char_tmp[j];
				}
			}
		}
		else if(!buf[5])
		{
			for (j = F_LIT_NS_BEGIN; j < F_LIT_MAX; j++)
			{
				f_ptr->d_attr[j] = s_attr;
				f_ptr->d_char[j] = char_tmp[F_LIT_STANDARD];
			}
		}
		else return 1;
	}

	/* Hack -- Process 'F' for flags */
	else if(buf[0] == 'F')
	{
		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if(*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* XXX XXX XXX Hack -- Read feature subtype */
			if(1 == sscanf(s, "SUBTYPE_%d", &i))
			{
				/* Extract a "subtype" */
				f_ptr->subtype =  i;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* XXX XXX XXX Hack -- Read feature power */
			if(1 == sscanf(s, "POWER_%d", &i))
			{
				/* Extract a "power" */
				f_ptr->power =  i;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* Parse this entry */
			if(0 != grab_one_feat_flag(f_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'W' for "More Info" (one line only) */
	else if(buf[0] == 'W')
	{
		int priority;

		/* Scan for the value */
		if(1 != sscanf(buf+2, "%d", &priority)) return (PARSE_ERROR_GENERIC);

		/* Save the value */
		f_ptr->priority = priority;
	}

	/* Process 'K' for "States" (up to four lines + default (which cannot be last)) */
	else if(buf[0] == 'K')
	{
		s16b offset;

		/* Find the next empty state slot (if any) */
		for (i = 0; i < MAX_FEAT_STATES; i++) if(f_ptr->state[i].action == FF_FLAG_MAX) break;

		/* Oops, no more slots */
		if(i == MAX_FEAT_STATES) return PARSE_ERROR_GENERIC;

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if(*t == ':') *t++ = '\0';

		/* Is this default entry? */
		if(streq(s, "DESTROYED"))
		{
			if(!add_tag(&offset, head, t)) return PARSE_ERROR_OUT_OF_MEMORY;

			/* Record a fake tag index */
			f_ptr->destroyed = -offset;
		}
		else
		{
			/* Reset */
			f_ptr->state[i].action = 0;

			/* Parse this entry */
			if(0 != grab_one_feat_action(f_ptr, s, i)) return PARSE_ERROR_INVALID_FLAG;

			if(!add_tag(&offset, head, t)) return PARSE_ERROR_OUT_OF_MEMORY;

			/* Record a fake tag index */
			f_ptr->state[i].result = -offset;
		}
	}

	/* Oops */
	else	return (6);

	/* Success */
	return PARSE_ERROR_NONE;
}


/*
 * Convert a fake tag to a real feat index
 */
s16b feature_tag_to_index(cptr str)
{
	u16b i;

	/* Search for real index corresponding to this fake tag */
	for (i = 0; i < f_head.info_num; i++)
	{
		if(streq(feature_tag + feature_info[i].tag, str))
		{
			/* Return the index */
			return (s16b)i;
		}
	}

	/* Not found */
	return -1;
}


/*
 * Search for real index corresponding to this fake tag
 */
static void search_real_feat(s16b *feat)
{
	int i;

	/* Don't convert non-fake tag */
	if(*feat >= 0) return;

	/* Search for real index corresponding to this fake tag */
	for (i = 0; i < f_head.info_num; i++)
	{
		if((-(*feat)) == feature_info[i].tag)
		{
			/* Record real index */
			*feat = (s16b)i;
			return;
		}
	}

	/* Undefined tag */
#ifdef JP
	msg_format("未定義のタグ '%s'。", feature_tag + (-(*feat)));
#else
	msg_format("%s is undefined.", feature_tag + (-(*feat)));
#endif
}


/*
 * Retouch fake tags of feature_info
 */
void retouch_feature_info(header *head)
{
	int i;

	/* Convert fake tags to real feat indices */
	for (i = 0; i < head->info_num; i++)
	{
		feature_type *f_ptr = &feature_info[i];
		int j;

		search_real_feat(&f_ptr->mimic);

		search_real_feat(&f_ptr->destroyed);

		for (j = 0; j < MAX_FEAT_STATES; j++) search_real_feat(&f_ptr->state[j].result);
	}
}


enum OBJECT_KIND_INFO {
	OK_INFO_ID,
	OK_INFO_NAME,
	OK_INFO_UI_NAME,
	OK_INFO_E_NAME,
	OK_INFO_E_UI_NAME,

	OK_INFO_SYMBOL,
	OK_INFO_COLOR,
	OK_INFO_TVAL,
	OK_INFO_SVAL,
	OK_INFO_PVAL,

	OK_INFO_STR,
	OK_INFO_INT,
	OK_INFO_WIS,
	OK_INFO_DEX,
	OK_INFO_CON,

	OK_INFO_CHA,
	OK_INFO_SEARCH,
	OK_INFO_STEALTH,
	OK_INFO_MAGIC_MASTERY,
	OK_INFO_SPEED,
	OK_INFO_DEPTH,
	OK_INFO_RARITY,
	OK_INFO_WEIGHT,
	OK_INFO_COST,

	OK_INFO_CHARGE,
	OK_INFO_BASE_AC,
	OK_INFO_BASE_EV,
	OK_INFO_BASE_DAMAGE,
	OK_INFO_PLUS_HIT,

	OK_INFO_PLUS_DAM,
	OK_INFO_PLUS_AC,
	OK_INFO_PLUS_EV,
	OK_INFO_BOW_MUL,
	OK_INFO_ADD_DEPTH_RARITY,

	OK_INFO_FLAGS,
	OK_INFO_DESCRIPTION,
	OK_INFO_E_DESCRIPTION,
	OK_INFO_COMMENT,
	OK_INFO_MIN_SIZE,

	OK_INFO_MAX_SIZE,
	OK_INFO_SLOT,
	OK_INFO_AP_RATE,
	OBJECT_KIND_INFO_CSV_COLUMNS
};

static cptr object_kind_info_csv_list[OBJECT_KIND_INFO_CSV_COLUMNS] =
{
	"ID",
	"NAME",
	"UI_NAME",
	"E_NAME",
	"E_UI_NAME",

	"SYMBOL",
	"COLOR",
	"TVAL",
	"SVAL",
	"PVAL",

	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",

	"CHA",
	"SRH",
	"STL",
	"MMS",
	"SPD",
	"DEPTH",
	"RARITY",
	"WEIGHT",
	"COST",

	"CHARGE",
	"BASE_AC",
	"BASE_EV",
	"BASE_DAMAGE",
	"PLUS_HIT",

	"PLUS_DAM",
	"PLUS_AC",
	"PLUS_EV",
	"BOW_MUL",
	"ADD_DEPTH_RARITY",

	"FLAGS",
	"DESCRIPTION",
	"E_DESCRIPTION",
	"COMMENT",
	"MIN_SIZE",

	"MAX_SIZE",
	"SLOT",
	"AP_RATE"
};

/*
 * Initialize the "object_kind_info" array, by parsing an ascii "template" file
 */
static enum OBJECT_KIND_INFO object_kind_info_csv_code[OBJECT_KIND_INFO_CSV_COLUMNS];

errr parse_object_kind_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b, c;
	char *s, *t;
	char tmp[10000], nt[80];

	if(get_split_offset(split, size, buf, OBJECT_KIND_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, object_kind_info_csv_list[0]))
	{
		object_kind_info_csv_code[0] = OK_INFO_ID;
		for(i = 1; i < OBJECT_KIND_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < OBJECT_KIND_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, object_kind_info_csv_list[j]))
				{
					object_kind_info_csv_code[i] = j;
					break;
				}
			}
			if(j == OBJECT_KIND_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		object_kind *object_kind_ptr;

		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize Object Kind:%d]", n);

		object_kind_ptr = &object_kind_info[n];

		note(nt);

		for(i = 1; i < OBJECT_KIND_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			

			switch(object_kind_info_csv_code[i])
			{
			case OK_INFO_NAME:
				if(!add_name(&object_kind_ptr->name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case OK_INFO_UI_NAME:
				if(!add_name(&object_kind_ptr->flavospecies_name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case OK_INFO_E_NAME:
				break;

			case OK_INFO_E_UI_NAME:
				break;

			case OK_INFO_SYMBOL:
				object_kind_ptr->d_char = (byte)tmp[0];
				break;

			case OK_INFO_COLOR:
				object_kind_ptr->d_attr = color_char_to_acttr(tmp[0]);
				break;

			case OK_INFO_TVAL:
				for(j = 0; j <= 127; j++)
				{
					if(streq(tval_flags[j], tmp))
					{
						object_kind_ptr->tval = j;
						break;
					}
				}
				if(j > 127)
				{
					if(sscanf(tmp, "%d", &b) == 1)
						object_kind_ptr->tval = (byte)b;
					else
						object_kind_ptr->tval = 0;
				}
				break;

			case OK_INFO_SVAL:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->sval = (byte)b;
				else
					object_kind_ptr->sval = 0;
				break;

			case OK_INFO_PVAL:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->pval = (byte)b;
				else
					object_kind_ptr->pval = 0;
				break;

			case OK_INFO_STR:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->stat_val[STAT_STR] = (byte)b;
				else
					object_kind_ptr->stat_val[STAT_STR] = 0;
				break;

			case OK_INFO_INT:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->stat_val[STAT_INT] = (byte)b;
				else
					object_kind_ptr->stat_val[STAT_INT] = 0;
				break;

			case OK_INFO_WIS:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->stat_val[STAT_WIS] = (byte)b;
				else
					object_kind_ptr->stat_val[STAT_WIS] = 0;
				break;

			case OK_INFO_DEX:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->stat_val[STAT_DEX] = (byte)b;
				else
					object_kind_ptr->stat_val[STAT_DEX] = 0;
				break;

			case OK_INFO_CON:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->stat_val[STAT_CON] = (byte)b;
				else
					object_kind_ptr->stat_val[STAT_CON] = 0;
				break;

			case OK_INFO_CHA:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->stat_val[STAT_CHA] = (byte)b;
				else
					object_kind_ptr->stat_val[STAT_CHA] = 0;
				break;

			case OK_INFO_SEARCH:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->search = (byte)b;
				else
					object_kind_ptr->search = 0;
				break;

			case OK_INFO_STEALTH:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->stealth = (byte)b;
				else
					object_kind_ptr->stealth = 0;
				break;

			case OK_INFO_SPEED:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->speed = (byte)b;
				else
					object_kind_ptr->speed = 0;
				break;

			case OK_INFO_MAGIC_MASTERY:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->magic_mastery = (byte)b;
				else
					object_kind_ptr->magic_mastery = 0;
				break;

			case OK_INFO_DEPTH:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->level = (byte)b;
				else
					object_kind_ptr->level = 0;
				break;

			case OK_INFO_RARITY:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->extra = (byte)b;
				else
					object_kind_ptr->extra = 0;
				break;

			case OK_INFO_WEIGHT:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->weight = (s16b)b;
				else
					object_kind_ptr->weight = 0;
				break;

			case OK_INFO_COST:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->cost = (s32b)b;
				else
					object_kind_ptr->cost = 0;
				break;

			case OK_INFO_BASE_AC:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->ac = (byte)b;
				else
					object_kind_ptr->ac = 0;
				break;

			case OK_INFO_BASE_EV:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->ev = (byte)b;
				else
					object_kind_ptr->ev = 0;
				break;

			case OK_INFO_BASE_DAMAGE:
				if(sscanf(tmp, "%dd%d", &b, &c) == 2)
				{
					object_kind_ptr->dd = (byte)b;
					object_kind_ptr->ds = (byte)c;
				}
				else
				{
					object_kind_ptr->dd = 0;
					object_kind_ptr->ds = 0;
				}
				break;

			case OK_INFO_PLUS_HIT:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->to_hit = (s16b)b;
				else
					object_kind_ptr->to_hit = 0;
				break;

			case OK_INFO_PLUS_DAM:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->to_damage = (s16b)b;
				else
					object_kind_ptr->to_damage = 0;
				break;

			case OK_INFO_PLUS_AC:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->to_ac = (s16b)b;
				else
					object_kind_ptr->to_ac = 0;
				break;

			case OK_INFO_PLUS_EV:
				break;

			case OK_INFO_BOW_MUL:
				break;

			case OK_INFO_ADD_DEPTH_RARITY:
				/* XXX XXX XXX Simply read each number following a colon */
				for (j = 0, s = tmp; s; ++j)
				{
						/* Default chance */
					object_kind_ptr->chance[j] = 1;

						/* Store the attack damage index */
					object_kind_ptr->locale[j] = atoi(s+1);

						/* Find the slash */
					t = my_strchr(s+1, '/');

						/* Find the next colon */
					s = my_strchr(s+1, ':');

						/* If the slash is "nearby", use it */
					if(t && (!s || t < s))
					{
						int chance = atoi(t+1);
						if(chance > 0) object_kind_ptr->chance[j] = chance;
					}
				}
				break;

			case OK_INFO_FLAGS:
				if(!strlen(tmp)) break;
				s = tmp;
				/* Parse every entry textually */
				for (s = tmp; *s;)
				{
						/* Find the end of this entry */
					for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

						/* Nuke and skip any dividers */
					if(*t)
					{
						*t++ = '\0';
						while (*t == ' ' || *t == '|') t++;
					}

					if(0 != traits_precondition_splits(&object_kind_info[n].add_creature_traits, s))
						return PARSE_ERROR_INVALID_FLAG;

						/* Start the next entry */
					s = t;
				}
				break;

			case OK_INFO_DESCRIPTION:
				// Store the text
				if(!add_text(&object_kind_ptr->text, head, tmp, TRUE))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case OK_INFO_E_DESCRIPTION:
				break;

			case OK_INFO_COMMENT:
				break;

			case OK_INFO_MIN_SIZE:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->min_size = (s16b)b;
				else
					object_kind_ptr->min_size = 0;
				break;

			case OK_INFO_MAX_SIZE:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->max_size = (s16b)b;
				else
					object_kind_ptr->max_size = 0;
				break;

			case OK_INFO_SLOT:
				for(j = 0; j < MAX_INVENTORY_SLOTS; j++)
				{
					if(streq(equip_slot_flags[j], tmp))
					{
						object_kind_ptr->slot = j;
						break;
					}
				}
				if(j == MAX_INVENTORY_SLOTS)
					return PARSE_ERROR_GENERIC;
				break;

			case OK_INFO_AP_RATE:
				if(sscanf(tmp, "%d", &b) == 1)
					object_kind_ptr->ap_rate = (s16b)b;
				else
					object_kind_ptr->ap_rate = 100;
				break;

			case OK_INFO_CHARGE:
				if(sscanf(tmp, "%d", &b) == 1)
				{
					object_kind_ptr->charge_const = (s16b)b;
					object_kind_ptr->charge_dice  = 0;
				}
				else if(sscanf(tmp, "%d+d%d", &b, &c) == 2)
				{
					object_kind_ptr->charge_const = (s16b)b;
					object_kind_ptr->charge_dice  = (s16b)c;
				}
				else
				{
					object_kind_ptr->charge_const = 0;
					object_kind_ptr->charge_dice  = 0;
				}
				break;

			default:
				return PARSE_ERROR_GENERIC;

			}
		}
		
	}
	return PARSE_ERROR_NONE;
}


enum ARTIFACT_INFO {
	ARTIFACT_INFO_ID,
	ARTIFACT_INFO_NAME,
	ARTIFACT_INFO_E_NAME,
	ARTIFACT_INFO_TVAL,
	ARTIFACT_INFO_SVAL,
	ARTIFACT_INFO_STR,
	ARTIFACT_INFO_INT,
	ARTIFACT_INFO_WIS,
	ARTIFACT_INFO_DEX,
	ARTIFACT_INFO_CON,
	ARTIFACT_INFO_CHA,
	ARTIFACT_INFO_PVAL,
	ARTIFACT_INFO_DEPTH,
	ARTIFACT_INFO_RARITY,
	ARTIFACT_INFO_WEIGHT,
	ARTIFACT_INFO_COST,
	ARTIFACT_INFO_SIZE_LOWER,
	ARTIFACT_INFO_BASE_AC,
	ARTIFACT_INFO_BASE_EV,
	ARTIFACT_INFO_BASE_DAMAGE,
	ARTIFACT_INFO_PLUS_HIT,
	ARTIFACT_INFO_PLUS_DAM,
	ARTIFACT_INFO_PLUS_AC,
	ARTIFACT_INFO_PLUS_EV,
	ARTIFACT_INFO_FLAGS,
	ARTIFACT_INFO_DESCRIPTION,
	ARTIFACT_INFO_E_DESCRIPTION,
	ARTIFACT_INFO_COMMENT,
	ARTIFACT_INFO_SIZE_UPPER,
	ARTIFACT_INFO_AP_RATE,
	ARTIFACT_INFO_CHARGE,
	ARTIFACT_INFO_CSV_COLUMNS,
};

static cptr artifact_info_csv_list[ARTIFACT_INFO_CSV_COLUMNS] =
{
	"ID",
	"NAME",
	"E_NAME",
	"TVAL",
	"SVAL",
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHA",
	"PVAL",
	"DEPTH",
	"RARITY",
	"WEIGHT",
	"COST",
	"SIZE_LOWER",
	"BASE_AC",
	"BASE_EV",
	"BASE_DAMAGE",
	"PLUS_HIT",
	"PLUS_DAM",
	"PLUS_AC",
	"PLUS_EV",
	"FLAGS",
	"DESCRIPTION",
	"E_DESCRIPTION",
	"COMMENT",
	"SIZE_UPPER",
	"AP_RATE",
	"CHARGE",
};


static int artifact_info_csv_code[ARTIFACT_INFO_CSV_COLUMNS];
errr parse_artifact_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b, c;
	char *s, *t;
	char tmp[10000], nt[80];
	artifact_type *artifact_ptr;

	if(get_split_offset(split, size, buf, ARTIFACT_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, artifact_info_csv_list[0]))
	{
		artifact_info_csv_code[0] = ARTIFACT_INFO_ID;
		for(i = 1; i < ARTIFACT_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < ARTIFACT_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, artifact_info_csv_list[j]))
				{
					artifact_info_csv_code[i] = j;
					break;
				}
			}
			if(j == ARTIFACT_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize Artifact:%d]", n);

		artifact_ptr = &artifact_info[n];
		note(nt);

		for(i = 1; i < ARTIFACT_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);

			tmp[size[i]] = '\0';
			

			switch(artifact_info_csv_code[i])
			{

			case ARTIFACT_INFO_NAME:
				if(!add_name(&artifact_ptr->name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case ARTIFACT_INFO_E_NAME:
				break;

			case ARTIFACT_INFO_TVAL:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->tval = (byte)b;
				else
					artifact_ptr->tval = 0;
				break;

			case ARTIFACT_INFO_SVAL:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->sval = (byte)b;
				else
					artifact_ptr->sval = 0;
				break;

			case ARTIFACT_INFO_STR:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->stat[STAT_STR] = (byte)b;
				else
					artifact_ptr->stat[STAT_STR] = 0;
				break;

			case ARTIFACT_INFO_INT:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->stat[STAT_INT] = (byte)b;
				else
					artifact_ptr->stat[STAT_INT] = 0;
				break;

			case ARTIFACT_INFO_WIS:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->stat[STAT_WIS] = (byte)b;
				else
					artifact_ptr->stat[STAT_WIS] = 0;
				break;

			case ARTIFACT_INFO_DEX:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->stat[STAT_DEX] = (byte)b;
				else
					artifact_ptr->stat[STAT_DEX] = 0;
				break;

			case ARTIFACT_INFO_CON:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->stat[STAT_CON] = (byte)b;
				else
					artifact_ptr->stat[STAT_CON] = 0;
				break;

			case ARTIFACT_INFO_CHA:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->stat[STAT_CHA] = (byte)b;
				else
					artifact_ptr->stat[STAT_CHA] = 0;
				break;

			case ARTIFACT_INFO_PVAL:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->pval = (byte)b;
				else
					artifact_ptr->pval = 0;
				break;

			case ARTIFACT_INFO_DEPTH:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->level = (byte)b;
				else
					artifact_ptr->level = 0;
				break;

			case ARTIFACT_INFO_RARITY:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->rarity = (byte)b;
				else
					artifact_ptr->rarity = 0;
				break;

			case ARTIFACT_INFO_WEIGHT:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->weight = (s16b)b;
				else
					artifact_ptr->weight = 0;
				break;

			case ARTIFACT_INFO_COST:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->cost = (s32b)b;
				else
					artifact_ptr->cost = 0;
				break;

			case ARTIFACT_INFO_SIZE_LOWER:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->size_lower = (s16b)b;
				else
					artifact_ptr->size_lower = 0;
				break;

			case ARTIFACT_INFO_SIZE_UPPER:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->size_upper = (s16b)b;
				else
					artifact_ptr->size_upper = 0;
				break;

			case ARTIFACT_INFO_BASE_AC:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->ac = (s16b)b;
				else
					artifact_ptr->ac = 0;
				break;

			case ARTIFACT_INFO_BASE_EV:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->ev = (s16b)b;
				else
					artifact_ptr->ev = 0;
				break;

			case ARTIFACT_INFO_BASE_DAMAGE:
				if(sscanf(tmp, "%dd%d", &b, &c) == 2)
				{
					artifact_ptr->dd = (byte)b;
					artifact_ptr->ds = (byte)c;
				}
				else
				{
					artifact_ptr->dd = 0;
					artifact_ptr->ds = 0;
				}
				break;

			case ARTIFACT_INFO_PLUS_HIT:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->to_hit = (s16b)b;
				else
					artifact_ptr->to_hit = 0;
				break;

			case ARTIFACT_INFO_PLUS_DAM:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->to_damage = (s16b)b;
				else
					artifact_ptr->to_damage = 0;
				break;

			case ARTIFACT_INFO_PLUS_AC:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->to_ac = (s16b)b;
				else
					artifact_ptr->to_ac = 0;
				break;

			case ARTIFACT_INFO_PLUS_EV:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->to_ev = (s16b)b;
				else
					artifact_ptr->to_ev = 0;
				break;

			case ARTIFACT_INFO_FLAGS:
				{
					s = tmp;

					/* Parse every entry textually */
					for (s = tmp; *s;)
					{
						/* Find the end of this entry */
						for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;
						/* Nuke and skip any dividers */
						if(*t)
						{
							*t++ = '\0';
							while (*t == ' ' || *t == '|') t++;
						}

						/* Parse this entry */
						if(0 != traits_precondition_splits(&artifact_ptr->add_creature_traits, s))
							return PARSE_ERROR_INVALID_FLAG;

							/* Start the next entry */
						s = t;
					}
					break;

				}

			case ARTIFACT_INFO_DESCRIPTION:
				/* Store the text */
				if(!add_text(&artifact_ptr->text, head, tmp, TRUE))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case ARTIFACT_INFO_E_DESCRIPTION:
				break;

			case ARTIFACT_INFO_COMMENT:
				break;

			case ARTIFACT_INFO_AP_RATE:
				if(sscanf(tmp, "%d", &b) == 1)
					artifact_ptr->to_damage = (s16b)b;
				else
					artifact_ptr->to_damage = 0;
				break;

			case ARTIFACT_INFO_CHARGE:
				if(sscanf(tmp, "%d", &b) == 1)
				{
					artifact_ptr->charge_const = (s16b)b;
					artifact_ptr->charge_dice  = 0;
				}
				else if(sscanf(tmp, "%d+d%d", &b, &c) == 2)
				{
					artifact_ptr->charge_const = (s16b)b;
					artifact_ptr->charge_dice  = (s16b)c;
				}
				else
				{
					artifact_ptr->charge_const = 0;
					artifact_ptr->charge_dice  = 0;
				}
				break;

			default:
				return PARSE_ERROR_GENERIC;

			}
		}
		
	}
	return PARSE_ERROR_NONE;
}

enum OBJECT_EGO {
	OBJECT_EGO_INFO_ID,
	OBJECT_EGO_INFO_NAME,
	OBJECT_EGO_INFO_E_NAME,
	OBJECT_EGO_INFO_SLOT,
	OBJECT_EGO_INFO_RATING,
	OBJECT_EGO_INFO_MAX_HIT,
	OBJECT_EGO_INFO_MAX_DAM,
	OBJECT_EGO_INFO_MAX_AC,
	OBJECT_EGO_INFO_MAX_EV,
	OBJECT_EGO_INFO_STR,
	OBJECT_EGO_INFO_INT,
	OBJECT_EGO_INFO_WIS,
	OBJECT_EGO_INFO_DEX,
	OBJECT_EGO_INFO_CON,
	OBJECT_EGO_INFO_CHA,
	OBJECT_EGO_INFO_PVAL,
	OBJECT_EGO_INFO_DEPTH,
	OBJECT_EGO_INFO_RARITY,
	OBJECT_EGO_INFO_WEIGHT,
	OBJECT_EGO_INFO_COST,
	OBJECT_EGO_INFO_FLAG,
	OBJECT_EGO_INFO_COMMENT,
	OBJECT_EGO_INFO_AP_RATE,
	OBJECT_EGO_INFO_CHARGE,
	OBJECT_EGO_INFO_CSV_COLUMNS,
};

static cptr object_ego_info_csv_list[OBJECT_EGO_INFO_CSV_COLUMNS] =
{
	"ID",
	"NAME",
	"E_NAME",
	"SLOT",
	"RATING",
	"MAX_HIT",
	"MAX_DAM",
	"MAX_AC",
	"MAX_EV",
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHA",
	"PVAL",
	"DEPTH",
	"RARITY",
	"WEIGHT",
	"COST",
	"FLAG",
	"COMMENT",
	"CHARGE",
	"AP_RATE",
};

static int object_ego_info_csv_code[OBJECT_EGO_INFO_CSV_COLUMNS];
errr parse_object_ego_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b, c;
	char *s, *t;
	char tmp[10000], nt[80];
	ego_item_type *object_ego_ptr;

	if(get_split_offset(split, size, buf, OBJECT_EGO_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, object_ego_info_csv_list[0]))
	{
		object_ego_info_csv_code[0] = OBJECT_EGO_INFO_ID;
		for(i = 1; i < OBJECT_EGO_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < OBJECT_EGO_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, object_ego_info_csv_list[j]))
				{
					object_ego_info_csv_code[i] = j;
					break;
				}
			}
			if(j == OBJECT_EGO_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize Object Ego:%d]", n);

		object_ego_ptr = &object_ego_info[n];

		note(nt);

		for(i = 1; i < OBJECT_EGO_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);

			tmp[size[i]] = '\0';
			

			switch(object_ego_info_csv_code[i])
			{

			case OBJECT_EGO_INFO_NAME:
				if(!add_name(&object_ego_ptr->name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case OBJECT_EGO_INFO_E_NAME:
				break;

			case OBJECT_EGO_INFO_SLOT:
				for(j = 0; j < MAX_INVENTORY_SLOTS; j++)
				{
					if(streq(equip_slot_flags[j], tmp))
					{
						object_ego_ptr->slot = j;
						break;
					}
				}
				if(j == MAX_INVENTORY_SLOTS)
					return PARSE_ERROR_GENERIC;
				break;

			case OBJECT_EGO_INFO_RATING:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->rating = (byte)b;
				else
					object_ego_ptr->rating = 0;
				break;

			case OBJECT_EGO_INFO_MAX_HIT:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_to_hit = (byte)b;
				else
					object_ego_ptr->max_to_hit = 0;
				break;

			case OBJECT_EGO_INFO_MAX_DAM:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_to_damage = (byte)b;
				else
					object_ego_ptr->max_to_damage = 0;
				break;

			case OBJECT_EGO_INFO_MAX_AC:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_to_ac = (byte)b;
				else
					object_ego_ptr->max_to_ac = 0;
				break;

			case OBJECT_EGO_INFO_MAX_EV:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_to_ev = (byte)b;
				else
					object_ego_ptr->max_to_ev = 0;
				break;

			case OBJECT_EGO_INFO_PVAL:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_pval = (byte)b;
				else
					object_ego_ptr->max_pval = 0;
				break;

			case OBJECT_EGO_INFO_STR:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_stat[STAT_STR] = (byte)b;
				else
					object_ego_ptr->max_stat[STAT_STR] = 0;
				break;

			case OBJECT_EGO_INFO_INT:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_stat[STAT_INT] = (byte)b;
				else
					object_ego_ptr->max_stat[STAT_INT] = 0;
				break;

			case OBJECT_EGO_INFO_WIS:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_stat[STAT_WIS] = (byte)b;
				else
					object_ego_ptr->max_stat[STAT_WIS] = 0;
				break;

			case OBJECT_EGO_INFO_DEX:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_stat[STAT_DEX] = (byte)b;
				else
					object_ego_ptr->max_stat[STAT_DEX] = 0;
				break;

			case OBJECT_EGO_INFO_CON:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_stat[STAT_CON] = (byte)b;
				else
					object_ego_ptr->max_stat[STAT_CON] = 0;
				break;

			case OBJECT_EGO_INFO_CHA:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->max_stat[STAT_CHA] = (byte)b;
				else
					object_ego_ptr->max_stat[STAT_CHA] = 0;
				break;

			case OBJECT_EGO_INFO_DEPTH:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->level = (byte)b;
				else
					object_ego_ptr->level = 0;
				break;

			case OBJECT_EGO_INFO_RARITY:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->rarity = (byte)b;
				else
					object_ego_ptr->rarity = 0;
				break;

			case OBJECT_EGO_INFO_WEIGHT:
				break;

			case OBJECT_EGO_INFO_COST:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->cost = (s32b)b;
				else
					object_ego_ptr->cost = 0;
				break;

			case OBJECT_EGO_INFO_FLAG:
				s = tmp;
				/* Parse every entry textually */
				for (s = tmp; *s;)
				{
						/* Find the end of this entry */
					for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

						/* Nuke and skip any dividers */
					if(*t)
					{
						*t++ = '\0';
						while (*t == ' ' || *t == '|') t++;
					}

					/* Parse this entry */
					if(0 != traits_precondition_splits(&object_ego_ptr->add_creature_traits, s))
						return PARSE_ERROR_INVALID_FLAG;

						/* Start the next entry */
					s = t;
				}
				break;

			case OBJECT_EGO_INFO_COMMENT:
				break;

			case OBJECT_EGO_INFO_AP_RATE:
				if(sscanf(tmp, "%d", &b) == 1)
					object_ego_ptr->ap_rate = (s16b)b;
				else
					object_ego_ptr->ap_rate = 0;
				break;

			case OBJECT_EGO_INFO_CHARGE:
				if(sscanf(tmp, "%d", &b) == 1)
				{
					object_ego_ptr->charge_const = (s16b)b;
					object_ego_ptr->charge_dice  = 0;
				}
				else if(sscanf(tmp, "%d+d%d", &b, &c) == 2)
				{
					object_ego_ptr->charge_const = (s16b)b;
					object_ego_ptr->charge_dice  = (s16b)c;
				}
				else
				{
					object_ego_ptr->charge_const = 0;
					object_ego_ptr->charge_dice  = 0;
				}
				break;

			default:
				return PARSE_ERROR_GENERIC;

			}
		}
		
	}
	return PARSE_ERROR_NONE;
}

/*
 * Grab store flag in a species_type from a textual string
 */
static errr grab_store_flag(store_pre_type *stp_ptr, cptr what)
{
	if(grab_one_flag(&stp_ptr->flags, store_pre_info_flags, what) == 0)
		return PARSE_ERROR_NONE;

	/* Oops */
#ifdef JP
	msg_format("未知の店フラグ '%s'。", what);
#else
	msg_format("Unknown store flag '%s'.", what);
#endif


	/* Failure */
	return PARSE_ERROR_GENERIC;

}


/*
 * Grab one (basic) flag in a species_type from a textual string
 */
static errr grab_one_race_flags(traits_precondition *flag_ptr, cptr what, byte add, byte remove)
{


	/* Oops */
#ifdef JP
	msg_format("未知の種族フラグ '%s'。", what);
#else
	msg_format("Unknown race flag '%s'.", what);
#endif


	/* Failure */
	return PARSE_ERROR_GENERIC;
}


/*
 * Grab one (spell) flag in a species_type from a textual string
 */
static int grab_one_authority_flag(species_type *r_ptr, cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < max_authorities_idx; i++)
	{
		if(streq(what, authority_info[i].id_e))
		{
			return i;
		}
	}

	return -1;
}


enum SPECIES_TYPE {

	SPECIES_INFO_ID,
	SPECIES_INFO_NAME,
	SPECIES_INFO_E_NAME,
	SPECIES_INFO_SYM,
	SPECIES_INFO_COL,
	SPECIES_INFO_RACE1,
	SPECIES_INFO_RACE2,
	SPECIES_INFO_CLASS,
	SPECIES_INFO_PATRON,
	SPECIES_INFO_CHARA,
	SPECIES_INFO_RELM1,
	SPECIES_INFO_RELM2,
	SPECIES_INFO_LEV,
	SPECIES_INFO_RARE,
	SPECIES_INFO_SPELL,
	SPECIES_INFO_EXP,
	SPECIES_INFO_N_EXP,
	SPECIES_INFO_N_MIN,
	SPECIES_INFO_AGE,
	SPECIES_INFO_SC,
	SPECIES_INFO_DV,
	SPECIES_INFO_SP,
	SPECIES_INFO_IS,
	SPECIES_INFO_AC,
	SPECIES_INFO_EV,
	SPECIES_INFO_ALERT,
	SPECIES_INFO_STR,
	SPECIES_INFO_INT,
	SPECIES_INFO_WIS,
	SPECIES_INFO_DEX,
	SPECIES_INFO_CON,
	SPECIES_INFO_CHA,
	SPECIES_INFO_M_HB,
	SPECIES_INFO_M_HM,
	SPECIES_INFO_M_WB,
	SPECIES_INFO_M_WM,
	SPECIES_INFO_F_HB,
	SPECIES_INFO_F_HM,
	SPECIES_INFO_F_WB,
	SPECIES_INFO_F_WM,
	SPECIES_INFO_BATTLE,
	SPECIES_INFO_UNDERLING,
	SPECIES_INFO_ARTIFACT,
	SPECIES_INFO_COMMENT,
	SPECIES_INFO_FLAG,
	SPECIES_INFO_DESCRIPTION,
	SPECIES_INFO_AUTHORITY,
	SPECIES_INFO_SEX,
	SPECIES_INFO_E_DESCRIPTION,
	SPECIES_INFO_FATHER,
	SPECIES_INFO_MOTHER,

	SPECIES_INFO_HAND,
	SPECIES_INFO_RING,
	SPECIES_INFO_AMULET,
	SPECIES_INFO_BODY,
	SPECIES_INFO_OUTER,
	SPECIES_INFO_HEAD,
	SPECIES_INFO_ARMS,
	SPECIES_INFO_FEET,
	SPECIES_INFO_TAIL,
	SPECIES_INFO_CSV_COLUMNS
};

static cptr species_info_csv_list[SPECIES_INFO_CSV_COLUMNS] =
{
	"ID",
	"NAME",
	"E_NAME",
	"SYM",
	"COL",
	"RACE1",
	"RACE2",
	"CLASS",
	"PATRON",
	"CHARA",
	"RELM1",
	"RELM2",
	"LEV",
	"RARE",
	"SPELL",
	"EXP",
	"N_EXP",
	"N_MIN",
	"AGE",
	"SC",
	"DV",
	"SP",
	"VIS",
	"AC",
	"EV",
	"ALERT",
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHA",
	"M_HB",
	"M_HM",
	"M_WB",
	"M_WM",
	"F_HB",
	"F_HM",
	"F_WB",
	"F_WM",
	"BATTLE",
	"UNDERLING",
	"ARTIFACT",
	"COMMENT",
	"FLAG",
	"DESCRIPTION",
	"AUTHORITY",
	"SEX",
	"E_DESCRIPTION",
	"FATHER",
	"MOTHER",

	"HAND",
	"RING",
	"AMULET",
	"BODY",
	"OUTER",
	"HEAD",
	"ARMS",
	"FEET",
	"TAIL",
};

static int species_info_csv_code[SPECIES_INFO_CSV_COLUMNS];

errr parse_species_info_csv(char *buf, header *head)
{
	int id, tval, sval, prob, ego, num, side, offset;
	int split[80], size[80];
	int i, j, k;
	char *s, *t;
	char tmp[20000], nt[80];
	int b, ub;
	u32b flags;

	if(get_split_offset(split, size, buf, SPECIES_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, species_info_csv_list[0]))
	{
		species_info_csv_code[0] = SPECIES_INFO_ID;
		for(i = 1; i < SPECIES_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < SPECIES_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, species_info_csv_list[j]))
				{
					species_info_csv_code[i] = j;
					break;
				}
			}
			if(j == SPECIES_INFO_CSV_COLUMNS) return PARSE_ERROR_GENERIC; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		species_type *species_ptr;
		int n;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize Creature:%d]", n);

		species_ptr = &species_info[n];
		species_ptr->species_idx = n;
		species_ptr->ap_species_idx = n;


		note(nt);

		for(i = 1; i < SPECIES_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			

			switch(species_info_csv_code[i])
			{

			case SPECIES_INFO_NAME:
#if JP
				if(!add_name(&species_ptr->name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
#endif
				break;

			case SPECIES_INFO_E_NAME:
#if JP
				if(!add_name(&species_ptr->E_name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
#else
				if(!add_name(&species_ptr->name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
#endif
				break;

			case SPECIES_INFO_SYM:
				species_ptr->d_char = tmp[0];
				species_ptr->x_char = tmp[0];
				break;

			case SPECIES_INFO_COL:
				species_ptr->d_attr = color_char_to_acttr(tmp[0]);
				species_ptr->x_attr = color_char_to_acttr(tmp[0]);
				break;

			case SPECIES_INFO_RACE1:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->race_idx1 = (s16b)b;
				else 
					if(grab_one_index(&b, race_flags, tmp, TRUE)) return PARSE_ERROR_GENERIC;
				species_ptr->race_idx1 = (s16b)b;
				break;

			case SPECIES_INFO_RACE2:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->race_idx2 = (s16b)b;
				else 
					if(grab_one_index(&b, race_flags, tmp, TRUE)) return PARSE_ERROR_GENERIC;
				species_ptr->race_idx2 = (s16b)b;
				break;

			case SPECIES_INFO_CLASS:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->class_idx = (s16b)b;
				else 
					if(grab_one_index(&b, class_flags, tmp, FALSE)) return PARSE_ERROR_GENERIC;
					species_ptr->class_idx = (s16b)b;
				break;

			case SPECIES_INFO_PATRON:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->patron_idx = (s16b)b;
				else 
					if(grab_one_index(&b, NULL, tmp, TRUE)) return PARSE_ERROR_GENERIC;
					species_ptr->patron_idx = (s16b)b;
				break;

			case SPECIES_INFO_CHARA:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->chara_idx = (s16b)b;
				else 
					if(grab_one_index(&b, chara_flags, tmp, TRUE)) return PARSE_ERROR_GENERIC;
				species_ptr->chara_idx = (s16b)b;
				break;

			case SPECIES_INFO_RELM1:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->realm1 = (s16b)b;
				else 
					if(grab_one_index(&b, realm_flags, tmp, FALSE)) return PARSE_ERROR_GENERIC;
				species_ptr->realm1 = (s16b)b;
				break;

			case SPECIES_INFO_RELM2:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->realm2 = (s16b)b;
				else 
					if(grab_one_index(&b, realm_flags, tmp, FALSE)) return PARSE_ERROR_GENERIC;
				species_ptr->realm2 = (s16b)b;
				break;

			case SPECIES_INFO_LEV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->level = (byte)b;
				break;

			case SPECIES_INFO_RARE:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->rarity = (byte)b;
				break;

			case SPECIES_INFO_SPELL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->freq_spell = (byte)b;
				break;

			case SPECIES_INFO_EXP:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->exp = (s32b)b;
				break;

			case SPECIES_INFO_N_EXP:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->next_exp = (u32b)b;
				break;

			case SPECIES_INFO_N_MIN:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->next_species_idx = (s16b)b;
				break;

			case SPECIES_INFO_AGE:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->age = (s32b)b;
				break;

			case SPECIES_INFO_SC:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->sc = (s16b)b;
				break;

			case SPECIES_INFO_DV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->dr = (s16b)b;
				break;

			case SPECIES_INFO_SP:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->speed = (s16b)b;
				break;

			case SPECIES_INFO_IS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->sleep = (s16b)b;
				break;

			case SPECIES_INFO_AC:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->ac = (s16b)b;
				break;

			case SPECIES_INFO_ALERT:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->alert_range = (byte)b;
				break;

			case SPECIES_INFO_STR:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->stat_max[STAT_STR] = (s16b)b * STAT_FRACTION;
				species_ptr->stat_max_max[STAT_STR] = (s16b)b * STAT_FRACTION;
				break;

			case SPECIES_INFO_INT:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->stat_max[STAT_INT] = (s16b)b * STAT_FRACTION;
				species_ptr->stat_max_max[STAT_INT] = (s16b)b * STAT_FRACTION;
				break;

			case SPECIES_INFO_WIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->stat_max[STAT_WIS] = (s16b)b * STAT_FRACTION;
				species_ptr->stat_max_max[STAT_WIS] = (s16b)b * STAT_FRACTION;
				break;

			case SPECIES_INFO_DEX:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->stat_max[STAT_DEX] = (s16b)b * STAT_FRACTION;
				species_ptr->stat_max_max[STAT_DEX] = (s16b)b * STAT_FRACTION;
				break;

			case SPECIES_INFO_CON:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->stat_max[STAT_CON] = (s16b)b * STAT_FRACTION;
				species_ptr->stat_max_max[STAT_CON] = (s16b)b * STAT_FRACTION;
				break;

			case SPECIES_INFO_CHA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->stat_max[STAT_CHA] = (s16b)b * STAT_FRACTION;
				species_ptr->stat_max_max[STAT_CHA] = (s16b)b * STAT_FRACTION;
				break;

			case SPECIES_INFO_M_HB:
				if(sscanf(tmp, "%u", &ub) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->m_b_ht = (u32b)ub;
				break;

			case SPECIES_INFO_M_HM:
				if(sscanf(tmp, "%u", &ub) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->m_m_ht = (u32b)ub;
				break;

			case SPECIES_INFO_M_WB:
				if(sscanf(tmp, "%u", &ub) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->m_b_wt = (u32b)ub;
				break;

			case SPECIES_INFO_M_WM:
				if(sscanf(tmp, "%u", &ub) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->m_m_wt = (u32b)ub;
				break;

			case SPECIES_INFO_F_HB:
				if(sscanf(tmp, "%u", &ub) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->f_b_ht = (u32b)ub;
				break;

			case SPECIES_INFO_F_HM:
				if(sscanf(tmp, "%u", &ub) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->f_m_ht = (u32b)ub;
				break;

			case SPECIES_INFO_F_WB:
				if(sscanf(tmp, "%u", &ub) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->f_b_wt = (u32b)ub;
				break;

			case SPECIES_INFO_F_WM:
				if(sscanf(tmp, "%u", &ub) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->f_m_wt = (u32b)ub;
				break;

			case SPECIES_INFO_BATTLE:
				parse_special_melee(species_ptr->blow, tmp);
				break;

			case SPECIES_INFO_UNDERLING:
				offset = 0;
				k = 0;
				while(tmp[offset]) {
					if(3 != sscanf(tmp + offset, "%d:%dd%d", &id, &num, &side)) return PARSE_ERROR_GENERIC;		

					if(k == MAX_UNDERLINGS) return PARSE_ERROR_GENERIC;

					species_ptr->underling_id[k] = id;
					species_ptr->underling_d_num[k] = num;
					species_ptr->underling_d_side[k] = side;
					k++;
					while(tmp[offset] != '\n' && tmp[offset]) offset++;
					if(tmp[offset]) offset++;
				}
			break;

			case SPECIES_INFO_ARTIFACT:
				offset = 0;
				k = 0;
				while(tmp[offset]) {
					id = tval = sval = prob = ego = 0;
					flags = 0;
					if(4 == sscanf(tmp + offset, "%d:%d:%d:%d", &id, &tval, &sval, &prob)) {}
					else if(2 == sscanf(tmp + offset, "ART:%d:%d%%", &id, &prob)) {}
					else if(3 == sscanf(tmp + offset, "NORMAL:%d:%d:%d%%", &tval, &sval, &prob))
					{
						flags = AM_UNCURSED;
					}
					else if(3 == sscanf(tmp + offset, "GOOD:%d:%d:%d%%", &tval, &sval, &prob))
					{
						flags = AM_GOOD;
					}
					else if(3 == sscanf(tmp + offset, "GREAT:%d:%d:%d%%", &tval, &sval, &prob))
					{
						flags = AM_GREAT;
					}
					else if(4 == sscanf(tmp + offset, "EGO:%d:%d:%d:%d%%", &ego, &tval, &sval, &prob))
					{
						flags = AM_GREAT;
					}
					else return PARSE_ERROR_GENERIC;		

					if(k == INVEN_TOTAL) return PARSE_ERROR_GENERIC;

					species_ptr->artifact_id[k] = id;
					species_ptr->artifact_ego[k] = ego;
					species_ptr->artifact_flag[k] = flags;
					species_ptr->artifact_tval[k] = tval;
					species_ptr->artifact_sval[k] = sval;
					species_ptr->artifact_prob[k] = prob;
					k++;
					while(tmp[offset] != '\n' && tmp[offset]) offset++;
					if(tmp[offset]) offset++;
				}
				break;

			case SPECIES_INFO_COMMENT:
				/* Nothing */
				break;

			case SPECIES_INFO_FLAG:
				if(0 != traits_precondition_splits(&species_ptr->flags, tmp))
					return PARSE_ERROR_GENERIC;
				break;

			case SPECIES_INFO_DESCRIPTION:
				if(!add_text(&species_ptr->text, head, tmp, TRUE))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case SPECIES_INFO_E_DESCRIPTION:
				if(!add_text(&species_ptr->E_text, head, tmp, TRUE))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case SPECIES_INFO_AUTHORITY:
				/* Parse every entry */
				for (s = tmp; *s; )
				{
					// Find the end of this entry
					for (t = s; *t && (*t != ' ') && (*t != '\n') && (*t != '|'); ++t); // loop

					// Nuke and skip any dividers
					if(*t)
					{
						*t++ = '\0';
						while ((*t == ' ') || (*t == '|') || (*t == '\n')) t++;
					}

					// Parse this entry
					if((b = grab_one_authority_flag(&species_info[n], s)) < 0) return PARSE_ERROR_INVALID_FLAG;
					species_ptr->authority[b / 32] |= (0x0001 << (b % 32));

					// Start the next entry
					s = t;
				}

				break;

			case SPECIES_INFO_SEX:
				if((sscanf(tmp, "0x%x", &b) != 1) && grab_one_index(&b, NULL, tmp, TRUE)) return PARSE_ERROR_GENERIC;
				species_ptr->sex = (s16b)b;
				break;

			case SPECIES_INFO_FATHER:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->father_idx = (s16b)b;
				else 
					if(grab_one_index(&b, NULL, tmp, TRUE)) return PARSE_ERROR_GENERIC;
				species_ptr->father_idx = (s16b)b;
				break;

			case SPECIES_INFO_MOTHER:
				if(sscanf(tmp, "%d", &b) == 1)
					species_ptr->mother_idx = (s16b)b;
				else 
					if(grab_one_index(&b, NULL, tmp, TRUE)) return PARSE_ERROR_GENERIC;
				species_ptr->mother_idx = (s16b)b;
				break;

			case SPECIES_INFO_HAND:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_hand = (s16b)b;
				break;

			case SPECIES_INFO_RING:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_ring = (s16b)b;
				break;

			case SPECIES_INFO_AMULET:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_amulet = (s16b)b;
				break;

			case SPECIES_INFO_BODY:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_body = (s16b)b;
				break;

			case SPECIES_INFO_OUTER:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_outer = (s16b)b;
				break;

			case SPECIES_INFO_HEAD:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_head = (s16b)b;
				break;

			case SPECIES_INFO_ARMS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_arms = (s16b)b;
				break;

			case SPECIES_INFO_FEET:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_feet = (s16b)b;
				break;

			case SPECIES_INFO_TAIL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->slot_tail = (s16b)b;
				break;

			case SPECIES_INFO_EV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				species_ptr->ev = (s16b)b;
				break;

			default:
				return PARSE_ERROR_GENERIC; // Error
			}
		}
		
	}
	return PARSE_ERROR_NONE;
}

/*
 * Initialize the "re_info" array, by parsing an ascii "template" file
 */
errr parse_re_info(char *buf, header *head)
{
	int i;
	char *s;

	/* Current entry */
	static creature_ego *re_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if(buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = my_strchr(buf+2, ':');

			/* Verify that colon */
		if(!s) return PARSE_ERROR_GENERIC;

		/* Nuke the colon, advance to the name */
		*s++ = '\0';
#ifdef JP
		/* Paranoia -- require a name */
		if(!*s) return PARSE_ERROR_GENERIC;
#endif

		/* Get the index */
		i = atoi(buf+2);


		/* Verify information */
		if(i < error_idx) return (4);

		/* Verify information */
		if(i >= head->info_num) return (2);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		re_ptr = &re_info[i];
		re_ptr->name = 0;
#ifdef JP
		/* Store the name */
		if(!add_name(&re_ptr->name, head, s)) return PARSE_ERROR_OUT_OF_MEMORY;
#endif
	}

	/* There better be a current r_ptr */
	else if(!re_ptr) return (3);

#ifdef JP
	/* 英語名を読むルーチンを追加 */
	/* 'E' から始まる行は英語名 */
	else if(buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if(!add_name(&re_ptr->E_name, head, s)) return PARSE_ERROR_OUT_OF_MEMORY;
	}
#else
	else if(buf[0] == 'E')
	{
		/* Acquire the Text */
		s = buf+2;

		/* Store the name */
		if(!add_name(&re_ptr->name, head, s)) return PARSE_ERROR_OUT_OF_MEMORY;
	}
#endif

	/*stat*/
	else if(buf[0] == 'S')
	{
		int k[6];
		if(6 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d",
				&k[0], &k[1], &k[2], &k[3], &k[4], &k[5])) return PARSE_ERROR_GENERIC;
		
		re_ptr->stat[STAT_STR] = k[0];
		re_ptr->stat[STAT_INT] = k[1];
		re_ptr->stat[STAT_WIS] = k[2];
		re_ptr->stat[STAT_DEX] = k[3];
		re_ptr->stat[STAT_CON] = k[4];
		re_ptr->stat[STAT_CHA] = k[5];

	}

	/* Oops */
	else return (6);


	/* Success */
	return PARSE_ERROR_NONE;
}

enum ST_INFO
{
  ST_INFO_ID,
  ST_INFO_NAME,
  ST_INFO_E_NAME,
  ST_INFO_OWNER,
  ST_INFO_SIZE,
  ST_INFO_WEALTH,
  ST_INFO_FLAGS,
  ST_INFO_LEVEL,
  ST_INFO_CSV_COLUMNS
};

static cptr store_pre_info_csv_list[ST_INFO_CSV_COLUMNS] =
{
	"ID",
	"NAME",
	"E_NAME",
	"OWNER",
	"SIZE",
	"WEALTH",
	"FLAGS",
	"LEVEL",
};

static int store_pre_info_csv_code[SPECIES_INFO_CSV_COLUMNS];

errr parse_store_pre_info_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b;
	char tmp[10000], nt[80];
	char *s, *t;

	if(get_split_offset(split, size, buf, ST_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, store_pre_info_csv_list[0]))
	{
		store_pre_info_csv_code[0] = ST_INFO_ID;
		for(i = 1; i < ST_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < ST_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, store_pre_info_csv_list[j]))
				{
					store_pre_info_csv_code[i] = j;
					break;
				}
			}
			if(j == ST_INFO_CSV_COLUMNS) return PARSE_ERROR_GENERIC; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize Store:%d]", n);


		note(nt);

		for(i = 1; i < ST_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			

			switch(store_pre_info_csv_code[i])
			{

			case ST_INFO_NAME:
#if JP
				if(!add_name(&store_pre_info[n].name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
#endif
				break;

			case ST_INFO_E_NAME:
#if JP
				if(!add_name(&store_pre_info[n].E_name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
#else
				if(!add_name(&store_pre_info[n].name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
#endif
				break;

			case ST_INFO_OWNER:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				store_pre_info[n].owner_id = (s16b)b;
				break;

			case ST_INFO_SIZE:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				store_pre_info[n].size = (u16b)b;
				break;

			case ST_INFO_WEALTH:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				store_pre_info[n].wealth = (s32b)b;
				break;

			case ST_INFO_LEVEL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				store_pre_info[n].level = (byte)b;
				break;

			case ST_INFO_FLAGS:
				for (s = tmp; *s; ){

					for (t = s; *t && (*t != ' ') && (*t != '\n') && (*t != '|'); ++t) /* loop */;

					/* Nuke and skip any dividers */
					if(*t)
					{
						*t++ = '\0';
						while (*t == ' ' || *t == '|' || *t == '\n') t++;
					}

					/* Parse this entry */
					if(0 != grab_store_flag(&store_pre_info[n], s)) return (PARSE_ERROR_INVALID_FLAG);

					/* Start the next entry */
					s = t;
				}
				break;

			default:
				return 1;

			}
		}
		
	}
	return PARSE_ERROR_NONE;
}

enum TRAIT_INFO
{
  TRAIT_INFO_ID,
  TRAIT_INFO_ID2, 
  TRAIT_INFO_NAME,
  TRAIT_INFO_E_NAME,
  TRAIT_INFO_STR,
  TRAIT_INFO_INT,
  TRAIT_INFO_WIS,
  TRAIT_INFO_DEX,
  TRAIT_INFO_CON,
  TRAIT_INFO_CHA,
  TRAIT_INFO_AC,
  TRAIT_INFO_EV,
  TRAIT_INFO_VO,
  TRAIT_INFO_DIS,
  TRAIT_INFO_DEV,
  TRAIT_INFO_STL,
  TRAIT_INFO_SRH,
  TRAIT_INFO_FOS,
  TRAIT_INFO_THN,
  TRAIT_INFO_THB,
  TRAIT_INFO_INFRA,
  TRAIT_INFO_HITD_M,
  TRAIT_INFO_EXP,
  TRAIT_INFO_FLAGS,
  TRAIT_INFO_SPEED,
  TRAIT_INFO_DESCRIPTION,
  TRAIT_INFO_E_DESCRIPTION,
  TRAIT_INFO_SPELL,
  TRAIT_INFO_PRE_ID,
  TRAIT_INFO_ANTI_ID,
  TRAIT_INFO_BASE_LEVEL,
  TRAIT_INFO_MP_COST,
  TRAIT_INFO_USE_STAT,
  TRAIT_INFO_FAIL,
  TRAIT_INFO_CHARGE,
  TRAIT_INFO_MUTA_RARE,
  TRAIT_INFO_GET_DESC,
  TRAIT_INFO_E_GET_DESC,
  TRAIT_INFO_LOST_DESC,
  TRAIT_INFO_E_LOST_DESC,
  TRAIT_ACTIVATE_NORMAL_DESC,
  TRAIT_ACTIVATE_BLIND_DESC,
  TRAIT_ACTIVATE_NORMAL_E_DESC,
  TRAIT_ACTIVATE_BLIND_E_DESC,
  TRAIT_INFO_CSV_COLUMNS,
};

static cptr cfeature_info_csv_list[TRAIT_INFO_CSV_COLUMNS] =
{
	"ID",
	"ID2",
	"NAME",
	"E_NAME",
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHA",
	"AC",
	"EV",
	"VO",
	"DIS",
	"DEV",
	"STL",
	"SRH",
	"FOS",
	"THN",
	"THB",
	"INFRA",
	"HITD_M",
	"EXP",
	"FLAGS",
	"SPEED",
	"DESCRIPTION",
	"E_DESCRIPTION",
	"SPELL",
	"PRE_ID",
	"ANTI_ID",
	"BASE_LEVEL",
	"MP_COST",
	"USE_STAT",
	"FAIL",
	"CHARGE",
	"MUTA_RARE",
	"GET_DESC",
	"E_GET_DESC",
	"LOST_DESC",
	"E_LOST_DESC",
	"ACTIVATE_NORMAL_DESC",
	"ACTIVATE_BLIND_DESC",
	"ACTIVATE_NORMAL_E_DESC",
	"ACTIVATE_BLIND_E_DESC",
};

static int cfeature_info_csv_code[TRAIT_INFO_CSV_COLUMNS];

errr reprocess_trait(header *head)
{
	int i;
	for(i = 0; i < max_trait_idx; i++)
	{
		traits_precondition_splits(&trait_info[i].flags, head->tmp_ptr);
	}

	return PARSE_ERROR_NONE;
}

errr parse_trait_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b;
	char tmp[10000], nt[80];

	if(get_split_offset(split, size, buf, TRAIT_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, cfeature_info_csv_list[0]))
	{
		cfeature_info_csv_code[0] = TRAIT_INFO_ID;
		for(i = 1; i < TRAIT_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < TRAIT_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, cfeature_info_csv_list[j]))
				{
					cfeature_info_csv_code[i] = j;
					break;
				}
			}
			if(j == TRAIT_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		trait_type *trait_ptr;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize CF:%d]", n);
		trait_ptr = &trait_info[n];

		note(nt);

		for(i = 1; i < TRAIT_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			
			switch(cfeature_info_csv_code[i])
			{
				case TRAIT_INFO_ID2:
					strcpy(trait_ptr->id2, tmp);
				break;

				case TRAIT_INFO_NAME:
					strcpy(trait_ptr->title, tmp);
				break;

				case TRAIT_INFO_E_NAME:
					strcpy(trait_ptr->e_title, tmp);
				break;

				case TRAIT_INFO_STR:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->adj[STAT_STR] = (s16b)b;
				break;

				case TRAIT_INFO_INT:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->adj[STAT_INT] = (s16b)b;
				break;

				case TRAIT_INFO_WIS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->adj[STAT_WIS] = (s16b)b;
				break;

				case TRAIT_INFO_DEX:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->adj[STAT_DEX] = (s16b)b;
				break;

				case TRAIT_INFO_CON:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->adj[STAT_CON] = (s16b)b;
				break;

				case TRAIT_INFO_CHA:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->adj[STAT_CHA] = (s16b)b;
				break;

				case TRAIT_INFO_AC:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->ac = (s16b)b;
				break;

				case TRAIT_INFO_EV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->ev = (s16b)b;
				break;

				case TRAIT_INFO_VO:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->vo = (s16b)b;
				break;

				case TRAIT_INFO_DIS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->dis = (s16b)b;
				break;

				case TRAIT_INFO_DEV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->dev = (s16b)b;
				break;

				case TRAIT_INFO_STL:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->stl = (s16b)b;
				break;

				case TRAIT_INFO_SRH:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->srh = (s16b)b;
				break;

				case TRAIT_INFO_FOS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->fos = (s16b)b;
				break;

				case TRAIT_INFO_THN:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->thn = (s16b)b;
				break;

				case TRAIT_INFO_THB:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->thb = (s16b)b;
				break;

				case TRAIT_INFO_INFRA:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->infra = (s16b)b;
				break;

				case TRAIT_INFO_HITD_M:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->hitd_m = (s16b)b;
				break;

				case TRAIT_INFO_EXP:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->exp = (s16b)b;
				break;

				case TRAIT_INFO_CHARGE:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->charge = (s16b)b;
				break;

				case TRAIT_INFO_FLAGS:
					strcpy(&trait_tmp[n], tmp);
				break;

				case TRAIT_INFO_SPEED:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->speed = (s16b)b;
				break;

				case TRAIT_INFO_DESCRIPTION:
					if(!add_text(&trait_ptr->text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case TRAIT_INFO_E_DESCRIPTION:
					if(!add_text(&trait_ptr->E_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
				break;

				case TRAIT_INFO_SPELL:
					if(sscanf(tmp, "%d", &b) == 1)
						trait_ptr->effect_type = (byte)b;
					else if(grab_one_index(&b, trait_info_type, tmp, FALSE)) return PARSE_ERROR_GENERIC;
					else trait_ptr->effect_type = (byte)b;
				break;

				case TRAIT_INFO_PRE_ID:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->pre_id = (s16b)b;
				break;

				case TRAIT_INFO_ANTI_ID:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->anti_id = (s16b)b;
				break;

				case TRAIT_INFO_BASE_LEVEL:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->base_level = (s16b)b;
				break;

				case TRAIT_INFO_MP_COST:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->mp_cost = (s16b)b;
				break;

				case TRAIT_INFO_USE_STAT:
					if(sscanf(tmp, "%d", &b) != 1) {
						if(streq(tmp, "STR")) trait_ptr->use_stat = STAT_STR;
						else if(streq(tmp, "INT")) trait_ptr->use_stat = STAT_INT;
						else if(streq(tmp, "WIS")) trait_ptr->use_stat = STAT_WIS;
						else if(streq(tmp, "DEX")) trait_ptr->use_stat = STAT_DEX;
						else if(streq(tmp, "CON")) trait_ptr->use_stat = STAT_CON;
						else if(streq(tmp, "CHA")) trait_ptr->use_stat = STAT_CHA;
						else return PARSE_ERROR_INVALID_FLAG;
					}
					trait_ptr->use_stat = (s16b)b;
				break;

				case TRAIT_INFO_FAIL:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->fail = (s16b)b;
				break;

				case TRAIT_INFO_MUTA_RARE:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					trait_ptr->mutative_rarity = (s16b)b;
				break;

				case TRAIT_INFO_GET_DESC:
					if(!add_text(&trait_ptr->get_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;
				break;

				case TRAIT_INFO_E_GET_DESC:
					if(!add_text(&trait_ptr->E_get_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;
				break;

				case TRAIT_INFO_LOST_DESC:
					if(!add_text(&trait_ptr->lost_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;
				break;

				case TRAIT_INFO_E_LOST_DESC:
					if(!add_text(&trait_ptr->E_lost_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;
				break;

				case TRAIT_ACTIVATE_NORMAL_DESC:
					if(!add_text(&trait_ptr->activate_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;
				break;

				case TRAIT_ACTIVATE_BLIND_DESC:
					if(!add_text(&trait_ptr->blind_activate_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;
				break;

				case TRAIT_ACTIVATE_NORMAL_E_DESC:
				break;

				case TRAIT_ACTIVATE_BLIND_E_DESC:
				break;

				default:
					return PARSE_ERROR_GENERIC;

			}
		}
		
	}
	return PARSE_ERROR_NONE;
}


enum RC_INFO
{
	RC_INFO_ID,
	RC_INFO_COMMON,
	RC_INFO_NAME,
	RC_INFO_E_NAME,
	RC_INFO_SEX,
	RC_INFO_LEV,
	RC_INFO_DR,
	RC_INFO_P_STR,
	RC_INFO_P_INT,
	RC_INFO_P_WIS,

	RC_INFO_P_DEX,
	RC_INFO_P_CON,
	RC_INFO_P_CHA,
	RC_INFO_H_STR,
	RC_INFO_H_INT,
	RC_INFO_H_WIS,
	RC_INFO_H_DEX,
	RC_INFO_H_CON,
	RC_INFO_H_CHA,
	RC_INFO_P_DIS,

	RC_INFO_P_DEV,
	RC_INFO_P_ROB,
	RC_INFO_P_EVA,
	RC_INFO_P_VOL,
	RC_INFO_P_STL,
	RC_INFO_P_SRH,
	RC_INFO_P_FOS,
	RC_INFO_P_THN,
	RC_INFO_P_THB,
	RC_INFO_P_INFRA,
	RC_INFO_H_DIS,
	RC_INFO_H_DEV,

	RC_INFO_H_ROB,
	RC_INFO_H_EVA,
	RC_INFO_H_VOL,
	RC_INFO_H_STL,
	RC_INFO_H_SRH,
	RC_INFO_H_FOS,
	RC_INFO_H_THN,
	RC_INFO_H_THB,
	RC_INFO_H_INFRA,
	RC_INFO_M_HB,
	RC_INFO_M_HM,
	RC_INFO_M_WB,

	RC_INFO_M_WM,
	RC_INFO_F_HB,
	RC_INFO_F_HM,
	RC_INFO_F_WB,
	RC_INFO_F_WM,
	RC_INFO_P_HITD_M,

	RC_INFO_H_HITD_M,
	RC_INFO_P_EXP,
	RC_INFO_H_EXP,
	RC_INFO_SYM,
	RC_INFO_AGE,
	RC_INFO_AGE_ADD,
	RC_INFO_P_FLAGS,
	RC_INFO_H_FLAGS,
	RC_INFO_SUIT_CLASS,
	RC_INFO_DESCRIPTION,
	RC_INFO_E_DESCRIPTION,

	RC_INFO_HAND,
	RC_INFO_RING,
	RC_INFO_AMULET,
	RC_INFO_BODY,
	RC_INFO_OUTER,
	RC_INFO_HEAD,
	RC_INFO_ARMS,
	RC_INFO_FEET,
	RC_INFO_TAIL,
	RC_INFO_INTAKE,

	RC_INFO_SP_MELEE,

	RC_INFO_CSV_COLUMNS,
};

static cptr rc_info_csv_list[RC_INFO_CSV_COLUMNS] =
{
	"ID",
	"CM",
	"NAME",
	"E_NAME",
	"SEX",
	"LEV",
	"DR",
	"P_STR",
	"P_INT",
	"P_WIS",

	"P_DEX",
	"P_CON",
	"P_CHA",
	"H_STR",
	"H_INT",
	"H_WIS",
	"H_DEX",
	"H_CON",
	"H_CHA",
	"P_DIS",

	"P_DEV",
	"P_ROB",
	"P_EVA",
	"P_VOL",
	"P_STL",
	"P_SRH",
	"P_FOS",
	"P_THN",
	"P_THB",
	"P_INFRA",
	"H_DIS",
	"H_DEV",

	"H_ROB",
	"H_EVA",
	"H_VOL",
	"H_STL",
	"H_SRH",
	"H_FOS",
	"H_THN",
	"H_THB",
	"H_INFRA",
	"M_HB",
	"M_HM",
	"M_WB",

	"M_WM",
	"F_HB",
	"F_HM",
	"F_WB",
	"F_WM",
	"P_HITD_M",
	"H_HITD_M",
	"P_EXP",
	"H_EXP",
	"SYM",

	"AGE",
	"AGE_ADD",
	"P_FLAGS",
	"H_FLAGS",
	"SUIT_CLASS",
	"DESCRIPTION",
	"E_DESCRIPTION",

	"HAND",
	"RING",
	"AMULET",
	"BODY",
	"OUTER",
	"HEAD",
	"ARMS",
	"FEET",
	"TAIL",
	"INTAKE",

	"SP_MELEE",

};

static enum DUNGEON_INFO rc_info_csv_code[RC_INFO_CSV_COLUMNS];

errr parse_race_info_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b;
	char tmp[10000], nt[80];

	if(get_split_offset(split, size, buf, RC_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, rc_info_csv_list[0]))
	{
		rc_info_csv_code[0] = RC_INFO_ID;
		for(i = 1; i < RC_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < RC_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, rc_info_csv_list[j]))
				{
					rc_info_csv_code[i] = j;
					break;
				}
			}
			if(j == RC_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		race_type *race_ptr;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize RC:%d]", n);

		note(nt);
		race_ptr = &race_info[n];

		for(i = 1; i < RC_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			

			switch(rc_info_csv_code[i])
			{

			case RC_INFO_COMMON:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->race_category = (byte)b;
				break;

			case RC_INFO_NAME:

				if(!add_name(&race_ptr->name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case RC_INFO_E_NAME:
#if JP
				if(!add_name(&race_ptr->E_name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
#else
				if(!add_name(&race_ptr->name, head, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
#endif
				break;

			case RC_INFO_SEX:
				if(sscanf(tmp, "0x%x", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->sex_flag = (byte)b;
				break;

			case RC_INFO_LEV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->lev = (s16b)b;
				break;

			case RC_INFO_DR:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->dr = (s16b)b;
				break;

			case RC_INFO_P_STR:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_adj[STAT_STR] = (s16b)b;
				break;

			case RC_INFO_P_INT:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_adj[STAT_INT] = (s16b)b;
				break;

			case RC_INFO_P_WIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_adj[STAT_WIS] = (s16b)b;
				break;

			case RC_INFO_P_DEX:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_adj[STAT_DEX] = (s16b)b;
				break;

			case RC_INFO_P_CON:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_adj[STAT_CON] = (s16b)b;
				break;

			case RC_INFO_P_CHA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_adj[STAT_CHA] = (s16b)b;
				break;

			case RC_INFO_H_STR:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_adj[STAT_STR] = (s16b)b;
				break;

			case RC_INFO_H_INT:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_adj[STAT_INT] = (s16b)b;
				break;

			case RC_INFO_H_WIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_adj[STAT_WIS] = (s16b)b;
				break;

			case RC_INFO_H_DEX:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_adj[STAT_DEX] = (s16b)b;
				break;

			case RC_INFO_H_CON:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_adj[STAT_CON] = (s16b)b;
				break;

			case RC_INFO_H_CHA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_adj[STAT_CHA] = (s16b)b;
				break;

			case RC_INFO_P_DIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_dis = (s16b)b;
				break;

			case RC_INFO_P_DEV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_dev = (s16b)b;
				break;

			case RC_INFO_P_ROB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_rob = (s16b)b;
				break;

			case RC_INFO_P_EVA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_eva = (s16b)b;
				break;

			case RC_INFO_P_VOL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_vol = (s16b)b;
				break;

			case RC_INFO_P_STL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_stl = (s16b)b;
				break;

			case RC_INFO_P_SRH:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_srh = (s16b)b;
				break;

			case RC_INFO_P_FOS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_fos = (s16b)b;
				break;

			case RC_INFO_P_THN:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_thn = (s16b)b;
				break;

			case RC_INFO_P_THB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_thb = (s16b)b;
				break;

			case RC_INFO_P_INFRA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->infra = (byte)b;
				break;

			case RC_INFO_H_DIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_dis = (s16b)b;
				break;

			case RC_INFO_H_DEV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_dev = (s16b)b;
				break;

			case RC_INFO_H_ROB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_rob = (s16b)b;
				break;

			case RC_INFO_H_EVA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_eva = (s16b)b;
				break;

			case RC_INFO_H_VOL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_vol = (s16b)b;
				break;

			case RC_INFO_H_STL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_stl = (s16b)b;
				break;

			case RC_INFO_H_SRH:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_srh = (s16b)b;
				break;

			case RC_INFO_H_FOS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_fos = (s16b)b;
				break;

			case RC_INFO_H_THN:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_thn = (s16b)b;
				break;

			case RC_INFO_H_THB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_thb = (s16b)b;
				break;

			case RC_INFO_H_INFRA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->infra = (byte)b;
				break;

			case RC_INFO_M_HB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->m_b_ht = (s32b)b;
				break;

			case RC_INFO_M_HM:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->m_m_ht = (s32b)b;
				break;

			case RC_INFO_M_WB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->m_b_wt = (s32b)b;
				break;

			case RC_INFO_M_WM:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->m_m_wt = (s32b)b;
				break;

			case RC_INFO_F_HB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->f_b_ht = (s32b)b;
				break;

			case RC_INFO_F_HM:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->f_m_ht = (s32b)b;
				break;

			case RC_INFO_F_WB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->f_b_wt = (s32b)b;
				break;

			case RC_INFO_F_WM:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->f_m_wt = (s32b)b;
				break;

			case RC_INFO_P_HITD_M:
				//TODO
				break;

			case RC_INFO_H_HITD_M:
				//TODO
				break;

			case RC_INFO_P_EXP:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_exp = (s16b)b;
				break;

			case RC_INFO_H_EXP:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->r_s_exp = (s16b)b;
				break;

			case RC_INFO_SYM:
				race_ptr->symbol = tmp[0];
				break;

			case RC_INFO_AGE:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->b_age = (s32b)b;
				break;

			case RC_INFO_AGE_ADD:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->m_age = (s32b)b;
				break;

			case RC_INFO_P_FLAGS:
				if(0 != traits_precondition_splits(&race_ptr->p_flags, tmp))
					return PARSE_ERROR_GENERIC;
				break;

			case RC_INFO_H_FLAGS:
				if(0 != traits_precondition_splits(&race_ptr->h_flags, tmp))
					return PARSE_ERROR_GENERIC;
				break;

			case RC_INFO_SUIT_CLASS:
				break;

			case RC_INFO_DESCRIPTION:
				if(!add_text(&race_ptr->text, head, tmp, TRUE))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;


			case RC_INFO_E_DESCRIPTION:
#if JP
				if(!add_text(&race_ptr->E_text, head, tmp, TRUE))
					return PARSE_ERROR_OUT_OF_MEMORY;
#else
				if(!add_text(&race_ptr->text, head, tmp, TRUE))
					return PARSE_ERROR_OUT_OF_MEMORY;
#endif
				break;

			case RC_INFO_HAND:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_hand = (s16b)b;
				break;

			case RC_INFO_RING:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_ring = (s16b)b;
				break;

			case RC_INFO_AMULET:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_amulet = (s16b)b;
				break;

			case RC_INFO_BODY:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_body = (s16b)b;
				break;

			case RC_INFO_OUTER:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_outer = (s16b)b;
				break;

			case RC_INFO_HEAD:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_head = (s16b)b;
				break;

			case RC_INFO_ARMS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_arms = (s16b)b;
				break;

			case RC_INFO_FEET:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_feet = (s16b)b;
				break;

			case RC_INFO_TAIL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_tail = (s16b)b;
				break;

			case RC_INFO_INTAKE:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				race_ptr->slot_intake = (s16b)b;
				break;

			case RC_INFO_SP_MELEE:
				parse_special_melee(race_ptr->blow, tmp);
				break;

			default:
				return PARSE_ERROR_GENERIC;

			}
		}
		
	}
	return PARSE_ERROR_NONE;
}


enum CLASS_INFO {
	CL_INFO_ID,
	CL_INFO_TYPE,
	CL_INFO_SELECTABLE,
	CL_INFO_RARITY,
	CL_INFO_NAME,
	CL_INFO_E_NAME,
	CL_INFO_STR,
	CL_INFO_INT,
	CL_INFO_WIS,
	CL_INFO_DEX,
	CL_INFO_CON,
	CL_INFO_CHA,
	CL_INFO_A_STR,
	CL_INFO_A_INT,
	CL_INFO_A_WIS,
	CL_INFO_A_DEX,
	CL_INFO_A_CON,
	CL_INFO_A_CHA,
	CL_INFO_C_DIS,
	CL_INFO_C_DEV,
	CL_INFO_C_SAV,
	CL_INFO_C_STL,
	CL_INFO_C_SRH,
	CL_INFO_C_FOS,
	CL_INFO_C_THN,
	CL_INFO_C_THB,
	CL_INFO_X_DIS,
	CL_INFO_X_DEV,
	CL_INFO_X_SAV,
	CL_INFO_X_STL,
	CL_INFO_X_SRH,
	CL_INFO_X_FOS,
	CL_INFO_X_THN,
	CL_INFO_X_THB,
	CL_INFO_HD,
	CL_INFO_EXP,
	CL_INFO_PET_UPKEEP,
	CL_INFO_FLAGS,
	CL_INFO_DESCRIPTION,
	CL_INFO_E_DESCRIPTION,
	CL_INFO_CSV_COLUMNS,
};

static cptr cl_info_csv_list[CL_INFO_CSV_COLUMNS] =
{
	"ID",
	"TYPE",
	"SELECTABLE",
	"RARITY",
	"NAME",
	"E_NAME",
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHA",
	"A_STR",
	"A_INT",
	"A_WIS",
	"A_DEX",
	"A_CON",
	"A_CHA",
	"C_DIS",
	"C_DEV",
	"C_SAV",
	"C_STL",
	"C_SRH",
	"C_FOS",
	"C_THN",
	"C_THB",
	"X_DIS",
	"X_DEV",
	"X_SAV",
	"X_STL",
	"X_SRH",
	"X_FOS",
	"X_THN",
	"X_THB",
	"HD",
	"EXP",
	"PET_UPKEEP",
	"FLAGS",
	"DESCRIPTION",
	"E_DESCRIPTION",
};
static int cl_info_csv_code[CL_INFO_CSV_COLUMNS];

errr parse_class_info_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b;
	char tmp[10000], nt[80];

	if(get_split_offset(split, size, buf, CL_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, cl_info_csv_list[0]))
	{
		rc_info_csv_code[0] = CL_INFO_ID;
		for(i = 1; i < CL_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < CL_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, cl_info_csv_list[j]))
				{
					cl_info_csv_code[i] = j;
					break;
				}
			}
			if(j == CL_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		class_type *class_ptr;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize CL:%d]", n);

		class_ptr = &class_info[n];


		note(nt);

		for(i = 1; i < CL_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			

			switch(cl_info_csv_code[i])
			{

				case CL_INFO_TYPE:
					if(grab_one_index(&b, class_type_flags, tmp, FALSE)) return PARSE_ERROR_GENERIC;
					class_ptr->type = (byte)b;
					break;

				case CL_INFO_RARITY:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->rarity = (s16b)b;
					break;

				case CL_INFO_SELECTABLE:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->selectable = (byte)b;
					break;

				case CL_INFO_NAME:
					if(!add_name(&class_ptr->name, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case CL_INFO_E_NAME:
#if JP
					if(!add_name(&class_ptr->E_name, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
#else
					if(!add_name(&class_ptr->name, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
#endif
					break;

				case CL_INFO_STR:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj[STAT_STR] = (s16b)b;
					break;

				case CL_INFO_INT:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj[STAT_INT] = (s16b)b;
					break;

				case CL_INFO_WIS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj[STAT_WIS] = (s16b)b;
					break;

				case CL_INFO_DEX:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj[STAT_DEX] = (s16b)b;
					break;

				case CL_INFO_CON:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj[STAT_CON] = (s16b)b;
					break;

				case CL_INFO_CHA:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj[STAT_CHA] = (s16b)b;
					break;

				case CL_INFO_A_STR:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj_b[STAT_STR] = (s16b)b;
					break;

				case CL_INFO_A_INT:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj_b[STAT_INT] = (s16b)b;
					break;

				case CL_INFO_A_WIS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj_b[STAT_WIS] = (s16b)b;
					break;

				case CL_INFO_A_DEX:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj_b[STAT_DEX] = (s16b)b;
					break;

				case CL_INFO_A_CON:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj_b[STAT_CON] = (s16b)b;
					break;

				case CL_INFO_A_CHA:
					break;
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_adj_b[STAT_CHA] = (s16b)b;

				case CL_INFO_C_DIS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_dis = (s16b)b;
					break;

				case CL_INFO_C_DEV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_dev = (s16b)b;
					break;

				case CL_INFO_C_SAV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_sav = (s16b)b;
					break;

				case CL_INFO_C_STL:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_stl = (s16b)b;
					break;

				case CL_INFO_C_SRH:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_srh = (s16b)b;
					break;

				case CL_INFO_C_FOS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_fos = (s16b)b;
					break;

				case CL_INFO_C_THN:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_thn = (s16b)b;
					break;

				case CL_INFO_C_THB:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_thb = (s16b)b;
					break;

				case CL_INFO_X_DIS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->x_dis = (s16b)b;
					break;

				case CL_INFO_X_DEV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->x_dev = (s16b)b;
					break;

				case CL_INFO_X_SAV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->x_sav = (s16b)b;
					break;

				case CL_INFO_X_STL:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->x_stl = (s16b)b;
					break;

				case CL_INFO_X_SRH:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->x_srh = (s16b)b;
					break;

				case CL_INFO_X_FOS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->x_fos = (s16b)b;
					break;

				case CL_INFO_X_THN:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->x_thn = (s16b)b;
					break;

				case CL_INFO_X_THB:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->x_thb = (s16b)b;
					break;

				case CL_INFO_HD:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_mhp = (s16b)b;
					break;

				case CL_INFO_EXP:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->c_exp = (s16b)b;
					break;

				case CL_INFO_PET_UPKEEP:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					class_ptr->pet_upkeep_div = (byte)b;
					break;

				case CL_INFO_FLAGS:
					if(0 != traits_precondition_splits(&class_ptr->flags, tmp))
						return PARSE_ERROR_GENERIC;
					break;

				case CL_INFO_DESCRIPTION:
					if(!add_text(&class_ptr->text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case CL_INFO_E_DESCRIPTION:
					if(!add_text(&class_ptr->E_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

			}
		}
	}

	return PARSE_ERROR_NONE;
}



enum CHARA_INFO {
	CH_INFO_ID,
	CH_INFO_NAME,
	CH_INFO_E_NAME,
	CH_INFO_RARITY,
	CH_INFO_STR,
	CH_INFO_INT,
	CH_INFO_WIS,
	CH_INFO_DEX,
	CH_INFO_CON,
	CH_INFO_CHA,
	CH_INFO_C_DIS,
	CH_INFO_C_DEV,
	CH_INFO_C_SAV,
	CH_INFO_C_STL,
	CH_INFO_C_SRH,
	CH_INFO_C_FOS,
	CH_INFO_C_THN,
	CH_INFO_C_THB,
	CH_INFO_HD,
	CH_INFO_JP_NO,
	CH_INFO_SEX,
	CH_INFO_M_PENA,
	CH_INFO_FLAGS,
	CH_INFO_DESCRIPTION,
	CH_INFO_E_DESCRIPTION,
	CH_INFO_CSV_COLUMNS
};

static int ch_info_csv_code[CH_INFO_CSV_COLUMNS];

static cptr ch_info_csv_list[CH_INFO_CSV_COLUMNS] =
{
	"ID",
	"NAME",
	"E_NAME",
	"RARITY",
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHA",
	"C_DIS",
	"C_DEV",
	"C_SAV",
	"C_STL",
	"C_SRH",
	"C_FOS",
	"C_THN",
	"C_THB",
	"HD",
	"JP_NO",
	"SEX",
	"M_PENA",
	"FLAGS",
	"DESCRIPTION",
	"E_DESCRIPTION",
};

errr parse_chara_info_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b;
	char tmp[10000], nt[80];

	if(get_split_offset(split, size, buf, CH_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, ch_info_csv_list[0]))
	{
		rc_info_csv_code[0] = CH_INFO_ID;
		for(i = 1; i < CH_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < CH_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, ch_info_csv_list[j]))
				{
					ch_info_csv_code[i] = j;
					break;
				}
			}
			if(j == CH_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		chara_type *chara_ptr;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize CH:%d]", n);

		chara_ptr = &chara_info[n];


		note(nt);

		for(i = 1; i < CH_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			

			switch(ch_info_csv_code[i])
			{
				case CH_INFO_NAME:
					if(!add_name(&chara_ptr->name, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case CH_INFO_E_NAME:
#if JP
					if(!add_name(&chara_ptr->E_name, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
#else
					if(!add_name(&chara_ptr->name, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
#endif
					break;

				case CH_INFO_RARITY:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->rarity = (s16b)b;
					break;

				case CH_INFO_STR:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_adj[STAT_STR] = (s16b)b;
					break;

				case CH_INFO_INT:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_adj[STAT_INT] = (s16b)b;
					break;

				case CH_INFO_WIS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_adj[STAT_WIS] = (s16b)b;
					break;

				case CH_INFO_DEX:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_adj[STAT_DEX] = (s16b)b;
					break;

				case CH_INFO_CON:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_adj[STAT_CON] = (s16b)b;
					break;

				case CH_INFO_CHA:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_adj[STAT_CHA] = (s16b)b;
					break;

				case CH_INFO_C_DIS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_dis = (s16b)b;
					break;

				case CH_INFO_C_DEV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_dev = (s16b)b;
					break;

				case CH_INFO_C_SAV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_sav = (s16b)b;
					break;

				case CH_INFO_C_STL:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_stl = (s16b)b;
					break;

				case CH_INFO_C_SRH:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_srh = (s16b)b;
					break;

				case CH_INFO_C_FOS:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_fos = (s16b)b;
					break;

				case CH_INFO_C_THN:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_thn = (s16b)b;
					break;

				case CH_INFO_C_THB:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_thb = (s16b)b;
					break;

				case CH_INFO_HD:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->a_mhp = (s16b)b;
					break;

				case CH_INFO_JP_NO:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->no = (byte)b;
					break;

				case CH_INFO_SEX:
					if(sscanf(tmp, "0x%x", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->sex = (byte)b;
					break;

				case CH_INFO_M_PENA:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					chara_ptr->m_pena = (s16b)b;
					break;

				case CH_INFO_FLAGS:
					if(0 != traits_precondition_splits(&chara_ptr->flags, tmp))
						return PARSE_ERROR_GENERIC;
					break;

				case CH_INFO_DESCRIPTION:
					if(!add_text(&chara_ptr->text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case CH_INFO_E_DESCRIPTION:
					if(!add_text(&chara_ptr->E_text, head, tmp, TRUE))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

			}
		}
	}

	return PARSE_ERROR_NONE;
}


/*
 * Grab one flag for a dungeon type from a textual string
 */
static errr grab_one_dungeon_flag(dungeon_type *d_ptr, cptr what)
{
	if(grab_one_flag(&d_ptr->flags1, dungeon_info_flags1, what) == 0)
		return PARSE_ERROR_NONE;

	/* Oops */
#ifdef JP
	msg_format("未知のダンジョン・フラグ '%s'。", what);
#else
	msg_format("Unknown dungeon type flag '%s'.", what);
#endif

	/* Failure */
	return PARSE_ERROR_GENERIC;
}

enum DUNGEON_INFO {
	DU_INFO_ID,
	DU_INFO_NAME,
	DU_INFO_E_NAME,
	DU_INFO_DY,
	DU_INFO_DX,
	DU_INFO_TEXT,
	DU_INFO_E_TEXT,
	DU_INFO_FEAT_PROB_FLOOR,
	DU_INFO_FEAT_PROB_FILL,
	DU_INFO_OUTER_WALL,
	DU_INFO_INNER_WALL,
	DU_INFO_STREAM1,
	DU_INFO_STREAM2,
	DU_INFO_MINDEPTH,
	DU_INFO_MAXDEPTH,
	DU_INFO_MIN_PLEV,
	DU_INFO_PIT,
	DU_INFO_NEST,
	DU_INFO_MODE,
	DU_INFO_MIN_M_ALLOC_LEVEL,
	DU_INFO_MAX_M_ALLOC_CHANCE,
	DU_INFO_D_FLAGS,
	DU_INFO_C_FLAGS,
	DU_INFO_R_RACE,
	DU_INFO_FINAL_OBJECT,
	DU_INFO_FINAL_ARTIFACT,
	DU_INFO_FINAL_GUARDIAN,
	DU_INFO_SPECIAL_DIV,
	DU_INFO_TUNNEL_PERCENT,
	DU_INFO_OBJ_GREAT,
	DU_INFO_OBJ_GOOD,
	DU_INFO_RACE_POP,
	DU_INFO_VAULT_INFO,
	DU_INFO_PORTAL_INFO,
	DU_INFO_COMMENT,
	DU_INFO_CSV_COLUMNS
};

static int du_info_csv_code[DU_INFO_CSV_COLUMNS];
static cptr du_info_csv_list[DU_INFO_CSV_COLUMNS] =
{
	"ID",
	"NAME",
	"E_NAME",
	"DY",
	"DX",
	"TEXT",
	"E_TEXT",
	"FEAT_PROB_FLOOR",
	"FEAT_PROB_FILL",
	"OUTER_WALL",

	"INNER_WALL",
	"STREAM1",
	"STREAM2",
	"MINDEPTH",
	"MAXDEPTH",
	"MIN_PLEV",
	"PIT",
	"NEST",
	"MODE",
	"MIN_M_ALLOC_LEVEL",

	"MAX_M_ALLOC_CHANCE",
	"D_FLAGS",
	"C_FLAGS",
	"R_RACE",
	"FINAL_OBJECT",
	"FINAL_ARTIFACT",
	"FINAL_GUARDIAN",
	"SPECIAL_DIV",
	"TUNNEL_PERCENT",
	"OBJ_GREAT",

	"OBJ_GOOD",
	"RACE_POP",
	"VAULT_INFO",
	"PORTAL_INFO",
	"COMMENT"
};

errr parse_dungeon_info_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b;
	char tmp[10000], nt[80];
	char *zz[10];
	char *s, *t;

	if(get_split_offset(split, size, buf, DU_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, du_info_csv_list[0]))
	{
		rc_info_csv_code[0] = DU_INFO_ID;
		for(i = 1; i < DU_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < DU_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, du_info_csv_list[j]))
				{
					du_info_csv_code[i] = j;
					break;
				}
			}
			if(j == DU_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN;
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		dungeon_type *dungeon_ptr;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize DU:%d]", n);

		dungeon_ptr = &dungeon_info[n];


		note(nt);

		for(i = 1; i < DU_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			
			switch(du_info_csv_code[i])
			{
				case DU_INFO_NAME:
					if(!add_name(&dungeon_ptr->name, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case DU_INFO_E_NAME:
					if(!add_name(&dungeon_ptr->E_name, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case DU_INFO_DY:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->dy = (byte)b;
					break;

				case DU_INFO_DX:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->dx = (byte)b;
					break;

				case DU_INFO_TEXT:
					if(!add_name(&dungeon_ptr->text, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case DU_INFO_E_TEXT:
					if(!add_name(&dungeon_ptr->E_text, head, tmp))
						return PARSE_ERROR_OUT_OF_MEMORY;
					break;

				case DU_INFO_FEAT_PROB_FLOOR:
					if(tokenize(tmp, DUNGEON_FEAT_PROB_NUM * 2, zz, 0) != (DUNGEON_FEAT_PROB_NUM * 2)) return PARSE_ERROR_GENERIC; // Scan for the values
					dungeon_ptr->floor[0].feat = feature_tag_to_index(zz[0]);
					dungeon_ptr->floor[0].percent = atoi(zz[1]);
					dungeon_ptr->floor[1].feat = feature_tag_to_index(zz[2]);
					dungeon_ptr->floor[1].percent = atoi(zz[3]);
					dungeon_ptr->floor[2].feat = feature_tag_to_index(zz[4]);
					dungeon_ptr->floor[2].percent = atoi(zz[5]);
					break;

				case DU_INFO_FEAT_PROB_FILL:
					if(tokenize(tmp, DUNGEON_FEAT_PROB_NUM * 2, zz, 0) != (DUNGEON_FEAT_PROB_NUM * 2)) return PARSE_ERROR_GENERIC; // Scan for the values
					dungeon_ptr->fill[0].feat = feature_tag_to_index(zz[0]);
					dungeon_ptr->fill[0].percent = atoi(zz[1]);
					dungeon_ptr->fill[1].feat = feature_tag_to_index(zz[2]);
					dungeon_ptr->fill[1].percent = atoi(zz[3]);
					dungeon_ptr->fill[2].feat = feature_tag_to_index(zz[4]);
					dungeon_ptr->fill[2].percent = atoi(zz[5]);
					break;

				case DU_INFO_OUTER_WALL:
					dungeon_ptr->outer_wall = feature_tag_to_index(tmp);
					if(dungeon_ptr->outer_wall < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
					break;

				case DU_INFO_INNER_WALL:
					dungeon_ptr->inner_wall = feature_tag_to_index(tmp);
					if(dungeon_ptr->inner_wall < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
					break;

				case DU_INFO_STREAM1:
					dungeon_ptr->stream1 = feature_tag_to_index(tmp);
					if(dungeon_ptr->stream1 < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
					break;

				case DU_INFO_STREAM2:
					dungeon_ptr->stream2 = feature_tag_to_index(tmp);
					if(dungeon_ptr->stream2 < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
					break;

				case DU_INFO_MINDEPTH:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->mindepth = (s16b)b;
					break;

				case DU_INFO_MAXDEPTH:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->maxdepth = (s16b)b;
					break;

				case DU_INFO_MIN_PLEV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->min_plev = (byte)b;
					break;

				case DU_INFO_PIT:
					if(sscanf(tmp, "0x%x", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->pit = (s16b)b;
					break;

				case DU_INFO_NEST:
					if(sscanf(tmp, "0x%x", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->nest = (s16b)b;
					break;

				case DU_INFO_MODE:
					if(strcmp(tmp, "NONE") == 0) dungeon_ptr->mode = DUNGEON_MODE_NONE;
					if(strcmp(tmp, "AND") == 0) dungeon_ptr->mode = DUNGEON_MODE_AND;
					if(strcmp(tmp, "NAND") == 0) dungeon_ptr->mode = DUNGEON_MODE_NAND;
					if(strcmp(tmp, "OR") == 0) dungeon_ptr->mode = DUNGEON_MODE_OR;
					if(strcmp(tmp, "NOR") == 0) dungeon_ptr->mode = DUNGEON_MODE_NOR;
					break;

				case DU_INFO_MIN_M_ALLOC_LEVEL:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->min_m_alloc_level = (s16b)b;
					break;

				case DU_INFO_MAX_M_ALLOC_CHANCE:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->max_m_alloc_chance = (s16b)b;
					break;

				case DU_INFO_D_FLAGS:
					/* Parse every entry */
					for (s = tmp; *s; )
					{
						/* Find the end of this entry */
						for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

						/* Nuke and skip any dividers */
						if(*t)
						{
							*t++ = '\0';
							while (*t == ' ' || *t == '|') t++;
						}

						/* Parse this entry */
						if(0 != grab_one_dungeon_flag(dungeon_ptr, s)) return PARSE_ERROR_INVALID_FLAG;

						/* Start the next entry */
						s = t;
					}
					break;

				case DU_INFO_C_FLAGS:
					break;

				case DU_INFO_R_RACE:
					break;

				case DU_INFO_FINAL_OBJECT:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->final_object = (s16b)b;
					break;

				case DU_INFO_FINAL_ARTIFACT:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->final_artifact = (s16b)b;
					break;

				case DU_INFO_FINAL_GUARDIAN:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->final_artifact = (s16b)b;
					break;

				case DU_INFO_SPECIAL_DIV:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->special_div = (byte)b;
					break;

				case DU_INFO_TUNNEL_PERCENT:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->tunnel_percent = (byte)b;
					break;

				case DU_INFO_OBJ_GREAT:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->obj_great = (byte)b;
					break;

				case DU_INFO_OBJ_GOOD:
					if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
					dungeon_ptr->obj_good = (byte)b;
					break;

				case DU_INFO_RACE_POP:
					break;

				case DU_INFO_VAULT_INFO:
					break;

				case DU_INFO_PORTAL_INFO:
					break;

				case DU_INFO_COMMENT:
					break;
			}
		}
	}
	return(0);
}


enum AUTHORITY_INFO
{
	AU_INFO_ID,
	AU_INFO_NAME,
	AU_INFO_E_NAME,
	AU_INFO_A_STR,
	AU_INFO_A_INT,
	AU_INFO_A_WIS,
	AU_INFO_A_DEX,
	AU_INFO_A_CON,
	AU_INFO_A_CHA,
	AU_INFO_W_STR,
	AU_INFO_W_INT,
	AU_INFO_W_WIS,
	AU_INFO_W_DEX,
	AU_INFO_W_CON,
	AU_INFO_W_CHA,
	AU_INFO_A_DIS,
	AU_INFO_A_DEV,
	AU_INFO_A_SAV,
	AU_INFO_A_STL,
	AU_INFO_A_SRH,
	AU_INFO_A_FOS,
	AU_INFO_A_THN,
	AU_INFO_A_THB,
	AU_INFO_A_MHP,
	AU_INFO_W_DIS,
	AU_INFO_W_DEV,
	AU_INFO_W_SAV,
	AU_INFO_W_STL,
	AU_INFO_W_SRH,
	AU_INFO_W_FOS,
	AU_INFO_W_THN,
	AU_INFO_W_THB,
	AU_INFO_W_MHP,
	AU_INFO_RANK,
	AU_INFO_ID_E,
	AU_INFO_DV,
	AU_INFO_CP,
	AU_INFO_FLAGS,
	AU_INFO_CSV_COLUMNS
};

static cptr au_info_csv_list[AU_INFO_CSV_COLUMNS] =
{
	"ID",
	"NAME",
	"E_NAME",
	"A_STR",
	"A_INT",
	"A_WIS",
	"A_DEX",
	"A_CON",
	"A_CHA",
	"W_STR",
	"W_INT",
	"W_WIS",
	"W_DEX",
	"W_CON",
	"W_CHA",
	"A_DIS",
	"A_DEV",
	"A_SAV",
	"A_STL",
	"A_SRH",
	"A_FOS",
	"A_THN",
	"A_THB",
	"A_MHP",
	"W_DIS",
	"W_DEV",
	"W_SAV",
	"W_STL",
	"W_SRH",
	"W_FOS",
	"W_THN",
	"W_THB",
	"W_MHP",
	"RANK",
    "ID_E",
    "DR",
    "CAP",
    "FLAGS",
};

static int au_info_csv_code[AU_INFO_CSV_COLUMNS];

errr parse_authority_info_csv(char *buf, header *head)
{
	int split[80], size[80];
	int i, j, b;
	char tmp[10000], nt[80];

	if(get_split_offset(split, size, buf, AU_INFO_CSV_COLUMNS, ',', '"')){
		return PARSE_ERROR_GENERIC;
	}

	strncpy(tmp, buf + split[0], size[0]);
	tmp[size[0]] = '\0';

	if(!strcmp(tmp, au_info_csv_list[0]))
	{
		au_info_csv_code[0] = AU_INFO_ID;
		for(i = 1; i < AU_INFO_CSV_COLUMNS; i++)
		{
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			for(j = 1; j < AU_INFO_CSV_COLUMNS; j++)
			{
				if(!strcmp(tmp, au_info_csv_list[j]))
				{
					au_info_csv_code[i] = j;
					break;
				}
			}
			if(j == AU_INFO_CSV_COLUMNS) return PARSE_ERROR_COLUMN; /* ERROR */
		}
		return PARSE_ERROR_NONE;
	}
	else
	{
		int n;
		strncpy(tmp, buf + split[0], size[0]);
		tmp[size[0]] = '\0';
		sscanf(tmp, "%d", &n);
		sprintf(nt, "[Initialize Authority:%d]", n);


		note(nt);

		for(i = 1; i < AU_INFO_CSV_COLUMNS; i++)
		{
			
			strncpy(tmp, buf + split[i], size[i]);
			tmp[size[i]] = '\0';
			

			switch(au_info_csv_code[i])
			{
			case AU_INFO_NAME:

				if(!strcpy(authority_info[n].title, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case AU_INFO_E_NAME:
#if JP
				if(!strcpy(authority_info[n].E_title, tmp)) return PARSE_ERROR_OUT_OF_MEMORY;
#else
				if(!strcpy(&authority_info[n].title, tmp)) return PARSE_ERROR_OUT_OF_MEMORY;
#endif
				break;

			case AU_INFO_A_STR:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_adj[STAT_STR] = (s16b)b;
				break;

			case AU_INFO_A_INT:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_adj[STAT_INT] = (s16b)b;
				break;

			case AU_INFO_A_WIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_adj[STAT_WIS] = (s16b)b;
				break;

			case AU_INFO_A_DEX:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_adj[STAT_DEX] = (s16b)b;
				break;

			case AU_INFO_A_CON:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_adj[STAT_CON] = (s16b)b;
				break;

			case AU_INFO_A_CHA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_adj[STAT_CHA] = (s16b)b;
				break;

			case AU_INFO_W_STR:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_adj[STAT_STR] = (s16b)b;
				break;

			case AU_INFO_W_INT:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_adj[STAT_INT] = (s16b)b;
				break;

			case AU_INFO_W_WIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_adj[STAT_WIS] = (s16b)b;
				break;

			case AU_INFO_W_DEX:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_adj[STAT_DEX] = (s16b)b;
				break;

			case AU_INFO_W_CON:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_adj[STAT_CON] = (s16b)b;
				break;

			case AU_INFO_W_CHA:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_adj[STAT_CHA] = (s16b)b;
				break;

			case AU_INFO_A_DIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_dis = (s16b)b;
				break;

			case AU_INFO_A_DEV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_dev = (s16b)b;
				break;

			case AU_INFO_A_SAV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_sav = (s16b)b;
				break;

			case AU_INFO_A_STL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_stl = (s16b)b;
				break;

			case AU_INFO_A_SRH:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_srh = (s16b)b;
				break;

			case AU_INFO_A_FOS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_fos = (s16b)b;
				break;

			case AU_INFO_A_THN:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_thn = (s16b)b;
				break;

			case AU_INFO_A_THB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_thb = (s16b)b;
				break;

			case AU_INFO_A_MHP:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].a_mhp = (s16b)b;
				break;

			case AU_INFO_W_DIS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_dis = (s16b)b;
				break;

			case AU_INFO_W_DEV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_dev = (s16b)b;
				break;

			case AU_INFO_W_SAV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_sav = (s16b)b;
				break;

			case AU_INFO_W_STL:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_stl = (s16b)b;
				break;

			case AU_INFO_W_SRH:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_srh = (s16b)b;
				break;

			case AU_INFO_W_FOS:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_fos = (s16b)b;
				break;

			case AU_INFO_W_THN:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_thn = (s16b)b;
				break;

			case AU_INFO_W_THB:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_thb = (s16b)b;
				break;

			case AU_INFO_W_MHP:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].w_mhp = (s16b)b;
				break;

			case AU_INFO_RANK:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].rank = (byte)b;
				break;

			case AU_INFO_ID_E:
				if(!strcpy(authority_info[n].id_e, tmp))
					return PARSE_ERROR_OUT_OF_MEMORY;
				break;

			case AU_INFO_DV:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].dv = (s16b)b;
				break;

			case AU_INFO_CP:
				if(sscanf(tmp, "%d", &b) != 1) return PARSE_ERROR_GENERIC;
				authority_info[n].cp = (s16b)b;
				break;

			case AU_INFO_FLAGS:
				break;

			default:
				return PARSE_ERROR_GENERIC;

			}
		}
		
	}
	return PARSE_ERROR_NONE;
}

/* Random dungeon grid effects */
#define RANDOM_NONE         0x00000000
#define RANDOM_FEATURE      0x00000001
#define RANDOM_MONSTER      0x00000002
#define RANDOM_OBJECT       0x00000004
#define RANDOM_EGO          0x00000008
#define RANDOM_ARTIFACT     0x00000010
#define RANDOM_TRAP         0x00000020


typedef struct dungeon_grid dungeon_grid;

struct dungeon_grid
{
	int		feature;		/* Terrain feature */
	int		creature;		/* Creature */
	int		object;			/* Object */
	int		ego;			/* Ego-Item */
	int		artifact;		/* Artifact */
	int		trap;			/* Trap */
	int		cave_info;		/* Flags for CAVE_MARK, CAVE_GLOW, CAVE_ICKY, CAVE_ROOM */
	int		special;		/* Reserved for special terrain info */
	int		random;			/* Number of the random effect */
};


static dungeon_grid letter[255];


/*
 * Process "F:<letter>:<terrain>:<cave_info>:<creature>:<object>:<ego>:<artifact>:<trap>:<special>" -- info for dungeon grid
 */
static errr parse_line_feature(char *buf, u32b flags)
{
	int num;
	char *zz[9];


	if(flags & INIT_ONLY_BUILDINGS) return PARSE_ERROR_NONE;

	/* Tokenize the line */
	if((num = tokenize(buf+2, 9, zz, 0)) > 1)
	{
		/* Letter to assign */
		int index = zz[0][0];

		/* Reset the info for the letter */
		letter[index].feature = feat_none;
		letter[index].creature = 0;
		letter[index].object = 0;
		letter[index].ego = 0;
		letter[index].artifact = 0;
		letter[index].trap = feat_none;
		letter[index].cave_info = 0;
		letter[index].special = 0;
		letter[index].random = RANDOM_NONE;

		switch (num)
		{
			/* Special */
			case 9:
				letter[index].special = atoi(zz[8]);
				/* Fall through */
			/* Trap */
			case 8:
				if((zz[7][0] == '*') && !zz[7][1])
				{
					letter[index].random |= RANDOM_TRAP;
				}
				else
				{
					letter[index].trap = feature_tag_to_index(zz[7]);
					if(letter[index].trap < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
				}
				/* Fall through */
			/* Artifact */
			case 7:
				if(zz[6][0] == '*')
				{
					letter[index].random |= RANDOM_ARTIFACT;
					if(zz[6][1]) letter[index].artifact = atoi(zz[6] + 1);
				}
				else
				{
					letter[index].artifact = atoi(zz[6]);
				}
				/* Fall through */
			/* Ego-item */
			case 6:
				if(zz[5][0] == '*')
				{
					letter[index].random |= RANDOM_EGO;
					if(zz[5][1]) letter[index].ego = atoi(zz[5] + 1);
				}
				else
				{
					letter[index].ego = atoi(zz[5]);
				}
				/* Fall through */
			/* Object */
			case 5:
				if(zz[4][0] == '*')
				{
					letter[index].random |= RANDOM_OBJECT;
					if(zz[4][1]) letter[index].object = atoi(zz[4] + 1);
				}
				else
				{
					letter[index].object = atoi(zz[4]);
				}
				/* Fall through */
			/* Creature */
			case 4:
				if(zz[3][0] == '*')
				{
					letter[index].random |= RANDOM_MONSTER;
					if(zz[3][1]) letter[index].creature = atoi(zz[3] + 1);
				}
				else if(zz[3][0] == 'c')
				{
					if(!zz[3][1]) return PARSE_ERROR_GENERIC;
					letter[index].creature = - atoi(zz[3] + 1);
				}
				else
				{
					letter[index].creature = atoi(zz[3]);
				}
				/* Fall through */
			/* Cave info */
			case 3:
				letter[index].cave_info = atoi(zz[2]);
				/* Fall through */
			/* Feature */
			case 2:
				if((zz[1][0] == '*') && !zz[1][1])
				{
					letter[index].random |= RANDOM_FEATURE;
				}
				else
				{
					letter[index].feature = feature_tag_to_index(zz[1]);
					if(letter[index].feature < 0) return PARSE_ERROR_UNDEFINED_TERRAIN_TAG;
				}
				break;
		}

		return PARSE_ERROR_NONE;
	}

	return PARSE_ERROR_GENERIC;
}


/*
 * Process "B:<Index>:<Command>:..." -- Building definition
 */
static errr parse_line_building(char *buf)
{
	int i;
	char *zz[80];
	int index;
	char *s;

#ifdef JP
	if(buf[2] == '$')
		return PARSE_ERROR_NONE;
	s = buf + 2;
#else
	if(buf[2] != '$')
		return PARSE_ERROR_NONE;
	s = buf + 3;
#endif
	/* Get the building number */
	index = atoi(s);

	/* Find the colon after the building number */
	s = my_strchr(s, ':');

	/* Verify that colon */
	if(!s) return PARSE_ERROR_GENERIC;

	/* Nuke the colon, advance to the sub-index */
	*s++ = '\0';

	/* Paranoia -- require a sub-index */
	if(!*s) return PARSE_ERROR_GENERIC;

	/* Building definition sub-index */
	switch (s[0])
	{
		/* Building name, owner, race */
		case 'N':
		{
			if(tokenize(s + 2, 3, zz, 0) == 3)
			{
				/* Name of the building */
				strcpy(building[index].name, zz[0]);

				/* Name of the owner */
				strcpy(building[index].ownespecies_name, zz[1]);

				/* Race of the owner */
				strcpy(building[index].owner_race, zz[2]);

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		/* Building Action */
		case 'A':
		{
			int num;
			if((num = tokenize(s + 2, 8, zz, 0)) >= 7)
			{
				/* Index of the action */
				int action_index = atoi(zz[0]);

				/* Name of the action */
				strcpy(building[index].act_names[action_index], zz[1]);

				/* Cost of the action for members */
				building[index].member_costs[action_index] = atoi(zz[2]);

				/* Cost of the action for non-members */
				building[index].other_costs[action_index] = atoi(zz[3]);

				/* Letter assigned to the action */
				building[index].letters[action_index] = zz[4][0];

				/* Action code */
				building[index].actions[action_index] = atoi(zz[5]);

				/* Action restriction */
				building[index].action_restr[action_index] = atoi(zz[6]);

				/* Action restriction */
				if(num >= 8) building[index].action_misc[action_index] = atoi(zz[7]);


				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		/* Building Classes */
		case 'C':
		{
			char *zz[MAX_CLASS];
			int n;

			n = tokenize(buf+2, MAX_CLASS, zz, 0);
			for (i = 0; i < n; i++)
			{
				building[index].member_class[i] = atoi(zz[i]);
			}
			if(n < MAX_RACES)
			{
				for (i = n; i < MAX_CLASS ; i++)
				building[index].member_class[i] = 0;
			}
		}

		/* Building Races */
		case 'R':
		{
			char *zz[MAX_RACES];
			int n;

			n = tokenize(buf+2, MAX_RACES, zz, 0);
			for (i = 0; i < n; i++)
			{
				building[index].member_race[i] = atoi(zz[i]);
			}
			if(n < MAX_RACES)
			{
				for (i = n; i < MAX_RACES ; i++)
				building[index].member_race[i] = 0;
			}

		}

		/* Building Realms */
		case 'M':
		{
			if(tokenize(s+2, MAX_MAGIC, zz, 0) == MAX_MAGIC)
			{
				for (i = 0; i < MAX_MAGIC; i++)
				{
					building[index].member_realm[i+1] = atoi(zz[i]);
				}

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		case 'Z':
		{
			/* Ignore scripts */
			break;
		}

		default:
		{
			return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
		}
	}

	return PARSE_ERROR_NONE;
}


/*
 * Place the object j_ptr to a grid
 */
static void drop_here(floor_type *floor_ptr, object_type *j_ptr, int y, int x)
{
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	object_type *object_ptr;

	// Get new object
	s16b object_idx = object_pop();

	// Access new object
	object_ptr = &object_list[object_idx];

	// Structure copy
	object_copy(object_ptr, j_ptr);

	// Locate
	object_ptr->fy = y;
	object_ptr->fx = x;

	// No creature
	object_ptr->held_m_idx = 0;

	// Build a stack
	object_ptr->next_object_idx = c_ptr->object_idx;

	// Place the object
	c_ptr->object_idx = object_idx;
}


/*
 * Parse a sub-file of the "extra info"
 */
static errr process_dungeon_file_aux(floor_type *floor_ptr, char *buf, int ymin, int xmin, int ymax, int xmax, int *y, int *x, u32b flags, int quest_id)
{
	int i;

	char *zz[100];


	/* Skip "empty" lines */
	if(!buf[0]) return PARSE_ERROR_NONE;

	/* Skip "blank" lines */
	if(isspace(buf[0])) return PARSE_ERROR_NONE;

	/* Skip comments */
	if(buf[0] == '#') return PARSE_ERROR_NONE;

	/* Require "?:*" format */
	if(buf[1] != ':') return PARSE_ERROR_GENERIC;


	/* Process "%:<fname>" */
	if(buf[0] == '%')
	{
		// Attempt to Process the given file
		return (process_dungeon_file(floor_ptr, buf + 2, ymin, xmin, ymax, xmax, flags, quest_id));
	}

	/* Process "F:<letter>:<terrain>:<cave_info>:<creature>:<object>:<ego>:<artifact>:<trap>:<special>" -- info for dungeon grid */
	if(buf[0] == 'F')
	{
		return parse_line_feature(buf, flags);
	}

	/* Process "D:<dungeon>" -- info for the cave grids */
	else if(buf[0] == 'D')
	{
		object_type object_type_body;

		/* Acquire the text */
		char *s = buf + 2;

		/* Length of the text */
		int len = strlen(s);

		if(flags & INIT_ONLY_BUILDINGS) return PARSE_ERROR_NONE;

		for (*x = xmin, i = 0; ((*x < xmax) && (i < len)); (*x)++, s++, i++)
		{
			/* Access the grid */
			cave_type *c_ptr = &floor_ptr->cave[*y][*x];

			int idx = s[0];

//			c_ptr->special = 11;

			int object_index = letter[idx].object;
			int creature_index = letter[idx].creature;
			int random = letter[idx].random;
			int artifact_index = letter[idx].artifact;

			/* Lay down a floor */
			c_ptr->feat = conv_dungeon_feat(floor_ptr, letter[idx].feature);

			/* Only the features */
			if(flags & INIT_ONLY_FEATURES) continue;

			/* Cave info */
			c_ptr->info = letter[idx].cave_info;

			/* Create a creature */
			if(random & RANDOM_MONSTER)
			{
				floor_ptr->creature_level = floor_ptr->base_level + creature_index;
				place_creature(NULL, floor_ptr, *y, *x, (PC_ALLOW_SLEEP | PC_ALLOW_GROUP));
				floor_ptr->creature_level = floor_ptr->base_level;
			}
			else if(creature_index)
			{
				int old_cur_num, old_max_num;
				bool clone = FALSE;

				if(creature_index < 0)
				{
					creature_index = -creature_index;
					clone = TRUE;
				}
				old_cur_num = species_info[creature_index].cur_num;
				old_max_num = species_info[creature_index].max_num;

				/* Make alive again */
				if(has_trait_species(&species_info[creature_index], TRAIT_UNIQUE))
				{
					species_info[creature_index].cur_num = 0;
					species_info[creature_index].max_num = 1;
				}

				/* Make alive again */
				/* Hack -- Non-unique Nazguls are semi-unique */
				else if(has_trait_raw(&species_info[creature_index].flags, TRAIT_NAZGUL))
				{
					if(species_info[creature_index].cur_num == species_info[creature_index].max_num)
					{
						species_info[creature_index].max_num++;
					}
				}

				/* Place it */
				place_creature_species(NULL, floor_ptr, *y, *x, creature_index, (PC_ALLOW_SLEEP | PC_NO_KAGE));
				if(clone)
				{
					/* clone */
					creature_list[hack_m_idx_ii].smart |= SM_CLONED;

					/* Make alive again for real unique creature */
					species_info[creature_index].cur_num = old_cur_num;
					species_info[creature_index].max_num = old_max_num;
				}
			}

			/* Object (and possible trap) */
			if((random & RANDOM_OBJECT) && (random & RANDOM_TRAP))
			{
				floor_ptr->object_level = floor_ptr->base_level + object_index;

				// Random trap and random treasure defined
				// 25% chance for trap and 75% chance for object
				if(randint0(100) < 75) place_object(floor_ptr, *y, *x, 0L, NULL);
				else place_trap(floor_ptr, *y, *x);

				floor_ptr->object_level = floor_ptr->base_level;
			}
			else if(random & RANDOM_OBJECT)
			{
				floor_ptr->object_level = floor_ptr->base_level + object_index;

				// Create an out of deep object
				if(randint0(100) < 75) place_object(floor_ptr, *y, *x, 0L, NULL);
				else if(randint0(100) < 80) place_object(floor_ptr, *y, *x, AM_GOOD, NULL);
				else place_object(floor_ptr, *y, *x, AM_GOOD | AM_GREAT, NULL);

				floor_ptr->object_level = floor_ptr->base_level;
			}
			/* Random trap */
			else if(random & RANDOM_TRAP)
			{
				place_trap(floor_ptr, *y, *x);
			}
			/* Hidden trap (or door) */
			else if(letter[idx].trap)
			{
				c_ptr->mimic = c_ptr->feat;
				c_ptr->feat = conv_dungeon_feat(floor_ptr, letter[idx].trap);
			}
			else if(object_index)
			{
				/* Get local object */
				object_type *object_ptr = &object_type_body;

				/* Create the item */
				object_prep(object_ptr, object_index, ITEM_FREE_SIZE);

				if(object_ptr->tval == TV_GOLD)
				{
					make_gold(floor_ptr, object_ptr, letter[idx].special, object_index - OBJ_GOLD_LIST);
				}

				/* Apply magic (no messages, no artifacts) */
				apply_magic(player_ptr, object_ptr, floor_ptr->base_level, AM_NO_FIXED_ART | AM_GOOD, 0);

				drop_here(floor_ptr, object_ptr, *y, *x);
			}

			/* Artifact */
			if(artifact_index)
			{
				if(artifact_info[artifact_index].cur_num)
				{
					int k_idx = lookup_kind(TV_SCROLL, SV_SCROLL_ACQUIREMENT);
					object_type forge;
					object_type *quest_ptr = &forge;

					object_prep(quest_ptr, k_idx, ITEM_FREE_SIZE);

					/* Drop it in the dungeon */
					drop_here(floor_ptr, quest_ptr, *y, *x);
				}
				else
				{
					/* Create the artifact */
					if(drop_named_art(player_ptr, artifact_index, *y, *x))
						artifact_info[artifact_index].cur_num = 1;
				}
			}

			/* Terrain special */
			c_ptr->special = letter[idx].special;
		}

		(*y)++;

		return PARSE_ERROR_NONE;
	}

	/* Process "Q:<number>:<command>:... -- quest info */
	else if(buf[0] == 'Q')
	{
		int num;
		quest_type *quest_ptr;
#ifdef JP
		if(buf[2] == '$') return PARSE_ERROR_NONE;
		num = tokenize(buf + 2, 33, zz, 0);
#else
		if(buf[2] != '$') return PARSE_ERROR_NONE;
		num = tokenize(buf + 3, 33, zz, 0);
#endif

		/* Have we enough parameters? */
		if(num < 3) return (PARSE_ERROR_TOO_FEW_ARGUMENTS);

		quest_ptr = &(quest[atoi(zz[0])]); // Get the quest

		/* Process "Q:<q_index>:Q:<type>:<num_mon>:<cur_num>:<max_num>:<level>:<species_idx>:<k_idx>:<flags>" -- quest info */
		if(zz[1][0] == 'Q')
		{
			if(flags & INIT_ASSIGN)
			{
				species_type *r_ptr;
				artifact_type *a_ptr;

				if(num < 9) return (PARSE_ERROR_TOO_FEW_ARGUMENTS);

				quest_ptr->type    = atoi(zz[2]);
				quest_ptr->num_mon = atoi(zz[3]);
				quest_ptr->cur_num = atoi(zz[4]);
				quest_ptr->max_num = atoi(zz[5]);
				quest_ptr->level   = atoi(zz[6]);
				quest_ptr->species_idx   = atoi(zz[7]);
				quest_ptr->k_idx   = atoi(zz[8]);
				quest_ptr->dungeon = atoi(zz[9]);

				if(num > 10) quest_ptr->flags  = atoi(zz[10]);

				r_ptr = &species_info[quest_ptr->species_idx];
				//if(has_trait_species(r_ptr, TRAIT_UNIQUE))
					//TODO r_ptr->flags1 |= RF1_QUESTOR;

				a_ptr = &artifact_info[quest_ptr->k_idx];
				add_flag(a_ptr->flags, TRAIT_QUESTITEM);
			}
			return PARSE_ERROR_NONE;
		}

		/* Process "Q:<q_index>:N:<name>" -- quest name */
		else if(zz[1][0] == 'N')
		{
			if(flags & (INIT_ASSIGN | INIT_SHOW_TEXT)) strcpy(quest_ptr->name, zz[2]);
			return PARSE_ERROR_NONE;
		}

		/* Process "Q:<q_index>:T:<text>" -- quest description line */
		else if(zz[1][0] == 'T')
		{
			if(flags & INIT_SHOW_TEXT)
			{
				strcpy(questp_text[questp_text_line], zz[2]);
				questp_text_line++;
			}

			return PARSE_ERROR_NONE;
		}
	}

	/* Process "W:<command>: ..." -- info for the wilderness */
	else if(buf[0] == 'W')
	{
		return parse_line_wilderness(buf, ymin, xmin, ymax, xmax, y, x);
	}

	/* Process "P:<y>:<x>" -- player position */
	else if(buf[0] == 'P')
	{
		if(flags & INIT_CREATE_DUNGEON)
		{
			if(tokenize(buf + 2, 2, zz, 0) == 2)
			{
				int panels_x, panels_y;

				/* Hack - Set the dungeon size */
				panels_y = (*y / SCREEN_HGT);
				if(*y % SCREEN_HGT) panels_y++;
				floor_ptr->height = panels_y * SCREEN_HGT;

				panels_x = (*x / SCREEN_WID);
				if(*x % SCREEN_WID) panels_x++;
				floor_ptr->width = panels_x * SCREEN_WID;

				/* Assume illegal panel */
				panel_row_min = floor_ptr->height;
				panel_col_min = floor_ptr->width;

				/* Place player in a quest level */
				if(quest)
				{
					int y, x;

					/* Delete the creature (if any) */
					delete_creature(floor_ptr, player_ptr->fy, player_ptr->fx);

					y = atoi(zz[0]);
					x = atoi(zz[1]);

					player_ptr->fy = y;
					player_ptr->fx = x;
				}
				/* Place player in the town */
				else if(!player_ptr->oldpx && !player_ptr->oldpy)
				{
					player_ptr->oldpy = atoi(zz[0]);
					player_ptr->oldpx = atoi(zz[1]);
				}
			}
		}

		return PARSE_ERROR_NONE;
	}

	/* Process "B:<Index>:<Command>:..." -- Building definition */
	else if(buf[0] == 'B')
	{
		return parse_line_building(buf);
	}

	/* Process "R:<RaceRate>" -- set Race Bias */
	else if(buf[0] == 'R')
	{
		char *zz[MAX_RACES];
		int n;

		n = tokenize(buf+2, MAX_RACES, zz, 0);
		for (i = 0; i < n; i++)
		{
			floor_ptr->race_population[i] = atoi(zz[i]);
		}
		if(n < MAX_RACES)
		{
			for (i = n; i < MAX_RACES ; i++) floor_ptr->race_population[i] = 0;
		}
		return PARSE_ERROR_NONE;

	}

	/* Process "M:<type>:<maximum>" -- set maximum values */
	else if(buf[0] == 'M')
	{
		if(tokenize(buf+2, 2, zz, 0) == 2)
		{
			/* Maximum towns */
			if(zz[0][0] == 'T')
			{
				max_towns = atoi(zz[1]);
			}

			/* Maximum quests */
			else if(zz[0][0] == 'Q')
			{
				max_quests = atoi(zz[1]);
			}

			/* Maximum species_idx */
			else if(zz[0][0] == 'R')
			{
				/* Maximum species_idx */
				if(zz[0][1] == 'E')
				{
					max_creature_ego_idx = atoi(zz[1]);
				}
				else
				{
					max_species_idx = atoi(zz[1]);
				}
			}

			/* Maximum k_idx */
			else if(zz[0][0] == 'K')
			{
				max_object_kind_idx = atoi(zz[1]);
			}

			/* Maximum v_idx */
			else if(zz[0][0] == 'V')
			{
				max_vault_idx = atoi(zz[1]);
			}

			/* Maximum f_idx */
			else if(zz[0][0] == 'F')
			{
				max_feature_idx = atoi(zz[1]);
			}

			/* Maximum a_idx */
			else if(zz[0][0] == 'A')
			{
				/* Maximum species_idx */
				if(zz[0][1] == 'U')
				{
					max_authorities_idx = atoi(zz[1]);
				}
				else
				{
					max_artifact_idx = atoi(zz[1]);
				}
			}

			/* Maximum e_idx */
			else if(zz[0][0] == 'E')
			{
				max_object_ego_idx = atoi(zz[1]);
			}

			/* Maximum d_idx */
			else if(zz[0][0] == 'D')
			{
				max_dungeon_idx = atoi(zz[1]); 
			}

			/* Maximum object_idx */
			else if(zz[0][0] == 'O')
			{
				max_object_idx = atoi(zz[1]);
			}

			/* Maximum m_idx */
			else if(zz[0][0] == 'M')
			{
				max_creature_idx = atoi(zz[1]);
			}

			/* Maximum m_idx */
			else if(zz[0][0] == 'C')
			{
				max_trait_idx = atoi(zz[1]);
			}

			/* Wilderness size */
			else if(zz[0][0] == 'W')
			{
				/* Maximum wild_x_size */
				if(zz[0][1] == 'X')
					max_wild_x = atoi(zz[1]);
				/* Maximum wild_y_size */
				if(zz[0][1] == 'Y')
					max_wild_y = atoi(zz[1]);
			}

			/* Maximum species_idx */
			else if(zz[0][0] == 'S')
			{
				/* Maximum species_idx */
				if(zz[0][1] == 'T')
				{
					max_store_idx = atoi(zz[1]);
				}
			}

			return PARSE_ERROR_NONE;
		}
	}

	return PARSE_ERROR_GENERIC;
}


static char tmp[8];
static cptr variant = "D\'ANGBAND";


/*
 * Helper function for "process_dungeon_file()"
 */
static cptr process_dungeon_file_expr(floor_type *floor_ptr, char **sp, char *fp)
{
	cptr v;
	char *b;
	char *s;
	char b1 = '[';
	char b2 = ']';
	char f = ' ';

	s = (*sp);	// Initial

	/* Skip spaces */
	while (isspace(*s)) s++;

	/* Save start */
	b = s;

	/* Default */
	v = "?o?o?";

	/* Analyze */
	if(*s == b1)
	{
		const char *p;
		const char *t;

		/* Skip b1 */
		s++;

		/* First */
		t = process_dungeon_file_expr(floor_ptr, &s, &f);

		/* Oops */
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
				t = process_dungeon_file_expr(floor_ptr, &s, &f);
				if(*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		else if(streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(floor_ptr, &s, &f);
				if(*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		else if(streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(floor_ptr, &s, &f);
				if(*t && streq(t, "1")) v = "0";
			}
		}

		/* Function: EQU */
		else if(streq(t, "EQU"))
		{
			v = "0";
			if(*s && (f != b2))
			{
				t = process_dungeon_file_expr(floor_ptr, &s, &f);
			}
			while (*s && (f != b2))
			{
				p = process_dungeon_file_expr(floor_ptr, &s, &f);
				if(streq(t, p)) v = "1";
			}
		}

		/* Function: LEQ */
		else if(streq(t, "LEQ"))
		{
			v = "1";
			if(*s && (f != b2))
			{
				t = process_dungeon_file_expr(floor_ptr, &s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_dungeon_file_expr(floor_ptr, &s, &f);
				if(*t && atoi(p) > atoi(t)) v = "0";
			}
		}

		/* Function: GEQ */
		else if(streq(t, "GEQ"))
		{
			v = "1";
			if(*s && (f != b2))
			{
				t = process_dungeon_file_expr(floor_ptr, &s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_dungeon_file_expr(floor_ptr, &s, &f);

				/* Compare two numbers instead of string */
				if(*t && atoi(p) < atoi(t)) v = "0";
			}
		}

		/* Oops */
		else
		{
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(floor_ptr, &s, &f);
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
		while (iskanji(*s) || (isprint(*s) && !my_strchr(" []", *s)))
		{
			if(iskanji(*s)) s++;
			s++;
		}
#else
		while (isprint(*s) && !my_strchr(" []", *s)) ++s;
#endif

		/* Extract final and Terminate */
		if((f = *s) != '\0') *s++ = '\0';

		// Variable
		if(*b == '$')
		{
			// System
			if(streq(b+1, "SYS"))
			{
				v = ANGBAND_SYS;
			}

			// Graphics
			else if(streq(b+1, "GRAF"))
			{
				v = ANGBAND_GRAF;
			}

			else if(streq(b+1, "MONOCHROME"))
			{
				if(arg_monochrome)
					v = "ON";
				else
					v = "OFF";
			}

			// Race1
			else if(streq(b+1, "RACE1"))
			{
#ifdef JP
				v = race_info[player_ptr->race_idx1].E_title;
#else
				v = race_info[player_ptr->race_idx1].title;
#endif
			}

			// Race2
			else if(streq(b+1, "RACE2"))
			{
#ifdef JP
				v = race_info[player_ptr->race_idx2].E_title;
#else
				v = race_info[player_ptr->race_idx2].title;
#endif
			}

			// Class
			else if(streq(b+1, "CLASS"))
			{
#ifdef JP
				v = class_info[player_ptr->class_idx].E_title;
#else
				v = class_info[player_ptr->class_idx].title;
#endif
			}

			// First realm
			else if(streq(b+1, "REALM1"))
			{
#ifdef JP
				v = E_realm_names[player_ptr->realm1];
#else
				v = realm_names[player_ptr->realm1];
#endif
			}

			// Second realm
			else if(streq(b+1, "REALM2"))
			{
#ifdef JP
				v = E_realm_names[player_ptr->realm2];
#else
				v = realm_names[player_ptr->realm2];
#endif
			}

			// Player name
			else if(streq(b+1, "PLAYER"))
			{
				static char tmp_playespecies_name[128];
				char *pn, *tpn;
				for (pn = player_ptr->name, tpn = tmp_playespecies_name; *pn; pn++, tpn++)
				{
#ifdef JP
					if(iskanji(*pn))
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

			/* Town */
			else if(streq(b+1, "TOWN"))
			{
				sprintf(tmp, "%d", floor_ptr->town_num);
				v = tmp;
			}

			/* Level */
			else if(streq(b+1, "LEVEL"))
			{
				sprintf(tmp, "%d", player_ptr->lev);
				v = tmp;
			}

			/* Current quest number */
			else if(streq(b+1, "QUEST_NUMBER"))
			{
				sprintf(tmp, "%d", floor_list[player_ptr->floor_id].quest);
				v = tmp;
			}

			/* Number of last quest */
			else if(streq(b+1, "LEAVING_QUEST"))
			{
				sprintf(tmp, "%d", leaving_quest);
				v = tmp;
			}

			/* Quest status */
			else if(prefix(b+1, "QUEST"))
			{
				/* "QUEST" uses a special parameter to determine the number of the quest */
				sprintf(tmp, "%d", quest[atoi(b+6)].status);
				v = tmp;
			}

			/* Random */
			else if(prefix(b+1, "RANDOM"))
			{
				/* "RANDOM" uses a special parameter to determine the number of the quest */
				sprintf(tmp, "%d", (int)(seed_town%atoi(b+7)));
				v = tmp;
			}

			/* Variant name */
			else if(streq(b+1, "VARIANT"))
			{
				v = variant;
			}

			/* Wilderness */
			else if(streq(b+1, "WILDERNESS"))
			{
				sprintf(tmp, "NORMAL");
				v = tmp;
			}
		}

		/* Constant */
		else
		{
			v = b;
		}
	}

	/* Save */
	(*fp) = f;

	/* Save */
	(*sp) = s;

	/* Result */
	return (v);
}


errr process_dungeon_file(floor_type *floor_ptr, cptr name, int ymin, int xmin, int ymax, int xmax, u32b flags, int quest_id)
{
	FILE *fp;
	char buf[1024];
	int num = -1;
	errr err = 0;
	bool bypass = FALSE;
	int x = xmin, y = ymin;

	path_build(buf, sizeof(buf), ANGBAND_DIR_EDIT, name);	// Build the filename
	fp = my_fopen(buf, "r");	// Open the file
	if(!fp) return (-1);		// No such file

	while (0 == my_fgets(fp, buf, sizeof(buf)))	// Process the file
	{
		num++;	// Count lines

		/* Skip "empty" lines */
		if(!buf[0]) continue;

		/* Skip "blank" lines */
		if(isspace(buf[0])) continue;

		/* Skip comments */
		if(buf[0] == '#') continue;

		/* Process "?:<expr>" */
		if((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			cptr v;
			char *s;

			/* Start */
			s = buf + 2;

			/* Parse the expr */
			v = process_dungeon_file_expr(floor_ptr, &s, &f);

			/* Set flag */
			bypass = (streq(v, "0") ? TRUE : FALSE);

			/* Continue */
			continue;
		}

		/* Apply conditionals */
		if(bypass) continue;

		// Process the line
		err = process_dungeon_file_aux(floor_ptr, buf, ymin, xmin, ymax, xmax, &y, &x, flags, quest_id);

		if(err) break;
	}

	// Errors
	if(err)
	{
		cptr oops = (((err > 0) && (err < PARSE_ERROR_MAX)) ? err_str[err] : "unknown");
		msg_format("Error %d (%s) at line %d of '%s'.", err, oops, num, name);
#ifdef JP
		msg_format("'%s'を解析中。", buf);
#else
		msg_format("Parsing '%s'.", buf);
#endif
		msg_print(NULL);
	}

	my_fclose(fp);
	return (err);
}
