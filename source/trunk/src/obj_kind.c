/* File: obj_kind.c */

#include "angband.h"


bool object_is_potion(creature_type *creature_ptr, object_type *object_ptr)
{
	return (object_kind_info[object_ptr->k_idx].tval == TV_POTION);
}


bool object_is_shoukinkubi(creature_type *creature_ptr, object_type *object_ptr)
{
	int i;

	/* Require corpse or skeleton */
	if(object_ptr->tval != TV_CORPSE) return FALSE;

	/* Today's wanted */
	if(today_mon > 0 && (streq(species_name + species_info[object_ptr->pval].name, species_name + species_info[today_mon].name))) return TRUE;

	/* Tsuchinoko */
	if(object_ptr->pval == SPECIES_TSUCHINOKO) return TRUE;

	/* Unique creature */
	for (i = 0; i < MAX_BOUNTY; i++)
		if(object_ptr->pval == kubi_species_idx[i]) break;
	if(i < MAX_BOUNTY) return TRUE;

	/* Not wanted */
	return FALSE;
}


/*
 * Favorite weapons
 */
bool object_is_favorite(creature_type *creature_ptr, object_type *object_ptr)
{
	/* Only melee weapons match */
	if(!(object_ptr->tval == TV_POLEARM ||
	      object_ptr->tval == TV_SWORD ||
	      object_ptr->tval == TV_DIGGING ||
	      object_ptr->tval == TV_HAFTED))
	{
		return FALSE;
	}

	/* Favorite weapons are varied depend on the class */
	switch (creature_ptr->class_idx)
	{
	case CLASS_PRIEST:
	{
		u32b flgs[MAX_TRAITS_FLAG];
		object_flags_known(object_ptr, flgs);

		if(!have_flag(flgs, TRAIT_BLESSED_BRAND) && 
		    !(object_ptr->tval == TV_HAFTED))
			return FALSE;
		break;
	}

	case CLASS_MONK:
	case CLASS_FORCETRAINER:
		/* Icky to wield? */
		/*
		if(!(skill_info[creature_ptr->class_idx].w_max[object_ptr->tval-TV_WEAPON_BEGIN][object_ptr->sval]))
			return FALSE;
		*/
		break;

	case CLASS_BEASTMASTER:
	case CLASS_CAVALRY:
	{
		u32b flgs[MAX_TRAITS_FLAG];
		object_flags_known(object_ptr, flgs);

		/* Is it known to be suitable to using while riding? */
		if(!(have_flag(flgs, TRAIT_RIDING)))
			return FALSE;

		break;
	}

	case CLASS_NINJA:
		/* Icky to wield? */
		/*
		if(skill_info[creature_ptr->class_idx].w_max[object_ptr->tval-TV_WEAPON_BEGIN][object_ptr->sval] <= WEAPON_EXP_BEGINNER)
			return FALSE;
		*/
		break;

	default:
		/* All weapons are okay for non-special classes */
		return TRUE;
	}

	return TRUE;
}


/*
 * Rare weapons/aromors
 * including Blade of Chaos, Dragon armors, etc.
 */
bool object_is_rare(creature_type *creature_ptr, object_type *object_ptr)
{
	// creature_ptr currently null

	switch(object_ptr->tval)
	{
	case TV_HAFTED:
		if(object_ptr->sval == SV_MACE_OF_DISRUPTION ||
		    object_ptr->sval == SV_WIZSTAFF) return TRUE;
		break;

	case TV_POLEARM:
		if(object_ptr->sval == SV_SCYTHE_OF_SLICING ||
		    object_ptr->sval == SV_DEATH_SCYTHE) return TRUE;
		break;

	case TV_SWORD:
		if(object_ptr->sval == SV_BLADE_OF_CHAOS ||
		    object_ptr->sval == SV_DIAMOND_EDGE ||
		    object_ptr->sval == SV_DOKUBARI ||
		    object_ptr->sval == SV_HAYABUSA) return TRUE;
		break;

	case TV_SHIELD:
		if(object_ptr->sval == SV_DRAGON_SHIELD ||
		    object_ptr->sval == SV_MIRROR_SHIELD) return TRUE;
		break;

	case TV_HELM:
		if(object_ptr->sval == SV_DRAGON_HELM) return TRUE;
		break;

	case TV_BOOTS:
		if(object_ptr->sval == SV_PAIR_OF_DRAGON_GREAVE) return TRUE;
		break;

	case TV_CLOAK:
		if(object_ptr->sval == SV_ELVEN_CLOAK ||
		    object_ptr->sval == SV_ETHEREAL_CLOAK ||
		    object_ptr->sval == SV_SHADOW_CLOAK) return TRUE;
		break;

	case TV_GLOVES:
		if(object_ptr->sval == SV_SET_OF_DRAGON_GLOVES) return TRUE;
		break;

	case TV_SOFT_ARMOR:
		if(object_ptr->sval == SV_KUROSHOUZOKU ||
		    object_ptr->sval == SV_ABUNAI_MIZUGI) return TRUE;
		break;

	case TV_DRAG_ARMOR:
		return TRUE;

	default:
		break;
	}

	/* Any others are not "rare" objects. */
	return FALSE;
}


/*
 * Check if an object is weapon (including bows and ammo)
 */
bool object_is_weapon(object_type *object_ptr)
{
	if(object_kind_info[object_ptr->k_idx].slot == INVEN_SLOT_ARMS) return TRUE;
	return FALSE;
}
bool object_is_weapon2(creature_type *creature_ptr, object_type *object_ptr)
{
	return object_is_weapon(object_ptr);
}


