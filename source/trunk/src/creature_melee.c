#include "angband.h"

int melee_hit_chance(creature_type *creature_ptr, int to_hit, int ev)
{
	int chance = 0;
	int hit_power = creature_ptr->skill_thn + (creature_ptr->to_hit[0] + to_hit) * BTH_PLUS_ADJ;

	if (hit_power <= 0) return 5;

	chance = 100 - ((ev * 75) / hit_power);

	if (chance > 95) chance = 95;
	if (chance < 5) chance = 5;
	if (creature_ptr->chara_idx == CHARA_NAMAKE)
		chance = (chance * 19 + 9) / 20;
	return chance;
}

/*
 * Determine if the player "hits" a creature (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_melee(creature_type *attacker_ptr, int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	if (attacker_ptr->chara_idx == CHARA_NAMAKE)
		if (one_in_(20)) return (FALSE);

	/* Wimpy attack never hits */
	if (chance <= 0) return (FALSE);

	/* Penalize invisible targets */
	if (!vis) chance = (chance + 1) / 2;

	/* Power must defeat armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}


/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 */
s16b test_critial_melee(creature_type *creature_ptr, int weight, int plus, int dam, s16b meichuu, int mode)
{
	int i, k;

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (creature_ptr->lev * 3));

	/* Chance */
	if ((randint1((creature_ptr->class_idx == CLASS_NINJA) ? 4444 : 5000) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		k = weight + randint1(650);
		if ((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);

		if (k < 400)
		{
#ifdef JP
			msg_print("�育�������������I");
#else
			msg_print("It was a good hit!");
#endif

			dam = 2 * dam + 5;
		}
		else if (k < 700)
		{
#ifdef JP
			msg_print("���Ȃ�̎育�������������I");
#else
			msg_print("It was a great hit!");
#endif

			dam = 2 * dam + 10;
		}
		else if (k < 900)
		{
#ifdef JP
			msg_print("��S�̈ꌂ���I");
#else
			msg_print("It was a superb hit!");
#endif

			dam = 3 * dam + 15;
		}
		else if (k < 1300)
		{
#ifdef JP
			msg_print("�ō��̉�S�̈ꌂ���I");
#else
			msg_print("It was a *GREAT* hit!");
#endif

			dam = 3 * dam + 20;
		}
		else
		{
#ifdef JP
			msg_print("��ނȂ��ō��̉�S�̈ꌂ���I");
#else
			msg_print("It was a *SUPERB* hit!");
#endif

			dam = ((7 * dam) / 2) + 25;
		}
	}

	return (dam);
}

