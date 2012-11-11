/* File: object1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Object code, part 1 */

#include "angband.h"

#if defined(MACINTOSH) || defined(MACH_O_CARBON)
#ifdef verify
#undef verify
#endif
#endif
/*
 * Reset the "visual" lists
 *
 * This involves resetting various things to their "default" state.
 *
 * If the "prefs" flag is TRUE, then we will also load the appropriate
 * "user pref file" based on the current setting of the "use_graphics"
 * flag.  This is useful for switching "graphics" on/off.
 *
 * The features, objects, and creatures, should all be encoded in the
 * relevant "font.pref" and/or "graf.prf" files.  XXX XXX XXX
 *
 * The "prefs" parameter is no longer meaningful.  XXX XXX XXX
 */
void reset_visuals(void)
{
	int i, j;

	/* Extract some info about terrain features */
	for (i = 0; i < max_feature_idx; i++)
	{
		feature_type *f_ptr = &feature_info[i];

		/* Assume we will use the underlying values */
		for (j = 0; j < F_LIT_MAX; j++)
		{
			f_ptr->x_attr[j] = f_ptr->d_attr[j];
			f_ptr->x_char[j] = f_ptr->d_char[j];
		}
	}

	/* Extract default attr/char code for objects */
	for (i = 0; i < max_object_kind_idx; i++)
	{
		object_kind *k_ptr = &object_kind_info[i];

		/* Default attr/char */
		k_ptr->x_attr = k_ptr->d_attr;
		k_ptr->x_char = k_ptr->d_char;
	}

	/* Extract default attr/char code for creatures */
	for (i = 0; i < max_species_idx; i++)
	{
		species_type *r_ptr = &species_info[i];

		/* Default attr/char */
		r_ptr->x_attr = r_ptr->d_attr;
		r_ptr->x_char = r_ptr->d_char;
	}

	if(use_graphics)
	{
		char buf[1024];

		/* Process "graf.prf" */
		process_pref_file("graf.prf");

		/* Access the "character" pref file */
		sprintf(buf, "graf-%s.prf", player_base);

		/* Process "graf-<playername>.prf" */
		process_pref_file(buf);
	}

	/* Normal symbols */
	else
	{
		char buf[1024];

		/* Process "font.prf" */

		process_pref_file("font.prf");

		/* Access the "character" pref file */
		sprintf(buf, "font-%s.prf", player_base);

		/* Process "font-<playername>.prf" */
		process_pref_file(buf);
	}
}


/*
 * Obtain the "flags" for an item
 */
void object_flags(object_type *object_ptr, u32b flgs[TRAIT_FLAG_MAX])
{
	object_kind *k_ptr = &object_kind_info[object_ptr->k_idx];
	int i;

	/* Base object */
	for (i = 0; i < TRAIT_FLAG_MAX; i++)
		flgs[i] = k_ptr->flags[i];

	/* Artifact */
	if(object_is_fixed_artifact(object_ptr))
	{
		artifact_type *a_ptr = &artifact_info[object_ptr->name1];

		for (i = 0; i < TRAIT_FLAG_MAX; i++)
			flgs[i] = a_ptr->flags[i];
	}

	/* Ego-item */
	if(object_is_ego(object_ptr))
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];

		for (i = 0; i < TRAIT_FLAG_MAX; i++)
			flgs[i] |= e_ptr->flags[i];

		if((object_ptr->name2 == EGO_LITE_AURA_FIRE) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if((object_ptr->name2 == EGO_LITE_INFRA) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_INFRA);
		}
		else if((object_ptr->name2 == EGO_LITE_EYE) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_NO_BLIND);
			remove_flag(flgs, TRAIT_SEE_INVISIBLE);
		}
	}

	/* Random artifact ! */
	for (i = 0; i < TRAIT_FLAG_MAX; i++)
		flgs[i] |= object_ptr->trait_flags[i];

	if(object_is_smith(object_ptr))
	{
		int add = object_ptr->xtra3 - 1;

		if(add < TRAIT_FLAG_MAX)
		{
			add_flag(flgs, add);
		}
		else if(add == ESSENCE_TMP_RES_ACID)
		{
			add_flag(flgs, TRAIT_RES_ACID);
		}
		else if(add == ESSENCE_TMP_RES_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
		}
		else if(add == ESSENCE_TMP_RES_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
		}
		else if(add == ESSENCE_TMP_RES_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
		}
		else if(add == ESSENCE_SH_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if(add == ESSENCE_SH_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_AURA_ELEC);
		}
		else if(add == ESSENCE_SH_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
			add_flag(flgs, TRAIT_AURA_COLD);
		}
		else if(add == ESSENCE_RESISTANCE)
		{
			add_flag(flgs, TRAIT_RES_ACID);
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_RES_COLD);
		}
		else if(add == TRAIT_SHATTER)
		{
			//TODO:TR_ACTIVATE add_flag(flgs, TR_ACTIVATE);
		}
	}
}



/*
 * Obtain the "flags" for an item which are known to the player
 */
void object_flags_known(object_type *object_ptr, u32b flgs[TRAIT_FLAG_MAX])
{
	bool spoil = FALSE;
	int i;

	object_kind *k_ptr = &object_kind_info[object_ptr->k_idx];

	/* Clear */
	for (i = 0; i < TRAIT_FLAG_MAX; i++)
		flgs[i] = 0;

	if(!object_is_aware(object_ptr)) return;

	/* Base object */
	for (i = 0; i < TRAIT_FLAG_MAX; i++)
		flgs[i] = k_ptr->flags[i];

	/* Must be identified */
	if(!object_is_known(object_ptr)) return;

	/* Ego-item (known basic flags) */
	if(object_is_ego(object_ptr))
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];

		for (i = 0; i < TRAIT_FLAG_MAX; i++)
			flgs[i] |= e_ptr->flags[i];

		if((object_ptr->name2 == EGO_LITE_AURA_FIRE) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if((object_ptr->name2 == EGO_LITE_INFRA) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_INFRA);
		}
		else if((object_ptr->name2 == EGO_LITE_EYE) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_NO_BLIND);
			remove_flag(flgs, TRAIT_SEE_INVISIBLE);
		}
	}


#ifdef SPOIL_ARTIFACTS
	/* Full knowledge for some artifacts */
	if(object_is_artifact(object_ptr)) spoil = TRUE;
#endif /* SPOIL_ARTIFACTS */

#ifdef SPOIL_EGO_ITEMS
	/* Full knowledge for some ego-items */
	if(object_is_ego(object_ptr)) spoil = TRUE;
#endif /* SPOIL_EGO_ITEMS */

	/* Need full knowledge or spoilers */
	if(spoil || (object_ptr->ident & IDENT_MENTAL))
	{
		/* Artifact */
		if(object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];

			for (i = 0; i < TRAIT_FLAG_MAX; i++)
				flgs[i] = a_ptr->flags[i];
		}

		/* Random artifact ! */
		for (i = 0; i < TRAIT_FLAG_MAX; i++)
			flgs[i] |= object_ptr->trait_flags[i];
	}

	if(object_is_smith(object_ptr))
	{
		int add = object_ptr->xtra3 - 1;

		if(add < TRAIT_FLAG_MAX)
		{
			add_flag(flgs, add);
		}
		else if(add == ESSENCE_TMP_RES_ACID)
		{
			add_flag(flgs, TRAIT_RES_ACID);
		}
		else if(add == ESSENCE_TMP_RES_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
		}
		else if(add == ESSENCE_TMP_RES_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
		}
		else if(add == ESSENCE_TMP_RES_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
		}
		else if(add == ESSENCE_SH_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if(add == ESSENCE_SH_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_AURA_ELEC);
		}
		else if(add == ESSENCE_SH_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
			add_flag(flgs, TRAIT_AURA_COLD);
		}
		else if(add == ESSENCE_RESISTANCE)
		{
			add_flag(flgs, TRAIT_RES_ACID);
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_RES_COLD);
		}
	}
}


/*
 * Determine the "Activation" (if any) for an artifact
 * Return a string, or NULL for "no activation"
 */
cptr item_activation(object_type *object_ptr)
{

#ifdef JP
	return "空気の息";
#else
	return "breathe air";
#endif

}


/*
 * Describe a "fully identified" item
 */
