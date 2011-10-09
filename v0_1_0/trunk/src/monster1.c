/* File: monster1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: describe monsters (using monster memory) */

#include "angband.h"


/*
 * Pronoun arrays, by gender.
 */
static cptr wd_he[3] =
#ifdef JP
{ "それ", "彼", "彼女" };
#else
{ "it", "he", "she" };
#endif

static cptr wd_his[3] =
#ifdef JP
{ "それの", "彼の", "彼女の" };
#else
{ "its", "his", "her" };
#endif



/*
 * Pluralizer.  Args(count, singular, plural)
 */
#define plural(c,s,p) \
    (((c) == 1) ? (s) : (p))






/*
 * Determine if the "armor" is known
 * The higher the level, the fewer kills needed.
 */
static bool know_armour(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	s32b level = r_ptr->level;

	s32b kills = r_ptr->r_tkills;

	if (cheat_know) return (TRUE);

	/* Normal monsters */
	if (kills > 304 / (4 + level)) return (TRUE);

	/* Skip non-uniques */
	if (!(is_unique_species(r_ptr))) return (FALSE);

	/* Unique monsters */
	if (kills > 304 / (38 + (5 * level) / 4)) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Determine if the "damage" of the given attack is known
 * the higher the level of the monster, the fewer the attacks you need,
 * the more damage an attack does, the more attacks you need
 */
static bool know_damage(int species_idx, int i)
{
	species_type *r_ptr = &species_info[species_idx];

	s32b level = r_ptr->level;

	s32b a = r_ptr->r_blows[i];

	s32b d1 = r_ptr->blow[i].d_dice;
	s32b d2 = r_ptr->blow[i].d_side;

	s32b d = d1 * d2;

	if (d >= ((4+level)*MAX_UCHAR)/80) d = ((4+level)*MAX_UCHAR-1)/80;

	/* Normal monsters */
	if ((4 + level) * a > 80 * d) return (TRUE);

	/* Skip non-uniques */
	if (!(is_unique_species(r_ptr))) return (FALSE);

	/* Unique monsters */
	if ((4 + level) * (2 * a) > 80 * d) return (TRUE);

	/* Assume false */
	return (FALSE);
}


/*
 * Prepare hook for c_roff(). It will be changed for spoiler generation in wizard1.c.
 */
void (*hook_c_roff)(byte attr, cptr str) = c_roff;

static void hooked_roff(cptr str)
{
	/* Spawn */
	hook_c_roff(TERM_WHITE, str);
}


/*
 * Hack -- display monster information using "hooked_roff()"
 *
 * This function should only be called with the cursor placed at the
 * left edge of the screen, on a cleared line, in which the recall is
 * to take place.  One extra blank line is left after the recall.
 */
static void roff_aux(creature_type *pl_ptr, species_type *sp_ptr, int mode)
{
	bool            old = FALSE;
	int             m, n, r;
	cptr            p, q;

#ifdef JP
	char            jverb_buf[64];
#else
	bool            sin = FALSE;
#endif
	int             msex = 0;

	bool nightmare = ironman_nightmare && !(mode & 0x02);
	int speed = nightmare ? sp_ptr->speed + 5 : sp_ptr->speed;

	bool            breath = FALSE;
	bool            magic = FALSE;

	u32b		flags1;
	u32b		flags2;
	u32b		flags3;
	u32b		flags4;
	u32b		flags5;
	u32b		flags6;
	u32b		flags7;
	u32b		flags10;

	byte drop_gold, drop_item;

	int		vn = 0;
	byte		color[96];
	cptr		vp[96];

	bool know_everything = FALSE;

	/* Obtain a copy of the "known" number of drops */
	drop_gold = sp_ptr->r_drop_gold;
	drop_item = sp_ptr->r_drop_item;

	/* Obtain a copy of the "known" flags */
	flags1 = (sp_ptr->flags1 & sp_ptr->r_flags1);
	flags2 = (sp_ptr->flags2 & sp_ptr->r_flags2);
	flags3 = (sp_ptr->flags3 & sp_ptr->r_flags3);
	flags4 = (sp_ptr->flags4 & sp_ptr->r_flags4);
	flags5 = (sp_ptr->flags5 & sp_ptr->r_flags5);
	flags6 = (sp_ptr->flags6 & sp_ptr->r_flags6);
	flags7 = (sp_ptr->flags7 & sp_ptr->flags7);
	flags10 = (sp_ptr->flags10 & sp_ptr->r_flags10);

	/* cheat_know or research_mon() */
	if (cheat_know || (mode & 0x01))
		know_everything = TRUE;

	/* Cheat -- Know everything */
	if (know_everything)
	{
		/* Hack -- maximal drops */
		drop_gold = drop_item = 0;

		/* Hack -- but only "valid" drops */
		if (sp_ptr->flags1 & RF1_ONLY_GOLD) drop_item = 0;
		if (sp_ptr->flags1 & RF1_ONLY_ITEM) drop_gold = 0;

		/* Hack -- know all the flags */
		flags1 = sp_ptr->flags1;
		flags2 = sp_ptr->flags2;
		flags3 = sp_ptr->flags3;
		flags4 = sp_ptr->flags4;
		flags5 = sp_ptr->flags5;
		flags6 = sp_ptr->flags6;
		flags10 = sp_ptr->flags10;
	}


	/* Extract a gender (if applicable) */
	if (is_female_species(sp_ptr)) msex = 2;
	else if (is_male_species(sp_ptr)) msex = 1;

	/* Assume some "obvious" flags */
	//if (is_unique_species(sp_ptr))  flags1 |= (RF1_UNIQUE);
	if (sp_ptr->flags1 & RF1_QUESTOR) flags1 |= (RF1_QUESTOR);
	//if (sp_ptr->flags1 & RF1_MALE)    flags1 |= (RF1_MALE);
	//if (sp_ptr->flags1 & RF1_FEMALE)  flags1 |= (RF1_FEMALE);

	/* Assume some "creation" flags */
	if (sp_ptr->flags1 & RF1_FRIENDS) flags1 |= (RF1_FRIENDS);
	if (sp_ptr->flags1 & RF1_ESCORT)  flags1 |= (RF1_ESCORT);
	if (sp_ptr->flags1 & RF1_ESCORTS) flags1 |= (RF1_ESCORTS);

	/* Killing a monster reveals some properties */
	if (sp_ptr->r_tkills || know_everything)
	{
		/* Know "race" flags */
		//TODO
		//if (sp_ptr->flags3 & RF3_ORC)      flags3 |= (RF3_ORC);
		//if (sp_ptr->flags3 & RF3_TROLL)    flags3 |= (RF3_TROLL);
		//if (sp_ptr->flags3 & RF3_GIANT)    flags3 |= (RF3_GIANT);
		//if (sp_ptr->flags3 & RF3_DRAGON)   flags3 |= (RF3_DRAGON);
		//if (sp_ptr->flags3 & RF3_DEMON)    flags3 |= (RF3_DEMON);
		//if (sp_ptr->flags3 & RF3_UNDEAD)   flags3 |= (RF3_UNDEAD);
		//if (sp_ptr->flags3 & RF3_EVIL)     flags3 |= (RF3_EVIL);
		//if (sp_ptr->flags3 & RF3_GOOD)     flags3 |= (RF3_GOOD);
		//if (sp_ptr->flags3 & RF3_ANIMAL)   flags3 |= (RF3_ANIMAL);
		if (sp_ptr->flags3 & RF3_PUELLA_MAGI) flags3 |= (RF3_PUELLA_MAGI);
		//if (sp_ptr->flags2 & RF2_HUMAN)    flags2 |= (RF2_HUMAN);

		/* Know 'quantum' flag */
		if (sp_ptr->flags2 & RF2_QUANTUM)  flags2 |= (RF2_QUANTUM);

		/* Know "forced" flags */
		if (sp_ptr->flags1 & RF1_FORCE_DEPTH) flags1 |= (RF1_FORCE_DEPTH);
		if (sp_ptr->flags1 & RF1_FORCE_MAXHP) flags1 |= (RF1_FORCE_MAXHP);
	}

	/* For output_monster_spoiler() */
	if (mode & 0x02)
	{
		/* Nothing to do */
	}
	else

	/* Treat uniques differently */
	if (flags1 & RF1_UNIQUE)
	{
		/* Hack -- Determine if the unique is "dead" */
		bool dead = (sp_ptr->max_num == 0) ? TRUE : FALSE;

		/* We've been killed... */
		if (sp_ptr->r_deaths)
		{
			/* Killed ancestors */
#ifdef JP
			hooked_roff(format("%^sはあなたの先祖を %d 人葬っている",
					   wd_he[msex], sp_ptr->r_deaths));
#else
			hooked_roff(format("%^s has slain %d of your ancestors",
					   wd_he[msex], sp_ptr->r_deaths));
#endif


			/* But we've also killed it */
			if (dead)
			{
#ifdef JP
				hooked_roff(format("が、すでに仇討ちは果たしている！"));
#else
				hooked_roff(format(", but you have avenged %s!  ",
					    plural(sp_ptr->r_deaths, "him", "them")));
#endif

			}

			/* Unavenged (ever) */
			else
			{
#ifdef JP
				hooked_roff(format("のに、まだ仇討ちを果たしていない。"));
#else
				hooked_roff(format(", who %s unavenged.  ",
					    plural(sp_ptr->r_deaths, "remains", "remain")));
#endif

			}

			/* Start a new line */
			hooked_roff("\n");
		}

		/* Dead unique who never hurt us */
		else if (dead)
		{
#ifdef JP
			hooked_roff("あなたはこの仇敵をすでに葬り去っている。");
#else
			hooked_roff("You have slain this foe.  ");
#endif

			/* Start a new line */
			hooked_roff("\n");
		}
	}

	/* Not unique, but killed us */
	else if (sp_ptr->r_deaths)
	{
		/* Dead ancestors */
#ifdef JP
		hooked_roff(format("このモンスターはあなたの先祖を %d 人葬っている",
			    sp_ptr->r_deaths ));
#else
		hooked_roff(format("%d of your ancestors %s been killed by this creature, ",
			    sp_ptr->r_deaths, plural(sp_ptr->r_deaths, "has", "have")));
#endif


		/* Some kills this life */
		if (sp_ptr->r_pkills)
		{
#ifdef JP
			hooked_roff(format("が、あなたはこのモンスターを少なくとも %d 体は倒している。", sp_ptr->r_pkills));
#else
			hooked_roff(format("and you have exterminated at least %d of the creatures.  ", sp_ptr->r_pkills));
#endif

		}

		/* Some kills past lives */
		else if (sp_ptr->r_tkills)
		{
#ifdef JP
			hooked_roff(format("が、%sはこのモンスターを少なくとも %d 体は倒している。",
				    "あなたの先祖", sp_ptr->r_tkills));
#else
			hooked_roff(format("and %s have exterminated at least %d of the creatures.  ",
				    "your ancestors", sp_ptr->r_tkills));
#endif

		}

		/* No kills */
		else
		{
#ifdef JP
			hooked_roff(format("が、まだ%sを倒したことはない。", wd_he[msex]));
#else
			hooked_roff(format("and %s is not ever known to have been defeated.  ", wd_he[msex]));
#endif

		}

		/* Start a new line */
		hooked_roff("\n");
	}

	/* Normal monsters */
	else
	{
		/* Killed some this life */
		if (sp_ptr->r_pkills)
		{
#ifdef JP
			hooked_roff(format("あなたはこのモンスターを少なくとも %d 体は殺している。", sp_ptr->r_pkills));
#else
			hooked_roff(format("You have killed at least %d of these creatures.  ", sp_ptr->r_pkills));
#endif

		}

		/* Killed some last life */
		else if (sp_ptr->r_tkills)
		{
#ifdef JP
			hooked_roff(format("あなたの先祖はこのモンスターを少なくとも %d 体は殺している。", sp_ptr->r_tkills));
#else
			hooked_roff(format("Your ancestors have killed at least %d of these creatures.  ", sp_ptr->r_tkills));
#endif

		}

		/* Killed none */
		else
		{
#ifdef JP
			hooked_roff("このモンスターを倒したことはない。");
#else
			hooked_roff("No battles to the death are recalled.  ");
#endif
		}

		/* Start a new line */
		hooked_roff("\n");
	}

	/* Descriptions */
	{
		cptr tmp = r_text + sp_ptr->text;

		if (tmp[0])
		{
			/* Dump it */
			hooked_roff(tmp);

			/* Start a new line */
			hooked_roff("\n");
		}
	}

	if (sp_ptr->species_idx == MON_KAGE)
	{
		/* All done */
		hooked_roff("\n");

		return;
	}

	/* Nothing yet */
	old = FALSE;

	/* Describe location */
	if (sp_ptr->level == 0)
	{
#ifdef JP
		hooked_roff(format("%^sは町に住み", wd_he[msex]));
#else
		hooked_roff(format("%^s lives in the town", wd_he[msex]));
#endif

		old = TRUE;
	}
	else if (sp_ptr->r_tkills || know_everything)
	{
		if (depth_in_feet)
		{
#ifdef JP
			hooked_roff(format("%^sは通常地下 %d フィートで出現し",
#else
			hooked_roff(format("%^s is normally found at depths of %d feet",
#endif

				    wd_he[msex], sp_ptr->level * 50));
		}
		else
		{
#ifdef JP
			hooked_roff(format("%^sは通常地下 %d 階で出現し",
#else
			hooked_roff(format("%^s is normally found on dungeon level %d",
#endif

				    wd_he[msex], sp_ptr->level));
		}
		old = TRUE;
	}


	/* Describe movement */
	if (sp_ptr->species_idx == MON_CHAMELEON)
	{
#ifdef JP
		hooked_roff("、他のモンスターに化ける。");
#else
		hooked_roff("and can take the shape of other monster.");
#endif
		return;
	}
	else
	{
		/* Introduction */
		if (old)
		{
#ifdef JP
			hooked_roff("、");
#else
			hooked_roff(", and ");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^sは", wd_he[msex]));
#else
			hooked_roff(format("%^s ", wd_he[msex]));
#endif

			old = TRUE;
		}
#ifndef JP
		hooked_roff("moves");
#endif

		/* Random-ness */
		if ((flags1 & RF1_RAND_50) || (flags1 & RF1_RAND_25))
		{
			/* Adverb */
			if ((flags1 & RF1_RAND_50) && (flags1 & RF1_RAND_25))
			{
#ifdef JP
				hooked_roff("かなり");
#else
				hooked_roff(" extremely");
#endif

			}
			else if (flags1 & RF1_RAND_50)
			{
#ifdef JP
				hooked_roff("幾分");
#else
				hooked_roff(" somewhat");
#endif

			}
			else if (flags1 & RF1_RAND_25)
			{
#ifdef JP
				hooked_roff("少々");
#else
				hooked_roff(" a bit");
#endif

			}

			/* Adjective */
#ifdef JP
			hooked_roff("不規則に");
#else
			hooked_roff(" erratically");
#endif


			/* Hack -- Occasional conjunction */
#ifdef JP
			if (speed != 110) hooked_roff("、かつ");
#else
			if (speed != 110) hooked_roff(", and");
#endif

		}

		/* Speed */
		if (speed > 110)
		{
#ifdef JP
			if (speed > 139) hook_c_roff(TERM_RED, "信じ難いほど");
			else if (speed > 134) hook_c_roff(TERM_ORANGE, "猛烈に");
			else if (speed > 129) hook_c_roff(TERM_ORANGE, "非常に");
			else if (speed > 124) hook_c_roff(TERM_UMBER, "かなり");
			else if (speed < 120) hook_c_roff(TERM_L_UMBER, "やや");
			hook_c_roff(TERM_L_RED, "素早く");
#else
			if (speed > 139) hook_c_roff(TERM_RED, " incredibly");
			else if (speed > 134) hook_c_roff(TERM_ORANGE, " extremely");
			else if (speed > 129) hook_c_roff(TERM_ORANGE, " very");
			else if (speed > 124) hook_c_roff(TERM_UMBER, " fairly");
			else if (speed < 120) hook_c_roff(TERM_L_UMBER, " somewhat");
			hook_c_roff(TERM_L_RED, " quickly");
#endif

		}
		else if (speed < 110)
		{
#ifdef JP
			if (speed < 90) hook_c_roff(TERM_L_GREEN, "信じ難いほど");
			else if (speed < 95) hook_c_roff(TERM_BLUE, "非常に");
			else if (speed < 100) hook_c_roff(TERM_BLUE, "かなり");
			else if (speed > 104) hook_c_roff(TERM_GREEN, "やや");
			hook_c_roff(TERM_L_BLUE, "ゆっくりと");
#else
			if (speed < 90) hook_c_roff(TERM_L_GREEN, " incredibly");
			else if (speed < 95) hook_c_roff(TERM_BLUE, " very");
			else if (speed < 100) hook_c_roff(TERM_BLUE, " fairly");
			else if (speed > 104) hook_c_roff(TERM_GREEN, " somewhat");
			hook_c_roff(TERM_L_BLUE, " slowly");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff("普通の速さで");
#else
			hooked_roff(" at normal speed");
#endif

		}
#ifdef JP
		hooked_roff("動いている");
#endif
	}

	/* The code above includes "attack speed" */
	if (flags1 & RF1_NEVER_MOVE)
	{
		/* Introduce */
		if (old)
		{
#ifdef JP
			hooked_roff("、しかし");
#else
			hooked_roff(", but ");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^sは", wd_he[msex]));
#else
			hooked_roff(format("%^s ", wd_he[msex]));
#endif

			old = TRUE;
		}

		/* Describe */
#ifdef JP
		hooked_roff("侵入者を追跡しない");
#else
		hooked_roff("does not deign to chase intruders");
#endif

	}

	/* End this sentence */
	if (old)
	{
#ifdef JP
		hooked_roff("。");
#else
		hooked_roff(".  ");
#endif

		old = FALSE;
	}


	/* Describe experience if known */
	if (sp_ptr->r_tkills || know_everything)
	{
		/* Introduction */
#ifdef JP
		hooked_roff("この");
#else
		if (flags1 & RF1_UNIQUE)
		{
			hooked_roff("Killing this");
		}
		else
		{
			hooked_roff("A kill of this");
		}
#endif


/*TODO: DESC*/
		hooked_roff(desc_race_name(pl_ptr));


#ifdef JP
		hooked_roff("を倒すことは");
#endif
		/* Group some variables */
		{
			long i, j;

#ifdef JP
			i = pl_ptr->lev;
			hooked_roff(format(" %lu レベルのキャラクタにとって", (long)i));

			i = (long)sp_ptr->mexp * sp_ptr->level / (pl_ptr->max_plv+2);
			j = ((((long)sp_ptr->mexp * sp_ptr->level % (pl_ptr->max_plv+2)) *
			       (long)1000 / (pl_ptr->max_plv+2) + 5) / 10);

			hooked_roff(format(" 約%ld.%02ld ポイントの経験となる。",
				(long)i, (long)j ));
#else
			/* calculate the integer exp part */
			i = (long)sp_ptr->mexp * sp_ptr->level / (pl_ptr->max_plv+2);

			/* calculate the fractional exp part scaled by 100, */
			/* must use long arithmetic to avoid overflow  */
			j = ((((long)sp_ptr->mexp * sp_ptr->level % (pl_ptr->max_plv+2)) *
			       (long)1000 / (pl_ptr->max_plv+2) + 5) / 10);

			/* Mention the experience */
			hooked_roff(format(" is worth about %ld.%02ld point%s",
				    (long)i, (long)j,
				    (((i == 1) && (j == 0)) ? "" : "s")));

			/* Take account of annoying English */
			p = "th";
			i = pl_ptr->lev % 10;
			if ((pl_ptr->lev / 10) == 1) /* nothing */;
			else if (i == 1) p = "st";
			else if (i == 2) p = "nd";
			else if (i == 3) p = "rd";

			/* Take account of "leading vowels" in numbers */
			q = "";
			i = pl_ptr->lev;
			if ((i == 8) || (i == 11) || (i == 18)) q = "n";

			/* Mention the dependance on the player's level */
			hooked_roff(format(" for a%s %lu%s level character.  ",
				    q, (long)i, p));
#endif

		}
	}

	if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_ELEC) && (flags3 & RF3_AURA_COLD))
	{
#ifdef JP
		hook_c_roff(TERM_VIOLET, format("%^sは炎と氷とスパークに包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_VIOLET, format("%^s is surrounded by flames, ice and electricity.  ", wd_he[msex]));
#endif
	}
	else if ((flags2 & RF2_AURA_FIRE) && (flags2 & RF2_AURA_ELEC))
	{
#ifdef JP
		hook_c_roff(TERM_L_RED, format("%^sは炎とスパークに包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_RED, format("%^s is surrounded by flames and electricity.  ", wd_he[msex]));
#endif
	}
	else if ((flags2 & RF2_AURA_FIRE) && (flags3 & RF3_AURA_COLD))
	{
#ifdef JP
		hook_c_roff(TERM_BLUE, format("%^sは炎と氷に包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_BLUE, format("%^s is surrounded by flames and ice.  ", wd_he[msex]));
#endif
	}
	else if ((flags3 & RF3_AURA_COLD) && (flags2 & RF2_AURA_ELEC))
	{
#ifdef JP
		hook_c_roff(TERM_L_GREEN, format("%^sは氷とスパークに包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_GREEN, format("%^s is surrounded by ice and electricity.  ", wd_he[msex]));
#endif
	}
	else if (flags2 & RF2_AURA_FIRE)
	{
#ifdef JP
		hook_c_roff(TERM_RED, format("%^sは炎に包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_RED, format("%^s is surrounded by flames.  ", wd_he[msex]));
#endif
	}
	else if (flags3 & RF3_AURA_COLD)
	{
#ifdef JP
		hook_c_roff(TERM_BLUE, format("%^sは氷に包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_BLUE, format("%^s is surrounded by ice.  ", wd_he[msex]));
#endif
	}
	else if (flags2 & RF2_AURA_ELEC)
	{
#ifdef JP
		hook_c_roff(TERM_L_BLUE, format("%^sはスパークに包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_BLUE, format("%^s is surrounded by electricity.  ", wd_he[msex]));
#endif
	}

	if (flags2 & RF2_REFLECTING)
	{
#ifdef JP
		hooked_roff(format("%^sは矢の呪文を跳ね返す。", wd_he[msex]));
#else
		hooked_roff(format("%^s reflects bolt spells.  ", wd_he[msex]));
#endif

	}

	/* Describe escorts */
	if ((flags1 & RF1_ESCORT) || (flags1 & RF1_ESCORTS) || (sp_ptr->underling_id[0] != 0))
	{
	    int i = 0;
#ifdef JP
		hooked_roff(format("%^sは護衛を率いておりその構成は",
#else
		hooked_roff(format("%^s usually appears with escorts ",
#endif
        wd_he[msex]));

		while(sp_ptr->underling_id[i] != 0)
		{
			char *name = r_name + species_info[sp_ptr->underling_id[i]].name;
#ifdef JP
			hooked_roff(format("、「%s」が %dd%d 体", name, sp_ptr->underling_d_num[i], sp_ptr->underling_d_side[i]));
#else
			hooked_roff(format(", %dd%d %s", sp_ptr->underling_d_num[i], sp_ptr->underling_d_side[i], name));
#endif
			i++;
		}

		if ((flags1 & RF1_ESCORT) || (flags1 & RF1_ESCORTS))
		{
#ifdef JP
			if(sp_ptr->underling_id[0] != 0)
				hooked_roff(format("そして"));
			hooked_roff(format("多くの同族"));
#else
			if(sp_ptr->underling_id[0] != 0)
				hooked_roff(format(" and"));
			hooked_roff(format(" some the same races"));
#endif

		}

#ifdef JP
		hooked_roff(format("である。"));
#else
		hooked_roff(format("."));
#endif



	}






	/* Describe friends */
	else if (flags1 & RF1_FRIENDS)
	{
#ifdef JP
		hooked_roff(format("%^sは通常集団で現れる。",
#else
		hooked_roff(format("%^s usually appears in groups.  ",
#endif

			    wd_he[msex]));
	}


	/* Collect inate attacks */
	vn = 0;
#ifdef JP
	if (flags4 & RF4_SHRIEK)  {vp[vn] = "悲鳴で助けを求める";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & RF4_SHRIEK)  {vp[vn] = "shriek for help";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flags4 & RF4_ROCKET)  {vp[vn] = "ロケットを発射する";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & RF4_ROCKET)  {vp[vn] = "shoot a rocket";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags4 & RF4_SHOOT) {vp[vn] = "射撃をする";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & RF4_SHOOT) {vp[vn] = "fire an arrow";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags6 & (RF6_SPECIAL)) {vp[vn] = "特別な行動をする";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_SPECIAL)) {vp[vn] = "do something";color[vn++] = TERM_VIOLET;}
#endif

	/* Describe inate attacks */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
#ifdef JP
			if (n != vn - 1)
			{
				jverb(vp[n], jverb_buf, JVERB_OR);
				hook_c_roff(color[n], jverb_buf);
				hook_c_roff(color[n], "り");
				hooked_roff("、");
			}
			else hook_c_roff(color[n], vp[n]);
#else
			/* Intro */
			if (n == 0) hooked_roff(" may ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" or ");

			/* Dump */
			hook_c_roff(color[n], vp[n]);
#endif

		}

		/* End */
#ifdef JP
		hooked_roff("ことがある。");
#else
		hooked_roff(".  ");
#endif

	}


	/* Collect breaths */
	vn = 0;
#ifdef JP
	if (flags4 & (RF4_BR_ACID))		{vp[vn] = "酸";color[vn++] = TERM_GREEN;}
#else
	if (flags4 & (RF4_BR_ACID))		{vp[vn] = "acid";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_ELEC))		{vp[vn] = "稲妻";color[vn++] = TERM_BLUE;}
#else
	if (flags4 & (RF4_BR_ELEC))		{vp[vn] = "lightning";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_FIRE))		{vp[vn] = "火炎";color[vn++] = TERM_RED;}
#else
	if (flags4 & (RF4_BR_FIRE))		{vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_COLD))		{vp[vn] = "冷気";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & (RF4_BR_COLD))		{vp[vn] = "frost";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_POIS))		{vp[vn] = "毒";color[vn++] = TERM_L_GREEN;}
#else
	if (flags4 & (RF4_BR_POIS))		{vp[vn] = "poison";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NETH))		{vp[vn] = "地獄";color[vn++] = TERM_L_DARK;}
#else
	if (flags4 & (RF4_BR_NETH))		{vp[vn] = "nether";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_LITE))		{vp[vn] = "閃光";color[vn++] = TERM_YELLOW;}
#else
	if (flags4 & (RF4_BR_LITE))		{vp[vn] = "light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DARK))		{vp[vn] = "暗黒";color[vn++] = TERM_L_DARK;}
#else
	if (flags4 & (RF4_BR_DARK))		{vp[vn] = "darkness";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_CONF))		{vp[vn] = "混乱";color[vn++] = TERM_L_UMBER;}
#else
	if (flags4 & (RF4_BR_CONF))		{vp[vn] = "confusion";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_SOUN))		{vp[vn] = "轟音";color[vn++] = TERM_ORANGE;}
#else
	if (flags4 & (RF4_BR_SOUN))		{vp[vn] = "sound";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_CHAO))		{vp[vn] = "カオス";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_CHAO))		{vp[vn] = "chaos";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DISE))		{vp[vn] = "劣化";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_DISE))		{vp[vn] = "disenchantment";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NEXU))		{vp[vn] = "因果混乱";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BR_NEXU))		{vp[vn] = "nexus";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_TIME))		{vp[vn] = "時間逆転";color[vn++] = TERM_L_BLUE;}
#else
	if (flags4 & (RF4_BR_TIME))		{vp[vn] = "time";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_INER))		{vp[vn] = "遅鈍";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_INER))		{vp[vn] = "inertia";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_GRAV))		{vp[vn] = "重力";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_GRAV))		{vp[vn] = "gravity";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_SHAR))		{vp[vn] = "破片";color[vn++] = TERM_L_UMBER;}
#else
	if (flags4 & (RF4_BR_SHAR))		{vp[vn] = "shards";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_PLAS))		{vp[vn] = "プラズマ";color[vn++] = TERM_L_RED;}