/*
 * Check if an object is weapon (including bows and ammo)
 */
bool object_is_weapon_ammo(object_type *object_ptr)
{
	if(TV_MISSILE_BEGIN <= object_ptr->tval && object_ptr->tval <= TV_WEAPON_END) return TRUE;
	return FALSE;
}
bool object_is_weapon_ammo2(creature_type *creature_ptr, object_type *object_ptr)
{
	if(TV_MISSILE_BEGIN <= object_ptr->tval && object_ptr->tval <= TV_WEAPON_END) return TRUE;
	return FALSE;
}

/*
 * Check if an object is ammo
 */
bool object_is_ammo(object_type *object_ptr)
{
	if(TV_MISSILE_BEGIN <= object_ptr->tval && object_ptr->tval <= TV_MISSILE_END) return TRUE;
	return FALSE;
}


/*
 * Check if an object is armour
 */
bool object_is_armour(object_type *object_ptr)
{
	if(TV_ARMOR_BEGIN <= object_ptr->tval && object_ptr->tval <= TV_ARMOR_END) return TRUE;
	return FALSE;
}
bool object_is_armour2(creature_type *creature_ptr, object_type *object_ptr)
{
	return object_is_armour(object_ptr);
}


/*
 * Check if an object is weapon, armour or ammo
 */
bool object_is_weapon_armour_ammo(object_type *object_ptr)
{
	if(object_is_weapon_ammo(object_ptr) || object_is_armour(object_ptr)) return TRUE;

	return FALSE;

}
bool object_is_weapon_armour_ammo2(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_is_weapon_ammo(object_ptr) || object_is_armour(object_ptr)) return TRUE;

	return FALSE;
}


/*
 * Melee weapons
 */
bool object_is_melee_weapon(creature_type *creature_ptr, object_type *object_ptr)
{
	if(TV_DIGGING <= object_ptr->tval && object_ptr->tval <= TV_SWORD) return TRUE;

	return FALSE;
}


/*
 * Wearable including all weapon, all armour, bow, light source, amulet, and ring
 */
bool object_is_wearable(object_type *object_ptr)
{
	if(TV_WEARABLE_BEGIN <= object_ptr->tval && object_ptr->tval <= TV_WEARABLE_END) return TRUE;

	return FALSE;
}


/*
 * Equipment including all wearable objects and ammo
 */
bool object_is_equipment(object_type *object_ptr)
{
	if(TV_EQUIP_BEGIN <= object_ptr->tval && object_ptr->tval <= TV_EQUIP_END) return TRUE;

	return FALSE;
}
bool object_is_equipment2(creature_type *creature_ptr, object_type *object_ptr)
{
	if(TV_EQUIP_BEGIN <= object_ptr->tval && object_ptr->tval <= TV_EQUIP_END) return TRUE;

	return FALSE;
}


/*
 * Poison needle can not be enchanted
 */
bool object_refuse_enchant_weapon(creature_type *creature_ptr, object_type *object_ptr)
{
	if(has_trait_object(object_ptr, TRAIT_CRITICAL_SLAYING)) return TRUE;
	return FALSE;
}


/*
 * Check if an object is weapon (including bows and ammo) and allows enchantment
 */
bool object_allow_enchant_weapon(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_is_weapon_ammo(object_ptr) && !object_refuse_enchant_weapon(creature_ptr, object_ptr)) return TRUE;

	return FALSE;
}


/*
 * Check if an object is melee weapon and allows enchantment
 */
bool object_allow_enchant_melee_weapon(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_is_melee_weapon(creature_ptr, object_ptr) && !object_refuse_enchant_weapon(creature_ptr, object_ptr)) return TRUE;

	return FALSE;
}


/*
 * Check if an object is made by a smith's special ability
 */
bool object_is_smith(object_type *object_ptr)
{
	if(object_is_weapon_armour_ammo(object_ptr) && object_ptr->xtra3) return TRUE;
	return FALSE;
}


bool object_is_smith2(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_is_weapon_armour_ammo(object_ptr) && object_ptr->xtra3) return TRUE;

	return FALSE;
}


/*
 * Check if an object is artifact
 */
bool object_is_artifact(object_type *object_ptr)
{
	if(object_is_fixed_artifact(object_ptr) || object_ptr->art_name) return TRUE;

	return FALSE;
}

bool object_is_artifact_aux(object_type *object_ptr)
{
	if(object_is_fixed_artifact(object_ptr) || object_ptr->art_name) return TRUE;

	return FALSE;
}


/*
 * Check if an object is neither artifact, ego, nor 'smith' object
 */
bool object_is_nameless(creature_type *creature_ptr, object_type *object_ptr)
{
	if(!object_is_artifact(object_ptr) && !object_is_ego(object_ptr) && !object_is_smith(object_ptr))
		return TRUE;



	return FALSE;
}


/*
 * Check if an object is melee weapon and allows wielding with two-hands
 */
bool object_allow_two_hands_wielding(creature_type *creature_ptr, object_type *object_ptr)
{
	if(object_is_melee_weapon(creature_ptr, object_ptr) && ((object_ptr->weight > 99) || (object_ptr->tval == TV_POLEARM))) return TRUE;

	return FALSE;
}


/*
 * Hook to determine if an object is readable
 */
bool item_tester_hook_readable(creature_type *creature_ptr, object_type *object_ptr)
{
	if((object_ptr->tval==TV_SCROLL) || (object_ptr->tval==TV_PARCHMENT) || has_trait_object(object_ptr, TRAIT_CAN_READ)) return TRUE;
	return FALSE; // Assume not
}
