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

	if (use_graphics)
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
void object_flags(object_type *object_ptr, u32b flgs[TR_FLAG_SIZE])
{
	object_kind *k_ptr = &object_kind_info[object_ptr->k_idx];
	int i;

	/* Base object */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = k_ptr->flags[i];

	/* Artifact */
	if (object_is_fixed_artifact(object_ptr))
	{
		artifact_type *a_ptr = &artifact_info[object_ptr->name1];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] = a_ptr->flags[i];
	}

	/* Ego-item */
	if (object_is_ego(object_ptr))
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= e_ptr->flags[i];

		if ((object_ptr->name2 == EGO_LITE_AURA_FIRE) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if ((object_ptr->name2 == EGO_LITE_INFRA) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TR_INFRA);
		}
		else if ((object_ptr->name2 == EGO_LITE_EYE) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_NO_BLIND);
			remove_flag(flgs, TRAIT_SEE_INVISIBLE);
		}
	}

	/* Random artifact ! */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] |= object_ptr->art_flags[i];

	if (object_is_smith(object_ptr))
	{
		int add = object_ptr->xtra3 - 1;

		if (add < TR_FLAG_MAX)
		{
			add_flag(flgs, add);
		}
		else if (add == ESSENCE_TMP_RES_ACID)
		{
			add_flag(flgs, TRAIT_RES_ACID);
		}
		else if (add == ESSENCE_TMP_RES_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
		}
		else if (add == ESSENCE_TMP_RES_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
		}
		else if (add == ESSENCE_TMP_RES_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
		}
		else if (add == ESSENCE_SH_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if (add == ESSENCE_SH_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_AURA_ELEC);
		}
		else if (add == ESSENCE_SH_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
			add_flag(flgs, TRAIT_AURA_COLD);
		}
		else if (add == ESSENCE_RESISTANCE)
		{
			add_flag(flgs, TRAIT_RES_ACID);
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_RES_COLD);
		}
		else if (add == TRAIT_SHATTER)
		{
			//TODO:TR_ACTIVATE add_flag(flgs, TR_ACTIVATE);
		}
	}
}



/*
 * Obtain the "flags" for an item which are known to the player
 */