#else
	if (flags4 & (RF4_BR_PLAS))		{vp[vn] = "plasma";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_WALL))		{vp[vn] = "フォース";color[vn++] = TERM_UMBER;}
#else
	if (flags4 & (RF4_BR_WALL))		{vp[vn] = "force";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_MANA))		{vp[vn] = "魔力";color[vn++] = TERM_L_BLUE;}
#else
	if (flags4 & (RF4_BR_MANA))		{vp[vn] = "mana";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_NUKE))		{vp[vn] = "放射性廃棄物";color[vn++] = TERM_L_GREEN;}
#else
	if (flags4 & (RF4_BR_NUKE))		{vp[vn] = "toxic waste";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
	if (flags4 & (RF4_BR_DISI))		{vp[vn] = "分解";color[vn++] = TERM_SLATE;}
#else
	if (flags4 & (RF4_BR_DISI))		{vp[vn] = "disintegration";color[vn++] = TERM_SLATE;}
#endif


	/* Describe breaths */
	if (vn)
	{
		/* Note breath */
		breath = TRUE;

		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("や");
#else
			if (n == 0) hooked_roff(" may breathe ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}
#ifdef JP
		hooked_roff("のブレスを吐くことがある");
#endif
	}


	/* Collect spells */
	vn = 0;
#ifdef JP
if (flags5 & (RF5_BA_ACID))         {vp[vn] = "アシッド・ボール";color[vn++] = TERM_GREEN;}
#else
	if (flags5 & (RF5_BA_ACID))         {vp[vn] = "produce acid balls";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_ELEC))         {vp[vn] = "サンダー・ボール";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BA_ELEC))         {vp[vn] = "produce lightning balls";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_FIRE))         {vp[vn] = "ファイア・ボール";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BA_FIRE))         {vp[vn] = "produce fire balls";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_COLD))         {vp[vn] = "アイス・ボール";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_BA_COLD))         {vp[vn] = "produce frost balls";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_POIS))         {vp[vn] = "悪臭雲";color[vn++] = TERM_L_GREEN;}