bool screen_object(object_type *object_ptr, u32b mode)
{
	int                     i = 0, j, k;

	u32b flgs[TRAIT_FLAG_MAX];

	char temp[70 * 20];
	cptr            info[128];
	char object_name[MAX_NLEN];
	int wid, hgt;

	int trivial_info = 0;

	/* Extract the flags */
	object_flags(object_ptr, flgs);

	/* Extract the description */
	{
		roff_to_buf(object_ptr->name1 ? (artifact_text + artifact_info[object_ptr->name1].text) :
			    (object_kind_text + object_kind_info[object_ptr->k_idx].text),
			    77 - 15, temp, sizeof(temp));
		for (j = 0; temp[j]; j += 1 + strlen(&temp[j]))
		{ info[i] = &temp[j]; i++;}
	}

	if(object_is_equipment(object_ptr))
	{
		/* Descriptions of a basic equipment is just a flavor */
		trivial_info = i;
	}

	/* Figurines, a hack */
	if(object_ptr->tval == TV_FIGURINE)
	{
#ifdef JP
info[i++] = "それは投げた時ペットに変化する。";
#else
		info[i++] = "It will transform into a pet when thrown.";
#endif

	}

	if((object_ptr->tval == TV_POLEARM) && (object_ptr->sval == SV_DEATH_SCYTHE))
	{
#ifdef JP
info[i++] = "それは自分自身に攻撃が返ってくることがある。";
#else
		info[i++] = "It causes you to strike yourself sometimes.";
#endif

#ifdef JP
info[i++] = "それは無敵のバリアを切り裂く。";
#else
		info[i++] = "It always penetrates invulnerability barriers.";
#endif
	}

	if(object_ptr->name2 == EGO_RING_THROW)
	{
#ifdef JP
info[i++] = "それは物を強く投げることを可能にする。";
#else
		info[i++] = "It provides great strength when you throw an item.";
#endif
	}

	if(object_ptr->tval == TV_STATUE)
	{
		species_type *r_ptr = &species_info[object_ptr->pval];


		if(object_ptr->sval == SV_DAKIMAKURA)
#ifdef JP
			info[i++] = "それはベッドに置くと癒される。";
#else
			info[i++] = "It is shameful.";
#endif	
		else if(object_ptr->sval == SV_DOUJINSHI)
#ifdef JP
			info[i++] = "それは公序良俗に反している。";
#else
			info[i++] = "It is HENTAI.";
#endif	
		else if(object_ptr->pval == SPECIES_RICHARD_STOLENMAN || object_ptr->sval == SV_FS_FIGURE || object_ptr->sval == SV_FIGURE )
#ifdef JP
			info[i++] = "それは部屋に飾ると恥ずかしい。";
#else
			info[i++] = "It is shameful.";
#endif
		else if(has_trait_species(r_ptr, TRAIT_ELDRITCH_HORROR))
#ifdef JP
			info[i++] = "それは部屋に飾ると恐い。";
#else
			info[i++] = "It is fearful.";
#endif
		else
#ifdef JP
			info[i++] = "それは部屋に飾ると楽しい。";
#else
		info[i++] = "It is cheerful.";
#endif
	}

	/* Hack -- describe lite's */
	if(object_ptr->tval == TV_LITE)
	{
		if(object_ptr->name2 == EGO_LITE_DARKNESS)
		{
#ifdef JP
			info[i++] = "それは全く光らない。";
#else
			info[i++] = "It provides no light.";
#endif

			if(object_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
				info[i++] = "それは明かりの半径を狭める(半径に-3)。";
#else
				info[i++] = "It decreases radius of light source by 3.";
#endif
			}
			else if(object_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
				info[i++] = "それは明かりの半径を狭める(半径に-2)。";
#else
				info[i++] = "It decreases radius of light source by 2.";
#endif
			}
			else
			{
#ifdef JP
				info[i++] = "それは明かりの半径を狭める(半径に-1)。";
#else
				info[i++] = "It decreases radius of light source by 1.";
#endif
			}
		}
		else if(object_is_fixed_artifact(object_ptr))
		{
#ifdef JP
info[i++] = "それは永遠なる明かり(半径 3)を授ける。";
#else
			info[i++] = "It provides light (radius 3) forever.";
#endif

		}
		else if(object_ptr->name2 == EGO_LITE_SHINE)
		{
			if(object_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
info[i++] = "それは永遠なる明かり(半径 3)を授ける。";
#else
				info[i++] = "It provides light (radius 3) forever.";
#endif

			}
			else if(object_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
info[i++] = "それは燃料補給によって明かり(半径 3)を授ける。";
#else
				info[i++] = "It provides light (radius 3) when fueled.";
#endif

			}
			else
			{
#ifdef JP
info[i++] = "それは燃料補給によって明かり(半径 2)を授ける。";
#else
				info[i++] = "It provides light (radius 2) when fueled.";
#endif

			}
		}
		else
		{
			if(object_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
info[i++] = "それは永遠なる明かり(半径 2)を授ける。";
#else
				info[i++] = "It provides light (radius 2) forever.";
#endif

			}
			else if(object_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
info[i++] = "それは燃料補給によって明かり(半径 2)を授ける。";
#else
				info[i++] = "It provides light (radius 2) when fueled.";
#endif

			}
			else
			{
#ifdef JP
info[i++] = "それは燃料補給によって明かり(半径 1)を授ける。";
#else
				info[i++] = "It provides light (radius 1) when fueled.";
#endif

			}
		}
		if(object_ptr->name2 == EGO_LITE_LONG)
		{
#ifdef JP
info[i++] = "それは長いターン明かりを授ける。";
#else
			info[i++] = "It provides light for much longer time.";
#endif
		}
	}


	/* And then describe it fully */

	if(have_flag(flgs, TRAIT_RIDING))
	{
		if((object_ptr->tval == TV_POLEARM) && ((object_ptr->sval == SV_LANCE) || (object_ptr->sval == SV_HEAVY_LANCE)))
#ifdef JP
info[i++] = "それは乗馬中は非常に使いやすい。";
#else
			info[i++] = "It is made for use while riding.";
#endif
		else
		{
#ifdef JP
			info[i++] = "それは乗馬中でも使いやすい。";
#else
			info[i++] = "It is suitable for use while riding.";
#endif
			/* This information is not important enough */
			trivial_info++;
		}
	}
	if(have_flag(flgs, STAT_STR))
	{
#ifdef JP
info[i++] = "それは腕力に影響を及ぼす。";
#else
		info[i++] = "It affects your strength.";
#endif

	}
	if(have_flag(flgs, STAT_INT))
	{
#ifdef JP
info[i++] = "それは知能に影響を及ぼす。";
#else
		info[i++] = "It affects your intelligence.";
#endif

	}
	if(have_flag(flgs, STAT_WIS))
	{
#ifdef JP
info[i++] = "それは賢さに影響を及ぼす。";
#else
		info[i++] = "It affects your wisdom.";
#endif

	}
	if(have_flag(flgs, STAT_DEX))
	{
#ifdef JP
info[i++] = "それは器用さに影響を及ぼす。";
#else
		info[i++] = "It affects your dexterity.";
#endif

	}
	if(have_flag(flgs, STAT_CON))
	{
#ifdef JP
info[i++] = "それは耐久力に影響を及ぼす。";
#else
		info[i++] = "It affects your constitution.";
#endif

	}
	if(have_flag(flgs, STAT_CHA))
	{
#ifdef JP
info[i++] = "それは魅力に影響を及ぼす。";
#else
		info[i++] = "It affects your charisma.";
#endif

	}

	if(have_flag(flgs, TRAIT_MAGIC_MASTERY))
	{
#ifdef JP
info[i++] = "それは魔法道具使用能力に影響を及ぼす。";
#else
		info[i++] = "It affects your ability to use magic devices.";
#endif

	}
	if(has_trait_object(object_ptr, TRAIT_STEALTH))
	{
#ifdef JP
info[i++] = "それは隠密行動能力に影響を及ぼす。";
#else
		info[i++] = "It affects your stealth.";
#endif

	}
	if(have_flag(flgs, TRAIT_SEARCH))
	{
#ifdef JP
info[i++] = "それは探索能力に影響を及ぼす。";
#else
		info[i++] = "It affects your searching.";
#endif

	}
	if(have_flag(flgs, TRAIT_INFRA))
	{
#ifdef JP
info[i++] = "それは赤外線視力に影響を及ぼす。";
#else
		info[i++] = "It affects your infravision.";
#endif

	}
	if(have_flag(flgs, TRAIT_TUNNEL))
	{
#ifdef JP
info[i++] = "それは採掘能力に影響を及ぼす。";
#else
		info[i++] = "It affects your ability to tunnel.";
#endif

	}
	if(have_flag(flgs, TRAIT_SPEED))
	{
#ifdef JP
info[i++] = "それはスピードに影響を及ぼす。";
#else
		info[i++] = "It affects your speed.";
#endif

	}
	if(has_trait_object(object_ptr, TRAIT_BLOWS))
	{
#ifdef JP
info[i++] = "それは打撃回数に影響を及ぼす。";
#else
		info[i++] = "It affects your attack speed.";
#endif

	}

	if(have_flag(flgs, TRAIT_ACID_BRAND))
	{
#ifdef JP
info[i++] = "それは酸によって大きなダメージを与える。";
#else
		info[i++] = "It does extra damage from acid.";
#endif

	}
	if(have_flag(flgs, TRAIT_ELEC_BRAND))
	{
#ifdef JP
info[i++] = "それは電撃によって大きなダメージを与える。";
#else
		info[i++] = "It does extra damage from electricity.";
#endif

	}
	if(have_flag(flgs, TRAIT_FIRE_BRAND))
	{
#ifdef JP
info[i++] = "それは火炎によって大きなダメージを与える。";
#else
		info[i++] = "It does extra damage from fire.";
#endif

	}
	if(have_flag(flgs, TRAIT_COLD_BRAND))
	{
#ifdef JP
info[i++] = "それは冷気によって大きなダメージを与える。";
#else
		info[i++] = "It does extra damage from frost.";
#endif

	}

	if(have_flag(flgs, TRAIT_POIS_BRAND))
	{
#ifdef JP
info[i++] = "それは敵を毒する。";
#else
		info[i++] = "It poisons your foes.";
#endif

	}

	if(have_flag(flgs, TRAIT_CHAOTIC_BRAND))
	{
#ifdef JP
info[i++] = "それはカオス的な効果を及ぼす。";
#else
		info[i++] = "It produces chaotic effects.";
#endif

	}

	if(have_flag(flgs, TRAIT_VAMPIRIC_BRAND))
	{
#ifdef JP
info[i++] = "それは敵からヒットポイントを吸収する。";
#else
		info[i++] = "It drains life from your foes.";
#endif

	}

	if(have_flag(flgs, TRAIT_SHATTER))
	{
#ifdef JP
info[i++] = "それは地震を起こすことができる。";
#else
		info[i++] = "It can cause earthquakes.";
#endif

	}

	if(have_flag(object_ptr->trait_flags, TRAIT_VORPAL))
	{
#ifdef JP
info[i++] = "それは非常に切れ味が鋭く敵を切断することができる。";
#else
		info[i++] = "It is very sharp and can cut your foes.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_DRAGON))
	{
#ifdef JP
info[i++] = "それはドラゴンにとっての天敵である。";
#else
		info[i++] = "It is a great bane of dragons.";
#endif

	}
	else if(have_flag(flgs, TRAIT_SLAY_DRAGON))
	{
#ifdef JP
info[i++] = "それはドラゴンに対して特に恐るべき力を発揮する。";
#else
		info[i++] = "It is especially deadly against dragons.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_ORC))
	{
#ifdef JP
info[i++] = "それはオークにとっての天敵である。";
#else
		info[i++] = "It is a great bane of orcs.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLAY_ORC))
	{
#ifdef JP
info[i++] = "それはオークに対して特に恐るべき力を発揮する。";
#else
		info[i++] = "It is especially deadly against orcs.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_TROLL))
	{
#ifdef JP
info[i++] = "それはトロルにとっての天敵である。";
#else
		info[i++] = "It is a great bane of trolls.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLAY_TROLL))
	{
#ifdef JP
info[i++] = "それはトロルに対して特に恐るべき力を発揮する。";
#else
		info[i++] = "It is especially deadly against trolls.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_GIANT))
	{
#ifdef JP
info[i++] = "それは巨人にとっての天敵である。";
#else
		info[i++] = "It is a great bane of giants.";
#endif
	}
	else if(have_flag(flgs, TRAIT_SLAY_GIANT))
	{
#ifdef JP
info[i++] = "それはジャイアントに対して特に恐るべき力を発揮する。";
#else
		info[i++] = "It is especially deadly against giants.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_DEMON))
	{
#ifdef JP
info[i++] = "それはデーモンにとっての天敵である。";
#else
		info[i++] = "It is a great bane of demons.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLAY_DEMON))
	{
#ifdef JP
info[i++] = "それはデーモンに対して聖なる力を発揮する。";
#else
		info[i++] = "It strikes at demons with holy wrath.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_UNDEAD))
	{
#ifdef JP
info[i++] = "それはアンデッドにとっての天敵である。";
#else
		info[i++] = "It is a great bane of undead.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLAY_UNDEAD))
	{
#ifdef JP
info[i++] = "それはアンデッドに対して聖なる力を発揮する。";
#else
		info[i++] = "It strikes at undead with holy wrath.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_EVIL))
	{
#ifdef JP
info[i++] = "それは邪悪なる存在にとっての天敵である。";
#else
		info[i++] = "It is a great bane of evil creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLAY_EVIL))
	{
#ifdef JP
info[i++] = "それは邪悪なる存在に対して聖なる力で攻撃する。";
#else
		info[i++] = "It fights against evil with holy fury.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_GOOD))
	{
#ifdef JP
info[i++] = "それは善良なる存在にとっての天敵である。";
#else
		info[i++] = "It is a great bane of good creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLAY_GOOD))
	{
#ifdef JP
info[i++] = "それは善良なる存在に対して地獄の力で攻撃する。";
#else
		info[i++] = "It fights against good with hellish fury.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_ANIMAL))
	{
#ifdef JP
info[i++] = "それは自然界の動物にとっての天敵である。";
#else
		info[i++] = "It is a great bane of natural creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLAY_ANIMAL))
	{
#ifdef JP
info[i++] = "それは自然界の動物に対して特に恐るべき力を発揮する。";
#else
		info[i++] = "It is especially deadly against natural creatures.";
#endif

	}

	if(have_flag(flgs, TRAIT_KILL_HUMAN))
	{
#ifdef JP
info[i++] = "それは人間にとっての天敵である。";
#else
		info[i++] = "It is a great bane of humans.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLAY_HUMAN))
	{
#ifdef JP
info[i++] = "それは人間に対して特に恐るべき力を発揮する。";
#else
		info[i++] = "It is especially deadly against humans.";
#endif

	}

	if(have_flag(flgs, TRAIT_FORCE_WEAPON))
	{
#ifdef JP
info[i++] = "それは使用者の魔力を使って攻撃する。";
#else
		info[i++] = "It powerfully strikes at a creature using your mana.";
#endif

	}
	if(have_flag(flgs, TRAIT_DEC_MANA))
	{
#ifdef JP
info[i++] = "それは魔力の消費を押さえる。";
#else
		info[i++] = "It decreases your mana consumption.";
#endif

	}
	if(have_flag(flgs, TRAIT_SUSTAIN_STR))
	{
#ifdef JP
info[i++] = "それはあなたの腕力を維持する。";
#else
		info[i++] = "It sustains your strength.";
#endif

	}
	if(have_flag(flgs, TRAIT_SUSTAIN_INT))
	{
#ifdef JP
info[i++] = "それはあなたの知能を維持する。";
#else
		info[i++] = "It sustains your intelligence.";
#endif

	}
	if(have_flag(flgs, TRAIT_SUSTAIN_WIS))
	{
#ifdef JP
info[i++] = "それはあなたの賢さを維持する。";
#else
		info[i++] = "It sustains your wisdom.";
#endif

	}
	if(have_flag(flgs, TRAIT_SUSTAIN_DEX))
	{
#ifdef JP
info[i++] = "それはあなたの器用さを維持する。";
#else
		info[i++] = "It sustains your dexterity.";
#endif

	}
	if(have_flag(flgs, TRAIT_SUSTAIN_CON))
	{
#ifdef JP
info[i++] = "それはあなたの耐久力を維持する。";
#else
		info[i++] = "It sustains your constitution.";
#endif

	}
	if(have_flag(flgs, TRAIT_SUSTAIN_CHR))
	{
#ifdef JP
info[i++] = "それはあなたの魅力を維持する。";
#else
		info[i++] = "It sustains your charisma.";
#endif

	}

	if(have_flag(flgs, TRAIT_IM_ACID))
	{
#ifdef JP
info[i++] = "それは酸に対する完全な免疫を授ける。";
#else
		info[i++] = "It provides immunity to acid.";
#endif

	}
	if(have_flag(flgs, TRAIT_IM_ELEC))
	{
#ifdef JP
info[i++] = "それは電撃に対する完全な免疫を授ける。";
#else
		info[i++] = "It provides immunity to electricity.";
#endif

	}
	if(have_flag(flgs, TRAIT_IM_FIRE))
	{
#ifdef JP
info[i++] = "それは火に対する完全な免疫を授ける。";
#else
		info[i++] = "It provides immunity to fire.";
#endif

	}
	if(have_flag(flgs, TRAIT_IM_COLD))
	{
#ifdef JP
info[i++] = "それは寒さに対する完全な免疫を授ける。";
#else
		info[i++] = "It provides immunity to cold.";
#endif

	}

	if(have_flag(flgs, TRAIT_THROW_MIGHTY))
	{
#ifdef JP
info[i++] = "それは敵に投げて大きなダメージを与えることができる。";
#else
		info[i++] = "It is perfectly balanced for throwing.";
#endif
	}

	if(have_flag(flgs, TRAIT_FREE_ACTION))
	{
#ifdef JP
info[i++] = "それは麻痺に対する完全な免疫を授ける。";
#else
		info[i++] = "It provides immunity to paralysis.";
#endif

	}
	if(have_flag(flgs, TRAIT_HOLD_LIFE))
	{
#ifdef JP
info[i++] = "それは生命力吸収に対する耐性を授ける。";
#else
		info[i++] = "It provides resistance to life draining.";
#endif

	}
	if(have_flag(flgs, TRAIT_FEARLESS))
	{
#ifdef JP
info[i++] = "それは恐怖への完全な耐性を授ける。";
#else
		info[i++] = "It makes you completely fearless.";
#endif

	}
	if(have_flag(flgs, TRAIT_RES_ACID))
	{
#ifdef JP
info[i++] = "それは酸への耐性を授ける。";
#else
		info[i++] = "It provides resistance to acid.";
#endif

	}
	if(have_flag(flgs, TRAIT_RES_ELEC))
	{
#ifdef JP
info[i++] = "それは電撃への耐性を授ける。";
#else
		info[i++] = "It provides resistance to electricity.";
#endif

	}
	if(have_flag(flgs, TRAIT_RES_FIRE))
	{
#ifdef JP
info[i++] = "それは火への耐性を授ける。";
#else
		info[i++] = "It provides resistance to fire.";
#endif

	}
	if(have_flag(flgs, TRAIT_RES_COLD))
	{
#ifdef JP
info[i++] = "それは寒さへの耐性を授ける。";
#else
		info[i++] = "It provides resistance to cold.";
#endif

	}
	if(have_flag(flgs, TRAIT_RES_POIS))
	{
#ifdef JP
info[i++] = "それは毒への耐性を授ける。";
#else
		info[i++] = "It provides resistance to poison.";
#endif

	}

	if(have_flag(flgs, TRAIT_RES_LITE))
	{
#ifdef JP
info[i++] = "それは閃光への耐性を授ける。";
#else
		info[i++] = "It provides resistance to light.";
#endif

	}
	if(have_flag(flgs, TRAIT_RES_DARK))
	{
#ifdef JP
info[i++] = "それは暗黒への耐性を授ける。";
#else
		info[i++] = "It provides resistance to dark.";
#endif

	}

	if(have_flag(flgs, TRAIT_NO_BLIND))
	{
#ifdef JP
info[i++] = "それは盲目への耐性を授ける。";
#else
		info[i++] = "It provides resistance to blindness.";
#endif

	}
	if(have_flag(flgs, TRAIT_NO_CONF))
	{
#ifdef JP
info[i++] = "それは混乱への耐性を授ける。";
#else
		info[i++] = "It provides resistance to confusion.";
#endif

	}
	if(have_flag(object_ptr->trait_flags, TRAIT_RES_SOUN))
	{
#ifdef JP
info[i++] = "それは轟音への耐性を授ける。";
#else
		info[i++] = "It provides resistance to sound.";
#endif

	}
	if(have_flag(flgs, TRAIT_RES_SHAR))
	{
#ifdef JP
info[i++] = "それは破片への耐性を授ける。";
#else
		info[i++] = "It provides resistance to shards.";
#endif

	}

	if(have_flag(object_ptr->trait_flags, TRAIT_RES_NETH))
	{
#ifdef JP
info[i++] = "それは地獄への耐性を授ける。";
#else
		info[i++] = "It provides resistance to nether.";
#endif

	}
	if(have_flag(object_ptr->trait_flags, TRAIT_RES_NEXU))
	{
#ifdef JP
info[i++] = "それは因果混乱への耐性を授ける。";
#else
		info[i++] = "It provides resistance to nexus.";
#endif

	}
	if(have_flag(object_ptr->trait_flags, TRAIT_RES_CHAO))
	{
#ifdef JP
info[i++] = "それはカオスへの耐性を授ける。";
#else
		info[i++] = "It provides resistance to chaos.";
#endif

	}
	if(have_flag(object_ptr->trait_flags, TRAIT_RES_DISE))
	{
#ifdef JP
info[i++] = "それは劣化への耐性を授ける。";
#else
		info[i++] = "It provides resistance to disenchantment.";
#endif

	}

	if(have_flag(flgs, TRAIT_LEVITATION))
	{
#ifdef JP
info[i++] = "それは宙に浮くことを可能にする。";
#else
		info[i++] = "It allows you to levitate.";
#endif

	}
	if(have_flag(flgs, TRAIT_LITE))
	{
		if((object_ptr->name2 == EGO_DARK))
#ifdef JP
info[i++] = "それは明かりの半径を狭める(半径に-1)。";
#else
			info[i++] = "It decreases radius of your light source by 1.";
#endif
		else
#ifdef JP
info[i++] = "それは永遠の明かりを授ける(半径に+1)。";
#else
			info[i++] = "It provides permanent light. (radius +1)";
#endif

	}
	if(have_flag(flgs, TRAIT_SEE_INVISIBLE))
	{
#ifdef JP
info[i++] = "それは透明なクリーチャーを見ることを可能にする。";
#else
		info[i++] = "It allows you to see invisible creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_ESP))
	{
#ifdef JP
info[i++] = "それはテレパシー能力を授ける。";
#else
		info[i++] = "It gives telepathic powers.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_ANIMAL))
	{
#ifdef JP
info[i++] = "それは自然界の生物を感知する。";
#else
		info[i++] = "It senses natural creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_UNDEAD))
	{
#ifdef JP
info[i++] = "それはアンデッドを感知する。";
#else
		info[i++] = "It senses undead.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_DEMON))
	{
#ifdef JP
info[i++] = "それは悪魔を感知する。";
#else
		info[i++] = "It senses demons.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_ORC))
	{
#ifdef JP
info[i++] = "それはオークを感知する。";
#else
		info[i++] = "It senses orcs.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_TROLL))
	{
#ifdef JP
info[i++] = "それはトロルを感知する。";
#else
		info[i++] = "It senses trolls.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_GIANT))
	{
#ifdef JP
info[i++] = "それは巨人を感知する。";
#else
		info[i++] = "It senses giants.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_DRAGON))
	{
#ifdef JP
info[i++] = "それはドラゴンを感知する。";
#else
		info[i++] = "It senses dragons.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_HUMAN))
	{
#ifdef JP
info[i++] = "それは人間を感知する。";
#else
		info[i++] = "It senses humans.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_EVIL))
	{
#ifdef JP
info[i++] = "それは邪悪な存在を感知する。";
#else
		info[i++] = "It senses evil creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_GOOD))
	{
#ifdef JP
info[i++] = "それは善良な存在を感知する。";
#else
		info[i++] = "It senses good creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_NONLIVING))
	{
#ifdef JP
info[i++] = "それは活動する無生物体を感知する。";
#else
		info[i++] = "It senses non-living creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_SENSE_UNIQUE))
	{
#ifdef JP
info[i++] = "それは特別な強敵を感知する。";
#else
		info[i++] = "It senses unique creatures.";
#endif

	}
	if(have_flag(flgs, TRAIT_SLOW_DIGEST))
	{
#ifdef JP
info[i++] = "それはあなたの新陳代謝を遅くする。";
#else
		info[i++] = "It slows your metabolism.";
#endif

	}
	if(have_flag(flgs, TRAIT_REGENERATE))
	{
#ifdef JP
info[i++] = "それは体力回復力を強化する。";
#else
		info[i++] = "It speeds your regenerative powers.";
#endif

	}
	if(have_flag(flgs, TRAIT_WARNING))
	{
#ifdef JP
info[i++] = "それは危険に対して警告を発する。";
#else
		info[i++] = "It warns you of danger";
#endif

	}
	if(have_flag(flgs, TRAIT_REFLECTING))
	{
#ifdef JP
info[i++] = "それは矢やボルトを反射する。";
#else
		info[i++] = "It reflects bolts and arrows.";
#endif

	}
	if(have_flag(flgs, TRAIT_AURA_FIRE))
	{
#ifdef JP
info[i++] = "それは炎のバリアを張る。";
#else
		info[i++] = "It produces a fiery sheath.";
#endif

	}
	if(have_flag(flgs, TRAIT_AURA_ELEC))
	{
#ifdef JP
info[i++] = "それは電気のバリアを張る。";
#else
		info[i++] = "It produces an electric sheath.";
#endif

	}
	if(have_flag(flgs, TRAIT_AURA_COLD))
	{
#ifdef JP
info[i++] = "それは冷気のバリアを張る。";
#else
		info[i++] = "It produces a sheath of coldness.";
#endif

	}
	if(have_flag(flgs, TRAIT_ANTI_MAGIC))
	{
#ifdef JP
info[i++] = "それは反魔法バリアを張る。";
#else
		info[i++] = "It produces an anti-magic shell.";
#endif

	}
	if(have_flag(flgs, TRAIT_PREVENT_TELEPORT))
	{
#ifdef JP
info[i++] = "それはテレポートを邪魔する。";
#else
		info[i++] = "It prevents teleportation.";
#endif

	}
	if(have_flag(flgs, TRAIT_EXTRA_ATTACK_MIGHT))
	{
#ifdef JP
info[i++] = "それは矢／ボルト／弾をより強力に発射することができる。";
#else
		info[i++] = "It fires missiles with extra might.";
#endif

	}
	if(have_flag(flgs, TRAIT_EXTRA_ATTACK_SPEED))
	{
#ifdef JP
info[i++] = "それは矢／ボルト／弾を非常に早く発射することができる。";
#else
		info[i++] = "It fires missiles excessively fast.";
#endif

	}

	if(have_flag(flgs, TRAIT_BLESSED_BRAND))
	{
#ifdef JP
info[i++] = "それは神に祝福されている。";
#else
		info[i++] = "It has been blessed by the gods.";
#endif

	}

	if(object_is_cursed(object_ptr))
	{
		if(have_flag(object_ptr->curse_flags, TRAIT_DIVINE_CURSE))
		{
			char tx[80];
			tx[0] = '\0';
#ifdef JP
			sprintf(tx, "それは神格 %d の神域の呪いがかけられている。", object_ptr->xtra1);
#else
			sprintf(tx, "It is divinely cursed (Divine Rank %d)", object_ptr->xtra1);
#endif
			info[i++] = tx;

		}
		else if(have_flag(object_ptr->curse_flags, TRAIT_HEAVY_CURSE))
		{
#ifdef JP
info[i++] = "それは強力な呪いがかけられている。";
#else
			info[i++] = "It is heavily cursed.";
#endif

		}
		else
		{
#ifdef JP
info[i++] = "それは呪われている。";
#else
			info[i++] = "It is cursed.";
#endif

			/*
			 * It's a trivial infomation since there is
			 * fake inscription {cursed}
			 */
			trivial_info++;
		}
	}

	/* Describe about this kind of object instead of THIS fake object */
	if(mode & SCROBJ_FAKE_OBJECT)
	{
		switch (object_ptr->tval)
		{
		case TV_RING:
			switch (object_ptr->sval)
			{
			case SV_RING_LORDLY:
#ifdef JP
				info[i++] = "それは幾つかのランダムな耐性を授ける。";
#else
				info[i++] = "It provides some random resistances.";
#endif
				break;
			case SV_RING_WARNING:
#ifdef JP
				info[i++] = "それはひとつの低級なESPを授ける事がある。";
#else
				info[i++] = "It may provide a low rank ESP.";
#endif
				break;
			}
			break;

		case TV_AMULET:
			switch (object_ptr->sval)
			{
			case SV_AMULET_RESISTANCE:
#ifdef JP
				info[i++] = "それは毒への耐性を授ける事がある。";
#else
				info[i++] = "It may provides resistance to poison.";
#endif
#ifdef JP
				info[i++] = "それはランダムな耐性を授ける事がある。";
#else
				info[i++] = "It may provide a random resistances.";
#endif
				break;
			case SV_AMULET_THE_MAGI:
#ifdef JP
				info[i++] = "それは最大で３つまでの低級なESPを授ける。";
#else
				info[i++] = "It provides up to three low rank ESPs.";
#endif
				break;
			}
			break;
		}
	}

	if(have_flag(flgs, TRAIT_IGNORE_ACID) &&
	    have_flag(flgs, TRAIT_IGNORE_ELEC) &&
	    have_flag(flgs, TRAIT_IGNORE_FIRE) &&
	    have_flag(flgs, TRAIT_IGNORE_COLD))
	{
#ifdef JP
		info[i++] = "それは酸・電撃・火炎・冷気では傷つかない。";
#else
		info[i++] = "It cannot be harmed by the elements.";
#endif
	}
	else
	{
		if(have_flag(flgs, TRAIT_IGNORE_ACID))
		{
#ifdef JP
			info[i++] = "それは酸では傷つかない。";
#else
			info[i++] = "It cannot be harmed by acid.";
#endif
		}
		if(have_flag(flgs, TRAIT_IGNORE_ELEC))
		{
#ifdef JP
			info[i++] = "それは電撃では傷つかない。";
#else
			info[i++] = "It cannot be harmed by electricity.";
#endif
		}
		if(have_flag(flgs, TRAIT_IGNORE_FIRE))
		{
#ifdef JP
			info[i++] = "それは火炎では傷つかない。";
#else
			info[i++] = "It cannot be harmed by fire.";
#endif
		}
		if(have_flag(flgs, TRAIT_IGNORE_COLD))
		{
#ifdef JP
			info[i++] = "それは冷気では傷つかない。";
#else
			info[i++] = "It cannot be harmed by cold.";
#endif
		}
	}

	if(mode & SCROBJ_FORCE_DETAIL) trivial_info = 0;

	/* No relevant informations */
	if(i <= trivial_info) return (FALSE);

	/* Save the screen */
	screen_save();

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Display Item name */
	if(!(mode & SCROBJ_FAKE_OBJECT))
		object_desc(object_name, object_ptr, 0);
	else
		object_desc(object_name, object_ptr, (OD_NAME_ONLY | OD_STORE));

	prt(object_name, 0, 0);

	/* Erase the screen */
	for (k = 1; k < hgt; k++) prt("", k, 13);

	/* Label the information */
	if((object_ptr->tval == TV_STATUE) && (object_ptr->sval == SV_PHOTO))
	{
		species_type *r_ptr = &species_info[object_ptr->pval];
		int namelen = strlen(species_name + r_ptr->name);
		prt(format("%s: '", species_name + r_ptr->name), 1, 15);
		Term_queue_bigchar(18 + namelen, 1, r_ptr->x_attr, r_ptr->x_char, 0, 0);
		prt("'", 1, (use_bigtile ? 20 : 19) + namelen);
	}
	else
#ifdef JP
prt("     アイテムの能力:", 1, 15);
#else
	prt("     Item Attributes:", 1, 15);
#endif

	/* We will print on top of the map (column 13) */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
		prt(info[j], k++, 15);

		/* Every 20 entries (lines 2 to 21), start over */
		if((k == hgt - 2) && (j+1 < i))
		{
#ifdef JP
prt("-- 続く --", k, 15);
#else
			prt("-- more --", k, 15);
#endif
			inkey();
			for (; k > 2; k--) prt("", k, 15);
		}
	}

	/* Wait for it */
#ifdef JP
prt("[何かキーを押すとゲームに戻ります]", k, 15);
#else
	prt("[Press any key to continue]", k, 15);
#endif

	inkey();

	/* Restore the screen */
	screen_load();

	/* Gave knowledge */
	return (TRUE);
}



// Convert an inventory index into a one character label
// Note that the label does NOT distinguish inven/equip.
char index_to_label(int i)
{
	if(i < 0 || i >= INVEN_TOTAL) return '-';
	return (i < 26 ? (char)i + 'a' : (char)(i - 26) + 'A'); 
}

// Convert a label into the index of an item in the "inven"
// Return "-1" if the label does not indicate a real item
s16b label_to_item(creature_type *creature_ptr, int c)
{
	int i;
	i = (islower(c) ? A2I(c) : -1);						// Convert
	if((i < 0) || (i > INVEN_TOTAL)) return (-1);		// Verify the index
	if(!creature_ptr->inventory[i].k_idx) return (-1);	// Empty slots can never be chosen
	return i;	// Return the index
}

// Return a string mentioning how a given item is carried
cptr mention_use_idx(creature_type *creature_ptr, int slot, int num)
{
	cptr p;

	/* Examine the location */
	switch (slot)
	{
#ifdef JP
		case INVEN_SLOT_INVENTORY: p = "所持"; break;
		case INVEN_SLOT_ARMS: p = "腕部"; break;
		case INVEN_SLOT_FEET: p = "脚部"; break;
		case INVEN_SLOT_TAIL: p = "尾部"; break;
		case INVEN_SLOT_INTAKE: p = "体内"; break;
		case INVEN_SLOT_AMMO: p = "矢弾"; break;
		case INVEN_SLOT_AMULET: p = "護符"; break;
		case INVEN_SLOT_LITE: p = "光源"; break;
		case INVEN_SLOT_INSTRUMENT: p = "楽器"; break;
		case INVEN_SLOT_BODY: p = "体"; break;
		case INVEN_SLOT_OUTER: p = "上衣"; break;
#else
		case INVEN_SLOT_INVENTORY: p = "In pack"; break;
		case INVEN_SLOT_ARMS: p = "On hands"; break;
		case INVEN_SLOT_FEET: p = "On feet"; break;
		case INVEN_SLOT_TAIL: p = "On tail"; break;
		case INVEN_SLOT_INTAKE: p = "Intake"; break;
		case INVEN_SLOT_AMMO: p = "Projectile"; break;
		case INVEN_SLOT_AMULET: p = "Amulet"; break;
		case INVEN_SLOT_LITE: p = "Light source"; break;
		case INVEN_SLOT_INSTRUMENT: p = "Instrument"; break;
		case INVEN_SLOT_BODY: p = "On body"; break;
		case INVEN_SLOT_OUTER: p = "About body"; break;
#endif

#ifdef JP
		case INVEN_SLOT_HAND:  
			if(creature_ptr->heavy_wield[num])
			{
				p = "運搬中"; break;
			}
			else
			{
				switch(num)
				{
					case 0: 
						if(has_trait(creature_ptr, TRAIT_HUMANOID))
						{
							if(has_trait(creature_ptr, TRAIT_LEFT_HANDER)) p = "左手";
							else p = "右手";
						}
						else p = "第１手";
						break;
					case 1:
						if(has_trait(creature_ptr, TRAIT_HUMANOID))
						{
							if(has_trait(creature_ptr, TRAIT_LEFT_HANDER)) p = "右手";
							else p = "左手";
						}
						else p = "第２手";
						break;
					case 2: p = "第３手"; break;
					case 3: p = "第４手"; break;
					case 4: p = "第５手"; break;
					case 5: p = "第６手"; break;
					case 6: p = "第７手"; break;
					case 7: p = "第８手"; break;
					default: p = "手？"; break;
				}
			}
			break;
#else
		case INVEN_SLOT_HAND:
			p = creature_ptr->heavy_wield[0] ? "Just lifting" : (creature_ptr->can_melee[0] ? "Wielding" : "On arm"); break;
#endif

#ifdef JP
		case INVEN_SLOT_BOW:
			p = "射撃用";
//			p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "運搬中" : "射撃用"; break;
			break;
#else
		case INVEN_SLOT_BOW:
			p = "Shooting";
//			p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "Just holding" : "Shooting"; break;
			break;
#endif

#ifdef JP
		case INVEN_SLOT_HEAD:
			switch(num)
			{
				case 0: p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "頭部" : "第１頭"; break;
				case 1: p = "第２頭"; break;
				case 2: p = "第３頭"; break;
				case 3: p = "第４頭"; break;
				case 4: p = "第５頭"; break;
				case 5: p = "第６頭"; break;
				case 6: p = "第７頭"; break;
				case 7: p = "第８頭"; break;
				default: p = "頭？"; break;
			}
			break;
#else
		case INVEN_SLOT_HEAD: p = "On head"; break;
#endif

#ifdef JP
		case INVEN_SLOT_RING:
			switch(num)
			{
				case 0: p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "右指" : "第１指"; break;
				case 1: p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "左指" : "第２指"; break;
				default: p = "指？"; break;
			}
			break;
#else
		case INVEN_SLOT_RING: p = "Ring"; break; break;
#endif
	}

	return p;	// Return the result
}

cptr mention_use_ptr(creature_type *creature_ptr, object_type *object_ptr)
{
	return mention_use_idx(creature_ptr, object_ptr->equipped_slot_type, object_ptr->equipped_slot_num);
}


/*
 * Return a string describing how a given item is being worn.
 * Currently, only used for items in the equipment, not inventory.
 */
cptr describe_use(creature_type *creature_ptr, int i)
{
	cptr p;

	switch (i)
	{
#ifdef JP
		case INVEN_SLOT_HAND:  p = creature_ptr->heavy_wield[0] ? "運搬中の" : ((creature_ptr->two_handed && creature_ptr->can_melee[0]) ? "両手に装備している" : (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "左手に装備している" : "右手に装備している")); break;
#else
		case INVEN_SLOT_HAND:  p = creature_ptr->heavy_wield[0] ? "just lifting" : (creature_ptr->can_melee[0] ? "attacking creatures with" : "wearing on your arm"); break;
#endif

#ifdef JP
		case INVEN_SLOT_BOW:   p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "持つだけで精一杯の" : "射撃用に装備している"; break;
#else
		case INVEN_SLOT_BOW:   p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "just holding" : "shooting missiles with"; break;
#endif

#ifdef JP
		case INVEN_SLOT_RING: p = (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "左手の指にはめている" : "右手の指にはめている"); break;
#else
		case INVEN_SLOT_RING: p = (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "wearing on your left hand" : "wearing on your right hand"); break;
#endif

#ifdef JP
		case INVEN_SLOT_AMULET:	p = "首にかけている";		break;
		case INVEN_SLOT_LITE:	p = "光源にしている";		break;
		case INVEN_SLOT_BODY:	p = "体に着ている";			break;
		case INVEN_SLOT_OUTER:	p = "身にまとっている";		break;
		case INVEN_SLOT_HEAD:	p = "頭にかぶっている";		break;
		case INVEN_SLOT_ARMS:	p = "手につけている";		break;
		case INVEN_SLOT_FEET:	p = "足にはいている";		break;
		default:				p = "ザックに入っている";	break;
#else
		case INVEN_SLOT_AMULET:	p = "wearing around your neck";	break;
		case INVEN_SLOT_LITE:	p = "using to light the way";	break;
		case INVEN_SLOT_BODY:	p = "wearing on your body";		break;
		case INVEN_SLOT_OUTER:	p = "wearing on your back";		break;
		case INVEN_SLOT_HEAD:	p = "wearing on your head";		break;
		case INVEN_SLOT_ARMS:	p = "wearing on your hands";	break;
		case INVEN_SLOT_FEET:	p = "wearing on your feet";		break;
		default:				p = "carrying in your pack";	break;
#endif
	}

	return p;	// Return the result
}


/* Hack: Check if a spellbook is one of the realms we can use. -- TY */

bool check_book_realm(creature_type *creature_ptr, const byte book_tval, const byte book_sval)
{
	if(book_tval < TV_LIFE_BOOK) return FALSE;
	if(creature_ptr->class_idx == CLASS_SORCERER)
	{
		return is_magic(tval2realm(book_tval));
	}
	else if(creature_ptr->class_idx == CLASS_RED_MAGE)
	{
		if(is_magic(tval2realm(book_tval)))
			return ((book_tval == TV_ARCANE_BOOK) || (book_sval < 2));
	}
	return (REALM1_BOOK(creature_ptr) == book_tval || REALM2_BOOK(creature_ptr) == book_tval);
}


/*
 * Check an item against the item tester info
 */
bool item_tester_okay(creature_type *creature_ptr, object_type *object_ptr, bool (*item_tester_hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval)
{

	/* Require an item */
	if(!is_valid_object(object_ptr)) return (FALSE);

	/* Hack -- ignore "gold" */
	/* TODO remove
	if(object_ptr->tval == TV_GOLD)
	{
		// See xtra2.c
		extern bool show_gold_on_floor;

		if(!show_gold_on_floor) return (FALSE);
	}
	*/

	/* Check the tval */
	if(item_tester_tval)
	{
		/* Is it a spellbook? If so, we need a hack -- TY */
		if((item_tester_tval <= TV_DEATH_BOOK) &&
			(item_tester_tval >= TV_LIFE_BOOK))
			return check_book_realm(creature_ptr, object_ptr->tval, object_ptr->sval);
		else
			if(item_tester_tval != object_ptr->tval) return (FALSE);
	}

	/* Check the hook */
	if(item_tester_hook)
	{
		if(!(*item_tester_hook)(creature_ptr, object_ptr)) return (FALSE);
	}

	/* Assume okay */
	return (TRUE);
}


static void display_item_aux(creature_type *creature_ptr, int idx, int y)
{
	char	tmp_val[80];
	char	object_name[MAX_NLEN];
	int		n;
	int		wid, hgt;
	byte	attr = TERM_WHITE;

	object_type *object_ptr = &creature_ptr->inventory[idx];
	Term_get_size(&wid, &hgt);	// Get size

	tmp_val[0] = index_to_label(idx);	// Prepare an "index"
	tmp_val[1] = ')';
	tmp_val[2] = ' ';

	Term_erase(0, y, 255);
	Term_putstr(0, y, 3, TERM_WHITE, tmp_val);	// Display the index (or blank space)

	if(is_valid_object(object_ptr))
	{
		object_desc(object_name, object_ptr, 0);
		attr = tval_to_acttr[object_ptr->tval % 128];	// Get a color
		if(attr == TERM_DARK) attr = TERM_WHITE;
		if(object_ptr->timeout) attr = TERM_L_DARK;		// Grey out charging items
	}
	else
	{
		strcpy(object_name, "------");
		attr = TERM_L_DARK;
	}

	n = strlen(object_name);					// Obtain the length of the description
	Term_putstr(15, y, n, attr, object_name);		// Display the entry itself
	Term_erase(15 + n, y, 255);						// Erase the rest of the line

	if(show_weights)	// Display the weight if needed
	{
		int wgt = object_ptr->weight * object_ptr->number;
		char buf[80];
		format_weight(buf, wgt);
		(void)sprintf(tmp_val, "%10s", buf);
		prt(tmp_val, y, wid - 10);
	}
}

// Choice window "shadow" of the "show_item_list()" function
void display_inven(creature_type *creature_ptr)
{
	int		i, z;
	object_type	*object_ptr;
	int		list[INVEN_TOTAL];
	int		wid, hgt;

	Term_get_size(&wid, &hgt);	// Get size

	for (z = 0, i = 0; i < INVEN_TOTAL; i++)	// Find the "final" slot
	{
		object_ptr = &creature_ptr->inventory[i];
		if(IS_EQUIPPED(object_ptr)) continue;
		if(!is_valid_object(object_ptr)) continue;	// Skip non-objects
		list[z] = i;
		z++;	// Track
	}

	for (i = 0; i < z; i++)
	{
		display_item_aux(creature_ptr, list[i], i);
		Term_putstr(4, i, wid, TERM_WHITE, mention_use_ptr(creature_ptr, object_ptr));
	}
	for (i = z; i < hgt; i++) Term_erase(0, i, 255);	// Erase the rest of the window
}

// Choice window "shadow" of the "show_item_list()" function
void display_equip(creature_type *creature_ptr)
{
	register        int i, j, l, n;
	byte            attr = TERM_WHITE;
	int             wid, hgt;

	Term_get_size(&wid, &hgt);	// Get size

	n = 0;
	for (i = 0; i < MAX_INVENTORY_SLOTS; i++)
	{
		if(i == INVEN_SLOT_INVENTORY) continue;
		for(j = 0; j < creature_ptr->item_slot_num[i]; j++)
		{
			l = get_equipped_slot_idx(creature_ptr, i, j);
			display_item_aux(creature_ptr, l, n);
			Term_putstr(4, n, wid, TERM_WHITE, mention_use_idx(creature_ptr, i, j));
			n++;
		}
	}

	for (i = n; i < hgt; i++) Term_erase(0, i, 255);	// Erase the rest of the window

}


/*
 * Find the "first" inventory object with the given "tag".
 *
 * A "tag" is a numeral "n" appearing as "@n" anywhere in the
 * inscription of an object.  Alphabetical characters don't work as a
 * tag in this form.
 *
 * Also, the tag "@xn" will work as well, where "n" is a any tag-char,
 * and "x" is the "current" command_cmd code.
 */
static bool get_tag(creature_type *creature_ptr, int *cp, char tag, int mode)
{
	int i;
	cptr s;

	/**** Find a tag in the form of {@x#} (allow alphabet tag) ***/

	/* Check every creature_ptr->inventory object */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		if(IS_EQUIPPED(object_ptr) && mode != USE_INVEN) continue;
		if(!IS_EQUIPPED(object_ptr) && mode != USE_EQUIP) continue;

		/* Skip empty inscriptions */
		if(!object_ptr->inscription) continue;

		/* Skip non-choice */
		if(!item_tester_okay(creature_ptr, object_ptr, NULL, 0)) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(object_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if((s[1] == command_cmd) && (s[2] == tag))
			{
				/* Save the actual creature_ptr->inventory ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}


	/**** Find a tag in the form of {@#} (allows only numerals)  ***/

	/* Don't allow {@#} with '#' being alphabet */
	if(tag < '0' || '9' < tag)
	{
		/* No such tag */
		return FALSE;
	}

	/* Check every object */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];

		if(IS_EQUIPPED(object_ptr) && mode != USE_INVEN) continue;
		if(!IS_EQUIPPED(object_ptr) && mode != USE_EQUIP) continue;

		/* Skip non-objects */
		if(!is_valid_object(object_ptr)) continue;

		/* Skip empty inscriptions */
		if(!object_ptr->inscription) continue;

		/* Skip non-choice */
		if(!item_tester_okay(creature_ptr, object_ptr, NULL, 0)) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(object_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the normal tags */
			if(s[1] == tag)
			{
				/* Save the actual creature_ptr->inventory ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}

	/* No such tag */
	return (FALSE);
}


/*
 * Find the "first" floor object with the given "tag".
 *
 * A "tag" is a numeral "n" appearing as "@n" anywhere in the
 * inscription of an object.  Alphabetical characters don't work as a
 * tag in this form.
 *
 * Also, the tag "@xn" will work as well, where "n" is a any tag-char,
 * and "x" is the "current" command_cmd code.
 */
static bool get_tag_floor(int *cp, char tag, int floor_list[], int floor_num)
{
	int i;
	cptr s;

	/**** Find a tag in the form of {@x#} (allow alphabet tag) ***/

	/* Check every object in the grid */
	for (i = 0; i < floor_num && i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &object_list[floor_list[i]];

		/* Skip empty inscriptions */
		if(!object_ptr->inscription) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(object_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if((s[1] == command_cmd) && (s[2] == tag))
			{
				/* Save the actual floor object ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}


	/**** Find a tag in the form of {@#} (allows only numerals)  ***/

	/* Don't allow {@#} with '#' being alphabet */
	if(tag < '0' || '9' < tag)
	{
		/* No such tag */
		return FALSE;
	}

	/* Check every object in the grid */
	for (i = 0; i < floor_num && i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &object_list[floor_list[i]];

		/* Skip empty inscriptions */
		if(!object_ptr->inscription) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(object_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the normal tags */
			if(s[1] == tag)
			{
				/* Save the floor object ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = my_strchr(s + 1, '@');
		}
	}

	/* No such tag */
	return (FALSE);
}


/*
 * Move around label characters with correspond tags
 */
static void prepare_label_string(creature_type *creature_ptr, char *label, int mode)
{
	cptr alphabet_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int  i, j;

	/* Prepare normal labels */
	strcpy(label, alphabet_chars);

	/* Move each label */
	for (i = 0, j = 0; i < INVEN_TOTAL; i++)
	{
		int index;
		char c;
		object_type *object_ptr = &creature_ptr->inventory[i];
		if(!IS_EQUIPPED(object_ptr) && mode != USE_EQUIP) continue;
		if( IS_EQUIPPED(object_ptr) && mode != USE_INVEN) continue;

		c = alphabet_chars[j];

		/* Find a tag with this label */
		if(get_tag(creature_ptr, &index, c, mode))
		{
			/* Delete the overwritten label */
			if(label[j] == c) label[j] = ' ';

			/* Move the label to the place of corresponding tag */
			label[index] = c;
		}
		j++;
	}
}


/*
 * Move around label characters with correspond tags (floor version)
 */
static void prepare_label_string_floor(char *label, int floor_list[], int floor_num)
{
	cptr alphabet_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int  i;

	/* Prepare normal labels */
	strcpy(label, alphabet_chars);

	/* Move each label */
	for (i = 0; i < 52; i++)
	{
		int index;
		char c = alphabet_chars[i];

		/* Find a tag with this label */
		if(get_tag_floor(&index, c, floor_list, floor_num))
		{
			/* Delete the overwritten label */
			if(label[i] == c) label[i] = ' ';

			/* Move the label to the place of corresponding tag */
			label[index] = c;
		}
	}
}


/*
 * Display the inventory.
 *
 * Hack -- do not display "trailing" empty slots
 */
int show_item_list(int target_item, creature_type *creature_ptr, u32b flags, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr))
{
	int             i, j, k, l, m, n;
	int             col, cur_col, len;
	object_type     *object_ptr;
	char            object_name[MAX_NLEN];
	char            tmp_val[MAX_NLEN];
	int             out_index[INVEN_TOTAL];
	byte            out_color[INVEN_TOTAL];
	char            out_desc[INVEN_TOTAL][MAX_NLEN];
	int             target_item_label = 0;
	int             wid, hgt;
	char            inven_label[52 + 1];

	int             slot[INVEN_TOTAL];
	int             num[INVEN_TOTAL];

	col = 999;	// Starting column
	Term_get_size(&wid, &hgt);	// Get size
	len = wid - col - 1;	// Default "max-length"

	prepare_label_string(creature_ptr, inven_label, USE_INVEN);

	if(flags & SHOW_ITEM_EQUIPMENT)
	{
		for(k = 0, i = 0; i < MAX_INVENTORY_SLOTS; i++)
		{
			if(i == INVEN_SLOT_INVENTORY) continue;
			n = creature_ptr->item_slot_num[i]; 
			for(j = 1; j <= n; j++)
			{
				m = get_equipped_slot_idx(creature_ptr, i, j);
				if(m < 0) continue;
				object_ptr = &creature_ptr->inventory[m];
				object_desc(object_name, object_ptr, 0); // Describe the object

				// Save the object index, color, and description
				out_index[k] = m;
				out_color[k] = tval_to_acttr[object_ptr->tval % 128];

				if(object_ptr->timeout) out_color[k] = TERM_L_DARK; // Grey out charging items

				(void)strcpy(out_desc[k], object_name);

				l = strlen(out_desc[k]); // Find the predicted "line length"
				if(show_weights) l += 15; // Be sure to account for the weight

				if(show_item_graph) // Account for icon if displayed
				{
					l += 2;
					if(use_bigtile) l++;
				}

				if(l > len) len = l; // Maintain the maximum length

				slot[k] = i;
				num[k] = j;

				k++; // Advance to next "line"
			}
		}
	}

	// Display item
	if(flags & SHOW_ITEM_INVENTORY)
	{
		for (k = 0, i = 0; i < INVEN_TOTAL; i++)
		{
			object_ptr = &creature_ptr->inventory[i];

			/* Is this item acceptable? */
			if(!is_valid_object(object_ptr)) continue;

			if(!(flags & SHOW_ITEM_INVENTORY) && !item_tester_okay(creature_ptr, object_ptr, hook, 0)) continue;
			if(!((IS_EQUIPPED(object_ptr) && (flags & SHOW_ITEM_EQUIPMENT)) || (!IS_EQUIPPED(object_ptr) && (flags & SHOW_ITEM_INVENTORY)))) continue;

			/* Describe the object */
			object_desc(object_name, object_ptr, 0);

			/* Save the object index, color, and description */
			out_index[k] = i;
			out_color[k] = tval_to_acttr[object_ptr->tval % 128];

			/* Grey out charging items */
			if(object_ptr->timeout)
			{
				out_color[k] = TERM_L_DARK;
			}

			(void)strcpy(out_desc[k], object_name);

			/* Find the predicted "line length" */
			l = strlen(out_desc[k]);

			/* Be sure to account for the weight */
			if(show_weights) l += 15;

			/* Account for icon if displayed */
			if(show_item_graph)
			{
				l += 2;
				if(use_bigtile) l++;
			}

			/* Maintain the maximum length */
			if(l > len) len = l;

			slot[k] = GET_INVEN_SLOT_TYPE(creature_ptr, i);
			num[k] = IS_EQUIPPED(object_ptr);

			/* Advance to next "line" */
			k++;
		}
	}

	if(flags & SHOW_ITEM_RIGHT_SET) col = (len > wid - 9) ? 0 : (wid - len - 9);
	else col = 1;

	if(!k)
	{
		if(flags & SHOW_ITEM_EQUIPMENT)
		{
#if JP
			put_str("[何も装備できない]", 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 23 : 1);
#else
			put_str("[No Equipment]", 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 15 : 1);
#endif
		}
		else if(flags & SHOW_ITEM_INVENTORY)
		{
#if JP
			put_str("[何も持っていない]", 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 21 : 1);
#else
			put_str("[No Inventory]", 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 15 : 1);
#endif
		}
		return 0;
	}


	// Output each entry
	for (j = 0; j < k; j++)
	{
		i = out_index[j]; // Get the index
		object_ptr = &creature_ptr->inventory[i]; // Get the item
		prt("", j + 1, col ? col - 2 : col); // Clear the line

		if(use_menu && target_item)
		{
			if(j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "》");
#else
				strcpy(tmp_val, "> ");
#endif
				target_item_label = i;
			}
			else strcpy(tmp_val, "  ");
		}
		else
		{
			sprintf(tmp_val, "%c)", index_to_label(i)); // Prepare an index --(--
		}

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j + 1, col);

		cur_col = col + 3;

		/* Display graphics for object, if desired */
		if(show_item_graph)
		{
			byte a = object_attr(object_ptr);
			char c = object_char(object_ptr);

			if(j + 1 >= hgt) break;
			Term_queue_bigchar(cur_col, j + 1, a, c, 0, 0);
			if(use_bigtile) cur_col++;

			cur_col += 2;
		}

		// Display the entry itself
		c_put_str(IS_EQUIPPED(object_ptr) ? TERM_WHITE : TERM_L_DARK, mention_use_ptr(creature_ptr, object_ptr) , j + 1, cur_col);
		c_put_str(out_color[j], out_desc[j], j + 1, cur_col + 7);

		// Display the weight if needed
		if(show_weights)
		{
			int wgt = object_ptr->weight * object_ptr->number;
			char buf[80];
			format_weight(buf, wgt);
			(void)sprintf(tmp_val, "%10s", buf);

			prt(tmp_val, j + 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 10 : len + 3);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if(j && (j < INVEN_TOTAL)) prt("", j + 1, col ? col - 2 : col);

	/* Save the new column */
	//command_gap = col;

	return target_item_label;
}


/*
 * Flip "inven" and "equip" in any sub-windows
 */
void toggle_inven_equip(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		/* Unused */
		if(!angband_term[j]) continue;

		/* Flip inven to equip */
		if(window_flag[j] & (PW_INVEN))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_INVEN);
			window_flag[j] |= (PW_EQUIP);

			/* Window stuff */
			play_window |= (PW_EQUIP);
		}

		/* Flip inven to equip */
		else if(window_flag[j] & (PW_EQUIP))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_EQUIP);
			window_flag[j] |= (PW_INVEN);

			/* Window stuff */
			play_window |= (PW_INVEN);
		}
	}
}



/*
 * Verify the choice of an item.
 *
 * The item can be negative to mean "item on floor".
 */
static bool verify(creature_type *creature_ptr, cptr prompt, int item)
{
	char        object_name[MAX_NLEN];
	char        out_val[MAX_NLEN+20];
	object_type *object_ptr;


	/* inventory */
	if(item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Floor */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	/* Describe */
	object_desc(object_name, object_ptr, 0);

	/* Prompt */
#ifdef JP
(void)sprintf(out_val, "%s%sですか? ", prompt, object_name);
#else
	(void)sprintf(out_val, "%s %s? ", prompt, object_name);
#endif


	/* Query */
	return (get_check(out_val));
}


/*
 * Hack -- allow user to "prevent" certain choices
 *
 * The item can be negative to mean "item on floor".
 */
static bool get_item_allow(creature_type *creature_ptr, int item)
{
	cptr s;

	object_type *object_ptr;

	if(!command_cmd) return TRUE; /* command_cmd is no longer effective */

	/* creature_ptr->inventory */
	if(item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Floor */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	/* No inscription */
	if(!object_ptr->inscription) return (TRUE);

	/* Find a '!' */
	s = my_strchr(quark_str(object_ptr->inscription), '!');

	/* Process preventions */
	while (s)
	{
		/* Check the "restriction" */
		if((s[1] == command_cmd) || (s[1] == '*'))
		{
			/* Verify the choice */
#ifdef JP
			if(!verify(creature_ptr, "本当に", item)) return (FALSE);
#else
			if(!verify("Really try", item)) return (FALSE);
#endif

		}

		/* Find another '!' */
		s = my_strchr(s + 1, '!');
	}

	/* Allow it */
	return (TRUE);
}



/*
 * Auxiliary function for "get_item()" -- test an index
 */
static bool get_item_okay(creature_type *creature_ptr, int i, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval)
{
	if((i < 0) || (i >= INVEN_TOTAL)) return (FALSE); // Illegal items
	if(!item_tester_okay(creature_ptr, &creature_ptr->inventory[i], hook, item_tester_tval)) return (FALSE); // Verify the item
	return (TRUE); // Assume okay
}



/*
 * Determine whether get_item() can get some item or not
 * assuming mode = (USE_EQUIP | USE_INVEN | USE_FLOOR).
 */
bool can_get_item(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int j, floor_list[23], floor_num = 0;

	for (j = 0; j < INVEN_TOTAL; j++)
		if(item_tester_okay(creature_ptr, &creature_ptr->inventory[j], NULL, 0))
			return TRUE;

	floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);
	if(floor_num)
		return TRUE;

	return FALSE;
}

int get_equip_slot(creature_type *creature_ptr, int slot, cptr r, cptr s)
{
	selection se[16];
	int i, n, slot_num = creature_ptr->item_slot_num[slot];
	char buf[100];
	object_type *object_ptr;

	screen_save();
	c_put_str(TERM_L_BLUE, r, 0, 0);

	if(slot_num == 0 || slot == INVEN_SLOT_INVENTORY)
	{
		msg_print(s);
		n = -1;
	}
	if(slot_num == 1)
	{
		n = 0;
	}
	else
	{
		for(i = 0; i < slot_num; i++)
		{
			object_ptr = get_equipped_slot_ptr(creature_ptr, slot, i);
			object_desc(buf, object_ptr, 0);
			sprintf(se[i].cap, "%-6s %s", mention_use_ptr(creature_ptr, object_ptr), buf);
			se[i].code = i;
			se[i].key = '\0';

			if(object_ptr->timeout)
			{
				 se[i].d_color = TERM_L_DARK;
				 se[i].l_color = TERM_L_DARK;
			}
			else
			{
				se[i].d_color = tval_to_acttr[object_ptr->tval % 128];
				se[i].l_color = tval_to_acttr[object_ptr->tval % 128];
			}

		}

#if JP
		strcpy(se[i - 1].cap, "キャンセル");
#else
		strcpy(se[i - 1].cap, "Cancel");
#endif
		se[i].code = 0;
		se[i].key = ESCAPE;
		se[i].d_color = TERM_L_DARK;
		se[i].l_color = TERM_WHITE;

		n = get_selection(se, slot_num, 0, 1, 22, 1, 30, NULL, GET_SE_NO_FRAME | GET_SE_AUTO_WIDTH | GET_SE_AUTO_HEIGHT | GET_SE_RIGHT);
	}
	screen_load();

	return n;
}


/*
 * Let the user select an item, save its "index"
 *
 * Return TRUE only if an acceptable item was chosen by the user.
 *
 * The selected item must satisfy the "item_tester_hook()" function,
 * if that hook is set, and the "item_tester_tval", if that value is set.
 *
 * All "item_tester" restrictions are cleared before this function returns.
 *
 * The user is allowed to choose acceptable items from the equipment,
 * inventory, or floor, respectively, if the proper flag was given,
 * and there are any acceptable items in that location.
 *
 * The equipment or inventory are displayed (even if no acceptable
 * items are in that location) if the proper flag was given.
 *
 * If there are no acceptable items available anywhere, and "str" is
 * not NULL, then it will be used as the text of a warning message
 * before the function returns.
 *
 * Note that the user must press "-" to specify the item on the floor,
 * and there is no way to "examine" the item on the floor, while the
 * use of "capital" letters will "examine" an inventory/equipment item,
 * and prompt for its use.
 *
 * If a legal item is selected from the inventory, we save it in "cp"
 * directly (0 to 35), and return TRUE.
 *
 * If a legal item is selected from the floor, we save it in "cp" as
 * a negative (-1 to -511), and return TRUE.
 *
 * If no item is available, we do nothing to "cp", and we display a
 * warning message, using "str" if available, and return FALSE.
 *
 * If no item is selected, we do nothing to "cp", and return FALSE.
 *
 * Global "command_new" is used when viewing the inventory or equipment
 * to allow the user to enter a command while viewing those screens, and
 * also to induce "auto-enter" of stores, and other such stuff.
 *
 * Global "command_see" may be set before calling this function to start
 * out in "browse" mode.  It is cleared before this function returns.
 *
 * Global "command_wrk" is used to choose between equip/inven listings.
 * If it is TRUE then we are viewing inventory, else equipment.
 *
 * We always erase the prompt when we are done, leaving a blank line,
 * or a warning message, if appropriate, if no items are available.
 */
bool get_item(creature_type *creature_ptr, int *cp, cptr pmt, cptr str, int mode, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	s16b this_object_idx, next_object_idx = 0;

	char which = ' ';

	int j, k, i1, i2, e1, e2;

	bool done, item;

	bool oops = FALSE;

	bool equip = FALSE;
	bool inven = FALSE;
	bool floor = FALSE;
	bool equip_slot = FALSE;
	bool select_the_force = FALSE;

	bool allow_floor = FALSE;

	bool toggle = FALSE;

	char tmp_val[160];
	char out_val[160];

	int menu_line = (use_menu ? 1 : 0);
	int max_inven = 0;
	int max_equip = 0;

	static char prev_tag = '\0';
	char cur_tag = '\0';

	if(easy_floor || use_menu) return get_item_floor(creature_ptr, cp, pmt, str, mode, hook, item_tester_tval);

	// Extract args
	if(mode & USE_EQUIP) equip = TRUE;
	if(mode & USE_INVEN) inven = TRUE;
	if(mode & USE_FLOOR) floor = TRUE;
	if(mode & USE_EQUIP_SLOT) equip_slot = TRUE;
	if(mode & USE_FORCE) select_the_force = TRUE;

	// Get the item index
	if(repeat_pull(cp))
	{
		// the_force
		if(select_the_force && (*cp == INVEN_FORCE))
		{
			item_tester_tval = 0;
			hook = NULL;
			command_cmd = 0; // Hack -- command_cmd is no longer effective
			return (TRUE);
		}

		// Floor item?
		else if(floor && (*cp < 0))
		{
			object_type *object_ptr;

			// Special index
			k = 0 - (*cp);

			// Acquire object
			object_ptr = &object_list[k];

			// Validate the item
			if(item_tester_okay(creature_ptr, object_ptr, hook, item_tester_tval))
			{
				// Forget restrictions
				item_tester_tval = 0;
				hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}

		else if((*cp >= 0) && (*cp < INVEN_TOTAL))
		{
			if(prev_tag && command_cmd)
			{
				// Look up the tag and validate the item
				if(!get_tag(creature_ptr, &k, prev_tag, !IS_EQUIPPED(&creature_ptr->inventory[*cp]) ? USE_EQUIP : USE_INVEN));
				else if(!!IS_EQUIPPED(&creature_ptr->inventory[*cp]) ? !inven : !equip);
				else if(!get_item_okay(creature_ptr, k, hook, item_tester_tval));
				else
				{
					/* Accept that choice */
					(*cp) = k;

					/* Forget restrictions */
					item_tester_tval = 0;
					hook = NULL;
					command_cmd = 0; /* Hack -- command_cmd is no longer effective */

					/* Success */
					return TRUE;
				}

				prev_tag = '\0'; /* prev_tag is no longer effective */
			}

			/* Verify the item */
			else if(get_item_okay(creature_ptr, *cp, hook, item_tester_tval))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}
	}

	/* Paranoia XXX XXX XXX */
	msg_print(NULL);

	/* Not done */
	done = FALSE;

	/* No item selected */
	item = FALSE;

	// Full creature_ptr->inventory
	i1 = 0;
	i2 = INVEN_TOTAL - 1;

	// Forbid creature_ptr->inventory
	if(!inven) i2 = -1;
	else if(use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
			if(item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_inven++;
	}

	// Restrict inventory indexes
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i1, hook, item_tester_tval))) i1++;
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i2, hook, item_tester_tval))) i2--;

	// Full equipment
	e1 = 0;
	e2 = INVEN_TOTAL - 1;

	// Forbid equipment
	if(!equip) e2 = -1;

	else if(use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
		{
			if(!IS_EQUIPPED(&creature_ptr->inventory[j])) continue;
			if(item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_equip++;
		}
	}

	// Restrict equipment indexes
	while ((e1 <= e2) && (!get_item_okay(creature_ptr, e1, hook, item_tester_tval))) e1++;
	while ((e1 <= e2) && (!get_item_okay(creature_ptr, e2, hook, item_tester_tval))) e2--;

	// Restrict floor usage
	if(floor)
	{
		/* Scan all objects in the grid */
		for (this_object_idx = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx; this_object_idx; this_object_idx = next_object_idx)
		{
			object_type *object_ptr;

			/* Acquire object */
			object_ptr = &object_list[this_object_idx];

			/* Acquire next object */
			next_object_idx = object_ptr->next_object_idx;

			/* Accept the item on the floor if legal */
			if(item_tester_okay(creature_ptr, object_ptr, hook, item_tester_tval) && (object_ptr->marked & OM_FOUND)) allow_floor = TRUE;
		}
	}

	// Require at least one legal choice
	if(!allow_floor && (i1 > i2) && (e1 > e2))
	{
		command_see = FALSE; // Cancel creature_ptr->command_see
		oops = TRUE; // Oops
		done = TRUE; // Done

		if(select_the_force) {
		    *cp = INVEN_FORCE;
		    item = TRUE;
		}
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if(command_see && command_wrk && equip)
		{
			command_wrk = TRUE;
		}

		/* Use creature_ptr->inventory if allowed */
		else if(inven)
		{
			command_wrk = FALSE;
		}

		/* Use equipment if allowed */
		else if(equip)
		{
			command_wrk = TRUE;
		}

		/* Use creature_ptr->inventory for floor */
		else
		{
			command_wrk = FALSE;
		}
	}

	if((always_show_list == TRUE) || use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if(command_see)
	{
		/* Save screen */
		screen_save();
	}

	/* Repeat until done */
	while (!done)
	{
		int get_item_label = 0;

		/* Show choices */
		int ni = 0;
		int ne = 0;

		/* Scan windows */
		for (j = 0; j < 8; j++)
		{
			/* Unused */
			if(!angband_term[j]) continue;

			/* Count windows displaying inven */
			if(window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if(window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if((command_wrk && ni && !ne) ||
		    (!command_wrk && !ni && ne))
		{
			/* Toggle */
			toggle_inven_equip();

			/* Track toggles */
			toggle = !toggle;
		}

		/* Update */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Redraw windows */
		window_stuff(player_ptr);


		/* inventory screen */
		if(!command_wrk)
		{
			/* Redraw if needed */
			if(command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_INVENTORY, hook);
		}

		/* Equipment screen */
		else
		{
			/* Redraw if needed */
			if(command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_EQUIPMENT, hook);
		}

		/* Viewing creature_ptr->inventory */
		if(!command_wrk)
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "持ち物:");
#else
			sprintf(out_val, "Inven:");
#endif

			/* Some legal items */
			if((i1 <= i2) && !use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif
					index_to_label(i2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if(!command_see && !use_menu) strcat(out_val, " '*'一覧,");
#else
			if(!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
#ifdef JP
			if(equip) strcat(out_val, format(" %s 装備品,", use_menu ? "'4'or'6'" : "'/'"));
#else
			if(equip) strcat(out_val, format(" %s for Equip,", use_menu ? "4 or 6" : "/"));
#endif
		}

		/* Viewing equipment */
		else
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "装備品:");
#else
			sprintf(out_val, "Equip:");
#endif

			/* Some legal items */
			if((e1 <= e2) && !use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif
					index_to_label(e2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if(!command_see && !use_menu) strcat(out_val, " '*'一覧,");
#else
			if(!command_see) strcat(out_val, " * to see,");
#endif

			/* Append */
#ifdef JP
			if(inven) strcat(out_val, format(" %s 持ち物,", use_menu ? "'4'or'6'" : "'/'"));
#else
			if(inven) strcat(out_val, format(" %s for Inven,", use_menu ? "4 or 6" : "'/'"));
#endif
		}

		// Indicate legality of the "floor" item
#ifdef JP
		if(allow_floor) strcat(out_val, " '-'床上,");
		if(select_the_force) strcat(out_val, " 'w'練気術,");
#else
		if(allow_floor) strcat(out_val, " - for floor,");
		if(select_the_force) strcat(out_val, " w for the Force,");
#endif

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);

		/* Get a key */
		which = inkey();

		if(use_menu)
		{
		int max_line = (command_wrk ? max_equip : max_inven);
		switch (which)
		{
			case ESCAPE:
			case 'z':
			case 'Z':
			case '0':
			{
				done = TRUE;
				break;
			}

			case '8':
			case 'k':
			case 'K':
			{
				menu_line += (max_line - 1);
				break;
			}

			case '2':
			case 'j':
			case 'J':
			{
				menu_line++;
				break;
			}

			case '4':
			case '6':
			case 'h':
			case 'H':
			case 'l':
			case 'L':
			{
				/* Verify legality */
				if(!inven || !equip)
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				command_wrk = !command_wrk;
				max_line = (command_wrk ? max_equip : max_inven);
				if(menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if(command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if(!get_item_okay(creature_ptr, get_item_label, hook, item_tester_tval))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if(!get_item_allow(creature_ptr, get_item_label))
					{
						done = TRUE;
						break;
					}

					/* Accept that choice */
					(*cp) = get_item_label;
				}

				item = TRUE;
				done = TRUE;
				break;
			}
			case 'w':
			{
				if(select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}
			}
		}
		if(menu_line > max_line) menu_line -= max_line;
		}
		else
		{
		/* Parse it */
		switch (which)
		{
			case ESCAPE:
			{
				done = TRUE;
				break;
			}

			case '*':
			case '?':
			case ' ':
			{
				/* Hide the list */
				if(command_see)
				{
					/* Flip flag */
					command_see = FALSE;

					/* Load screen */
					screen_load();
				}

				/* Show the list */
				else
				{
					/* Save screen */
					screen_save();

					/* Flip flag */
					command_see = TRUE;
				}
				break;
			}

			case '/':
			{
				/* Verify legality */
				if(!inven || !equip)
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				command_wrk = !command_wrk;

				/* Need to redraw */
				break;
			}

			case '-':
			{
				/* Use floor item */
				if(allow_floor)
				{
					/* Scan all objects in the grid */
					for (this_object_idx = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx; this_object_idx; this_object_idx = next_object_idx)
					{
						object_type *object_ptr;

						/* Acquire object */
						object_ptr = &object_list[this_object_idx];

						/* Acquire next object */
						next_object_idx = object_ptr->next_object_idx;

						/* Validate the item */
						if(!item_tester_okay(creature_ptr, object_ptr, hook, item_tester_tval)) continue;

						/* Special index */
						k = 0 - this_object_idx;

						/* Verify the item (if required) */
#ifdef JP
if(other_query_flag && !verify(creature_ptr, "本当に", k)) continue;
#else
						if(other_query_flag && !verify("Try", k)) continue;
#endif


						/* Allow player to "refuse" certain actions */
						if(!get_item_allow(creature_ptr, k)) continue;

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
						break;
					}

					/* Outer break */
					if(done) break;
				}

				/* Oops */
				bell();
				break;
			}

			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				/* Look up the tag */
				if(!get_tag(creature_ptr, &k, which, command_wrk ? USE_EQUIP : USE_INVEN))
				{
					bell();
					break;
				}

				/* Hack -- Validate the item */
				if(!!IS_EQUIPPED(&creature_ptr->inventory[k]) ? !inven : !equip)
				{
					bell();
					break;
				}

				/* Validate the item */
				if(!get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if(!get_item_allow(creature_ptr, k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				cur_tag = which;
				break;
			}

			case 'w':
			{
				if(select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}

				/* Fall through */
			}

			default:
			{
				int ver;
				bool not_found = FALSE;

				/* Look up the alphabetical tag */
				if(!get_tag(creature_ptr, &k, which, command_wrk ? USE_EQUIP : USE_INVEN))
				{
					not_found = TRUE;
				}

				/* Hack -- Validate the item */
				else if(!IS_EQUIPPED(&creature_ptr->inventory[k]) ? !inven : !equip)
				{
					not_found = TRUE;
				}

				/* Validate the item */
				else if(!get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					not_found = TRUE;
				}

				if(!not_found)
				{
					/* Accept that choice */
					(*cp) = k;
					item = TRUE;
					done = TRUE;
					cur_tag = which;
					break;
				}

				/* Extract "query" setting */
				ver = isupper(which);
				which = tolower(which);

				/* Convert letter to creature_ptr->inventory index */
				if(!command_wrk)
				{
					if(which == '(') k = i1;
					else if(which == ')') k = i2;
					else k = label_to_item(creature_ptr, which);
				}

				/* Convert letter to equipment index */
				else
				{
					if(which == '(') k = e1;
					else if(which == ')') k = e2;
					else k = label_to_item(creature_ptr, which);
				}

				/* Validate the item */
				if(!get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					bell();
					break;
				}

				/* Verify the item */
#ifdef JP
				if(ver && !verify(creature_ptr, "本当に", k))
#else
				if(ver && !verify("Try", k))
#endif

				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if(!get_item_allow(creature_ptr, k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
		}
		}
	}


	/* Fix the screen if necessary */
	if(command_see)
	{
		/* Load screen */
		screen_load();

		/* Hack -- Cancel "display" */
		command_see = FALSE;
	}


	/* Forget the item_tester_tval restriction */
	item_tester_tval = 0;

	/* Forget the hook restriction */
	hook = NULL;


	/* Clean up  'show choices' */
	/* Toggle again if needed */
	if(toggle) toggle_inven_equip();

	/* Update */
	play_window |= (PW_INVEN | PW_EQUIP);

	/* Window stuff */
	window_stuff(player_ptr);


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if(oops && str) msg_print(str);

	if(item)
	{
		repeat_push(*cp);
		if(command_cmd) prev_tag = cur_tag;

		command_cmd = 0; /* Hack -- command_cmd is no longer effective */
	}

	/* Result */
	return (item);
}

/*
 * scan_floor --
 *
 * Return a list of object_list[] indexes of items at the given cave
 * location. Valid flags are:
 *
 *		mode & 0x01 -- Item tester
 *		mode & 0x02 -- Marked items only
 *		mode & 0x04 -- Stop after first
 */
int scan_floor(int *items, floor_type *floor_ptr, int y, int x, int mode)
{
	int this_object_idx, next_object_idx;
	int num = 0;

	if(!in_bounds(floor_ptr, y, x)) return 0; // Sanity

	/* Scan all objects in the grid */
	for (this_object_idx = floor_ptr->cave[y][x].object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;

		/* Acquire object */
		object_ptr = &object_list[this_object_idx];

		/* Acquire next object */
		next_object_idx = object_ptr->next_object_idx;

		/* Item tester */
		if((mode & 0x01) && !item_tester_okay(player_ptr, object_ptr, NULL, 0)) continue;

		/* Marked */
		if((mode & 0x02) && !(object_ptr->marked & OM_FOUND)) continue;

		/* Accept this item */
		/* XXX Hack -- Enforce limit */
		if(num < 23)
			items[num] = this_object_idx;

		num++;

		/* Only one */
		if(mode & 0x04) break;
	}

	/* Result */
	return num;
}


/*
 * Display a list of the items on the floor at the given location.
 */
int show_floor(floor_type *floor_ptr, int target_item, int y, int x, int *min_width)
{
	int i, j, k, l;
	int col, len;

	object_type *object_ptr;

	char object_name[MAX_NLEN];

	char tmp_val[80];

	int out_index[23];
	byte out_color[23];
	char out_desc[23][MAX_NLEN];
	int target_item_label = 0;

	int floor_list[23], floor_num;
	int wid, hgt;
	char floor_label[52 + 1];

	bool dont_need_to_show_weights = TRUE;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Default length */
	len = MAX((*min_width), 20);

	/* Scan for objects in the grid, using item_tester_okay() */
	floor_num = scan_floor(floor_list, floor_ptr, y, x, 0x03);

	/* Display the floor objects */
	for (k = 0, i = 0; i < floor_num && i < 23; i++)
	{
		object_ptr = &object_list[floor_list[i]];

		/* Describe the object */
		object_desc(object_name, object_ptr, 0);

		/* Save the index */
		out_index[k] = i;

		/* Acquire inventory color */
		out_color[k] = tval_to_acttr[object_ptr->tval & 0x7F];

		/* Save the object description */
		strcpy(out_desc[k], object_name);

		/* Find the predicted "line length" */
		l = strlen(out_desc[k]) + 5;

		/* Be sure to account for the weight */
		if(show_weights) l += 9;

		if(object_ptr->tval != TV_GOLD) dont_need_to_show_weights = FALSE;

		/* Maintain the maximum length */
		if(l > len) len = l;

		/* Advance to next "line" */
		k++;
	}

	if(show_weights && dont_need_to_show_weights) len -= 9;

	/* Save width */
	*min_width = len;

	/* Find the column to start in */
	col = (len > wid - 4) ? 0 : (wid - len - 1);

	prepare_label_string_floor(floor_label, floor_list, floor_num);

	/* Output each entry */
	for (j = 0; j < k; j++)
	{
		/* Get the index */
		i = floor_list[out_index[j]];

		/* Get the item */
		object_ptr = &object_list[i];

		/* Clear the line */
		prt("", j + 1, col ? col - 2 : col);

		if(use_menu && target_item)
		{
			if(j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "》");
#else
				strcpy(tmp_val, "> ");
#endif
				target_item_label = i;
			}
			else strcpy(tmp_val, "   ");
		}
		else
		{
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", floor_label[j]);
		}

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j + 1, col);

		/* Display the entry itself */
		c_put_str(out_color[j], out_desc[j], j + 1, col + 3);

		/* Display the weight if needed */
		if(show_weights && (object_ptr->tval != TV_GOLD))
		{
			char weight[80];
			int wgt = object_ptr->weight * object_ptr->number;
			format_weight(weight, wgt);
			sprintf(tmp_val, "%10s", weight);

			prt(tmp_val, j + 1, wid - 10);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if(j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

	return target_item_label;
}

// This version of get_item() is called by get_item() when
// the easy_floor is on.
bool get_item_floor(creature_type *creature_ptr, int *cp, cptr pmt, cptr str, int mode, bool (*hook)(creature_type *creature_ptr, object_type *object_ptr), int item_tester_tval)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	char n1 = ' ', n2 = ' ', which = ' ';
	int j, k, i1, i2, e1, e2;
	bool done, item;
	bool oops = FALSE;

	// Extract args
	bool equip = (mode & USE_EQUIP) ? TRUE : FALSE;
	bool inven = (mode & USE_INVEN) ? TRUE : FALSE;
	bool floor = (mode & USE_FLOOR) ? TRUE : FALSE;
	bool select_the_force = (mode & USE_FORCE) ? TRUE : FALSE;

	bool allow_equip = FALSE;
	bool allow_inven = FALSE;
	bool allow_floor = FALSE;

	bool toggle = FALSE;

	char tmp_val[160];
	char out_val[160];

	int floor_num, floor_list[23], floor_top = 0;
	int min_width = 0;

	int menu_line = (use_menu ? 1 : 0);
	int max_inven = 0;
	int max_equip = 0;

	static char prev_tag = '\0';
	char cur_tag = '\0';

	/* Get the item index */
	if(repeat_pull(cp))
	{
		/* the_force */
		if(select_the_force && (*cp == INVEN_FORCE))
		{
			item_tester_tval = 0;
			hook = NULL;
			command_cmd = 0; /* Hack -- command_cmd is no longer effective */
			return (TRUE);
		}

		/* Floor item? */
		else if(floor && (*cp < 0))
		{
			if(prev_tag && command_cmd)
			{
				/* Scan all objects in the grid */
				floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);

				/* Look up the tag */
				if(get_tag_floor(&k, prev_tag, floor_list, floor_num))
				{
					/* Accept that choice */
					(*cp) = 0 - floor_list[k];

					/* Forget restrictions */
					item_tester_tval = 0;
					hook = NULL;
					command_cmd = 0; /* Hack -- command_cmd is no longer effective */

					/* Success */
					return TRUE;
				}

				prev_tag = '\0'; /* prev_tag is no longer effective */
			}

			/* Validate the item */
			else if(item_tester_okay(creature_ptr, &object_list[0 - (*cp)], hook, item_tester_tval))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}

		else if(*cp >= 0 && *cp < INVEN_TOTAL)
		{
			object_type *object_ptr = &creature_ptr->inventory[*cp];
			if(prev_tag && command_cmd)
			{
				/* Look up the tag and validate the item */
				if(!get_tag(creature_ptr, &k, prev_tag, IS_EQUIPPED(object_ptr) ? USE_EQUIP : USE_INVEN)) /* Reject */;
				else if(!IS_EQUIPPED(object_ptr) ? !inven : !equip) /* Reject */;
				else if(!get_item_okay(creature_ptr, k, hook, item_tester_tval)) /* Reject */;
				else
				{
					/* Accept that choice */
					(*cp) = k;

					/* Forget restrictions */
					item_tester_tval = 0;
					hook = NULL;
					command_cmd = 0; /* Hack -- command_cmd is no longer effective */

					/* Success */
					return TRUE;
				}

				prev_tag = '\0'; /* prev_tag is no longer effective */
			}

			/* Verify the item */
			else if(get_item_okay(creature_ptr, *cp, hook, item_tester_tval))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}
	}

	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Not done */
	done = FALSE;

	/* No item selected */
	item = FALSE;


	/* Full creature_ptr->inventory */
	i1 = 0;
	i2 = INVEN_TOTAL - 1;

	/* Forbid creature_ptr->inventory */
	if(!inven) i2 = -1;
	else if(use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
			if(item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_inven++;
	}

	/* Restrict creature_ptr->inventory indexes */
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i1, hook, item_tester_tval))) i1++;
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i2, hook, item_tester_tval))) i2--;


	/* Full equipment */
	e1 = 0;
	e2 = INVEN_TOTAL - 1;

	/* Forbid equipment */
	if(!equip) e2 = -1;
	else if(use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
		{
			if(!IS_EQUIPPED(&creature_ptr->inventory[j])) continue; // Skip no equipment
			if(item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_equip++;
		}
	}

	/* Restrict equipment indexes */
	while ((e1 <= e2) && (!get_item_okay(creature_ptr, e1, hook, item_tester_tval))) e1++;
	while ((e1 <= e2) && (!get_item_okay(creature_ptr, e2, hook, item_tester_tval))) e2--;

	/*
	if(equip && creature_ptr->two_handed && !item_tester_no_two_handed)
	{
		if(creature_ptr->can_melee[0])
		{
			if(e2 < INVEN_2) e2 = INVEN_2;
		}
		else if(creature_ptr->can_melee[1]) e1 = INVEN_1;
	}
	*/


	/* Count "okay" floor items */
	floor_num = 0;

	/* Restrict floor usage */
	if(floor)
	{
		/* Scan all objects in the grid */
		floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);
	}

	/* Accept inventory */
	if(i1 <= i2) allow_inven = TRUE;

	/* Accept equipment */
	if(e1 <= e2) allow_equip = TRUE;

	/* Accept floor */
	if(floor_num) allow_floor = TRUE;

	/* Require at least one legal choice */
	if(!allow_inven && !allow_equip && !allow_floor)
	{
		/* Cancel creature_ptr->command_see */
		command_see = FALSE;

		/* Oops */
		oops = TRUE;

		/* Done */
		done = TRUE;

		if(select_the_force) {
		    *cp = INVEN_FORCE;
		    item = TRUE;
		}
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if(command_see && (command_wrk == (USE_EQUIP))
			&& allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use creature_ptr->inventory if allowed */
		else if(allow_inven)
		{
			command_wrk = (USE_INVEN);
		}

		/* Use equipment if allowed */
		else if(allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use floor if allowed */
		else if(allow_floor)
		{
			command_wrk = (USE_FLOOR);
		}
	}

	/*
	 * 追加オプション(always_show_list)が設定されている場合は常に一覧を表示する
	 */
	if((always_show_list == TRUE) || use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if(command_see)
	{
		/* Save screen */
		screen_save();
	}

	/* Repeat until done */
	while (!done)
	{
		int get_item_label = 0;

		/* Show choices */
		int ni = 0;
		int ne = 0;

		/* Scan windows */
		for (j = 0; j < 8; j++)
		{
			/* Unused */
			if(!angband_term[j]) continue;

			/* Count windows displaying inven */
			if(window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if(window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if((command_wrk == (USE_EQUIP) && ni && !ne) ||
		    (command_wrk == (USE_INVEN) && !ni && ne))
		{
			/* Toggle */
			toggle_inven_equip();

			/* Track toggles */
			toggle = !toggle;
		}

		/* Update */
		play_window |= (PW_INVEN | PW_EQUIP);

		/* Redraw windows */
		window_stuff(player_ptr);

		/* creature_ptr->inventory screen */
		if(command_wrk == (USE_INVEN))
		{
			/* Extract the legal requests */
			n1 = I2A(i1);
			n2 = I2A(i2);

			/* Redraw if needed */
			if(command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_INVENTORY, hook);
		}

		/* Equipment screen */
		else if(command_wrk == (USE_EQUIP))
		{
			/* Extract the legal requests */
			n1 = I2A(e1);
			n2 = I2A(e2);

			/* Redraw if needed */
			if(command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_EQUIPMENT, hook);
		}

		/* Floor screen */
		else if(command_wrk == (USE_FLOOR))
		{
			j = floor_top;
			k = MIN(floor_top + 23, floor_num) - 1;

			/* Extract the legal requests */
			n1 = I2A(j - floor_top);
			n2 = I2A(k - floor_top);

			/* Redraw if needed */
			if(command_see) get_item_label = show_floor(floor_ptr, menu_line, creature_ptr->fy, creature_ptr->fx, &min_width);
		}

		/* Viewing creature_ptr->inventory */
		if(command_wrk == (USE_INVEN))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "持ち物:");
#else
			sprintf(out_val, "Inven:");
#endif

			if(!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif
					index_to_label(i2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if(!command_see && !use_menu) strcat(out_val, " '*'一覧,");
#else
			if(!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
			if(allow_equip)
			{
#ifdef JP
				if(!use_menu)
					strcat(out_val, " '/' 装備品,");
				else if(allow_floor)
					strcat(out_val, " '6' 装備品,");
				else
					strcat(out_val, " '4'or'6' 装備品,");
#else
				if(!use_menu)
					strcat(out_val, " / for Equip,");
				else if(allow_floor)
					strcat(out_val, " 6 for Equip,");
				else
					strcat(out_val, " 4 or 6 for Equip,");
#endif
			}

			/* Append */
			if(allow_floor)
			{
#ifdef JP
				if(!use_menu)
					strcat(out_val, " '-'床上,");
				else if(allow_equip)
					strcat(out_val, " '4' 床上,");
				else
					strcat(out_val, " '4'or'6' 床上,");
#else
				if(!use_menu)
					strcat(out_val, " - for floor,");
				else if(allow_equip)
					strcat(out_val, " 4 for floor,");
				else
					strcat(out_val, " 4 or 6 for floor,");
#endif
			}
		}

		/* Viewing equipment */
		else if(command_wrk == (USE_EQUIP))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "装備品:");
#else
			sprintf(out_val, "Equip:");
#endif

			if(!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif
					index_to_label(e2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if(!command_see && !use_menu) strcat(out_val, " '*'一覧,");
#else
			if(!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
			if(allow_inven)
			{
#ifdef JP
				if(!use_menu)
					strcat(out_val, " '/' 持ち物,");
				else if(allow_floor)
					strcat(out_val, " '4' 持ち物,");
				else
					strcat(out_val, " '4'or'6' 持ち物,");
#else
				if(!use_menu)
					strcat(out_val, " / for Inven,");
				else if(allow_floor)
					strcat(out_val, " 4 for Inven,");
				else
					strcat(out_val, " 4 or 6 for Inven,");
#endif
			}

			/* Append */
			if(allow_floor)
			{
#ifdef JP
				if(!use_menu)
					strcat(out_val, " '-'床上,");
				else if(allow_inven)
					strcat(out_val, " '6' 床上,");
				else
					strcat(out_val, " '4'or'6' 床上,");
#else
				if(!use_menu)
					strcat(out_val, " - for floor,");
				else if(allow_inven)
					strcat(out_val, " 6 for floor,");
				else
					strcat(out_val, " 4 or 6 for floor,");
#endif
			}
		}

		/* Viewing floor */
		else if(command_wrk == (USE_FLOOR))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "床上:");
#else
			sprintf(out_val, "Floor:");
#endif

			if(!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',", n1, n2);
#else
				sprintf(tmp_val, " %c-%c,'(',')',", n1, n2);
#endif

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if(!command_see && !use_menu) strcat(out_val, " '*'一覧,");
#else
			if(!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			if(use_menu)
			{
				if(allow_inven && allow_equip)
				{
#ifdef JP
					strcat(out_val, " '4' 装備品, '6' 持ち物,");
#else
					strcat(out_val, " 4 for Equip, 6 for Inven,");
#endif
				}
				else if(allow_inven)
				{
#ifdef JP
					strcat(out_val, " '4'or'6' 持ち物,");
#else
					strcat(out_val, " 4 or 6 for Inven,");
#endif
				}
				else if(allow_equip)
				{
#ifdef JP
					strcat(out_val, " '4'or'6' 装備品,");
#else
					strcat(out_val, " 4 or 6 for Equip,");
#endif
				}
			}
			/* Append */
			else if(allow_inven)
			{
#ifdef JP
				strcat(out_val, " '/' 持ち物,");
#else
				strcat(out_val, " / for Inven,");
#endif
			}
			else if(allow_equip)
			{
#ifdef JP
				strcat(out_val, " '/'装備品,");
#else
				strcat(out_val, " / for Equip,");
#endif
			}

			/* Append */
			if(command_see && !use_menu)
			{
#ifdef JP
				strcat(out_val, " Enter 次,");
#else
				strcat(out_val, " Enter for scroll down,");
#endif
			}
		}

		/* Append */
#ifdef JP
		if(select_the_force) strcat(out_val, " 'w'練気術,");
#else
		if(select_the_force) strcat(out_val, " w for the Force,");
#endif

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);

		/* Get a key */
		which = inkey();

		if(use_menu)
		{
		int max_line = 1;
		if(command_wrk == USE_INVEN) max_line = max_inven;
		else if(command_wrk == USE_EQUIP) max_line = max_equip;
		else if(command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
		switch (which)
		{
			case ESCAPE:
			case 'z':
			case 'Z':
			case '0':
			{
				done = TRUE;
				break;
			}

			case '8':
			case 'k':
			case 'K':
			{
				menu_line += (max_line - 1);
				break;
			}

			case '2':
			case 'j':
			case 'J':
			{
				menu_line++;
				break;
			}

			case '4':
			case 'h':
			case 'H':
			{
				/* Verify legality */
				if(command_wrk == (USE_INVEN))
				{
					if(allow_floor) command_wrk = USE_FLOOR;
					else if(allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else if(command_wrk == (USE_EQUIP))
				{
					if(allow_inven) command_wrk = USE_INVEN;
					else if(allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if(command_wrk == (USE_FLOOR))
				{
					if(allow_equip) command_wrk = USE_EQUIP;
					else if(allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				if(command_wrk == USE_INVEN) max_line = max_inven;
				else if(command_wrk == USE_EQUIP) max_line = max_equip;
				else if(command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if(menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case '6':
			case 'l':
			case 'L':
			{
				/* Verify legality */
				if(command_wrk == (USE_INVEN))
				{
					if(allow_equip) command_wrk = USE_EQUIP;
					else if(allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if(command_wrk == (USE_EQUIP))
				{
					if(allow_floor) command_wrk = USE_FLOOR;
					else if(allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else if(command_wrk == (USE_FLOOR))
				{
					if(allow_inven) command_wrk = USE_INVEN;
					else if(allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				if(command_wrk == USE_INVEN) max_line = max_inven;
				else if(command_wrk == USE_EQUIP) max_line = max_equip;
				else if(command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if(menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if(command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if(!get_item_okay(creature_ptr, get_item_label, hook, item_tester_tval))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if(!get_item_allow(creature_ptr, get_item_label))
					{
						done = TRUE;
						break;
					}

					/* Accept that choice */
					(*cp) = get_item_label;
				}

				item = TRUE;
				done = TRUE;
				break;
			}
			case 'w':
			{
				if(select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}
			}
		}
		if(menu_line > max_line) menu_line -= max_line;
		}
		else
		{
		/* Parse it */
		switch (which)
		{
			case ESCAPE:
			{
				done = TRUE;
				break;
			}

			case '*':
			case '?':
			case ' ':
			{
				/* Hide the list */
				if(command_see)
				{
					/* Flip flag */
					command_see = FALSE;

					/* Load screen */
					screen_load();
				}

				/* Show the list */
				else
				{
					/* Save screen */
					screen_save();

					/* Flip flag */
					command_see = TRUE;
				}
				break;
			}

			case '\n':
			case '\r':
			case '+':
			{
				int i, object_idx;
				cave_type *c_ptr = &floor_ptr->cave[creature_ptr->fy][creature_ptr->fx];

				if(command_wrk != (USE_FLOOR)) break;

				/* Get the object being moved. */
				object_idx = c_ptr->object_idx;

				/* Only rotate a pile of two or more objects. */
				if(!(object_idx && object_list[object_idx].next_object_idx)) break;

				/* Remove the first object from the list. */
				excise_object_idx(object_idx);

				/* Find end of the list. */
				i = c_ptr->object_idx;
				while (object_list[i].next_object_idx)
					i = object_list[i].next_object_idx;

				/* Add after the last object. */
				object_list[i].next_object_idx = object_idx;

				/* Re-scan floor list */ 
				floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);

				/* Hack -- Fix screen */
				if(command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				break;
			}

			case '/':
			{
				if(command_wrk == (USE_INVEN))
				{
					if(!allow_equip)
					{
						bell();
						break;
					}
					command_wrk = (USE_EQUIP);
				}
				else if(command_wrk == (USE_EQUIP))
				{
					if(!allow_inven)
					{
						bell();
						break;
					}
					command_wrk = (USE_INVEN);
				}
				else if(command_wrk == (USE_FLOOR))
				{
					if(allow_inven)
					{
						command_wrk = (USE_INVEN);
					}
					else if(allow_equip)
					{
						command_wrk = (USE_EQUIP);
					}
					else
					{
						bell();
						break;
					}
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Need to redraw */
				break;
			}

			case '-':
			{
				if(!allow_floor)
				{
					bell();
					break;
				}

				/*
				 * If we are already examining the floor, and there
				 * is only one item, we will always select it.
				 * If we aren't examining the floor and there is only
				 * one item, we will select it if floor_query_flag
				 * is FALSE.
				 */
				if(floor_num == 1)
				{
					if((command_wrk == (USE_FLOOR)) || (!carry_query_flag))
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if(!get_item_allow(creature_ptr, k))
						{
							done = TRUE;
							break;
						}

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;

						break;
					}
				}

				/* Hack -- Fix screen */
				if(command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				command_wrk = (USE_FLOOR);

				break;
			}

			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				if(command_wrk != USE_FLOOR)
				{
					/* Look up the tag */
					if(!get_tag(creature_ptr, &k, which, command_wrk))
					{
						bell();
						break;
					}

					/* Validate the item */
					if(!get_item_okay(creature_ptr, k, hook, item_tester_tval))
					{
						bell();
						break;
					}
				}
				else
				{
					/* Look up the alphabetical tag */
					if(get_tag_floor(&k, which, floor_list, floor_num))
					{
						/* Special index */
						k = 0 - floor_list[k];
					}
					else
					{
						bell();
						break;
					}
				}

				/* Allow player to "refuse" certain actions */
				if(!get_item_allow(creature_ptr, k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				cur_tag = which;
				break;
			}

#if 0
			case '\n':
			case '\r':
			{
				/* Choose "default" creature_ptr->inventory item */
				if(command_wrk == (USE_INVEN))
				{
					k = ((i1 == i2) ? i1 : -1);
				}

				/* Choose "default" equipment item */
				else if(command_wrk == (USE_EQUIP))
				{
					k = ((e1 == e2) ? e1 : -1);
				}

				/* Choose "default" floor item */
				else if(command_wrk == (USE_FLOOR))
				{
					if(floor_num == 1)
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if(!get_item_allow(creature_ptr, k))
						{
							done = TRUE;
							break;
						}

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
					}
					break;
				}

				/* Validate the item */
				if(!get_item_okay(creature_ptr, k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if(!get_item_allow(creature_ptr, k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
#endif

			case 'w':
			{
				if(select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}

				/* Fall through */
			}

			default:
			{
				int ver;

				if(command_wrk != USE_FLOOR)
				{
					bool not_found = FALSE;

					/* Look up the alphabetical tag */
					if(!get_tag(creature_ptr, &k, which, command_wrk))
					{
						not_found = TRUE;
					}

					/* Hack -- Validate the item */
					else if(!!IS_EQUIPPED(&creature_ptr->inventory[k]) ? !inven : !equip)
					{
						not_found = TRUE;
					}

					/* Validate the item */
					else if(!get_item_okay(creature_ptr, k, hook, item_tester_tval))
					{
						not_found = TRUE;
					}

					if(!not_found)
					{
						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
						cur_tag = which;
						break;
					}
				}
				else
				{
					/* Look up the alphabetical tag */
					if(get_tag_floor(&k, which, floor_list, floor_num))
					{
						/* Special index */
						k = 0 - floor_list[k];

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
						cur_tag = which;
						break;
					}
				}

				/* Extract "query" setting */
				ver = isupper(which);
				which = tolower(which);

				/* Convert letter to creature_ptr->inventory index */
				if(command_wrk == (USE_INVEN) || command_wrk == (USE_EQUIP))
				{
					if(which == '(') k = i1;
					else if(which == ')') k = i2;
					else k = label_to_item(creature_ptr, which);
				}

				/* Convert letter to floor index */
				else if(command_wrk == USE_FLOOR)
				{
					if(which == '(') k = 0;
					else if(which == ')') k = floor_num - 1;
					else k = islower(which) ? A2I(which) : -1;
					if(k < 0 || k >= floor_num || k >= 23)
					{
						bell();
						break;
					}

					/* Special index */
					k = 0 - floor_list[k];
				}

				/* Validate the item */
				if((command_wrk != USE_FLOOR) && !get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					bell();
					break;
				}

				/* Verify the item */
#ifdef JP
				if(ver && !verify(creature_ptr, "本当に", k))
#else
				if(ver && !verify("Try", k))
#endif

				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if(!get_item_allow(creature_ptr, k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
		}
		}
	}

	/* Fix the screen if necessary */
	if(command_see)
	{
		/* Load screen */
		screen_load();

		/* Hack -- Cancel "display" */
		command_see = FALSE;
	}


	/* Forget the item_tester_tval restriction */
	item_tester_tval = 0;

	/* Forget the hook restriction */
	hook = NULL;


	/* Clean up  'show choices' */
	/* Toggle again if needed */
	if(toggle) toggle_inven_equip();

	/* Update */
	play_window |= (PW_INVEN | PW_EQUIP);

	/* Window stuff */
	window_stuff(player_ptr);


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if(oops && str) msg_print(str);

	if(item)
	{
		repeat_push(*cp);
		if(command_cmd) prev_tag = cur_tag;
		command_cmd = 0; /* Hack -- command_cmd is no longer effective */
	}

	/* Result */
	return (item);
}


static bool py_pickup_floor_aux(creature_type *creature_ptr)
{
	s16b this_object_idx;

	cptr q, s;

	int item;

	/* Get an object */
#ifdef JP
	q = "どれを拾いますか？";
	s = "もうザックには床にあるどのアイテムも入らない。";
#else
	q = "Get which item? ";
	s = "You no longer have any room for the objects on the floor.";
#endif

	if(get_item(creature_ptr, &item, q, s, (USE_FLOOR), inven_carry_okay, 0))
	{
		this_object_idx = 0 - item;
	}
	else
	{
		return (FALSE);
	}

	// Pick up the object
	py_pickup_aux(creature_ptr, this_object_idx);

	return (TRUE);
}


/*
 * Make the player carry everything in a grid
 *
 * If "pickup" is FALSE then only gold will be picked up
 *
 * This is called by py_pickup() when easy_floor is TRUE.
 */
void py_pickup_floor(creature_type *creature_ptr, bool pickup)
{
	s16b this_object_idx, next_object_idx = 0;

	char object_name[MAX_NLEN];
	object_type *object_ptr;

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int floor_num = 0, floor_list[23], floor_object_idx = 0;

	int can_pickup = 0;

	/* Scan the pile of objects */
	for (this_object_idx = floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;

		/* Access the object */
		object_ptr = &object_list[this_object_idx];

		/* Describe the object */
		object_desc(object_name, object_ptr, 0);

		/* Access the next object */
		next_object_idx = object_ptr->next_object_idx;

		/* Hack -- disturb */
		disturb(player_ptr, 0, 0);

		/* Pick up gold */
		if(object_ptr->tval == TV_GOLD)
		{
			/* Message */
#ifdef JP
		msg_format(" $%ld の価値がある%sを見つけた。",
			   (long)object_ptr->pval, object_name);
#else
			msg_format("You have found %ld gold pieces worth of %s.",
				(long) object_ptr->pval, object_name);
#endif


			/* Collect the gold */
			creature_ptr->au += object_ptr->pval;

			/* Redraw gold */
			play_redraw |= (PR_GOLD);

			/* Window stuff */
			play_window |= (PW_PLAYER);

			/* Delete the gold */
			delete_object_idx(this_object_idx);

			/* Check the next object */
			continue;
		}
		else if(object_ptr->marked & OM_NOMSG)
		{
			/* If 0 or 1 non-NOMSG items are in the pile, the NOMSG ones are
			 * ignored. Otherwise, they are included in the prompt. */
			object_ptr->marked &= ~(OM_NOMSG);
			continue;
		}

		/* Count non-gold objects that can be picked up. */
		if(inven_carry_okay(creature_ptr, object_ptr))
		{
			can_pickup++;
		}

		/* Remember this object index */
		if(floor_num < 23)
			floor_list[floor_num] = this_object_idx;

		/* Count non-gold objects */
		floor_num++;

		/* Remember this index */
		floor_object_idx = this_object_idx;
	}

	/* There are no non-gold objects */
	if(!floor_num)
		return;

	/* Mention the number of objects */
	if(!pickup)
	{
		/* One object */
		if(floor_num == 1)
		{
			/* Access the object */
			object_ptr = &object_list[floor_object_idx];

			/* Describe the object */
			object_desc(object_name, object_ptr, 0);

			/* Message */
#ifdef JP
			msg_format("%sがある。", object_name);
#else
			msg_format("You see %s.", object_name);
#endif

		}

		/* Multiple objects */
		else
		{
			/* Message */
#ifdef JP
			msg_format("%d 種のアイテムがある。", floor_num);
#else
			msg_format("You see a pile of %d items.", floor_num);
#endif

		}

		/* Done */
		return;
	}

	/* The player has no room for anything on the floor. */
	if(!can_pickup)
	{
		/* One object */
		if(floor_num == 1)
		{
			/* Access the object */
			object_ptr = &object_list[floor_object_idx];

			/* Describe the object */
			object_desc(object_name, object_ptr, 0);

			/* Message */
#ifdef JP
				msg_format("ザックには%sを入れる隙間がない。", object_name);
#else
			msg_format("You have no room for %s.", object_name);
#endif

		}

		/* Multiple objects */
		else
		{
			/* Message */
#ifdef JP
			msg_format("ザックには床にあるどのアイテムも入らない。", object_name);
#else
			msg_print("You have no room for any of the objects on the floor.");
#endif

		}

		/* Done */
		return;
	}

	/* One object */
	if(floor_num == 1)
	{
		/* Hack -- query every object */
		if(carry_query_flag)
		{
			char out_val[MAX_NLEN+20];

			/* Access the object */
			object_ptr = &object_list[floor_object_idx];

			/* Describe the object */
			object_desc(object_name, object_ptr, 0);

			/* Build a prompt */
#ifdef JP
			(void) sprintf(out_val, "%sを拾いますか? ", object_name);
#else
			(void) sprintf(out_val, "Pick up %s? ", object_name);
#endif


			/* Ask the user to confirm */
			if(!get_check(out_val))
			{
				/* Done */
				return;
			}
		}

		/* Access the object */
		object_ptr = &object_list[floor_object_idx];

		/* Pick up the object */
		py_pickup_aux(creature_ptr, floor_object_idx);
	}

	/* Allow the user to choose an object */
	else
	{
		while (can_pickup--)
		{
			if(!py_pickup_floor_aux(creature_ptr)) break;
		}
	}
}