void object_flags_known(object_type *object_ptr, u32b flgs[TR_FLAG_SIZE])
{
	bool spoil = FALSE;
	int i;

	object_kind *k_ptr = &object_kind_info[object_ptr->k_idx];

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0;

	if (!object_is_aware(object_ptr)) return;

	/* Base object */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = k_ptr->flags[i];

	/* Must be identified */
	if (!object_is_known(object_ptr)) return;

	/* Ego-item (known basic flags) */
	if (object_is_ego(object_ptr))
	{
		ego_item_type *e_ptr = &object_ego_info[object_ptr->name2];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= e_ptr->flags[i];

		if ((object_ptr->name2 == EGO_LITE_AURA_FIRE) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if ((object_ptr->name2 == EGO_LITE_INFRA) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TR_INFRA);
		}
		else if ((object_ptr->name2 == EGO_LITE_EYE) && !object_ptr->xtra4 && (object_ptr->sval <= SV_LITE_LANTERN))
		{
			remove_flag(flgs, TRAIT_NO_BLIND);
			remove_flag(flgs, TRAIT_SEE_INVISIBLE);
		}
	}


#ifdef SPOIL_ARTIFACTS
	/* Full knowledge for some artifacts */
	if (object_is_artifact(object_ptr)) spoil = TRUE;
#endif /* SPOIL_ARTIFACTS */

#ifdef SPOIL_EGO_ITEMS
	/* Full knowledge for some ego-items */
	if (object_is_ego(object_ptr)) spoil = TRUE;
#endif /* SPOIL_EGO_ITEMS */

	/* Need full knowledge or spoilers */
	if (spoil || (object_ptr->ident & IDENT_MENTAL))
	{
		/* Artifact */
		if (object_is_fixed_artifact(object_ptr))
		{
			artifact_type *a_ptr = &artifact_info[object_ptr->name1];

			for (i = 0; i < TR_FLAG_SIZE; i++)
				flgs[i] = a_ptr->flags[i];
		}

		/* Random artifact ! */
		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= object_ptr->art_flags[i];
	}

	if (object_is_smith(object_ptr))
	{
		int add = object_ptr->xtra3 - 1;

		if (add < TR_FLAG_MAX)
		{
			add_flag(flgs, add);
		}
		else if (add == ESSENCE_TMP_RES_ACID)
		{
			add_flag(flgs, TRAIT_RES_ACID);
		}
		else if (add == ESSENCE_TMP_RES_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
		}
		else if (add == ESSENCE_TMP_RES_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
		}
		else if (add == ESSENCE_TMP_RES_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
		}
		else if (add == ESSENCE_SH_FIRE)
		{
			add_flag(flgs, TRAIT_RES_FIRE);
			add_flag(flgs, TRAIT_AURA_FIRE);
		}
		else if (add == ESSENCE_SH_ELEC)
		{
			add_flag(flgs, TRAIT_RES_ELEC);
			add_flag(flgs, TRAIT_AURA_ELEC);
		}
		else if (add == ESSENCE_SH_COLD)
		{
			add_flag(flgs, TRAIT_RES_COLD);
			add_flag(flgs, TRAIT_AURA_COLD);
		}
		else if (add == ESSENCE_RESISTANCE)
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
	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(object_ptr, flgs);

	/* Require activation ability */

	//TODO not TR_ACTIVATE
	return "None";



	/*
	 * We need to deduce somehow that it is a random artifact -- one
	 * problem: It could be a random artifact which has NOT YET received
	 * a name. Thus we eliminate other possibilities instead of checking
	 * for art_name
	 */

	if (!object_is_fixed_artifact(object_ptr) &&
	    !object_is_ego(object_ptr) &&
	    !(object_ptr->xtra1) &&
	    (object_ptr->xtra2))
	{
		switch (object_ptr->xtra2)
		{
			case ACT_SUNLIGHT:
			{
#ifdef JP
return "���z���� : 10 �^�[����";
#else
				return "beam of sunlight every 10 turns";
#endif

			}
			case ACT_BO_MISS_1:
			{
#ifdef JP
return "�}�W�b�N�E�~�T�C��(2d6) : 2 �^�[����";
#else
				return "magic missile (2d6) every 2 turns";
#endif

			}
			case ACT_BA_POIS_1:
			{
#ifdef JP
return "���L�_ (12), ���a 3 , 4+d4 �^�[����";
#else
				return "stinking cloud (12), rad. 3, every 4+d4 turns";
#endif

			}
			case ACT_BO_ELEC_1:
			{
#ifdef JP
return "�T���_�[�E�{���g(4d8) : 5+d5 �^�[����";
#else
				return "lightning bolt (4d8) every 5+d5 turns";
#endif

			}
			case ACT_BO_ACID_1:
			{
#ifdef JP
return "�A�V�b�h�E�{���g(5d8) : 6+d6 �^�[����";
#else
				return "acid bolt (5d8) every 6+d6 turns";
#endif

			}
			case ACT_BO_COLD_1:
			{
#ifdef JP
return "�A�C�X�E�{���g(6d8) : 7+d7 �^�[����";
#else
				return "frost bolt (6d8) every 7+d7 turns";
#endif

			}
			case ACT_BO_FIRE_1:
			{
#ifdef JP
return "�t�@�C�A�E�{���g(9d8) : 8+d8 �^�[����";
#else
				return "fire bolt (9d8) every 8+d8 turns";
#endif

			}
			case ACT_BA_COLD_1:
			{
#ifdef JP
return "�A�C�X�E�{�[�� (48) : 400 �^�[����";
#else
				return "ball of cold (48) every 400 turns";
#endif

			}
			case ACT_BA_FIRE_1:
			{
#ifdef JP
return "�t�@�C�A�E�{�[�� (72) : 400 �^�[����";
#else
				return "ball of fire (72) every 400 turns";
#endif

			}
			case ACT_DRAIN_1:
			{
#ifdef JP
return "�����͋z�� (100) : 100+d100 �^�[����";
#else
				return "drain life (100) every 100+d100 turns";
#endif

			}
			case ACT_BA_COLD_2:
			{
#ifdef JP
return "�A�C�X�E�{�[�� (100) : 300 �^�[����";
#else
				return "ball of cold (100) every 300 turns";
#endif

			}
			case ACT_BA_ELEC_2:
			{
#ifdef JP
return "�T���_�[�E�{�[�� (100) : 500 �^�[����";
#else
				return "ball of lightning (100) every 500 turns";
#endif

			}
			case ACT_DRAIN_2:
			{
#ifdef JP
return "�����͋z��(120) : 400 �^�[����";
#else
				return "drain life (120) every 400 turns";
#endif

			}
			case ACT_VAMPIRE_1:
			{
#ifdef JP
return "�z���h���C�� (3*50) : 400 �^�[����";
#else
				return "vampiric drain (3*50) every 400 turns";
#endif

			}
			case ACT_BO_MISS_2:
			{
#ifdef JP
return "�� (150) : 90+d90 �^�[����";
#else
				return "arrows (150) every 90+d90 turns";
#endif

			}
			case ACT_BA_FIRE_2:
			{
#ifdef JP
return "�t�@�C�A�E�{�[�� (120) : 225+d225 �^�[����";
#else
				return "fire ball (120) every 225+d225 turns";
#endif

			}
			case ACT_BA_COLD_3:
			{
#ifdef JP
return "�A�C�X�E�{�[�� (200) : 325+d325 �^�[����";
#else
				return "ball of cold (200) every 325+d325 turns";
#endif

			}
			case ACT_BA_ELEC_3:
			{
#ifdef JP
return "�T���_�[�E�{�[�� (250) : 425+d425 �^�[����";
#else
				return "ball of lightning (250) every 425+d425 turns";
#endif

			}
			case ACT_WHIRLWIND:
			{
#ifdef JP
return "�J�}�C�^�` : 250 �^�[����";
#else
				return "whirlwind attack every 250 turns";
#endif

			}
			case ACT_VAMPIRE_2:
			{
#ifdef JP
return "�z���h���C�� (3*100) : 400 �^�[����";
#else
				return "vampiric drain (3*100) every 400 turns";
#endif

			}
			case ACT_CALL_CHAOS:
			{
#ifdef JP
return "���׏��� : 350 �^�[����"; /*nuke me*/
#else
				return "call chaos every 350 turns";
#endif

			}
			case ACT_ROCKET:
			{
#ifdef JP
return "���P�b�g (120+level) : 400 �^�[����";
#else
				return "launch rocket (120+level) every 400 turns";
#endif

			}
			case ACT_DISP_EVIL:
			{
#ifdef JP
return "�׈��ގU (level*5) : 300+d300 �^�[����";
#else
				return "dispel evil (level*5) every 300+d300 turns";
#endif

			}
			case ACT_BA_MISS_3:
			{
#ifdef JP
return "�G�������g�̃u���X (300) : 500 �^�[����";
#else
				return "elemental breath (300) every 500 turns";
#endif

			}
			case ACT_DISP_GOOD:
			{
#ifdef JP
return "�P�ǑގU (level*5) : 300+d300 �^�[����";
#else
				return "dispel good (level*5) every 300+d300 turns";
#endif

			}
			case ACT_CONFUSE:
			{
#ifdef JP
return "�p�j�b�N�E�N���[�`���[ : 15 �^�[����";
#else
				return "confuse creature every 15 turns";
#endif

			}
			case ACT_SLEEP:
			{
#ifdef JP
return "���͂̃N���[�`���[�𖰂点�� : 55 �^�[����";
#else
				return "sleep nearby creatures every 55 turns";
#endif

			}
			case ACT_QUAKE:
			{
#ifdef JP
return "�n�k (���a 10) : 50 �^�[����";
#else
				return "earthquake (rad 10) every 50 turns";
#endif

			}
			case ACT_TERROR:
			{
#ifdef JP
return "���Q : 3 * (level+10) �^�[����";
#else
				return "terror every 3 * (level+10) turns";
#endif

			}
			case ACT_TELE_AWAY:
			{
#ifdef JP
return "�e���|�[�g�E�A�E�F�C : 150 �^�[����";
#else
				return "teleport away every 200 turns";
#endif

			}
			case ACT_BANISH_EVIL:
			{
#ifdef JP
return "�׈����� : 250+d250 �^�[����";
#else
				return "banish evil every 250+d250 turns";
#endif

			}
			case ACT_GENOCIDE:
			{
#ifdef JP
return "���E : 500 �^�[����";
#else
				return "genocide every 500 turns";
#endif

			}
			case ACT_MASS_GENO:
			{
#ifdef JP
return "���Ӗ��E : 1000 �^�[����";
#else
				return "mass genocide every 1000 turns";
#endif

			}
			case ACT_CHARM_ANIMAL:
			{
#ifdef JP
return "�������� : 300 �^�[����";
#else
				return "charm animal every 300 turns";
#endif

			}
			case ACT_CHARM_UNDEAD:
			{
#ifdef JP
return "�A���f�b�h�]�� : 333 �^�[����";
#else
				return "enslave undead every 333 turns";
#endif

			}
			case ACT_CHARM_OTHER:
			{
#ifdef JP
return "�N���[�`���[���� : 400 �^�[����";
#else
				return "charm creature every 400 turns";
#endif

			}
			case ACT_CHARM_ANIMALS:
			{
#ifdef JP
return "�����F�a : 500 �^�[����";
#else
				return "animal friendship every 500 turns";
#endif

			}
			case ACT_CHARM_OTHERS:
			{
#ifdef JP
return "���Ӗ��� : 750 �^�[����";
#else
				return "mass charm every 750 turns";
#endif

			}
			case ACT_SUMMON_ANIMAL:
			{
#ifdef JP
return "�������� : 200+d300 �^�[����";
#else
				return "summon animal every 200+d300 turns";
#endif

			}
			case ACT_SUMMON_PHANTOM:
			{
#ifdef JP
return "���쏢�� : 200+d200 �^�[����";
#else
				return "summon phantasmal servant every 200+d200 turns";
#endif

			}
			case ACT_SUMMON_ELEMENTAL:
			{
#ifdef JP
return "�G�������^������ : 750 �^�[����";
#else
				return "summon elemental every 750 turns";
#endif

			}
			case ACT_SUMMON_DEMON:
			{
#ifdef JP
return "�������� : 666+d333 �^�[����";
#else
				return "summon demon every 666+d333 turns";
#endif

			}
			case ACT_SUMMON_UNDEAD:
			{
#ifdef JP
return "�A���f�b�h���� : 666+d333 �^�[����";
#else
				return "summon undead every 666+d333 turns";
#endif

			}
			case ACT_CURE_LW:
			{
#ifdef JP
return "���|���� & 30 hp �� : 10 �^�[����";
#else
				return "remove fear & heal 30 hp every 10 turns";
#endif

			}
			case ACT_CURE_MW:
			{
#ifdef JP
return "4d8 hp & ���� : 3+d3 �^�[����";
#else
				return "heal 4d8 & wounds every 3+d3 turns";
#endif

			}
			case ACT_CURE_POISON:
			{
#ifdef JP
return "���|����/�ŏ��� : 5 �^�[����";
#else
				return "remove fear and cure poison every 5 turns";
#endif

			}
			case ACT_REST_LIFE:
			{
#ifdef JP
return "�o���l���� : 450 �^�[����";
#else
				return "restore life levels every 450 turns";
#endif

			}
			case ACT_REST_ALL:
			{
#ifdef JP
return "�S�X�e�[�^�X�ƌo���l���� : 750 �^�[����";
#else
				return "restore stats and life levels every 750 turns";
#endif

			}
			case ACT_CURE_700:
			{
#ifdef JP
return "700 hp �� : 250 �^�[����";
#else
				return "heal 700 hit points every 250 turns";
#endif

			}
			case ACT_CURE_1000:
			{
#ifdef JP
return "1000 hp �� : 888 �^�[����";
#else
				return "heal 1000 hit points every 888 turns";
#endif

			}
			case ACT_ESP:
			{
#ifdef JP
return "�e���p�V�[ (���� 25+d30) : 200 �^�[����";
#else
				return "telepathy (dur 25+d30) every 200 turns";
#endif

			}
			case ACT_BERSERK:
			{
#ifdef JP
return "�m�C���g�Əj�� (���� 50+d50) : 100+d100 �^�[����";
#else
				return "heroism and blessed (dur 50+d50) every 100+d100 turns";
#endif

			}
			case ACT_PROT_EVIL:
			{
#ifdef JP
return "�Ύ׈����E (���� level*3 + d25) : 225+d225 �^�[����";
#else
				return "protect evil (dur level*3 + d25) every 225+d225 turns";
#endif

			}
			case ACT_RESIST_ALL:
			{
#ifdef JP
return "�S�ϐ� (���� 40+d40) : 200 �^�[����";
#else
				return "resist elements (dur 40+d40) every 200 turns";
#endif

			}
			case ACT_SPEED:
			{
#ifdef JP
return "���� (���� 20+d20) : 250 �^�[����";
#else
				return "speed (dur 20+d20) every 250 turns";
#endif

			}
			case ACT_XTRA_SPEED:
			{
#ifdef JP
return "���� (���� 75+d75) : 200+d200 �^�[����";
#else
				return "speed (dur 75+d75) every 200+d200 turns";
#endif

			}
			case ACT_WRAITH:
			{
#ifdef JP
return "�H�̉� (���� level/2 + d(level/2)) : 1000 �^�[����";
#else
				return "wraith form (dur level/2 + d(level/2)) every 1000 turns";
#endif

			}
			case ACT_INVULN:
			{
#ifdef JP
return "���G�� (���� 8+d8) : 1000 �^�[����";
#else
				return "invulnerability (dur 8+d8) every 1000 turns";
#endif

			}
			case ACT_LIGHT:
			{
#ifdef JP
return "���ӏƖ� (�_���[�W 2d15) : 10+d10 �^�[����";
#else
				return "light area (dam 2d15) every 10+d10 turns";
#endif

			}
			case ACT_MAP_LIGHT:
			{
#ifdef JP
return "���ӏƖ� (�_���[�W 2d15) & ���Ӄ}�b�v : 50+d50 �^�[����";
#else
				return "light (dam 2d15) & map area every 50+d50 turns";
#endif

			}
			case ACT_DETECT_ALL:
			{
#ifdef JP
				return "�S���m : 55+d55 �^�[����";
#else
				return "detection every 55+d55 turns";
#endif

			}
			case ACT_DETECT_XTRA:
			{
#ifdef JP
				return "�S���m�A�T���A*�Ӓ�* : 1000 �^�[����";
#else
				return "detection, probing and identify true every 1000 turns";
#endif

			}
			case ACT_ID_FULL:
			{
#ifdef JP
				return "*�Ӓ�* : 750 �^�[����";
#else
				return "identify true every 750 turns";
#endif

			}
			case ACT_ID_PLAIN:
			{
#ifdef JP
				return "�Ӓ� : 10 �^�[����";
#else
				return "identify spell every 10 turns";
#endif

			}
			case ACT_RUNE_EXPLO:
			{
#ifdef JP
				return "�����̃��[�� : 200 �^�[����";
#else
				return "explosive rune every 200 turns";
#endif

			}
			case ACT_RUNE_PROT:
			{
#ifdef JP
				return "���̃��[�� : 400 �^�[����";
#else
				return "rune of protection every 400 turns";
#endif

			}
			case ACT_SATIATE:
			{
#ifdef JP
				return "�󕠏[�� : 200 �^�[����";
#else
				return "satisfy hunger every 200 turns";
#endif

			}
			case ACT_DEST_DOOR:
			{
#ifdef JP
return "�h�A�j�� : 10 �^�[����";
#else
				return "destroy doors every 10 turns";
#endif

			}
			case ACT_STONE_MUD:
			{
#ifdef JP
return "��Ηn�� : 5 �^�[����";
#else
				return "stone to mud every 5 turns";
#endif

			}
			case ACT_RECHARGE:
			{
#ifdef JP
return "���͏[�U : 70 �^�[����";
#else
				return "recharging every 70 turns";
#endif

			}
			case ACT_ALCHEMY:
			{
#ifdef JP
return "�B���p : 500 �^�[����";
#else
				return "alchemy every 500 turns";
#endif

			}
			case ACT_DIM_DOOR:
			{
#ifdef JP
return "�����̔� : 100 �^�[����";
#else
				return "dimension door every 100 turns";
#endif

			}
			case ACT_TELEPORT:
			{
#ifdef JP
return "�e���|�[�g (range 100) : 45 �^�[����";
#else
				return "teleport (range 100) every 45 turns";
#endif

			}
			case ACT_RECALL:
			{
#ifdef JP
return "�A�҂̏� : 200 �^�[����";
#else
				return "word of recall every 200 turns";
#endif

			}
			default:
			{
#ifdef JP
return "����`";
#else
				return "something undefined";
#endif

			}
		}
	}

	/* Some artifacts can be activated */
	switch (object_ptr->name1)
	{
		case ART_NARTHANC:
		{
#ifdef JP
return "�t�@�C�A�E�{���g(9d8) : 8+d8 �^�[����";
#else
			return "fire bolt (9d8) every 8+d8 turns";
#endif

		}
		case ART_NIMTHANC:
		{
#ifdef JP
return "�A�C�X�E�{���g(6d8) : 7+d7 �^�[����";
#else
			return "frost bolt (6d8) every 7+d7 turns";
#endif

		}
		case ART_DETHANC:
		{
#ifdef JP
return "�T���_�[�E�{���g(4d8) : 5+d5 �^�[����";
#else
			return "lightning bolt (4d8) every 6+d6 turns";
#endif

		}
		case ART_RILIA:
		{
#ifdef JP
return "���L�_(12) : 4+d4 �^�[����";
#else
			return "stinking cloud (12) every 4+d4 turns";
#endif

		}
		case ART_FIONA:
		{
#ifdef JP
return "�A�C�X�E�{�[��(48) : 5+d5 �^�[����";
#else
			return "frost ball (48) every 5+d5 turns";
#endif

		}
		case ART_FLORA:
		{
#ifdef JP
return "���|����/�ŏ��� : 5 �^�[����";
#else
			return "remove fear and cure poison every 5 turns";
#endif

		}
		case TRAIT_SUMMON_DAWN_LEGION:
		{
#ifdef JP
return "�ł̎t�c���� : 500+d500 �^�[����";
#else
			return "summon the Legion of the Dawn every 500+d500 turns";
#endif

		}
		case ART_ANDURIL:
		{
#ifdef JP
return "�t�@�C�A�E�{�[��(72) : 400 �^�[����";
#else
			return "fire ball (72) every 400 turns";
#endif

		}
		case ART_FIRESTAR:
		{
#ifdef JP
return "����t�@�C�A�E�{�[��(72) : 100 �^�[����";
#else
			return "large fire ball (72) every 100 turns";
#endif

		}
		case ART_CASPANION:
		{
#ifdef JP
return "�h�A/�g���b�v���� : 10 �^�[����";
#else
			return "door and trap destruction every 10 turns";
#endif

		}
		case ART_AVAVIR:
		case ART_MAGATAMA:
		case ART_HEAVENLY_MAIDEN:
		{
#ifdef JP
return "�A�҂̏� : 200 �^�[����";
#else
			return "word of recall every 200 turns";
#endif

		}
		case ART_EONWE:
		{
#ifdef JP
return "���Ӗ��E : 1000 �^�[����";
#else
			return "mass genocide every 1000 turns";
#endif

		}
		case ART_LOTHARANG:
		{
#ifdef JP
return "���̎���(4d8) : 3+d3 �^�[����";
#else
			return "cure wounds (4d8) every 3+d3 turns";
#endif

		}
		case ART_RUNESPEAR:
		{
#ifdef JP
return "�T���_�[�E�{�[�� (100) : 200 �^�[����";
#else
			return "lightning ball (100) every 200 turns";
#endif

		}
		case ART_AEGLOS:
		{
#ifdef JP
return "�A�C�X�E�{�[�� (100) : 200 �^�[����";
#else
			return "frost ball (100) every 200 turns";
#endif

		}
		case ART_SOULKEEPER:
		{
#ifdef JP
			return "�̗͉�(1000) : 888 �^�[����";
#else
			return "heal (1000) every 888 turns";
#endif

		}
		case ART_LOHENGRIN:
		{
#ifdef JP
return ("�� (777)�A�����A�m�C���g : 300 �^�[����");
#else
			return ("heal (777), curing and heroism every 300 turns");
#endif

		}
		case ART_JULIAN:
		{
#ifdef JP
return "���E : 500 �^�[����";
#else
			return "genocide every 500 turns";
#endif

		}
		case ART_LUTHIEN:
		{
#ifdef JP
return "�o���l���� : 450 �^�[����";
#else
			return "restore life levels every 450 turns";
#endif

		}
		case ART_ULMO:
		{
#ifdef JP
return "�e���|�[�g�E�A�E�F�C : 150 �^�[����";
#else
			return "teleport away every 150 turns";
#endif

		}
		case ART_COLLUIN:
		case ART_SEIRYU:
		{
#ifdef JP
return "�S�ϐ�(20+d20�^�[��) : 111 �^�[����";
#else
			return "resistance (20+d20 turns) every 111 turns";
#endif

		}
		case ART_HOLCOLLETH:
		{
#ifdef JP
return "�X���[�v(II) : 55 �^�[����";
#else
			return "sleep II every 55 turns";
#endif

		}
		case ART_THINGOL:
		{
#ifdef JP
return "���͏[�U : 70 �^�[����";
#else
			return "recharge item I every 70 turns";
#endif

		}
		case ART_COLANNON:
		{
#ifdef JP
return "�e���|�[�g : 45 �^�[����";
#else
			return "teleport every 45 turns";
#endif

		}
		case ART_TOTILA:
		{
#ifdef JP
return "�p�j�b�N�E�N���[�`���[ : 15 �^�[����";
#else
			return "confuse creature every 15 turns";
#endif

		}
		case ART_RAZORBACK:
		{
#ifdef JP
return "�X�^�[�E�{�[��(150) : 1000 �^�[����";
#else
			return "star ball (150) every 1000 turns";
#endif

		}
		case ART_BLADETURNER:
		{
#ifdef JP
return "�G�������g�̃u���X (300), �m�C���g�A�j���A�ϐ�";
#else
			return "breathe elements (300), hero, bless, and resistance";
#endif

		}
		case ART_ELENDIL:
		{
#ifdef JP
return "���@�̒n�}�ƌ� : 50+d50 �^�[����";
#else
			return "magic mapping and light every 50+d50 turns";
#endif

		}
		case ART_JUDGE:
		{
#ifdef JP
return "�̗͂ƈ����ւ��ɐ痢��ƋA�� : 20+d20 �^�[����";
#else
			return "clairvoyance and recall, draining you every 20+d20 turns";
#endif

		}
		case TRAIT_DISPEL_EVIL_1:
		{
#ifdef JP
return "�׈��ގU(x5) : 200+d200 �^�[����";
#else
			return "dispel evil (x5) every 200+d200 turns";
#endif
#ifdef JP
return "�׈��ގU(x5) : 100+d100 �^�[����";
#else
			return "dispel evil (x5) every 100+d100 turns";
#endif

		}

		case ART_CARLAMMAS:
		case ART_HERMIT:
		{
#ifdef JP
return "�Ύ׈����E : 225+d225 �^�[����";
#else
			return "protection from evil every 225+d225 turns";
#endif

		}
		case ART_FRAKIR:
		{
#ifdef JP
return "�����U��(100) : 100+d100 �^�[����";
#else
			return "a strangling attack (100) every 100+d100 turns";
#endif

		}
		case ART_TULKAS:
		{
#ifdef JP
return "�X�s�[�h(75+d75�^�[��) : 100+d100 �^�[����";
#else
			return "haste self (75+d75 turns) every 150+d150 turns";
#endif

		}
		case ART_NARYA:
		{
#ifdef JP
return "����t�@�C�A�E�{�[��(300) : 225+d225 �^�[����";
#else
			return "large fire ball (300) every 225+d225 turns";
#endif

		}
		case ART_NENYA:
		{
#ifdef JP
return "����A�C�X�E�{�[��(400) : 325+d325 �^�[����";
#else
			return "large frost ball (400) every 325+d325 turns";
#endif

		}
		case ART_VILYA:
		case ART_GOURYU:
		{
#ifdef JP
return "����T���_�[�E�{�[��(500) : 425+d425 �^�[����";
#else
			return "large lightning ball (500) every 425+d425 turns";
#endif

		}
		case ART_DOR:
		case ART_TERROR:
		//case ART_STONEMASK:
		{
#ifdef JP
			return "�S�����ւ̋��|�̌��� : 3*(���x��+10) �^�[����";
#else
			return "rays of fear in every direction every 3*(level+10) turns";
#endif

		}
		case ART_PALANTIR:
		{
#ifdef JP
return "���̊K�ɂ��郆�j�[�N�N���[�`���[��\�� : 200�^�[����";
#else
			return "list of the uniques on the level every 200 turns";
#endif
		}

		case ART_STONE_LORE:
		{
#ifdef JP
return "�댯�𔺂��Ӓ� : ���ł�";
#else
			return "perilous identify every turn";
#endif
		}
		case ART_FARAMIR:
		{
#ifdef JP
return "�Q���̋쏜 : 55+d55�^�[����";
#else
			return "dispel small life every 55+d55 turns";
#endif
		}
		case ART_BOROMIR:
		{
#ifdef JP
return "�N���[�`���[���Q : 40+d40�^�[����";
#else
			return "frighten creatures every 40+d40 turns";
#endif
		}
		case ART_HIMRING:
		{
#ifdef JP
return "�Ύ׈����E : 200+d200 �^�[����";
#else
			return "protection from evil every 200 + d200 turns";
#endif
		}
		case ART_ICANUS:
		{
#ifdef JP
return "���̖͂�(120) : 120+d120 �^�[����";
#else
			return "a mana bolt (120) every 120+d120 turns";
#endif
		}
		case ART_HURIN:
		{
#ifdef JP
return "�m�C���g, �X�s�[�h(50+d50�^�[��) : 100+d200 �^�[����";
#else
			return "hero and +10 to speed (50) every 100+200d turns";
#endif
		}
		case ART_GIL_GALAD:
		{
#ifdef JP
return "ῂ����� : 250 �^�[����";
#else
			return "blinding light every 250 turns";
#endif
		}
		case ART_YENDOR:
		{
#ifdef JP
return "���͏[�U : 200 �^�[����";
#else
			return "recharge item every 200 turns";
#endif
		}
		/*TODO
		case ART_MURAMASA:
		{
#ifdef JP
return "�r�͂̏㏸ : �m��50%�ŉ���";
#else
			return "increase STR (destroyed 50%)";
#endif
		}
		*/
		case ART_FLY_STONE:
		{
#ifdef JP
return "���̗͂�(400) : 250+d250�^�[����";
#else
			return "a mana storm every 250+d250 turns";
#endif
		}
		case ART_ARRYU:
		{
#ifdef JP
return "�n�E���h���� : 300+d150�^�[����";
#else
			return "summon hound every 300+d150 turns";
#endif
		}
		case ART_GAEBOLG:
		{
#ifdef JP
			return "����X�^�[�E�{�[��(200) : 200+d200 �^�[����";
#else
			return "large star ball (200) every 200+d200 turns";
#endif

		}
		case ART_INROU:
		{
#ifdef JP
return "��̃A�� : 150+d150 �^�[����";
#else
			return "reveal your identity every 150+d150 turns";
#endif

		}
		case ART_HYOUSIGI:
		{
#ifdef JP
return "���q�؂�ł��Ȃ炷 : ���ł�";
#else
			return "beat wooden clappers every turn";
#endif

		}
		case ART_EARENDIL:
		{
#ifdef JP
return "���� : 100�^�[����";
#else
			return "curing every 100 turns";
#endif

		}

		case ART_BOLISHOI:
		{
#ifdef JP
return "�������� : 200�^�[����";
#else
			return "charm animal every 200 turns";
#endif

		}
		case ART_ARUNRUTH:
		{
#ifdef JP
return "�A�C�X�E�{���g(12d8) : 50 �^�[����";
#else
			return "frost bolt (12d8) every 50 turns";
#endif

		}
		case ART_BLOOD:
		{
#ifdef JP
return "�����ύX : 3333 �^�[����";
#else
			return "change zokusei every 3333 turns";
#endif

		}
		case ART_MOOK:
		{
#ifdef JP
return "��C�̑ϐ� : 40+d40�^�[����";
#else
			return "resist cold every 40+d40 turns";
#endif

		}
		case ART_NIGHT:
		case ART_HELL:
		{
#ifdef JP
return "�Í��̗�(250) : 150+d150 �^�[����";
#else
			return "darkness storm (250) every 150+d150 turns";
#endif

		}
		case ART_SACRED_KNIGHTS:
		{
#ifdef JP
return "*����*�ƒ���: ���ł�";
#else
			return "dispel curse and probing every turn";
#endif

		}
		case ART_CHARMED:
		{
#ifdef JP
return "���͕���: 777 �^�[����";
#else
			return "restore mana every 777 turns";
#endif

		}
		case ART_AESCULAPIUS:
		{
#ifdef JP
			return "�S�X�e�[�^�X�ƌo���l���� : 750 �^�[����";
#else
			return "restore stats and life levels every 750 turns";
#endif
		}
	}


	if ((object_ptr->tval == TV_POLEARM) && (object_ptr->sval == SV_TSURIZAO))
	{
#ifdef JP
return "�ނ������ : ���ł�";
#else
		return "fishing : every time";
#endif

	}

	if (object_is_smith(object_ptr))
	{
		switch (object_ptr->xtra3 - 1)
		{
		case ESSENCE_TMP_RES_ACID:
#ifdef JP
			return "�_�ւ̑ϐ� : 50+d50�^�[����";
#else
			return "resist acid every 50+d50 turns";
#endif

		case ESSENCE_TMP_RES_ELEC:
#ifdef JP
			return "�d���ւ̑ϐ� : 50+d50�^�[����";
#else
			return "resist elec every 50+d50 turns";
#endif

		case ESSENCE_TMP_RES_FIRE:
#ifdef JP
			return "�΂ւ̑ϐ� : 50+d50�^�[����";
#else
			return "resist fire every 50+d50 turns";
#endif

		case ESSENCE_TMP_RES_COLD:
#ifdef JP
			return "��C�ւ̑ϐ� : 50+d50�^�[����";
#else
			return "resist cold every 50+d50 turns";
#endif

		case TRAIT_SHATTER:
#ifdef JP
			return "�n�k : 100+d100 �^�[����";
#else
			return "earthquake every 100+d100 turns";
#endif
		}
	}

	if (object_ptr->name2 == EGO_TRUMP)
	{
#ifdef JP
return "�e���|�[�g : 50+d50 �^�[����";
#else
		return "teleport every 50+d50 turns";
#endif

	}

	if (object_ptr->name2 == EGO_LITE_ILLUMINATION)
	{
#ifdef JP
return "�C���~�l�[�V���� : 10+d10 �^�[����";
#else
			return "illumination every 10+d10 turns";
#endif
	}

	else if (object_ptr->name2 == EGO_EARTHQUAKES)
	{
#ifdef JP
return "�n�k : 100+d100 �^�[����";
#else
		return "earthquake every 100+d100 turns";
#endif

	}

	else if (object_ptr->name2 == EGO_JUMP)
	{
#ifdef JP
return "�V���[�g�E�e���|�[�g : 10+d10 �^�[����";
#else
		return "blink every 10+d10 turns";
#endif

	}

	if (object_ptr->tval == TV_RING)
	{
		if (object_is_ego(object_ptr))
		{
			switch (object_ptr->name2)
			{
			case EGO_RING_HERO:
#ifdef JP
return "�m�C���g : 100+d100�^�[����";
#else
				return "heroism every 100+d100 turns";
#endif
			case EGO_RING_MAGIC_MIS:
#ifdef JP
return "�}�W�b�N�E�~�T�C��(2d6) : 2 �^�[����";
#else
			return "magic missile (2d6) every 2 turns";
#endif
			case EGO_RING_FIRE_BOLT:
#ifdef JP
return "�t�@�C�A�E�{���g(9d8) : 8+d8 �^�[����";
#else
			return "fire bolt (9d8) every 8+d8 turns";
#endif
			case EGO_RING_COLD_BOLT:
#ifdef JP
return "�A�C�X�E�{���g(6d8) : 7+d7 �^�[����";
#else
				return "frost bolt (6d8) every 7+d7 turns";
#endif
			case EGO_RING_ELEC_BOLT:
#ifdef JP
return "�T���_�[�E�{���g(4d8) : 5+d5 �^�[����";
#else
				return "lightning bolt (4d8) every 5+d5 turns";
#endif
			case EGO_RING_ACID_BOLT:
#ifdef JP
return "�A�V�b�h�E�{���g(5d8) : 6+d6 �^�[����";
#else
				return "acid bolt (5d8) every 6+d6 turns";
#endif
			case EGO_RING_MANA_BOLT:
#ifdef JP
return "���̖͂�(120) : 120+d120 �^�[����";
#else
			return "a mana bolt (120) every 120+d120 turns";
#endif
			case EGO_RING_FIRE_BALL:
#ifdef JP
return "�t�@�C�A�E�{�[�� (100) : 80+d80 �^�[����";
#else
				return "fire ball (100) every 80+d80 turns";
#endif
			case EGO_RING_COLD_BALL:
#ifdef JP
return "�A�C�X�E�{�[�� (100) : 80+d80 �^�[����";
#else
				return "cold ball (100) every 80+d80 turns";
#endif
			case EGO_RING_ELEC_BALL:
#ifdef JP
return "�T���_�[�E�{�[�� (100) : 80+d80 �^�[����";
#else
				return "elec ball (100) every 80+d80 turns";
#endif
			case EGO_RING_ACID_BALL:
#ifdef JP
return "�A�V�b�h�E�{�[�� (100) : 80+d80 �^�[����";
#else
				return "acid ball (100) every 80+d80 turns";
#endif
			case EGO_RING_MANA_BALL:
#ifdef JP
return "���̗͂� (250) : 300 �^�[����";
#else
				return "mana storm (250) every 300 turns";
#endif
			case EGO_RING_DRAGON_F:
				if (object_ptr->sval == SV_RING_FLAMES)
#ifdef JP
return "�Ή��̃u���X (200) �Ɖ΂ւ̑ϐ� : 200 �^�[����";
#else
					return "breath of fire (200) and resist fire every 200 turns";
#endif
				else
#ifdef JP
return "�Ή��̃u���X (200) : 250 �^�[����";
#else
					return "fire breath (200) every 250 turns";
#endif
			case EGO_RING_DRAGON_C:
				if (object_ptr->sval == SV_RING_ICE)
#ifdef JP
return "��C�̃u���X (200) �Ɨ�C�ւ̑ϐ� : 200 �^�[����";
#else
					return "breath of cold (200) and resist cold every 200 turns";
#endif
				else
#ifdef JP
return "��C�̃u���X (200) : 250 �^�[����";
#else
					return "cold breath (200) every 250 turns";
#endif
			case EGO_RING_M_DETECT:
#ifdef JP
return "�S�N���[�`���[���m : 150 �^�[����";
#else
				return "detect all creatures every 150 turns";
#endif
			case EGO_RING_D_SPEED:
#ifdef JP
return "�X�s�[�h(15+d30�^�[��) : 100 �^�[����";
#else
				return "haste self (15+d30 turns) every 100 turns";
#endif
			case EGO_RING_BERSERKER:
#ifdef JP
return "����m��(25+d25�^�[��) : 75+d75 �^�[����";
#else
				return "berserk (25+d25 turns) every 75+d75 turns";
#endif
			case EGO_RING_TELE_AWAY:
#ifdef JP
return "�e���|�[�g�E�A�E�F�C : 150 �^�[����";
#else
			return "teleport away every 150 turns";
#endif
			case EGO_RING_TRUE:
#ifdef JP
return "�m�C���g�A�j���A���ɂ̑ϐ� : 777 �^�[����";
#else
			return "hero, bless, and ultimate resistance every 777 turns";
#endif
			}
		}
		switch (object_ptr->sval)
		{
			case SV_RING_FLAMES:
#ifdef JP
return "�t�@�C�A�E�{�[�� (100) �Ɖ΂ւ̑ϐ� : 50+d50 �^�[����";
#else
				return "ball of fire (100) and resist fire every 50+d50 turns";
#endif

			case SV_RING_ICE:
#ifdef JP
return "�A�C�X�E�{�[�� (100) �Ɨ�C�ւ̑ϐ� : 50+d50 �^�[����";
#else
				return "ball of cold (100) and resist cold every 50+d50 turns";
#endif

			case SV_RING_ACID:
#ifdef JP
return "�A�V�b�h�E�{�[�� (100) �Ǝ_�ւ̑ϐ� : 50+d50 �^�[����";
#else
				return "ball of acid (100) and resist acid every 50+d50 turns";
#endif

			case SV_RING_ELEC:
#ifdef JP
return "�T���_�[�E�{�[�� (100) �Ɠd���ւ̑ϐ� : 50+d50 �^�[����";
#else
				return "ball of elec (100) and resist elec every 50+d50 turns";
#endif

			default:
				return NULL;
		}
	}

	if (object_ptr->tval == TV_AMULET)
	{
		if (object_is_ego(object_ptr))
		{
			switch (object_ptr->name2)
			{
			case EGO_AMU_IDENT:
#ifdef JP
				return "�Ӓ� : 10 �^�[����";
#else
				return "identify every 10 turns";
#endif
			case EGO_AMU_CHARM:
#ifdef JP
				return "�N���[�`���[���� : 200 �^�[����";
#else
				return "charm creature every 200 turns";
#endif
			case EGO_AMU_JUMP:
#ifdef JP
				return "�V���[�g�E�e���|�[�g : 10+d10 �^�[����";
#else
				return "blink every 10+d10 turns";
#endif
			case EGO_AMU_TELEPORT:
#ifdef JP
				return "�e���|�[�g : 50+d50 �^�[����";
#else
				return "teleport every 50+d50 turns";
#endif
			case EGO_AMU_D_DOOR:
#ifdef JP
				return "�����̔� : 200 �^�[����";
#else
				return "dimension door every 200 turns";
#endif
			case EGO_AMU_RES_FIRE_:
#ifdef JP
				return "�Ή��ւ̑ϐ� : 50+d50�^�[����";
#else
				return "resist fire every 50+d50 turns";
#endif
			case EGO_AMU_RES_COLD_:
#ifdef JP
				return "��C�ւ̑ϐ� : 50+d50�^�[����";
#else
				return "resist cold every 50+d50 turns";
#endif
			case EGO_AMU_RES_ELEC_:
#ifdef JP
				return "�d���ւ̑ϐ� : 50+d50�^�[����";
#else
				return "resist elec every 50+d50 turns";
#endif
			case EGO_AMU_RES_ACID_:
#ifdef JP
				return "�_�ւ̑ϐ� : 50+d50�^�[����";
#else
				return "resist acid every 50+d50 turns";
#endif
			case EGO_AMU_DETECTION:
#ifdef JP
				return "�S���m : 55+d55�^�[����";
#else
				return "detect all floor every 55+d55 turns";
#endif
			}
		}
	}

	if (object_ptr->tval == TV_WHISTLE)
	{
#ifdef JP
return "�y�b�g�Ăъ� : 100+d100�^�[����";
#else
		return "call pet every 100+d100 turns";
#endif
	}

	if (object_ptr->tval == TV_CAPTURE)
	{
#ifdef JP
return "�N���[�`���[��߂���A���͉������B";
#else
		return "captures or releases a creature.";
#endif
	}

	/* Require dragon scale mail */
#ifdef JP
if (object_ptr->tval != TV_DRAG_ARMOR) return ("��Ȍ�");
#else
	if (object_ptr->tval != TV_DRAG_ARMOR) return ("a strange glow");
#endif


	/* Branch on the sub-type */
	switch (object_ptr->sval)
	{
		case SV_DRAGON_BLUE:
		{
#ifdef JP
return "��Ȃ̃u���X(100) : 150+d150 �^�[����";
#else
			return "breathe lightning (100) every 150+d150 turns";
#endif

		}
		case SV_DRAGON_WHITE:
		{
#ifdef JP
return "��C�̃u���X(110) : 150+d150 �^�[����";
#else
			return "breathe frost (110) every 150+d150 turns";
#endif

		}
		case SV_DRAGON_BLACK:
		{
#ifdef JP
return "�_�̃u���X(130) : 150+d150 �^�[����";
#else
			return "breathe acid (130) every 150+d150 turns";
#endif

		}
		case SV_DRAGON_GREEN:
		{
#ifdef JP
return "�ł̃K�X�̃u���X(150) : 180+d180 �^�[����";
#else
			return "breathe poison gas (150) every 180+d180 turns";
#endif

		}
		case SV_DRAGON_RED:
		{
#ifdef JP
return "�Ή��̃u���X(200) : 200+d200 �^�[����";
#else
			return "breathe fire (200) every 200+d200 turns";
#endif

		}
		case SV_DRAGON_MULTIHUED:
		{
#ifdef JP
return "���F�̃u���X(250) : 200+d200 �^�[����";
#else
			return "breathe multi-hued (250) every 200+d200 turns";
#endif

		}
		case SV_DRAGON_BRONZE:
		{
#ifdef JP
return "�����̃u���X(120) : 180+d180 �^�[����";
#else
			return "breathe confusion (120) every 180+d180 turns";
#endif

		}
		case SV_DRAGON_GOLD:
		{
#ifdef JP
return "�����̃u���X(130) : 180+d180 �^�[����";
#else
			return "breathe sound (130) every 180+d180 turns";
#endif

		}
		case SV_DRAGON_CHAOS:
		{
#ifdef JP
return "�J�I�X/�򉻂̃u���X(220) : 200+d200 �^�[����";
#else
			return "breathe chaos/disenchant (220) every 200+d200 turns";
#endif

		}
		case SV_DRAGON_LAW:
		{
#ifdef JP
return "����/�j�Ђ̃u���X(230) : 200+d200 �^�[����";
#else
			return "breathe sound/shards (230) every 200+d200 turns";
#endif

		}
		case SV_DRAGON_BALANCE:
		{
#ifdef JP
return "�o�����X�̃u���X (250) 200+d200 �^�[����";
#else
			return "breathe balance (250) every 200+d200 turns";
#endif

		}
		case SV_DRAGON_SHINING:
		{
#ifdef JP
return "�M��/�Í��̃u���X(200) : 200+d200 �^�[����";
#else
			return "breathe light/darkness (200) every 200+d200 turns";
#endif

		}
		case SV_DRAGON_POWER:
		{
#ifdef JP
return "�G�������g�̃u���X(300) : 200+d200 �^�[����";
#else
			return "breathe the elements (300) every 200+d200 turns";
#endif

		}
	}

	/* Oops */
#ifdef JP
return "��C�̑�";
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

	u32b flgs[TR_FLAG_SIZE];

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

	if (object_is_equipment(object_ptr))
	{
		/* Descriptions of a basic equipment is just a flavor */
		trivial_info = i;
	}

	/* Mega-Hack -- describe activation */

	//TODO:TR_ACTIVATE
	{
#ifdef JP
		info[i++] = "�n�������Ƃ��̌���...";
#else
		info[i++] = "It can be activated for...";
#endif

		info[i++] = item_activation(object_ptr);
#ifdef JP
info[i++] = "...�������������Ă��Ȃ���΂Ȃ�Ȃ��B";
#else
		info[i++] = "...if it is being worn.";
#endif

	}

	/* Figurines, a hack */
	if (object_ptr->tval == TV_FIGURINE)
	{
#ifdef JP
info[i++] = "����͓��������y�b�g�ɕω�����B";
#else
		info[i++] = "It will transform into a pet when thrown.";
#endif

	}

	if ((object_ptr->tval == TV_SWORD) && (object_ptr->sval == SV_DOKUBARI))
	{
#ifdef JP
info[i++] = "����͑�����ꌂ�œ|�����Ƃ�����B";
#else
		info[i++] = "It will attempt to kill a creature instantly.";
#endif

	}

	if ((object_ptr->tval == TV_POLEARM) && (object_ptr->sval == SV_DEATH_SCYTHE))
	{
#ifdef JP
info[i++] = "����͎������g�ɍU�����Ԃ��Ă��邱�Ƃ�����B";
#else
		info[i++] = "It causes you to strike yourself sometimes.";
#endif

#ifdef JP
info[i++] = "����͖��G�̃o���A��؂�􂭁B";
#else
		info[i++] = "It always penetrates invulnerability barriers.";
#endif
	}

	if (object_ptr->name2 == EGO_TWO_WEAPON)
	{
#ifdef JP
info[i++] = "����͓񓁗��ł̖����������コ����B";
#else
		info[i++] = "It affects your ability to hit when you are wielding two weapons.";
#endif

	}

	if (object_ptr->name2 == EGO_AMU_FOOL)
	{
#ifdef JP
info[i++] = "����͖��@�̓�Փx���グ��B";
#else
		info[i++] = "It interferes with casting spells.";
#endif
	}

	if (object_ptr->name2 == EGO_RING_THROW)
	{
#ifdef JP
info[i++] = "����͕������������邱�Ƃ��\�ɂ���B";
#else
		info[i++] = "It provides great strength when you throw an item.";
#endif
	}

	if (object_ptr->name2 == EGO_AMU_NAIVETY)
	{
#ifdef JP
info[i++] = "����͖��@��R�͂�������B";
#else
		info[i++] = "It decreases your magic resistance.";
#endif
	}

	if (object_ptr->tval == TV_STATUE)
	{
		species_type *r_ptr = &species_info[object_ptr->pval];


		if(object_ptr->sval == SV_DAKIMAKURA)
#ifdef JP
			info[i++] = "����̓x�b�h�ɒu���Ɩ������B";
#else
			info[i++] = "It is shameful.";
#endif	
		else if(object_ptr->sval == SV_DOUJINSHI)
#ifdef JP
			info[i++] = "����͌����Ǒ��ɔ����Ă���B";
#else
			info[i++] = "It is HENTAI.";
#endif	
		else if (object_ptr->pval == SPECIES_RICHARD_STOLENMAN || object_ptr->sval == SV_FS_FIGURE || object_ptr->sval == SV_FIGURE )
#ifdef JP
			info[i++] = "����͕����ɏ���ƒp���������B";
#else
			info[i++] = "It is shameful.";
#endif
		else if (has_trait_species(r_ptr, TRAIT_ELDRITCH_HORROR))
#ifdef JP
			info[i++] = "����͕����ɏ���Ƌ����B";
#else
			info[i++] = "It is fearful.";
#endif
		else
#ifdef JP
			info[i++] = "����͕����ɏ���Ɗy�����B";
#else
		info[i++] = "It is cheerful.";
#endif
	}

	/* Hack -- describe lite's */
	if (object_ptr->tval == TV_LITE)
	{
		if (object_ptr->name2 == EGO_LITE_DARKNESS)
		{
#ifdef JP
			info[i++] = "����͑S������Ȃ��B";
#else
			info[i++] = "It provides no light.";
#endif

			if (object_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
				info[i++] = "����͖�����̔��a�����߂�(���a��-3)�B";
#else
				info[i++] = "It decreases radius of light source by 3.";
#endif
			}
			else if (object_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
				info[i++] = "����͖�����̔��a�����߂�(���a��-2)�B";
#else
				info[i++] = "It decreases radius of light source by 2.";
#endif
			}
			else
			{
#ifdef JP
				info[i++] = "����͖�����̔��a�����߂�(���a��-1)�B";
#else
				info[i++] = "It decreases radius of light source by 1.";
#endif
			}
		}
		else if (object_is_fixed_artifact(object_ptr))
		{
#ifdef JP
info[i++] = "����͉i���Ȃ閾����(���a 3)��������B";
#else
			info[i++] = "It provides light (radius 3) forever.";
#endif

		}
		else if (object_ptr->name2 == EGO_LITE_SHINE)
		{
			if (object_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
info[i++] = "����͉i���Ȃ閾����(���a 3)��������B";
#else
				info[i++] = "It provides light (radius 3) forever.";
#endif

			}
			else if (object_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
info[i++] = "����͔R���⋋�ɂ���Ė�����(���a 3)��������B";
#else
				info[i++] = "It provides light (radius 3) when fueled.";
#endif

			}
			else
			{
#ifdef JP
info[i++] = "����͔R���⋋�ɂ���Ė�����(���a 2)��������B";
#else
				info[i++] = "It provides light (radius 2) when fueled.";
#endif

			}
		}
		else
		{
			if (object_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
info[i++] = "����͉i���Ȃ閾����(���a 2)��������B";
#else
				info[i++] = "It provides light (radius 2) forever.";
#endif

			}
			else if (object_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
info[i++] = "����͔R���⋋�ɂ���Ė�����(���a 2)��������B";
#else
				info[i++] = "It provides light (radius 2) when fueled.";
#endif

			}
			else
			{
#ifdef JP
info[i++] = "����͔R���⋋�ɂ���Ė�����(���a 1)��������B";
#else
				info[i++] = "It provides light (radius 1) when fueled.";
#endif

			}
		}
		if (object_ptr->name2 == EGO_LITE_LONG)
		{
#ifdef JP
info[i++] = "����͒����^�[���������������B";
#else
			info[i++] = "It provides light for much longer time.";
#endif
		}
	}


	/* And then describe it fully */

	if (have_flag(flgs, TR_RIDING))
	{
		if ((object_ptr->tval == TV_POLEARM) && ((object_ptr->sval == SV_LANCE) || (object_ptr->sval == SV_HEAVY_LANCE)))
#ifdef JP
info[i++] = "����͏�n���͔��Ɏg���₷���B";
#else
			info[i++] = "It is made for use while riding.";
#endif
		else
		{
#ifdef JP
			info[i++] = "����͏�n���ł��g���₷���B";
#else
			info[i++] = "It is suitable for use while riding.";
#endif
			/* This information is not important enough */
			trivial_info++;
		}
	}
	if (have_flag(flgs, STAT_STR))
	{
#ifdef JP
info[i++] = "����͘r�͂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your strength.";
#endif

	}
	if (have_flag(flgs, STAT_INT))
	{
#ifdef JP
info[i++] = "����͒m�\�ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your intelligence.";
#endif

	}
	if (have_flag(flgs, STAT_WIS))
	{
#ifdef JP
info[i++] = "����͌����ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your wisdom.";
#endif

	}
	if (have_flag(flgs, STAT_DEX))
	{
#ifdef JP
info[i++] = "����͊�p���ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your dexterity.";
#endif

	}
	if (have_flag(flgs, STAT_CON))
	{
#ifdef JP
info[i++] = "����͑ϋv�͂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your constitution.";
#endif

	}
	if (have_flag(flgs, STAT_CHA))
	{
#ifdef JP
info[i++] = "����͖��͂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your charisma.";
#endif

	}

	if (have_flag(flgs, TR_MAGIC_MASTERY))
	{
#ifdef JP
info[i++] = "����͖��@����g�p�\�͂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your ability to use magic devices.";
#endif

	}
	if (have_flag(flgs, TR_STEALTH))
	{
#ifdef JP
info[i++] = "����͉B���s���\�͂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your stealth.";
#endif

	}
	if (have_flag(flgs, TR_SEARCH))
	{
#ifdef JP
info[i++] = "����͒T���\�͂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your searching.";
#endif

	}
	if (have_flag(flgs, TR_INFRA))
	{
#ifdef JP
info[i++] = "����͐ԊO�����͂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your infravision.";
#endif

	}
	if (have_flag(flgs, TR_TUNNEL))
	{
#ifdef JP
info[i++] = "����͍̌@�\�͂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your ability to tunnel.";
#endif

	}
	if (have_flag(flgs, TR_SPEED))
	{
#ifdef JP
info[i++] = "����̓X�s�[�h�ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your speed.";
#endif

	}
	if (have_flag(flgs, TR_BLOWS))
	{
#ifdef JP
info[i++] = "����͑Ō��񐔂ɉe�����y�ڂ��B";
#else
		info[i++] = "It affects your attack speed.";
#endif

	}

	if (have_flag(flgs, TRAIT_ACID_BRAND))
	{
#ifdef JP
info[i++] = "����͎_�ɂ���đ傫�ȃ_���[�W��^����B";
#else
		info[i++] = "It does extra damage from acid.";
#endif

	}
	if (have_flag(flgs, TRAIT_ELEC_BRAND))
	{
#ifdef JP
info[i++] = "����͓d���ɂ���đ傫�ȃ_���[�W��^����B";
#else
		info[i++] = "It does extra damage from electricity.";
#endif

	}
	if (have_flag(flgs, TRAIT_FIRE_BRAND))
	{
#ifdef JP
info[i++] = "����͉Ή��ɂ���đ傫�ȃ_���[�W��^����B";
#else
		info[i++] = "It does extra damage from fire.";
#endif

	}
	if (have_flag(flgs, TRAIT_COLD_BRAND))
	{
#ifdef JP
info[i++] = "����͗�C�ɂ���đ傫�ȃ_���[�W��^����B";
#else
		info[i++] = "It does extra damage from frost.";
#endif

	}

	if (have_flag(flgs, TRAIT_POIS_BRAND))
	{
#ifdef JP
info[i++] = "����͓G��ł���B";
#else
		info[i++] = "It poisons your foes.";
#endif

	}

	if (have_flag(flgs, TRAIT_CHAOTIC_BRAND))
	{
#ifdef JP
info[i++] = "����̓J�I�X�I�Ȍ��ʂ��y�ڂ��B";
#else
		info[i++] = "It produces chaotic effects.";
#endif

	}

	if (have_flag(flgs, TRAIT_VAMPIRIC_BRAND))
	{
#ifdef JP
info[i++] = "����͓G����q�b�g�|�C���g���z������B";
#else
		info[i++] = "It drains life from your foes.";
#endif

	}

	if (have_flag(flgs, TRAIT_SHATTER))
	{
#ifdef JP
info[i++] = "����͒n�k���N�������Ƃ��ł���B";
#else
		info[i++] = "It can cause earthquakes.";
#endif

	}

	if (have_flag(object_ptr->trait_flags, TRAIT_VORPAL))
	{
#ifdef JP
info[i++] = "����͔��ɐ؂ꖡ���s���G��ؒf���邱�Ƃ��ł���B";
#else
		info[i++] = "It is very sharp and can cut your foes.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_DRAGON))
	{
#ifdef JP
info[i++] = "����̓h���S���ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of dragons.";
#endif

	}
	else if (have_flag(flgs, TRAIT_SLAY_DRAGON))
	{
#ifdef JP
info[i++] = "����̓h���S���ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
#else
		info[i++] = "It is especially deadly against dragons.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_ORC))
	{
#ifdef JP
info[i++] = "����̓I�[�N�ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of orcs.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLAY_ORC))
	{
#ifdef JP
info[i++] = "����̓I�[�N�ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
#else
		info[i++] = "It is especially deadly against orcs.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_TROLL))
	{
#ifdef JP
info[i++] = "����̓g�����ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of trolls.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLAY_TROLL))
	{
#ifdef JP
info[i++] = "����̓g�����ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
#else
		info[i++] = "It is especially deadly against trolls.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_GIANT))
	{
#ifdef JP
info[i++] = "����͋��l�ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of giants.";
#endif
	}
	else if (have_flag(flgs, TRAIT_SLAY_GIANT))
	{
#ifdef JP
info[i++] = "����̓W���C�A���g�ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
#else
		info[i++] = "It is especially deadly against giants.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_DEMON))
	{
#ifdef JP
info[i++] = "����̓f�[�����ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of demons.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLAY_DEMON))
	{
#ifdef JP
info[i++] = "����̓f�[�����ɑ΂��Đ��Ȃ�͂𔭊�����B";
#else
		info[i++] = "It strikes at demons with holy wrath.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_UNDEAD))
	{
#ifdef JP
info[i++] = "����̓A���f�b�h�ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of undead.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLAY_UNDEAD))
	{
#ifdef JP
info[i++] = "����̓A���f�b�h�ɑ΂��Đ��Ȃ�͂𔭊�����B";
#else
		info[i++] = "It strikes at undead with holy wrath.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_EVIL))
	{
#ifdef JP
info[i++] = "����͎׈��Ȃ鑶�݂ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of evil creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLAY_EVIL))
	{
#ifdef JP
info[i++] = "����͎׈��Ȃ鑶�݂ɑ΂��Đ��Ȃ�͂ōU������B";
#else
		info[i++] = "It fights against evil with holy fury.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_GOOD))
	{
#ifdef JP
info[i++] = "����͑P�ǂȂ鑶�݂ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of good creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLAY_GOOD))
	{
#ifdef JP
info[i++] = "����͑P�ǂȂ鑶�݂ɑ΂��Ēn���̗͂ōU������B";
#else
		info[i++] = "It fights against good with hellish fury.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_ANIMAL))
	{
#ifdef JP
info[i++] = "����͎��R�E�̓����ɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of natural creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLAY_ANIMAL))
	{
#ifdef JP
info[i++] = "����͎��R�E�̓����ɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
#else
		info[i++] = "It is especially deadly against natural creatures.";
#endif

	}

	if (have_flag(flgs, TRAIT_KILL_HUMAN))
	{
#ifdef JP
info[i++] = "����͐l�ԂɂƂ��Ă̓V�G�ł���B";
#else
		info[i++] = "It is a great bane of humans.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLAY_HUMAN))
	{
#ifdef JP
info[i++] = "����͐l�Ԃɑ΂��ē��ɋ���ׂ��͂𔭊�����B";
#else
		info[i++] = "It is especially deadly against humans.";
#endif

	}

	if (have_flag(flgs, TR_FORCE_WEAPON))
	{
#ifdef JP
info[i++] = "����͎g�p�҂̖��͂��g���čU������B";
#else
		info[i++] = "It powerfully strikes at a creature using your mana.";
#endif

	}
	if (have_flag(flgs, TRAIT_DEC_MANA))
	{
#ifdef JP
info[i++] = "����͖��͂̏������������B";
#else
		info[i++] = "It decreases your mana consumption.";
#endif

	}
	if (have_flag(flgs, TRAIT_SUSTAIN_STR))
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̘r�͂��ێ�����B";
#else
		info[i++] = "It sustains your strength.";
#endif

	}
	if (have_flag(flgs, TRAIT_SUSTAIN_INT))
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̒m�\���ێ�����B";
#else
		info[i++] = "It sustains your intelligence.";
#endif

	}
	if (have_flag(flgs, TRAIT_SUSTAIN_WIS))
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̌������ێ�����B";
#else
		info[i++] = "It sustains your wisdom.";
#endif

	}
	if (have_flag(flgs, TRAIT_SUSTAIN_DEX))
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̊�p�����ێ�����B";
#else
		info[i++] = "It sustains your dexterity.";
#endif

	}
	if (have_flag(flgs, TRAIT_SUSTAIN_CON))
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̑ϋv�͂��ێ�����B";
#else
		info[i++] = "It sustains your constitution.";
#endif

	}
	if (have_flag(flgs, TRAIT_SUSTAIN_CHR))
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̖��͂��ێ�����B";
#else
		info[i++] = "It sustains your charisma.";
#endif

	}

	if (have_flag(flgs, TRAIT_IM_ACID))
	{
#ifdef JP
info[i++] = "����͎_�ɑ΂��銮�S�ȖƉu��������B";
#else
		info[i++] = "It provides immunity to acid.";
#endif

	}
	if (have_flag(flgs, TRAIT_IM_ELEC))
	{
#ifdef JP
info[i++] = "����͓d���ɑ΂��銮�S�ȖƉu��������B";
#else
		info[i++] = "It provides immunity to electricity.";
#endif

	}
	if (have_flag(flgs, TRAIT_IM_FIRE))
	{
#ifdef JP
info[i++] = "����͉΂ɑ΂��銮�S�ȖƉu��������B";
#else
		info[i++] = "It provides immunity to fire.";
#endif

	}
	if (have_flag(flgs, TRAIT_IM_COLD))
	{
#ifdef JP
info[i++] = "����͊����ɑ΂��銮�S�ȖƉu��������B";
#else
		info[i++] = "It provides immunity to cold.";
#endif

	}

	if (have_flag(flgs, TRAIT_THROW_MIGHTY))
	{
#ifdef JP
info[i++] = "����͓G�ɓ����đ傫�ȃ_���[�W��^���邱�Ƃ��ł���B";
#else
		info[i++] = "It is perfectly balanced for throwing.";
#endif
	}

	if (have_flag(flgs, TRAIT_FREE_ACTION))
	{
#ifdef JP
info[i++] = "����͖�Ⴢɑ΂��銮�S�ȖƉu��������B";
#else
		info[i++] = "It provides immunity to paralysis.";
#endif

	}
	if (have_flag(flgs, TRAIT_HOLD_LIFE))
	{
#ifdef JP
info[i++] = "����͐����͋z���ɑ΂���ϐ���������B";
#else
		info[i++] = "It provides resistance to life draining.";
#endif

	}
	if (have_flag(flgs, TRAIT_FEARLESS))
	{
#ifdef JP
info[i++] = "����͋��|�ւ̊��S�ȑϐ���������B";
#else
		info[i++] = "It makes you completely fearless.";
#endif

	}
	if (have_flag(flgs, TRAIT_RES_ACID))
	{
#ifdef JP
info[i++] = "����͎_�ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to acid.";
#endif

	}
	if (have_flag(flgs, TRAIT_RES_ELEC))
	{
#ifdef JP
info[i++] = "����͓d���ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to electricity.";
#endif

	}
	if (have_flag(flgs, TRAIT_RES_FIRE))
	{
#ifdef JP
info[i++] = "����͉΂ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to fire.";
#endif

	}
	if (have_flag(flgs, TRAIT_RES_COLD))
	{
#ifdef JP
info[i++] = "����͊����ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to cold.";
#endif

	}
	if (have_flag(flgs, TRAIT_RES_POIS))
	{
#ifdef JP
info[i++] = "����͓łւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to poison.";
#endif

	}

	if (have_flag(flgs, TRAIT_RES_LITE))
	{
#ifdef JP
info[i++] = "����͑M���ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to light.";
#endif

	}
	if (have_flag(flgs, TRAIT_RES_DARK))
	{
#ifdef JP
info[i++] = "����͈Í��ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to dark.";
#endif

	}

	if (have_flag(flgs, TRAIT_NO_BLIND))
	{
#ifdef JP
info[i++] = "����͖Ӗڂւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to blindness.";
#endif

	}
	if (have_flag(flgs, TRAIT_NO_CONF))
	{
#ifdef JP
info[i++] = "����͍����ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to confusion.";
#endif

	}
	if (have_flag(object_ptr->trait_flags, TRAIT_RES_SOUN))
	{
#ifdef JP
info[i++] = "����͍����ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to sound.";
#endif

	}
	if (have_flag(flgs, TRAIT_RES_SHAR))
	{
#ifdef JP
info[i++] = "����͔j�Ђւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to shards.";
#endif

	}

	if (have_flag(object_ptr->trait_flags, TRAIT_RES_NETH))
	{
#ifdef JP
info[i++] = "����͒n���ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to nether.";
#endif

	}
	if (have_flag(object_ptr->trait_flags, TRAIT_RES_NEXU))
	{
#ifdef JP
info[i++] = "����͈��ʍ����ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to nexus.";
#endif

	}
	if (have_flag(object_ptr->trait_flags, TRAIT_RES_CHAO))
	{
#ifdef JP
info[i++] = "����̓J�I�X�ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to chaos.";
#endif

	}
	if (have_flag(object_ptr->trait_flags, TRAIT_RES_DISE))
	{
#ifdef JP
info[i++] = "����͗򉻂ւ̑ϐ���������B";
#else
		info[i++] = "It provides resistance to disenchantment.";
#endif

	}

	if (have_flag(flgs, TRAIT_LEVITATION))
	{
#ifdef JP
info[i++] = "����͒��ɕ������Ƃ��\�ɂ���B";
#else
		info[i++] = "It allows you to levitate.";
#endif

	}
	if (have_flag(flgs, TR_LITE))
	{
		if ((object_ptr->name2 == EGO_DARK) || (object_ptr->name1 == ART_NIGHT))
#ifdef JP
info[i++] = "����͖�����̔��a�����߂�(���a��-1)�B";
#else
			info[i++] = "It decreases radius of your light source by 1.";
#endif
		else
#ifdef JP
info[i++] = "����͉i���̖������������(���a��+1)�B";
#else
			info[i++] = "It provides permanent light. (radius +1)";
#endif

	}
	if (have_flag(flgs, TRAIT_SEE_INVISIBLE))
	{
#ifdef JP
info[i++] = "����͓����ȃN���[�`���[�����邱�Ƃ��\�ɂ���B";
#else
		info[i++] = "It allows you to see invisible creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_ESP))
	{
#ifdef JP
info[i++] = "����̓e���p�V�[�\�͂�������B";
#else
		info[i++] = "It gives telepathic powers.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_ANIMAL))
	{
#ifdef JP
info[i++] = "����͎��R�E�̐��������m����B";
#else
		info[i++] = "It senses natural creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_UNDEAD))
	{
#ifdef JP
info[i++] = "����̓A���f�b�h�����m����B";
#else
		info[i++] = "It senses undead.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_DEMON))
	{
#ifdef JP
info[i++] = "����͈��������m����B";
#else
		info[i++] = "It senses demons.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_ORC))
	{
#ifdef JP
info[i++] = "����̓I�[�N�����m����B";
#else
		info[i++] = "It senses orcs.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_TROLL))
	{
#ifdef JP
info[i++] = "����̓g���������m����B";
#else
		info[i++] = "It senses trolls.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_GIANT))
	{
#ifdef JP
info[i++] = "����͋��l�����m����B";
#else
		info[i++] = "It senses giants.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_DRAGON))
	{
#ifdef JP
info[i++] = "����̓h���S�������m����B";
#else
		info[i++] = "It senses dragons.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_HUMAN))
	{
#ifdef JP
info[i++] = "����͐l�Ԃ����m����B";
#else
		info[i++] = "It senses humans.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_EVIL))
	{
#ifdef JP
info[i++] = "����͎׈��ȑ��݂����m����B";
#else
		info[i++] = "It senses evil creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_GOOD))
	{
#ifdef JP
info[i++] = "����͑P�ǂȑ��݂����m����B";
#else
		info[i++] = "It senses good creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_NONLIVING))
	{
#ifdef JP
info[i++] = "����͊������閳�����̂����m����B";
#else
		info[i++] = "It senses non-living creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_SENSE_UNIQUE))
	{
#ifdef JP
info[i++] = "����͓��ʂȋ��G�����m����B";
#else
		info[i++] = "It senses unique creatures.";
#endif

	}
	if (have_flag(flgs, TRAIT_SLOW_DIGEST))
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̐V��ӂ�x������B";
#else
		info[i++] = "It slows your metabolism.";
#endif

	}
	if (have_flag(flgs, TRAIT_REGENERATE))
	{
#ifdef JP
info[i++] = "����̗͉͑񕜗͂���������B";
#else
		info[i++] = "It speeds your regenerative powers.";
#endif

	}
	if (have_flag(flgs, TRAIT_WARNING))
	{
#ifdef JP
info[i++] = "����͊댯�ɑ΂��Čx���𔭂���B";
#else
		info[i++] = "It warns you of danger";
#endif

	}
	if (have_flag(flgs, TRAIT_REFLECTING))
	{
#ifdef JP
info[i++] = "����͖��{���g�𔽎˂���B";
#else
		info[i++] = "It reflects bolts and arrows.";
#endif

	}
	if (have_flag(flgs, TRAIT_AURA_FIRE))
	{
#ifdef JP
info[i++] = "����͉��̃o���A�𒣂�B";
#else
		info[i++] = "It produces a fiery sheath.";
#endif

	}
	if (have_flag(flgs, TRAIT_AURA_ELEC))
	{
#ifdef JP
info[i++] = "����͓d�C�̃o���A�𒣂�B";
#else
		info[i++] = "It produces an electric sheath.";
#endif

	}
	if (have_flag(flgs, TRAIT_AURA_COLD))
	{
#ifdef JP
info[i++] = "����͗�C�̃o���A�𒣂�B";
#else
		info[i++] = "It produces a sheath of coldness.";
#endif

	}
	if (have_flag(flgs, TRAIT_ANTI_MAGIC))
	{
#ifdef JP
info[i++] = "����͔����@�o���A�𒣂�B";
#else
		info[i++] = "It produces an anti-magic shell.";
#endif

	}
	if (have_flag(flgs, TRAIT_PREVENT_TELEPORT))
	{
#ifdef JP
info[i++] = "����̓e���|�[�g���ז�����B";
#else
		info[i++] = "It prevents teleportation.";
#endif

	}
	if (have_flag(flgs, TRAIT_EXTRA_ATTACK_MIGHT))
	{
#ifdef JP
info[i++] = "����͖�^�{���g�^�e����苭�͂ɔ��˂��邱�Ƃ��ł���B";
#else
		info[i++] = "It fires missiles with extra might.";
#endif

	}
	if (have_flag(flgs, TRAIT_EXTRA_ATTACK_SPEED))
	{
#ifdef JP
info[i++] = "����͖�^�{���g�^�e����ɑ������˂��邱�Ƃ��ł���B";
#else
		info[i++] = "It fires missiles excessively fast.";
#endif

	}

	if (have_flag(flgs, TRAIT_BLESSED_BRAND))
	{
#ifdef JP
info[i++] = "����͐_�ɏj������Ă���B";
#else
		info[i++] = "It has been blessed by the gods.";
#endif

	}

	if (object_is_cursed(object_ptr))
	{
		if (object_ptr->curse_flags & TRC_DIVINE_CURSE)
		{
			char tx[80];
			tx[0] = '\0';
#ifdef JP
			sprintf(tx, "����͐_�i %d �̐_��̎􂢂��������Ă���B", object_ptr->xtra1);
#else
			sprintf(tx, "It is divinely cursed (Divine Rank %d)", object_ptr->xtra1);
#endif
			info[i++] = tx;

		}
		else if (object_ptr->curse_flags & TRC_HEAVY_CURSE)
		{
#ifdef JP
info[i++] = "����͋��͂Ȏ􂢂��������Ă���B";
#else
			info[i++] = "It is heavily cursed.";
#endif

		}
		else
		{
#ifdef JP
info[i++] = "����͎���Ă���B";
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

	if ((have_flag(flgs, TRAIT_TY_CURSE)) || (object_ptr->curse_flags & TRC_TY_CURSE))
	{
#ifdef JP
info[i++] = "����͑��ẨЁX�������O���h���Ă���B";
#else
		info[i++] = "It carries an ancient foul curse.";
#endif

	}
/*
	if ((have_flag(flgs, TRAIT_ANTIPATHY)) || (object_ptr->curse_flags & TRC_AGGRAVATE))
	{
#ifdef JP
info[i++] = "����͕t�߂̃N���[�`���[��{�点��B";
#else
		info[i++] = "It aggravates nearby creatures.";
#endif

	}
*/
	if ((have_flag(flgs, TRAIT_DRAIN_EXP)) || (object_ptr->curse_flags & TRC_DRAIN_EXP))
	{
#ifdef JP
info[i++] = "����͌o���l���z�����B";
#else
		info[i++] = "It drains experience.";
#endif

	}
	if (object_ptr->curse_flags & TRC_SLOW_REGEN)
	{
#ifdef JP
info[i++] = "����͉񕜗͂���߂�B";
#else
		info[i++] = "It slows your regenerative powers.";
#endif

	}
	if (object_ptr->curse_flags & TRC_ADD_L_CURSE)
	{
#ifdef JP
info[i++] = "����͎ア�􂢂𑝂₷�B";
#else
		info[i++] = "It adds weak curses.";
#endif

	}
	if (object_ptr->curse_flags & TRC_ADD_H_CURSE)
	{
#ifdef JP
info[i++] = "����͋��͂Ȏ􂢂𑝂₷�B";
#else
		info[i++] = "It adds heavy curses.";
#endif

	}
	if (object_ptr->curse_flags & TRC_CALL_ANIMAL)
	{
#ifdef JP
info[i++] = "����͓������Ăъ񂹂�B";
#else
		info[i++] = "It attracts animals.";
#endif

	}
	if (object_ptr->curse_flags & TRC_CALL_DEMON)
	{
#ifdef JP
info[i++] = "����͈������Ăъ񂹂�B";
#else
		info[i++] = "It attracts demons.";
#endif

	}
	if (object_ptr->curse_flags & TRC_CALL_DRAGON)
	{
#ifdef JP
info[i++] = "����̓h���S�����Ăъ񂹂�B";
#else
		info[i++] = "It attracts dragons.";
#endif

	}
	if (object_ptr->curse_flags & TRC_COWARDICE)
	{
#ifdef JP
info[i++] = "����͋��|���������N�����B";
#else
		info[i++] = "It makes you subject to cowardice.";
#endif

	}
	if ((have_flag(flgs, TR_TELEPORT)) || (object_ptr->curse_flags & TRC_TELEPORT))
	{
#ifdef JP
info[i++] = "����̓����_���ȃe���|�[�g�������N�����B";
#else
		info[i++] = "It induces random teleportation.";
#endif

	}
	if (object_ptr->curse_flags & TRC_LOW_MELEE)
	{
#ifdef JP
info[i++] = "����͍U�����O���₷���B";
#else
		info[i++] = "It causes you to miss blows.";
#endif

	}
	if (object_ptr->curse_flags & TRC_LOW_AC)
	{
#ifdef JP
info[i++] = "����͍U�����󂯂₷���B";
#else
		info[i++] = "It helps your enemies' blows.";
#endif

	}
	if (object_ptr->curse_flags & TRC_LOW_MAGIC)
	{
#ifdef JP
info[i++] = "����͖��@�������ɂ�������B";
#else
		info[i++] = "It encumbers you while spellcasting.";
#endif

	}
	if (object_ptr->curse_flags & TRC_FAST_DIGEST)
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̐V��ӂ𑬂�����B";
#else
		info[i++] = "It speeds your metabolism.";
#endif

	}
	if (object_ptr->curse_flags & TRC_DRAIN_HP)
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̗̑͂��z�����B";
#else
		info[i++] = "It drains you.";
#endif

	}
	if (object_ptr->curse_flags & TRC_DRAIN_MANA)
	{
#ifdef JP
info[i++] = "����͂��Ȃ��̖��͂��z�����B";
#else
		info[i++] = "It drains your mana.";
#endif

	}

	/* Describe about this kind of object instead of THIS fake object */
	if (mode & SCROBJ_FAKE_OBJECT)
	{
		switch (object_ptr->tval)
		{
		case TV_RING:
			switch (object_ptr->sval)
			{
			case SV_RING_LORDLY:
#ifdef JP
				info[i++] = "����͊���̃����_���ȑϐ���������B";
#else
				info[i++] = "It provides some random resistances.";
#endif
				break;
			case SV_RING_WARNING:
#ifdef JP
				info[i++] = "����͂ЂƂ̒ዉ��ESP�������鎖������B";
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
				info[i++] = "����͓łւ̑ϐ��������鎖������B";
#else
				info[i++] = "It may provides resistance to poison.";
#endif
#ifdef JP
				info[i++] = "����̓����_���ȑϐ��������鎖������B";
#else
				info[i++] = "It may provide a random resistances.";
#endif
				break;
			case SV_AMULET_THE_MAGI:
#ifdef JP
				info[i++] = "����͍ő�łR�܂ł̒ዉ��ESP��������B";
#else
				info[i++] = "It provides up to three low rank ESPs.";
#endif
				break;
			}
			break;
		}
	}

	if (have_flag(flgs, TR_IGNORE_ACID) &&
	    have_flag(flgs, TR_IGNORE_ELEC) &&
	    have_flag(flgs, TR_IGNORE_FIRE) &&
	    have_flag(flgs, TR_IGNORE_COLD))
	{
#ifdef JP
		info[i++] = "����͎_�E�d���E�Ή��E��C�ł͏����Ȃ��B";
#else
		info[i++] = "It cannot be harmed by the elements.";
#endif
	}
	else
	{
		if (have_flag(flgs, TR_IGNORE_ACID))
		{
#ifdef JP
			info[i++] = "����͎_�ł͏����Ȃ��B";
#else
			info[i++] = "It cannot be harmed by acid.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_ELEC))
		{
#ifdef JP
			info[i++] = "����͓d���ł͏����Ȃ��B";
#else
			info[i++] = "It cannot be harmed by electricity.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_FIRE))
		{
#ifdef JP
			info[i++] = "����͉Ή��ł͏����Ȃ��B";
#else
			info[i++] = "It cannot be harmed by fire.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_COLD))
		{
#ifdef JP
			info[i++] = "����͗�C�ł͏����Ȃ��B";
#else
			info[i++] = "It cannot be harmed by cold.";
#endif
		}
	}

	if (mode & SCROBJ_FORCE_DETAIL) trivial_info = 0;

	/* No relevant informations */
	if (i <= trivial_info) return (FALSE);

	/* Save the screen */
	screen_save();

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Display Item name */
	if (!(mode & SCROBJ_FAKE_OBJECT))
		object_desc(object_name, object_ptr, 0);
	else
		object_desc(object_name, object_ptr, (OD_NAME_ONLY | OD_STORE));

	prt(object_name, 0, 0);

	/* Erase the screen */
	for (k = 1; k < hgt; k++) prt("", k, 13);

	/* Label the information */
	if ((object_ptr->tval == TV_STATUE) && (object_ptr->sval == SV_PHOTO))
	{
		species_type *r_ptr = &species_info[object_ptr->pval];
		int namelen = strlen(species_name + r_ptr->name);
		prt(format("%s: '", species_name + r_ptr->name), 1, 15);
		Term_queue_bigchar(18 + namelen, 1, r_ptr->x_attr, r_ptr->x_char, 0, 0);
		prt("'", 1, (use_bigtile ? 20 : 19) + namelen);
	}
	else
#ifdef JP
prt("     �A�C�e���̔\��:", 1, 15);
#else
	prt("     Item Attributes:", 1, 15);
#endif

	/* We will print on top of the map (column 13) */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
		prt(info[j], k++, 15);

		/* Every 20 entries (lines 2 to 21), start over */
		if ((k == hgt - 2) && (j+1 < i))
		{
#ifdef JP
prt("-- ���� --", k, 15);
#else
			prt("-- more --", k, 15);
#endif
			inkey();
			for (; k > 2; k--) prt("", k, 15);
		}
	}

	/* Wait for it */
#ifdef JP
prt("[�����L�[�������ƃQ�[���ɖ߂�܂�]", k, 15);
#else
	prt("[Press any key to continue]", k, 15);
#endif

	inkey();

	/* Restore the screen */
	screen_load();

	/* Gave knowledge */
	return (TRUE);
}



/*
 * Convert an inventory index into a one character label
 * Note that the label does NOT distinguish inven/equip.
 */
char index_to_label(int i)
{
	if(i < 0 || i >= INVEN_TOTAL) return '-';
	return (i < 26 ? (char)i + 'a' : (char)(i - 26) + 'A'); 
}


/*
 * Convert a label into the index of an item in the "inven"
 * Return "-1" if the label does not indicate a real item
 */
s16b label_to_item(creature_type *creature_ptr, int c)
{
	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1);

	/* Verify the index */
	if ((i < 0) || (i > INVEN_TOTAL)) return (-1);

	/* Empty slots can never be chosen */
	if (!creature_ptr->inventory[i].k_idx) return (-1);

	/* Return the index */
	return (i);
}


/*
 * Return a string mentioning how a given item is carried
 */
cptr mention_use(creature_type *creature_ptr, int slot, int num)
{
	cptr p;

#ifdef JP
	if(!num) return "����";
#else
	if(!num) return "In pack";
#endif

	/* Examine the location */
	switch (slot)
	{
#ifdef JP
		case INVEN_SLOT_INVENTORY:
			p = "����";
			break;
#else
		case INVEN_SLOT_INVENTORY:
			p = "In pack";
			break;
#endif

#ifdef JP
		case INVEN_SLOT_HAND:  
			if(creature_ptr->heavy_wield[num])
			{
				p = "�^����";
				break;
			}
			else
			{
				switch(num)
				{
					case 1:
						p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "�E��" : "��P��";
						break;
					case 2:
						p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "����" : "��Q��";
						break;
					case 3:
						p = "��R��";
						break;
					case 4:
						p = "��S��";
						break;
					case 5:
						p = "��T��";
						break;
					case 6:
						p = "��U��";
						break;
					case 7:
						p = "��V��";
						break;
					case 8:
						p = "��W��";
						break;
					default:
						p = "��H";
						break;
				}
			}
			break;
#else
		case INVEN_SLOT_HAND:
			p = creature_ptr->heavy_wield[0] ? "Just lifting" : (creature_ptr->can_melee[0] ? "Wielding" : "On arm"); break;
#endif

#ifdef JP
		case INVEN_SLOT_BOW:
			p = "�ˌ��p";
//			p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "�^����" : "�ˌ��p"; break;
			break;
#else
		case INVEN_SLOT_BOW:
			p = "Shooting";
//			p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "Just holding" : "Shooting"; break;
			break;
#endif

#ifdef JP
		case INVEN_SLOT_AMMO:
			p = "��e"; break;
#else
		case INVEN_SLOT_AMMO:
			p = "Projectile"; break;
#endif

#ifdef JP
		case INVEN_SLOT_RING:
			switch(num)
			{
				case 1:
					p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "�E�w" : "��P�w";
					break;
				case 2:
					p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "���w" : "��Q�w";
					break;
				default:
					p = "�w�H";
					break;
			}
			break;
#else
		case INVEN_SLOT_RING:
			p = "1st Ring"; break;
			break;
#endif

#ifdef JP
		case INVEN_SLOT_AMULET:
			p = "�아"; break;
#else
		case INVEN_SLOT_AMULET:
			p = "Amulet"; break;
#endif

#ifdef JP
		case INVEN_SLOT_LITE:
			p = "����"; break;
#else
		case INVEN_SLOT_LITE:
			p = "Light source"; break;
#endif

#ifdef JP
		case INVEN_SLOT_INSTRUMENT:
			p = "�y��"; break;
#else
		case INVEN_SLOT_INSTRUMENT:
			p = "Instrument"; break;
#endif

#ifdef JP
		case INVEN_SLOT_BODY:
			p = "��"; break;
#else
		case INVEN_SLOT_BODY:
			p = "On body"; break;
#endif

#ifdef JP
		case INVEN_SLOT_OUTER:
			p = "���"; break;
#else
		case INVEN_SLOT_OUTER:
			p = "About body"; break;
#endif

#ifdef JP
		case INVEN_SLOT_HEAD:
			switch(num)
			{
				case 1:
					p = has_trait(creature_ptr, TRAIT_HUMANOID) ? "����" : "��P��";
					break;
				case 2:
					p = "��Q��";
					break;
				case 3:
					p = "��R��";
					break;
				case 4:
					p = "��S��";
					break;
				case 5:
					p = "��T��";
					break;
				case 6:
					p = "��U��";
					break;
				case 7:
					p = "��V��";
					break;
				case 8:
					p = "��W��";
					break;
				default:
					p = "���H";
					break;
			}
			break;
#else
		case INVEN_SLOT_HEAD:
			p = "On head"; break;
#endif

#ifdef JP
		case INVEN_SLOT_ARMS:
			p = "�r��"; break;
#else
		case INVEN_SLOT_ARMS:
			p = "On hands"; break;
#endif

#ifdef JP
		case INVEN_SLOT_FEET:
			p = "�r��"; break;
#else
		case INVEN_SLOT_FEET:
			p = "On feet"; break;
#endif

#ifdef JP
		case INVEN_SLOT_TAIL:
			p = "����"; break;
#else
		case INVEN_SLOT_TAIL:
			p = "On tail"; break;
#endif

#ifdef JP
		case INVEN_SLOT_INTAKE:
			p = "�̓�"; break;
#else
		case INVEN_SLOT_INTAKE:
			p = "Intake"; break;
#endif

	}

	/* Return the result */
	return p;
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
		case INVEN_SLOT_HAND:  p = creature_ptr->heavy_wield[0] ? "�^������" : ((creature_ptr->two_handed && creature_ptr->can_melee[0]) ? "����ɑ������Ă���" : (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "����ɑ������Ă���" : "�E��ɑ������Ă���")); break;
#else
		case INVEN_SLOT_HAND:  p = creature_ptr->heavy_wield[0] ? "just lifting" : (creature_ptr->can_melee[0] ? "attacking creatures with" : "wearing on your arm"); break;
#endif

#ifdef JP
		case INVEN_SLOT_BOW:   p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "�������Ő���t��" : "�ˌ��p�ɑ������Ă���"; break;
#else
		case INVEN_SLOT_BOW:   p = (adj_str_equipping_weight[creature_ptr->stat_ind[STAT_STR]] < creature_ptr->inventory[i].weight / 10) ? "just holding" : "shooting missiles with"; break;
#endif

#ifdef JP
		case INVEN_SLOT_RING: p = (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "����̎w�ɂ͂߂Ă���" : "�E��̎w�ɂ͂߂Ă���"); break;
#else
		case INVEN_SLOT_RING: p = (has_trait(creature_ptr, TRAIT_LEFT_HANDER) ? "wearing on your left hand" : "wearing on your right hand"); break;
#endif

#ifdef JP
		case INVEN_SLOT_AMULET:  p = "��ɂ����Ă���"; break;
#else
		case INVEN_SLOT_AMULET:  p = "wearing around your neck"; break;
#endif

#ifdef JP
		case INVEN_SLOT_LITE:  p = "�����ɂ��Ă���"; break;
#else
		case INVEN_SLOT_LITE:  p = "using to light the way"; break;
#endif

#ifdef JP
		case INVEN_SLOT_BODY:  p = "�̂ɒ��Ă���"; break;
#else
		case INVEN_SLOT_BODY:  p = "wearing on your body"; break;
#endif

#ifdef JP
		case INVEN_SLOT_OUTER: p = "�g�ɂ܂Ƃ��Ă���"; break;
#else
		case INVEN_SLOT_OUTER: p = "wearing on your back"; break;
#endif

#ifdef JP
		case INVEN_SLOT_HEAD:  p = "���ɂ��Ԃ��Ă���"; break;
#else
		case INVEN_SLOT_HEAD:  p = "wearing on your head"; break;
#endif

#ifdef JP
		case INVEN_SLOT_ARMS: p = "��ɂ��Ă���"; break;
#else
		case INVEN_SLOT_ARMS: p = "wearing on your hands"; break;
#endif

#ifdef JP
		case INVEN_SLOT_FEET:  p = "���ɂ͂��Ă���"; break;
#else
		case INVEN_SLOT_FEET:  p = "wearing on your feet"; break;
#endif

#ifdef JP
		default:          p = "�U�b�N�ɓ����Ă���"; break;
#else
		default:          p = "carrying in your pack"; break;
#endif
	}

	/* Return the result */
	return p;
}


/* Hack: Check if a spellbook is one of the realms we can use. -- TY */

bool check_book_realm(creature_type *creature_ptr, const byte book_tval, const byte book_sval)
{
	if (book_tval < TV_LIFE_BOOK) return FALSE;
	if (creature_ptr->class_idx == CLASS_SORCERER)
	{
		return is_magic(tval2realm(book_tval));
	}
	else if (creature_ptr->class_idx == CLASS_RED_MAGE)
	{
		if (is_magic(tval2realm(book_tval)))
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
	if (!object_ptr->k_idx) return (FALSE);

	/* Hack -- ignore "gold" */
	/* TODO remove
	if (object_ptr->tval == TV_GOLD)
	{
		// See xtra2.c
		extern bool show_gold_on_floor;

		if (!show_gold_on_floor) return (FALSE);
	}
	*/

	/* Check the tval */
	if (item_tester_tval)
	{
		/* Is it a spellbook? If so, we need a hack -- TY */
		if ((item_tester_tval <= TV_DEATH_BOOK) &&
			(item_tester_tval >= TV_LIFE_BOOK))
			return check_book_realm(creature_ptr, object_ptr->tval, object_ptr->sval);
		else
			if (item_tester_tval != object_ptr->tval) return (FALSE);
	}

	/* Check the hook */
	if (item_tester_hook)
	{
		if (!(*item_tester_hook)(creature_ptr, object_ptr)) return (FALSE);
	}

	/* Assume okay */
	return (TRUE);
}




/*
 * Choice window "shadow" of the "show_item_list()" function
 */
void display_inven(creature_type *creature_ptr)
{
	register        int i, n, z = 0;
	object_type     *object_ptr;
	byte            attr = TERM_WHITE;
	char            tmp_val[80];
	char            object_name[MAX_NLEN];
	int             wid, hgt;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Find the "final" slot */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_ptr = &creature_ptr->inventory[i];

		/* Skip non-objects */
		if (!object_ptr->k_idx) continue;

		/* Track */
		z = i + 1;
	}

	/* Display the pack */
	for (i = 0; i < z; i++)
	{
		/* Examine the item */
		object_ptr = &creature_ptr->inventory[i];

		/* Start with an empty "index" */
		tmp_val[0] = tmp_val[1] = tmp_val[2] = ' ';

		/* Is this item "acceptable"? */
		if (item_tester_okay(creature_ptr, object_ptr, NULL, 0))
		{
			/* Prepare an "index" */
			tmp_val[0] = index_to_label(i);

			/* Bracket the "index" --(-- */
			tmp_val[1] = ')';
		}

		/* Display the index (or blank space) */
		Term_putstr(0, i, 3, TERM_WHITE, tmp_val);

		/* Obtain an item description */
		object_desc(object_name, object_ptr, 0);

		/* Obtain the length of the description */
		n = strlen(object_name);

		/* Get a color */
		attr = tval_to_acttr[object_ptr->tval % 128];
		if (attr == TERM_DARK) attr = TERM_WHITE;

		/* Grey out charging items */
		if (object_ptr->timeout)
		{
			attr = TERM_L_DARK;
		}

		/* Display the entry itself */
		Term_putstr(3, i, n, attr, object_name);

		/* Erase the rest of the line */
		Term_erase(3+n, i, 255);

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt = object_ptr->weight * object_ptr->number;
			char buf[80];
			format_weight(buf, wgt);
			(void)sprintf(tmp_val, "%10s", buf);

			prt(tmp_val, i, wid - 10);
		}
	}

	/* Erase the rest of the window */
	for (i = z; i < hgt; i++)
	{
		/* Erase the line */
		Term_erase(0, i, 255);
	}
}



/*
 * Choice window "shadow" of the "show_item_list()" function
 */
void display_equip(creature_type *creature_ptr)
{
	register        int i, j, n;
	object_type     *object_ptr;
	byte            attr = TERM_WHITE;
	char            tmp_val[80];
	char            object_name[MAX_NLEN];
	int             wid, hgt;

	/* Get size */
	Term_get_size(&wid, &hgt);

	n = 0;
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		for(j = 0; j < creature_ptr->item_slot_num[INVEN_SLOT_HAND]; j++)
		{
			object_ptr = get_equipped_slot_ptr(creature_ptr, INVEN_SLOT_HAND, j);
			if(object_ptr->k_idx)
			{
				/* Obtain an item description */
				object_desc(object_name, object_ptr, 0);

				/* Display the index (or blank space) */
				Term_putstr(0, n, 3, TERM_WHITE, object_name);
				n++;
			}
		}

	}


	/* Display the equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		/* Examine the item */
		object_ptr = &creature_ptr->inventory[i];

		if(!IS_EQUIPPED(object_ptr)) continue;

		/* Start with an empty "index" */
		tmp_val[0] = tmp_val[1] = tmp_val[2] = ' ';

		/* Display the index (or blank space) */
		Term_putstr(0, i, 3, TERM_WHITE, tmp_val);

		/* Obtain an item description */
		object_desc(object_name, object_ptr, 0);
		attr = tval_to_acttr[object_ptr->tval % 128];

		/* Obtain the length of the description */
		n = strlen(object_name);

		/* Grey out charging items */
		if (object_ptr->timeout)
		{
			attr = TERM_L_DARK;
		}

		/* Display the entry itself */
		Term_putstr(3, i, n, attr, object_name);

		/* Erase the rest of the line */
		Term_erase(3+n, i, 255);

		/* Display the weight (if needed) */
		if (show_weights)
		{
			int wgt = object_ptr->weight * object_ptr->number;
			char buf[80];
			format_weight(buf, wgt);
			(void)sprintf(tmp_val, "%10s", buf);

			prt(tmp_val, i, wid - (show_labels ? 28 : 10));
		}

		/* Display the slot description (if needed) */
		if (show_labels)
		{
			Term_putstr(wid - 20, i, -1, TERM_WHITE, " <-- ");
			prt(mention_use(creature_ptr, GET_INVEN_SLOT_TYPE(creature_ptr, i), IS_EQUIPPED(object_ptr)), i, wid - 15);
		}
	}

	/* Erase the rest of the window */
	//TODO
	for (i = INVEN_TOTAL; i < hgt; i++)
	{
		/* Clear that line */
		Term_erase(0, i, 255);
	}
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
		if (!object_ptr->k_idx) continue;

		if (IS_EQUIPPED(object_ptr) && mode != USE_INVEN) continue;
		if (!IS_EQUIPPED(object_ptr) && mode != USE_EQUIP) continue;

		/* Skip empty inscriptions */
		if (!object_ptr->inscription) continue;

		/* Skip non-choice */
		if (!item_tester_okay(creature_ptr, object_ptr, NULL, 0)) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(object_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if ((s[1] == command_cmd) && (s[2] == tag))
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
	if (tag < '0' || '9' < tag)
	{
		/* No such tag */
		return FALSE;
	}

	/* Check every object */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &creature_ptr->inventory[i];

		if (IS_EQUIPPED(object_ptr) && mode != USE_INVEN) continue;
		if (!IS_EQUIPPED(object_ptr) && mode != USE_EQUIP) continue;

		/* Skip non-objects */
		if (!object_ptr->k_idx) continue;

		/* Skip empty inscriptions */
		if (!object_ptr->inscription) continue;

		/* Skip non-choice */
		if (!item_tester_okay(creature_ptr, object_ptr, NULL, 0)) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(object_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the normal tags */
			if (s[1] == tag)
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
		if (!object_ptr->inscription) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(object_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if ((s[1] == command_cmd) && (s[2] == tag))
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
	if (tag < '0' || '9' < tag)
	{
		/* No such tag */
		return FALSE;
	}

	/* Check every object in the grid */
	for (i = 0; i < floor_num && i < INVEN_TOTAL; i++)
	{
		object_type *object_ptr = &object_list[floor_list[i]];

		/* Skip empty inscriptions */
		if (!object_ptr->inscription) continue;

		/* Find a '@' */
		s = my_strchr(quark_str(object_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the normal tags */
			if (s[1] == tag)
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
		if (get_tag(creature_ptr, &index, c, mode))
		{
			/* Delete the overwritten label */
			if (label[j] == c) label[j] = ' ';

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
		if (get_tag_floor(&index, c, floor_list, floor_num))
		{
			/* Delete the overwritten label */
			if (label[i] == c) label[i] = ' ';

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


	/* Starting column */
	col = 999;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Default "max-length" */
	len = wid - col - 1;

	prepare_label_string(creature_ptr, inven_label, USE_INVEN);

	if(flags & SHOW_ITEM_EQUIPMENT)
	{
		for(k = 0, i = 1; i < MAX_INVENTORY_SLOT; i++)
		{
			n = creature_ptr->item_slot_num[i]; 
			for(j = 1; j <= n; j++)
			{
				m = get_equipped_slot_idx(creature_ptr, i, j); 
				object_ptr = &creature_ptr->inventory[m];
				object_desc(object_name, object_ptr, 0); // Describe the object

				// Save the object index, color, and description
				out_index[k] = m;
				out_color[k] = tval_to_acttr[object_ptr->tval % 128];

				if (object_ptr->timeout) out_color[k] = TERM_L_DARK; // Grey out charging items

				(void)strcpy(out_desc[k], object_name);

				l = strlen(out_desc[k]); // Find the predicted "line length"
				if (show_weights) l += 15; // Be sure to account for the weight

				if (show_item_graph) // Account for icon if displayed
				{
					l += 2;
					if (use_bigtile) l++;
				}

				if (l > len) len = l; // Maintain the maximum length

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
			if (!object_ptr->k_idx) continue;

			if (!(flags & SHOW_ITEM_INVENTORY) && !item_tester_okay(creature_ptr, object_ptr, hook, 0)) continue;
			if (!((IS_EQUIPPED(object_ptr) && (flags & SHOW_ITEM_EQUIPMENT)) || (!IS_EQUIPPED(object_ptr) && (flags & SHOW_ITEM_INVENTORY)))) continue;

			/* Describe the object */
			object_desc(object_name, object_ptr, 0);

			/* Save the object index, color, and description */
			out_index[k] = i;
			out_color[k] = tval_to_acttr[object_ptr->tval % 128];

			/* Grey out charging items */
			if (object_ptr->timeout)
			{
				out_color[k] = TERM_L_DARK;
			}

			(void)strcpy(out_desc[k], object_name);

			/* Find the predicted "line length" */
			l = strlen(out_desc[k]);

			/* Be sure to account for the weight */
			if (show_weights) l += 15;

			/* Account for icon if displayed */
			if (show_item_graph)
			{
				l += 2;
				if (use_bigtile) l++;
			}

			/* Maintain the maximum length */
			if (l > len) len = l;

			slot[k] = GET_INVEN_SLOT_TYPE(creature_ptr, i);
			num[k] = IS_EQUIPPED(object_ptr);

			/* Advance to next "line" */
			k++;
		}
	}


	if(flags & SHOW_ITEM_RIGHT_SET){
		col = (len > wid - 9) ? 0 : (wid - len - 9);
	}
	else
	{
		col = 1;
	}


	if(!k)
	{
		if(flags & SHOW_ITEM_EQUIPMENT)
		{
#if JP
			put_str("[���������ł��Ȃ�]", 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 23 : 1);
#else
			put_str("[No Equipment]", 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 15 : 1);
#endif
		}
		else if(flags & SHOW_ITEM_INVENTORY)
		{
#if JP
			put_str("[���������Ă��Ȃ�]", 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 21 : 1);
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

		if (use_menu && target_item)
		{
			if (j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "�t");
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
		if (show_item_graph)
		{
			byte a = object_attr(object_ptr);
			char c = object_char(object_ptr);

			if(j + 1 >= hgt) break;
			Term_queue_bigchar(cur_col, j + 1, a, c, 0, 0);
			if (use_bigtile) cur_col++;

			cur_col += 2;
		}

		// Display the entry itself
		c_put_str(IS_EQUIPPED(object_ptr) ? TERM_WHITE : TERM_L_DARK, mention_use(creature_ptr, slot[j], num[j]) , j + 1, cur_col);
		c_put_str(out_color[j], out_desc[j], j + 1, cur_col + 7);

		// Display the weight if needed
		if (show_weights)
		{
			int wgt = object_ptr->weight * object_ptr->number;
			char buf[80];
			format_weight(buf, wgt);
			(void)sprintf(tmp_val, "%10s", buf);

			prt(tmp_val, j + 1, flags & SHOW_ITEM_RIGHT_SET ? wid - 10 : len + 3);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (j && (j < INVEN_TOTAL)) prt("", j + 1, col ? col - 2 : col);

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
		if (!angband_term[j]) continue;

		/* Flip inven to equip */
		if (window_flag[j] & (PW_INVEN))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_INVEN);
			window_flag[j] |= (PW_EQUIP);

			/* Window stuff */
			play_window |= (PW_EQUIP);
		}

		/* Flip inven to equip */
		else if (window_flag[j] & (PW_EQUIP))
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
	if (item >= 0)
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
(void)sprintf(out_val, "%s%s�ł���? ", prompt, object_name);
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

	if (!command_cmd) return TRUE; /* command_cmd is no longer effective */

	/* creature_ptr->inventory */
	if (item >= 0)
	{
		object_ptr = &creature_ptr->inventory[item];
	}

	/* Floor */
	else
	{
		object_ptr = &object_list[0 - item];
	}

	/* No inscription */
	if (!object_ptr->inscription) return (TRUE);

	/* Find a '!' */
	s = my_strchr(quark_str(object_ptr->inscription), '!');

	/* Process preventions */
	while (s)
	{
		/* Check the "restriction" */
		if ((s[1] == command_cmd) || (s[1] == '*'))
		{
			/* Verify the choice */
#ifdef JP
			if (!verify(creature_ptr, "�{����", item)) return (FALSE);
#else
			if (!verify("Really try", item)) return (FALSE);
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
	if ((i < 0) || (i >= INVEN_TOTAL)) return (FALSE); // Illegal items
	if (!item_tester_okay(creature_ptr, &creature_ptr->inventory[i], hook, item_tester_tval)) return (FALSE); // Verify the item
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
		if (item_tester_okay(creature_ptr, &creature_ptr->inventory[j], NULL, 0))
			return TRUE;

	floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);
	if (floor_num)
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
		n = 0;
	}
	if(slot_num == 1)
	{
		n = 1;
	}
	else
	{
		for(i = 1; i <= slot_num; i++)
		{
			object_ptr = get_equipped_slot_ptr(creature_ptr, slot, i);
			object_desc(buf, object_ptr, 0);
			sprintf(se[i - 1].cap, "%-6s %s", mention_use(creature_ptr, slot, i), buf);
			se[i - 1].code = i;
			se[i - 1].key = '\0';

			if (object_ptr->timeout)
			{
				 se[i - 1].d_color = TERM_L_DARK;
				 se[i - 1].l_color = TERM_L_DARK;
			}
			else
			{
				se[i - 1].d_color = tval_to_acttr[object_ptr->tval % 128];
				se[i - 1].l_color = tval_to_acttr[object_ptr->tval % 128];
			}

		}

#if JP
		strcpy(se[i - 1].cap, "�L�����Z��");
#else
		strcpy(se[i - 1].cap, "Cancel");
#endif
		se[i - 1].code = 0;
		se[i - 1].key = ESCAPE;
		se[i - 1].d_color = TERM_L_DARK;
		se[i - 1].l_color = TERM_WHITE;

		n = get_selection(se, slot_num + 1, 0, 1, 22, 1, 30, NULL, GET_SE_NO_FRAME | GET_SE_AUTO_WIDTH | GET_SE_AUTO_HEIGHT | GET_SE_RIGHT);
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

	if (easy_floor || use_menu) return get_item_floor(creature_ptr, cp, pmt, str, mode, hook, item_tester_tval);

	// Extract args
	if (mode & USE_EQUIP) equip = TRUE;
	if (mode & USE_INVEN) inven = TRUE;
	if (mode & USE_FLOOR) floor = TRUE;
	if (mode & USE_EQUIP_SLOT) equip_slot = TRUE;
	if (mode & USE_FORCE) select_the_force = TRUE;

	// Get the item index
	if (repeat_pull(cp))
	{
		// the_force
		if (select_the_force && (*cp == INVEN_FORCE))
		{
			item_tester_tval = 0;
			hook = NULL;
			command_cmd = 0; // Hack -- command_cmd is no longer effective
			return (TRUE);
		}

		// Floor item?
		else if (floor && (*cp < 0))
		{
			object_type *object_ptr;

			// Special index
			k = 0 - (*cp);

			// Acquire object
			object_ptr = &object_list[k];

			// Validate the item
			if (item_tester_okay(creature_ptr, object_ptr, hook, item_tester_tval))
			{
				// Forget restrictions
				item_tester_tval = 0;
				hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}

		else if ((*cp >= 0) && (*cp < INVEN_TOTAL))
		{
			if (prev_tag && command_cmd)
			{
				// Look up the tag and validate the item
				if (!get_tag(creature_ptr, &k, prev_tag, !IS_EQUIPPED(&creature_ptr->inventory[*cp]) ? USE_EQUIP : USE_INVEN));
				else if (!!IS_EQUIPPED(&creature_ptr->inventory[*cp]) ? !inven : !equip);
				else if (!get_item_okay(creature_ptr, k, hook, item_tester_tval));
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
			else if (get_item_okay(creature_ptr, *cp, hook, item_tester_tval))
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
	if (!inven) i2 = -1;
	else if (use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
			if (item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_inven++;
	}

	// Restrict inventory indexes
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i1, hook, item_tester_tval))) i1++;
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i2, hook, item_tester_tval))) i2--;

	// Full equipment
	e1 = 0;
	e2 = INVEN_TOTAL - 1;

	// Forbid equipment
	if (!equip) e2 = -1;

	else if (use_menu)
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
	if (floor)
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
			if (item_tester_okay(creature_ptr, object_ptr, hook, item_tester_tval) && (object_ptr->marked & OM_FOUND)) allow_floor = TRUE;
		}
	}

	// Require at least one legal choice
	if (!allow_floor && (i1 > i2) && (e1 > e2))
	{
		command_see = FALSE; // Cancel creature_ptr->command_see
		oops = TRUE; // Oops
		done = TRUE; // Done

		if (select_the_force) {
		    *cp = INVEN_FORCE;
		    item = TRUE;
		}
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if (command_see && command_wrk && equip)
		{
			command_wrk = TRUE;
		}

		/* Use creature_ptr->inventory if allowed */
		else if (inven)
		{
			command_wrk = FALSE;
		}

		/* Use equipment if allowed */
		else if (equip)
		{
			command_wrk = TRUE;
		}

		/* Use creature_ptr->inventory for floor */
		else
		{
			command_wrk = FALSE;
		}
	}

	if ((always_show_list == TRUE) || use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if (command_see)
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
			if (!angband_term[j]) continue;

			/* Count windows displaying inven */
			if (window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if (window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if ((command_wrk && ni && !ne) ||
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
		window_stuff();


		/* inventory screen */
		if (!command_wrk)
		{
			/* Redraw if needed */
			if (command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_INVENTORY, hook);
		}

		/* Equipment screen */
		else
		{
			/* Redraw if needed */
			if (command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_EQUIPMENT, hook);
		}

		/* Viewing creature_ptr->inventory */
		if (!command_wrk)
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "������:");
#else
			sprintf(out_val, "Inven:");
#endif

			/* Some legal items */
			if ((i1 <= i2) && !use_menu)
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
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
#ifdef JP
			if (equip) strcat(out_val, format(" %s �����i,", use_menu ? "'4'or'6'" : "'/'"));
#else
			if (equip) strcat(out_val, format(" %s for Equip,", use_menu ? "4 or 6" : "/"));
#endif
		}

		/* Viewing equipment */
		else
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "�����i:");
#else
			sprintf(out_val, "Equip:");
#endif

			/* Some legal items */
			if ((e1 <= e2) && !use_menu)
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
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see) strcat(out_val, " * to see,");
#endif

			/* Append */
#ifdef JP
			if (inven) strcat(out_val, format(" %s ������,", use_menu ? "'4'or'6'" : "'/'"));
#else
			if (inven) strcat(out_val, format(" %s for Inven,", use_menu ? "4 or 6" : "'/'"));
#endif
		}

		// Indicate legality of the "floor" item
#ifdef JP
		if (allow_floor) strcat(out_val, " '-'����,");
		if (select_the_force) strcat(out_val, " 'w'���C�p,");
#else
		if (allow_floor) strcat(out_val, " - for floor,");
		if (select_the_force) strcat(out_val, " w for the Force,");
#endif

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);

		/* Get a key */
		which = inkey();

		if (use_menu)
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
				if (!inven || !equip)
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				command_wrk = !command_wrk;
				max_line = (command_wrk ? max_equip : max_inven);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if (command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if (!get_item_okay(creature_ptr, get_item_label, hook, item_tester_tval))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if (!get_item_allow(creature_ptr, get_item_label))
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
				if (select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}
			}
		}
		if (menu_line > max_line) menu_line -= max_line;
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
				if (command_see)
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
				if (!inven || !equip)
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
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
				if (allow_floor)
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
						if (!item_tester_okay(creature_ptr, object_ptr, hook, item_tester_tval)) continue;

						/* Special index */
						k = 0 - this_object_idx;

						/* Verify the item (if required) */
#ifdef JP
if (other_query_flag && !verify(creature_ptr, "�{����", k)) continue;
#else
						if (other_query_flag && !verify("Try", k)) continue;
#endif


						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(creature_ptr, k)) continue;

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
						break;
					}

					/* Outer break */
					if (done) break;
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
				if (!get_tag(creature_ptr, &k, which, command_wrk ? USE_EQUIP : USE_INVEN))
				{
					bell();
					break;
				}

				/* Hack -- Validate the item */
				if (!!IS_EQUIPPED(&creature_ptr->inventory[k]) ? !inven : !equip)
				{
					bell();
					break;
				}

				/* Validate the item */
				if (!get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(creature_ptr, k))
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
				if (select_the_force) {
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
				if (!get_tag(creature_ptr, &k, which, command_wrk ? USE_EQUIP : USE_INVEN))
				{
					not_found = TRUE;
				}

				/* Hack -- Validate the item */
				else if (!IS_EQUIPPED(&creature_ptr->inventory[k]) ? !inven : !equip)
				{
					not_found = TRUE;
				}

				/* Validate the item */
				else if (!get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					not_found = TRUE;
				}

				if (!not_found)
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
				if (!command_wrk)
				{
					if (which == '(') k = i1;
					else if (which == ')') k = i2;
					else k = label_to_item(creature_ptr, which);
				}

				/* Convert letter to equipment index */
				else
				{
					if (which == '(') k = e1;
					else if (which == ')') k = e2;
					else k = label_to_item(creature_ptr, which);
				}

				/* Validate the item */
				if (!get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					bell();
					break;
				}

				/* Verify the item */
#ifdef JP
				if (ver && !verify(creature_ptr, "�{����", k))
#else
				if (ver && !verify("Try", k))
#endif

				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(creature_ptr, k))
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
	if (command_see)
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
	if (toggle) toggle_inven_equip();

	/* Update */
	play_window |= (PW_INVEN | PW_EQUIP);

	/* Window stuff */
	window_stuff();


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if (oops && str) msg_print(str);

	if (item)
	{
		repeat_push(*cp);
		if (command_cmd) prev_tag = cur_tag;

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

	if (!in_bounds(floor_ptr, y, x)) return 0; // Sanity

	/* Scan all objects in the grid */
	for (this_object_idx = floor_ptr->cave[y][x].object_idx; this_object_idx; this_object_idx = next_object_idx)
	{
		object_type *object_ptr;

		/* Acquire object */
		object_ptr = &object_list[this_object_idx];

		/* Acquire next object */
		next_object_idx = object_ptr->next_object_idx;

		/* Item tester */
		if ((mode & 0x01) && !item_tester_okay(player_ptr, object_ptr, NULL, 0)) continue;

		/* Marked */
		if ((mode & 0x02) && !(object_ptr->marked & OM_FOUND)) continue;

		/* Accept this item */
		/* XXX Hack -- Enforce limit */
		if (num < 23)
			items[num] = this_object_idx;

		num++;

		/* Only one */
		if (mode & 0x04) break;
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
		if (show_weights) l += 9;

		if (object_ptr->tval != TV_GOLD) dont_need_to_show_weights = FALSE;

		/* Maintain the maximum length */
		if (l > len) len = l;

		/* Advance to next "line" */
		k++;
	}

	if (show_weights && dont_need_to_show_weights) len -= 9;

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

		if (use_menu && target_item)
		{
			if (j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "�t");
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
		if (show_weights && (object_ptr->tval != TV_GOLD))
		{
			char weight[80];
			int wgt = object_ptr->weight * object_ptr->number;
			format_weight(weight, wgt);
			sprintf(tmp_val, "%10s", weight);

			prt(tmp_val, j + 1, wid - 10);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

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
	if (repeat_pull(cp))
	{
		/* the_force */
		if (select_the_force && (*cp == INVEN_FORCE))
		{
			item_tester_tval = 0;
			hook = NULL;
			command_cmd = 0; /* Hack -- command_cmd is no longer effective */
			return (TRUE);
		}

		/* Floor item? */
		else if (floor && (*cp < 0))
		{
			if (prev_tag && command_cmd)
			{
				/* Scan all objects in the grid */
				floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);

				/* Look up the tag */
				if (get_tag_floor(&k, prev_tag, floor_list, floor_num))
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
			else if (item_tester_okay(creature_ptr, &object_list[0 - (*cp)], hook, item_tester_tval))
			{
				/* Forget restrictions */
				item_tester_tval = 0;
				hook = NULL;
				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return TRUE;
			}
		}

		else if (*cp >= 0 && *cp < INVEN_TOTAL)
		{
			object_type *object_ptr = &creature_ptr->inventory[*cp];
			if (prev_tag && command_cmd)
			{
				/* Look up the tag and validate the item */
				if (!get_tag(creature_ptr, &k, prev_tag, IS_EQUIPPED(object_ptr) ? USE_EQUIP : USE_INVEN)) /* Reject */;
				else if (!IS_EQUIPPED(object_ptr) ? !inven : !equip) /* Reject */;
				else if (!get_item_okay(creature_ptr, k, hook, item_tester_tval)) /* Reject */;
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
			else if (get_item_okay(creature_ptr, *cp, hook, item_tester_tval))
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
	if (!inven) i2 = -1;
	else if (use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
			if (item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_inven++;
	}

	/* Restrict creature_ptr->inventory indexes */
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i1, hook, item_tester_tval))) i1++;
	while ((i1 <= i2) && (!get_item_okay(creature_ptr, i2, hook, item_tester_tval))) i2--;


	/* Full equipment */
	e1 = 0;
	e2 = INVEN_TOTAL - 1;

	/* Forbid equipment */
	if (!equip) e2 = -1;
	else if (use_menu)
	{
		for (j = 0; j < INVEN_TOTAL; j++)
		{
			if (!IS_EQUIPPED(&creature_ptr->inventory[j])) continue; // Skip no equipment
			if (item_tester_okay(creature_ptr, &creature_ptr->inventory[j], hook, item_tester_tval)) max_equip++;
		}
	}

	/* Restrict equipment indexes */
	while ((e1 <= e2) && (!get_item_okay(creature_ptr, e1, hook, item_tester_tval))) e1++;
	while ((e1 <= e2) && (!get_item_okay(creature_ptr, e2, hook, item_tester_tval))) e2--;

	/*
	if (equip && creature_ptr->two_handed && !item_tester_no_two_handed)
	{
		if (creature_ptr->can_melee[0])
		{
			if (e2 < INVEN_2) e2 = INVEN_2;
		}
		else if (creature_ptr->can_melee[1]) e1 = INVEN_1;
	}
	*/


	/* Count "okay" floor items */
	floor_num = 0;

	/* Restrict floor usage */
	if (floor)
	{
		/* Scan all objects in the grid */
		floor_num = scan_floor(floor_list, floor_ptr, creature_ptr->fy, creature_ptr->fx, 0x03);
	}

	/* Accept inventory */
	if (i1 <= i2) allow_inven = TRUE;

	/* Accept equipment */
	if (e1 <= e2) allow_equip = TRUE;

	/* Accept floor */
	if (floor_num) allow_floor = TRUE;

	/* Require at least one legal choice */
	if (!allow_inven && !allow_equip && !allow_floor)
	{
		/* Cancel creature_ptr->command_see */
		command_see = FALSE;

		/* Oops */
		oops = TRUE;

		/* Done */
		done = TRUE;

		if (select_the_force) {
		    *cp = INVEN_FORCE;
		    item = TRUE;
		}
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if (command_see && (command_wrk == (USE_EQUIP))
			&& allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use creature_ptr->inventory if allowed */
		else if (allow_inven)
		{
			command_wrk = (USE_INVEN);
		}

		/* Use equipment if allowed */
		else if (allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use floor if allowed */
		else if (allow_floor)
		{
			command_wrk = (USE_FLOOR);
		}
	}

	/*
	 * �ǉ��I�v�V����(always_show_list)���ݒ肳��Ă���ꍇ�͏�Ɉꗗ��\������
	 */
	if ((always_show_list == TRUE) || use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if (command_see)
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
			if (!angband_term[j]) continue;

			/* Count windows displaying inven */
			if (window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if (window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if ((command_wrk == (USE_EQUIP) && ni && !ne) ||
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
		window_stuff();

		/* creature_ptr->inventory screen */
		if (command_wrk == (USE_INVEN))
		{
			/* Extract the legal requests */
			n1 = I2A(i1);
			n2 = I2A(i2);

			/* Redraw if needed */
			if (command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_INVENTORY, hook);
		}

		/* Equipment screen */
		else if (command_wrk == (USE_EQUIP))
		{
			/* Extract the legal requests */
			n1 = I2A(e1);
			n2 = I2A(e2);

			/* Redraw if needed */
			if (command_see) get_item_label = show_item_list(menu_line, creature_ptr, SHOW_ITEM_RIGHT_SET | SHOW_ITEM_EQUIPMENT, hook);
		}

		/* Floor screen */
		else if (command_wrk == (USE_FLOOR))
		{
			j = floor_top;
			k = MIN(floor_top + 23, floor_num) - 1;

			/* Extract the legal requests */
			n1 = I2A(j - floor_top);
			n2 = I2A(k - floor_top);

			/* Redraw if needed */
			if (command_see) get_item_label = show_floor(floor_ptr, menu_line, creature_ptr->fy, creature_ptr->fx, &min_width);
		}

		/* Viewing creature_ptr->inventory */
		if (command_wrk == (USE_INVEN))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "������:");
#else
			sprintf(out_val, "Inven:");
#endif

			if (!use_menu)
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
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
			if (allow_equip)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '/' �����i,");
				else if (allow_floor)
					strcat(out_val, " '6' �����i,");
				else
					strcat(out_val, " '4'or'6' �����i,");
#else
				if (!use_menu)
					strcat(out_val, " / for Equip,");
				else if (allow_floor)
					strcat(out_val, " 6 for Equip,");
				else
					strcat(out_val, " 4 or 6 for Equip,");
#endif
			}

			/* Append */
			if (allow_floor)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '-'����,");
				else if (allow_equip)
					strcat(out_val, " '4' ����,");
				else
					strcat(out_val, " '4'or'6' ����,");
#else
				if (!use_menu)
					strcat(out_val, " - for floor,");
				else if (allow_equip)
					strcat(out_val, " 4 for floor,");
				else
					strcat(out_val, " 4 or 6 for floor,");
#endif
			}
		}

		/* Viewing equipment */
		else if (command_wrk == (USE_EQUIP))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "�����i:");
#else
			sprintf(out_val, "Equip:");
#endif

			if (!use_menu)
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
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			/* Append */
			if (allow_inven)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '/' ������,");
				else if (allow_floor)
					strcat(out_val, " '4' ������,");
				else
					strcat(out_val, " '4'or'6' ������,");
#else
				if (!use_menu)
					strcat(out_val, " / for Inven,");
				else if (allow_floor)
					strcat(out_val, " 4 for Inven,");
				else
					strcat(out_val, " 4 or 6 for Inven,");
#endif
			}

			/* Append */
			if (allow_floor)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '-'����,");
				else if (allow_inven)
					strcat(out_val, " '6' ����,");
				else
					strcat(out_val, " '4'or'6' ����,");
#else
				if (!use_menu)
					strcat(out_val, " - for floor,");
				else if (allow_inven)
					strcat(out_val, " 6 for floor,");
				else
					strcat(out_val, " 4 or 6 for floor,");
#endif
			}
		}

		/* Viewing floor */
		else if (command_wrk == (USE_FLOOR))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "����:");
#else
			sprintf(out_val, "Floor:");
#endif

			if (!use_menu)
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
			if (!command_see && !use_menu) strcat(out_val, " '*'�ꗗ,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif

			if (use_menu)
			{
				if (allow_inven && allow_equip)
				{
#ifdef JP
					strcat(out_val, " '4' �����i, '6' ������,");
#else
					strcat(out_val, " 4 for Equip, 6 for Inven,");
#endif
				}
				else if (allow_inven)
				{
#ifdef JP
					strcat(out_val, " '4'or'6' ������,");
#else
					strcat(out_val, " 4 or 6 for Inven,");
#endif
				}
				else if (allow_equip)
				{
#ifdef JP
					strcat(out_val, " '4'or'6' �����i,");
#else
					strcat(out_val, " 4 or 6 for Equip,");
#endif
				}
			}
			/* Append */
			else if (allow_inven)
			{
#ifdef JP
				strcat(out_val, " '/' ������,");
#else
				strcat(out_val, " / for Inven,");
#endif
			}
			else if (allow_equip)
			{
#ifdef JP
				strcat(out_val, " '/'�����i,");
#else
				strcat(out_val, " / for Equip,");
#endif
			}

			/* Append */
			if (command_see && !use_menu)
			{
#ifdef JP
				strcat(out_val, " Enter ��,");
#else
				strcat(out_val, " Enter for scroll down,");
#endif
			}
		}

		/* Append */
#ifdef JP
		if (select_the_force) strcat(out_val, " 'w'���C�p,");
#else
		if (select_the_force) strcat(out_val, " w for the Force,");
#endif

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);

		/* Get a key */
		which = inkey();

		if (use_menu)
		{
		int max_line = 1;
		if (command_wrk == USE_INVEN) max_line = max_inven;
		else if (command_wrk == USE_EQUIP) max_line = max_equip;
		else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
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
				if (command_wrk == (USE_INVEN))
				{
					if (allow_floor) command_wrk = USE_FLOOR;
					else if (allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (allow_inven) command_wrk = USE_INVEN;
					else if (allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_equip) command_wrk = USE_EQUIP;
					else if (allow_inven) command_wrk = USE_INVEN;
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
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				if (command_wrk == USE_INVEN) max_line = max_inven;
				else if (command_wrk == USE_EQUIP) max_line = max_equip;
				else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case '6':
			case 'l':
			case 'L':
			{
				/* Verify legality */
				if (command_wrk == (USE_INVEN))
				{
					if (allow_equip) command_wrk = USE_EQUIP;
					else if (allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (allow_floor) command_wrk = USE_FLOOR;
					else if (allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_inven) command_wrk = USE_INVEN;
					else if (allow_equip) command_wrk = USE_EQUIP;
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
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				if (command_wrk == USE_INVEN) max_line = max_inven;
				else if (command_wrk == USE_EQUIP) max_line = max_equip;
				else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if (command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if (!get_item_okay(creature_ptr, get_item_label, hook, item_tester_tval))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if (!get_item_allow(creature_ptr, get_item_label))
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
				if (select_the_force) {
					*cp = INVEN_FORCE;
					item = TRUE;
					done = TRUE;
					break;
				}
			}
		}
		if (menu_line > max_line) menu_line -= max_line;
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
				if (command_see)
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

				if (command_wrk != (USE_FLOOR)) break;

				/* Get the object being moved. */
				object_idx = c_ptr->object_idx;

				/* Only rotate a pile of two or more objects. */
				if (!(object_idx && object_list[object_idx].next_object_idx)) break;

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
				if (command_see)
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
				if (command_wrk == (USE_INVEN))
				{
					if (!allow_equip)
					{
						bell();
						break;
					}
					command_wrk = (USE_EQUIP);
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (!allow_inven)
					{
						bell();
						break;
					}
					command_wrk = (USE_INVEN);
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_inven)
					{
						command_wrk = (USE_INVEN);
					}
					else if (allow_equip)
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
				if (command_see)
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
				if (!allow_floor)
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
				if (floor_num == 1)
				{
					if ((command_wrk == (USE_FLOOR)) || (!carry_query_flag))
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(creature_ptr, k))
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
				if (command_see)
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
				if (command_wrk != USE_FLOOR)
				{
					/* Look up the tag */
					if (!get_tag(creature_ptr, &k, which, command_wrk))
					{
						bell();
						break;
					}

					/* Validate the item */
					if (!get_item_okay(creature_ptr, k, hook, item_tester_tval))
					{
						bell();
						break;
					}
				}
				else
				{
					/* Look up the alphabetical tag */
					if (get_tag_floor(&k, which, floor_list, floor_num))
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
				if (!get_item_allow(creature_ptr, k))
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
				if (command_wrk == (USE_INVEN))
				{
					k = ((i1 == i2) ? i1 : -1);
				}

				/* Choose "default" equipment item */
				else if (command_wrk == (USE_EQUIP))
				{
					k = ((e1 == e2) ? e1 : -1);
				}

				/* Choose "default" floor item */
				else if (command_wrk == (USE_FLOOR))
				{
					if (floor_num == 1)
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(creature_ptr, k))
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
				if (!get_item_okay(creature_ptr, k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(creature_ptr, k))
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
				if (select_the_force) {
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

				if (command_wrk != USE_FLOOR)
				{
					bool not_found = FALSE;

					/* Look up the alphabetical tag */
					if (!get_tag(creature_ptr, &k, which, command_wrk))
					{
						not_found = TRUE;
					}

					/* Hack -- Validate the item */
					else if (!!IS_EQUIPPED(&creature_ptr->inventory[k]) ? !inven : !equip)
					{
						not_found = TRUE;
					}

					/* Validate the item */
					else if (!get_item_okay(creature_ptr, k, hook, item_tester_tval))
					{
						not_found = TRUE;
					}

					if (!not_found)
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
					if (get_tag_floor(&k, which, floor_list, floor_num))
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
				if (command_wrk == (USE_INVEN) || command_wrk == (USE_EQUIP))
				{
					if (which == '(') k = i1;
					else if (which == ')') k = i2;
					else k = label_to_item(creature_ptr, which);
				}

				/* Convert letter to floor index */
				else if (command_wrk == USE_FLOOR)
				{
					if (which == '(') k = 0;
					else if (which == ')') k = floor_num - 1;
					else k = islower(which) ? A2I(which) : -1;
					if (k < 0 || k >= floor_num || k >= 23)
					{
						bell();
						break;
					}

					/* Special index */
					k = 0 - floor_list[k];
				}

				/* Validate the item */
				if ((command_wrk != USE_FLOOR) && !get_item_okay(creature_ptr, k, hook, item_tester_tval))
				{
					bell();
					break;
				}

				/* Verify the item */
#ifdef JP
				if (ver && !verify(creature_ptr, "�{����", k))
#else
				if (ver && !verify("Try", k))
#endif

				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(creature_ptr, k))
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
	if (command_see)
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
	if (toggle) toggle_inven_equip();

	/* Update */
	play_window |= (PW_INVEN | PW_EQUIP);

	/* Window stuff */
	window_stuff();


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if (oops && str) msg_print(str);

	if (item)
	{
		repeat_push(*cp);
		if (command_cmd) prev_tag = cur_tag;
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
	q = "�ǂ���E���܂����H";
	s = "�����U�b�N�ɂ͏��ɂ���ǂ̃A�C�e��������Ȃ��B";
#else
	q = "Get which item? ";
	s = "You no longer have any room for the objects on the floor.";
#endif

	if (get_item(creature_ptr, &item, q, s, (USE_FLOOR), inven_carry_okay, 0))
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
		if (object_ptr->tval == TV_GOLD)
		{
			/* Message */
#ifdef JP
		msg_format(" $%ld �̉��l������%s���������B",
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
		else if (object_ptr->marked & OM_NOMSG)
		{
			/* If 0 or 1 non-NOMSG items are in the pile, the NOMSG ones are
			 * ignored. Otherwise, they are included in the prompt. */
			object_ptr->marked &= ~(OM_NOMSG);
			continue;
		}

		/* Count non-gold objects that can be picked up. */
		if (inven_carry_okay(creature_ptr, object_ptr))
		{
			can_pickup++;
		}

		/* Remember this object index */
		if (floor_num < 23)
			floor_list[floor_num] = this_object_idx;

		/* Count non-gold objects */
		floor_num++;

		/* Remember this index */
		floor_object_idx = this_object_idx;
	}

	/* There are no non-gold objects */
	if (!floor_num)
		return;

	/* Mention the number of objects */
	if (!pickup)
	{
		/* One object */
		if (floor_num == 1)
		{
			/* Access the object */
			object_ptr = &object_list[floor_object_idx];

#ifdef ALLOW_EASY_SENSE

			/* Option: Make object sensing easy */
			if (easy_sense)
			{
				/* Sense the object */
				(void) sense_object(object_ptr);
			}

#endif /* ALLOW_EASY_SENSE */

			/* Describe the object */
			object_desc(object_name, object_ptr, 0);

			/* Message */
#ifdef JP
			msg_format("%s������B", object_name);
#else
			msg_format("You see %s.", object_name);
#endif

		}

		/* Multiple objects */
		else
		{
			/* Message */
#ifdef JP
			msg_format("%d ��̃A�C�e��������B", floor_num);
#else
			msg_format("You see a pile of %d items.", floor_num);
#endif

		}

		/* Done */
		return;
	}

	/* The player has no room for anything on the floor. */
	if (!can_pickup)
	{
		/* One object */
		if (floor_num == 1)
		{
			/* Access the object */
			object_ptr = &object_list[floor_object_idx];

#ifdef ALLOW_EASY_SENSE

			/* Option: Make object sensing easy */
			if (easy_sense)
			{
				/* Sense the object */
				(void) sense_object(object_ptr);
			}

#endif /* ALLOW_EASY_SENSE */

			/* Describe the object */
			object_desc(object_name, object_ptr, 0);

			/* Message */
#ifdef JP
				msg_format("�U�b�N�ɂ�%s�����錄�Ԃ��Ȃ��B", object_name);
#else
			msg_format("You have no room for %s.", object_name);
#endif

		}

		/* Multiple objects */
		else
		{
			/* Message */
#ifdef JP
			msg_format("�U�b�N�ɂ͏��ɂ���ǂ̃A�C�e��������Ȃ��B", object_name);
#else
			msg_print("You have no room for any of the objects on the floor.");
#endif

		}

		/* Done */
		return;
	}

	/* One object */
	if (floor_num == 1)
	{
		/* Hack -- query every object */
		if (carry_query_flag)
		{
			char out_val[MAX_NLEN+20];

			/* Access the object */
			object_ptr = &object_list[floor_object_idx];

#ifdef ALLOW_EASY_SENSE

			/* Option: Make object sensing easy */
			if (easy_sense)
			{
				/* Sense the object */
				(void) sense_object(object_ptr);
			}

#endif /* ALLOW_EASY_SENSE */

			/* Describe the object */
			object_desc(object_name, object_ptr, 0);

			/* Build a prompt */
#ifdef JP
			(void) sprintf(out_val, "%s���E���܂���? ", object_name);
#else
			(void) sprintf(out_val, "Pick up %s? ", object_name);
#endif


			/* Ask the user to confirm */
			if (!get_check(out_val))
			{
				/* Done */
				return;
			}
		}

		/* Access the object */
		object_ptr = &object_list[floor_object_idx];

#ifdef ALLOW_EASY_SENSE

		/* Option: Make object sensing easy */
		if (easy_sense)
		{
			/* Sense the object */
			(void) sense_object(object_ptr);
		}

#endif /* ALLOW_EASY_SENSE */

		/* Pick up the object */
		py_pickup_aux(creature_ptr, floor_object_idx);
	}

	/* Allow the user to choose an object */
	else
	{
		while (can_pickup--)
		{
			if (!py_pickup_floor_aux(creature_ptr)) break;
		}
	}
}
