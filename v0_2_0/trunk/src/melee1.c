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
static int check_hit(creature_type *target_ptr, int power, int level, int stun)
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
	ac = target_ptr->ac + target_ptr->to_a;
	if (target_ptr->special_attack & ATTACK_SUIKEN) ac += (target_ptr->lev * 2);

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
bool special_melee(creature_type *attacker_ptr, creature_type *target_ptr)
{

	species_type *r_ptr = &species_info[attacker_ptr->species_idx];

	int ap_cnt;

	int i, k, tmp, ac, rlev;
	int do_cut, do_stun;

	s32b gold;

	object_type *o_ptr;

	char o_name[MAX_NLEN];

	char attacker_name[100];
	char target_name[100];

	char ddesc[80];

	bool blinked;
	bool touched = FALSE, fear = FALSE, alive = TRUE;
	bool explode = FALSE;
	bool do_silly_attack = (one_in_(2) && target_ptr->image);
	int get_damage = 0;
#ifdef JP
	int abbreviate = 0;
#endif

	/* Not allowed to attack */
	if (has_cf_creature(attacker_ptr, CF_NEVER_BLOW)) return (FALSE);

	if (d_info[dungeon_type].flags1 & DF1_NO_MELEE) return (FALSE);

	/* ...nor if friendly */
	if (!is_hostile(attacker_ptr)) return FALSE;

	/* Extract the effective monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);


	/* Get the monster name (or "it") */
	creature_desc(attacker_name, attacker_ptr, 0);
	creature_desc(target_name, target_ptr, 0);

	/* Get the "died from" information (i.e. "a kobold") */
	creature_desc(ddesc, attacker_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

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

		/* Hack -- no more attacks */
		if (!method) break;

		if (is_pet(player_ptr, attacker_ptr) && (is_unique_species(r_ptr)) && (method == RBM_EXPLODE))
		{
			method = RBM_HIT;
			d_dice /= 10;
		}

		/* Stop if player is dead or gone */
		if (!target_ptr->playing || target_ptr->is_dead) break;
		if (distance(target_ptr->fy, target_ptr->fx, attacker_ptr->fy, attacker_ptr->fx) > 1) break;

		/* Handle "leaving" */
		if (target_ptr->leaving) break;

		if (method == RBM_SHOOT) continue;

		/* Extract the attack "power" */
		power = mbe_info[effect].power;

		/* Total armor */
		ac = target_ptr->ac + target_ptr->to_a;

		/* Monster hits player */
		if (!effect || check_hit(target_ptr, power, rlev, attacker_ptr->stun))
		{
			/* Always disturbing */
			disturb(player_ptr, 1, 0);


			/* Hack -- Apply "protection from evil" */
			if ((target_ptr->protevil > 0) &&
			    is_enemy_of_good_creature(target_ptr) &&
			    (target_ptr->lev >= rlev) &&
			    ((randint0(100) + target_ptr->lev) > 50))
			{
				/* Remember the Evil-ness */
				//TODO if (is_original_ap_and_seen(target_ptr, attacker_ptr)) r_ptr->r_flags3 |= RF3_EVIL;

				/* Message */
#ifdef JP
				if (abbreviate)
				    msg_format("åÇëﬁÇµÇΩÅB");
				else
				    msg_format("%^sÇÕåÇëﬁÇ≥ÇÍÇΩÅB", attacker_name);
				abbreviate = 1;/*ÇQâÒñ⁄à»ç~ÇÕè»ó™ */
#else
				msg_format("%^s is repelled.", attacker_name);
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					if(is_player(attacker_ptr))
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
					act = desc_insult[randint0(attacker_ptr->species_idx == MON_DEBBY ? 10 : 8)];
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
					if (attacker_ptr->species_idx == MON_JAIAN)
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
				    msg_format("%^sÇÕ%sÇ%s", attacker_name, target_name, act);
				else if (abbreviate == 1)
				    msg_format("%^sÇÕ%sÇ…%s", attacker_name, target_name, act);
				else if (abbreviate == 2)
				    msg_format("%^sÇÕ%sÇÃ%s", attacker_name, target_name, act);
				else if (abbreviate == 3)
				    msg_format("%^sÇÕ%sÇ…å¸ÇØ%s", attacker_name, target_name, act);
				else if (abbreviate == 4)
				    msg_format("%^s%s", attacker_name, act);
				else /* if (abbreviate == -1) */
				    msg_format("%s", act);
				abbreviate = -1;/*ÇQâÒñ⁄à»ç~ÇÕè»ó™ */
#else
				msg_format("%^s %s %s", attacker_name, act, do_silly_attack ? target_name : "");
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
					if (((randint1(rlev*2+300) > (ac+200)) || one_in_(13)) && !(target_ptr->multishadow && (turn & 1)))
					{
						int tmp_damage = damage - (damage * ((ac < 150) ? ac : 150) / 250);
#ifdef JP
						msg_print("ÉNÉäÉeÉBÉJÉãÉqÉbÉgÅI");
#else
						msg_print("It was a critical hit!");
#endif

						tmp_damage = MAX(damage, tmp_damage*2);

						/* Take damage */
						get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, tmp_damage, ddesc, NULL, -1);
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
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					break;
				}

				case RBE_POISON:
				{
					if (explode) break;

					/* Take "poison" effect */
					if (!(target_ptr->resist_pois || IS_OPPOSE_POIS(target_ptr)) && !(target_ptr->multishadow && (turn & 1)))
					{
						if (set_poisoned(target_ptr, target_ptr->poisoned + randint1(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					/* Take some damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_POIS);

					break;
				}

				case RBE_UN_BONUS:
				{
					if (explode) break;

					/* Allow complete resist */
					if (!target_ptr->resist_disen && !(target_ptr->multishadow && (turn & 1)))
					{
						/* Apply disenchantment */
						if (apply_disenchant(target_ptr, 0))
						{
							/* Hack -- Update AC */
							update_stuff(target_ptr, TRUE);
							obvious = TRUE;
						}
					}

					/* Take some damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_DISEN);

					break;
				}

				case RBE_UN_POWER:
				{
					/* Take some damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item */
						i = randint0(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &target_ptr->inventory[i];

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
							heal = MIN(heal, attacker_ptr->mhp - attacker_ptr->chp);

							/* Message */
#ifdef JP
							msg_print("ñÇìπãÔÇ©ÇÁÉGÉlÉãÉMÅ[ÇãzÇ¢éÊÇ¡ÇΩÅI");
#else
							msg_print("Energy drains from your magic device!");
#endif


							/* Obvious */
							obvious = TRUE;

							/* Heal the monster */
							attacker_ptr->chp += heal;

							/* Redraw (later) if needed */
							//TODO if (&m_info[health_who] == attacker_ptr) play_redraw |= (PR_HEALTH);
							//if (&m_info[target_ptr->riding] == attacker_ptr) play_redraw |= (PR_UHEALTH);

							/* Uncharge */
							o_ptr->pval = 0;

							/* Combine / Reorder the pack */
							target_ptr->notice |= (PN_COMBINE | PN_REORDER);

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
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Confused monsters cannot steal successfully. -LM-*/
					if (attacker_ptr->confused) break;

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Obvious */
					obvious = TRUE;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!target_ptr->paralyzed &&
					    (randint0(100) < (adj_dex_safe[target_ptr->stat_ind[STAT_DEX]] +
							      target_ptr->lev)))
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
						gold = (target_ptr->au / 10) + randint1(25);
						if (gold < 2) gold = 2;
						if (gold > 5000) gold = (target_ptr->au / 20) + randint1(3000);
						if (gold > target_ptr->au) gold = target_ptr->au;
						target_ptr->au -= gold;
						if (gold <= 0)
						{
#ifdef JP
							msg_print("ÇµÇ©ÇµâΩÇ‡ìêÇ‹ÇÍÇ»Ç©Ç¡ÇΩÅB");
#else
							msg_print("Nothing was stolen.");
#endif

						}
						else if (target_ptr->au)
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
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Confused monsters cannot steal successfully. -LM-*/
					if (attacker_ptr->confused) break;

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!target_ptr->paralyzed &&
					    (randint0(100) < (adj_dex_safe[target_ptr->stat_ind[STAT_DEX]] +
							      target_ptr->lev)))
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
						o_ptr = &target_ptr->inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip artifacts */
						if (object_is_artifact(o_ptr)) continue;

						/* Get a description */
						object_desc(target_ptr, o_name, o_ptr, OD_OMIT_PREFIX);

						/* Message */
#ifdef JP
						msg_format("%s(%c)Ç%sìêÇ‹ÇÍÇΩÅI",
							   o_name, index_to_label(target_ptr, i),
							   ((o_ptr->number > 1) ? "àÍÇ¬" : ""));
#else
						msg_format("%sour %s (%c) was stolen!",
							   ((o_ptr->number > 1) ? "One of y" : "Y"),
							   o_name, index_to_label(target_ptr, i));
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
							j_ptr->next_o_idx = attacker_ptr->hold_o_idx;

							/* Build stack */
							attacker_ptr->hold_o_idx = o_idx;
						}

						/* Steal the items */
						inven_item_increase(target_ptr, i, -1);
						inven_item_optimize(target_ptr, i);

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
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Steal some food */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item from the pack */
						i = randint0(INVEN_PACK);

						/* Get the item */
						o_ptr = &target_ptr->inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip non-food objects */
						if ((o_ptr->tval != TV_FOOD) && !((o_ptr->tval == TV_CORPSE) && (o_ptr->sval))) continue;

						/* Get a description */
						object_desc(target_ptr, o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

						/* Message */
#ifdef JP
						msg_format("%s(%c)Ç%sêHÇ◊ÇÁÇÍÇƒÇµÇ‹Ç¡ÇΩÅI",
							  o_name, index_to_label(target_ptr, i),
							  ((o_ptr->number > 1) ? "àÍÇ¬" : ""));
#else
						msg_format("%sour %s (%c) was eaten!",
							   ((o_ptr->number > 1) ? "One of y" : "Y"),
							   o_name, index_to_label(target_ptr, i));
#endif


						/* Steal the items */
						inven_item_increase(target_ptr, i, -1);
						inven_item_optimize(target_ptr, i);

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
					o_ptr = &target_ptr->inventory[INVEN_LITE];

					/* Take some damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Drain fuel */
					if ((o_ptr->xtra4 > 0) && (!object_is_fixed_artifact(o_ptr)))
					{
						/* Reduce fuel */
						o_ptr->xtra4 -= (250 + (s16b)randint1(250));
						if (o_ptr->xtra4 < 1) o_ptr->xtra4 = 1;

						/* Notice */
						if (!target_ptr->blind)
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
					get_damage += acid_dam(target_ptr, damage, ddesc, -1);

					/* Hack -- Update AC */
					update_stuff(target_ptr, TRUE);

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
					get_damage += elec_dam(target_ptr, damage, ddesc, -1);

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
					get_damage += fire_dam(target_ptr, damage, ddesc, -1);

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
					get_damage += cold_dam(target_ptr, damage, ddesc, -1);

					/* Learn about the player */
					//TODO update_smart_learn(m_idx, DRS_COLD);

					break;
				}

				case RBE_BLIND:
				{
					/* Take damage */
					get_damage += take_hit(NULL, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead) break;

					/* Increase "blind" */
					if (!target_ptr->resist_blind && !(target_ptr->multishadow && (turn & 1)))
					{
						if (set_blind(target_ptr, target_ptr->blind + 10 + randint1(rlev)))
						{
#ifdef JP
							if (attacker_ptr->species_idx == MON_DIO) msg_print("ÅuÇ«Ç§ÇæÉbÅIÇ±ÇÃååÇÃñ⁄í◊ÇµÇÕÉbÅIÅv");
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
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead) break;

					/* Increase "confused" */
					if (!target_ptr->resist_conf && !(target_ptr->multishadow && (turn & 1)))
					{
						if (set_confused(target_ptr, target_ptr->confused + 3 + randint1(rlev)))
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
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead) break;

					/* Increase "afraid" */
					if ((target_ptr->multishadow && (turn & 1)))
					{
						/* Do nothing */
					}
					else if (target_ptr->resist_fear)
					{
#ifdef JP
						msg_print("ÇµÇ©Çµã∞ï|Ç…êNÇ≥ÇÍÇ»Ç©Ç¡ÇΩÅI");
#else
						msg_print("You stand your ground!");
#endif

						obvious = TRUE;
					}
					else if (randint0(100 + r_ptr->level/2) < target_ptr->skill_rob)
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
						if (set_afraid(target_ptr, target_ptr->afraid + 3 + randint1(rlev)))
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
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead) break;

					/* Increase "paralyzed" */
					if ((target_ptr->multishadow && (turn & 1)))
					{
						/* Do nothing */
					}
					else if (target_ptr->free_act)
					{
#ifdef JP
						msg_print("ÇµÇ©Çµå¯â Ç™Ç»Ç©Ç¡ÇΩÅI");
#else
						msg_print("You are unaffected!");
#endif

						obvious = TRUE;
					}
					else if (randint0(100 + r_ptr->level/2) < target_ptr->skill_rob)
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
						if (!target_ptr->paralyzed)
						{
							if (set_paralyzed(target_ptr, 3 + randint1(rlev)))
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
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(target_ptr, STAT_STR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_INT:
				{
					/* Damage (physical) */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(target_ptr, STAT_INT)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_WIS:
				{
					/* Damage (physical) */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(target_ptr, STAT_WIS)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_DEX:
				{
					/* Damage (physical) */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(target_ptr, STAT_DEX)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CON:
				{
					/* Damage (physical) */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(target_ptr, STAT_CON)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CHR:
				{
					/* Damage (physical) */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Damage (stat) */
					if (do_dec_stat(target_ptr, STAT_CHR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_ALL:
				{
					/* Damage (physical) */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Damage (stats) */
					if (do_dec_stat(target_ptr, STAT_STR)) obvious = TRUE;
					if (do_dec_stat(target_ptr, STAT_DEX)) obvious = TRUE;
					if (do_dec_stat(target_ptr, STAT_CON)) obvious = TRUE;
					if (do_dec_stat(target_ptr, STAT_INT)) obvious = TRUE;
					if (do_dec_stat(target_ptr, STAT_WIS)) obvious = TRUE;
					if (do_dec_stat(target_ptr, STAT_CHR)) obvious = TRUE;

					break;
				}

				case RBE_SHATTER:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Reduce damage based on the player armor class */
					damage -= (damage * ((ac < 150) ? ac : 150) / 250);

					/* Take damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					/* Radius 8 earthquake centered at the monster */
					if (damage > 23 || explode)
					{
						//TODO earthquake_aux(attacker_ptr->fy, attacker_ptr->fx, 8, m_idx);
					}

					break;
				}

				case RBE_EXP_10:
				{
					s32b d = damroll(10, 6) + (target_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					(void)drain_exp(target_ptr, d, d / 10, 95);
					break;
				}

				case RBE_EXP_20:
				{
					s32b d = damroll(20, 6) + (target_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					(void)drain_exp(target_ptr, d, d / 10, 90);
					break;
				}

				case RBE_EXP_40:
				{
					s32b d = damroll(40, 6) + (target_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					(void)drain_exp(target_ptr, d, d / 10, 75);
					break;
				}

				case RBE_EXP_80:
				{
					s32b d = damroll(80, 6) + (target_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					(void)drain_exp(target_ptr, d, d / 10, 50);
					break;
				}

				case RBE_DISEASE:
				{
					/* Take some damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					/* Take "poison" effect */
					if (!(target_ptr->resist_pois || IS_OPPOSE_POIS(target_ptr)))
					{
						if (set_poisoned(target_ptr, target_ptr->poisoned + randint1(rlev) + 5))
						{
							obvious = TRUE;
						}
					}

					/* Damage CON (10% chance)*/
					if ((randint1(100) < 11) && !has_cf_creature(target_ptr, CF_ANDROID))
					{
						/* 1% chance for perm. damage */
						bool perm = one_in_(10);
						if (dec_stat(target_ptr, STAT_CON, randint1(10), perm))
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
					if (!target_ptr->resist_time && !(target_ptr->multishadow && (turn & 1)))
					{
						switch (randint1(10))
						{
							case 1: case 2: case 3: case 4: case 5:
							{
								if (has_cf_creature(target_ptr, CF_ANDROID)) break;
#ifdef JP
								msg_print("êlê∂Ç™ãtñﬂÇËÇµÇΩãCÇ™Ç∑ÇÈÅB");
#else
								msg_print("You feel life has clocked back.");
#endif

								lose_exp(target_ptr, 100 + (target_ptr->exp / 100) * MON_DRAIN_LIFE);
								break;
							}

							case 6: case 7: case 8: case 9:
							{
								int stat = randint0(6);

								switch (stat)
								{
#ifdef JP
									case STAT_STR: act = "ã≠Ç≠"; break;
									case STAT_INT: act = "ëèñæÇ≈"; break;
									case STAT_WIS: act = "å´ñæÇ≈"; break;
									case STAT_DEX: act = "äÌópÇ≈"; break;
									case STAT_CON: act = "åíçNÇ≈"; break;
									case STAT_CHR: act = "î¸ÇµÇ≠"; break;
#else
									case STAT_STR: act = "strong"; break;
									case STAT_INT: act = "bright"; break;
									case STAT_WIS: act = "wise"; break;
									case STAT_DEX: act = "agile"; break;
									case STAT_CON: act = "hale"; break;
									case STAT_CHR: act = "beautiful"; break;
#endif

								}

#ifdef JP
								msg_format("Ç†Ç»ÇΩÇÕà»ëOÇŸÇ«%sÇ»Ç≠Ç»Ç¡ÇƒÇµÇ‹Ç¡ÇΩ...ÅB", act);
#else
								msg_format("You're not as %s as you used to be...", act);
#endif


								target_ptr->stat_cur[stat] = (target_ptr->stat_cur[stat] * 3) / 4;
								if (target_ptr->stat_cur[stat] < 3) target_ptr->stat_cur[stat] = 3;
								target_ptr->update |= (PU_BONUS);
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
									target_ptr->stat_cur[k] = (target_ptr->stat_cur[k] * 7) / 8;
									if (target_ptr->stat_cur[k] < 3) target_ptr->stat_cur[k] = 3;
								}
								target_ptr->update |= (PU_BONUS);
								break;
							}
						}
					}
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					break;
				}
				case RBE_EXP_VAMP:
				{
					s32b d = damroll(60, 6) + (target_ptr->exp / 100) * MON_DRAIN_LIFE;
					bool resist_drain;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					get_damage += take_hit(attacker_ptr, target_ptr, DAMAGE_ATTACK, damage, ddesc, NULL, -1);

					if (target_ptr->is_dead || (target_ptr->multishadow && (turn & 1))) break;

					resist_drain = !drain_exp(target_ptr, d, d / 10, 50);

					/* Heal the attacker? */
					if (target_ptr->mimic_form)
					{
						if (mimic_info[target_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_NONLIVING)
							resist_drain = TRUE;
					}
					else
					{
						if(has_cf_creature(target_ptr, CF_NONLIVING) || is_undead_creature(target_ptr) || is_demon_creature(target_ptr))
						{
							resist_drain = TRUE;
							break;
						}
					}

					if ((damage > 5) && !resist_drain)
					{
						bool did_heal = FALSE;

						if (attacker_ptr->chp < attacker_ptr->mhp) did_heal = TRUE;

						/* Heal */
						attacker_ptr->chp += damroll(4, damage / 6);
						if (attacker_ptr->chp > attacker_ptr->mhp) attacker_ptr->chp = attacker_ptr->mhp;

						/* Redraw (later) if needed */
						//TODO if (health_who == m_idx) play_redraw |= (PR_HEALTH);
						//TODO if (target_ptr->riding == m_idx) play_redraw |= (PR_UHEALTH);

						/* Special message */
						if (attacker_ptr->ml && did_heal)
						{
#ifdef JP
msg_format("%sÇÕëÃóÕÇâÒïúÇµÇΩÇÊÇ§ÇæÅB", attacker_name);
#else
							msg_format("%^s appears healthier.", attacker_name);
#endif

						}
					}

					break;
				}
				case RBE_DR_MANA:
				{
					/* Obvious */
					obvious = TRUE;

					if ((target_ptr->multishadow && (turn & 1)))
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
						target_ptr->csp -= damage;
						if (target_ptr->csp < 0)
						{
							target_ptr->csp = 0;
							target_ptr->csp_frac = 0;
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
				if (k) (void)set_cut(target_ptr, target_ptr->cut + k);
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
				if (k) (void)set_stun(target_ptr, target_ptr->stun + k);
			}

			if (explode)
			{
				sound(SOUND_EXPLODE);

				take_hit(attacker_ptr, attacker_ptr, 0, attacker_ptr->chp + 1, NULL, NULL, -1);
				if(attacker_ptr->species_idx == 0)
				{
					blinked = FALSE;
					alive = FALSE;
				}
			}

			if (touched)
			{
				if (target_ptr->sh_fire && alive && !target_ptr->is_dead)
				{
					if (!has_cf_creature(attacker_ptr, CF_RES_SHAR))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(attacker_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇÕìÀëRîMÇ≠Ç»Ç¡ÇΩÅI", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, "ÇÕäDÇÃéRÇ…Ç»Ç¡ÇΩÅB", -1);
#else
						msg_format("%^s is suddenly very hot!", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, " turns into a pile of ash.", -1);
#endif
						if(attacker_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						//if (is_original_ap_and_seen(target_ptr, attacker_ptr))
						//TODO 	r_ptr->r_flags10 |= (r_ptr->flags10 & RF10_EFF_IM_FIRE_MASK);
					}
				}

				if (target_ptr->sh_elec && alive && !target_ptr->is_dead)
				{
					if (!has_cf_creature(attacker_ptr, CF_RES_ELEC))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(attacker_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇÕìdåÇÇÇ≠ÇÁÇ¡ÇΩÅI", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, "ÇÕîRÇ¶äkÇÃéRÇ…Ç»Ç¡ÇΩÅB", -1);
#else
						msg_format("%^s gets zapped!", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, " turns into a pile of cinder.", -1);
#endif
						if(attacker_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						//TODO if (is_original_ap_and_seen(target_ptr, attacker_ptr))
							//TODO r_ptr->r_flags10 |= (r_ptr->flags10 & RF10_EFF_IM_ELEC_MASK);
					}
				}

				if (target_ptr->sh_cold && alive && !target_ptr->is_dead)
				{
					if (!has_cf_creature(attacker_ptr, CF_RES_COLD))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(attacker_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇÕó‚ãCÇÇ≠ÇÁÇ¡ÇΩÅI", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, "ÇÕìÄÇËÇ¬Ç¢ÇΩÅB", -1);
#else
						msg_format("%^s is very cold!", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, " was frozen.", -1);
#endif
						if(attacker_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						//TODO if (is_original_ap_and_seen(target_ptr, attacker_ptr))
							//TODO r_ptr->r_flags10 |= (r_ptr->flags10 & RF10_EFF_IM_COLD_MASK);
					}
				}

				/* by henkma */
				if (target_ptr->dustrobe && alive && !target_ptr->is_dead)
				{
					if (!has_cf_creature(attacker_ptr, CF_RES_SHAR))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(attacker_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇÕãæÇÃîjï–ÇÇ≠ÇÁÇ¡ÇΩÅI", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, "ÇÕÉYÉ^ÉYÉ^Ç…Ç»Ç¡ÇΩÅB", -1);
#else
						msg_format("%^s gets zapped!", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, " had torn to pieces.", -1);
#endif
						if(attacker_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						//if (is_original_ap_and_seen(target_ptr, attacker_ptr))
							//TODO r_ptr->r_flags10 |= (r_ptr->flags10 & RF10_EFF_RES_SHAR_MASK);
					}

					if (is_mirror_grid(&cave[target_ptr->fy][target_ptr->fx]))
					{
						teleport_player(target_ptr, 10, 0L);
					}
				}

				if (target_ptr->tim_sh_holy && alive && !target_ptr->is_dead)
				{
					if (is_enemy_of_good_creature(target_ptr))
					{
						if (!(attacker_ptr->resist_ultimate))
						{
							int dam = damroll(2, 6);

							/* Modify the damage */
							dam = mon_damage_mod(attacker_ptr, dam, FALSE);

#ifdef JP
							msg_format("%^sÇÕêπÇ»ÇÈÉIÅ[ÉâÇ≈èùÇ¬Ç¢ÇΩÅI", attacker_name);
							take_hit(target_ptr, attacker_ptr, 0, dam, NULL, "ÇÕì|ÇÍÇΩÅB", -1);
#else
							msg_format("%^s is injured by holy power!", attacker_name);
							take_hit(target_ptr, attacker_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
							if(attacker_ptr->species_idx == 0)
							{
								blinked = FALSE;
								alive = FALSE;
							}
							if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
						}
						else
						{
							if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
						}
					}
				}

				if (target_ptr->tim_sh_touki && alive && !target_ptr->is_dead)
				{
					if (!(attacker_ptr->resist_ultimate))
					{
						int dam = damroll(2, 6);

						/* Modify the damage */
						dam = mon_damage_mod(attacker_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^sÇ™âsÇ¢ì¨ãCÇÃÉIÅ[ÉâÇ≈èùÇ¬Ç¢ÇΩÅI", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, "ÇÕì|ÇÍÇΩÅB", -1);
#else
						msg_format("%^s is injured by the Force", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
						if(attacker_ptr->species_idx == 0)
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, CF_RES_ALL);
					}
				}

				if (hex_spelling(target_ptr, HEX_SHADOW_CLOAK) && alive && !target_ptr->is_dead)
				{
					int dam = 1;
					object_type *o_ptr = &target_ptr->inventory[INVEN_1STARM];

					if (!has_cf_creature(attacker_ptr, CF_RES_DARK))
					{
						if (o_ptr->k_idx)
						{
							int basedam = ((o_ptr->dd + target_ptr->to_dd[0]) * (o_ptr->ds + target_ptr->to_ds[0] + 1));
							dam = basedam / 2 + o_ptr->to_d + target_ptr->to_d[0];
						}

						/* Cursed armor makes damages doubled */
						o_ptr = &target_ptr->inventory[INVEN_BODY];
						if ((o_ptr->k_idx) && object_is_cursed(o_ptr)) dam *= 2;

						/* Modify the damage */
						dam = mon_damage_mod(attacker_ptr, dam, FALSE);

#ifdef JP
						msg_format("âeÇÃÉIÅ[ÉâÇ™%^sÇ…îΩåÇÇµÇΩÅI", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, "ÇÕì|ÇÍÇΩÅB", -1);
#else
						msg_format("Enveloped shadows attack %^s.", attacker_name);
						take_hit(target_ptr, attacker_ptr, 0, dam, NULL, " is destroyed.", -1);
#endif
						if(attacker_ptr->species_idx == 0)
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
								o_ptr = &target_ptr->inventory[typ[j][0]];
								if ((o_ptr->k_idx) && object_is_cursed(o_ptr) && object_is_armour(o_ptr))
									project(attacker_ptr, 0, attacker_ptr->fy, attacker_ptr->fx, (target_ptr->lev * 2), typ[j][1], flg, -1);
							}
						}
					}
					else
					{
						if(is_original_ap_and_seen(player_ptr, target_ptr))
						{
							reveal_creature_info(target_ptr, CF_RES_ALL);
							reveal_creature_info(target_ptr, CF_RES_DARK);
						}
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
				if (attacker_ptr->ml)
				{
					/* Disturbing */
					disturb(player_ptr, 1, 0);

					/* Message */
#ifdef JP
					if (abbreviate)
					    msg_format("%sÇ©ÇÌÇµÇΩÅB", (target_ptr->special_attack & ATTACK_SUIKEN) ? "äÔñ≠Ç»ìÆÇ´Ç≈" : "");
					else
					    msg_format("%s%^sÇÃçUåÇÇÇ©ÇÌÇµÇΩÅB", (target_ptr->special_attack & ATTACK_SUIKEN) ? "äÔñ≠Ç»ìÆÇ´Ç≈" : "", attacker_name);
					abbreviate = 1;/*ÇQâÒñ⁄à»ç~ÇÕè»ó™ */
#else
					msg_format("%^s misses you.", attacker_name);
#endif

				}
				damage = 0;

				break;
			}
		}


		/* Analyze "visible" monsters only */
		if (is_original_ap_and_seen(target_ptr, attacker_ptr) && !do_silly_attack)
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

		if (target_ptr->riding && damage)
		{
			char attacker_name[80];
			creature_desc(attacker_name, &creature_list[target_ptr->riding], 0);
			if (rakuba(target_ptr, (damage > 200) ? 200 : damage, FALSE))
			{
#ifdef JP
msg_format("%^sÇ©ÇÁóéÇøÇƒÇµÇ‹Ç¡ÇΩÅI", attacker_name);
#else
				msg_format("You have fallen from %s.", attacker_name);
#endif
			}
		}

		if (target_ptr->special_defense & NINJA_KAWARIMI)
		{
			if (kawarimi(target_ptr, FALSE)) return TRUE;
		}
	}

	/* Hex - revenge damage stored */
	revenge_store(target_ptr, get_damage);

	if ((target_ptr->tim_eyeeye || hex_spelling(target_ptr, HEX_EYE_FOR_EYE))
		&& get_damage > 0 && !target_ptr->is_dead)
	{
#ifdef JP
		msg_format("çUåÇÇ™%sé©êgÇèùÇ¬ÇØÇΩÅI", attacker_name);
#else
		char attacker_name_self[80];

		/* hisself */
		creature_desc(attacker_name_self, attacker_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE);

		msg_format("The attack of %s has wounded %s!", attacker_name, attacker_name_self);
#endif
		project(attacker_ptr, 0, attacker_ptr->fy, attacker_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
		if (target_ptr->tim_eyeeye) set_tim_eyeeye(target_ptr, target_ptr->tim_eyeeye-5, TRUE);
	}

	if ((target_ptr->counter || (target_ptr->special_defense & KATA_MUSOU)) && alive && !target_ptr->is_dead && attacker_ptr->ml && (target_ptr->csp > 7))
	{
		char attacker_name[80];
		creature_desc(attacker_name, attacker_ptr, 0);

		target_ptr->csp -= 7;
#ifdef JP
		msg_format("%^sÇ…îΩåÇÇµÇΩÅI", attacker_name);
#else
		msg_format("Your counterattack to %s!", attacker_name);
#endif
		weapon_attack(target_ptr, attacker_ptr->fy, attacker_ptr->fx, HISSATSU_COUNTER);
		fear = FALSE;

		/* Redraw mana */
		play_redraw |= (PR_MANA);
	}

	/* Blink away */
	if (blinked && alive && !target_ptr->is_dead)
	{
		if (teleport_barrier(target_ptr, attacker_ptr))
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
	if (target_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !inside_arena)
	{
		r_ptr->r_deaths++;
	}

	if (target_ptr->special_defense & KATA_IAI)
	{
		set_action(target_ptr, ACTION_NONE);
	}

	/* Assume we attacked */
	return (TRUE);
}