#else
	if (flags5 & (RF5_BA_POIS))         {vp[vn] = "produce poison balls";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_NETH))         {vp[vn] = "地獄球";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BA_NETH))         {vp[vn] = "produce nether balls";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_WATE))         {vp[vn] = "ウォーター・ボール";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BA_WATE))         {vp[vn] = "produce water balls";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags4 & (RF4_BA_NUKE))         {vp[vn] = "放射能球";color[vn++] = TERM_L_GREEN;}
#else
	if (flags4 & (RF4_BA_NUKE))         {vp[vn] = "produce balls of radiation";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_MANA))         {vp[vn] = "魔力の嵐";color[vn++] = TERM_L_BLUE;}
#else
	if (flags5 & (RF5_BA_MANA))         {vp[vn] = "invoke mana storms";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_DARK))         {vp[vn] = "暗黒の嵐";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BA_DARK))         {vp[vn] = "invoke darkness storms";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BA_LITE))         {vp[vn] = "スターバースト";color[vn++] = TERM_YELLOW;}
#else
	if (flags5 & (RF5_BA_LITE))         {vp[vn] = "invoke starburst";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags4 & (RF4_BA_CHAO))         {vp[vn] = "純ログルス";color[vn++] = TERM_VIOLET;}
#else
	if (flags4 & (RF4_BA_CHAO))         {vp[vn] = "invoke raw Logrus";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_HAND_DOOM))       {vp[vn] = "破滅の手";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_HAND_DOOM))       {vp[vn] = "invoke the Hand of Doom";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_PSY_SPEAR))            {vp[vn] = "光の剣";color[vn++] = TERM_YELLOW;}
#else
	if (flags6 & (RF6_PSY_SPEAR))            {vp[vn] = "psycho-spear";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags5 & (RF5_DRAIN_MANA))      {vp[vn] = "魔力吸収";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_DRAIN_MANA))      {vp[vn] = "drain mana";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_MIND_BLAST))      {vp[vn] = "精神攻撃";color[vn++] = TERM_L_RED;}
#else
	if (flags5 & (RF5_MIND_BLAST))      {vp[vn] = "cause mind blasting";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BRAIN_SMASH))     {vp[vn] = "脳攻撃";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BRAIN_SMASH))     {vp[vn] = "cause brain smashing";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_1))         {vp[vn] = "軽傷＋呪い";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_1))         {vp[vn] = "cause light wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_2))         {vp[vn] = "重傷＋呪い";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_2))         {vp[vn] = "cause serious wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_3))         {vp[vn] = "致命傷＋呪い";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_3))         {vp[vn] = "cause critical wounds and cursing";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_CAUSE_4))         {vp[vn] = "秘孔を突く";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_CAUSE_4))         {vp[vn] = "cause mortal wounds";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ACID))         {vp[vn] = "アシッド・ボルト";color[vn++] = TERM_GREEN;}
#else
	if (flags5 & (RF5_BO_ACID))         {vp[vn] = "produce acid bolts";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ELEC))         {vp[vn] = "サンダー・ボルト";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BO_ELEC))         {vp[vn] = "produce lightning bolts";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_FIRE))         {vp[vn] = "ファイア・ボルト";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_BO_FIRE))         {vp[vn] = "produce fire bolts";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_COLD))         {vp[vn] = "アイス・ボルト";color[vn++] = TERM_L_WHITE;}
#else
	if (flags5 & (RF5_BO_COLD))         {vp[vn] = "produce frost bolts";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_NETH))         {vp[vn] = "地獄の矢";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BO_NETH))         {vp[vn] = "produce nether bolts";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_WATE))         {vp[vn] = "ウォーター・ボルト";color[vn++] = TERM_BLUE;}
#else
	if (flags5 & (RF5_BO_WATE))         {vp[vn] = "produce water bolts";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_MANA))         {vp[vn] = "魔力の矢";color[vn++] = TERM_L_BLUE;}
#else
	if (flags5 & (RF5_BO_MANA))         {vp[vn] = "produce mana bolts";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_PLAS))         {vp[vn] = "プラズマ・ボルト";color[vn++] = TERM_L_RED;}
#else
	if (flags5 & (RF5_BO_PLAS))         {vp[vn] = "produce plasma bolts";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags5 & (RF5_BO_ICEE))         {vp[vn] = "極寒の矢";color[vn++] = TERM_WHITE;}
