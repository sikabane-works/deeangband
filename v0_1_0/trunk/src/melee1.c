/* File: melee1.c */

/* Purpose: Monster attacks */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * Critical blow.  All hits that do 95% of total possible damage,
 * and which also do at least 20 damage, or, sometimes, N damage.
 * This is used only to determine "cuts" and "stuns".
 */
static int monster_critical(int dice, int sides, int dam)
{
	int max = 0;
	int total = dice * sides;

	/* Must do at least 95% of perfect */
	if (dam < total * 19 / 20) return (0);

	/* Weak blows rarely work */
	if ((dam < 20) && (randint0(100) >= dam)) return (0);

	/* Perfect damage */
	if ((dam >= total) && (dam >= 40)) max++;

	/* Super-charge */
	if (dam >= 20)
	{
		while (randint0(100) < 2) max++;
	}

	/* Critical damage */
	if (dam > 45) return (6 + max);
	if (dam > 33) return (5 + max);
	if (dam > 25) return (4 + max);
	if (dam > 18) return (3 + max);
	if (dam > 11) return (2 + max);
	return (1 + max);
}





/*
 * Determine if a monster attack against the player succeeds.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match monster power against player armor.
 */
static int check_hit(creature_type *tar_ptr, int power, int level, int stun)
{
	int i, k, ac;

	/* Percentile dice */
	k = randint0(100);

	if (stun && one_in_(2)) return FALSE;

	/* Hack -- Always miss or hit */
	if (k < 10) return (k < 5);

	/* Calculate the "attack quality" */
	i = (power + (level * 3));

	/* Total armor */
	ac = tar_ptr->ac + tar_ptr->to_a;
	if (tar_ptr->special_attack & ATTACK_SUIKEN) ac += (tar_ptr->lev * 2);

	/* Power and Level compete against Armor */
	if ((i > 0) && (randint1(i) > ((ac * 3) / 4))) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_insult[] =
{
#ifdef JP
	"Ç™Ç†Ç»ÇΩÇïéêJÇµÇΩÅI",
	"Ç™Ç†Ç»ÇΩÇÃïÍÇïéêJÇµÇΩÅI",
	"Ç™Ç†Ç»ÇΩÇåyïÃÇµÇΩÅI",
	"Ç™Ç†Ç»ÇΩÇêJÇﬂÇΩÅI",
	"Ç™Ç†Ç»ÇΩÇâòÇµÇΩÅI",
	"Ç™Ç†Ç»ÇΩÇÃâÒÇËÇ≈óxÇ¡ÇΩÅI",
	"Ç™‡ŒÂˆÇ»êgÇ‘ÇËÇÇµÇΩÅI",
	"Ç™Ç†Ç»ÇΩÇÇ⁄ÇÒÇ‚ÇËÇ∆å©ÇΩÅIÅIÅI",
	"Ç™Ç†Ç»ÇΩÇÉpÉâÉTÉCÉgåƒÇŒÇÌÇËÇµÇΩÅI",
	"Ç™Ç†Ç»ÇΩÇÉTÉCÉ{Å[ÉOàµÇ¢ÇµÇΩÅI"
#else
	"insults you!",
	"insults your mother!",
	"gives you the finger!",
	"humiliates you!",
	"defiles you!",
	"dances around you!",
	"makes obscene gestures!",
	"moons you!!!"
	"calls you a parasite!",
	"calls you a cyborg!"
#endif

};



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_moan[] =
{
#ifdef JP
	"ÇÕâΩÇ©ÇîﬂÇµÇÒÇ≈Ç¢ÇÈÇÊÇ§ÇæÅB",
	"Ç™îﬁÇÃéîÇ¢å¢Çå©Ç»Ç©Ç¡ÇΩÇ©Ç∆êqÇÀÇƒÇ¢ÇÈÅB",
	"Ç™ìÍí£ÇËÇ©ÇÁèoÇƒçsÇØÇ∆åæÇ¡ÇƒÇ¢ÇÈÅB",
	"ÇÕÉLÉmÉRÇ™Ç«Ç§Ç∆Ç©ôÍÇ¢ÇƒÇ¢ÇÈÅB"
#else
	"seems sad about something.",
	"asks if you have seen his dogs.",
	"tells you to get off his land.",
	"mumbles something about mushrooms."
#endif

};


/*
 * Attack the player via physical attacks.
 */
bool special_melee(creature_type *atk_ptr, creature_type *tar_ptr)
{

	species_type *r_ptr = &r_info[atk_ptr->species_idx];

	int ap_cnt;

	int i, k, tmp, ac, rlev;
	int do_cut, do_stun;

	s32b gold;

	object_type *o_ptr;

	char o_name[MAX_NLEN];

	char atk_name[100];
	char tar_name[100];

	char ddesc[80];

	bool blinked;
	bool touched = FALSE, fear = FALSE, alive = TRUE;
	bool explode = FALSE;
	bool do_silly_attack = (one_in_(2) && tar_ptr->image);
	int get_damage = 0;
#ifdef JP
	int abbreviate = 0;
#endif

	/* Not allowed to attack */
	if (is_never_blow_creature(atk_ptr)) return (FALSE);

	if (d_info[dungeon_type].flags1 & DF1_NO_MELEE) return (FALSE);

	/* ...nor if friendly */
	if (!is_hostile(atk_ptr)) return FALSE;

	/* Extract the effective monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);


	/* Get the monster name (or "it") */
	monster_desc(atk_name, atk_ptr, 0);
	monster_desc(tar_name, tar_ptr, 0);

	/* Get the "died from" information (i.e. "a kobold") */
	monster_desc(ddesc, atk_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

	if (tar_ptr->special_defense & KATA_IAI)
	{
#ifdef JP
		msg_format("%sÇÕëäéËÇ™èPÇ¢Ç©Ç©ÇÈëOÇ…ëfëÅÇ≠ïêäÌÇêUÇÈÇ¡ÇΩÅB", tar_name);
#else
		msg_format("%s took \"sen\", drew and cut in one motion before %s moved.", tar_name, atk_name);
#endif
		if (creature_attack(tar_ptr, atk_ptr->fy, atk_ptr->fx, HISSATSU_IAI)) return TRUE;
	}

	if ((tar_ptr->special_defense & NINJA_KAWARIMI) && (randint0(55) < (tar_ptr->lev*3/5+20)))
	{
		if (kawarimi(TRUE)) return TRUE;
	}

	/* Assume no blink */
	blinked = FALSE;

	/* Scan through all four blows */
	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		bool obvious = FALSE;

		int power = 0;
		int damage = 0;

		cptr act = NULL;

		/* Extract the attack infomation */
		int effect = r_ptr->blow[ap_cnt].effect;
		int method = r_ptr->blow[ap_cnt].method;
		int d_dice = r_ptr->blow[ap_cnt].d_dice;
		int d_side = r_ptr->blow[ap_cnt].d_side;


		if (!atk_ptr->species_idx) break;

		/* Hack -- no more attacks */
		if (!method) break;

		if (is_pet(atk_ptr) && (is_unique_species(r_ptr)) && (method == RBM_EXPLODE))
		{
			method = RBM_HIT;
			d_dice /= 10;
		}

		/* Stop if player is dead or gone */
		if (!tar_ptr->playing || tar_ptr->is_dead) break;
		if (distance(tar_ptr->fy, tar_ptr->fx, atk_ptr->fy, atk_ptr->fx) > 1) break;

		/* Handle "leaving" */
		if (tar_ptr->leaving) break;

		if (method == RBM_SHOOT) continue;

		/* Extract the attack "power" */
		power = mbe_info[effect].power;

		/* Total armor */
		ac = tar_ptr->ac + tar_ptr->to_a;

		/* Monster hits player */
		if (!effect || check_hit(tar_ptr, power, rlev, atk_ptr->stun))
		{
			/* Always disturbing */
			disturb(1, 0);


			/* Hack -- Apply "protection from evil" */
			if ((tar_ptr->protevil > 0) &&
			    is_enemy_of_good_creature(tar_ptr) &&
			    (tar_ptr->lev >= rlev) &&
			    ((randint0(100) + tar_ptr->lev) > 50))
			{
				/* Remember the Evil-ness */
				//TODO if (is_original_ap_and_seen(tar_ptr, atk_ptr)) r_ptr->r_flags3 |= RF3_EVIL;

				/* Message */
#ifdef JP
				if (abbreviate)
				    msg_format("åÇëﬁÇµÇΩÅB");
				else
				    msg_format("%^sÇÕåÇëﬁÇ≥ÇÍÇΩÅB", atk_name);
				abbreviate = 1;/*ÇQâÒñ⁄à»ç~ÇÕè»ó™ */
#else
				msg_format("%^s is repelled.", atk_name);
#endif


				/* Hack -- Next attack */
				continue;
			}


			/* Assume no cut or stun */
			do_cut = do_stun = 0;

			/* Describe the attack method */
			switch (method)
			{
				case RBM_HIT:
				{
#ifdef JP
					abbreviate = 0;
					act = "â£Ç¡ÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "hit";
					else
						act = "hits";
#endif

					do_cut = do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

				case RBM_TOUCH:
				{
#ifdef JP
					abbreviate = 1;
					act = "êGÇÍÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "touch";
					else
						act = "touches";
#endif

					touched = TRUE;
					sound(SOUND_TOUCH);
					break;
				}

				case RBM_PUNCH:
				{
#ifdef JP
					abbreviate = 0;
					act = "ÉpÉìÉ`ÇµÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "punch";
					else
						act = "punches";
#endif

					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

				case RBM_KICK:
				{
#ifdef JP
					abbreviate = 0;
					act = "èRÇ¡ÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "kick";
					else
						act = "kicks";
#endif

					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

				case RBM_CLAW:
				{
#ifdef JP
					abbreviate = 0;
					act = "à¯Ç¡ë~Ç¢ÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "claw";
					else
						act = "claws";
#endif

					touched = TRUE;
					do_cut = 1;
					sound(SOUND_CLAW);
					break;
				}

				case RBM_BITE:
				{
#ifdef JP
					abbreviate = 0;
					act = "äöÇÒÇæÅB";
#else
					if(is_player(atk_ptr))
						act = "bite";
					else
						act = "bites";
#endif

					do_cut = 1;
					touched = TRUE;
					sound(SOUND_BITE);
					break;
				}

				case RBM_STING:
				{
#ifdef JP
					abbreviate = 0;
					act = "éhÇµÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "sting";
					else
						act = "stings";
#endif

					touched = TRUE;
					sound(SOUND_STING);
					break;
				}

				case RBM_SLASH:
				{
#ifdef JP
					abbreviate = 0;
					act = "éaÇ¡ÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "slash";
					else
						act = "slashes";
#endif

					touched = TRUE;
					do_cut = 1;
					sound(SOUND_CLAW);
					break;
				}

				case RBM_BUTT:
				{
#ifdef JP
					abbreviate = 0;
					act = "äpÇ≈ìÀÇ¢ÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "butt";
					else
						act = "butts";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

				case RBM_CRUSH:
				{
#ifdef JP
					abbreviate = 1;
					act = "ëÃìñÇΩÇËÇµÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "crush";
					else
						act = "crushes";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

				case RBM_ENGULF:
				{
#ifdef JP
					abbreviate = 0;
					act = "à˘Ç›çûÇÒÇæÅB";
#else
					if(is_player(atk_ptr))
						act = "engulf";
					else
						act = "engulfs";
#endif

					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

				case RBM_CHARGE:
				{
#ifdef JP
					abbreviate = 1;
					act = "êøãÅèëÇÇÊÇ±ÇµÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "charge";
					else
						act = "charges";
#endif

					touched = TRUE;
					sound(SOUND_BUY); /* Note! This is "charges", not "charges at". */
					break;
				}

				case RBM_CRAWL:
				{
#ifdef JP
					abbreviate = 2;
					act = "ëÃÇÃè„ÇîáÇ¢âÒÇ¡ÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "crawl on";
					else
						act = "crawls on";
#endif

					touched = TRUE;
					sound(SOUND_SLIME);
					break;
				}

				case RBM_DROOL:
				{
#ifdef JP
					abbreviate = 1;
					act = "ÇÊÇæÇÍÇÇΩÇÁÇµÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "drool on";
					else
						act = "drools on";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_SPIT:
				{
#ifdef JP
					abbreviate = 3;
					act = "ë¡ÇìfÇ´Ç©ÇØÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "spit on";
					else
						act = "spits on";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_EXPLODE:
				{
#ifdef JP
					abbreviate = 4;
					act = "ÇÕîöî≠ÇµÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "explode";
					else
						act = "explodes";
#endif

					explode = TRUE;
					break;
				}

				case RBM_GAZE:
				{
#ifdef JP
					abbreviate = 0;
					act = "Ç…ÇÁÇÒÇæÅB";
#else
					if(is_player(atk_ptr))
						act = "gaze at";
					else
						act = "gazes at";
#endif

					break;
				}

				case RBM_WAIL:
				{
#ifdef JP
					abbreviate = 3;
					act = "ãÉÇ´ã©ÇÒÇæÅB";
#else
					if(is_player(atk_ptr))
						act = "wail at";
					else
						act = "wails at";
#endif

					sound(SOUND_WAIL);
					break;
				}

				case RBM_SPORE:
				{
#ifdef JP
					abbreviate = 3;
					act = "ñEéqÇîÚÇŒÇµÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "release spores at";
					else
						act = "releases spores at";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_XXX4:
				{
#ifdef JP
					abbreviate = 3;
					act = "XXX4 Çî≠éÀÇµÇΩÅB";
#else
					if(is_player(atk_ptr))
						act = "project XXX4's at";
					else
						act = "projects XXX4's at";
#endif

					break;
				}

				case RBM_BEG:
				{
#ifdef JP
					abbreviate = 1;
					act = "ã‡ÇÇπÇ™ÇÒÇæÅB";
#else
					if(is_player(atk_ptr))
						act = "beg";
					else
						act = "begs";
#endif

					sound(SOUND_MOAN);
					break;
				}

				case RBM_INSULT:
				{
#ifdef JP
					abbreviate = 2;
#endif
					act = desc_insult[randint0(atk_ptr->species_idx == MON_DEBBY ? 10 : 8)];
					sound(SOUND_MOAN);
					break;
				}

				case RBM_MOAN:
				{
#ifdef JP
					abbreviate = 2;
#endif
					act = desc_moan[randint0(4)];
					sound(SOUND_MOAN);
					break;
				}

				case RBM_SHOW:
				{
#ifdef JP
					abbreviate = 4;
#endif
					if (atk_ptr->species_idx == MON_JAIAN)
					{
#ifdef JP
						switch(randint1(15))
						{
						  case 1:
						  case 6:
						  case 11:
							act = "ÅuÅÙÇ®Å`ÇÍÇÕÉWÉÉÉCÉAÉìÅ`Å`ÉKÅ`ÉLÇæÇ¢ÇµÇÂÇ§Å`Åv";
							break;
						  case 2:
							act = "ÅuÅÙÇƒÅ`ÇÒÇ©ÇﬁÅ`ÇƒÇ´ÇÃÇ®Å`Ç∆Ç±ÇæÇ∫Å`Å`Åv";
							break;
						  case 3:
							act = "ÅuÅÙÇÃÅ`Ç—ëæÉXÉlïvÇÕÉÅÇ∂Ç·Ç»Ç¢ÇÊÅ`Å`Åv";
							break;
						  case 4:
							act = "ÅuÅÙÇØÅ`ÇÒÇ©ÉXÉ|Å`ÉcÅ`Ç«ÇÒÇ∆Ç±Ç¢Å`Åv";
							break;
						  case 5:
							act = "ÅuÅÙÇ§ÇΩÅ`Ç‡Å`Å`Ç§Å`Ç‹Ç¢Ç∫Å`Ç‹Ç©ÇµÇ∆ÇØÅ`Åv";
							break;
						  case 7:
							act = "ÅuÅÙÇ‹Å`ÇøÇ¢ÇøÇŒÅ`ÇÒÇÃÇ…Å`ÇÒÇ´Ç‡ÇÃÅ`Å`Åv";
							break;
						  case 8:
							act = "ÅuÅÙÇ◊ÇÒÇ´ÇÂÇ§ÇµÇ„Ç≠ÇæÇ¢ÉÅÇ∂Ç·Ç»Ç¢ÇÊÅ`Å`Åv";
							break;
						  case 9:
							act = "ÅuÅÙÇ´ÇÕÇ‚Ç≥ÇµÅ`Ç≠ÇƒÅ`ÇøÅ`Ç©ÇÁÇ‡ÇøÅ`Åv";
							break;
						  case 10:
							act = "ÅuÅÙÇ©Ç®Å`Ç‡Å`Å`ÉXÉ^ÉCÉãÇ‡Å`ÉoÉcÉOÉìÇ≥Å`Åv";
							break;
						  case 12:
							act = "ÅuÅÙÇ™Ç¡Ç±Ç§Ç¢Å`ÇøÇÃÅ`Ç†Å`ÇŒÇÍÇÒÇ⁄Ç§Å`Å`Åv";
							break;
						  case 13:
							act = "ÅuÅÙÉhÅ`ÉâÇ‡ÉhÉâÉ~Ç‡ÉÅÇ∂Ç·Ç»Ç¢ÇÊÅ`Å`Åv";
							break;
						  case 14:
							act = "ÅuÅÙÇÊÇ∂Ç∞ÇÒÇ€ÇØÇ¡Ç∆Å`Ç»Å`Ç≠ÇΩÇ¡ÇƒÅ`Åv";
							break;
						  case 15:
							act = "ÅuÅÙÇ†ÇµÅ`ÇÃÅ`Å`Ç»Ç™Ç≥Å`ÇÕÅ`Ç‹ÇØÇ»Ç¢Ç∫Å`Åv";
							break;
						}
#else
						act = "horribly sings 'I AM GIAAAAAN. THE BOOOSS OF THE KIIIIDS.'";
#endif
					}
					else
					{
						if (one_in_(3))
#ifdef JP
							act = "ÇÕÅÙñlÇÁÇÕäyÇµÇ¢â∆ë∞ÅÙÇ∆âÃÇ¡ÇƒÇ¢ÇÈÅB";
						else
							act = "ÇÕÅÙÉAÉC ÉâÉu ÉÜÅ[ÅAÉÜÅ[ ÉâÉu É~Å[ÅÙÇ∆âÃÇ¡ÇƒÇ¢ÇÈÅB";
#else
							act = "sings 'We are a happy family.'";
						else
							act = "sings 'I love you, you love me.'";
#endif
					}

					sound(SOUND_SHOW);
					break;
				}
			}

			/* Message */
			if (act)
			{
				if (do_silly_attack)
				{
#ifdef JP
					abbreviate = -1;
#endif
					act = silly_attacks[randint0(MAX_SILLY_ATTACK)];
				}
#ifdef JP
				if (abbreviate == 0)
				    msg_format("%^sÇÕ%sÇ%s", atk_name, tar_name, act);
				else if (abbreviate == 1)
				    msg_format("%^sÇÕ%sÇ…%s", atk_name, tar_name, act);
				else if (abbreviate == 2)
				    msg_format("%^sÇÕ%sÇÃ%s", atk_name, tar_name, act);
				else if (abbreviate == 3)
				    msg_format("%^sÇÕ%sÇ…å¸ÇØ%s", atk_name, tar_name, act);
				else if (abbreviate == 4)
				    msg_format("%^s%s", atk_name, act);
				else /* if (abbreviate == -1) */
				    msg_format("%s", act);
				abbreviate = -1;/*ÇQâÒñ⁄à»ç~ÇÕè»ó™ */
#else
				msg_format("%^s %s %s", atk_name, act, do_silly_attack ? tar_name : "");
#endif
			}

			/* Hack -- assume all attacks are obvious */
			obvious = TRUE;

			/* Roll out the damage */
			damage = damroll(d_dice, d_side);

			/*
			 * Skip the effect when exploding, since the explosion
			 * already causes the effect.
			 */
			if (explode)
				damage = 0;
			/* Apply appropriate damage */
			switch (effect)
			{
				case 0:
				{
					/* Hack -- Assume obvious */
					obvious = TRUE;

					/* Hack -- No damage */
					damage = 0;

					break;
				}

				case RBE_SUPERHURT:
				{
					if (((randint1(rlev*2+300) > (ac+200)) || one_in_(13)) && !(tar_ptr->multishadow && (turn & 1)))
					{
						int tmp_damage = damage - (damage * ((ac < 150) ? ac : 150) / 250);
#ifdef JP
						msg_print("ÉNÉäÉeÉBÉJÉãÉqÉbÉgÅI");
#else
						msg_print("It was a critical hit!");
#endif

						tmp_damage = MAX(damage, tmp_damage*2);

						/* Take damage */
						get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, tmp_damage, ddesc, NULL, -1);
						break;
					}
				}
				case RBE_HURT:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Player armor reduces total damage */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					break;
				}

				case RBE_POISON:
				{
					if (explode) break;

					/* Take "poison" effect */
					if (!(tar_ptr->resist_pois || IS_OPPOSE_POIS(tar_ptr)) && !(tar_ptr->multishadow && (turn & 1)))
					{
						if (set_poisoned(tar_ptr, tar_ptr->poisoned + randint1(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					/* Take some damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_POIS);

					break;
				}

				case RBE_UN_BONUS:
				{
					if (explode) break;

					/* Allow complete resist */
					if (!tar_ptr->resist_disen && !(tar_ptr->multishadow && (turn & 1)))
					{
						/* Apply disenchantment */
						if (apply_disenchant(tar_ptr, 0))
						{
							/* Hack -- Update AC */
							update_stuff(tar_ptr, TRUE);
							obvious = TRUE;
						}
					}

					/* Take some damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_DISEN);

					break;
				}

				case RBE_UN_POWER:
				{
					/* Take some damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item */
						i = randint0(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &tar_ptr->inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Drain charged wands/staffs */
						if (((o_ptr->tval == TV_STAFF) ||
						     (o_ptr->tval == TV_WAND)) &&
						    (o_ptr->pval))
						{
							/* Calculate healed hitpoints */
							int heal=rlev * o_ptr->pval;
							if( o_ptr->tval == TV_STAFF)
							    heal *=  o_ptr->number;

							/* Don't heal more than max hp */
							heal = MIN(heal, atk_ptr->mhp - atk_ptr->chp);

							/* Message */
#ifdef JP
							msg_print("ñÇìπãÔÇ©ÇÁÉGÉlÉãÉMÅ[ÇãzÇ¢éÊÇ¡ÇΩÅI");
#else
							msg_print("Energy drains from your magic device!");
#endif


							/* Obvious */
							obvious = TRUE;

							/* Heal the monster */
							atk_ptr->chp += heal;

							/* Redraw (later) if needed */
							//TODO if (&m_info[tar_ptr->health_who] == atk_ptr) play_redraw |= (PR_HEALTH);
							//if (&m_info[tar_ptr->riding] == atk_ptr) play_redraw |= (PR_UHEALTH);

							/* Uncharge */
							o_ptr->pval = 0;

							/* Combine / Reorder the pack */
							tar_ptr->notice |= (PN_COMBINE | PN_REORDER);

							/* Window stuff */
							play_window |= (PW_INVEN);

							/* Done */
							break;
						}
					}

					break;
				}

				case RBE_EAT_GOLD:
				{
					/* Take some damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Confused monsters cannot steal successfully. -LM-*/
					if (atk_ptr->confused) break;

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Obvious */
					obvious = TRUE;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!tar_ptr->paralyzed &&
					    (randint0(100) < (adj_dex_safe[tar_ptr->stat_ind[A_DEX]] +
							      tar_ptr->lev)))
					{
						/* Saving throw message */
#ifdef JP
						msg_print("ÇµÇ©ÇµëfëÅÇ≠ç‡ïzÇéÁÇ¡ÇΩÅI");
#else
						msg_print("You quickly protect your money pouch!");
#endif


						/* Occasional blink anyway */
						if (randint0(3)) blinked = TRUE;
					}

					/* Eat gold */
					else
					{
						gold = (tar_ptr->au / 10) + randint1(25);
						if (gold < 2) gold = 2;
						if (gold > 5000) gold = (tar_ptr->au / 20) + randint1(3000);
						if (gold > tar_ptr->au) gold = tar_ptr->au;
						tar_ptr->au -= gold;
						if (gold <= 0)
						{
#ifdef JP
							msg_print("ÇµÇ©ÇµâΩÇ‡ìêÇ‹ÇÍÇ»Ç©Ç¡ÇΩÅB");
#else
							msg_print("Nothing was stolen.");
#endif

						}
						else if (tar_ptr->au)
						{
#ifdef JP
							msg_print("ç‡ïzÇ™åyÇ≠Ç»Ç¡ÇΩãCÇ™Ç∑ÇÈÅB");
							msg_format("$%ld ÇÃÇ®ã‡Ç™ìêÇ‹ÇÍÇΩÅI", (long)gold);
#else
							msg_print("Your purse feels lighter.");
							msg_format("%ld coins were stolen!", (long)gold);
#endif
						}
						else
						{
#ifdef JP
							msg_print("ç‡ïzÇ™åyÇ≠Ç»Ç¡ÇΩãCÇ™Ç∑ÇÈÅB");
							msg_print("Ç®ã‡Ç™ëSïîìêÇ‹ÇÍÇΩÅI");
#else
							msg_print("Your purse feels lighter.");
							msg_print("All of your coins were stolen!");
#endif
						}

						/* Redraw gold */
						play_redraw |= (PR_GOLD);

						/* Window stuff */
						play_window |= (PW_PLAYER);

						/* Blink away */
						blinked = TRUE;
					}

					break;
				}

				case RBE_EAT_ITEM:
				{
					/* Take some damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Confused monsters cannot steal successfully. -LM-*/
					if (atk_ptr->confused) break;

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!tar_ptr->paralyzed &&
					    (randint0(100) < (adj_dex_safe[tar_ptr->stat_ind[A_DEX]] +
							      tar_ptr->lev)))
					{
						/* Saving throw message */
#ifdef JP
						msg_print("ÇµÇ©ÇµÇ†ÇÌÇƒÇƒÉUÉbÉNÇéÊÇËï‘ÇµÇΩÅI");
#else
						msg_print("You grab hold of your backpack!");
#endif


						/* Occasional "blink" anyway */
						blinked = TRUE;

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						s16b o_idx;

						/* Pick an item */
						i = randint0(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &tar_ptr->inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip artifacts */
						if (object_is_artifact(tar_ptr, o_ptr)) continue;

						/* Get a description */
						object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

						/* Message */
#ifdef JP
						msg_format("%s(%c)Ç%sìêÇ‹ÇÍÇΩÅI",
							   o_name, index_to_label(tar_ptr, i),
							   ((o_ptr->number > 1) ? "àÍÇ¬" : ""));
#else
						msg_format("%sour %s (%c) was stolen!",
							   ((o_ptr->number > 1) ? "One of y" : "Y"),
							   o_name, index_to_label(tar_ptr, i));
#endif

						/* Make an object */
						o_idx = o_pop();

						/* Success */
						if (o_idx)
						{
							object_type *j_ptr;

							/* Get new object */
							j_ptr = &o_list[o_idx];

							/* Copy object */
							object_copy(j_ptr, o_ptr);

							/* Modify number */
							j_ptr->number = 1;

							/* Hack -- If a rod or wand, allocate total
							 * maximum timeouts or charges between those
							 * stolen and those missed. -LM-
							 */
							if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
							{
								j_ptr->pval = o_ptr->pval / o_ptr->number;
								o_ptr->pval -= j_ptr->pval;
							}

							/* Forget mark */
							j_ptr->marked = OM_TOUCHED;

							/* Memorize monster */
							//TODO j_ptr->held_m_idx = m_idx;

							/* Build stack */
							j_ptr->next_o_idx = atk_ptr->hold_o_idx;

							/* Build stack */
							atk_ptr->hold_o_idx = o_idx;
						}

						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Blink away */
						blinked = TRUE;

						/* Done */
						break;
					}

					break;
				}

				case RBE_EAT_FOOD:
				{
					/* Take some damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Steal some food */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item from the pack */
						i = randint0(INVEN_PACK);

						/* Get the item */
						o_ptr = &tar_ptr->inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip non-food objects */
						if ((o_ptr->tval != TV_FOOD) && !((o_ptr->tval == TV_CORPSE) && (o_ptr->sval))) continue;

						/* Get a description */
						object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

						/* Message */
#ifdef JP
						msg_format("%s(%c)Ç%sêHÇ◊ÇÁÇÍÇƒÇµÇ‹Ç¡ÇΩÅI",
							  o_name, index_to_label(tar_ptr, i),
							  ((o_ptr->number > 1) ? "àÍÇ¬" : ""));
#else
						msg_format("%sour %s (%c) was eaten!",
							   ((o_ptr->number > 1) ? "One of y" : "Y"),
							   o_name, index_to_label(tar_ptr, i));
#endif


						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					break;
				}

				case RBE_EAT_LITE:
				{
					/* Access the lite */
					o_ptr = &tar_ptr->inventory[INVEN_LITE];

					/* Take some damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Drain fuel */
					if ((o_ptr->xtra4 > 0) && (!object_is_fixed_artifact(o_ptr)))
					{
						/* Reduce fuel */
						o_ptr->xtra4 -= (250 + (s16b)randint1(250));
						if (o_ptr->xtra4 < 1) o_ptr->xtra4 = 1;

						/* Notice */
						if (!tar_ptr->blind)
						{
#ifdef JP
							msg_print("ñæÇ©ÇËÇ™à√Ç≠Ç»Ç¡ÇƒÇµÇ‹Ç¡ÇΩÅB");
#else
							msg_print("Your light dims.");
#endif

							obvious = TRUE;
						}

						/* Window stuff */
						play_window |= (PW_EQUIP);
					}

					break;
				}

				case RBE_ACID:
				{
					if (explode) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("é_ÇóÅÇ—ÇπÇÁÇÍÇΩÅI");
#else
					msg_print("You are covered in acid!");
#endif


					/* Special damage */
					get_damage += acid_dam(tar_ptr, damage, ddesc, -1);

					/* Hack -- Update AC */
					update_stuff(tar_ptr, TRUE);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_ACID);

					break;
				}

				case RBE_ELEC:
				{
					if (explode) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("ìdåÇÇóÅÇ—ÇπÇÁÇÍÇΩÅI");
#else
					msg_print("You are struck by electricity!");
#endif


					/* Special damage */
					get_damage += elec_dam(tar_ptr, damage, ddesc, -1);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_ELEC);

					break;
				}

				case RBE_FIRE:
				{
					if (explode) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("ëSêgÇ™âäÇ…ïÔÇ‹ÇÍÇΩÅI");
#else
					msg_print("You are enveloped in flames!");
#endif


					/* Special damage */
					get_damage += fire_dam(tar_ptr, damage, ddesc, -1);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_FIRE);

					break;
				}

				case RBE_COLD:
				{
					if (explode) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("ëSêgÇ™ó‚ãCÇ≈ï¢ÇÌÇÍÇΩÅI");
#else
					msg_print("You are covered with frost!");
#endif


					/* Special damage */
					get_damage += cold_dam(tar_ptr, damage, ddesc, -1);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_COLD);

					break;
				}

				case RBE_BLIND:
				{
					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead) break;

					/* Increase "blind" */
					if (!tar_ptr->resist_blind && !(tar_ptr->multishadow && (turn & 1)))
					{
						if (set_blind(tar_ptr, tar_ptr->blind + 10 + randint1(rlev)))
						{
#ifdef JP
							if (atk_ptr->species_idx == MON_DIO) msg_print("ÅuÇ«Ç§ÇæÉbÅIÇ±ÇÃååÇÃñ⁄í◊ÇµÇÕÉbÅIÅv");
#else
							/* nanka */
#endif
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_BLIND);

					break;
				}

				case RBE_CONFUSE:
				{
					if (explode) break;
					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead) break;

					/* Increase "confused" */
					if (!tar_ptr->resist_conf && !(tar_ptr->multishadow && (turn & 1)))
					{
						if (set_confused(tar_ptr, tar_ptr->confused + 3 + randint1(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_CONF);

					break;
				}

				case RBE_TERRIFY:
				{
					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead) break;

					/* Increase "afraid" */
					if ((tar_ptr->multishadow && (turn & 1)))
					{
						/* Do nothing */
					}
					else if (tar_ptr->resist_fear)
					{
#ifdef JP
						msg_print("ÇµÇ©Çµã∞ï|Ç…êNÇ≥ÇÍÇ»Ç©Ç¡ÇΩÅI");
#else
						msg_print("You stand your ground!");
#endif

						obvious = TRUE;
					}
					else if (randint0(100 + r_ptr->level/2) < tar_ptr->skill_rob)
					{
#ifdef JP
						msg_print("ÇµÇ©Çµã∞ï|Ç…êNÇ≥ÇÍÇ»Ç©Ç¡ÇΩÅI");
#else
						msg_print("You stand your ground!");
#endif

						obvious = TRUE;
					}
					else
					{
						if (set_afraid(tar_ptr, tar_ptr->afraid + 3 + randint1(rlev)))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_FEAR);

					break;
				}

				case RBE_PARALYZE:
				{
					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead) break;

					/* Increase "paralyzed" */
					if ((tar_ptr->multishadow && (turn & 1)))
					{
						/* Do nothing */
					}
					else if (tar_ptr->free_act)
					{
#ifdef JP
						msg_print("ÇµÇ©Çµå¯â Ç™Ç»Ç©Ç¡ÇΩÅI");
#else
						msg_print("You are unaffected!");
#endif

						obvious = TRUE;
					}
					else if (randint0(100 + r_ptr->level/2) < tar_ptr->skill_rob)
					{
#ifdef JP
						msg_print("ÇµÇ©Çµå¯óÕÇíµÇÀï‘ÇµÇΩÅI");
#else
						msg_print("You resist the effects!");
#endif

						obvious = TRUE;
					}
					else
					{
						if (!tar_ptr->paralyzed)
						{
							if (set_paralyzed(tar_ptr, 3 + randint1(rlev)))
							{
								obvious = TRUE;
							}
						}
					}

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_FREE);

					break;
				}

				case RBE_LOSE_STR:
				{
					/* Damage (physical) */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(tar_ptr, A_STR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_INT:
				{
					/* Damage (physical) */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(tar_ptr, A_INT)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_WIS:
				{
					/* Damage (physical) */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(tar_ptr, A_WIS)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_DEX:
				{
					/* Damage (physical) */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(tar_ptr, A_DEX)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CON:
				{
					/* Damage (physical) */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(tar_ptr, A_CON)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CHR:
				{
					/* Damage (physical) */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(tar_ptr, A_CHR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_ALL:
				{
					/* Damage (physical) */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Damage (stats) */
					if (do_dec_stat(tar_ptr, A_STR)) obvious = TRUE;
					if (do_dec_stat(tar_ptr, A_DEX)) obvious = TRUE;
					if (do_dec_stat(tar_ptr, A_CON)) obvious = TRUE;
					if (do_dec_stat(tar_ptr, A_INT)) obvious = TRUE;
					if (do_dec_stat(tar_ptr, A_WIS)) obvious = TRUE;
					if (do_dec_stat(tar_ptr, A_CHR)) obvious = TRUE;

					break;
				}

				case RBE_SHATTER:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Reduce damage based on the player armor class */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Radius 8 earthquake centered at the monster */
					if (damage > 23 || explode)
					{
						//TODO earthquake_aux(atk_ptr->fy, atk_ptr->fx, 8, m_idx);
					}

					break;
				}

				case RBE_EXP_10:
				{
					s32b d = damroll(10, 6) + (tar_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					(void)drain_exp(tar_ptr, d, d / 10, 95);
					break;
				}

				case RBE_EXP_20:
				{
					s32b d = damroll(20, 6) + (tar_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					(void)drain_exp(tar_ptr, d, d / 10, 90);
					break;
				}

				case RBE_EXP_40:
				{
					s32b d = damroll(40, 6) + (tar_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					(void)drain_exp(tar_ptr, d, d / 10, 75);
					break;
				}

				case RBE_EXP_80:
				{
					s32b d = damroll(80, 6) + (tar_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					(void)drain_exp(tar_ptr, d, d / 10, 50);
					break;
				}

				case RBE_DISEASE:
				{
					/* Take some damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					/* Take "poison" effect */
					if (!(tar_ptr->resist_pois || IS_OPPOSE_POIS(tar_ptr)))
					{
						if (set_poisoned(tar_ptr, tar_ptr->poisoned + randint1(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					/* Damage CON (10% chance)*/
					if ((randint1(100) < 11) && (tar_ptr->race_idx1 != RACE_ANDROID))
					{
						/* 1% chance for perm. damage */
						bool perm = one_in_(10);
						if (dec_stat(tar_ptr, A_CON, randint1(10), perm))
						{
#ifdef JP
							msg_print("ïaÇ™Ç†Ç»ÇΩÇêIÇÒÇ≈Ç¢ÇÈãCÇ™Ç∑ÇÈÅB");
#else
							msg_print("You feel strange sickness.");
#endif

							obvious = TRUE;
						}
					}

					break;
				}
				case RBE_TIME:
				{
					if (explode) break;
					if (!tar_ptr->resist_time && !(tar_ptr->multishadow && (turn & 1)))
					{
						switch (randint1(10))
						{
							case 1: case 2: case 3: case 4: case 5:
							{
								if (tar_ptr->race_idx1 == RACE_ANDROID) break;
#ifdef JP
								msg_print("êlê∂Ç™ãtñﬂÇËÇµÇΩãCÇ™Ç∑ÇÈÅB");
#else
								msg_print("You feel life has clocked back.");
#endif

								lose_exp(tar_ptr, 100 + (tar_ptr->exp / 100) * MON_DRAIN_LIFE);
								break;
							}

							case 6: case 7: case 8: case 9:
							{
								int stat = randint0(6);

								switch (stat)
								{
#ifdef JP
									case A_STR: act = "ã≠Ç≠"; break;
									case A_INT: act = "ëèñæÇ≈"; break;
									case A_WIS: act = "å´ñæÇ≈"; break;
									case A_DEX: act = "äÌópÇ≈"; break;
									case A_CON: act = "åíçNÇ≈"; break;
									case A_CHR: act = "î¸ÇµÇ≠"; break;
#else
									case A_STR: act = "strong"; break;
									case A_INT: act = "bright"; break;
									case A_WIS: act = "wise"; break;
									case A_DEX: act = "agile"; break;
									case A_CON: act = "hale"; break;
									case A_CHR: act = "beautiful"; break;
#endif

								}

#ifdef JP
								msg_format("Ç†Ç»ÇΩÇÕà»ëOÇŸÇ«%sÇ»Ç≠Ç»Ç¡ÇƒÇµÇ‹Ç¡ÇΩ...ÅB", act);
#else
								msg_format("You're not as %s as you used to be...", act);
#endif


								tar_ptr->stat_cur[stat] = (tar_ptr->stat_cur[stat] * 3) / 4;
								if (tar_ptr->stat_cur[stat] < 3) tar_ptr->stat_cur[stat] = 3;
								tar_ptr->update |= (PU_BONUS);
								break;
							}

							case 10:
							{
#ifdef JP
						msg_print("Ç†Ç»ÇΩÇÕà»ëOÇŸÇ«óÕã≠Ç≠Ç»Ç≠Ç»Ç¡ÇƒÇµÇ‹Ç¡ÇΩ...ÅB");
#else
								msg_print("You're not as powerful as you used to be...");
#endif


								for (k = 0; k < 6; k++)
								{
									tar_ptr->stat_cur[k] = (tar_ptr->stat_cur[k] * 7) / 8;
									if (tar_ptr->stat_cur[k] < 3) tar_ptr->stat_cur[k] = 3;
								}
								tar_ptr->update |= (PU_BONUS);
								break;
							}
						}
					}
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					break;
				}
				case RBE_EXP_VAMP:
				{
					s32b d = damroll(60, 6) + (tar_ptr->exp / 100) * MON_DRAIN_LIFE;
					bool resist_drain;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(NULL, tar_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (tar_ptr->is_dead || (tar_ptr->multishadow && (turn & 1))) break;

					resist_drain = !drain_exp(tar_ptr, d, d / 10, 50);

					/* Heal the attacker? */
					if (tar_ptr->mimic_form)
					{
						if (mimic_info[tar_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING)
							resist_drain = TRUE;
					}
					else
					{
						switch (tar_ptr->race_idx1)
						{
						case RACE_ZOMBIE:
						case RACE_VAMPIRE:
						case RACE_LICH:
						case RACE_SKELETON:
						case RACE_DEMON:
						case RACE_BALROG:
						case RACE_GOLEM:
						case RACE_ANDROID:
							resist_drain = TRUE;
							break;
						}
					}

					if ((damage > 5) && !resist_drain)
					{
						bool did_heal = FALSE;

						if (atk_ptr->chp < atk_ptr->mhp) did_heal = TRUE;

						/* Heal */
						atk_ptr->chp += damroll(4, damage / 6);
						if (atk_ptr->chp > atk_ptr->mhp) atk_ptr->chp = atk_ptr->mhp;

						/* Redraw (later) if needed */
						//TODO if (tar_ptr->health_who == m_idx) play_redraw |= (PR_HEALTH);
						//TODO if (tar_ptr->riding == m_idx) play_redraw |= (PR_UHEALTH);

						/* Special message */
						if (atk_ptr->ml && did_heal)
						{
#ifdef JP
msg_format("%sÇÕëÃóÕÇâÒïúÇµÇΩÇÊÇ§ÇæÅB", atk_name);
#else
							msg_format("%^s appears healthier.", atk_name);
#endif

						}
					}

					break;
				}
				case RBE_DR_MANA:
				{
					/* Obvious */
					obvious = TRUE;

					if ((tar_ptr->multishadow && (turn & 1)))
					{
#ifdef JP
						msg_print("çUåÇÇÕå∂âeÇ…ñΩíÜÇµÅAÇ†Ç»ÇΩÇ…ÇÕìÕÇ©Ç»Ç©Ç¡ÇΩÅB");
#else
						msg_print("The attack hits Shadow, you are unharmed!");
#endif
					}
					else
					{
						do_cut = 0;

						/* Take damage */
						tar_ptr->csp -= damage;
						if (tar_ptr->csp < 0)
						{
							tar_ptr->csp = 0;
							tar_ptr->csp_frac = 0;
						}

						play_redraw |= (PR_MANA);
					}

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_MANA);

					break;
				}
			}

			/* Hack -- only one of cut or stun */
			if (do_cut && do_stun)
			{
				/* Cancel cut */
				if (randint0(100) < 50)
				{
					do_cut = 0;
				}

				/* Cancel stun */
				else
				{
					do_stun = 0;
				}
			}

			/* Handle cut */
			if (do_cut)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0: k = 0; break;
					case 1: k = randint1(5); break;
					case 2: k = randint1(5) + 5; break;
					case 3: k = randint1(20) + 20; break;
					case 4: k = randint1(50) + 50; break;
					case 5: k = randint1(100) + 100; break;
					case 6: k = 300; break;
					default: k = 500; break;
				}

				/* Apply the cut */
				if (k) (void)set_cut(tar_ptr, tar_ptr->cut + k);
			}

			/* Handle stun */
			if (do_stun)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0: k = 0; break;
					case 1: k = randint1(5); break;
					case 2: k = randint1(5) + 10; break;
					case 3: k = randint1(10) + 20; break;
					case 4: k = randint1(15) + 30; break;
					case 5: k = randint1(20) + 40; break;
					case 6: k = 80; break;
					default: k = 150; break;
				}

				/* Apply the stun */
				if (k) (void)set_stun(tar_ptr, tar_ptr->stun + k);
			}

			if (explode)
			{
				sound(SOUND_EXPLODE);

				take_hit(atk_ptr, atk_ptr, 0, atk_ptr->chp + 1, NULL, NULL, -1);
				if(atk_ptr->species_idx == 0)
				{
					blinked = FALSE;
					alive = FALSE;
				}
			}

			if (touched)
			{
				if (tar_ptr->sh_fire && alive && !tar_ptr->is_dead)
				{
					if (!(r_ptr->flags10 & RF10_EFF_IM_FIRE_MASK))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(atk_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇÕìÀëRîMÇ≠Ç»Ç¡ÇΩÅI", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, "ÇÕäDÇÃéRÇ…Ç»Ç¡ÇΩÅB", -1);
#else
						msg_format("%^s is suddenly very hot!", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, " turns into a pile of ash.", -1);
#endif
						if(atk_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(tar_ptr, atk_ptr))
							r_ptr->r_flags10 |= (r_ptr->flags10 & RF10_EFF_IM_FIRE_MASK);
					}
				}

				if (tar_ptr->sh_elec && alive && !tar_ptr->is_dead)
				{
					if (!(r_ptr->flags10 & RF10_EFF_IM_ELEC_MASK))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(atk_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇÕìdåÇÇÇ≠ÇÁÇ¡ÇΩÅI", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, "ÇÕîRÇ¶äkÇÃéRÇ…Ç»Ç¡ÇΩÅB", -1);
#else
						msg_format("%^s gets zapped!", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, " turns into a pile of cinder.", -1);
#endif
						if(atk_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(tar_ptr, atk_ptr))
							r_ptr->r_flags10 |= (r_ptr->flags10 & RF10_EFF_IM_ELEC_MASK);
					}
				}

				if (tar_ptr->sh_cold && alive && !tar_ptr->is_dead)
				{
					if (!(r_ptr->flags10 & RF10_EFF_IM_COLD_MASK))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(atk_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇÕó‚ãCÇÇ≠ÇÁÇ¡ÇΩÅI", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, "ÇÕìÄÇËÇ¬Ç¢ÇΩÅB", -1);
#else
						msg_format("%^s is very cold!", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, " was frozen.", -1);
#endif
						if(atk_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(tar_ptr, atk_ptr))
							r_ptr->r_flags10 |= (r_ptr->flags10 & RF10_EFF_IM_COLD_MASK);
					}
				}

				/* by henkma */
				if (tar_ptr->dustrobe && alive && !tar_ptr->is_dead)
				{
					if (!(r_ptr->flags10 & RF10_EFF_RES_SHAR_MASK))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(atk_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇÕãæÇÃîjï–ÇÇ≠ÇÁÇ¡ÇΩÅI", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, "ÇÕÉYÉ^ÉYÉ^Ç…Ç»Ç¡ÇΩÅB", -1);
#else
						msg_format("%^s gets zapped!", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, " had torn to pieces.", -1);
#endif
						if(atk_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(tar_ptr, atk_ptr))
							r_ptr->r_flags10 |= (r_ptr->flags10 & RF10_EFF_RES_SHAR_MASK);
					}

					if (is_mirror_grid(&cave[tar_ptr->fy][tar_ptr->fx]))
					{
						teleport_player(tar_ptr, 10, 0L);
					}
				}

				if (tar_ptr->tim_sh_holy && alive && !tar_ptr->is_dead)
				{
					if (is_enemy_of_good_creature(tar_ptr))
					{
						if (!(atk_ptr->resist_ultimate))
						{
							int dam = damroll(2, 6);

							/* Modify the damage */
							dam = mon_damage_mod(atk_ptr, dam, FALSE);

#ifdef JP
							msg_format("%^sÇÕêπÇ»ÇÈÉIÅ[ÉâÇ≈èùÇ¬Ç¢ÇΩÅI", atk_name);
							take_hit(tar_ptr, atk_ptr, 0, dam, NULL, "ÇÕì|ÇÍÇΩÅB", -1);
#else
							msg_format("%^s is injured by holy power!", atk_name);
							take_hit(tar_ptr, atk_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
							if(atk_ptr->species_idx == 0)
							{
								blinked = FALSE;
								alive = FALSE;
							}
							if (is_original_ap_and_seen(tar_ptr, atk_ptr));
								// TODO r_ptr->r_flags3 |= RF3_EVIL;
						}
						else
						{
							if (is_original_ap_and_seen(tar_ptr, atk_ptr))
								r_ptr->r_flags10 |= RF10_RES_ALL;
						}
					}
				}

				if (tar_ptr->tim_sh_touki && alive && !tar_ptr->is_dead)
				{
					if (!(atk_ptr->resist_ultimate))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(atk_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇ™âsÇ¢ì¨ãCÇÃÉIÅ[ÉâÇ≈èùÇ¬Ç¢ÇΩÅI", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, "ÇÕì|ÇÍÇΩÅB", -1);
#else
						msg_format("%^s is injured by the Force", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
						if(atk_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(tar_ptr, atk_ptr))
							r_ptr->r_flags10 |= RF10_RES_ALL;
					}
				}

				if (hex_spelling(tar_ptr, HEX_SHADOW_CLOAK) && alive && !tar_ptr->is_dead)
				{
					int dam = 1;
					object_type *o_ptr = &tar_ptr->inventory[INVEN_1STARM];

					if (!(atk_ptr->resist_ultimate || r_ptr->flags10 & RF10_RES_DARK))
					{
						if (o_ptr->k_idx)
						{
							int basedam = ((o_ptr->dd + tar_ptr->to_dd[0]) * (o_ptr->ds + tar_ptr->to_ds[0] + 1));
							dam = basedam / 2 + o_ptr->to_d + tar_ptr->to_d[0];
						}

						/* Cursed armor makes damages doubled */
						o_ptr = &tar_ptr->inventory[INVEN_BODY];
						if ((o_ptr->k_idx) && object_is_cursed(o_ptr)) dam *= 2;

						/* Modify the damage */
						dam = mon_damage_mod(atk_ptr, dam, FALSE);

#ifdef JP
						msg_format("âeÇÃÉIÅ[ÉâÇ™%^sÇ…îΩåÇÇµÇΩÅI", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, "ÇÕì|ÇÍÇΩÅB", -1);
#else
						msg_format("Enveloped shadows attack %^s.", atk_name);
						take_hit(tar_ptr, atk_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
						if(atk_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
						else /* monster does not dead */
						{
							int j;
							int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
							int typ[4][2] = {
								{ INVEN_1STHEAD, GF_OLD_CONF },
								{ INVEN_2NDARM,  GF_OLD_SLEEP },
								{ INVEN_1STHANDS, GF_TURN_ALL },
								{ INVEN_FEET, GF_OLD_SLOW }
							};

							/* Some cursed armours gives an extra effect */
							for (j = 0; j < 4; j++)
							{
								o_ptr = &tar_ptr->inventory[typ[j][0]];
								if ((o_ptr->k_idx) && object_is_cursed(o_ptr) && object_is_armour(tar_ptr, o_ptr))
									project(atk_ptr, 0, atk_ptr->fy, atk_ptr->fx, (tar_ptr->lev * 2), typ[j][1], flg, -1);
							}
						}
					}
					else
					{
						if (is_original_ap_and_seen(tar_ptr, atk_ptr))
							r_ptr->r_flags10 |= (RF10_RES_ALL | RF10_RES_DARK);
					}
				}
			}
		}

		/* Monster missed player */
		else
		{
			/* Analyze failed attacks */
			switch (method)
			{
				case RBM_HIT:
				case RBM_TOUCH:
				case RBM_PUNCH:
				case RBM_KICK:
				case RBM_CLAW:
				case RBM_BITE:
				case RBM_STING:
				case RBM_SLASH:
				case RBM_BUTT:
				case RBM_CRUSH:
				case RBM_ENGULF:
				case RBM_CHARGE:

				/* Visible monsters */
				if (atk_ptr->ml)
				{
					/* Disturbing */
					disturb(1, 0);

					/* Message */
#ifdef JP
					if (abbreviate)
					    msg_format("%sÇ©ÇÌÇµÇΩÅB", (tar_ptr->special_attack & ATTACK_SUIKEN) ? "äÔñ≠Ç»ìÆÇ´Ç≈" : "");
					else
					    msg_format("%s%^sÇÃçUåÇÇÇ©ÇÌÇµÇΩÅB", (tar_ptr->special_attack & ATTACK_SUIKEN) ? "äÔñ≠Ç»ìÆÇ´Ç≈" : "", atk_name);
					abbreviate = 1;/*ÇQâÒñ⁄à»ç~ÇÕè»ó™ */
#else
					msg_format("%^s misses you.", atk_name);
#endif

				}
				damage = 0;

				break;
			}
		}


		/* Analyze "visible" monsters only */
		if (is_original_ap_and_seen(tar_ptr, atk_ptr) && !do_silly_attack)
		{
			/* Count "obvious" attacks (and ones that cause damage) */
			if (obvious || damage || (r_ptr->r_blows[ap_cnt] > 10))
			{
				/* Count attacks of this type */
				if (r_ptr->r_blows[ap_cnt] < MAX_UCHAR)
				{
					r_ptr->r_blows[ap_cnt]++;
				}
			}
		}

		if (tar_ptr->riding && damage)
		{
			char atk_name[80];
			monster_desc(atk_name, &creature_list[tar_ptr->riding], 0);
			if (rakuba(tar_ptr, (damage > 200) ? 200 : damage, FALSE))
			{
#ifdef JP
msg_format("%^sÇ©ÇÁóéÇøÇƒÇµÇ‹Ç¡ÇΩÅI", atk_name);
#else
				msg_format("You have fallen from %s.", atk_name);
#endif
			}
		}

		if (tar_ptr->special_defense & NINJA_KAWARIMI)
		{
			if (kawarimi(FALSE)) return TRUE;
		}
	}

	/* Hex - revenge damage stored */
	revenge_store(tar_ptr, get_damage);

	if ((tar_ptr->tim_eyeeye || hex_spelling(tar_ptr, HEX_EYE_FOR_EYE))
		&& get_damage > 0 && !tar_ptr->is_dead)
	{
#ifdef JP
		msg_format("çUåÇÇ™%sé©êgÇèùÇ¬ÇØÇΩÅI", atk_name);
#else
		char atk_name_self[80];

		/* hisself */
		monster_desc(atk_name_self, atk_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE);

		msg_format("The attack of %s has wounded %s!", atk_name, atk_name_self);
#endif
		project(atk_ptr, 0, atk_ptr->fy, atk_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
		if (tar_ptr->tim_eyeeye) set_tim_eyeeye(tar_ptr, tar_ptr->tim_eyeeye-5, TRUE);
	}

	if ((tar_ptr->counter || (tar_ptr->special_defense & KATA_MUSOU)) && alive && !tar_ptr->is_dead && atk_ptr->ml && (tar_ptr->csp > 7))
	{
		char atk_name[80];
		monster_desc(atk_name, atk_ptr, 0);

		tar_ptr->csp -= 7;
#ifdef JP
		msg_format("%^sÇ…îΩåÇÇµÇΩÅI", atk_name);
#else
		msg_format("Your counterattack to %s!", atk_name);
#endif
		creature_attack(tar_ptr, atk_ptr->fy, atk_ptr->fx, HISSATSU_COUNTER);
		fear = FALSE;

		/* Redraw mana */
		play_redraw |= (PR_MANA);
	}

	/* Blink away */
	if (blinked && alive && !tar_ptr->is_dead)
	{
		if (teleport_barrier(tar_ptr, atk_ptr))
		{
#ifdef JP
			msg_print("ìDñ_ÇÕèŒÇ¡Çƒì¶Ç∞...ÇÊÇ§Ç∆ÇµÇΩÇ™ÉoÉäÉAÇ…ñhÇ™ÇÍÇΩÅB");
#else
			msg_print("The thief flees laughing...? But magic barrier obstructs it.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("ìDñ_ÇÕèŒÇ¡Çƒì¶Ç∞ÇΩÅI");
#else
			msg_print("The thief flees laughing!");
#endif
			//TODO teleport_away(m_idx, MAX_SIGHT * 2 + 5, 0L);
		}
	}


	/* Always notice cause of death */
	if (tar_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !inside_arena)
	{
		r_ptr->r_deaths++;
	}

	if (tar_ptr->special_defense & KATA_IAI)
	{
		set_action(tar_ptr, ACTION_NONE);
	}

	/* Assume we attacked */
	return (TRUE);
}
