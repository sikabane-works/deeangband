// File: trait.c

/*
* Copyright (c) 2012 Deskull
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*
*/

#include "angband.h"
#include "command.h"
#include "creature_melee.h"
#include "creature_throwing.h"
#include "diary.h"
#include "files.h"
#include "floors.h"
#include "riding.h"
#include "mutation.h"

bool do_active_trait_tmp(creature_type *caster_ptr, TRAIT_ID id, bool message)
{
	return do_active_trait(caster_ptr, id, message, 100, 0L);
}

bool do_active_trait(creature_type *caster_ptr, TRAIT_ID id, bool message, POWER power, FLAGS_32 option)
{
	trait_type *trait_ptr = &trait_info[id];
	floor_type *floor_ptr = GET_FLOOR_PTR(caster_ptr);
	creature_type *target_ptr = NULL;
	cave_type *cave_ptr = &floor_ptr->cave[caster_ptr->fy][caster_ptr->fx];
	feature_type *feature_ptr = &feature_info[cave_ptr->feat];
	bool effected = FALSE;

	char caster_name[100] = KW_IT, target_name[100] = KW_IT;
	int i, k;
	int rad = 3;
	DIRECTION dir = 0;
	CREATURE_LEV user_level = caster_ptr->lev;
	POWER damage = 0;
	FLAGS_32 mode = (PC_ALLOW_GROUP | PC_FORCE_PET);
	FLAGS_32 u_mode = 0L;

	int count = 0;

	COODINATES x = 0, y = 0;

	bool kichigai_talk = FALSE;
	bool summoned = FALSE;
	bool fumble_summoned = FALSE;

	cptr summoned_name = "";

	// process flags
	bool blind = (has_trait(player_ptr, TRAIT_BLIND) ? TRUE : FALSE);

	if(randint1(50 + user_level) < user_level / 10) u_mode = PC_ALLOW_UNIQUE;

	creature_desc(caster_name, caster_ptr, 0);

	if(!is_player(caster_ptr)) disturb(player_ptr, 1, 0);

	if(trait_info[id].effect_type == TRAIT_EFFECT_TYPE_TARGET)
	{
		if(!get_aim_dir(caster_ptr, MAX_RANGE_SUB, &dir)) return FALSE;
		y = target_row;
		x = target_col;
	}
	else if(trait_info[id].effect_type == TRAIT_EFFECT_TYPE_ADJACENCY)
	{
		if(!get_rep_dir2(caster_ptr, &dir)) return FALSE;	
		y = caster_ptr->fy + ddy[dir];
		x = caster_ptr->fx + ddx[dir];
		cave_ptr = &floor_ptr->cave[y][x];
	}
	else if(trait_info[id].effect_type == TRAIT_EFFECT_TYPE_ADJACENCY2)
	{
		if(!get_rep_dir(caster_ptr, &dir, FALSE)) return FALSE;
		y = caster_ptr->fy + ddy[dir];
		x = caster_ptr->fx + ddx[dir];
	}

	if(message)
	{
		if(is_player(caster_ptr) || !blind)
		{
			if(strlen(trait_text + trait_ptr->activate_text))
				msg_format(trait_text + trait_ptr->activate_text, caster_name);
			else		
				msg_format(MES_TRAIT_DEFAULT_ACTIVATE(caster_name, trait_info[id].title));
		}
		else
		{
			if(strlen(trait_text + trait_ptr->blind_activate_text))
				msg_format(trait_text + trait_ptr->blind_activate_text, caster_name);
			else		
				msg_format(MES_TRAIT_DEFAULT_ACTIVATE(caster_name, trait_info[id].title));
		}
	}


	if(option & AT_OPTION_USE_MOUTH) stop_mouth(caster_ptr);

	switch(id)
	{

		/* Mass Spells */

	case TRAIT_DISPEL_EVIL_1: project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, power); break;
	case TRAIT_DISPEL_GOOD_1: project_all_vision(caster_ptr, DO_EFFECT_DISP_GOOD, power); break;
	case TRAIT_BANISH_EVIL: project_all_vision(caster_ptr, DO_EFFECT_AWAY_EVIL, power); break;
	case TRAIT_DISPEL_LIVES: project_all_vision(caster_ptr, DO_EFFECT_DISP_LIVING, randint1(power)); break;
	case TRAIT_HASTE_OTHERS: project_all_vision(caster_ptr, DO_EFFECT_SPEED_OTHERS, power); break;
	case TRAIT_MASS_SLEEP: project_all_vision(caster_ptr, DO_EFFECT_OLD_SLEEP, power); break;
	case TRAIT_MASS_SLOW: project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, power); break;
	case TRAIT_MIND_WAVE: msg_print(MES_TRAIT_MIND_WAVE); (void)project_all_vision(caster_ptr, DO_EFFECT_PSI, power); break;

	case TRAIT_MOON_DAZZLING:
		msg_print(MES_TRAIT_MOON_DAZZLING_DONE);
		project_all_vision(caster_ptr, DO_EFFECT_ENGETSU, power);
		project_all_vision(caster_ptr, DO_EFFECT_ENGETSU, power);
		project_all_vision(caster_ptr, DO_EFFECT_ENGETSU, power);
		break;

	case TRAIT_HOLINESS:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, 150);
		add_timed_trait(caster_ptr, TRAIT_PROT_EVIL, randint1(25) + 3 * user_level, TRUE);
		heal_creature(caster_ptr, 50);
		break;

	case TRAIT_DISPEL_UNDEAD_DEMON:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_UNDEAD, randint1(power));
		project_all_vision(caster_ptr, DO_EFFECT_DISP_DEMON, randint1(power));
		break;

	case TRAIT_TERROR:
	case TRAIT_LAY_OF_FEAR: project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, 40 + user_level); break;

	case TRAIT_PESTICIDE:
	case TRAIT_DISPEL_SMALL_LIFE: project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, 4); break;

	case TRAIT_NATURE_WRATH:
		{
			int d_dam = 4 * power;
			int b_dam = (100 + power) * 2;
			int b_rad = 1 + power / 12;
			int q_rad = 20 + power / 2;
			project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, d_dam);
			earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, q_rad);
			project(caster_ptr, 0, b_rad, caster_ptr->fy, caster_ptr->fx, b_dam, DO_EFFECT_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM, -1);
		}

	case TRAIT_EXORCISM:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_UNDEAD, power);
		project_all_vision(caster_ptr, DO_EFFECT_DISP_DEMON, power);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_EVIL, power);
		break;

	case TRAIT_HOLY_WORD:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, power);
		heal_creature(caster_ptr, power);
		break;

	case TRAIT_DAZZLE:
		project_all_vision(caster_ptr, DO_EFFECT_STUN, power);
		project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, power);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, power);
		break;

	case TRAIT_MIRROR_SLEEP:
		for(x = 0; x < floor_ptr->width; x++)
		{
			for(y=0; y < floor_ptr->height; y++)
			{
				if(is_mirror_grid(&floor_ptr->cave[y][x]))
				{
					project(caster_ptr, 0, 2, y, x, user_level, DO_EFFECT_OLD_SLEEP, (PROJECT_GRID|PROJECT_ITEM|PROJECT_KILL|PROJECT_JUMP|PROJECT_NO_HANGEKI),-1);
				}
			}
		}
		break;

	case TRAIT_WAVE_NETHER:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, randint1(power));
		project_all_vision(caster_ptr, DO_EFFECT_DISP_GOOD, randint1(power));
		break;

	case TRAIT_DISPEL_ALL: project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, power); break;

	case TRAIT_FRIGHTEN_SOUND:
		if(MUSIC_SINGING_ANY(caster_ptr)) stop_singing(caster_ptr);
		if(HEX_SPELLING_ANY(caster_ptr)) stop_hex_spell_all(caster_ptr);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, (3 * user_level / 2) + 10);
		break;

	case TRAIT_BLAZING_LIGHT:
		SELF_FIELD(caster_ptr, DO_EFFECT_LITE, 300, 6, -1);
		project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, 3 * user_level / 2);
		break;

	case TRAIT_INROU:
		{
			int count = 0, i;
			if(summon_named_creature(0, floor_ptr, caster_ptr->fy, caster_ptr->fx, SPECIES_SUKE, PC_FORCE_PET))
			{
				msg_print(MES_TRAIT_INROU_SUKE);
				count++;
			}
			if(summon_named_creature(0, floor_ptr, caster_ptr->fy, caster_ptr->fx, SPECIES_KAKU, PC_FORCE_PET))
			{
				msg_print(MES_TRAIT_INROU_KAKU);
				count++;
			}
			if(!count)
			{
				for (i = creature_max - 1; i > 0; i--)
				{
					target_ptr = &creature_list[i];
					if(!target_ptr->species_idx) continue;
					if(!((target_ptr->species_idx == SPECIES_SUKE) || (target_ptr->species_idx == SPECIES_KAKU))) continue;
					if(!los(floor_ptr, target_ptr->fy, target_ptr->fx, caster_ptr->fy, caster_ptr->fx)) continue;
					if(!projectable(floor_ptr, MAX_RANGE, target_ptr->fy, target_ptr->fx, caster_ptr->fy, caster_ptr->fx)) continue;
					count++;
					break;
				}
			}

			if(count)
			{
				msg_print(MES_SUMMON_INROU);
				sukekaku = TRUE;
				project_all_vision(caster_ptr, DO_EFFECT_STUN, 120);
				project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, 120);
				project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, 120);
				project_all_vision(caster_ptr, DO_EFFECT_STASIS, 120);
				sukekaku = FALSE;
			}
			else msg_print(MES_NO_HAPPEN);
			break;
		}

	case TRAIT_DIVINE_INTERVENTION:
		{
			int b_dam = power * 11;
			int d_dam = power * 4;
			int heal = 100;

			project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, b_dam, DO_EFFECT_HOLY_FIRE, PROJECT_KILL, -1);
			project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, d_dam);
			project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, power);
			project_all_vision(caster_ptr, DO_EFFECT_STUN, power);
			project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, power);
			project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, power);
			project_all_vision(caster_ptr, DO_EFFECT_STASIS, power);
			heal_creature(caster_ptr, heal);
		}
		break;

		/* Genocide Spells */

	case TRAIT_GENOCIDE_ONE: cast_ball_hide(caster_ptr, DO_EFFECT_GENOCIDE, MAX_RANGE_SUB, power, 0); break;
	case TRAIT_SYMBOL_GENOCIDE: (void)symbol_genocide(caster_ptr, 200, TRUE); break;
	case TRAIT_MASS_GENOCIDE: (void)mass_genocide(caster_ptr, 200, TRUE); break;

	case TRAIT_BANISH:
		if(!cave_ptr->creature_idx)
		{
			msg_print(MES_TRAIT_BANISH_NO_TARGET); break;
		}
		target_ptr = &creature_list[cave_ptr->creature_idx];
		if(is_enemy_of_good_creature(target_ptr) && !(has_trait(target_ptr, TRAIT_QUESTOR)) && !(has_trait(target_ptr, TRAIT_UNIQUE)) &&
			!floor_ptr->fight_arena_mode && !floor_ptr->quest && (user_level < randint1(user_level)) && !has_trait(target_ptr, TRAIT_NO_GENOCIDE))
		{
			if(record_named_pet && is_pet(player_ptr, target_ptr) && target_ptr->nickname)
			{
				creature_desc(target_name, target_ptr, CD_INDEF_VISIBLE);
				write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_GENOCIDE, target_name);
			}
			delete_creature(&creature_list[cave_ptr->creature_idx]);
			msg_print(MES_TRAIT_BANISH_DONE);
		}
		else
		{
			msg_print(MES_TRAIT_BANISH_UNAFFECTED);
			if(one_in_(13)) set_timed_trait(target_ptr, TRAIT_NO_GENOCIDE, PERMANENT_TIMED, FALSE);
		}
		break;

		/* Charm Spells */

	case TRAIT_CHARM_ANIMAL: cast_ball(caster_ptr, DO_EFFECT_CONTROL_ANIMAL, MAX_RANGE_SUB, user_level, 0); break;
	case TRAIT_CHARM_UNDEAD: cast_ball(caster_ptr, DO_EFFECT_CONTROL_UNDEAD, MAX_RANGE_SUB, user_level, 0); break;
	case TRAIT_CHARM_OTHER: cast_ball(caster_ptr, DO_EFFECT_CHARM, MAX_RANGE_SUB, user_level, 0); break;
	case TRAIT_CHARM_ANIMALS: (void)project_all_vision(caster_ptr, DO_EFFECT_CONTROL_ANIMAL, user_level * 2); break;
	case TRAIT_CHARM_OTHERS: project_all_vision(caster_ptr, DO_EFFECT_CHARM, user_level * 2); break;
	case TRAIT_DOMINATE_LIVE: (void)cast_ball_hide(caster_ptr, DO_EFFECT_CONTROL_LIVING, MAX_RANGE_SUB, user_level, 0); break;
	case TRAIT_DOMINATE_LIVES: project_all_vision(caster_ptr, DO_EFFECT_CONTROL_LIVING, user_level); break;
	case TRAIT_DOMINATE_DEMON: cast_ball(caster_ptr, DO_EFFECT_CONTROL_DEMON, MAX_RANGE_SUB, power, 0); break;

		/* Summoning Spell */

	case TRAIT_S_KIN:
		{
			if(caster_ptr->species_idx == SPECIES_SERPENT)
			{
#ifdef JP
				if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
				else msg_format("%^sがダンジョンの主を召喚した。", caster_name);
#else
				if(blind) msg_format("%^s mumbles.", caster_name);
				else msg_format("%^s magically summons guardians of dungeons.", caster_name);
#endif
			}
			else
			{
#ifdef JP
				if(blind) msg_format("%^sが何かをつぶやいた。", caster_name);
				else msg_format("%^sは魔法で%sを召喚した。", caster_name, (has_trait(caster_ptr, TRAIT_UNIQUE) ? "手下" : "仲間"));
#else
				if(blind) msg_format("%^s mumbles.", caster_name);
				else msg_format("%^s magically summons %s %s.", caster_name, m_poss, has_trait(caster_ptr, TRAIT_UNIQUE) ? "minions" : "kin"));
#endif
			}

			switch (caster_ptr->species_idx)
			{
			case SPECIES_RICHARD_STOLENMAN: summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_IE, mode); break;
			case SPECIES_LOUSY: summoning(caster_ptr, y, x, user_level, TRAIT_S_LOUSE, PC_ALLOW_GROUP); break;
			default: summoning(caster_ptr, y, x, user_level, TRAIT_S_KIN, PC_ALLOW_GROUP); break;
			}

			break;
		}

	case TRAIT_GRENADE: summon_named_creature(caster_ptr, floor_ptr, y, x, SPECIES_SHURYUUDAN, mode); break;
	case TRAIT_S_GREATER_DEMON: cast_summon_greater_demon(caster_ptr); break;

	case TRAIT_S_ANIMAL:
	case TRAIT_S_PHANTOM:
	case TRAIT_S_ELEMENTAL:
	case TRAIT_S_DEMON:
	case TRAIT_S_UNDEAD:
	case TRAIT_S_DAWN_LEGION:
	case TRAIT_S_OCTOPUS:
	case TRAIT_S_EAGLE:
	case TRAIT_S_DUNGEON_MASTER:
	case TRAIT_S_LOCKE_CLONE:
	case TRAIT_S_CYBER:
	case TRAIT_S_MONSTER:
	case TRAIT_S_MONSTERS:
	case TRAIT_S_ANT:
	case TRAIT_S_SPIDER:
	case TRAIT_S_HOUND:
	case TRAIT_S_HYDRA:
	case TRAIT_S_ANGEL:
	case TRAIT_S_DRAGON:
	case TRAIT_S_NAZGUL:
	case TRAIT_S_HI_UNDEAD:
	case TRAIT_S_HI_DRAGON:
	case TRAIT_S_AMBERITES:
	case TRAIT_S_UNIQUE:
	case TRAIT_GROW_MOLD:
		(void)summoning(caster_ptr, caster_ptr->fy, caster_ptr->fx, user_level, id, (PC_ALLOW_GROUP | PC_FORCE_PET)); break;

		/* Get Timed Trait Spells */

	case TRAIT_REMOVE_POISON: (void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE); break;
	case TRAIT_GET_ELEMENT_BRAND: choose_ele_attack(caster_ptr); break;
	case TRAIT_GET_IMMUNE: choose_ele_immune(caster_ptr, 10 + randint1(10)); break;

	case TRAIT_RESIST:
	case TRAIT_MAGIC_RES_ELEMENT:
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, randint1(40) + 40, FALSE)) effected = TRUE;
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(40) + 40, FALSE)) effected = TRUE;
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(40) + 40, FALSE)) effected = TRUE;
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(40) + 40, FALSE)) effected = TRUE;
		if(set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, randint1(40) + 40, FALSE)) effected = TRUE;
		break;

	case TRAIT_GET_ULTRA_RESISTANCE:
		set_timed_trait(caster_ptr, TRAIT_FAST, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_ULTRA_RES, power, FALSE);
		break;

	case TRAIT_BECOME_HERO:
		if(set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE)) effected = TRUE;
		if(add_timed_trait(caster_ptr, TRAIT_HERO, randint1(25) + 25, TRUE)) effected = TRUE;
		if(heal_creature(caster_ptr, 10)) effected = TRUE;
		break;

	case TRAIT_BERSERK:
		if(set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE)) effected = TRUE;
		if(add_timed_trait(caster_ptr, TRAIT_S_HERO, randint1(25) + 25, TRUE)) effected = TRUE;
		if(heal_creature(caster_ptr, 30)) effected = TRUE;
		break;

	case TRAIT_DEVIL_CLOAK:
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_AURA_FIRE, power, FALSE);
		set_timed_trait(caster_ptr, TRAIT_AFRAID, power, TRUE);
		break;

	case TRAIT_REMOVE_FEAR: (void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE); break;
	case TRAIT_MAGIC_RES_COLD: (void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(20) + 20, FALSE); break;
	case TRAIT_BLESSING_SELF: set_timed_trait(caster_ptr, TRAIT_BLESSED, power, FALSE); break;
	case TRAIT_GET_ESP: (void)set_timed_trait(caster_ptr, TRAIT_ESP, randint1(30) + 25, FALSE); break;
	case TRAIT_HASTE: set_timed_trait(caster_ptr, TRAIT_FAST, randint1(20) + 20, TRUE); break;
	case TRAIT_HASTE_2: set_timed_trait(caster_ptr, TRAIT_FAST, randint1(75) + 75, TRUE); break;
	case TRAIT_GET_EYE_FOR_EYE: set_timed_trait(caster_ptr, TRAIT_EYE_EYE, randint1(10) + 10, FALSE); break;
	case TRAIT_SLOW_SELF: add_timed_trait(caster_ptr, TRAIT_SLOW, randint1(30) + 15, TRUE); break;
	case TRAIT_WRAITH_FORM: set_timed_trait(caster_ptr, TRAIT_WRAITH_FORM, randint1(user_level / 2) + (user_level / 2), FALSE); break;
	case TRAIT_INVULNER: (void)set_timed_trait(caster_ptr, TRAIT_INVULNERABLE, randint1(7) + 7, FALSE); break;
	case TRAIT_GET_SEE_INVISIBLE: set_timed_trait(caster_ptr, TRAIT_SEE_INVISIBLE, randint1(24) + 24, FALSE); break;
	case TRAIT_GET_SEE_INFRA: set_timed_trait(caster_ptr, TRAIT_SEE_INFRA, randint1(24) + 24, FALSE); break;
	case TRAIT_GET_REGENERATE: set_timed_trait(caster_ptr, TRAIT_REGENERATE, 24 + randint1(24), FALSE); break;
	case TRAIT_GET_PASS_WALL: set_timed_trait(caster_ptr, TRAIT_PASS_WALL, randint1(power) + power, FALSE); break;
	case TRAIT_PROT_EVIL: set_timed_trait(caster_ptr, TRAIT_PROT_EVIL, randint1(25) + 3 * user_level, FALSE); break;
	case TRAIT_GET_LIGHT_SPEED: set_timed_trait(caster_ptr, TRAIT_LIGHT_SPEED, randint1(16) + 16, FALSE); break;
	case TRAIT_GET_DUST_ROBE: set_timed_trait(caster_ptr, TRAIT_DUST_ROBE, 20 + randint1(20), FALSE); break;
	case TRAIT_GET_LEVITATION: set_timed_trait(caster_ptr, TRAIT_LEVITATION, randint1(30) + 30, FALSE); break;
	case TRAIT_GET_RESIST_MAGIC: set_timed_trait(caster_ptr, TRAIT_RESIST_MAGIC, randint1(20) + 20, FALSE); break;
	case TRAIT_GET_AURA_MANA: set_timed_trait(caster_ptr, TRAIT_AURA_MANA, randint1(power) + power, FALSE); break;
	case TRAIT_GET_HOLY_AURA: set_timed_trait(caster_ptr, TRAIT_HOLY_AURA, randint1(power) + power, FALSE); break;
	case TRAIT_GET_RES_NETH: set_timed_trait(caster_ptr, TRAIT_RES_NETH, randint1(power) + power, FALSE); break;
	case TRAIT_GET_RES_TIME: set_timed_trait(caster_ptr, TRAIT_RES_TIME, randint1(power) + power, FALSE); break;
	case TRAIT_GET_CONFUSING_MELEE: set_timed_trait(caster_ptr, TRAIT_CONFUSING_MELEE, PERMANENT_TIMED, TRUE); break;
	case TRAIT_GET_MULTI_SHADOW: set_timed_trait(caster_ptr, TRAIT_MULTI_SHADOW, 6+randint1(6), FALSE); break;
	case TRAIT_GET_MAGIC_DEF: set_timed_trait(caster_ptr, TRAIT_MAGIC_DEF, randint1(power) + power, FALSE); break;

	case TRAIT_MIRROR_SHIELD:
		set_timed_trait(caster_ptr, TRAIT_SHIELD, power, FALSE);
		if(user_level > 31) set_timed_trait(caster_ptr, TRAIT_REFLECTING, power, FALSE);
		if(user_level > 39) set_timed_trait(caster_ptr, TRAIT_RESIST_MAGIC, power, FALSE);
		break;

	case TRAIT_GET_KAWARIMI:
		if(!(caster_ptr->posture & NINJA_KAWARIMI))
		{
			msg_print(MES_TRAIT_NINJA_EVADE);
			caster_ptr->posture |= NINJA_KAWARIMI;
			prepare_redraw(PR_STATUS);
		}
		break;

	case TRAIT_MULTI_BLESS_1:
		(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, FALSE);
		(void)set_timed_trait(caster_ptr, TRAIT_HERO, randint1(50) + 50, FALSE);
		(void)set_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(50) + 50, FALSE);
		(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ACID, randint1(50) + 50, FALSE);
		(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_ELEC, randint1(50) + 50, FALSE);
		(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, randint1(50) + 50, FALSE);
		(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_COLD, randint1(50) + 50, FALSE);
		(void)set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_POIS, randint1(50) + 50, FALSE);
		(void)heal_creature(caster_ptr, 10);
		break;

	case TRAIT_POLYMORPH:
		if(!get_check(MES_POLYSELF_ASK)) return FALSE;
		do_poly_self(caster_ptr);
		break;

	case TRAIT_POLYMORPH_VAMPIRE:
		{
			int base = 10 + power / 2;
			set_timed_trait(caster_ptr, TRAIT_VAMPIRE, base + randint1(base), TRUE);
		}
		break;

	case TRAIT_POLYMORPH_DEMON:
		{
			int base = 10 + power / 2;
			caster_ptr->mimic_race_idx = RACE_DEMON;
			set_timed_trait(caster_ptr, TRAIT_MIMIC, base + randint1(base), FALSE);
		}
		break;

	case TRAIT_POLYMORPH_DEMONLORD:
		{
			int base = 10 + power / 2;
			caster_ptr->mimic_race_idx = RACE_FIEND_LORD;
			set_timed_trait(caster_ptr, TRAIT_MIMIC, base + randint1(base), FALSE);
		}
		break;

		/* Light Spell */

	case TRAIT_ILLUMINE:
	case TRAIT_LIGHT_AREA:
	case TRAIT_ILLUMINATION:
		effected = lite_area(caster_ptr, diceroll(2, 15), 3);
		break;

	case TRAIT_DARKNESS: (void)unlite_area(caster_ptr, 10, 3); break;


		/* Detect Spells */

	case TRAIT_DETECT_INVISIBLE: detect_creatures_invis(caster_ptr, DETECT_RAD_DEFAULT); break;
	case TRAIT_DETECT_MAP: map_area(caster_ptr, DETECT_RAD_MAP); break;
	case TRAIT_DETECT_FELOCITY: detect_creatures_mind(caster_ptr, DETECT_RAD_DEFAULT); break;
	case TRAIT_DETECT_NONLIVING: detect_creatures_nonliving(caster_ptr, DETECT_RAD_DEFAULT); break;
	case TRAIT_DETECT_EVIL: detect_creatures_evil(caster_ptr, DETECT_RAD_DEFAULT); break;
	case TRAIT_DETECT_ALL: detect_all(caster_ptr, DETECT_RAD_DEFAULT); break;
	case TRAIT_DETECT_OBJECT: detect_objects_normal(caster_ptr, DETECT_RAD_DEFAULT); break;
	case TRAIT_DETECT_OBJECT_MAGIC: detect_objects_magic(caster_ptr, DETECT_RAD_DEFAULT); break;
	case TRAIT_DETECT_TRAPS: detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE); break;
	case TRAIT_DETECT_DOOR: (void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT); break;
	case TRAIT_SMELL_MON: detect_creatures_normal(caster_ptr, DETECT_RAD_DEFAULT); break;

	case TRAIT_SMELL_MET:
		detect_treasure(caster_ptr, power / 5);
		detect_objects_gold(caster_ptr, power / 5);
		break;

	case TRAIT_DETECT_DOOR_TRAP:
		(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
		(void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_DETECT_ITEM_GOLD:
		detect_objects_normal(caster_ptr, DETECT_RAD_DEFAULT);
		detect_treasure(caster_ptr, DETECT_RAD_DEFAULT);
		detect_objects_gold(caster_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_MIRROR_SEEING:
		{
			int tmp = is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]) ? 4 : 0;
			if(user_level + tmp > 4) detect_creatures_normal(caster_ptr, DETECT_RAD_DEFAULT);
			if(user_level + tmp > 18) detect_creatures_invis(caster_ptr, DETECT_RAD_DEFAULT);
			if(user_level + tmp > 28) set_timed_trait(caster_ptr, TRAIT_ESP, user_level, FALSE);
			if(user_level + tmp > 38) map_area(caster_ptr, DETECT_RAD_MAP);
			if(tmp == 0 && user_level < 5 ) msg_print(MES_PREVENT_NO_MIRROR);
		}
		break;

	case TRAIT_NATURE_AWARENESS:
		map_area(caster_ptr, DETECT_RAD_MAP);
		detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
		detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
		detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);
		detect_creatures_normal(caster_ptr, DETECT_RAD_DEFAULT);
		break;

	case TRAIT_ENLIGHTENMENT:
		msg_print(MES_DETECT_ENLIGHTMENT);
		wiz_lite(floor_ptr, caster_ptr, FALSE);
		effected = TRUE;
		break;

	case TRAIT_STAR_ENLIGHTENMENT:
		msg_print(MES_DETECT_MORE_ENLIGHTMENT);
		msg_print(NULL);
		wiz_lite(floor_ptr, caster_ptr, FALSE);
		(void)do_inc_stat(caster_ptr, STAT_INT);
		(void)do_inc_stat(caster_ptr, STAT_WIS);
		(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
		(void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_treasure(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_objects_gold(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_objects_normal(caster_ptr, DETECT_RAD_DEFAULT);
		identify_pack(caster_ptr);
		creature_knowledge(caster_ptr);
		effected = TRUE;
		break;

	case TRAIT_CALL_SUNLIGHT:
		{
			POWER dam = 150;
			COODINATES rad = 8;

			SELF_FIELD(caster_ptr, DO_EFFECT_LITE, dam, rad, -1);
			wiz_lite(floor_ptr, caster_ptr, FALSE);

			if(has_trait(caster_ptr, TRAIT_HURT_LITE) && !has_trait(caster_ptr, TRAIT_RES_LITE))
			{
				msg_format(MES_DAMAGE_SUNLIGHT(caster_ptr));
				take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 50, COD_SUNLIGHT, NULL, -1);
			}
		}
		break;

	case TRAIT_JEWEL_OF_JUDGEMENT:
		msg_print(MES_MELEE_JOJ_DONE);
		wiz_lite(floor_ptr, caster_ptr, FALSE);
		msg_print(MES_MELEE_JOJ_STRAIN);
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(3, 8), COD_JOJ_RISK, NULL, -1);

		(void)detect_traps(caster_ptr, DETECT_RAD_DEFAULT, TRUE);
		(void)detect_doors(caster_ptr, DETECT_RAD_DEFAULT);
		(void)detect_stairs(caster_ptr, DETECT_RAD_DEFAULT);

		if(get_check(MES_RECALL_ASK)) (void)word_of_recall(caster_ptr, randint0(21) + 15);
		break;

	case TRAIT_SEARCH_UNIQUE:
		for (i = creature_max - 1; i >= 1; i--) // Access the creature
		{
			target_ptr = &creature_list[i];
			if(!is_valid_creature(target_ptr) && !IS_IN_THIS_FLOOR(target_ptr)) continue;
			if(has_trait(target_ptr, TRAIT_UNIQUE)) msg_format("%s． ", target_ptr->name);
			break;
		}

		/* Identify Spell */

	case TRAIT_IDENTIFY: if(ident_spell(caster_ptr, FALSE)) return TRUE; break;
	case TRAIT_IDENTIFY_TRUE: if(identify_fully(caster_ptr, FALSE)) return TRUE; break;

	case TRAIT_PERILOUS_IDENTIFY:
		if(!ident_spell(caster_ptr, FALSE)) return FALSE;
		if(!dec_mana(caster_ptr, PERILOUS_IDENTIFY_COST));
		{
			msg_print(COD_PERILOUS_IDENTIFY);
			(void)set_timed_trait(caster_ptr, TRAIT_PARALYZED, randint1(5 * randint1(10) + 1), TRUE);
			(void)set_timed_trait(caster_ptr, TRAIT_CONFUSED, randint1(5 * randint1(10) + 1), TRUE);
		}
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(1, 12), MES_PERILOUS_SECRET, NULL, -1);
		if(one_in_(5)) (void)add_timed_trait(caster_ptr, TRAIT_CONFUSED, randint1(10), TRUE);
		if(one_in_(20)) take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, diceroll(4, 10), MES_PERILOUS_SECRET, NULL, -1);
		break;

	case TRAIT_WEAPON_MASTER_JUDGE:
		if(power > 44) if(!identify_fully(caster_ptr, TRUE)) return TRUE;
		else if(!ident_spell(caster_ptr, TRUE)) return TRUE;
		break;

		/* Recharge Spells */
	case TRAIT_MAGIC_CHARGE_2: recharge(caster_ptr, 130); break;
	case TRAIT_MAGIC_CHARGE_EX: if(!recharge(caster_ptr, 1000)) return FALSE; break;

		/* Disarm Type Spell */
	case TRAIT_DESTROY_DOOR_TRAP: project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_KILL_DOOR, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1); break;
	case TRAIT_BEAM_DISARM: cast_beam(caster_ptr, DO_EFFECT_KILL_TRAP, MAX_RANGE_SUB, 0, -1); break;
	case TRAIT_DESTROY_DOOR_TRAP_BEAM: cast_beam(caster_ptr, DO_EFFECT_KILL_DOOR, MAX_RANGE_SUB, 0, -1); break;
	case TRAIT_CREATE_CLONE: cast_bolt(caster_ptr, DO_EFFECT_OLD_CLONE, MAX_RANGE_SUB, 0, -1); break;

	case TRAIT_STAR_DESTROY: if(destroy_area(caster_ptr, caster_ptr->fy, caster_ptr->fx, 13 + (COODINATES)randint0(5), FALSE)) effected = TRUE; break;

		/* Ball Attack Spells */

	case TRAIT_STAR_BALL: cast_ball_aux(y, x, caster_ptr, DO_EFFECT_LITE, 200, 3, id); break;
	case TRAIT_FORCE_FIST: cast_ball(caster_ptr, DO_EFFECT_DISINTEGRATE, MAX_RANGE_SUB, power, 0); break;
	case TRAIT_SMOKE_BALL: cast_ball(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, user_level*3, 3); break;

	case TRAIT_ROCKET:
	case TRAIT_CRIMSON_ROCKET:
		cast_ball(caster_ptr, DO_EFFECT_ROCKET, (caster_ptr->chp / 4) > 800 ? 800 : (caster_ptr->chp / 4), damage, 2); break;

	case TRAIT_ELEMENTAL_BALL:
		{
			POWER dam = 75 + power;
			COODINATES rad = 2;
			int type;
			switch (randint1(4))
			{
			case 1:  type = DO_EFFECT_FIRE; break;
			case 2:  type = DO_EFFECT_ELEC; break;
			case 3:  type = DO_EFFECT_COLD; break;
			default: type = DO_EFFECT_ACID; break;
			}
			cast_ball(caster_ptr, type, MAX_RANGE_SUB, dam, rad);
		}
		break;

	case TRAIT_MANA_BURST: cast_ball(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_HOLYFIRE: cast_ball(caster_ptr, DO_EFFECT_HOLY_FIRE, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_HELLFIRE: cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_LITE: cast_ball(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_DISI: cast_ball(caster_ptr, DO_EFFECT_DISINTEGRATE, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_DRAI: cast_ball(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, power, rad); break;

	case TRAIT_MIRROR_CRASH: cast_ball(caster_ptr, DO_EFFECT_SHARDS, MAX_RANGE_SUB, power, (COODINATES)(user_level > 20 ? (user_level - 20) / 8 + 1 : 0)); break;
	case TRAIT_BA_NUKE: cast_ball(caster_ptr, DO_EFFECT_NUKE, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_CHAO: cast_ball(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_ACID: cast_ball(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, power, rad); break;

	case TRAIT_BA_ELEC_L:
	case TRAIT_BA_ELEC: cast_ball(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, power, rad); break;

	case TRAIT_BA_FIRE_L:
	case TRAIT_BA_FIRE: cast_ball(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, power, rad); break;

	case TRAIT_BA_COLD_L:
	case TRAIT_BA_COLD: cast_ball(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, power, rad); break;

	case TRAIT_BA_POIS: cast_ball(caster_ptr, DO_EFFECT_POIS, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_NETH: cast_ball(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_WATE: cast_ball(caster_ptr, DO_EFFECT_WATER, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_MANA: cast_ball(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BA_DARK: cast_ball(caster_ptr, DO_EFFECT_DARK, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_BAZOOKA: cast_ball(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_SUCCUBUS_KISS: cast_ball(caster_ptr, DO_EFFECT_NEXUS, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_PULVERISE: cast_ball(caster_ptr, DO_EFFECT_TELEKINESIS, MAX_RANGE_SUB, diceroll(8 + ((user_level - 5) / 4), 8), (user_level > 20 ? (user_level - 20) / 8 + 1 : 0)); break;


	/* Breath Attack Spell */

	case TRAIT_BR_DISI: breath(caster_ptr, DO_EFFECT_DISINTEGRATE, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_ELEMENTAL_BREATH: breath(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_ACID: breath(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_ELEC: breath(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_FIRE: breath(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_COLD: breath(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_POIS: breath(caster_ptr, DO_EFFECT_POIS, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_NETH: breath(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_LITE: breath(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_DARK: breath(caster_ptr, DO_EFFECT_DARK, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_CONF: breath(caster_ptr, DO_EFFECT_CONFUSION, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_SOUN: breath(caster_ptr, DO_EFFECT_SOUND, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_CHAO: breath(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_DISE: breath(caster_ptr, DO_EFFECT_DISENCHANT, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_NEXU: breath(caster_ptr, DO_EFFECT_NEXUS, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_TIME: breath(caster_ptr, DO_EFFECT_TIME, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_INER: breath(caster_ptr, DO_EFFECT_INERTIA, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_GRAV: breath(caster_ptr, DO_EFFECT_GRAVITY, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_SHAR: breath(caster_ptr, DO_EFFECT_SHARDS, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_PLAS: breath(caster_ptr, DO_EFFECT_PLASMA, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_WALL: breath(caster_ptr, DO_EFFECT_FORCE, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_MANA: breath(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, power, rad, id); break;
	case TRAIT_BR_NUKE: breath(caster_ptr, DO_EFFECT_NUKE, MAX_RANGE_SUB, power, rad, id); break;

	/* Bolt Type Spell */

	case TRAIT_BO_ACID_MINI:
	case TRAIT_BO_ACID:
		cast_bolt(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, power, TRAIT_BO_ACID); break;

	case TRAIT_BO_ELEC_MINI:
	case TRAIT_BO_ELEC:
	case TRAIT_PUNISHMENT:
		cast_bolt(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, power, TRAIT_BO_ELEC); break;

	case TRAIT_BO_FIRE_MINI:
	case TRAIT_BO_FIRE:
		cast_bolt(caster_ptr, DO_EFFECT_FIRE, MAX_RANGE_SUB, power, TRAIT_BO_FIRE); break;

	case TRAIT_BO_COLD_MINI:
	case TRAIT_BO_COLD:
	case TRAIT_COLD_TOUCH:
		cast_bolt(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, power, TRAIT_BO_COLD); break;

	case TRAIT_POISON_DART: cast_bolt(caster_ptr, DO_EFFECT_POIS, MAX_RANGE_SUB, user_level, 0); break;
	case TRAIT_BO_DARK: cast_bolt(caster_ptr, DO_EFFECT_DARK, MAX_RANGE_SUB, power, beam_chance(caster_ptr)); break;
	case TRAIT_BO_NETH: cast_bolt(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, power, TRAIT_BO_NETH); break;
	case TRAIT_BO_WATE: cast_bolt(caster_ptr, DO_EFFECT_WATER, MAX_RANGE_SUB, power, TRAIT_BO_WATE); break;
	case TRAIT_BO_MANA: cast_bolt(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, power, TRAIT_BO_MANA); break;
	case TRAIT_BO_PLAS: cast_bolt(caster_ptr, DO_EFFECT_PLASMA, MAX_RANGE_SUB, power, TRAIT_BO_PLAS); break;
	case TRAIT_BO_ICEE: cast_bolt(caster_ptr, DO_EFFECT_ICE, MAX_RANGE_SUB, power, TRAIT_BO_ICEE); break;
	case TRAIT_BO_SOUN: cast_bolt(caster_ptr, DO_EFFECT_SOUND, MAX_RANGE_SUB, power, 0); break;
	case TRAIT_BO_CHAO: cast_bolt_or_beam(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, power, beam_chance(caster_ptr)); break;
	case TRAIT_BO_JAM: cast_bolt(caster_ptr, DO_EFFECT_JAM_DOOR, MAX_RANGE_SUB, power, -1); break;
	case TRAIT_MIND_BLST: cast_bolt(caster_ptr, DO_EFFECT_PSI, MAX_RANGE_SUB, power, 0); break;

	case TRAIT_MIRROR_BOLT:
		if(user_level > 9 && is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]) )
		{
			cast_beam(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, diceroll(3+((user_level-1)/5),4), 0);
		}
		else cast_bolt(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, diceroll(3+((user_level-1)/5),4), 0);
		break;

	case TRAIT_HASTE_OTHER: cast_bolt(caster_ptr, DO_EFFECT_SPEED_OTHERS, MAX_RANGE_SUB, user_level, -1); break;

	case TRAIT_MISSILE:
	case TRAIT_RAY_GUN:
	case TRAIT_BLASTER:
		cast_bolt(caster_ptr,DO_EFFECT_MISSILE, MAX_RANGE_SUB, power, 0); break;

	case TRAIT_STRANGLING: cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, power, -1); break;

	case TRAIT_DRAIN_LIFE1:
	case TRAIT_DRAIN_LIFE2:
		cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, power, id);
		break;

	case TRAIT_VAMPIRIC_DRAIN_1:
	case TRAIT_VAMPIRIC_DRAIN_2:
		for (i = 0; i < 3; i++) if(cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, power, id)) heal_creature(caster_ptr, power); break;

	case TRAIT_SCARE_CREATURE:
	case TRAIT_SHOUT:
	case TRAIT_SCARE:
		cast_bolt(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, user_level, -1);
		break;

	case TRAIT_BLIND: cast_bolt(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, user_level * 2, id); break;

	case TRAIT_CONFUSE_TOUCH:
	case TRAIT_PANIC_CREATURE:
	case TRAIT_CONF:
		cast_bolt(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, user_level * 2, id); break;

	case TRAIT_SLOW: cast_bolt(caster_ptr, DO_EFFECT_SLOW_OTHERS, MAX_RANGE_SUB, user_level * 2, id); break;
	case TRAIT_HOLD: cast_bolt(caster_ptr, DO_EFFECT_OLD_SLEEP, MAX_RANGE_SUB, user_level, -1); break;

	/* Curse Attack Spell */

	case TRAIT_PSI_DRAIN: cast_ball(caster_ptr, DO_EFFECT_PSI_DRAIN, dir, power, 0); break;
	case TRAIT_DRAIN_MANA: cast_ball(caster_ptr, DO_EFFECT_DRAIN_MANA, MAX_RANGE_SUB, power, 0); break;
	case TRAIT_MIND_BLAST: cast_ball_hide(caster_ptr, DO_EFFECT_MIND_BLAST, MAX_RANGE_SUB, power, 0); break;
	case TRAIT_BRAIN_SMASH: cast_ball_hide(caster_ptr, DO_EFFECT_BRAIN_SMASH, MAX_RANGE_SUB, power, 0); break;
	case TRAIT_CAUSE_1: cast_ball_hide(caster_ptr, DO_EFFECT_CAUSE_1, MAX_RANGE_SUB, damage, 0); break;
	case TRAIT_CAUSE_2: cast_ball_hide(caster_ptr, DO_EFFECT_CAUSE_2, MAX_RANGE_SUB, damage, 0); break;
	case TRAIT_CAUSE_3: cast_ball_hide(caster_ptr, DO_EFFECT_CAUSE_3, MAX_RANGE_SUB, damage, 0); break;
	case TRAIT_CAUSE_4: cast_ball_hide(caster_ptr, DO_EFFECT_CAUSE_4, MAX_RANGE_SUB, damage, 0); break;
	case TRAIT_HAND_DOOM: cast_ball_hide(caster_ptr, DO_EFFECT_HAND_DOOM, MAX_RANGE_SUB, 200, 0); break;
	case TRAIT_DEATH_RAY: cast_bolt(caster_ptr, DO_EFFECT_DEATH_RAY, MAX_RANGE_SUB, user_level, -1); break;
	case TRAIT_CAUSE_BLOODY_CURSE:
		cast_ball_hide(caster_ptr, DO_EFFECT_BLOOD_CURSE, MAX_RANGE_SUB, 600, 0);
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, 20 + randint1(30), COD_BLOOD_CURSE, NULL, -1);
		break;
	case TRAIT_MALEDICTION:
		cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, MAX_RANGE_SUB, (3 + (power - 1) / 5 * 4), 0);
		if(one_in_(5))
		{
			int effect = randint1(1000);
			if(effect == 666) cast_ball_hide(caster_ptr, DO_EFFECT_DEATH_RAY, MAX_RANGE_SUB, power * 200, 0);
			else if(effect < 500) cast_ball_hide(caster_ptr, DO_EFFECT_TURN_ALL, MAX_RANGE_SUB, power, 0);
			else if(effect < 800) cast_ball_hide(caster_ptr, DO_EFFECT_CONF_OTHERS, MAX_RANGE_SUB, power, 0);
			else cast_ball_hide(caster_ptr, DO_EFFECT_STUN, MAX_RANGE_SUB, power, 0);
		}
		break;

	/* Generate Feature Spell */

	case TRAIT_EXPLOSIVE_RUNE: explosive_rune(caster_ptr); break;
	case TRAIT_PROTECT_RUNE: warding_glyph(caster_ptr); break;
	case TRAIT_STONE_WALL: wall_stone(caster_ptr); break;
	case TRAIT_CREATE_FOREST: project(0, 0, 1, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_MAKE_TREE, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1); break;
	case TRAIT_EARTHQUAKE: earthquake(caster_ptr, caster_ptr->fy, caster_ptr->fx, 10); break;
	case TRAIT_STONE_TO_MUD: cast_bolt(caster_ptr, DO_EFFECT_KILL_WALL, MAX_RANGE_SUB, 20 + randint1(30), -1); break;
	case TRAIT_STAIR_BUILDING: stair_creation(caster_ptr, floor_ptr); break;

	case TRAIT_FLOW_LAVA:
		{
			POWER dam = (55 + power) * 2;
			COODINATES rad = 3;
			SELF_FIELD(caster_ptr, DO_EFFECT_FIRE, dam, rad, -1);
			cast_ball_hide(caster_ptr, DO_EFFECT_LAVA_FLOW, MAX_RANGE_SUB, 2 + randint1(2), rad);
		}

	/* Swarm Spell */

	case TRAIT_METEOR_SWARM: cast_meteor(caster_ptr, power, 2); break;
	case TRAIT_FIRE_SWARM: rengoku_kaen(caster_ptr); break;

	case TRAIT_ACROBAT:
		{
			if(one_in_(3))
			{
				msg_format(MES_TRAIT_ACROBAT(caster_ptr));
				teleport_away(caster_ptr, 10, TELEPORT_NONMAGICAL);
				prepare_update(caster_ptr, PU_CREATURES);
			}
			else
			{
				int get_damage = 0;
				msg_format(MES_TRAIT_ACROBAT_EFFECT(caster_ptr));
				damage = diceroll(4, 8);
				teleport_creature_to(target_ptr, caster_ptr->fy, caster_ptr->fx, TELEPORT_NONMAGICAL | TELEPORT_PASSIVE);
				sound(SOUND_FALL);
				if(has_trait(target_ptr, TRAIT_CAN_FLY)) msg_format(MES_TRAIT_ACROBAT_FLOAT_GENTLY(target_ptr));
				else
				{
					msg_format(MES_TRAIT_ACROBAT_CRUSH(target_ptr));
					damage += diceroll(6, 8);
				}

				// Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.

				get_damage = take_damage_to_creature(NULL, target_ptr, DAMAGE_NOESCAPE, damage, caster_name, NULL, -1);
				if(target_ptr->timed_trait[TRAIT_EYE_EYE] && get_damage > 0 && !gameover)
				{
					msg_format(MES_MELEE_EYE_EYE(caster_ptr));
					project(caster_ptr, 0, 0, caster_ptr->fy, caster_ptr->fx, get_damage, DO_EFFECT_MISSILE, PROJECT_KILL, -1);
					set_timed_trait(target_ptr, TRAIT_EYE_EYE, target_ptr->timed_trait[TRAIT_EYE_EYE]-5, TRUE);
				}

				if(target_ptr->riding) close_combat(caster_ptr, target_ptr->fy, target_ptr->fx, 0);
			}
			break;
		}

	case TRAIT_WRATH_OF_GOD: cast_wrath_of_the_god(caster_ptr, power, 2); break;

	case TRAIT_CRUSADE:
		{
			int base = 25;
			int sp_sides = 20 + power;
			int sp_base = power;

			int i;
			project_all_vision(caster_ptr, DO_EFFECT_CRUSADE, user_level * 4);
			for (i = 0; i < 12; i++)
			{
				int attempt = 10;
				COODINATES my = 0, mx = 0;

				while (attempt--)
				{
					scatter(floor_ptr, &my, &mx, caster_ptr->fy, caster_ptr->fx, 4, 0);
					if(cave_empty_bold2(floor_ptr, my, mx)) break; // Require empty grids
				}
				if(attempt < 0) continue;
				summoning(NULL, my, mx, power, TRAIT_S_KNIGHTS, (PC_ALLOW_GROUP | PC_FORCE_PET | PC_HASTE));
			}
			set_timed_trait(caster_ptr, TRAIT_HERO, randint1(base) + base, FALSE);
			set_timed_trait(caster_ptr, TRAIT_BLESSED, randint1(base) + base, FALSE);
			set_timed_trait(caster_ptr, TRAIT_FAST, randint1(sp_sides) + sp_base, FALSE);
			set_timed_trait(caster_ptr, TRAIT_PROT_EVIL, randint1(base) + base, FALSE);
			set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
		}
		break;

	case TRAIT_TRAPS:
		project(caster_ptr, 0, 1, target_row, target_col, 0, DO_EFFECT_MAKE_TRAP, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
		break;

	case TRAIT_FORGET:
		/* TODO */
		break;

	case TRAIT_ANIM_DEAD:
		project(caster_ptr, 0, 5, caster_ptr->fy, caster_ptr->fx, 0, DO_EFFECT_ANIM_DEAD, PROJECT_ITEM | PROJECT_HIDE, -1);
		break;

		/* Self Damage Spell */

	case TRAIT_FOOD_POISONING_6D6: take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(6, 6), COD_POISONOUS_FOOD, NULL, -1); break;
	case TRAIT_FOOD_POISONING_8D8: take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(8, 8), COD_POISONOUS_FOOD, NULL, -1); break;
	case TRAIT_FOOD_POISONING_10D10: take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(10, 10), COD_POISONOUS_FOOD, NULL, -1); break;

	case TRAIT_SELF_DETONATIONS:
		msg_print(MES_TRAIT_SELF_DETONATION);
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, diceroll(50, 20), COD_POTION_OF_DETONATION, NULL, -1);

		(void)add_timed_trait(caster_ptr, TRAIT_STUN, 75, TRUE);
		(void)add_timed_trait(caster_ptr, TRAIT_CUT, 5000, TRUE);
		effected = TRUE;
		break;

	case TRAIT_SELF_DEATH:
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, 5000, COD_POTION_OF_DEATH, NULL, -1);
		effected = TRUE;
		break;


		/* Melee Type Spell */

	case TRAIT_PANIC_HIT: hit_and_away(caster_ptr); break;
	case TRAIT_RUSH_ATTACK: rush_attack(caster_ptr, NULL); break;

	case TRAIT_MASSACRE:
	case TRAIT_WHIRLWIND:
		massacre(caster_ptr); break;

	case TRAIT_SWORD_DANCING:
		{
			COODINATES y = 0, x = 0;
			int i;
			for (i = 0; i < STAT_MAX; i++)
			{
				dir = (DIRECTION)randint0(8);
				y = caster_ptr->fy + ddy_ddd[dir];
				x = caster_ptr->fx + ddx_ddd[dir];
				cave_ptr = &floor_ptr->cave[y][x];

				// Hack -- attack creatures 
				if(cave_ptr->creature_idx) close_combat(caster_ptr, y, x, 0);
				else msg_format(MES_TRAIT_SWORD_DANCING_MISS(caster_ptr));
			}
			break;
		}

	case TRAIT_STOP_HEX:
		if(stop_hex_spell(caster_ptr)) cost_tactical_energy(caster_ptr, 10);
		break;

	case TRAIT_EAT_MAGIC:
		if(!eat_magic(caster_ptr, user_level * 2)) return FALSE;
		break;

	case TRAIT_EVOCATION:
		project_all_vision(caster_ptr, DO_EFFECT_DISP_ALL, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_AWAY_ALL, user_level * 4);
		break;

	case TRAIT_KATON:
		SELF_FIELD(caster_ptr, DO_EFFECT_FIRE, 50 + user_level, user_level / 10 + 2, -1);
		teleport_creature(caster_ptr, 30, 0L);
		set_timed_trait(caster_ptr, TRAIT_MAGIC_RES_FIRE, user_level, FALSE);
		break;

	case TRAIT_HOLY_LANCE:
		cast_beam(caster_ptr, DO_EFFECT_HOLY_FIRE, MAX_RANGE_SUB, user_level * 3, 0);
		break;

	case TRAIT_HELL_LANCE:
		cast_beam(caster_ptr, DO_EFFECT_HELL_FIRE, MAX_RANGE_SUB, user_level * 3, 0);
		break;

	case TRAIT_HELLFIRE:
		{
			POWER dam = 666;
			COODINATES rad = 3;
			cast_ball(caster_ptr, DO_EFFECT_HELL_FIRE, MAX_RANGE_SUB, dam, rad);
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, 20 + randint1(30), COD_HELLFIRE_RISK, NULL, -1);
		}
		break;

	case TRAIT_HP_TO_SP_ACTIVE:
		{
			int gain_sp = take_damage_to_creature(NULL, caster_ptr, DAMAGE_USELIFE, user_level, COD_HP_TO_MP, NULL, -1) / 5;
			if(gain_sp) inc_mana(caster_ptr, gain_sp);
			else msg_print(MES_CONVERT_FAILED);
			break;
		}

	case TRAIT_SP_TO_HP_ACTIVE:
		if(caster_ptr->csp >= user_level / 5)
		{
			dec_mana(caster_ptr, user_level / 5); 
			heal_creature(caster_ptr, user_level);
		}
		else msg_print(MES_CONVERT_FAILED);
		break;

	case TRAIT_CONFUSING_LIGHT:
		project_all_vision(caster_ptr, DO_EFFECT_SLOW_OTHERS, user_level);
		project_all_vision(caster_ptr, DO_EFFECT_STUN, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_CONF_OTHERS, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_TURN_ALL, user_level * 4);
		project_all_vision(caster_ptr, DO_EFFECT_STASIS, user_level * 4);
		break;

	case TRAIT_BINDING_FIELD:
		if(!binding_field(caster_ptr, MAX_RANGE, user_level * 11 + 5)) msg_print(MES_TRAIT_MIRROR_BINDING_FAILED);
		break;

	case TRAIT_DOUBLE_ATTACK:
		{
			if(!(empty_hands(caster_ptr, TRUE) & EMPTY_HAND_RARM))
			{
				msg_print(MES_PREVENT_BY_NO_BAREBAND);
				return FALSE;
			}
			if(caster_ptr->riding)
			{
				msg_print(MES_PREVENT_BY_RIDING);
				return FALSE;
			}

			if(id == -3)
			{
				if(!choose_combat_option(caster_ptr)) return FALSE;
				prepare_update(caster_ptr, CRU_BONUS);
			}
			else if(id == -4)
			{
				if(floor_ptr->cave[y][x].creature_idx)
				{
					if(one_in_(3)) msg_print(MES_TRAIT_RUSH1);
					else if(one_in_(2)) msg_print(MES_TRAIT_RUSH2);
					else msg_print(MES_TRAIT_RUSH3);
					close_combat(caster_ptr, y, x, 0);
					if(floor_ptr->cave[y][x].creature_idx)
					{
						handle_stuff(caster_ptr);
						close_combat(caster_ptr, y, x, 0);
					}
					cost_tactical_energy(caster_ptr, 100);
				}
				else
				{
					msg_print(MES_NO_DICRECTION_CREATURE);
					msg_print(NULL);
				}
			}
			break;
		}

	case TRAIT_CLEAR_MIND:
		if(caster_ptr->total_friends)
		{
			msg_print(MES_PREVENT_BY_PET);
			return FALSE;
		}
		msg_print(MES_RESTORE_SOME_MP);
		inc_mana(caster_ptr, 3 + user_level / 20);
		break;

	case TRAIT_TAKE_PHOTO:
		cast_beam(caster_ptr, DO_EFFECT_PHOTO, 1, 1, 0);
		break;

	case TRAIT_DOUBLE_REVENGE:
		if(!do_cmd_mane(caster_ptr, TRUE)) return FALSE;
		break;

	case TRAIT_ABSORB_MAGIC:
		if(!gain_magic(caster_ptr)) return FALSE;
		break;

	case TRAIT_STOP_SINGING:
		// Singing is already stopped
		if(!caster_ptr->now_singing && !caster_ptr->pre_singing) return FALSE;
		stop_singing(caster_ptr);
		cost_tactical_energy(caster_ptr, 10);
		break;

	case TRAIT_DOUBLE_MAGIC:
		if(!can_do_cmd_cast(caster_ptr)) return FALSE;
		handle_stuff(caster_ptr);
		do_cmd_cast(caster_ptr);
		handle_stuff(caster_ptr);
		if(!has_trait(caster_ptr, TRAIT_PARALYZED) && can_do_cmd_cast(caster_ptr))
			do_cmd_cast(caster_ptr);
		break;

	case TRAIT_POSTURE:
		if(!choose_combat_option(caster_ptr)) return FALSE;
		prepare_update(caster_ptr, CRU_BONUS);
		break;

	case TRAIT_POSTURE2:
		if(caster_ptr->total_friends)
		{
			msg_print(MES_PREVENT_BY_PET);
			break;
		}
		if(caster_ptr->posture & KATA_IAI | KATA_FUUJIN | KATA_KOUKIJIN | KATA_MUSOU)
		{
			msg_print(MES_PREVENT_BY_POSTURE);
			break;
		}
		msg_print(MES_TRAIT_CONCENTRATION);
		inc_mana(caster_ptr, caster_ptr->msp / 2);
		break;

	case TRAIT_LEARNING:
		if(caster_ptr->action == ACTION_LEARN) set_action(caster_ptr, ACTION_NONE);
		else set_action(caster_ptr, ACTION_LEARN);
		cancel_tactical_action(caster_ptr);
		break;

	case TRAIT_RODEO: rodeo(caster_ptr); break;
	case TRAIT_BREAK_MIRROR: remove_all_mirrors(caster_ptr, GET_FLOOR_PTR(caster_ptr), TRUE); break;

	case TRAIT_MIRROR_CONC:
		{
			if(caster_ptr->total_friends)
			{
				msg_print(MES_PREVENT_BY_PET);
				return FALSE;
			}
			if(is_mirror_grid(&floor_ptr->cave[caster_ptr->fy][caster_ptr->fx]))
			{
				msg_print(MES_RESTORE_SOME_MP);
				inc_mana(caster_ptr, user_level * user_level / 100);
			}
			else msg_print(MES_PREVENT_BY_NO_MIRROR);
			break;
		}

	case TRAIT_QUICK_WALK:
		{
			if(caster_ptr->action == ACTION_HAYAGAKE) set_action(caster_ptr, ACTION_NONE);
			else
			{
				if(!have_flag(feature_ptr->flags, FF_PROJECT) || (!has_trait(caster_ptr, TRAIT_CAN_FLY) && have_flag(feature_ptr->flags, FF_DEEP)))
					msg_print(MES_HAYAGAKE_PREVENT);
				else set_action(caster_ptr, ACTION_HAYAGAKE);
			}
			cancel_tactical_action(caster_ptr);
			break;
		}

	case TRAIT_VAMPIRISM:
		{
			if(is_melee_limitation_field(floor_ptr)) return FALSE;
			else
			{
				int dummy = 0;
				cave_ptr = &floor_ptr->cave[y][x];
				if(!cave_ptr->creature_idx)
				{
					msg_print(MES_TRAIT_VAMPIRISM_NO_TARGET);
					break;
				}
				msg_print(MES_TRAIT_VAMPIRISM_DONE);

				dummy = user_level + randint1(user_level) * MAX(1, user_level / 10);   // Dmg
				if(cast_bolt(caster_ptr, DO_EFFECT_OLD_DRAIN, MAX_RANGE_SUB, dummy, -1))
				{
					if(caster_ptr->food < CREATURE_FOOD_FULL) (void)heal_creature(caster_ptr, dummy); /* No heal if we are "full" */
					else msg_print(MES_TRAIT_VAMPIRISM_NO_HUNGER);

					// Gain nutritional sustenance: 150/hp drained
					// A Food ration gives 5000 food points (by contrast)
					// Don't ever get more than "Full" this way
					// But if we ARE Gorged, it won't cure us
					dummy = caster_ptr->food + MIN(5000, 100 * dummy);
					if(caster_ptr->food < CREATURE_FOOD_MAX) // Not gorged already
						(void)set_food(caster_ptr, dummy >= CREATURE_FOOD_MAX ? CREATURE_FOOD_MAX - 1 : dummy);
				}
				else msg_print(MES_TRAIT_VAMPIRISM_FAILED);
			}
			break;
		}

	case TRAIT_SHADOW_SHIFT: alter_reality(caster_ptr); break;

	case TRAIT_BATTLE_FRENZY:
		{
			int sp_base = power / 2;
			int sp_sides = 20 + power / 2;

			set_timed_trait(caster_ptr, TRAIT_S_HERO, randint1(25) + 25, FALSE);
			heal_creature(caster_ptr, 30);
			set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
			set_timed_trait(caster_ptr, TRAIT_FAST, randint1(sp_sides) + sp_base, TRUE);
		}

	case TRAIT_THROW_BOULDER:
		cast_bolt(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, (3 * user_level) / 2, 0);
		break;

	case TRAIT_SPREADING_THROW:
		spreading_throw(caster_ptr);
		break;

	case TRAIT_CHAIN_HOOK:
		chain_hook(caster_ptr);
		break;

	case TRAIT_SPIT_ACID:
		if(user_level < 25) cast_bolt(caster_ptr, DO_EFFECT_ACID, MAX_RANGE_SUB, user_level, 0);
		else SELF_FIELD(caster_ptr, DO_EFFECT_SOUND, user_level * 2, 2, -1);
		break;

	case TRAIT_HOLDING_DUST:
		if(user_level < 25) project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, user_level, DO_EFFECT_OLD_SLEEP, PROJECT_KILL | PROJECT_HIDE, -1);
		else (void)project_all_vision(caster_ptr, DO_EFFECT_OLD_SLEEP, user_level);
		break;

	case TRAIT_TURN_UNDEAD:
		project_all_vision(caster_ptr, DO_EFFECT_TURN_UNDEAD, user_level);
		break;

	case TRAIT_EXPAND_HLIZN:
		(void)set_timed_trait(caster_ptr, TRAIT_TSUBURERU, randint1(20) + 30, FALSE);
		break;


	/* Beam Type Attack Spell */

	case TRAIT_BE_GRAV: cast_beam(caster_ptr, DO_EFFECT_GRAVITY, MAX_RANGE_SUB, diceroll(9 + (power - 5) / 4, 8), 0); break;
	case TRAIT_SEEKER_RAY: cast_beam(caster_ptr, DO_EFFECT_SEEKER, MAX_RANGE_SUB, diceroll(11+(user_level - 5) / 4,8), 0); break;
	case TRAIT_BEAM_CANNON: cast_beam(caster_ptr, DO_EFFECT_MISSILE, MAX_RANGE_SUB, user_level * 2, 0); break;
	case TRAIT_BEAM_MANA: cast_beam(caster_ptr, DO_EFFECT_MANA, MAX_RANGE_SUB, diceroll(10 + (power / 20), 15), 0); break;
	case TRAIT_HYPN_GAZE: cast_ball(caster_ptr, DO_EFFECT_CHARM, MAX_RANGE_SUB, user_level, 0); break;
	case TRAIT_LIGHTNING_BEAM: cast_beam(caster_ptr, DO_EFFECT_ELEC, power / 6 + 2, diceroll(3 + (power - 1) / 5, 4), 0); break;
	case TRAIT_LASER_EYE: cast_beam(caster_ptr, DO_EFFECT_LITE, MAX_RANGE_SUB, 2 * user_level, 0); break;
	case TRAIT_SUPERRAY: cast_beam(caster_ptr, DO_EFFECT_SUPER_RAY, MAX_RANGE_SUB, 150 + randint1(2 * user_level), 0); break;
	case TRAIT_LITE_LINE:
	case TRAIT_SUNLIGHT: cast_beam(caster_ptr, DO_EFFECT_LITE_WEAK, MAX_RANGE_SUB, diceroll(6, 8), id); break;

	case TRAIT_PSY_SPEAR:
		damage = has_trait(caster_ptr, TRAIT_POWERFUL) ? (randint1(user_level * 2) + 150) : (randint1(user_level * 3 / 2) + 100);
		(void)cast_beam(caster_ptr, DO_EFFECT_PSY_SPEAR, MAX_RANGE_SUB, damage, 0);
		break;

		/* Scout Spell */

	case TRAIT_SELF_KNOWLEDGE: creature_knowledge(caster_ptr); break;
	case TRAIT_SCAN_CREATURE: probing(floor_ptr); break;

		/* Status Spell */

	case TRAIT_RES_STR: if(do_res_stat(caster_ptr, STAT_STR)) effected = TRUE; break;
	case TRAIT_RES_INT: if(do_res_stat(caster_ptr, STAT_INT)) effected = TRUE; break;
	case TRAIT_RES_WIS: if(do_res_stat(caster_ptr, STAT_WIS)) effected = TRUE; break;
	case TRAIT_RES_DEX: if(do_res_stat(caster_ptr, STAT_DEX)) effected = TRUE; break;
	case TRAIT_RES_CON: if(do_res_stat(caster_ptr, STAT_CON)) effected = TRUE; break;
	case TRAIT_RES_CHR: if(do_res_stat(caster_ptr, STAT_CHA)) effected = TRUE; break;

	case TRAIT_INC_STR: if(do_inc_stat(caster_ptr, STAT_STR)) effected = TRUE; break;
	case TRAIT_INC_INT: if(do_inc_stat(caster_ptr, STAT_INT)) effected = TRUE; break;
	case TRAIT_INC_WIS: if(do_inc_stat(caster_ptr, STAT_WIS)) effected = TRUE; break;
	case TRAIT_INC_DEX: if(do_inc_stat(caster_ptr, STAT_DEX)) effected = TRUE; break;
	case TRAIT_INC_CON: if(do_inc_stat(caster_ptr, STAT_CON)) effected = TRUE; break;
	case TRAIT_INC_CHR: if(do_inc_stat(caster_ptr, STAT_CHA)) effected = TRUE; break;

	case TRAIT_DEC_STR: if(do_dec_stat(caster_ptr, STAT_STR)) effected = TRUE; break;
	case TRAIT_DEC_INT: if(do_dec_stat(caster_ptr, STAT_INT)) effected = TRUE; break;
	case TRAIT_DEC_WIS: if(do_dec_stat(caster_ptr, STAT_WIS)) effected = TRUE; break;
	case TRAIT_DEC_DEX: if(do_dec_stat(caster_ptr, STAT_DEX)) effected = TRUE; break;
	case TRAIT_DEC_CON: if(do_dec_stat(caster_ptr, STAT_CON)) effected = TRUE; break;
	case TRAIT_DEC_CHR: if(do_dec_stat(caster_ptr, STAT_CHA)) effected = TRUE; break;

		/* Standard Healing Spell */

	case TRAIT_REGAL_HEAL_OF_AMBER:
	case TRAIT_HEAL:
	case TRAIT_SELF_HEALING_2D8:
	case TRAIT_SELF_HEALING_4D8:
	case TRAIT_SELF_HEALING_6D8:
	case TRAIT_SELF_HEALING_10D10:
	case TRAIT_SELF_HEALING_50D20:
	case TRAIT_SELF_HEALING_80D20:
	case TRAIT_SELF_HEALING_30D100:
	case TRAIT_SELF_HEALING_100D100:
		if(heal_creature(caster_ptr, power)) effected = TRUE;
		break;

	case TRAIT_HEAVENLY_CHOIR:
		(void)set_timed_trait(caster_ptr, TRAIT_HERO, randint1(25) + 25, TRUE);
		(void)heal_creature(caster_ptr, 777);
		break;

		/* Restore Soul Spell */

	case TRAIT_RESTORE_LIFE:
	case TRAIT_RESTORE_LEVEL:
		if(restore_exp(caster_ptr)) effected = TRUE;
		break;

	case TRAIT_GAIN_EXP:
		if(caster_ptr->exp < CREATURE_MAX_EXP)
		{
			s32b ee = (caster_ptr->exp / 2) + 10;
			if(ee > 100000L) ee = 100000L;
			gain_exp(caster_ptr, ee, 0, TRUE);
			effected = TRUE;
		}
		break;

		/* Cure Spell */

	case TRAIT_RESTORE_ALL:
	case TRAIT_CURE_OF_AESCULAPIUS:
		if(do_res_stat(caster_ptr, STAT_STR)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_INT)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_WIS)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_DEX)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_CON)) effected = TRUE;
		if(do_res_stat(caster_ptr, STAT_CHA)) effected = TRUE;
		break;

	case TRAIT_CURING:
		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_CONFUSED, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_BLIND, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 0, TRUE);
		break;

	case TRAIT_PATTERN_WALK:
		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_HALLUCINATION, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_STUN, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_CUT, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_BLIND, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
		(void)do_res_stat(caster_ptr, STAT_STR);
		(void)do_res_stat(caster_ptr, STAT_INT);
		(void)do_res_stat(caster_ptr, STAT_WIS);
		(void)do_res_stat(caster_ptr, STAT_DEX);
		(void)do_res_stat(caster_ptr, STAT_CON);
		(void)do_res_stat(caster_ptr, STAT_CHA);
		(void)restore_exp(caster_ptr);
		break;

		/* Generate Object Type */

	case TRAIT_ACQUIREMENT1: acquirement(floor_ptr, caster_ptr->fy, caster_ptr->fx, 1, TRUE, FALSE); break;
	case TRAIT_ACQUIREMENT2: acquirement(floor_ptr, caster_ptr->fy, caster_ptr->fx, randint1(2) + 1, TRUE, FALSE); break;

	case TRAIT_CREATE_FOOD:
		{
			object_type *quest_ptr;
			object_type forge;
			quest_ptr = &forge; // Get local object
			generate_object(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION)); // Create the food ration 
			(void)drop_near(floor_ptr, quest_ptr, -1, caster_ptr->fy, caster_ptr->fx); // Drop the object from heaven 
		}
		break;

	case TRAIT_CREATE_AMMO: if(!do_cmd_archer(caster_ptr)) return FALSE; break;

	/* Enchant Object Type */

	case TRAIT_RUSTPROOF: rustproof(caster_ptr); break;
	case TRAIT_PURISH_SHIELD: pulish_shield(caster_ptr); break;
	case TRAIT_BLESS_WEAPON: if(!bless_weapon(caster_ptr)) return FALSE; break;
	case TRAIT_ENCHANT_REMOVE: mundane_spell(caster_ptr, FALSE); break;
	case TRAIT_ENCHANT_WEAPON_BOOST: enchant_spell(caster_ptr, randint0(4) + 1, randint0(4) + 1, 0, 0, 0); break;
	case TRAIT_ENCHANT_ARMOR_BOOST: enchant_spell(caster_ptr, 0, 0, randint0(3) + 2, 0, 0); break;
	case TRAIT_ENCHANT_ELEMENTAL_BRAND: brand_weapon(caster_ptr, randint0(2)); break;
	case TRAIT_ENCHANT_FIRE_BRAND: brand_weapon(caster_ptr, 1); break;
	case TRAIT_ENCHANT_CHAOS_BRAND: brand_weapon(caster_ptr, 2); break;
	case TRAIT_ENCHANT_POISON_BRAND: brand_weapon(caster_ptr, 3); break;
	case TRAIT_ENCHANT_VAMPIRIC_BRAND: brand_weapon(caster_ptr, 4); break;
	case TRAIT_ENCHANT_TRUMP_BRAND: brand_weapon(caster_ptr, 5); break;
	case TRAIT_ENCHANT_RANDOM_BRAND: brand_weapon(caster_ptr, randint0(18)); break;
	case TRAIT_ENCHANT_HOLY_BRAND: brand_weapon(caster_ptr, 13); break;
	case TRAIT_ADD_FIRE_BRAND: (void)brand_bolts(caster_ptr); break;

	/* Wonder Type Spell */

	case TRAIT_WANDER: cast_wonder(caster_ptr); break;
	case TRAIT_INVOKE_SPIRITS: cast_invoke_spirits(caster_ptr); break;
	case TRAIT_SHUFFLE: cast_shuffle(caster_ptr); break;
	case TRAIT_BIZARRE_THING_OF_THE_RING: ring_of_power(caster_ptr); break;
	case TRAIT_CALL_CHAOS: call_chaos(caster_ptr); break;

	/* Storm Attack Spell */

	case TRAIT_STORM_FIRE: SELF_FIELD(caster_ptr, DO_EFFECT_FIRE, 300 + 3 * power, 8, -1);
	case TRAIT_STORM_NETHER: cast_ball(caster_ptr, DO_EFFECT_NETHER, MAX_RANGE_SUB, power, power / 5); break;
	case TRAIT_STORM_CHAOS: cast_ball(caster_ptr, DO_EFFECT_CHAOS, MAX_RANGE_SUB, power, power / 5); break;
	case TRAIT_LIGHTNING_STORM: cast_ball(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_COLD_STORM: cast_ball(caster_ptr, DO_EFFECT_COLD, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_WATER_STORM: cast_ball(caster_ptr, DO_EFFECT_WATER, MAX_RANGE_SUB, power, rad); break;
	case TRAIT_SONIC_BOOM:
		msg_print(MES_TRAIT_SONIC_BOOM);
		project(caster_ptr, 0, rad, caster_ptr->fy, caster_ptr->fx, power, DO_EFFECT_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
		break;

	case TRAIT_STORM_MANA:
		msg_print(MES_TRAIT_MANA_FIELD_DONE);
		project(caster_ptr, 0, 5, caster_ptr->fy, caster_ptr->fx, (randint1(200) + 300) * 2, DO_EFFECT_MANA, PROJECT_KILL | PROJECT_ITEM | PROJECT_GRID, -1);
		if(!saving_throw(caster_ptr, SAVING_VO, 120, 0)) (void)take_damage_to_creature(NULL, caster_ptr, DAMAGE_NOESCAPE, 50, COD_UNCONTROLED_MANA_FIELD, NULL, -1);
		break;

	case TRAIT_HIDE_IN_MYST:
		SELF_FIELD(caster_ptr, DO_EFFECT_POIS, 75 + user_level * 2 / 3, user_level / 5 + 2, -1);
		SELF_FIELD(caster_ptr, DO_EFFECT_OLD_DRAIN, 75 + user_level * 2 / 3, user_level / 5 + 2, -1);
		SELF_FIELD(caster_ptr, DO_EFFECT_CONFUSION, 75 + user_level * 2 / 3, user_level / 5 + 2, -1);
		teleport_creature(caster_ptr, 30, 0L);
		break;

	case TRAIT_INSANITY_CIRCLE:
		SELF_FIELD(caster_ptr, DO_EFFECT_CHAOS, power, rad, -1);
		SELF_FIELD(caster_ptr, DO_EFFECT_CONFUSION, power, rad, -1);
		SELF_FIELD(caster_ptr, DO_EFFECT_CHARM, power, rad, -1);
		break;

	case TRAIT_SHRIEK:
		SELF_FIELD(caster_ptr, DO_EFFECT_SOUND, 2 * user_level, 8, -1);
		aggravate_creatures(caster_ptr);
		break;

	case TRAIT_RADIATION: SELF_FIELD(caster_ptr, DO_EFFECT_NUKE, ((COODINATES)user_level * 2), 3 + ((COODINATES)user_level / 20), -1); break;

		/* Chain Type Spells */

	case TRAIT_CHAIN_LIGHTNING:
		{
			int dice = 5 + power / 10;
			int sides = 8;
			for(dir = 0; dir <= 9; dir++) cast_beam(caster_ptr, DO_EFFECT_ELEC, MAX_RANGE_SUB, diceroll(dice, sides), 0);
		}
		break;

	case TRAIT_SPLASH_LITE:
		{
			int num = diceroll(5, 3);
			COODINATES y, x;
			int attempts;

			for (k = 0; k < num; k++)
			{
				attempts = 1000;
				y = caster_ptr->fy;
				x = caster_ptr->fx;
				while (attempts--)
				{
					scatter(floor_ptr, &y, &x, caster_ptr->fy, caster_ptr->fx, 4, 0);
					if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) continue;
					if(!CREATURE_BOLD(caster_ptr, y, x)) break;
				}
				project(caster_ptr, 0, 0, y, x, diceroll(6 + user_level / 8, 10), DO_EFFECT_LITE_WEAK, (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_KILL), -1);
			}
			break;
		}

	case TRAIT_STAR_BALL2:
		{
			int num = diceroll(5, 3);
			int attempts;
			for (k = 0; k < num; k++)
			{
				attempts = 1000;
				while (attempts--)
				{
					scatter(floor_ptr, &y, &x, caster_ptr->fy, caster_ptr->fx, 4, 0);
					if(!CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_PROJECT)) continue;
					if(!CREATURE_BOLD(caster_ptr, y, x)) break;
				}
				project(caster_ptr, 0, 3, y, x, 150, DO_EFFECT_ELEC, (PROJECT_THRU | PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
			}
			break;
		}

	/* Teleport Spells */

	case TRAIT_BLINK:
		if(teleport_barrier(target_ptr, caster_ptr)) msg_format(MES_TRAIT_TELEPORT_BLOCK(caster_ptr));
		else
		{
			msg_format(MES_TRAIT_BLINK_DONE(caster_ptr));
			teleport_away(caster_ptr, 10, 0L);
			prepare_update(caster_ptr, PU_CREATURES);
		}
		break;

	case TRAIT_ACTIVE_TELEPORT:
		if(teleport_barrier(target_ptr, caster_ptr)) msg_format(MES_TRAIT_TELEPORT_BLOCK(caster_ptr));
		else
		{
			msg_format(MES_TRAIT_TELEPORT_DONE(caster_ptr));
			teleport_away_followable(caster_ptr);
		}
		break;

	case TRAIT_VTELEPORT: teleport_creature(caster_ptr, 10 + 4 * (COODINATES)user_level, 0L); break;
	case TRAIT_TELE_AWAY: (void)cast_beam(caster_ptr, DO_EFFECT_AWAY_ALL, MAX_RANGE_SUB, user_level, 0); break;
	case TRAIT_MASS_TELE_AWAY: (void)cast_beam(caster_ptr, DO_EFFECT_AWAY_ALL, MAX_RANGE_SUB, user_level, 0); break;
	case TRAIT_TELE_LEVEL: teleport_level(caster_ptr, 0); break;
	case TRAIT_SWAP_POS: (void)teleport_swap(caster_ptr, dir); break;
	case TRAIT_DIMENSION_DOOR: dimension_door(caster_ptr); break;
	case TRAIT_GETAWAY: getaway(caster_ptr); break;
	case TRAIT_RECALL: word_of_recall(caster_ptr, randint0(21) + 15); break;
	case TRAIT_FETCH_CREATURE: fetch_servants(caster_ptr); break;

	case TRAIT_TELE_TO:
		{
			creature_type *target_ptr;
			species_type *species_ptr;

			if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
			if(!floor_ptr->cave[target_row][target_col].creature_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			target_ptr = &creature_list[floor_ptr->cave[target_row][target_col].creature_idx];
			species_ptr = &species_info[target_ptr->species_idx];
			creature_desc(target_name, target_ptr, 0);
			if(has_trait(target_ptr, TRAIT_RES_TELE))
			{
				if((has_trait(target_ptr, TRAIT_UNIQUE)) || has_trait(target_ptr, TRAIT_RES_ALL))
				{
					if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					msg_format(MES_IS_UNAFFECTED, target_name);
					break;
				}
				else if(target_ptr->lev > randint1(100))
				{
					if(is_original_ap_and_seen(player_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_RES_TELE);
					msg_format(MES_RESISTED, target_name);
					break;
				}
			}
			msg_format(MES_TRAIT_TELEPORT_BACK_DONE(target_ptr));
			teleport_creature_to2(floor_ptr->cave[target_row][target_col].creature_idx, caster_ptr->fy, caster_ptr->fx, 100, TELEPORT_PASSIVE);
			break;
		}

	/* etc Spells */

	case TRAIT_RESET_RECALL: reset_recall(caster_ptr); break;
	case TRAIT_RUMOR: get_rumor(caster_ptr); break;
	case TRAIT_SATIATE: (void)set_food(caster_ptr, CREATURE_FOOD_MAX - 1); break;
	case TRAIT_PHLOGISTON: phlogiston(caster_ptr); break;
	case TRAIT_SHOOT: if(!do_cmd_throw_aux(caster_ptr, 2 + user_level / 40, FALSE, 0)) return FALSE; break;
	case TRAIT_POLYMORPH_OTHER: cast_bolt(caster_ptr, DO_EFFECT_OLD_POLY, MAX_RANGE_SUB, power, -1); break;
	case TRAIT_MIDAS_TCH: (void)alchemy(caster_ptr); break;
	case TRAIT_TRAP_WALK: walk_creature(caster_ptr, dir, easy_disarm, TRUE); break;

	case TRAIT_IMPROVE_FORCE:
		msg_print(MES_TRAIT_FORCE_IMPROVE);
		caster_ptr->charged_force += (70 + user_level);
		prepare_update(caster_ptr, CRU_BONUS);
		if(randint1(caster_ptr->charged_force) > (user_level * 4 + 120))
		{
			msg_print(MES_TRAIT_FORCE_EXPRODE);
			cast_ball(caster_ptr, DO_EFFECT_MANA, 0, caster_ptr->charged_force / 2, 10);
			take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, caster_ptr->charged_force / 2, COD_UNC_FORCE, NULL, -1);
		}
		break;

	case TRAIT_DISCHARGE_MINION: discharge_minion(caster_ptr); break;
	case TRAIT_STERILITY:
		take_damage_to_creature(NULL, caster_ptr, DAMAGE_LOSELIFE, randint1(17) + 17, COD_ABSTINENCE, NULL, -1);
		floor_ptr->num_of_reproduction += MAX_REPRO;
		break;

	case TRAIT_MIRROR_SEAL: seal_of_mirror(caster_ptr, user_level * 4 + 100); break;
	case TRAIT_MIRROR_SET:
		if(number_of_mirrors(caster_ptr) < 4 + user_level / 10) place_mirror(caster_ptr);
		else msg_format(MES_TRAIT_MIRROR_SET_LIMIT);
		break;

	case TRAIT_ARREST_CREATURE: cast_ball_hide(caster_ptr, DO_EFFECT_STASIS, MAX_RANGE_SUB, user_level*2, 0); break;
	case TRAIT_ARREST_EVIL: cast_ball_hide(caster_ptr, DO_EFFECT_STASIS_EVIL, MAX_RANGE_SUB, power * 2, 0); break;

	case TRAIT_HEAL_OTHER: cast_bolt(caster_ptr, DO_EFFECT_OLD_HEAL, MAX_RANGE_SUB, power, -1); break;

	case TRAIT_WOODEN_CRAPPING: aggravate_creatures(caster_ptr); break;

	case TRAIT_CAPTURE_CREATURE: monster_ball(caster_ptr); break;
	case TRAIT_FISHING: fishing(caster_ptr, y, x); break;
	case TRAIT_TELEKINES: fetch(caster_ptr, MAX_RANGE, dir, user_level * 10, TRUE); break;
	case TRAIT_SHOCK_WAVE: shock_wave(caster_ptr); break;
	case TRAIT_EAT_ROCK: eat_rock(caster_ptr, y, x); break;
	case TRAIT_DET_CURSE: check_cursed_inventory(caster_ptr); break;
	case TRAIT_STAR_DUST: fire_blast(caster_ptr, DO_EFFECT_LITE, dir, power, 2, 10, 3); break;
	case TRAIT_SCATTER_EVIL: cast_ball(caster_ptr, DO_EFFECT_AWAY_EVIL, MAX_RANGE_SUB, power, 0); break;

	case TRAIT_SPECIAL:
		{
			switch (caster_ptr->species_idx)
			{

			case SPECIES_BANORLUPART:
				{
					int dummy_hp = (caster_ptr->chp + 1) / 2;
					int dummy_mhp = caster_ptr->mhp/2;
					COODINATES dummy_y = caster_ptr->fy;
					COODINATES dummy_x = caster_ptr->fx;

					if(floor_ptr->fight_arena_mode || floor_ptr->gamble_arena_mode || !summon_possible(caster_ptr, caster_ptr->fy, caster_ptr->fx)) return FALSE;
					delete_creature(&creature_list[floor_ptr->cave[caster_ptr->fy][caster_ptr->fx].creature_idx]);
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_BANOR, mode);
					creature_list[hack_creature_idx_ii].chp = dummy_hp;
					creature_list[hack_creature_idx_ii].mhp = dummy_mhp;
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_LUPART, mode);
					creature_list[hack_creature_idx_ii].chp = dummy_hp;
					creature_list[hack_creature_idx_ii].mhp = dummy_mhp;

					msg_format(MES_TRAIT_SPLIT_TWO_MEN(caster_ptr));
					break;
				}

			case SPECIES_BANOR:
			case SPECIES_LUPART:
				{
					int dummy_hp = 0;
					int dummy_mhp = 0;
					COODINATES dummy_y = caster_ptr->fy;
					COODINATES dummy_x = caster_ptr->fx;

					if(!species_info[SPECIES_BANOR].cur_num || !species_info[SPECIES_LUPART].cur_num) return FALSE;
					for (k = 1; k < creature_max; k++)
					{
						if(creature_list[k].species_idx == SPECIES_BANOR || creature_list[k].species_idx == SPECIES_LUPART)
						{
							dummy_hp += creature_list[k].chp;
							dummy_mhp += creature_list[k].mhp;
							if(creature_list[k].species_idx != caster_ptr->species_idx)
							{
								dummy_y = creature_list[k].fy;
								dummy_x = creature_list[k].fx;
							}
							delete_creature(&creature_list[k]);
						}
					}
					summon_named_creature(0, floor_ptr, dummy_y, dummy_x, SPECIES_BANORLUPART, mode);
					creature_list[hack_creature_idx_ii].chp = dummy_hp;
					creature_list[hack_creature_idx_ii].mhp = dummy_mhp;
					msg_print(MES_TRAIT_COMBINE);
					break;
				}

			}
			break;

	case TRAIT_LIVING_TRUMP: try_livingtrump(caster_ptr); break;

	case TRAIT_WEIGH_MAG:
		//TODO Erase
		break;

	case TRAIT_SLEEP_TOUCH:
	case TRAIT_SLEEP:
	case TRAIT_SANCTUARY:
		project(caster_ptr, 0, 1, caster_ptr->fy, caster_ptr->fx, user_level, DO_EFFECT_OLD_SLEEP, PROJECT_KILL | PROJECT_HIDE, -1);
		break;

	case TRAIT_CHANGE_BRAND:
		//get_bloody_moon_flags(object_ptr);
		if(has_trait(caster_ptr, TRAIT_ANDROID)) calc_android_exp(caster_ptr);
		prepare_update(caster_ptr, CRU_BONUS | CRU_HP);
		break;

	case TRAIT_SHIKO:
		(void)set_timed_trait(caster_ptr, TRAIT_AFRAID, 0, TRUE);
		(void)set_timed_trait(caster_ptr, TRAIT_HERO, randint1(20) + 20, FALSE);
		project_all_vision(caster_ptr, DO_EFFECT_DISP_EVIL, user_level * 3);
		break;

	case TRAIT_REMOVE_CURSE_1: if(remove_curse(caster_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);
	case TRAIT_REMOVE_CURSE_2: if(remove_all_curse(caster_ptr)) msg_print(MES_REMOVED_OBJECT_CURSE);

	case TRAIT_RESTORE_MANA:
		{
			int i;
			for (i = 0; i < EATER_EXT * 2; i++)
			{
				caster_ptr->current_charge[i] += (caster_ptr->max_charge[i] < 10) ? EATER_CHARGE * 3 : caster_ptr->max_charge[i]*EATER_CHARGE/3;
				if(caster_ptr->current_charge[i] > caster_ptr->max_charge[i]*EATER_CHARGE) caster_ptr->current_charge[i] = caster_ptr->max_charge[i]*EATER_CHARGE;
			}
			for (; i < EATER_EXT * 3; i++)
			{
				OBJECT_KIND_ID k_idx = lookup_kind(TV_ROD, (SVAL)(i - EATER_EXT * 2));
				caster_ptr->current_charge[i] -= ((caster_ptr->max_charge[i] < 10) ? EATER_ROD_CHARGE*3 : caster_ptr->max_charge[i]*EATER_ROD_CHARGE/3)*object_kind_info[k_idx].pval;
				if(caster_ptr->current_charge[i] < 0) caster_ptr->current_charge[i] = 0;
			}
			msg_print(MES_MANA_RECOVERLY);
			prepare_window(PW_PLAYER);

			inc_mana(caster_ptr, caster_ptr->msp);
			msg_print(MES_MANA_RECOVERLY);
			prepare_redraw(PW_PLAYER | PW_SPELL);
			effected = TRUE;

			if(set_timed_trait(caster_ptr, TRAIT_S_HERO, 0, TRUE)) effected = TRUE;
			break;
		}

	case TRAIT_SALT_WATER:
		if(!has_trait(caster_ptr, TRAIT_NONLIVING)) (void)set_food(caster_ptr, CREATURE_FOOD_STARVE - 1); // Only living creatures get thirsty
		(void)set_timed_trait(caster_ptr, TRAIT_POISONED, 0, TRUE);
		(void)add_timed_trait(caster_ptr, TRAIT_PARALYZED, 4, TRUE);
		break;

	case TRAIT_DISPEL:
		{
			CREATURE_ID creature_idx;
			if(!target_set(caster_ptr, 0, TARGET_KILL)) return FALSE;
			creature_idx = floor_ptr->cave[target_row][target_col].creature_idx;
			if(!creature_idx) break;
			if(!player_has_los_bold(target_row, target_col)) break;
			if(!projectable(floor_ptr, MAX_RANGE, caster_ptr->fy, caster_ptr->fx, target_row, target_col)) break;
			dispel_creature(caster_ptr);
			break;
		}

		case TRAIT_WORLD:
		caster_ptr->time_stopper = TRUE;
		msg_print(NULL);

		cost_tactical_energy(caster_ptr, -1000 - (100 + randint1(200) + 200) * TURNS_PER_TICK / 10);
		prepare_redraw(PR_MAP);
		prepare_update(caster_ptr, PU_CREATURES);
		prepare_window(PW_OVERHEAD | PW_DUNGEON);

		handle_stuff(caster_ptr);
		break;

		}

	default: msg_warning("Undefined active trait."); break;

	}

	if(fumble_summoned) msg_format(MES_SUMMON_FUMBLE(summoned_name));

	if(kichigai_talk)
	{
		if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK)) msg_print("やりやがったな！");
		else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK)) msg_print("弱いものいじめはやめるんだ！");
		if(has_trait(target_ptr, TRAIT_ECHIZEN_TALK)) msg_print("くっそ～");
		else if(has_trait(target_ptr, TRAIT_CHARGEMAN_TALK)) msg_print("なんて事をするんだ！");
	}

	if(summoned) if(blind && count) msg_print(MES_SUMMON_FEELING);

	return effected;
}