#else
	if (flags5 & (RF5_BO_ICEE))         {vp[vn] = "produce ice bolts";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
if (flags5 & (RF5_MISSILE))         {vp[vn] = "マジックミサイル";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_MISSILE))         {vp[vn] = "produce magic missiles";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_SCARE))           {vp[vn] = "恐怖";color[vn++] = TERM_SLATE;}
#else
	if (flags5 & (RF5_SCARE))           {vp[vn] = "terrify";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags5 & (RF5_BLIND))           {vp[vn] = "目くらまし";color[vn++] = TERM_L_DARK;}
#else
	if (flags5 & (RF5_BLIND))           {vp[vn] = "blind";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags5 & (RF5_CONF))            {vp[vn] = "混乱";color[vn++] = TERM_L_UMBER;}
#else
	if (flags5 & (RF5_CONF))            {vp[vn] = "confuse";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags5 & (RF5_SLOW))            {vp[vn] = "減速";color[vn++] = TERM_UMBER;}
#else
	if (flags5 & (RF5_SLOW))            {vp[vn] = "slow";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags5 & (RF5_HOLD))            {vp[vn] = "麻痺";color[vn++] = TERM_RED;}
#else
	if (flags5 & (RF5_HOLD))            {vp[vn] = "paralyze";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_HASTE))           {vp[vn] = "加速";color[vn++] = TERM_L_GREEN;}
#else
	if (flags6 & (RF6_HASTE))           {vp[vn] = "haste-self";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags6 & (RF6_HEAL))            {vp[vn] = "治癒";color[vn++] = TERM_WHITE;}
#else
	if (flags6 & (RF6_HEAL))            {vp[vn] = "heal-self";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
	if (flags6 & (RF6_INVULNER))        {vp[vn] = "無敵化";color[vn++] = TERM_WHITE;}
#else
	if (flags6 & (RF6_INVULNER))        {vp[vn] = "make invulnerable";color[vn++] = TERM_WHITE;}
#endif

#ifdef JP
if (flags4 & RF4_DISPEL)    {vp[vn] = "魔力消去";color[vn++] = TERM_L_WHITE;}
#else
	if (flags4 & RF4_DISPEL)    {vp[vn] = "dispel-magic";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags6 & (RF6_BLINK))           {vp[vn] = "ショートテレポート";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_BLINK))           {vp[vn] = "blink-self";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TPORT))           {vp[vn] = "テレポート";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_TPORT))           {vp[vn] = "teleport-self";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_WORLD))            {vp[vn] = "時を止める";color[vn++] = TERM_L_BLUE;}
#else
	if (flags6 & (RF6_WORLD))            {vp[vn] = "stop the time";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_TO))         {vp[vn] = "テレポートバック";color[vn++] = TERM_L_UMBER;}
#else
	if (flags6 & (RF6_TELE_TO))         {vp[vn] = "teleport to";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_AWAY))       {vp[vn] = "テレポートアウェイ";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_TELE_AWAY))       {vp[vn] = "teleport away";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_TELE_LEVEL))      {vp[vn] = "テレポート・レベル";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_TELE_LEVEL))      {vp[vn] = "teleport level";color[vn++] = TERM_ORANGE;}
#endif

	if (flags6 & (RF6_DARKNESS))
	{
		if ((pl_ptr->cls_idx != CLASS_NINJA) || (sp_ptr->flags3 & (RF3_HURT_LITE)) || is_undead_species(sp_ptr) || (sp_ptr->flags7 & RF7_DARK_MASK))
		{
#ifdef JP
			vp[vn] =  "暗闇"; color[vn++] = TERM_L_DARK;
#else
			vp[vn] = "create darkness"; color[vn++] = TERM_L_DARK;
#endif
		}
		else
		{
#ifdef JP
			vp[vn] = "閃光"; color[vn++] = TERM_YELLOW;
#else
			vp[vn] = "create light"; color[vn++] = TERM_YELLOW;
#endif
		}
	}

#ifdef JP
if (flags6 & (RF6_TRAPS))           {vp[vn] = "トラップ";color[vn++] = TERM_BLUE;}
#else
	if (flags6 & (RF6_TRAPS))           {vp[vn] = "create traps";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags6 & (RF6_FORGET))          {vp[vn] = "記憶消去";color[vn++] = TERM_BLUE;}
#else
	if (flags6 & (RF6_FORGET))          {vp[vn] = "cause amnesia";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
if (flags6 & (RF6_RAISE_DEAD))      {vp[vn] = "死者復活";color[vn++] = TERM_RED;}
#else
	if (flags6 & (RF6_RAISE_DEAD))      {vp[vn] = "raise dead";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_S_MONSTER))       {vp[vn] = "モンスター一体召喚";color[vn++] = TERM_SLATE;}
#else
	if (flags6 & (RF6_S_MONSTER))       {vp[vn] = "summon a monster";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_MONSTERS))      {vp[vn] = "モンスター複数召喚";color[vn++] = TERM_L_WHITE;}
#else
	if (flags6 & (RF6_S_MONSTERS))      {vp[vn] = "summon monsters";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_KIN))           {vp[vn] = "救援召喚";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_KIN))           {vp[vn] = "summon aid";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_ANT))           {vp[vn] = "アリ召喚";color[vn++] = TERM_RED;}
#else
	if (flags6 & (RF6_S_ANT))           {vp[vn] = "summon ants";color[vn++] = TERM_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_S_SPIDER))        {vp[vn] = "クモ召喚";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_SPIDER))        {vp[vn] = "summon spiders";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HOUND))         {vp[vn] = "ハウンド召喚";color[vn++] = TERM_L_UMBER;}
#else
	if (flags6 & (RF6_S_HOUND))         {vp[vn] = "summon hounds";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HYDRA))         {vp[vn] = "ヒドラ召喚";color[vn++] = TERM_L_GREEN;}
#else
	if (flags6 & (RF6_S_HYDRA))         {vp[vn] = "summon hydras";color[vn++] = TERM_L_GREEN;}
#endif

