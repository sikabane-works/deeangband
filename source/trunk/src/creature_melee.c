#include "angband.h"

int melee_hit_chance(creature_type *creature_ptr, int to_hit, int ev)
{
	int chance = 0;
	int hit_power = creature_ptr->skill_thn + (creature_ptr->to_hit[0] + to_hit) * BTH_PLUS_ADJ;

	if(hit_power <= 0) return 5;

	chance = 100 - ((ev * 75) / hit_power);

	if(chance > 95) chance = 95;
	if(chance < 5) chance = 5;
	if(creature_ptr->chara_idx == CHARA_NAMAKE)
		chance = (chance * 19 + 9) / 20;
	return chance;
}

/*
 * Determine if the player "hits" a creature (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_melee(creature_type *attacker_ptr, int chance, int ev, int vis)
{
	int k;

	k = randint0(100);	// Percentile dice
	if(k < 10) return (k < 5); // Hack -- Instant miss or hit
	if(has_trait(attacker_ptr, TRAIT_MISS_MELEE) && (one_in_(20))) return FALSE;
	if(chance <= 0) return FALSE;	// Wimpy attack never hits

	if(!vis) chance = (chance + 1) / 2;	// Penalize invisible targets
	if(randint0(chance) < (ev * 3 / 4)) return FALSE;	// Power must defeat armor
	return TRUE;	// Assume hit
}


// Critical hits (by player)
// Factor in weapon weight, total plusses, player level.
POWER test_critial_melee(creature_type *creature_ptr, int weight, int plus, POWER dam, STAT meichuu, int mode)
{
	int i, k;

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (creature_ptr->lev * 3));

	/* Chance */
	if((randint1((creature_ptr->class_idx == CLASS_NINJA) ? 4444 : 5000) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		k = weight + randint1(650);
		if((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);

		if(k < 400)
		{
			msg_print(MES_CRITICAL_LEVEL1);
			dam = 2 * dam + 5;
		}
		else if(k < 700)
		{
			msg_print(MES_CRITICAL_LEVEL2);
			dam = 2 * dam + 10;
		}
		else if(k < 900)
		{
			msg_print(MES_CRITICAL_LEVEL3);
			dam = 3 * dam + 15;
		}
		else if(k < 1300)
		{
			msg_print(MES_CRITICAL_LEVEL4);
			dam = 3 * dam + 20;
		}
		else
		{
			msg_print(MES_CRITICAL_LEVEL5);
			dam = ((7 * dam) / 2) + 25;
		}
	}

	return (dam);
}