#ifdef JP
if (flags6 & (RF6_S_ANGEL))         {vp[vn] = "天使一体召喚";color[vn++] = TERM_YELLOW;}
#else
	if (flags6 & (RF6_S_ANGEL))         {vp[vn] = "summon an angel";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
if (flags6 & (RF6_S_DEMON))         {vp[vn] = "デーモン一体召喚";color[vn++] = TERM_L_RED;}
#else
	if (flags6 & (RF6_S_DEMON))         {vp[vn] = "summon a demon";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
if (flags6 & (RF6_S_UNDEAD))        {vp[vn] = "アンデッド一体召喚";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_UNDEAD))        {vp[vn] = "summon an undead";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_DRAGON))        {vp[vn] = "ドラゴン一体召喚";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_DRAGON))        {vp[vn] = "summon a dragon";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HI_UNDEAD))     {vp[vn] = "強力なアンデッド召喚";color[vn++] = TERM_L_DARK;}
#else
	if (flags6 & (RF6_S_HI_UNDEAD))     {vp[vn] = "summon Greater Undead";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
if (flags6 & (RF6_S_HI_DRAGON))     {vp[vn] = "古代ドラゴン召喚";color[vn++] = TERM_ORANGE;}
#else
	if (flags6 & (RF6_S_HI_DRAGON))     {vp[vn] = "summon Ancient Dragons";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
if (flags6 & (RF6_S_CYBER))         {vp[vn] = "サイバーデーモン召喚";color[vn++] = TERM_UMBER;}
#else
	if (flags6 & (RF6_S_CYBER))         {vp[vn] = "summon Cyberdemons";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
if (flags6 & (RF6_S_AMBERITES))     {vp[vn] = "アンバーの王族召喚";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_S_AMBERITES))     {vp[vn] = "summon Lords of Amber";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
if (flags6 & (RF6_S_UNIQUE))        {vp[vn] = "ユニーク・モンスター召喚";color[vn++] = TERM_VIOLET;}
#else
	if (flags6 & (RF6_S_UNIQUE))        {vp[vn] = "summon Unique Monsters";color[vn++] = TERM_VIOLET;}
#endif


	/* Describe spells */
	if (vn)
	{
		/* Note magic */
		magic = TRUE;

		/* Intro */
		if (breath)
		{
#ifdef JP
			hooked_roff("、なおかつ");
#else
			hooked_roff(", and is also");
#endif

		}
		else
		{
#ifdef JP
			hooked_roff(format("%^sは", wd_he[msex]));
#else
			hooked_roff(format("%^s is", wd_he[msex]));
#endif

		}

#ifdef JP
		/* Adverb */
		if (flags2 & (RF2_SMART)) hook_c_roff(TERM_YELLOW, "的確に");

		/* Verb Phrase */
		hooked_roff("魔法を使うことができ、");
#else
		/* Verb Phrase */
		hooked_roff(" magical, casting spells");

		/* Adverb */
		if (flags2 & RF2_SMART) hook_c_roff(TERM_YELLOW, " intelligently");
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("、");
#else
			if (n == 0) hooked_roff(" which ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}
#ifdef JP
		hooked_roff("の呪文を唱えることがある");
#endif
	}


	/* End the sentence about inate/other spells */
	if (breath || magic)
	{
		/* Total casting */
		m = sp_ptr->r_cast_spell;

		/* Average frequency */
		n = sp_ptr->freq_spell;

		/* Describe the spell frequency */
		if (m > 100 || know_everything)
		{
#ifdef JP
			hooked_roff(format("(確率:1/%d)", 100 / n));
#else
			hooked_roff(format("; 1 time in %d", 100 / n));
#endif

		}

		/* Guess at the frequency */
		else if (m)
		{
			n = ((n + 9) / 10) * 10;
#ifdef JP
			hooked_roff(format("(確率:約1/%d)", 100 / n));
#else
			hooked_roff(format("; about 1 time in %d", 100 / n));
#endif

		}

		/* End this sentence */
#ifdef JP
		hooked_roff("。");
#else
		hooked_roff(".  ");
#endif

	}

	/* Describe monster "toughness" */
	if (know_armour(sp_ptr->species_idx))
	{
		/* Armor */
#ifdef JP
		hooked_roff(format("%^sは AC%d の基本防御力、",
#else
		hooked_roff(format("%^s has an base armor rating of %d",
#endif

			    wd_he[msex], sp_ptr->ac));

		/* Maximized hitpoints */
		//TODO::Reconstruct HP Expression. 
	}

		/* Stat */
#ifdef JP
			hooked_roff(format("%dの腕力、",
#else
			hooked_roff(format(" STR:%d  ",
#endif
				    sp_ptr->stat_max[0]/10));

#ifdef JP
			hooked_roff(format("%dの知力、",
#else
			hooked_roff(format(" STR:%d  ",
#endif
				    sp_ptr->stat_max[1]/10));

#ifdef JP
			hooked_roff(format("%dの賢さ、",
#else
			hooked_roff(format(" STR:%d  ",
#endif
				    sp_ptr->stat_max[2]/10));

#ifdef JP
			hooked_roff(format("%dの器用さ、",
#else
			hooked_roff(format(" STR:%d  ",
#endif
				    sp_ptr->stat_max[3]/10));

#ifdef JP
			hooked_roff(format("%dの耐久力、",
#else
			hooked_roff(format(" STR:%d  ",
#endif
				    sp_ptr->stat_max[4]/10));

#ifdef JP
			hooked_roff(format("%dの魅力",
#else
			hooked_roff(format(" CHA:%d  ",
#endif
				    sp_ptr->stat_max[5]/10));

#ifdef JP
			hooked_roff(format("を持つ。"));
#else
			hooked_roff(format(","));
#endif




	/* Collect special abilities. */
	vn = 0;
#ifdef JP
	if (flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) { vp[vn] = "ダンジョンを照らす";     color[vn++] = TERM_WHITE; }
#else
	if (flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) { vp[vn] = "illuminate the dungeon"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags7 & (RF7_HAS_DARK_1 | RF7_HAS_DARK_2)) { vp[vn] = "ダンジョンを暗くする";   color[vn++] = TERM_L_DARK; }
#else
	if (flags7 & (RF7_HAS_DARK_1 | RF7_HAS_DARK_2)) { vp[vn] = "darken the dungeon";     color[vn++] = TERM_L_DARK; }
#endif

#ifdef JP
	if (flags2 & RF2_OPEN_DOOR) { vp[vn] = "ドアを開ける"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_OPEN_DOOR) { vp[vn] = "open doors"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_BASH_DOOR) { vp[vn] = "ドアを打ち破る"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_BASH_DOOR) { vp[vn] = "bash down doors"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_PASS_WALL) { vp[vn] = "壁をすり抜ける"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_PASS_WALL) { vp[vn] = "pass through walls"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_WALL) { vp[vn] = "壁を掘り進む"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_WALL) { vp[vn] = "bore through walls"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_MOVE_BODY) { vp[vn] = "弱いモンスターを押しのける"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_MOVE_BODY) { vp[vn] = "push past weaker monsters"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_BODY) { vp[vn] = "弱いモンスターを倒す"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_BODY) { vp[vn] = "destroy weaker monsters"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_TAKE_ITEM) { vp[vn] = "アイテムを拾う"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_TAKE_ITEM) { vp[vn] = "pick up objects"; color[vn++] = TERM_WHITE; }
#endif

#ifdef JP
	if (flags2 & RF2_KILL_ITEM) { vp[vn] = "アイテムを壊す"; color[vn++] = TERM_WHITE; }
#else
	if (flags2 & RF2_KILL_ITEM) { vp[vn] = "destroy objects"; color[vn++] = TERM_WHITE; }
#endif


	/* Describe special abilities. */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if (n != vn - 1)
			{
				jverb(vp[n], jverb_buf, JVERB_AND);
				hook_c_roff(color[n], jverb_buf);
				hooked_roff("、");
			}
			else hook_c_roff(color[n], vp[n]);
#else
			if (n == 0) hooked_roff(" can ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" and ");

			/* Dump */
			hook_c_roff(color[n], vp[n]);
#endif

		}

		/* End */
#ifdef JP
		hooked_roff("ことができる。");
#else
		hooked_roff(".  ");
#endif

	}


	/* Describe special abilities. */
	if (flags7 & (RF7_SELF_LITE_1 | RF7_SELF_LITE_2))
	{
#ifdef JP
		hooked_roff(format("%^sは光っている。", wd_he[msex]));
#else
		hooked_roff(format("%^s is shining.  ", wd_he[msex]));
#endif

	}
	if (flags7 & (RF7_SELF_DARK_1 | RF7_SELF_DARK_2))
	{
#ifdef JP
		hook_c_roff(TERM_L_DARK, format("%^sは暗黒に包まれている。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_DARK, format("%^s is surrounded by darkness.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_INVISIBLE)
	{
#ifdef JP
		hooked_roff(format("%^sは透明で目に見えない。", wd_he[msex]));
#else
		hooked_roff(format("%^s is invisible.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_COLD_BLOOD)
	{
#ifdef JP
		hooked_roff(format("%^sは冷血動物である。", wd_he[msex]));
#else
		hooked_roff(format("%^s is cold blooded.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_EMPTY_MIND)
	{
#ifdef JP
		hooked_roff(format("%^sはテレパシーでは感知できない。", wd_he[msex]));
#else
		hooked_roff(format("%^s is not detected by telepathy.  ", wd_he[msex]));
#endif

	}
	else if (flags2 & RF2_WEIRD_MIND)
	{
#ifdef JP
		hooked_roff(format("%^sはまれにテレパシーで感知できる。", wd_he[msex]));
#else
		hooked_roff(format("%^s is rarely detected by telepathy.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_MULTIPLY)
	{
#ifdef JP
		hook_c_roff(TERM_L_UMBER, format("%^sは爆発的に増殖する。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_UMBER, format("%^s breeds explosively.  ", wd_he[msex]));
#endif

	}
	if (flags2 & RF2_REGENERATE)
	{
#ifdef JP
		hook_c_roff(TERM_L_WHITE, format("%^sは素早く体力を回復する。", wd_he[msex]));
#else
		hook_c_roff(TERM_L_WHITE, format("%^s regenerates quickly.  ", wd_he[msex]));
#endif

	}
	if (flags7 & RF7_RIDING)
	{
#ifdef JP
		hook_c_roff(TERM_SLATE, format("%^sに乗ることができる。", wd_he[msex]));
#else
		hook_c_roff(TERM_SLATE, format("%^s is suitable for riding.  ", wd_he[msex]));
#endif

	}


	/* Collect susceptibilities */
	vn = 0;
#ifdef JP
	if (flags3 & RF3_HURT_ROCK) {vp[vn] = "岩を除去するもの";color[vn++] = TERM_UMBER;}
#else
	if (flags3 & RF3_HURT_ROCK) {vp[vn] = "rock remover";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags3 & RF3_HURT_LITE) {vp[vn] = "明るい光";color[vn++] = TERM_YELLOW;}
#else
	if (flags3 & RF3_HURT_LITE) {vp[vn] = "bright light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if (flags3 & RF3_HURT_FIRE) {vp[vn] = "炎";color[vn++] = TERM_RED;}
#else
	if (flags3 & RF3_HURT_FIRE) {vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

#ifdef JP
	if (flags3 & RF3_HURT_COLD) {vp[vn] = "冷気";color[vn++] = TERM_L_WHITE;}
#else
	if (flags3 & RF3_HURT_COLD) {vp[vn] = "cold";color[vn++] = TERM_L_WHITE;}
#endif


	/* Describe susceptibilities */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sには", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("や");
#else
			if (n == 0) hooked_roff(" is hurt by ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" and ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("でダメージを与えられる。");
#else
		hooked_roff(".  ");
#endif

	}


	/* Collect immunities */
	vn = 0;
#ifdef JP
	if (flags10 & RF10_IM_ACID) {vp[vn] = "酸";color[vn++] = TERM_GREEN;}
#else
	if (flags10 & RF10_IM_ACID) {vp[vn] = "acid";color[vn++] = TERM_GREEN;}
#endif

#ifdef JP
	if (flags10 & RF10_IM_ELEC) {vp[vn] = "稲妻";color[vn++] = TERM_BLUE;}
#else
	if (flags10 & RF10_IM_ELEC) {vp[vn] = "lightning";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if (flags10 & RF10_IM_FIRE) {vp[vn] = "炎";color[vn++] = TERM_RED;}
#else
	if (flags10 & RF10_IM_FIRE) {vp[vn] = "fire";color[vn++] = TERM_RED;}
#endif

#ifdef JP
	if (flags10 & RF10_IM_COLD) {vp[vn] = "冷気";color[vn++] = TERM_L_WHITE;}
#else
	if (flags10 & RF10_IM_COLD) {vp[vn] = "cold";color[vn++] = TERM_L_WHITE;}
#endif

#ifdef JP
	if (flags10 & RF10_IM_POIS) {vp[vn] = "毒";color[vn++] = TERM_L_GREEN;}
#else
	if (flags10 & RF10_IM_POIS) {vp[vn] = "poison";color[vn++] = TERM_L_GREEN;}
#endif


	/* Collect resistances */
#ifdef JP
	if (flags10 & RF10_RES_LITE) {vp[vn] = "閃光";color[vn++] = TERM_YELLOW;}
#else
	if (flags10 & RF10_RES_LITE) {vp[vn] = "light";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_DARK) {vp[vn] = "暗黒";color[vn++] = TERM_L_DARK;}
#else
	if (flags10 & RF10_RES_DARK) {vp[vn] = "dark";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_NETH) {vp[vn] = "地獄";color[vn++] = TERM_L_DARK;}
#else
	if (flags10 & RF10_RES_NETH) {vp[vn] = "nether";color[vn++] = TERM_L_DARK;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_WATE) {vp[vn] = "水";color[vn++] = TERM_BLUE;}
#else
	if (flags10 & RF10_RES_WATE) {vp[vn] = "water";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_PLAS) {vp[vn] = "プラズマ";color[vn++] = TERM_L_RED;}
#else
	if (flags10 & RF10_RES_PLAS) {vp[vn] = "plasma";color[vn++] = TERM_L_RED;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_SHAR) {vp[vn] = "破片";color[vn++] = TERM_L_UMBER;}
#else
	if (flags10 & RF10_RES_SHAR) {vp[vn] = "shards";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_SOUN) {vp[vn] = "轟音";color[vn++] = TERM_ORANGE;}
#else
	if (flags10 & RF10_RES_SOUN) {vp[vn] = "sound";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_CHAO) {vp[vn] = "カオス";color[vn++] = TERM_VIOLET;}
#else
	if (flags10 & RF10_RES_CHAO) {vp[vn] = "chaos";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_NEXU) {vp[vn] = "因果混乱";color[vn++] = TERM_VIOLET;}
#else
	if (flags10 & RF10_RES_NEXU) {vp[vn] = "nexus";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_DISE) {vp[vn] = "劣化";color[vn++] = TERM_VIOLET;}
#else
	if (flags10 & RF10_RES_DISE) {vp[vn] = "disenchantment";color[vn++] = TERM_VIOLET;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_WALL) {vp[vn] = "フォース";color[vn++] = TERM_UMBER;}
#else
	if (flags10 & RF10_RES_WALL) {vp[vn] = "force";color[vn++] = TERM_UMBER;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_INER) {vp[vn] = "遅鈍";color[vn++] = TERM_SLATE;}
#else
	if (flags10 & RF10_RES_INER) {vp[vn] = "inertia";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_TIME) {vp[vn] = "時間逆転";color[vn++] = TERM_L_BLUE;}
#else
	if (flags10 & RF10_RES_TIME) {vp[vn] = "time";color[vn++] = TERM_L_BLUE;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_GRAV) {vp[vn] = "重力";color[vn++] = TERM_SLATE;}
#else
	if (flags10 & RF10_RES_GRAV) {vp[vn] = "gravity";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags10 & RF10_RES_ALL) {vp[vn] = "あらゆる攻撃";color[vn++] = TERM_YELLOW;}
#else
	if (flags10 & RF10_RES_ALL) {vp[vn] = "all";color[vn++] = TERM_YELLOW;}
#endif

#ifdef JP
	if ((flags10 & RF10_RES_TELE) && !(is_unique_species(sp_ptr))) {vp[vn] = "テレポート";color[vn++] = TERM_ORANGE;}
#else
	if ((flags10 & RF10_RES_TELE) && !(is_unique_species(sp_ptr))) {vp[vn] = "teleportation";color[vn++] = TERM_ORANGE;}
#endif


	/* Describe immunities and resistances */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("と");
#else
			if (n == 0) hooked_roff(" resists ");
			else if (n < vn-1) hooked_roff(", ");
			else hooked_roff(" and ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("の耐性を持っている。");
#else
		hooked_roff(".  ");
#endif

	}


	if ((sp_ptr->r_xtra1 & MR1_SINKA) || know_everything)
	{
		if (sp_ptr->next_species_idx)
		{
#ifdef JP
			hooked_roff(format("%^sは経験を積むと、", wd_he[msex]));
#else
			hooked_roff(format("%^s will evolve into ", wd_he[msex]));
#endif
			hook_c_roff(TERM_YELLOW, format("%s", r_name+species_info[sp_ptr->next_species_idx].name));
#ifdef JP
			hooked_roff(format("に進化する。"));
#else
			hooked_roff(format(" when %s gets enugh experience.  ", wd_he[msex]));
#endif
		}
		else if (!(is_unique_species(sp_ptr)))
		{
#ifdef JP
			hooked_roff(format("%sは進化しない。", wd_he[msex]));
#else
			hooked_roff(format("%s won't evolve.  ", wd_he[msex]));
#endif
		}
	}

	/* Collect non-effects */
	vn = 0;
#ifdef JP
	if (flags3 & RF3_NO_STUN)  {vp[vn] = "朦朧としない";color[vn++] = TERM_ORANGE;}
#else
	if (flags3 & RF3_NO_STUN)  {vp[vn] = "stunned";color[vn++] = TERM_ORANGE;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_FEAR)  {vp[vn] = "恐怖を感じない";color[vn++] = TERM_SLATE;}
#else
	if (flags3 & RF3_NO_FEAR)  {vp[vn] = "frightened";color[vn++] = TERM_SLATE;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_CONF)  {vp[vn] = "混乱しない";color[vn++] = TERM_L_UMBER;}
#else
	if (flags3 & RF3_NO_CONF)  {vp[vn] = "confused";color[vn++] = TERM_L_UMBER;}
#endif

#ifdef JP
	if (flags3 & RF3_NO_SLEEP) {vp[vn] = "眠らされない";color[vn++] = TERM_BLUE;}
#else
	if (flags3 & RF3_NO_SLEEP) {vp[vn] = "slept";color[vn++] = TERM_BLUE;}
#endif

#ifdef JP
	if ((flags10 & RF10_RES_TELE) && (is_unique_species(sp_ptr))) {vp[vn] = "テレポートされない";color[vn++] = TERM_ORANGE;}
#else
	if ((flags10 & RF10_RES_TELE) && (is_unique_species(sp_ptr))) {vp[vn] = "teleported";color[vn++] = TERM_ORANGE;}
#endif

	/* Describe non-effects */
	if (vn)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s", wd_he[msex]));
#endif


		/* Scan */
		for (n = 0; n < vn; n++)
		{
			/* Intro */
#ifdef JP
			if ( n != 0 ) hooked_roff("し、");
#else
			if (n == 0) hooked_roff(" cannot be ");
			else if (n < vn - 1) hooked_roff(", ");
			else hooked_roff(" or ");
#endif


			/* Dump */
			hook_c_roff(color[n], vp[n]);
		}

		/* End */
#ifdef JP
		hooked_roff("。");
#else
		hooked_roff(".  ");
#endif

	}


	/* Do we know how aware it is? */
	if ((((int)sp_ptr->r_wake * (int)sp_ptr->r_wake) > sp_ptr->sleep) ||
		  (sp_ptr->r_ignore == MAX_UCHAR) ||
	    (sp_ptr->sleep == 0 && sp_ptr->r_tkills >= 10) || know_everything)
	{
		cptr act;

		if (sp_ptr->sleep > 200)
		{
#ifdef JP
			act = "を無視しがちであるが";
#else
			act = "prefers to ignore";
#endif

		}
		else if (sp_ptr->sleep > 95)
		{
#ifdef JP
			act = "に対してほとんど注意を払わないが";
#else
			act = "pays very little attention to";
#endif

		}
		else if (sp_ptr->sleep > 75)
		{
#ifdef JP
			act = "に対してあまり注意を払わないが";
#else
			act = "pays little attention to";
#endif

		}
		else if (sp_ptr->sleep > 45)
		{
#ifdef JP
			act = "を見過ごしがちであるが";
#else
			act = "tends to overlook";
#endif

		}
		else if (sp_ptr->sleep > 25)
		{
#ifdef JP
			act = "をほんの少しは見ており";
#else
			act = "takes quite a while to see";
#endif

		}
		else if (sp_ptr->sleep > 10)
		{
#ifdef JP
			act = "をしばらくは見ており";
#else
			act = "takes a while to see";
#endif

		}
		else if (sp_ptr->sleep > 5)
		{
#ifdef JP
			act = "を幾分注意深く見ており";
#else
			act = "is fairly observant of";
#endif

		}
		else if (sp_ptr->sleep > 3)
		{
#ifdef JP
			act = "を注意深く見ており";
#else
			act = "is observant of";
#endif

		}
		else if (sp_ptr->sleep > 1)
		{
#ifdef JP
			act = "をかなり注意深く見ており";
#else
			act = "is very observant of";
#endif

		}
		else if (sp_ptr->sleep > 0)
		{
#ifdef JP
			act = "を警戒しており";
#else
			act = "is vigilant for";
#endif

		}
		else
		{
#ifdef JP
			act = "をかなり警戒しており";
#else
			act = "is ever vigilant for";
#endif

		}

#ifdef JP
		hooked_roff(format("%^sは侵入者%s、 %d フィート先から侵入者に気付くことがある。",
		     wd_he[msex], act, 10 * sp_ptr->aaf));
#else
		hooked_roff(format("%^s %s intruders, which %s may notice from %d feet.  ",
			    wd_he[msex], act, wd_he[msex], 10 * sp_ptr->aaf));
#endif

	}


	/* Drops gold and/or items */
	if (drop_gold || drop_item)
	{
		/* Intro */
#ifdef JP
		hooked_roff(format("%^sは", wd_he[msex]));
#else
		hooked_roff(format("%^s may carry", wd_he[msex]));

		/* No "n" needed */
		sin = FALSE;
#endif


		/* Count maximum drop */
		n = MAX(drop_gold, drop_item);

		/* One drop (may need an "n") */
		if (n == 1)
		{
#ifdef JP
			hooked_roff("一つの");
#else
			hooked_roff(" a");
			sin = TRUE;
#endif
		}

		/* Two drops */
		else if (n == 2)
		{
#ifdef JP
			hooked_roff("一つか二つの");
#else
			hooked_roff(" one or two");
#endif

		}

		/* Many drops */
		else
		{
#ifdef JP
			hooked_roff(format(" %d 個までの", n));
#else
			hooked_roff(format(" up to %d", n));
#endif

		}


		/* Great */
/*
		if (flags1 & RF1_DROP_GREAT)
		{
#ifdef JP
			p = "特別な";
#else
			p = " exceptional";
#endif

		}
*/

		/* Good (no "n" needed) */
/*
		else if (flags1 & RF1_DROP_GOOD)
		{
#ifdef JP
			p = "上質な";
#else
			p = " good";
			sin = FALSE;
#endif
		}
*/

		/* Okay */
/*
		else
		{
			p = NULL;
		}
*/
		p = NULL;

		/* Objects */
		if (drop_item)
		{
			/* Handle singular "an" */
#ifndef JP
			if (sin) hooked_roff("n");
			sin = FALSE;
#endif

			/* Dump "object(s)" */
			if (p) hooked_roff(p);
#ifdef JP
			hooked_roff("アイテム");
#else
			hooked_roff(" object");
			if (n != 1) hooked_roff("s");
#endif


			/* Conjunction replaces variety, if needed for "gold" below */
#ifdef JP
			p = "や";
#else
			p = " or";
#endif

		}

		/* Treasures */
		if (drop_gold)
		{
#ifndef JP
			/* Cancel prefix */
			if (!p) sin = FALSE;

			/* Handle singular "an" */
			if (sin) hooked_roff("n");
			sin = FALSE;
#endif

			/* Dump "treasure(s)" */
			if (p) hooked_roff(p);
#ifdef JP
			hooked_roff("財宝");
#else
			hooked_roff(" treasure");
			if (n != 1) hooked_roff("s");
#endif

		}

		/* End this sentence */
#ifdef JP
		hooked_roff("を持っていることがある。");
#else
		hooked_roff(".  ");
#endif

	}


	/* Count the number of "known" attacks */
	for (n = 0, m = 0; m < 4; m++)
	{
		/* Skip non-attacks */
		if (!sp_ptr->blow[m].method) continue;
		if (sp_ptr->blow[m].method == RBM_SHOOT) continue;

		/* Count known attacks */
		if (sp_ptr->r_blows[m] || know_everything) n++;
	}

	/* Examine (and count) the actual attacks */
	for (r = 0, m = 0; m < 4; m++)
	{
		int method, effect, d1, d2;

		/* Skip non-attacks */
		if (!sp_ptr->blow[m].method) continue;
		if (sp_ptr->blow[m].method == RBM_SHOOT) continue;

		/* Skip unknown attacks */
		if (!sp_ptr->r_blows[m] && !know_everything) continue;

		/* Extract the attack info */
		method = sp_ptr->blow[m].method;
		effect = sp_ptr->blow[m].effect;
		d1 = sp_ptr->blow[m].d_dice;
		d2 = sp_ptr->blow[m].d_side;

		/* No method yet */
		p = NULL;

		/* Acquire the method */
		switch (method)
		{
#ifdef JP
case RBM_HIT:		p = "殴る"; break;
#else
			case RBM_HIT:		p = "hit"; break;
#endif

#ifdef JP
case RBM_TOUCH:		p = "触る"; break;
#else
			case RBM_TOUCH:		p = "touch"; break;
#endif

#ifdef JP
case RBM_PUNCH:		p = "パンチする"; break;
#else
			case RBM_PUNCH:		p = "punch"; break;
#endif

#ifdef JP
case RBM_KICK:		p = "蹴る"; break;
#else
			case RBM_KICK:		p = "kick"; break;
#endif

#ifdef JP
case RBM_CLAW:		p = "ひっかく"; break;
#else
			case RBM_CLAW:		p = "claw"; break;
#endif

#ifdef JP
case RBM_BITE:		p = "噛む"; break;
#else
			case RBM_BITE:		p = "bite"; break;
#endif

#ifdef JP
case RBM_STING:		p = "刺す"; break;
#else
			case RBM_STING:		p = "sting"; break;
#endif

#ifdef JP
case RBM_SLASH:		p = "斬る"; break;
#else
			case RBM_SLASH:		p = "slash"; break;
#endif

#ifdef JP
case RBM_BUTT:		p = "角で突く"; break;
#else
			case RBM_BUTT:		p = "butt"; break;
#endif

#ifdef JP
case RBM_CRUSH:		p = "体当たりする"; break;
#else
			case RBM_CRUSH:		p = "crush"; break;
#endif

#ifdef JP
case RBM_ENGULF:	p = "飲み込む"; break;
#else
			case RBM_ENGULF:	p = "engulf"; break;
#endif

#ifdef JP
case RBM_CHARGE: 	p = "請求書をよこす"; break;
#else
			case RBM_CHARGE: 	p = "charge";   break;
#endif

#ifdef JP
case RBM_CRAWL:		p = "体の上を這い回る"; break;
#else
			case RBM_CRAWL:		p = "crawl on you"; break;
#endif

#ifdef JP
case RBM_DROOL:		p = "よだれをたらす"; break;
#else
			case RBM_DROOL:		p = "drool on you"; break;
#endif

#ifdef JP
case RBM_SPIT:		p = "つばを吐く"; break;
#else
			case RBM_SPIT:		p = "spit"; break;
#endif

#ifdef JP
case RBM_EXPLODE:	p = "爆発する"; break;
#else
			case RBM_EXPLODE:	p = "explode"; break;
#endif

#ifdef JP
case RBM_GAZE:		p = "にらむ"; break;
#else
			case RBM_GAZE:		p = "gaze"; break;
#endif

#ifdef JP
case RBM_WAIL:		p = "泣き叫ぶ"; break;
#else
			case RBM_WAIL:		p = "wail"; break;
#endif

#ifdef JP
case RBM_SPORE:		p = "胞子を飛ばす"; break;
#else
			case RBM_SPORE:		p = "release spores"; break;
#endif

			case RBM_XXX4:		break;
#ifdef JP
case RBM_BEG:		p = "金をせがむ"; break;
#else
			case RBM_BEG:		p = "beg"; break;
#endif

#ifdef JP
case RBM_INSULT:	p = "侮辱する"; break;
#else
			case RBM_INSULT:	p = "insult"; break;
#endif

#ifdef JP
case RBM_MOAN:		p = "うめく"; break;
#else
			case RBM_MOAN:		p = "moan"; break;
#endif

#ifdef JP
case RBM_SHOW:  	p = "歌う"; break;
#else
			case RBM_SHOW:  	p = "sing"; break;
#endif

		}


		/* Default effect */
		q = NULL;

		/* Acquire the effect */
		switch (effect)
		{
#ifdef JP
case RBE_SUPERHURT:
case RBE_HURT:    	q = "攻撃する"; break;
#else
			case RBE_SUPERHURT:
			case RBE_HURT:    	q = "attack"; break;
#endif

#ifdef JP
case RBE_POISON:  	q = "毒をくらわす"; break;
#else
			case RBE_POISON:  	q = "poison"; break;
#endif

#ifdef JP
case RBE_UN_BONUS:	q = "劣化させる"; break;
#else
			case RBE_UN_BONUS:	q = "disenchant"; break;
#endif

#ifdef JP
case RBE_UN_POWER:	q = "魔力を吸い取る"; break;
#else
			case RBE_UN_POWER:	q = "drain charges"; break;
#endif

#ifdef JP
case RBE_EAT_GOLD:	q = "金を盗む"; break;
#else
			case RBE_EAT_GOLD:	q = "steal gold"; break;
#endif

#ifdef JP
case RBE_EAT_ITEM:	q = "アイテムを盗む"; break;
#else
			case RBE_EAT_ITEM:	q = "steal items"; break;
#endif

#ifdef JP
case RBE_EAT_FOOD:	q = "あなたの食料を食べる"; break;
#else
			case RBE_EAT_FOOD:	q = "eat your food"; break;
#endif

#ifdef JP
case RBE_EAT_LITE:	q = "明かりを吸収する"; break;
#else
			case RBE_EAT_LITE:	q = "absorb light"; break;
#endif

#ifdef JP
case RBE_ACID:    	q = "酸を飛ばす"; break;
#else
			case RBE_ACID:    	q = "shoot acid"; break;
#endif

#ifdef JP
case RBE_ELEC:    	q = "感電させる"; break;
#else
			case RBE_ELEC:    	q = "electrocute"; break;
#endif

#ifdef JP
case RBE_FIRE:    	q = "燃やす"; break;
#else
			case RBE_FIRE:    	q = "burn"; break;
#endif

#ifdef JP
case RBE_COLD:    	q = "凍らせる"; break;
#else
			case RBE_COLD:    	q = "freeze"; break;
#endif

#ifdef JP
case RBE_BLIND:   	q = "盲目にする"; break;
#else
			case RBE_BLIND:   	q = "blind"; break;
#endif

#ifdef JP
case RBE_CONFUSE: 	q = "混乱させる"; break;
#else
			case RBE_CONFUSE: 	q = "confuse"; break;
#endif

#ifdef JP
case RBE_TERRIFY: 	q = "恐怖させる"; break;
#else
			case RBE_TERRIFY: 	q = "terrify"; break;
#endif

#ifdef JP
case RBE_PARALYZE:	q = "麻痺させる"; break;
#else
			case RBE_PARALYZE:	q = "paralyze"; break;
#endif

#ifdef JP
case RBE_LOSE_STR:	q = "腕力を減少させる"; break;
#else
			case RBE_LOSE_STR:	q = "reduce strength"; break;
#endif

#ifdef JP
case RBE_LOSE_INT:	q = "知能を減少させる"; break;
#else
			case RBE_LOSE_INT:	q = "reduce intelligence"; break;
#endif

#ifdef JP
case RBE_LOSE_WIS:	q = "賢さを減少させる"; break;
#else
			case RBE_LOSE_WIS:	q = "reduce wisdom"; break;
#endif

#ifdef JP
case RBE_LOSE_DEX:	q = "器用さを減少させる"; break;
#else
			case RBE_LOSE_DEX:	q = "reduce dexterity"; break;
#endif

#ifdef JP
case RBE_LOSE_CON:	q = "耐久力を減少させる"; break;
#else
			case RBE_LOSE_CON:	q = "reduce constitution"; break;
#endif

#ifdef JP
case RBE_LOSE_CHR:	q = "魅力を減少させる"; break;
#else
			case RBE_LOSE_CHR:	q = "reduce charisma"; break;
#endif

#ifdef JP
case RBE_LOSE_ALL:	q = "全ステータスを減少させる"; break;
#else
			case RBE_LOSE_ALL:	q = "reduce all stats"; break;
#endif

#ifdef JP
case RBE_SHATTER:	q = "粉砕する"; break;
#else
			case RBE_SHATTER:	q = "shatter"; break;
#endif

#ifdef JP
case RBE_EXP_10:	q = "経験値を減少(10d6+)させる"; break;
#else
			case RBE_EXP_10:	q = "lower experience (by 10d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_20:	q = "経験値を減少(20d6+)させる"; break;
#else
			case RBE_EXP_20:	q = "lower experience (by 20d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_40:	q = "経験値を減少(40d6+)させる"; break;
#else
			case RBE_EXP_40:	q = "lower experience (by 40d6+)"; break;
#endif

#ifdef JP
case RBE_EXP_80:	q = "経験値を減少(80d6+)させる"; break;
#else
			case RBE_EXP_80:	q = "lower experience (by 80d6+)"; break;
#endif

#ifdef JP
case RBE_DISEASE:	q = "病気にする"; break;
#else
			case RBE_DISEASE:	q = "disease"; break;
#endif

#ifdef JP
case RBE_TIME:      q = "時間を逆戻りさせる"; break;
#else
			case RBE_TIME:      q = "time"; break;
#endif

#ifdef JP
case RBE_EXP_VAMP:  q = "生命力を吸収する"; break;
#else
			case RBE_EXP_VAMP:  q = "drain life force"; break;
#endif

#ifdef JP
case RBE_DR_MANA:  q = "魔力を奪う"; break;
#else
			case RBE_DR_MANA:  q = "drain mana force"; break;
#endif

		}


#ifdef JP
		if ( r == 0 ) hooked_roff( format("%^sは", wd_he[msex]) );

		/***若干表現を変更 ita ***/

			/* Describe damage (if known) */
		if (d1 && d2 && (know_everything || know_damage(sp_ptr->species_idx, m)))
		  {
		    
		    /* Display the damage */
		    hooked_roff(format(" %dd%d ", d1, d2));
		    hooked_roff("のダメージで");
		  }
		/* Hack -- force a method */
		if (!p) p = "何か奇妙なことをする";

		/* Describe the method */
		/* XXしてYYし/XXしてYYする/XXし/XXする */
		if(q) jverb( p ,jverb_buf, JVERB_TO);
		else if(r!=n-1) jverb( p ,jverb_buf, JVERB_AND);
		else strcpy(jverb_buf, p);

		hooked_roff(jverb_buf);

		/* Describe the effect (if any) */
		if (q)
		{
		  if(r!=n-1) jverb( q,jverb_buf, JVERB_AND);
		  else strcpy(jverb_buf,q); 
		  hooked_roff(jverb_buf);
		}
		if(r!=n-1) hooked_roff("、");
#else
		/* Introduce the attack description */
		if (!r)
		{
			hooked_roff(format("%^s can ", wd_he[msex]));
		}
		else if (r < n-1)
		{
			hooked_roff(", ");
		}
		else
		{
			hooked_roff(", and ");
		}


		/* Hack -- force a method */
		if (!p) p = "do something weird";

		/* Describe the method */
		hooked_roff(p);


		/* Describe the effect (if any) */
		if (q)
		{
			/* Describe the attack type */
			hooked_roff(" to ");
			hooked_roff(q);

			/* Describe damage (if known) */
			if (d1 && d2 && (know_everything || know_damage(species_idx, m)))
			{
				/* Display the damage */
				hooked_roff(" with damage");
				hooked_roff(format(" %dd%d", d1, d2));
			}
		}
#endif



		/* Count the attacks as printed */
		r++;
	}

	/* Finish sentence above */
	if (r)
	{
#ifdef JP
		hooked_roff("。");
#else
		hooked_roff(".  ");
#endif

	}

	/* Notice lack of attacks */
	else if (flags1 & RF1_NEVER_BLOW)
	{
#ifdef JP
		hooked_roff(format("%^sは物理的な攻撃方法を持たない。", wd_he[msex]));
#else
		hooked_roff(format("%^s has no physical attacks.  ", wd_he[msex]));
#endif

	}

	/* Or describe the lack of knowledge */
	else
	{
#ifdef JP
		hooked_roff(format("%s攻撃については何も知らない。", wd_his[msex]));
#else
		hooked_roff(format("Nothing is known about %s attack.  ", wd_his[msex]));
#endif

	}

	/*
	 * Notice "Quest" monsters, but only if you
	 * already encountered the monster.
	 */
	if ((flags1 & RF1_QUESTOR) && ((sp_ptr->r_sights) && (sp_ptr->max_num) && ((sp_ptr->species_idx == MON_OBERON) || (sp_ptr->species_idx == MON_SERPENT))))
	{
#ifdef JP
		hook_c_roff(TERM_VIOLET, "あなたはこのモンスターを殺したいという強い欲望を感じている...");
#else
		hook_c_roff(TERM_VIOLET, "You feel an intense desire to kill this monster...  ");
#endif

	}

	else if (flags7 & RF7_GUARDIAN)
	{
#ifdef JP
		hook_c_roff(TERM_L_RED, "このモンスターはダンジョンの主である。");
#else
		hook_c_roff(TERM_L_RED, "This monster is the master of a dungeon.");
#endif

	}


	/* All done */
	hooked_roff("\n");

}



/*
 * Hack -- Display the "name" and "attr/chars" of a monster race
 */
void roff_top(int species_idx)
{
	species_type	*r_ptr = &species_info[species_idx];

	byte		a1, a2;
	char		c1, c2;


	/* Access the chars */
	c1 = r_ptr->d_char;
	c2 = r_ptr->x_char;

	/* Access the attrs */
	a1 = r_ptr->d_attr;
	a2 = r_ptr->x_attr;


	/* Clear the top line */
	Term_erase(0, 0, 255);

	/* Reset the cursor */
	Term_gotoxy(0, 0);

#ifndef JP
	/* A title (use "The" for non-uniques) */
	if (!(is_unique_species(r_ptr)))
	{
		Term_addstr(-1, TERM_WHITE, "The ");
	}
#endif

	/* Dump the name */
	Term_addstr(-1, TERM_WHITE, (r_name + r_ptr->name));

	/* Append the "standard" attr/char info */
	Term_addstr(-1, TERM_WHITE, " ('");
	Term_add_bigch(a1, c1);
	Term_addstr(-1, TERM_WHITE, "')");

	/* Append the "optional" attr/char info */
	Term_addstr(-1, TERM_WHITE, "/('");
	Term_add_bigch(a2, c2);
	Term_addstr(-1, TERM_WHITE, "'):");

	/* Wizards get extra info */
	if (wizard)
	{
		char buf[6];

		sprintf(buf, "%d", species_idx);

		Term_addstr(-1, TERM_WHITE, " (");
		Term_addstr(-1, TERM_L_BLUE, buf);
		Term_addch(TERM_WHITE, ')');
	}
}



/*
 * Hack -- describe the given monster race at the top of the screen
 */
void screen_roff(int species_idx, int mode)
{
	/* Flush messages */
	msg_print(NULL);

	/* Begin recall */
	Term_erase(0, 1, 255);

	hook_c_roff = c_roff;

	/* Recall monster */
	roff_aux(p_ptr, &species_info[species_idx], mode);

	/* Describe monster */
	roff_top(species_idx);
}




/*
 * Hack -- describe the given monster race in the current "term" window
 */
void display_roff(int species_idx)
{
	int y;

	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* Begin recall */
	Term_gotoxy(0, 1);

	hook_c_roff = c_roff;

	/* Recall monster */
	roff_aux(p_ptr, &species_info[species_idx], 0);

	/* Describe monster */
	roff_top(species_idx);
}



/*
 * Hack -- output description of the given monster race
 */
void output_monster_spoiler(int species_idx, void (*roff_func)(byte attr, cptr str))
{
	hook_c_roff = roff_func;

	/* Recall monster */
	roff_aux(p_ptr, &species_info[species_idx], 0x03);
}


bool mon_hook_dungeon(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!is_wild_only_species(r_ptr))
		return TRUE;
	else
	{
		dungeon_info_type *d_ptr = &d_info[dungeon_type];
		if ((d_ptr->mflags8 & RF8_WILD_MOUNTAIN) &&
		    is_wild_mountain_species(r_ptr)) return TRUE;
		return FALSE;
	}
}


static bool mon_hook_ocean(int species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	return is_wild_ocean_species(species_ptr);
}


static bool mon_hook_shore(int species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	return is_wild_shore_species(species_ptr);
}


static bool mon_hook_waste(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (is_wild_waste_species(r_ptr) || is_wild_all_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


static bool mon_hook_town(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (is_wild_all_species(r_ptr) || is_wild_town_species(r_ptr) || is_citizen_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


static bool mon_hook_wood(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	return (is_wild_wood_species(r_ptr) || is_wild_all_species(r_ptr));
}


static bool mon_hook_volcano(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	return is_wild_wood_species(r_ptr);
}


static bool mon_hook_mountain(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	return is_wild_mountain_species(r_ptr);
}


static bool mon_hook_grass(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	return (is_wild_grass_species(r_ptr) || is_wild_all_species(r_ptr));
}


static bool mon_hook_deep_water(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!mon_hook_dungeon(species_idx)) return FALSE;

	if (is_aquatic_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


static bool mon_hook_shallow_water(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!mon_hook_dungeon(species_idx)) return FALSE;

	if (r_ptr->flags2 & RF2_AURA_FIRE)
		return FALSE;
	else
		return TRUE;
}


static bool mon_hook_lava(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!mon_hook_dungeon(species_idx)) return FALSE;

	if (((r_ptr->flags10 & RF10_EFF_IM_FIRE_MASK) ||
	     can_fly_species(r_ptr)) &&
	    !is_aura_cold_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


static bool mon_hook_floor(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (!is_aquatic_species(r_ptr) ||
	    can_fly_species(r_ptr))
		return TRUE;
	else
		return FALSE;
}


monster_hook_type get_monster_hook(void)
{
	if (!dun_level && !inside_quest)
	{
		switch (wilderness[wilderness_y][wilderness_x].terrain)
		{
		case TERRAIN_TOWN:
			return (monster_hook_type)mon_hook_town;
		case TERRAIN_DEEP_WATER:
			return (monster_hook_type)mon_hook_ocean;
		case TERRAIN_SHALLOW_WATER:
		case TERRAIN_SWAMP:
			return (monster_hook_type)mon_hook_shore;
		case TERRAIN_DIRT:
		case TERRAIN_DESERT:
			return (monster_hook_type)mon_hook_waste;
		case TERRAIN_GRASS:
			return (monster_hook_type)mon_hook_grass;
		case TERRAIN_TREES:
			return (monster_hook_type)mon_hook_wood;
		case TERRAIN_SHALLOW_LAVA:
		case TERRAIN_DEEP_LAVA:
			return (monster_hook_type)mon_hook_volcano;
		case TERRAIN_MOUNTAIN:
			return (monster_hook_type)mon_hook_mountain;
		default:
			return (monster_hook_type)mon_hook_dungeon;
		}
	}
	else
	{
		return (monster_hook_type)mon_hook_dungeon;
	}
}


monster_hook_type get_monster_hook2(int y, int x)
{
	feature_type *f_ptr = &f_info[cave[y][x].feat];

	/* Set the monster list */

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		/* Deep water */
		if (have_flag(f_ptr->flags, FF_DEEP))
		{
			return (monster_hook_type)mon_hook_deep_water;
		}

		/* Shallow water */
		else
		{
			return (monster_hook_type)mon_hook_shallow_water;
		}
	}

	/* Lava */
	else if (have_flag(f_ptr->flags, FF_LAVA))
	{
		return (monster_hook_type)mon_hook_lava;
	}

	else return (monster_hook_type)mon_hook_floor;
}


void set_friendly(creature_type *m_ptr)
{
	m_ptr->smart |= SM_FRIENDLY;
}

void set_pet(creature_type *m_ptr)
{
	if (!is_pet(m_ptr)) check_pets_num_and_align(m_ptr, TRUE);

	/* Check for quest completion */
	check_quest_completion(p_ptr, m_ptr);

	m_ptr->smart |= SM_PET;
	if (!(is_enemy_of_evil_creature(m_ptr) && is_enemy_of_good_creature(m_ptr)))
		m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
}

/*
 * Makes the monster hostile towards the player
 */
void set_hostile(creature_type *m_ptr)
{
	if (inside_battle) return;

	if (is_pet(m_ptr)) check_pets_num_and_align(m_ptr, FALSE);

	m_ptr->smart &= ~SM_PET;
	m_ptr->smart &= ~SM_FRIENDLY;
}


/*
 * Anger the monster
 */
void anger_monster(creature_type *m_ptr)
{
	if (inside_battle) return;
	if (is_friendly(m_ptr))
	{
		char m_name[80];

		monster_desc(m_name, m_ptr, 0);
#ifdef JP
msg_format("%^sは怒った！", m_name);
#else
		msg_format("%^s gets angry!", m_name);
#endif

		set_hostile(m_ptr);
	}
}


/*
 * Check if monster can cross terrain
 */
bool monster_can_cross_terrain(s16b feat, species_type *r_ptr, u16b mode)
{
	feature_type *f_ptr = &f_info[feat];

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_RIDING))
		{
			if (!can_fly_species(r_ptr)) return FALSE;
		}
		else
		{
			if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
		}
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && can_fly_species(r_ptr)) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && can_swim_species(r_ptr)) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS))
	{
		if (is_pass_wall_species(r_ptr) && (!(mode & CEM_RIDING) || p_ptr->pass_wall)) return TRUE;
	}

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	/* Some monsters can walk on mountains */
	if (have_flag(f_ptr->flags, FF_MOUNTAIN) && is_wild_mountain_species(r_ptr)) return TRUE;

	/* Water */
	if (have_flag(f_ptr->flags, FF_WATER))
	{
		if (!is_aquatic_species(r_ptr))
		{
			/* Deep water */
			if (have_flag(f_ptr->flags, FF_DEEP)) return FALSE;

			/* Shallow water */
			else if (is_aura_fire_species(r_ptr)) return FALSE;
		}
	}

	/* Aquatic monster into non-water? */
	else if (is_aquatic_species(r_ptr)) return FALSE;

	/* Lava */
	if (have_flag(f_ptr->flags, FF_LAVA))
	{
		if (!(r_ptr->flags10 & RF10_EFF_IM_FIRE_MASK)) return FALSE;
	}

	return TRUE;
}


/*
 * Strictly check if monster can enter the grid
 */
bool monster_can_enter(int y, int x, species_type *r_ptr, u16b mode)
{
	cave_type *c_ptr = &cave[y][x];

	/* Player or other monster */
	if (creature_bold(p_ptr, y, x)) return FALSE;
	if (c_ptr->m_idx) return FALSE;

	return monster_can_cross_terrain(c_ptr->feat, r_ptr, mode);
}


/*
 * Check if this monster has "hostile" alignment (aux)
 */
static bool check_hostile_align(byte sub_align1, byte sub_align2)
{
	if (sub_align1 != sub_align2)
	{
		if (((sub_align1 & SUB_ALIGN_EVIL) && (sub_align2 & SUB_ALIGN_GOOD)) ||
			((sub_align1 & SUB_ALIGN_GOOD) && (sub_align2 & SUB_ALIGN_EVIL)))
			return TRUE;
	}

	/* Non-hostile alignment */
	return FALSE;
}


/*
 * Check if two monsters are enemies
 */
bool are_enemies(creature_type *m_ptr, creature_type *n_ptr)
{
	species_type *r_ptr = &species_info[m_ptr->species_idx];
	species_type *s_ptr = &species_info[n_ptr->species_idx];

	if (inside_battle)
	{
		if (is_pet(m_ptr) || is_pet(n_ptr)) return FALSE;
		return TRUE;
	}

	if ((is_wild_all_species(r_ptr) || is_wild_town_species(r_ptr) || is_citizen_species(r_ptr))
	    && (is_wild_all_species(r_ptr) || is_wild_town_species(r_ptr) || is_citizen_species(s_ptr)))
	{
		if (!is_pet(m_ptr) && !is_pet(n_ptr)) return FALSE;
	}

	/* Friendly vs. opposite aligned normal or pet */
	if (check_hostile_align(m_ptr->sub_align, n_ptr->sub_align))
	{
		if (!(m_ptr->mflag2 & MFLAG2_CHAMELEON) || !(n_ptr->mflag2 & MFLAG2_CHAMELEON)) return TRUE;
	}

	/* Hostile vs. non-hostile */
	if (is_hostile(m_ptr) != is_hostile(n_ptr))
	{
		return TRUE;
	}

	/* Default */
	return FALSE;
}


/*
 * Check if this monster race has "hostile" alignment
 * If user is player, m_ptr == NULL.
 */
bool monster_has_hostile_align(creature_type *m_ptr, int pa_good, int pa_evil, species_type *r_ptr)
{
	byte sub_align1 = SUB_ALIGN_NEUTRAL;
	byte sub_align2 = SUB_ALIGN_NEUTRAL;

	if (m_ptr) /* For a monster */
	{
		sub_align1 = m_ptr->sub_align;
	}
	else /* For player */
	{
		if (p_ptr->good_rank < 0) sub_align1 |= SUB_ALIGN_GOOD;
		if (p_ptr->evil_rank < 0) sub_align1 |= SUB_ALIGN_EVIL;
	}

	/* Racial alignment flags */
	if (is_enemy_of_good_creature(m_ptr)) sub_align2 |= SUB_ALIGN_EVIL;
	if (is_enemy_of_evil_creature(m_ptr)) sub_align2 |= SUB_ALIGN_GOOD;

	if (check_hostile_align(sub_align1, sub_align2)) return TRUE;

	/* Non-hostile alignment */
	return FALSE;
}


/*
 * Is the monster "alive"?
 *
 * Used to determine the message to print for a killed monster.
 * ("dies", "destroyed")
 */
bool monster_living(species_type *r_ptr)
{
	/* Non-living, undead, or demon */
	if (r_ptr->flags3 & RF3_NONLIVING || is_undead_species(r_ptr)) return FALSE;

	if (is_demon_species(r_ptr)) return FALSE;

	return TRUE;
}


/*
 * Is this monster declined to be questor or bounty?
 */
bool no_questor_or_bounty_uniques(int species_idx)
{
	switch (species_idx)
	{
	/*
	 * Decline them to be questor or bounty because they use
	 * special motion "split and combine"
	 */
	case MON_BANORLUPART:
	case MON_BANOR:
	case MON_LUPART:
		return TRUE;
	default:
		return FALSE;
	}
}
