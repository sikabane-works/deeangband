/* File: monster2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: misc code for monsters */

#include "angband.h"

#define HORDE_NOGOOD 0x01
#define HORDE_NOEVIL 0x02

cptr horror_desc[MAX_SAN_HORROR] =
{
#ifdef JP
	"忌まわしい",
	"底知れぬ",
	"ぞっとする",
	"破滅的な",
	"冒涜的な",

	"いやな",
	"恐ろしい",
	"不潔な",
	"容赦のない",
	"おぞましい",

	"地獄の",
	"身の毛もよだつ",
	"地獄の",
	"忌まわしい",
	"悪夢のような",

	"嫌悪を感じる",
	"罰当たりな",
	"恐い",
	"不浄な",
	"言うもおぞましい",
#else
	"abominable",
	"abysmal",
	"appalling",
	"baleful",
	"blasphemous",

	"disgusting",
	"dreadful",
	"filthy",
	"grisly",
	"hideous",

	"hellish",
	"horrible",
	"infernal",
	"loathsome",
	"nightmarish",

	"repulsive",
	"sacrilegious",
	"terrible",
	"unclean",
	"unspeakable",
#endif

};

cptr delight_desc[MAX_SAN_HORROR] =
{
#ifdef JP
	"素晴らしい",
	"美しい",
	"奥ゆかしい",
	"愛おしい",
	"優美な",

	"馥郁たる",
	"天に昇るような",
	"恍惚たる",
	"祝福された",
	"明媚な",

	"絶頂するような",
	"法悦たる",
	"輝ける",
	"温かみある",
	"楽園のような",

	"好感を感じる",
	"絶賛すべき",
	"胸の熱くなる",
	"清浄な",
	"語るに尽くせぬ",
#else
	"wonderful",
	"beautiful",
	"elegant",
	"tender",
	"graceful",

	"fragrant",
	"happy",
	"enchanted",
	"blessed",
	"scenic",

	"ecstatic",
	"religious",
	"luminous",
	"warmhearted",
	"heavenly",

	"favorite",
	"excellent",
	"passional",
	"immaculate",
	"indescribable",
#endif

};


cptr funny_desc[MAX_SAN_FUNNY] =
{
#ifdef JP
	"間抜けな",
	"滑稽な",
	"ばからしい",
	"無味乾燥な",
	"馬鹿げた",

	"笑える",
	"ばかばかしい",
	"ぶっとんだ",
	"いかした",
	"ポストモダンな",

	"ファンタスティックな",
	"ダダイズム的な",
	"キュビズム的な",
	"宇宙的な",
	"卓越した",

	"理解不能な",
	"ものすごい",
	"驚くべき",
	"信じられない",
	"カオティックな",

	"野性的な",
	"非常識な",
#else
	"silly",
	"hilarious",
	"absurd",
	"insipid",
	"ridiculous",

	"laughable",
	"ludicrous",
	"far-out",
	"groovy",
	"postmodern",

	"fantastic",
	"dadaistic",
	"cubistic",
	"cosmic",
	"awesome",

	"incomprehensible",
	"fabulous",
	"amazing",
	"incredible",
	"chaotic",

	"wild",
	"preposterous",
#endif

};

cptr funny_comments[MAX_SAN_COMMENT] =
{
#ifdef JP
  /* nuke me */
	"最高だぜ！",
	"うひょー！",
	"いかすぜ！",
	"すんばらしい！",
	"ぶっとびー！"
#else
	"Wow, cosmic, man!",
	"Rad!",
	"Groovy!",
	"Cool!",
	"Far out!"
#endif

};


/*
 * Each player starts out with a few items, given as tval/sval pairs.
 * In addition, he always has some food and a few torches.
 */
static byte class_equipment_init[MAX_CLASS][10][2] =
{
	{
		// None
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Warrior */
		{ TV_RING, SV_RING_RES_FEAR }, /* Warriors need it! */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD },
		{ TV_SHIELD, SV_SMALL_METAL_SHIELD},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Mage */
		{ TV_SWORD, SV_DAGGER },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},

	},

	{
		/* Priest */
		{ TV_HAFTED, SV_MACE },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Rogue */
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Ranger */
		{ TV_SWORD, SV_DAGGER },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Paladin */
		{ TV_SCROLL, SV_SCROLL_PROTECTION_FROM_EVIL },
		{ TV_SWORD, SV_BROAD_SWORD },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Warrior-Mage */
		{ TV_SWORD, SV_SHORT_SWORD },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Chaos Warrior */
		{ TV_HARD_ARMOR, SV_METAL_SCALE_MAIL },
		{ TV_SWORD, SV_BROAD_SWORD },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Monk */
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POTION, SV_POTION_HEROISM },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Mindcrafter */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_SMALL_SWORD },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* High Mage */
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_SWORD, SV_DAGGER },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Tourist */
		{ TV_FOOD, SV_FOOD_JERKY},
		{ TV_SCROLL, SV_SCROLL_MAPPING },
		{ TV_BOW, SV_SLING},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Imitator */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_SHORT_SWORD},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Beastmaster */
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POLEARM, SV_SPEAR},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Sorcerer */
		{ TV_HAFTED, SV_WIZSTAFF }, /* Hack: for realm1 book */
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Archer */
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL},
		{ TV_SWORD, SV_SHORT_SWORD },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Magic eater */
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Bard */
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Red Mage */
		{ TV_SOFT_ARMOR, SV_HARD_LEATHER_ARMOR},
		{ TV_SWORD, SV_SHORT_SWORD },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Samurai */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_SWORD, SV_KATANA },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* ForceTrainer */
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_POTION, SV_POTION_RESTORE_MANA },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Blue Mage */
		{ TV_SOFT_ARMOR, SV_ROBE },
		{ TV_WAND, SV_WAND_MAGIC_MISSILE },
		{ TV_SWORD, SV_DAGGER },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Cavalry */
		{ TV_BOW, SV_SHORT_BOW },
		{ TV_SOFT_ARMOR, SV_LEATHER_SCALE_MAIL},
		{ TV_POLEARM, SV_BROAD_SPEAR},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Berserker */
		{ TV_POTION, SV_POTION_HEALING },
		{ TV_HARD_ARMOR, SV_AUGMENTED_CHAIN_MAIL },
		{ TV_POLEARM, SV_BROAD_AXE },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Weaponsmith */
		{ TV_RING, SV_RING_RES_FEAR }, /* Warriors need it! */
		{ TV_HARD_ARMOR, SV_CHAIN_MAIL },
		{ TV_POLEARM, SV_BROAD_AXE },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Mirror-Master */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_RING, SV_RING_SUSTAIN_INT},
		{ TV_SWORD, SV_DAGGER },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Ninja */
		{ TV_POTION, SV_POTION_SPEED },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		/* Sniper */
		{ TV_BOW, SV_LIGHT_XBOW },
		{ TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR },
		{ TV_SWORD, SV_DAGGER },
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		// Commoner
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

	{
		// Soldier
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
		{ 0, 0},
	},

};



/*
 * Set the target of counter attack
 */
void set_target(creature_type *m_ptr, int y, int x)
{
	m_ptr->target_y = y;
	m_ptr->target_x = x;
}


/*
 * Reset the target of counter attack
 */
void reset_target(creature_type *m_ptr)
{
	set_target(m_ptr, 0, 0);
}


/*
 *  Extract monster race pointer of a monster's true form
 */
species_type *real_species_ptr(creature_type *m_ptr)
{
	species_type *r_ptr = &species_info[m_ptr->species_idx];
	/* Extract real race */
	if (m_ptr->mflag2 & MFLAG2_CHAMELEON)
	{
		if (is_unique_species(r_ptr))
			return &species_info[MON_CHAMELEON_K];
		else
			return &species_info[MON_CHAMELEON];
	}
	else
	{
		return r_ptr;
	}
}


/*
 * Delete a monster by index.
 *
 * When a monster is deleted, all of its objects are deleted.
 */
void delete_species_idx(creature_type *creature_ptr)
{
	int x, y;
	species_type *r_ptr = &species_info[creature_ptr->species_idx];

	s16b this_o_idx, next_o_idx = 0;


	/* Get location */
	y = creature_ptr->fy;
	x = creature_ptr->fx;


	/* Hack -- Reduce the racial counter */
	real_species_ptr(creature_ptr)->cur_num--;

	/* Hack -- count the number of "reproducers" */
	if (has_cf_creature(creature_ptr, CF_MULTIPLY)) num_repro--;

	if (creature_ptr->paralyzed) (void)set_paralyzed(creature_ptr, 0);
	if (creature_ptr->fast) (void)set_fast(creature_ptr, 0, FALSE);
	if (creature_ptr->slow) (void)set_slow(creature_ptr, 0, FALSE);
	if (creature_ptr->stun) (void)set_stun(creature_ptr, 0);
	if (creature_ptr->confused) (void)set_confused(creature_ptr, 0);
	if (creature_ptr->afraid) (void)set_afraid(creature_ptr, 0);
	if (creature_ptr->invuln) (void)set_invuln(creature_ptr, 0, FALSE);


	/* Hack -- remove target monster */
	if (creature_ptr == &creature_list[target_who]) target_who = 0;

	/* Hack -- remove tracked monster */
	if (creature_ptr == &creature_list[health_who]) health_track(0);

	if (&creature_list[pet_t_m_idx] == creature_ptr) pet_t_m_idx = 0;
	if (&creature_list[riding_t_m_idx] == creature_ptr) riding_t_m_idx = 0;
	if (&creature_list[creature_ptr->riding] == creature_ptr) creature_ptr->riding = 0;

	/* Monster is gone */
	cave[y][x].m_idx = 0;


	/* Delete objects */
	for (this_o_idx = creature_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/*
		 * o_ptr->held_m_idx is needed in delete_object_idx()
		 * to prevent calling lite_spot()
		 */

		/* Delete the object */
		delete_object_idx(this_o_idx);
	}


	if (is_pet(player_ptr, creature_ptr)) check_pets_num_and_align(creature_ptr, FALSE);


	/* Wipe the Monster */
	(void)WIPE(creature_ptr, creature_type);

	/* Count monsters */
	m_cnt--;

	/* Visual update */
	lite_spot(y, x);

	/* Update some things */
	if (is_lighting_creature(creature_ptr) || is_darken_creature(creature_ptr))
		update |= (PU_MON_LITE);
}


/*
 * Delete the monster, if any, at a given location
 */
void delete_monster(int y, int x)
{
	cave_type *c_ptr;

	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Check the grid */
	c_ptr = &cave[y][x];

	/* Delete the monster (if any) */
	if (c_ptr->m_idx) delete_species_idx(&creature_list[c_ptr->m_idx]);
}


/*
 * Move an object from index i1 to index i2 in the object list
 */
static void compact_monsters_aux(int i1, int i2)
{
	int y, x, i;

	cave_type *c_ptr;

	creature_type *m_ptr;

	s16b this_o_idx, next_o_idx = 0;


	/* Do nothing */
	if (i1 == i2) return;


	/* Old monster */
	m_ptr = &creature_list[i1];

	/* Location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Cave grid */
	c_ptr = &cave[y][x];

	/* Update the cave */
	c_ptr->m_idx = i2;

	/* Repair objects being carried by monster */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Reset monster pointer */
		o_ptr->held_m_idx = i2;
	}

	/* Hack -- Update the target */
	if (target_who == i1) target_who = i2;

	/* Hack -- Update the target */
	if (pet_t_m_idx == i1) pet_t_m_idx = i2;
	if (riding_t_m_idx == i1) riding_t_m_idx = i2;

	/* Hack -- Update the riding */
	if (p_ptr->riding == i1) p_ptr->riding = i2;

	/* Hack -- Update the health bar */
	if (health_who == i1) health_track(i2);

	/* Hack -- Update parent index */
	if (is_pet(player_ptr, m_ptr))
	{
		for (i = 1; i < m_max; i++)
		{
			creature_type *m2_ptr = &creature_list[i];

			if (m2_ptr->parent_m_idx == i1)
				m2_ptr->parent_m_idx = i2;
		}
	}

	/* Structure copy */
	COPY(&creature_list[i2], &creature_list[i1], creature_type);

	/* Wipe the hole */
	(void)WIPE(&creature_list[i1], creature_type);

	for (i = 0; i < MAX_MTIMED; i++)
	{
		int mproc_idx = get_mproc_idx(&creature_list[i1], i);
		if (mproc_idx >= 0) mproc_list[i][mproc_idx] = &creature_list[i2];
	}
}


/*
 * Compact and Reorder the monster list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" monsters, we base the saving throw
 * on a combination of monster level, distance from player, and
 * current "desperation".
 *
 * After "compacting" (if needed), we "reorder" the monsters into a more
 * compact order, and we reset the allocation info, and the "live" array.
 */
void compact_monsters(int size)
{
	int		i, num, cnt;
	int		cur_lev, cur_dis, chance;

	/* Message (only if compacting) */
#ifdef JP
	if (size) msg_print("モンスター情報を圧縮しています...");
#else
	if (size) msg_print("Compacting monsters...");
#endif


	/* Compact at least 'size' objects */
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Check all the monsters */
		for (i = 1; i < m_max; i++)
		{
			creature_type *m_ptr = &creature_list[i];

			species_type *r_ptr = &species_info[m_ptr->species_idx];

			/* Paranoia -- skip "dead" monsters */
			if (!m_ptr->species_idx) continue;

			/* Hack -- High level monsters start out "immune" */
			if (r_ptr->level > cur_lev) continue;

			if (i == p_ptr->riding) continue;

			/* Ignore nearby monsters */
			if ((cur_dis > 0) && (m_ptr->cdis < cur_dis)) continue;

			/* Saving throw chance */
			chance = 90;

			/* Only compact "Quest" Monsters in emergencies */
			if ((is_quest_species(r_ptr)) && (cnt < 1000)) chance = 100;

			/* Try not to compact Unique Monsters */
			if (is_unique_species(r_ptr)) chance = 100;

			/* All monsters get a saving throw */
			if (randint0(100) < chance) continue;

			if (record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
			{
				char m_name[80];

				creature_desc(m_name, m_ptr, MD_INDEF_VISIBLE);
				do_cmd_write_nikki(NIKKI_NAMED_PET, RECORD_NAMED_PET_COMPACT, m_name);
			}

			/* Delete the monster */
			delete_species_idx(m_ptr);

			/* Count the monster */
			num++;
		}
	}


	/* Excise dead monsters (backwards!) */
	for (i = m_max - 1; i >= 1; i--)
	{
		/* Get the i'th monster */
		creature_type *m_ptr = &creature_list[i];

		/* Skip real monsters */
		if (m_ptr->species_idx) continue;

		/* Move last monster into open hole */
		compact_monsters_aux(m_max - 1, i);

		/* Compress "m_max" */
		m_max--;
	}
}

void birth_uniques(void)
{
	int i, j;
	char buf[80];

	int t = sizeof(creature_type);

	/* Init Unique Count */
	max_unique = 0;
	for(i = 0; i < max_species_idx; i++)
	{
		if(is_unique_species(&species_info[i])) max_unique++;
	}

	C_MAKE(u_info, max_unique, creature_type);
	C_WIPE(u_info, max_unique, creature_type);
	j = 0;


	for(i = 0; i < max_species_idx; i++)
	{
		sprintf(buf, "Please Wait ... Unique Making[%d]", i);
		prt(buf, 0, 0);
		Term_fresh();

		if(is_unique_species(&species_info[i]))
		{
			creature_type save_ptr;
			generate_creature(&u_info[j], i, &save_ptr, GC_AUTO);
			j++;
		}
	}
//	C_KILL(u_info, max_unique, creature_type);

}


/*
 * Delete/Remove all the monsters when the player leaves the level
 *
 * This is an efficient method of simulating multiple calls to the
 * "delete_monster()" function, with no visual effects.
 */
void wipe_creature_list(void)
{
	int i;

	/* Hack -- if Banor or Lupart dies, stay another dead */
	if (!species_info[MON_BANORLUPART].max_num)
	{
		if (species_info[MON_BANOR].max_num)
		{
			species_info[MON_BANOR].max_num = 0;
			species_info[MON_BANOR].r_pkills++;
			species_info[MON_BANOR].r_akills++;
			if (species_info[MON_BANOR].r_tkills < MAX_SHORT) species_info[MON_BANOR].r_tkills++;
		}
		if (species_info[MON_LUPART].max_num)
		{
			species_info[MON_LUPART].max_num = 0;
			species_info[MON_LUPART].r_pkills++;
			species_info[MON_LUPART].r_akills++;
			if (species_info[MON_LUPART].r_tkills < MAX_SHORT) species_info[MON_LUPART].r_tkills++;
		}
	}

	/* Delete all the monsters */
	for (i = m_max - 1; i >= 1; i--)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Monster is gone */
		cave[m_ptr->fy][m_ptr->fx].m_idx = 0;

		/* Wipe the Monster */
		(void)WIPE(m_ptr, creature_type);

	}

	/*
	 * Wiping racial counters of all monsters and incrementing of racial
	 * counters of monsters in party_mon[] are required to prevent multiple
	 * generation of unique monster who is the minion of player.
	 */

	/* Hack -- Wipe the racial counter of all monster races */
	for (i = 1; i < max_species_idx; i++) species_info[i].cur_num = 0;

	/* Reset "m_max" */
	m_max = 1;

	/* Reset "m_cnt" */
	m_cnt = 0;

	/* Reset "mproc_max[]" */
	for (i = 0; i < MAX_MTIMED; i++) mproc_max[i] = 0;

	/* Hack -- reset "reproducer" count */
	num_repro = 0;

	/* Hack -- no more target */
	target_who = 0;
	pet_t_m_idx = 0;
	riding_t_m_idx = 0;

	/* Hack -- no more tracking */
	health_track(0);
}


/*
 * Acquires and returns the index of a "free" monster.
 *
 * This routine should almost never fail, but it *can* happen.
 */
s16b m_pop(void)
{
	int i;


	/* Normal allocation */
	if (m_max < max_m_idx)
	{
		/* Access the next hole */
		i = m_max;

		/* Expand the array */
		m_max++;

		/* Count monsters */
		m_cnt++;

		/* Return the index */
		return (i);
	}


	/* Recycle dead monsters */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr;

		/* Acquire monster */
		m_ptr = &creature_list[i];

		/* Skip live monsters */
		if (m_ptr->species_idx) continue;

		/* Count monsters */
		m_cnt++;

		/* Use this monster */
		return (i);
	}


	/* Warn the player (except during dungeon creation) */
#ifdef JP
	if (character_dungeon) msg_print("モンスターが多すぎる！");
#else
	if (character_dungeon) msg_print("Too many monsters!");
#endif


	/* Try not to crash */
	return (0);
}




/*
 * Hack -- the "type" of the current "summon specific"
 */
static int summon_specific_type = 0;


/*
 * Hack -- the index of the summoning monster
 */
static int summon_specific_who = -1;


static bool summon_unique_okay = FALSE;


static bool summon_specific_aux(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	int okay = FALSE;

	/* Check our requirements */
	switch (summon_specific_type)
	{
		case SUMMON_ANT:
		{
			okay = (r_ptr->d_char == 'a');
			break;
		}

		case SUMMON_SPIDER:
		{
			okay = (r_ptr->d_char == 'S');
			break;
		}

		case SUMMON_HOUND:
		{
			okay = ((r_ptr->d_char == 'C') || (r_ptr->d_char == 'Z'));
			break;
		}

		case SUMMON_HYDRA:
		{
			okay = (r_ptr->d_char == 'M');
			break;
		}

		case SUMMON_ANGEL:
		{
			okay = (r_ptr->d_char == 'A' && (is_enemy_of_good_species(r_ptr) || is_enemy_of_evil_species(r_ptr)));
			break;
		}

		case SUMMON_DEMON:
		{
			okay = is_demon_species(r_ptr);
			break;
		}

		case SUMMON_UNDEAD:
		{
			okay = is_undead_species(r_ptr);
			break;
		}

		case SUMMON_DRAGON:
		{
			okay = is_dragon_species(r_ptr);
			break;
		}

		case SUMMON_HI_UNDEAD:
		{
			okay = ((r_ptr->d_char == 'L') ||
				(r_ptr->d_char == 'V') ||
				(r_ptr->d_char == 'W'));
			break;
		}

		case SUMMON_HI_DRAGON:
		{
			okay = (r_ptr->d_char == 'D');
			break;
		}

		case SUMMON_HI_DEMON:
		{
			okay = (((r_ptr->d_char == 'U') ||
				    (r_ptr->d_char == 'H') ||
				    (r_ptr->d_char == 'B')) &&
		  		    is_demon_species(r_ptr)) ? TRUE : FALSE;
			break;
		}

		case SUMMON_AMBERITES:
		{
			okay = (r_ptr->race_idx1 == RACE_AMBERITE) ? TRUE : FALSE;
			break;
		}

		case SUMMON_UNIQUE:
		{
			okay = (is_unique_species(r_ptr)) ? TRUE : FALSE;
			break;
		}

		case SUMMON_BIZARRE1:
		{
			okay = (r_ptr->d_char == 'm');
			break;
		}
		case SUMMON_BIZARRE2:
		{
			okay = (r_ptr->d_char == 'b');
			break;
		}
		case SUMMON_BIZARRE3:
		{
			okay = (r_ptr->d_char == 'Q');
			break;
		}

		case SUMMON_BIZARRE4:
		{
			okay = (r_ptr->d_char == 'v');
			break;
		}

		case SUMMON_BIZARRE5:
		{
			okay = (r_ptr->d_char == '$');
			break;
		}

		case SUMMON_BIZARRE6:
		{
			okay = ((r_ptr->d_char == '!') ||
				 (r_ptr->d_char == '?') ||
				 (r_ptr->d_char == '=') ||
				 (r_ptr->d_char == '$') ||
				 (r_ptr->d_char == '|'));
			break;
		}

		case SUMMON_GOLEM:
		{
			okay = (r_ptr->d_char == 'g');
			break;
		}

		case SUMMON_CYBER:
		{
			okay = ((r_ptr->d_char == 'U') && has_cf(&r_ptr->flags, CF_ROCKET));
			break;
		}


		case SUMMON_KIN:
		{
			okay = ((r_ptr->d_char == summon_kin_type) && (species_idx != MON_HAGURE));
			break;
		}

		case SUMMON_DAWN:
		{
			okay = (species_idx == MON_DAWN);
			break;
		}

		case SUMMON_ANIMAL:
		{
			okay = is_animal_species(r_ptr);
			break;
		}

		case SUMMON_ANIMAL_RANGER:
		{
			okay = (is_animal_species(r_ptr) &&
			       (my_strchr("abcflqrwBCHIJKMRS", r_ptr->d_char)) &&
			       !is_dragon_species(r_ptr) &&
			       !(is_enemy_of_good_species(r_ptr)) &&
			       !is_undead_species(r_ptr) &&
			       !is_demon_species(r_ptr) &&
			       !is_multiply_species(r_ptr));// &&
			       //TODO !(r_ptr->flags4 || r_ptr->flags5 || r_ptr->flags6));
			break;
		}

		case SUMMON_HI_DRAGON_LIVING:
		{
			okay = ((r_ptr->d_char == 'D') && monster_living(r_ptr));
			break;
		}

		case SUMMON_LIVING:
		{
			okay = monster_living(r_ptr);
			break;
		}

		case SUMMON_PHANTOM:
		{
			okay = (species_idx == MON_PHANTOM_B || species_idx == MON_PHANTOM_W);
			break;
		}

		case SUMMON_BLUE_HORROR:
		{
			okay = (species_idx == MON_BLUE_HORROR);
			break;
		}

		case SUMMON_ELEMENTAL:
		{
			okay = (r_ptr->d_char == 'E');
			break;
		}

		case SUMMON_VORTEX:
		{
			okay = (r_ptr->d_char == 'v');
			break;
		}

		case SUMMON_HYBRID:
		{
			okay = (r_ptr->d_char == 'H');
			break;
		}

		case SUMMON_BIRD:
		{
			okay = (r_ptr->d_char == 'B');
			break;
		}

		case SUMMON_KAMIKAZE:
		{
			int i;
			for (i = 0; i < 4; i++)
				if (r_ptr->blow[i].method == RBM_EXPLODE) okay = TRUE;
			break;
		}

		case SUMMON_KAMIKAZE_LIVING:
		{
			int i;

			for (i = 0; i < 4; i++)
				if (r_ptr->blow[i].method == RBM_EXPLODE) okay = TRUE;
			okay = (okay && monster_living(r_ptr));
			break;
		}

		case SUMMON_MANES:
		{
			okay = (species_idx == MON_MANES);
			break;
		}

		case SUMMON_LOUSE:
		{
			okay = (species_idx == MON_LOUSE);
			break;
		}

		case SUMMON_GUARDIANS:
		{
			okay = (is_guardian_species(r_ptr));
			break;
		}

		case SUMMON_KNIGHTS:
		{
			okay = ((species_idx == MON_NOV_PALADIN) ||
				(species_idx == MON_NOV_PALADIN_G) ||
				(species_idx == MON_PALADIN) ||
				(species_idx == MON_W_KNIGHT) ||
				(species_idx == MON_ULTRA_PALADIN) ||
				(species_idx == MON_KNI_TEMPLAR));
			break;
		}

		case SUMMON_EAGLES:
		{
			okay = (r_ptr->d_char == 'B' &&
				is_wild_mountain_species(r_ptr) &&
				is_wild_only_species(r_ptr));
			break;
		}

		case SUMMON_PIRANHAS:
		{
			okay = (species_idx == MON_PIRANHA);
			break;
		}

		case SUMMON_ARMAGE_GOOD:
		{
			okay = (r_ptr->d_char == 'A' && is_enemy_of_evil_species(r_ptr));
			break;
		}

		case SUMMON_ARMAGE_EVIL:
		{
			okay = ((is_demon_species(r_ptr)) ||
				(r_ptr->d_char == 'A' && is_enemy_of_good_species(r_ptr)));
			break;
		}
	}

	/* Result */
	/* Since okay is int, "return (okay);" is not correct. */
	return (bool)(okay ? TRUE : FALSE);
}


static int chameleon_change_m_idx = 0;


/*
 * Some dungeon types restrict the possible monsters.
 * Return TRUE is the monster is OK and FALSE otherwise
 */
static bool restrict_monster_to_dungeon(int species_idx)
{
	dungeon_info_type *d_ptr = &d_info[dungeon_type];
	species_type *r_ptr = &species_info[species_idx];
	//byte a;

	if (d_ptr->flags1 & DF1_CHAMELEON)
	{
		if (chameleon_change_m_idx) return TRUE;
	}
	if (d_ptr->flags1 & DF1_NO_MAGIC)
	{
		if (species_idx != MON_CHAMELEON &&
		    r_ptr->freq_spell && has_non_magic_skill_flags(&r_ptr->flags))
			return FALSE;
	}
	if (d_ptr->flags1 & DF1_NO_MELEE)
	{
		if (species_idx == MON_CHAMELEON) return TRUE;
		if (!(has_bolt_flags(&r_ptr->flags) || has_beam_flags(&r_ptr->flags) || has_ball_flags(&r_ptr->flags) || 
			has_cf(&r_ptr->flags, CF_CAUSE_1) || 
			has_cf(&r_ptr->flags, CF_CAUSE_2) || 
			has_cf(&r_ptr->flags, CF_CAUSE_3) || 
			has_cf(&r_ptr->flags, CF_CAUSE_4) || 
			has_cf(&r_ptr->flags, CF_MIND_BLAST) || 
			has_cf(&r_ptr->flags, CF_BRAIN_SMASH)))
			return FALSE;
	}
	if (d_ptr->flags1 & DF1_BEGINNER)
	{
		if (r_ptr->level > dun_level)
			return FALSE;
	}

	if (d_ptr->special_div >= 64) return TRUE;
	if (summon_specific_type && !(d_ptr->flags1 & DF1_CHAMELEON)) return TRUE;

	/* TODO
	switch (d_ptr->mode)
	{
	case DUNGEON_MODE_AND:
		for (a = 0; a < CF_FLAG_MAX; a++)
		{
			if(has_cf(&d_ptr->c_flags, a)
				&& !has_cf(&r_ptr->flags, a)) return FALSE; 
		}
		for (a = 0; a < 5; a++)
			if (d_ptr->r_char[a] && (d_ptr->r_char[a] != r_ptr->d_char)) return FALSE;

		return TRUE;

	case DUNGEON_MODE_NAND:
		for (a = 0; a < CF_FLAG_MAX; a++)
		{
			if(has_cf(&d_ptr->c_flags, a)
				&& !has_cf(&r_ptr->flags, a)) return TRUE; 
		}
		for (a = 0; a < 5; a++)
			if (d_ptr->r_char[a] && (d_ptr->r_char[a] != r_ptr->d_char)) return TRUE;

		return FALSE;

	case DUNGEON_MODE_OR:
		for (a = 0; a < CF_FLAG_MAX; a++)
		{
			if(has_cf(&d_ptr->c_flags, a)
				&& has_cf(&r_ptr->flags, a)) return TRUE; 
		}
		for (a = 0; a < 5; a++)
			if (d_ptr->r_char[a] == r_ptr->d_char) return TRUE;

		return FALSE;

	case DUNGEON_MODE_NOR:
		for (a = 0; a < CF_FLAG_MAX; a++)
		{
			if(has_cf(&d_ptr->c_flags, a)
				&& has_cf(&r_ptr->flags, a)) return FALSE; 
		}
		for (a = 0; a < 5; a++)
			if (d_ptr->r_char[a] == r_ptr->d_char) return FALSE;

		return TRUE;
	}
	*/

	return TRUE;
}

/*
 * Apply a "monster restriction function" to the "monster allocation table"
 */
errr get_mon_num_prep(creature_hook_type creature_hook,
					  creature_hook_type creature_hook2)
{
	int i;

	/* Todo: Check the hooks for non-changes */

	/* Set the new hooks */
	creature_hook_type get_mon_num_hook  = creature_hook;
	creature_hook_type get_mon_num2_hook = creature_hook2;

	/* Scan the allocation table */
	for (i = 0; i < alloc_race_size; i++)
	{
		species_type	*r_ptr;
		
		/* Get the entry */
		alloc_entry *entry = &alloc_race_table[i];

		entry->prob2 = 0;
		r_ptr = &species_info[entry->index];

		/* Skip monsters which don't pass the restriction */
		if ((get_mon_num_hook  && !((*get_mon_num_hook)(entry->index))) ||
		    (get_mon_num2_hook && !((*get_mon_num2_hook)(entry->index))))
			continue;

		if (!inside_battle && !chameleon_change_m_idx &&
		    summon_specific_type != SUMMON_GUARDIANS)
		{
			/* Hack -- don't create questors */
			if (is_quest_species(r_ptr))
				continue;

			if (is_guardian_species(r_ptr))
				continue;

			/* Depth Monsters never appear out of depth */
			if (is_force_depth_species(r_ptr) &&
			    (r_ptr->level > dun_level))
				continue;
		}

		/* Accept this monster */
		entry->prob2 = entry->prob1;

		if (dun_level && (!inside_quest || is_fixed_quest_idx(inside_quest)) && !restrict_monster_to_dungeon(entry->index) && !inside_battle)
		{
			int hoge = entry->prob2 * d_info[dungeon_type].special_div;
			entry->prob2 = hoge / 64;
			if (randint0(64) < (hoge & 0x3f)) entry->prob2++;
		}
	}

	/* Success */
	return (0);
}


/*
 * Apply a "monster restriction function" to the "monster allocation table"
 */
errr get_mon_num_prep2(creature_type *player_ptr,
					   creature_hook_type2 creature_hook,
					   creature_hook_type2 creature_hook2)
{
	int i;

	/* Todo: Check the hooks for non-changes */

	/* Set the new hooks */
	creature_hook_type2 get_mon_num_hook  = creature_hook;
	creature_hook_type2 get_mon_num2_hook = creature_hook2;

	/* Scan the allocation table */
	for (i = 0; i < alloc_race_size; i++)
	{
		species_type	*r_ptr;
		
		/* Get the entry */
		alloc_entry *entry = &alloc_race_table[i];

		entry->prob2 = 0;
		r_ptr = &species_info[entry->index];

		/* Skip monsters which don't pass the restriction */
		if ((get_mon_num_hook  && !((*get_mon_num_hook) (player_ptr, entry->index))) ||
		    (get_mon_num2_hook && !((*get_mon_num2_hook)(player_ptr, entry->index))))
			continue;

		if (!inside_battle && !chameleon_change_m_idx &&
		    summon_specific_type != SUMMON_GUARDIANS)
		{
			/* Hack -- don't create questors */
			if (is_quest_species(r_ptr))
				continue;

			if (is_guardian_species(r_ptr))
				continue;

			/* Depth Monsters never appear out of depth */
			if (is_force_depth_species(r_ptr) &&
			    (r_ptr->level > dun_level))
				continue;
		}

		/* Accept this monster */
		entry->prob2 = entry->prob1;

		if (dun_level && (!inside_quest || is_fixed_quest_idx(inside_quest)) && !restrict_monster_to_dungeon(entry->index) && !inside_battle)
		{
			int hoge = entry->prob2 * d_info[dungeon_type].special_div;
			entry->prob2 = hoge / 64;
			if (randint0(64) < (hoge & 0x3f)) entry->prob2++;
		}
	}

	/* Success */
	return (0);
}




static int mysqrt(int n)
{
	int tmp = n>>1;
	int tasu = 10;
	int kaeriti = 1;

	if (!tmp)
	{
		if (n) return 1;
		else return 0;
	}

	while(tmp)
	{
		if ((n/tmp) < tmp)
		{
			tmp >>= 1;
		}
		else break;
	}
	kaeriti = tmp;
	while(tasu)
	{
		if ((n/tmp) < tmp)
		{
			tasu--;
			tmp = kaeriti;
		}
		else
		{
			kaeriti = tmp;
			tmp += tasu;
		}
	}
	return kaeriti;
}

/*
 * Choose a monster race that seems "appropriate" to the given level
 *
 * This function uses the "prob2" field of the "monster allocation table",
 * and various local information, to calculate the "prob3" field of the
 * same table, which is then used to choose an "appropriate" monster, in
 * a relatively efficient manner.
 *
 * Note that "town" monsters will *only* be created in the town, and
 * "normal" monsters will *never* be created in the town, unless the
 * "level" is "modified", for example, by polymorph or summoning.
 *
 * There is a small chance (1/50) of "boosting" the given depth by
 * a small amount (up to four levels), except in the town.
 *
 * It is (slightly) more likely to acquire a monster of the given level
 * than one of a lower level.  This is done by choosing several monsters
 * appropriate to the given level and keeping the "hardest" one.
 *
 * Note that if no monsters are "appropriate", then this function will
 * fail, and return zero, but this should *almost* never happen.
 */
s16b get_mon_num(int level)
{
	int			i, j, p;

	int			species_idx;

	long		value, total;

	species_type	*r_ptr;

	alloc_entry		*table = alloc_race_table;

	int pls_kakuritu, pls_level;
	int hoge=mysqrt(level*10000L);

	if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;

	if ((dungeon_turn > hoge*(TURNS_PER_TICK*500L)) && !level)
	{
		pls_kakuritu = MAX(2, NASTY_MON-((dungeon_turn/(TURNS_PER_TICK*2500L)-hoge/10)));
		pls_level = MIN(8,3 + dungeon_turn/(TURNS_PER_TICK*20000L)-hoge/40);
	}
	else
	{
		pls_kakuritu = NASTY_MON;
		pls_level = 2;
	}

	if (d_info[dungeon_type].flags1 & DF1_MAZE)
	{
		pls_kakuritu = MIN(pls_kakuritu/2, pls_kakuritu-10);
		if (pls_kakuritu < 2) pls_kakuritu = 2;
		pls_level += 2;
		level += 3;
	}

	/* Boost the level */
	if ((level > 0) && !inside_battle && !(d_info[dungeon_type].flags1 & DF1_BEGINNER))
	{
		/* Nightmare mode allows more out-of depth monsters */
		if (curse_of_Iluvatar && !randint0(pls_kakuritu))
		{
			/* What a bizarre calculation */
			level = 1 + (level * MAX_DEPTH / randint1(MAX_DEPTH));
		}
		else
		{
			/* Occasional "nasty" monster */
			if (!randint0(pls_kakuritu))
			{
				/* Pick a level bonus */
				int d = MIN(5, level/10) + pls_level;

				/* Boost the level */
				level += d;
			}

			/* Occasional "nasty" monster */
			if (!randint0(pls_kakuritu))
			{
				/* Pick a level bonus */
				int d = MIN(5, level/10) + pls_level;

				/* Boost the level */
				level += d;
			}
		}
	}


	/* Reset total */
	total = 0L;

	/* Process probabilities */
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Monsters are sorted by depth */
		if (table[i].level > level) break;

		/* Default */
		table[i].prob3 = 0;

		/* Access the "species_idx" of the chosen monster */
		species_idx = table[i].index;

		/* Access the actual race */
		r_ptr = &species_info[species_idx];

		/* Citizens doesn't wander. */
		if (is_citizen_species(r_ptr)) continue;

		if (!inside_battle && !chameleon_change_m_idx)
		{
			/* Hack -- "unique" monsters must be "unique" */
			if (((is_unique_species(r_ptr)) || has_cf(&r_ptr->flags, CF_NAZGUL)) &&
			    (r_ptr->cur_num >= r_ptr->max_num))
			{
				continue;
			}

			if (is_sub_unique_species(r_ptr) && (r_ptr->cur_num >= 1))
			{
				continue;
			}

			if (species_idx == MON_BANORLUPART)
			{
				if (species_info[MON_BANOR].cur_num > 0) continue;
				if (species_info[MON_LUPART].cur_num > 0) continue;
			}
		}

		/* Accept */
		table[i].prob3 = table[i].prob2;

		/* Total */
		total += table[i].prob3;
	}

	/* No legal monsters */
	if (total <= 0) return (0);


	/* Pick a monster */
	value = randint0(total);

	/* Find the monster */
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Found the entry */
		if (value < table[i].prob3) break;

		/* Decrement */
		value = value - table[i].prob3;
	}


	/* Power boost */
	p = randint0(100);

	/* Try for a "harder" monster once (50%) or twice (10%) */
	if (p < 60)
	{
		/* Save old */
		j = i;

		/* Pick a monster */
		value = randint0(total);

		/* Find the monster */
		for (i = 0; i < alloc_race_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Try for a "harder" monster twice (10%) */
	if (p < 10)
	{
		/* Save old */
		j = i;

		/* Pick a monster */
		value = randint0(total);

		/* Find the monster */
		for (i = 0; i < alloc_race_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Result */
	return (table[i].index);
}





/*
 * Build a string describing a monster in some way.
 *
 * We can correctly describe monsters based on their visibility.
 * We can force all monsters to be treated as visible or invisible.
 * We can build nominatives, objectives, possessives, or reflexives.
 * We can selectively pronominalize hidden, visible, or all monsters.
 * We can use definite or indefinite descriptions for hidden monsters.
 * We can use definite or indefinite descriptions for visible monsters.
 *
 * Pronominalization involves the gender whenever possible and allowed,
 * so that by cleverly requesting pronominalization / visibility, you
 * can get messages like "You hit someone.  She screams in agony!".
 *
 * Reflexives are acquired by requesting Objective plus Possessive.
 *
 * If no m_ptr arg is given (?), the monster is assumed to be hidden,
 * unless the "Assume Visible" mode is requested.
 *
 * If no r_ptr arg is given, it is extracted from m_ptr and species_info
 * If neither m_ptr nor r_ptr is given, the monster is assumed to
 * be neuter, singular, and hidden (unless "Assume Visible" is set),
 * in which case you may be in trouble... :-)
 *
 * I am assuming that no monster name is more than 70 characters long,
 * so that "char desc[80];" is sufficiently large for any result.
 *
 * Mode Flags:
 *  MD_OBJECTIVE       --> Objective (or Reflexive)
 *  MD_POSSESSIVE      --> Possessive (or Reflexive)
 *  MD_INDEF_HIDDEN    --> Use indefinites for hidden monsters ("something")
 *  MD_INDEF_VISIBLE   --> Use indefinites for visible monsters ("a kobold")
 *  MD_PRON_HIDDEN     --> Pronominalize hidden monsters
 *  MD_PRON_VISIBLE    --> Pronominalize visible monsters
 *  MD_ASSUME_HIDDEN   --> Assume the monster is hidden
 *  MD_ASSUME_VISIBLE  --> Assume the monster is visible
 *  MD_TRUE_NAME       --> Chameleon's true name
 *  MD_IGNORE_HALLU    --> Ignore hallucination, and penetrate shape change
 *  MD_IGNORE_EGO_DESC --> Ignore Ego description
 *
 * Useful Modes:
 *  0x00 --> Full nominative name ("the kobold") or "it"
 *  MD_INDEF_HIDDEN --> Full nominative name ("the kobold") or "something"
 *  MD_ASSUME_VISIBLE --> Genocide resistance name ("the kobold")
 *  MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE --> Killing name ("a kobold")
 *  MD_PRON_VISIBLE | MD_POSSESSIVE
 *    --> Possessive, genderized if visable ("his") or "its"
 *  MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE
 *    --> Reflexive, genderized if visable ("himself") or "itself"
 *
 */
void creature_desc(char *desc, creature_type *creature_ptr, int mode)
{
	cptr            res;
	species_type    *species_ptr;

	cptr            name;
	char            buf[128];
	char            silly_name[1024];
	bool            seen, pron;
	bool            named = FALSE;


	desc[0] = '\0';


	if(is_player(creature_ptr))
	{
#ifdef JP
		(void)strcpy(desc, "あなた");
#else
		(void)strcpy(desc, "You");
#endif
		return;
	}

	species_ptr = &species_info[creature_ptr->ap_species_idx];

	/* Mode of MD_TRUE_NAME will reveal Chameleon's true name */
	if (mode & MD_TRUE_NAME) name = (species_name + real_species_ptr(creature_ptr)->name);
	else name = (species_name + species_ptr->name);

	/* Are we hallucinating? (Idea from Nethack...) */
	if (player_ptr->image && !(mode & MD_IGNORE_HALLU))
	{
		if (one_in_(2))
		{
#ifdef JP
			if (!get_rnd_line("silly_j.txt", creature_ptr->species_idx, silly_name))
#else
			if (!get_rnd_line("silly.txt", creature_ptr->species_idx, silly_name))
#endif

				named = TRUE;
		}

		if (!named)
		{
			species_type *hallu_race;

			do
			{
				hallu_race = &species_info[randint1(max_species_idx - 1)];
			}
			while (!hallu_race->name || is_unique_species(hallu_race));

			strcpy(silly_name, (species_name + hallu_race->name));
		}

		/* Better not strcpy it, or we could corrupt species_info... */
		name = silly_name;
	}


	/* Can we "see" it (exists + forced, or visible + not unforced) */
	seen = (creature_ptr && ((mode & MD_ASSUME_VISIBLE) || (!(mode & MD_ASSUME_HIDDEN) && creature_ptr->ml)));

	/* Sexed Pronouns (seen and allowed, or unseen and allowed) */
	pron = (creature_ptr && ((seen && (mode & MD_PRON_VISIBLE)) || (!seen && (mode & MD_PRON_HIDDEN))));


	/* First, try using pronouns, or describing hidden monsters */
	if (!seen || pron)
	{
		/* an encoding of the monster "sex" */
		int kind = 0x00;

		/* Extract the gender (if applicable) */
		if (IS_FEMALE(creature_ptr)) kind = 0x20;
		else if (IS_MALE(creature_ptr)) kind = 0x10;

		/* Ignore the gender (if desired) */
		if (!creature_ptr || !pron) kind = 0x00;


		/* Assume simple result */
#ifdef JP
		res = "何か";
#else
		res = "it";
#endif


		/* Brute force: split on the possibilities */
		switch (kind + (mode & (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE)))
		{
			/* Neuter, or unknown */
#ifdef JP
			case 0x00:                                                    res = "何か"; break;
			case 0x00 + (MD_OBJECTIVE):                                   res = "何か"; break;
			case 0x00 + (MD_POSSESSIVE):                                  res = "何かの"; break;
			case 0x00 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "何か自身"; break;
			case 0x00 + (MD_INDEF_HIDDEN):                                res = "何か"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "何か"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "何か"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "それ自身"; break;
#else
			case 0x00:                                                    res = "it"; break;
			case 0x00 + (MD_OBJECTIVE):                                   res = "it"; break;
			case 0x00 + (MD_POSSESSIVE):                                  res = "its"; break;
			case 0x00 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "itself"; break;
			case 0x00 + (MD_INDEF_HIDDEN):                                res = "something"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "something"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "something's"; break;
			case 0x00 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "itself"; break;
#endif


			/* Male (assume human if vague) */
#ifdef JP
			case 0x10:                                                    res = "彼"; break;
			case 0x10 + (MD_OBJECTIVE):                                   res = "彼"; break;
			case 0x10 + (MD_POSSESSIVE):                                  res = "彼の"; break;
			case 0x10 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "彼自身"; break;
			case 0x10 + (MD_INDEF_HIDDEN):                                res = "誰か"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "誰か"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "誰かの"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "彼自身"; break;
#else
			case 0x10:                                                    res = "he"; break;
			case 0x10 + (MD_OBJECTIVE):                                   res = "him"; break;
			case 0x10 + (MD_POSSESSIVE):                                  res = "his"; break;
			case 0x10 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "himself"; break;
			case 0x10 + (MD_INDEF_HIDDEN):                                res = "someone"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "someone"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "someone's"; break;
			case 0x10 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "himself"; break;
#endif


			/* Female (assume human if vague) */
#ifdef JP
			case 0x20:                                                    res = "彼女"; break;
			case 0x20 + (MD_OBJECTIVE):                                   res = "彼女"; break;
			case 0x20 + (MD_POSSESSIVE):                                  res = "彼女の"; break;
			case 0x20 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "彼女自身"; break;
			case 0x20 + (MD_INDEF_HIDDEN):                                res = "誰か"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "誰か"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "誰かの"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "彼女自身"; break;
#else
			case 0x20:                                                    res = "she"; break;
			case 0x20 + (MD_OBJECTIVE):                                   res = "her"; break;
			case 0x20 + (MD_POSSESSIVE):                                  res = "her"; break;
			case 0x20 + (MD_POSSESSIVE | MD_OBJECTIVE):                   res = "herself"; break;
			case 0x20 + (MD_INDEF_HIDDEN):                                res = "someone"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_OBJECTIVE):                 res = "someone"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_POSSESSIVE):                res = "someone's"; break;
			case 0x20 + (MD_INDEF_HIDDEN | MD_POSSESSIVE | MD_OBJECTIVE): res = "herself"; break;
#endif
		}

		/* Copy the result */
		(void)strcpy(desc, res);
	}


	/* Handle visible monsters, "reflexive" request */
	else if ((mode & (MD_POSSESSIVE | MD_OBJECTIVE)) == (MD_POSSESSIVE | MD_OBJECTIVE))
	{
		/* The monster is visible, so use its gender */
#ifdef JP
		if (IS_FEMALE(creature_ptr)) strcpy(desc, "彼女自身");
		else if (IS_MALE(creature_ptr)) strcpy(desc, "彼自身");
		else strcpy(desc, "それ自身");
#else
		if (IS_FEMALE(creature_ptr)) strcpy(desc, "herself");
		else if (IS_MALE(creature_ptr)) strcpy(desc, "himself");
		else strcpy(desc, "itself");
#endif
	}


	/* Handle all other visible monster requests */
	else
	{
		/* Tanuki? */
		if (is_pet(playespecies_ptr, creature_ptr) && !is_original_ap(creature_ptr))
		{
#ifdef JP
			char *t;
			strcpy(buf, name);
			t = buf;
			while(strncmp(t, "』", 2) && *t) t++;
			if (*t)
			{
				*t = '\0';
				(void)sprintf(desc, "%s？』", buf);
			}
			else
				(void)sprintf(desc, "%s？", name);
#else
			(void)sprintf(desc, "%s?", name);
#endif
		}
		else

		/* It could be a Unique */
		if ((is_unique_species(species_ptr)) && !(player_ptr->image && !(mode & MD_IGNORE_HALLU)))
		{
			/* Start with the name (thus nominative and objective) */
			if ((creature_ptr->mflag2 & MFLAG2_CHAMELEON) && !(mode & MD_TRUE_NAME))
			{
#ifdef JP
				char *t;
				strcpy(buf, name);
				t = buf;
				while (strncmp(t, "』", 2) && *t) t++;
				if (*t)
				{
					*t = '\0';
					(void)sprintf(desc, "%s？』", buf);
				}
				else
					(void)sprintf(desc, "%s？", name);
#else
				(void)sprintf(desc, "%s?", name);
#endif
			}

			/* Inside monster arena, and it is not your mount */
			else if (inside_battle && !(player_ptr->riding && (&creature_list[player_ptr->riding] == creature_ptr)))
			{
				/* It is a fake unique monster */
#ifdef JP
				(void)sprintf(desc, "%sもどき", name);
#else
				(void)sprintf(desc, "fake %s", name);
#endif
			}

			else
			{
				if(!(mode & MD_IGNORE_EGO_DESC)) creature_desc_ego_pre(desc, creature_ptr, species_ptr);
				(void)strcat(desc, species_name + species_ptr->name);
				if(!(mode & MD_IGNORE_EGO_DESC)
					) creature_desc_ego_post(desc, creature_ptr, species_ptr);
			}
		}

		/* It could be an indefinite monster */
		else if (mode & MD_INDEF_VISIBLE)
		{
			/* XXX Check plurality for "some" */

			/* Indefinite monsters need an indefinite article */
#ifdef JP
			(void)strcpy(desc, "");
#else
			(void)strcpy(desc, is_a_vowel(name[0]) ? "an " : "a ");
#endif

			if(!(mode & MD_IGNORE_EGO_DESC)) creature_desc_ego_pre(desc, creature_ptr, species_ptr);
			(void)strcat(desc, species_name + species_ptr->name);
			if(!(mode & MD_IGNORE_EGO_DESC)) creature_desc_ego_post(desc, creature_ptr, species_ptr);
		}

		/* It could be a normal, definite, monster */
		else
		{
			/* Definite monsters need a definite article */
			if (is_pet(playespecies_ptr, creature_ptr))
#ifdef JP
				(void)strcpy(desc, "あなたの");
#else
				(void)strcpy(desc, "your ");
#endif

			else
#ifdef JP
				(void)strcpy(desc, "");
#else
				(void)strcpy(desc, "the ");
#endif

			if(!(mode & MD_IGNORE_EGO_DESC)) creature_desc_ego_pre(desc, creature_ptr, species_ptr);
			(void)strcat(desc, species_name + species_ptr->name);
			if(!(mode & MD_IGNORE_EGO_DESC)) creature_desc_ego_post(desc, creature_ptr, species_ptr);
		}



		if (creature_ptr->nickname)
		{
#ifdef JP
			sprintf(buf,"「%s」",quark_str(creature_ptr->nickname));
#else
			sprintf(buf," called %s",quark_str(creature_ptr->nickname));
#endif
			strcat(desc,buf);
		}

		if (player_ptr->riding && (&creature_list[player_ptr->riding] == creature_ptr))
		{
#ifdef JP
			strcat(desc,"(乗馬中)");
#else
			strcat(desc,"(riding)");
#endif
		}

		if ((mode & MD_IGNORE_HALLU) && (creature_ptr->mflag2 & MFLAG2_CHAMELEON))
		{
			if (is_unique_species(species_ptr))
			{
#ifdef JP
				strcat(desc,"(カメレオンの王)");
#else
				strcat(desc,"(Chameleon Lord)");
#endif
			}
			else
			{
#ifdef JP
				strcat(desc,"(カメレオン)");
#else
				strcat(desc,"(Chameleon)");
#endif
			}
		}

		if ((mode & MD_IGNORE_HALLU) && !is_original_ap(creature_ptr))
		{
			strcat(desc, format("(%s)", species_name + species_info[creature_ptr->species_idx].name));
		}

		/* Handle the Possessive as a special afterthought */
		if (mode & MD_POSSESSIVE)
		{
			/* XXX Check for trailing "s" */
			
			/* Simply append "apostrophe" and "s" */
#ifdef JP
			(void)strcat(desc, "の");
#else
			(void)strcat(desc, "'s");
#endif
		}
	}
}


void creature_desc_ego_pre(char* desc, creature_type *creature_ptr, species_type *species_ptr)
{
	if(species_ptr->chara_idx == INDEX_VARIABLE){
#ifdef JP
		(void)strcat(desc, chara_info[creature_ptr->chara_idx].title);
		if(chara_info[creature_ptr->chara_idx].no)
			(void)strcat(desc, "な");
#else
		(void)strcat(desc, chara_info[creature_ptr->chara_idx].title);
#endif
	}

	if(species_ptr->race_idx1 == INDEX_VARIABLE || species_ptr->race_idx2 == INDEX_VARIABLE){
		(void)strcat(desc, desc_race_name(creature_ptr));
#ifdef JP
		(void)strcat(desc, "の");
#else
		(void)strcat(desc, "'s ");
#endif
	}

	if(has_cf_creature(creature_ptr, CF_VARIABLE_SIZE)){
		char tmp[80];
		tmp[0] = '\0';
#ifdef JP
		sprintf(tmp, "サイズ%dの", creature_ptr->size);
#else
		sprintf(tmp, "Size:%d ", creature_ptr->size);
#endif
		(void)strcat(desc, tmp);
	}

	else if(creature_ptr->monster_ego_idx != MONEGO_NONE){
		(void)strcat(desc, re_name + re_info[creature_ptr->monster_ego_idx].name);
	}

}

void creature_desc_ego_post(char* desc, creature_type *creature_ptr, species_type *species_ptr)
{

	if(creature_ptr->cls_idx == INDEX_VARIABLE){
#ifdef JP
		(void)strcat(desc, "の");
		(void)strcat(desc, class_info[species_ptr->cls_idx].title);
#else
		(void)strcat(desc, class_info[species_ptr->cls_idx].title);
#endif
	}

}



/*
 * Learn about a monster (by "probing" it)
 *
 * Return the number of new flags learnt.  -Mogami-
 */
int lore_do_probe(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	int i, n = 0;
	byte tmp_byte;

	/* Maximal info about awareness */
	if (r_ptr->r_wake != MAX_UCHAR) n++;
	if (r_ptr->r_ignore != MAX_UCHAR) n++;
	r_ptr->r_wake = r_ptr->r_ignore = MAX_UCHAR;

	/* Observe "maximal" attacks */
	for (i = 0; i < 4; i++)
	{
		/* Examine "actual" blows */
		if (r_ptr->blow[i].effect || r_ptr->blow[i].method)
		{
			/* Maximal observations */
			if (r_ptr->r_blows[i] != MAX_UCHAR) n++;
			r_ptr->r_blows[i] = MAX_UCHAR;
		}
	}

	/* Maximal drops */
	tmp_byte = 0;

	/* Only "valid" drops */
	if (!is_only_gold_species(r_ptr))
	{
		if (r_ptr->r_drop_item != tmp_byte) n++;
		r_ptr->r_drop_item = tmp_byte;
	}
	if (!is_only_item_species(r_ptr))
	{
		if (r_ptr->r_drop_gold != tmp_byte) n++;
		r_ptr->r_drop_gold = tmp_byte;
	}

	/* Observe many spells */
	if (r_ptr->r_cast_spell != MAX_UCHAR) n++;
	r_ptr->r_cast_spell = MAX_UCHAR;

	/* Count unknown flags */
	/*
	for (i = 0; i < 32; i++)
	{
		if (!(r_ptr->r_flags1 & (1L << i)) &&
		    (r_ptr->flags1 & (1L << i))) n++;
		if (!(r_ptr->r_flags2 & (1L << i)) &&
		    (r_ptr->flags2 & (1L << i))) n++;
		if (!(r_ptr->r_flags3 & (1L << i)) &&
		    (r_ptr->flags3 & (1L << i))) n++;
		if (!(r_ptr->r_flags4 & (1L << i)) &&
		    (r_ptr->flags4 & (1L << i))) n++;
		if (!(r_ptr->r_flags5 & (1L << i)) &&
		    (r_ptr->flags5 & (1L << i))) n++;
		if (!(r_ptr->r_flags6 & (1L << i)) &&
		    (r_ptr->flags6 & (1L << i))) n++;
		if (!(r_ptr->r_flags10 & (1L << i)) &&
		    (r_ptr->flags10 & (1L << i))) n++;

#if 0
		if (!(r_ptr->r_flags7 & (1L << i)) &&
		    (r_ptr->flags7 & (1L << i))) n++;
#endif
	}
	*/


	/* Know all the flags */
/*
	r_ptr->r_flags1 = r_ptr->flags1;
	r_ptr->r_flags2 = r_ptr->flags2;
	r_ptr->r_flags3 = r_ptr->flags3;
	r_ptr->r_flags4 = r_ptr->flags4;
	r_ptr->r_flags5 = r_ptr->flags5;
	r_ptr->r_flags6 = r_ptr->flags6;
	r_ptr->r_flags10 = r_ptr->flags10;
*/

	/* r_flags7 is actually unused */
	/* r_ptr->r_flags7 = r_ptr->flags7; */

	/* Know about evolution */
	if (!(r_ptr->r_xtra1 & MR1_SINKA)) n++;
	r_ptr->r_xtra1 |= MR1_SINKA;

	/* Update monster recall window */
	if (species_window_idx == species_idx)
	{
		/* Window stuff */
		play_window |= (PW_MONSTER);
	}

	/* Return the number of new flags learnt */
	return n;
}


/*
 * Take note that the given monster just dropped some treasure
 *
 * Note that learning the "GOOD"/"GREAT" flags gives information
 * about the treasure (even when the monster is killed for the first
 * time, such as uniques, and the treasure has not been examined yet).
 *
 * This "indirect" method is used to prevent the player from learning
 * exactly how much treasure a monster can drop from observing only
 * a single example of a drop.  This method actually observes how much
 * gold and items are dropped, and remembers that information to be
 * described later by the monster recall code.
 */
void lore_treasure(creature_type *cr_ptr, int num_item, int num_gold)
{
	species_type *r_ptr = &species_info[cr_ptr->species_idx];

	/* If the monster doesn't have original appearance, don't note */
	if (!is_original_ap(cr_ptr)) return;

	/* Note the number of things dropped */
	if (num_item > r_ptr->r_drop_item) r_ptr->r_drop_item = num_item;
	if (num_gold > r_ptr->r_drop_gold) r_ptr->r_drop_gold = num_gold;

	/* Update monster recall window */
	if (species_window_idx == cr_ptr->species_idx)
	{
		/* Window stuff */
		play_window |= (PW_MONSTER);
	}
}



void sanity_blast(creature_type *watcher_ptr, creature_type *m_ptr, bool necro)
{
	bool happened = FALSE;
	int power = 100;

	if (inside_battle || !character_dungeon) return;

	if (!necro)
	{
		char            m_name[80];
		species_type    *r_ptr = &species_info[m_ptr->ap_species_idx];

		power = r_ptr->level / 2;

		creature_desc(m_name, m_ptr, 0);

		if (!has_cf_creature(m_ptr, CF_UNIQUE))
		{
			if (has_cf_creature(m_ptr, CF_FRIENDS))
			power /= 2;
		}
		else power *= 2;

		if (!hack_mind)
			return; /* No effect yet, just loaded... */

		if (!m_ptr->ml)
			return; /* Cannot see it for some reason */

		if (!has_cf_creature(m_ptr, CF_ELDRITCH_HORROR))
			return; /* oops */



		if (is_pet(player_ptr, m_ptr))
			return; /* Pet eldritch horrors are safe most of the time */

		if (randint1(100) > power) return;

		if (saving_throw(watcher_ptr->skill_rob - power))
		{
			return; /* Save, no adverse effects */
		}

		if (watcher_ptr->image)
		{
			/* Something silly happens... */
#ifdef JP
			msg_format("%s%sの顔を見てしまった！",
				funny_desc[randint0(MAX_SAN_FUNNY)], m_name);
#else
			msg_format("You behold the %s visage of %s!",
				funny_desc[randint0(MAX_SAN_FUNNY)], m_name);
#endif


			if (one_in_(3))
			{
				msg_print(funny_comments[randint0(MAX_SAN_COMMENT)]);
				watcher_ptr->image = watcher_ptr->image + (s16b)randint1(r_ptr->level);
			}

			return; /* Never mind; we can't see it clearly enough */
		}

		/* Something frightening happens... */

		if (is_demon_creature(watcher_ptr) || (mimic_info[watcher_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON))
		{
#ifdef JP
			msg_format("%s%sの顔が垣間見えた。",
				delight_desc[randint0(MAX_SAN_DELIGHT)], m_name);
#else
			msg_format("You glance at the %s visage of %s.",
				delight_desc[randint0(MAX_SAN_DELIGHT)], m_name);
#endif
		}
		else
		{
#ifdef JP
			msg_format("%s%sの顔を見てしまった！",
				horror_desc[randint0(MAX_SAN_HORROR)], m_name);
#else
			msg_format("You behold the %s visage of %s!",
				horror_desc[randint0(MAX_SAN_HORROR)], m_name);
#endif
		}

		reveal_creature_info(m_ptr, CF_ELDRITCH_HORROR);

		/* Demon characters are unaffected */
		if (is_demon_creature(watcher_ptr) || (mimic_info[watcher_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON)) return;

		if (wizard) return;

		/* Undead characters are 50% likely to be unaffected */
		if (is_undead_creature(watcher_ptr) || (mimic_info[watcher_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_UNDEAD))
		{
			if (saving_throw(25 + watcher_ptr->lev)) return;
		}
	}
	else
	{
#ifdef JP
msg_print("ネクロノミコンを読んで正気を失った！");
#else
		msg_print("Your sanity is shaken by reading the Necronomicon!");
#endif

	}

	if (!saving_throw(watcher_ptr->skill_rob - power)) /* Mind blast */
	{
		if (!watcher_ptr->resist_conf)
		{
			(void)set_confused(watcher_ptr, watcher_ptr->confused + randint0(4) + 4);
		}
		if (!watcher_ptr->resist_chaos && one_in_(3))
		{
			(void)set_image(watcher_ptr, watcher_ptr->image + randint0(250) + 150);
		}
		return;
	}

	if (!saving_throw(watcher_ptr->skill_rob - power)) /* Lose int & wis */
	{
		do_dec_stat(watcher_ptr, STAT_INT);
		do_dec_stat(watcher_ptr, STAT_WIS);
		return;
	}

	if (!saving_throw(watcher_ptr->skill_rob - power)) /* Brain smash */
	{
		if (!watcher_ptr->resist_conf)
		{
			(void)set_confused(watcher_ptr, watcher_ptr->confused + randint0(4) + 4);
		}
		if (!watcher_ptr->free_act)
		{
			(void)set_paralyzed(watcher_ptr, watcher_ptr->paralyzed + randint0(4) + 4);
		}
		while (randint0(100) > watcher_ptr->skill_rob)
			(void)do_dec_stat(watcher_ptr, STAT_INT);
		while (randint0(100) > watcher_ptr->skill_rob)
			(void)do_dec_stat(watcher_ptr, STAT_WIS);
		if (!watcher_ptr->resist_chaos)
		{
			(void)set_image(watcher_ptr, watcher_ptr->image + randint0(250) + 150);
		}
		return;
	}

	if (!saving_throw(watcher_ptr->skill_rob - power)) /* Amnesia */
	{

		if (lose_all_info(watcher_ptr))
#ifdef JP
msg_print("あまりの恐怖に全てのことを忘れてしまった！");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		return;
	}

	if (saving_throw(watcher_ptr->skill_rob - power))
	{
		return;
	}

	/* Else gain permanent insanity */
	if (has_cf_creature(watcher_ptr, CF_MORONIC) &&
		(has_cf_creature(watcher_ptr, CF_COWARDICE) || (watcher_ptr->resist_fear)) &&
		(has_cf_creature(watcher_ptr, CF_HALLU) || (watcher_ptr->resist_chaos)))
	{
		/* The poor bastard already has all possible insanities! */
		return;
	}

	while (!happened)
	{
		switch (randint1(21))
		{
			case 1:
				if (!has_cf_creature(watcher_ptr, CF_MORONIC) && one_in_(5))
				{
					if ((watcher_ptr->stat_use[STAT_INT] < 4) && (watcher_ptr->stat_use[STAT_WIS] < 4))
					{
#ifdef JP
msg_print("あなたは完璧な馬鹿になったような気がした。しかしそれは元々だった。");
#else
						msg_print("You turn into an utter moron!");
#endif
					}
					else
					{
#ifdef JP
msg_print("あなたは完璧な馬鹿になった！");
#else
						msg_print("You turn into an utter moron!");
#endif
					}

					if (has_cf_creature(watcher_ptr, CF_HYPER_INT))
					{
#ifdef JP
msg_print("あなたの脳は生体コンピュータではなくなった。");
#else
						msg_print("Your brain is no longer a living computer.");
#endif

						//TODO watcher_ptr->flags14 &= ~(RF14_HYPER_INT);
					}
					//TODO watcher_ptr->flags14 |= RF14_MORONIC;
					happened = TRUE;
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				if (!has_cf_creature(watcher_ptr, CF_COWARDICE) && !watcher_ptr->resist_fear)
				{
#ifdef JP
msg_print("あなたはパラノイアになった！");
#else
					msg_print("You become paranoid!");
#endif


					/* Duh, the following should never happen, but anyway... */
					if (has_cf_creature(watcher_ptr, CF_FEARLESS))
					{
#ifdef JP
msg_print("あなたはもう恐れ知らずではなくなった。");
#else
						msg_print("You are no longer fearless.");
#endif

						//TODO watcher_ptr->flags14 &= ~(RF14_FEARLESS);
					}

					//TODO watcher_ptr->flags13 |= RF13_COWARDICE;
					happened = TRUE;
				}
				break;
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
				if (!has_cf_creature(watcher_ptr, CF_HALLU) && !watcher_ptr->resist_chaos)
				{
#ifdef JP
msg_print("幻覚をひき起こす精神錯乱に陥った！");
#else
					msg_print("You are afflicted by a hallucinatory insanity!");
#endif

					//TODO watcher_ptr->flags13 |= RF13_HALLU;
					happened = TRUE;
				}
				break;
			default:
				if (!has_cf_creature(watcher_ptr, CF_BERS_RAGE))
				{
#ifdef JP
msg_print("激烈な感情の発作におそわれるようになった！");
#else
					msg_print("You become subject to fits of berserk rage!");
#endif

					//TODO watcher_ptr->flags13 |= RF13_BERS_RAGE;
					happened = TRUE;
				}
				break;
		}
	}

	update |= CRU_BONUS;
	handle_stuff();
}


/*
 * This function updates the monster record of the given monster
 *
 * This involves extracting the distance to the player (if requested),
 * and then checking for visibility (natural, infravision, see-invis,
 * telepathy), updating the monster visibility flag, redrawing (or
 * erasing) the monster when its visibility changes, and taking note
 * of any interesting monster flags (cold-blooded, invisible, etc).
 *
 * Note the new "mflag" field which encodes several monster state flags,
 * including "view" for when the monster is currently in line of sight,
 * and "mark" for when the monster is currently visible via detection.
 *
 * The only monster fields that are changed here are "cdis" (the
 * distance from the player), "ml" (visible to the player), and
 * "mflag" (to maintain the "MFLAG_VIEW" flag).
 *
 * Note the special "update_monsters()" function which can be used to
 * call this function once for every monster.
 *
 * Note the "full" flag which requests that the "cdis" field be updated,
 * this is only needed when the monster (or the player) has moved.
 *
 * Every time a monster moves, we must call this function for that
 * monster, and update the distance, and the visibility.  Every time
 * the player moves, we must call this function for every monster, and
 * update the distance, and the visibility.  Whenever the player "state"
 * changes in certain ways ("blindness", "infravision", "telepathy",
 * and "see invisible"), we must call this function for every monster,
 * and update the visibility.
 *
 * Routines that change the "illumination" of a grid must also call this
 * function for any monster in that grid, since the "visibility" of some
 * monsters may be based on the illumination of their grid.
 *
 * Note that this function is called once per monster every time the
 * player moves.  When the player is running, this function is one
 * of the primary bottlenecks, along with "update_view()" and the
 * "process_monsters()" code, so efficiency is important.
 *
 * Note the optimized "inline" version of the "distance()" function.
 *
 * A monster is "visible" to the player if (1) it has been detected
 * by the player, (2) it is close to the player and the player has
 * telepathy, or (3) it is close to the player, and in line of sight
 * of the player, and it is "illuminated" by some combination of
 * infravision, torch light, or permanent light (invisible monsters
 * are only affected by "light" if the player can see invisible).
 *
 * Monsters which are not on the current panel may be "visible" to
 * the player, and their descriptions will include an "offscreen"
 * reference.  Currently, offscreen monsters cannot be targetted
 * or viewed directly, but old targets will remain set.  XXX XXX
 *
 * The player can choose to be disturbed by several things, including
 * "disturb_move" (monster which is viewable moves in some way), and
 * "disturb_near" (monster which is "easily" viewable moves in some
 * way).  Note that "moves" includes "appears" and "disappears".
 */
//TODO  Marge to calc_bonuses
void update_mon(creature_type *cr_ptr, int m_idx, bool full)
{
	creature_type *m_ptr = &creature_list[m_idx];

	species_type *r_ptr = &species_info[m_ptr->species_idx];

	bool do_disturb = disturb_move;

	int d;

	/* Current location */
	int fy = m_ptr->fy;
	int fx = m_ptr->fx;

	/* Seen at all */
	bool flag = FALSE;

	/* Seen by vision */
	bool easy = FALSE;

	/* Non-Ninja player in the darkness */
	bool in_darkness = (d_info[dungeon_type].flags1 & DF1_DARKNESS) && !cr_ptr->see_nocto;

	/* Do disturb? */
	if (disturb_high)
	{
		species_type *ap_r_ptr = &species_info[m_ptr->ap_species_idx];

		if (ap_r_ptr->r_tkills && ap_r_ptr->level >= cr_ptr->lev)
			do_disturb = TRUE;
	}

	/* Compute distance */
	if (full)
	{
		/* Distance components */
		int dy = (cr_ptr->fy > fy) ? (cr_ptr->fy - fy) : (fy - cr_ptr->fy);
		int dx = (cr_ptr->fx > fx) ? (cr_ptr->fx - fx) : (fx - cr_ptr->fx);

		/* Approximate distance */
		d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

		/* Restrict distance */
		if (d > 255) d = 255;

		if (!d) d = 1;

		/* Save the distance */
		m_ptr->cdis = d;
	}

	/* Extract distance */
	else
	{
		/* Extract the distance */
		d = m_ptr->cdis;
	}


	/* Detected */
	if (m_ptr->mflag2 & (MFLAG2_MARK)) flag = TRUE;


	/* Nearby */
	if (d <= (in_darkness ? MAX_SIGHT / 2 : MAX_SIGHT))
	{
		if (!in_darkness || (d <= MAX_SIGHT / 4))
		{
			if (cr_ptr->special_defense & KATA_MUSOU)
			{
				/* Detectable */
				flag = TRUE;

				if (is_original_ap(m_ptr) && !cr_ptr->image)
				{
					if (has_cf_creature(m_ptr, CF_SMART)) reveal_creature_info(m_ptr, CF_SMART);;
					if (has_cf_creature(m_ptr, CF_SMART)) reveal_creature_info(m_ptr, CF_STUPID);;
				}
			}

			/* Basic telepathy */
			/* Snipers get telepathy when they concentrate deeper */
			else if (cr_ptr->telepathy)
			{
				/* Empty mind, no telepathy */
				if (has_cf_creature(m_ptr, CF_EMPTY_MIND))
				{
					/* Memorize flags */
					if (is_original_ap_and_seen(player_ptr, m_ptr)) reveal_creature_info(m_ptr, CF_EMPTY_MIND);
				}

				/* Weird mind, occasional telepathy */
				else if (has_cf_creature(m_ptr, CF_WEIRD_MIND))
				{
					/* One in ten individuals are detectable */
					if ((m_idx % 10) == 5)
					{
						/* Detectable */
						flag = TRUE;

						if (is_original_ap(m_ptr) && !cr_ptr->image)
						{
							/* Memorize flags */
							reveal_creature_info(m_ptr, CF_WEIRD_MIND);
							reveal_creature_info(m_ptr, CF_SMART);
							reveal_creature_info(m_ptr, CF_STUPID);
						}
					}
				}

				/* Normal mind, allow telepathy */
				else
				{
					/* Detectable */
					flag = TRUE;

					if (is_original_ap(m_ptr) && !cr_ptr->image)
					{
						/* Hack -- Memorize mental flags */
						reveal_creature_info(m_ptr, CF_SMART);
						reveal_creature_info(m_ptr, CF_STUPID);
					}
				}
			}

			/* Magical sensing */
			if ((cr_ptr->esp_animal) && is_animal_creature(cr_ptr))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_undead) && is_undead_species(r_ptr))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_demon) && is_demon_species(r_ptr))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_orc) && (is_orc_creature(&creature_list[m_idx])))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_troll) && (is_troll_creature(&creature_list[m_idx])))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_giant) && is_giant_species(r_ptr))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_dragon) && is_dragon_species(r_ptr))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_human) && (is_human_species(r_ptr)))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_evil) && is_enemy_of_good_creature(cr_ptr))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_ALIGNMENT);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_good) && is_enemy_of_evil_creature(m_ptr))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, INFO_TYPE_ALIGNMENT);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_nonliving) && has_cf_creature(m_ptr, CF_NONLIVING) && !is_undead_creature(m_ptr)) 
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, CF_NONLIVING);
			}

			/* Magical sensing */
			if ((cr_ptr->esp_unique) && (is_unique_species(r_ptr)))
			{
				flag = TRUE;
				reveal_creature_info(m_ptr, CF_UNIQUE);
			}
		}

		/* Normal line of sight, and not blind */
		if (player_has_los_bold(fy, fx) && !cr_ptr->blind)
		{
			bool do_invisible = FALSE;
			bool do_cold_blood = FALSE;

			/* Snipers can see targets in darkness when they concentrate deeper */
			if (cr_ptr->concent >= CONCENT_RADAR_THRESHOLD)
			{
				/* Easy to see */
				easy = flag = TRUE;
			}

			/* Use "infravision" */
			if (d <= cr_ptr->see_infra)
			{
				/* Handle "cold blooded" monsters */
				if (has_cf_creature(m_ptr, CF_COLD_BLOOD) || !has_cf_creature(m_ptr, CF_AURA_FIRE))
				{
					/* Take note */
					do_cold_blood = TRUE;
				}

				/* Handle "warm blooded" monsters */
				else
				{
					/* Easy to see */
					easy = flag = TRUE;
				}
			}

			/* Use "illumination" */
			if (player_can_see_bold(cr_ptr, fy, fx))
			{
				/* Handle "invisible" monsters */
				if (has_cf_creature(m_ptr, CF_INVISIBLE))
				{
					/* Take note */
					do_invisible = TRUE;

					/* See invisible */
					if (cr_ptr->see_inv)
					{
						/* Easy to see */
						easy = flag = TRUE;
					}
				}

				/* Handle "normal" monsters */
				else
				{
					/* Easy to see */
					easy = flag = TRUE;
				}
			}

			/* Visible */
			if (flag)
			{
				if (is_original_ap(m_ptr) && !cr_ptr->image)
				{
					/* Memorize flags */
					reveal_creature_info(m_ptr, CF_INVISIBLE);
					reveal_creature_info(m_ptr, CF_COLD_BLOOD);
				}
			}
		}
	}


	/* The monster is now visible */
	if (flag)
	{
		/* It was previously unseen */
		if (!m_ptr->ml)
		{
			/* Mark as visible */
			m_ptr->ml = TRUE;

			/* Draw the monster */
			lite_spot(fy, fx);

			/* Update health bar as needed */
			if (health_who == m_idx) play_redraw |= (PR_HEALTH);
			if (cr_ptr->riding == m_idx) play_redraw |= (PR_UHEALTH);

			/* Hack -- Count "fresh" sightings */
			if (!cr_ptr->image)
			{
				if ((m_ptr->ap_species_idx == MON_KAGE) && (species_info[MON_KAGE].r_sights < MAX_SHORT))
					species_info[MON_KAGE].r_sights++;
				else if (is_original_ap(m_ptr) && (r_ptr->r_sights < MAX_SHORT))
					r_ptr->r_sights++;
			}

			/* Eldritch Horror */
			if (is_eldritch_horror_species(&species_info[m_ptr->ap_species_idx]))
			{
				sanity_blast(cr_ptr, m_ptr, FALSE);
			}

			/* Disturb on appearance */
			if (disturb_near && (projectable(cr_ptr->fy, cr_ptr->fx, m_ptr->fy, m_ptr->fx)))
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(player_ptr, 1, 0);
			}
		}
	}

	/* The monster is not visible */
	else
	{
		/* It was previously seen */
		if (m_ptr->ml)
		{
			/* Mark as not visible */
			m_ptr->ml = FALSE;

			/* Erase the monster */
			lite_spot(fy, fx);

			/* Update health bar as needed */
			if (health_who == m_idx) play_redraw |= (PR_HEALTH);
			if (cr_ptr->riding == m_idx) play_redraw |= (PR_UHEALTH);

			/* Disturb on disappearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(player_ptr, 1, 0);
			}
		}
	}


	/* The monster is now easily visible */
	if (easy)
	{
		/* Change */
		if (!(m_ptr->mflag & (MFLAG_VIEW)))
		{
			/* Mark as easily visible */
			m_ptr->mflag |= (MFLAG_VIEW);

			/* Disturb on appearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(player_ptr, 1, 0);
			}
		}
	}

	/* The monster is not easily visible */
	else
	{
		/* Change */
		if (m_ptr->mflag & (MFLAG_VIEW))

		{
			/* Mark as not easily visible */
			m_ptr->mflag &= ~(MFLAG_VIEW);

			/* Disturb on disappearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(player_ptr, 1, 0);
			}
		}
	}
}


/*
 * This function simply updates all the (non-dead) monsters (see above).
 */
void update_monsters(bool full)
{
	int i;

	/* Update each (live) monster */
	for (i = 1; i < m_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];

		/* Skip dead monsters */
		if (!m_ptr->species_idx) continue;

		/* Update the monster */
		update_mon(p_ptr, i, full);
	}
}


/*
 * Hack -- the index of the summoning monster
 */
static bool creature_hook_chameleon_lord(creature_type *player_ptr, int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	creature_type *m_ptr = &creature_list[chameleon_change_m_idx];
	species_type *old_r_ptr = &species_info[m_ptr->species_idx];

	if (!(is_unique_species(r_ptr))) return FALSE;
	if (is_friendly_species(r_ptr) || is_chameleon_species(r_ptr)) return FALSE;

	if (ABS(r_ptr->level - species_info[MON_CHAMELEON_K].level) > 5) return FALSE;

	if ((r_ptr->blow[0].method == RBM_EXPLODE) ||
		(r_ptr->blow[1].method == RBM_EXPLODE) ||
		(r_ptr->blow[2].method == RBM_EXPLODE) ||
		(r_ptr->blow[3].method == RBM_EXPLODE))
		return FALSE;

	if (!monster_can_cross_terrain(cave[m_ptr->fy][m_ptr->fx].feat, r_ptr, 0)) return FALSE;

	/* Not born */
	if (!is_chameleon_species(old_r_ptr))
	{
		if (creature_has_hostile_align(m_ptr, player_ptr, 0, 0, r_ptr)) return FALSE;
	}

	/* Born now */
	else if (summon_specific_who > 0)
	{
		if (creature_has_hostile_align(&creature_list[summon_specific_who], player_ptr, 0, 0, r_ptr)) return FALSE;
	}

	return TRUE;
}

static bool creature_hook_chameleon(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];
	creature_type *m_ptr = &creature_list[chameleon_change_m_idx];
	species_type *old_r_ptr = &species_info[m_ptr->species_idx];

	if (is_unique_species(r_ptr)) return FALSE;
	if (is_multiply_species(r_ptr)) return FALSE;
	if (is_friendly_species(r_ptr) || is_chameleon_species(r_ptr)) return FALSE;
	
	if ((r_ptr->blow[0].method == RBM_EXPLODE) || (r_ptr->blow[1].method == RBM_EXPLODE) || (r_ptr->blow[2].method == RBM_EXPLODE) || (r_ptr->blow[3].method == RBM_EXPLODE))
		return FALSE;

	if (!monster_can_cross_terrain(cave[m_ptr->fy][m_ptr->fx].feat, r_ptr, 0)) return FALSE;

	/* Not born */
	if (!is_chameleon_species(old_r_ptr))
	{
		if (is_enemy_of_evil_species(old_r_ptr) && !is_enemy_of_evil_species(r_ptr)) return FALSE;
		if (is_enemy_of_good_species(old_r_ptr) && !is_enemy_of_good_species(r_ptr)) return FALSE;
		if (!(is_enemy_of_evil_species(old_r_ptr) && is_enemy_of_good_species(old_r_ptr)) && (is_enemy_of_evil_species(r_ptr) && is_enemy_of_good_species(r_ptr))) return FALSE;
	}

	/* Born now */
	else if (summon_specific_who > 0)
	{
		if (creature_has_hostile_align(&creature_list[summon_specific_who], p_ptr, 0, 0, r_ptr)) return FALSE;
	}

	return (*(get_creature_hook()))(species_idx);
}


void choose_new_monster(int m_idx, bool born, int species_idx, int monster_ego_idx)
{
	int oldmhp;
	creature_type *m_ptr = &creature_list[m_idx];
	species_type *r_ptr;
	char old_m_name[80];
	bool old_unique = FALSE;
	int old_species_idx = m_ptr->species_idx;

	if (is_unique_species(&species_info[m_ptr->species_idx]))
		old_unique = TRUE;
	if (old_unique && (species_idx == MON_CHAMELEON)) species_idx = MON_CHAMELEON_K;
	r_ptr = &species_info[species_idx];

	creature_desc(old_m_name, m_ptr, 0);

	if (!species_idx)
	{
		int level;

		chameleon_change_m_idx = m_idx;
		if (old_unique)
			get_mon_num_prep2(p_ptr, creature_hook_chameleon_lord, NULL);
		else
			get_mon_num_prep(creature_hook_chameleon, NULL);

		if (old_unique)
			level = species_info[MON_CHAMELEON_K].level;
		else if (!dun_level)
			level = wilderness[wilderness_y][wilderness_x].level;
		else
			level = dun_level;

		if (d_info[dungeon_type].flags1 & DF1_CHAMELEON) level+= 2+randint1(3);

		species_idx = get_mon_num(level);
		r_ptr = &species_info[species_idx];

		chameleon_change_m_idx = 0;

		/* Paranoia */
		if (!species_idx) return;
	}

	if (is_pet(player_ptr, m_ptr)) check_pets_num_and_align(m_ptr, FALSE);

	m_ptr->species_idx = species_idx;
	m_ptr->ap_species_idx = species_idx;
	update_mon(p_ptr, m_idx, FALSE);
	lite_spot(m_ptr->fy, m_ptr->fx);

	if(monster_ego_idx == MONEGO_NONE)
	{
		m_ptr->monster_ego_idx = 0;
	}
	else
	{
		m_ptr->monster_ego_idx = monster_ego_idx;
	}


	if (is_lighting_species(&species_info[old_species_idx]) || is_darken_species(&species_info[old_species_idx]) ||
	    (is_lighting_species(r_ptr) || is_darken_species(r_ptr)))
		update |= (PU_MON_LITE);

	if (is_pet(player_ptr, m_ptr)) check_pets_num_and_align(m_ptr, TRUE);

	if (born)
	{
		/* Sub-alignment of a chameleon */
		if (is_enemy_of_evil_species(r_ptr) && is_enemy_of_good_species(r_ptr))
		{
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
			if (is_enemy_of_good_species(r_ptr)) m_ptr->sub_align |= SUB_ALIGN_EVIL;
			if (is_enemy_of_evil_species(r_ptr)) m_ptr->sub_align |= SUB_ALIGN_GOOD;
		}
		return;
	}

	if (m_idx == p_ptr->riding)
	{
		char m_name[80];
		creature_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("突然%sが変身した。", old_m_name);
#else
		msg_format("Suddenly, %s transforms!", old_m_name);
#endif
		if (!has_cf_creature(m_ptr, CF_RIDING))
#ifdef JP
			if (rakuba(p_ptr, 0, TRUE)) msg_print("地面に落とされた。");
#else
			if (rakuba(p_ptr, 0, TRUE)) msg_format("You have fallen from %s.", m_name);
#endif
	}

	oldmhp = m_ptr->mmhp;

	m_ptr->mhp = (long)(m_ptr->mhp * m_ptr->mmhp) / oldmhp;
	if (m_ptr->mhp < 1) m_ptr->mhp = 1;
	m_ptr->chp = (long)(m_ptr->chp * m_ptr->mmhp) / oldmhp;

	m_ptr->stat_use[0] = r_ptr->stat_max[0];
	m_ptr->stat_use[1] = r_ptr->stat_max[1];
	m_ptr->stat_use[2] = r_ptr->stat_max[2];
	m_ptr->stat_use[3] = r_ptr->stat_max[3];
	m_ptr->stat_use[4] = r_ptr->stat_max[4];
	m_ptr->stat_use[5] = r_ptr->stat_max[5];

	if(m_ptr->monster_ego_idx != MONEGO_NONE){
		m_ptr->stat_use[0] += re_info[monster_ego_idx].stat[0];
		m_ptr->stat_use[1] += re_info[monster_ego_idx].stat[1];
		m_ptr->stat_use[2] += re_info[monster_ego_idx].stat[2];
		m_ptr->stat_use[3] += re_info[monster_ego_idx].stat[3];
		m_ptr->stat_use[4] += re_info[monster_ego_idx].stat[4];
		m_ptr->stat_use[5] += re_info[monster_ego_idx].stat[5];
	}

	//TODO Reset Status
}


/*
 *  Hook for Tanuki
 */
static bool creature_hook_tanuki(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	if (is_unique_species(r_ptr)) return FALSE;
	if (is_multiply_species(r_ptr)) return FALSE;
	if (is_friendly_species(r_ptr) || is_chameleon_species(r_ptr)) return FALSE;
	if (is_aquatic_species(r_ptr)) return FALSE;
	
	if ((r_ptr->blow[0].method == RBM_EXPLODE) || (r_ptr->blow[1].method == RBM_EXPLODE) || (r_ptr->blow[2].method == RBM_EXPLODE) || (r_ptr->blow[3].method == RBM_EXPLODE))
		return FALSE;

	return (*(get_creature_hook()))(species_idx);
}


/*
 *  Set initial racial appearance of a monster
 */
static int initial_r_appearance(int species_idx)
{
	int attempts = 1000;

	int ap_species_idx;
	int min = MIN(base_level-5, 50);

	if (is_tanuki_species(&species_info[species_idx]))
		return species_idx;

	get_mon_num_prep(creature_hook_tanuki, NULL);

	while (--attempts)
	{
		ap_species_idx = get_mon_num(base_level + 10);
		if (species_info[ap_species_idx].level >= min) return ap_species_idx;
	}

	return species_idx;
}

static void deal_magic_book_aux(creature_type *creature_ptr, int realm)
{
	int min, max, tv;
	object_type forge;
	object_type *q_ptr;

	q_ptr = &forge;
	tv = TV_LIFE_BOOK + realm - 1;

	// First Book
	min = (creature_ptr->lev > 15 ? 2 : 1);
	max = (min + creature_ptr->lev / 10);
	object_prep(q_ptr, lookup_kind(tv, 0), ITEM_FREE_SIZE);
	q_ptr->number = (byte)rand_range(min, max);
	add_outfit(creature_ptr, q_ptr, 0);

	// Second Book
	if(creature_ptr->lev > 10 || one_in_(50))
	{
		min = (creature_ptr->lev > 22 ? 2 : 1);
		max = (min + creature_ptr->lev / 17);
		object_prep(q_ptr, lookup_kind(tv, 1), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(min, max);
		add_outfit(creature_ptr, q_ptr, 0);
	}

	// Third Book
	if(creature_ptr->lev > 32 || one_in_(300))
	{
		min = (creature_ptr->lev > 41 ? 2 : 1);
		max = (min + creature_ptr->lev / 37);
		object_prep(q_ptr, lookup_kind(tv, 2), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(min, max);
		add_outfit(creature_ptr, q_ptr, 0);
	}

	// Fourth Book
	if(creature_ptr->lev > 44 || one_in_(1000))
	{
		min = (creature_ptr->lev > 48 ? 2 : 1);
		max = (min + creature_ptr->lev / 50);
		object_prep(q_ptr, lookup_kind(tv, 3), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(min, max);
		add_outfit(creature_ptr, q_ptr, 0);
	}

}

static void deal_magic_book(creature_type *creature_ptr)
{
	if(creature_ptr->realm1 != REALM_NONE) deal_magic_book_aux(creature_ptr, creature_ptr->realm1);
	if(creature_ptr->realm2 != REALM_NONE) deal_magic_book_aux(creature_ptr, creature_ptr->realm2);
}

static void deal_potion(creature_type *creature_ptr)
{
	int min, max;
	object_type forge;
	object_type *q_ptr;
	q_ptr = &forge;

	if((creature_ptr->lev >= 3 || one_in_(5)) && creature_ptr->lev <= 12)
	{
		min = (1 + creature_ptr->lev / 10);
		max = (min + creature_ptr->lev * 2 / 3);
		object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_CURE_LIGHT), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(min, max);
		add_outfit(creature_ptr, q_ptr, 0);
	}

}

static void deal_food(creature_type *creature_ptr)
{
	object_type *q_ptr;
	object_type forge;
	int i;
	q_ptr = &forge;

	if(has_cf_creature(creature_ptr, CF_FOOD_EATER))
	{
		/* Food rations */
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(3, 7);

		add_outfit(creature_ptr, q_ptr, 0);
	}

	else if(has_cf_creature(creature_ptr, CF_CORPSE_EATER))
	{
		/* Prepare allocation table */
		get_mon_num_prep(creature_hook_human, NULL);

		for (i = rand_range(3,4); i > 0; i--)
		{
			object_prep(q_ptr, lookup_kind(TV_CORPSE, SV_CORPSE), ITEM_FREE_SIZE);
			q_ptr->pval = get_mon_num(2);
			q_ptr->number = 1;
			add_outfit(creature_ptr, q_ptr, 0);
		}
	}
	else if(has_cf_creature(creature_ptr, CF_WATER_DRINKER))
	{
		/* Potions of Water */
		object_prep(q_ptr, lookup_kind(TV_POTION, SV_POTION_WATER), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 23);
		add_outfit(creature_ptr, q_ptr, 0);
	}
	else if(has_cf_creature(creature_ptr, CF_FLASK_DRINKER))
	{
		/* Flasks of oil */
		object_prep(q_ptr, lookup_kind(TV_FLASK, SV_ANY), ITEM_FREE_SIZE);

		/* Fuel with oil (move pval to xtra4) */
		apply_magic(creature_ptr, q_ptr, 1, AM_NO_FIXED_ART, 0);
		q_ptr->number = (byte)rand_range(7, 12);
		add_outfit(creature_ptr, q_ptr, 0);
	}

}

static void deal_lite(creature_type *creature_ptr)
{
	object_type *q_ptr;
	object_type forge;
	q_ptr = &forge;

	if (has_cf_creature(creature_ptr, CF_HUMANOID))
	{
		if (has_cf_creature(creature_ptr, CF_VAMPIRE) && (creature_ptr->cls_idx != CLASS_NINJA))
		{
			// Hack -- Give the player scrolls of DARKNESS!
			object_prep(q_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_DARKNESS), ITEM_FREE_SIZE);
			q_ptr->number = (byte)rand_range(2, 5);
			add_outfit(creature_ptr, q_ptr, 0);
		}

		else if (creature_ptr->cls_idx != CLASS_NINJA)
		{
			// Hack -- Give the player some torches
			object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH), ITEM_FREE_SIZE);
			q_ptr->number = (byte)rand_range(3, 7);
			q_ptr->xtra4 = (s16b)rand_range(7, 10) * 500;
			add_outfit(creature_ptr, q_ptr, 0);
		}
	}
}

static u32b calc_deal_item_rank(creature_type *creature_ptr, object_type *object_ptr)
{
	u32b ret = AM_UNCURSED;
	int prob = randint0(100);
	int dealing_value = creature_ptr->lev * 10 + creature_ptr->sc * 5 + creature_ptr->dr * 30;
	dealing_value = dealing_value < 0 ? 0 : dealing_value;
	if(object_ptr->tval == TV_RING) dealing_value /= 3;
	if(object_ptr->tval == TV_AMULET) dealing_value /= 3;
	if((dealing_value - 100) / 2 > prob) ret |= AM_GOOD;    // 100-300
	if((dealing_value - 200) / 3 > prob) ret |= AM_GREAT;   // 200-500
	if((dealing_value - 400) / 4 > prob) ret |= AM_SPECIAL; // 400-800
	return ret;
}


void deal_item(creature_type *creature_ptr)
{
	int tv, sv;
	object_type	forge;
	object_type	*q_ptr;
	int i, number;
	species_type *species_ptr = &species_info[creature_ptr->species_idx];
	u32b mo_mode = 0L;

	creature_ptr->total_weight = 0;
	creature_ptr->inven_cnt = 0;
	creature_ptr->equip_cnt = 0;

	for(i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&creature_ptr->inventory[i]);
	}

	// TODO:  
	object_level = creature_ptr->lev * 2;

	/* inventory */
	number = 0;
	// TODO Inventory Count

	/* Get local object */
	q_ptr = &forge;

	/*
	for(i = 0; i < creature_ptr->sc / 10; i++)
	{
		make_object(q_ptr, AM_UNCURSED, 0, object_level);
		add_outfit(creature_ptr, q_ptr, 0);
	}
	*/

	//
	// Item depend on species_info
	//
	for(i = 0; i < INVEN_TOTAL; i++)
	{
		if(!(&creature_ptr->inventory[i])) break;
		if(!species_ptr->artifact_prob[i]) break;

		if(species_ptr->artifact_id[i])
		{
			artifact_type *a_ptr = &a_info[species_ptr->artifact_id[i]];
			if ((species_ptr->artifact_id[i] > 0) && ((randint0(100) < species_ptr->artifact_prob[i]) || wizard))
			{
				if (!a_ptr->cur_num)
				{
					object_type ob;
					// Equip the artifact
					create_named_art(creature_ptr, &ob, species_ptr->artifact_id[i]);
					a_ptr->cur_num = 1;
					add_outfit(creature_ptr, &ob, TRUE);
				}
			}

		}
		else
		{
			object_type ob;
			object_prep(&ob, lookup_kind(species_ptr->artifact_tval[i], species_ptr->artifact_sval[i]), creature_ptr->size);
			create_ego(&ob, object_level, species_ptr->artifact_ego[i]);
			add_outfit(creature_ptr, &ob, TRUE);
		}
	}

	// Dealing MagicBook
	deal_magic_book(creature_ptr);

	//TODO
	// Dealing Potion
	if(is_player(creature_ptr)) deal_potion(creature_ptr);

	//TODO
	// Food depend on creature_flags
	if(is_player(creature_ptr)) deal_food(creature_ptr);

	if(is_player(creature_ptr)) deal_lite(creature_ptr);

	if(IS_RACE(creature_ptr, RACE_BALROG))
	{
		q_ptr = &forge;

		object_prep(q_ptr, lookup_kind(TV_LITE, SV_LITE_UDUN), creature_ptr->size);
		add_outfit(creature_ptr, q_ptr, ADD_OUTFIT_EQUIP);
	}

	if(IS_RACE(creature_ptr, RACE_ISTARI))
	{
		q_ptr = &forge;

		object_prep(q_ptr, lookup_kind(TV_HAFTED, SV_ISTARISTAFF), creature_ptr->size);
		add_outfit(creature_ptr, q_ptr, ADD_OUTFIT_EQUIP);
	}

	q_ptr = &forge;

	// Item depend on Class
	if ((creature_ptr->cls_idx == CLASS_RANGER) || (creature_ptr->cls_idx == CLASS_CAVALRY), ITEM_FREE_SIZE)
	{
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);
		add_outfit(creature_ptr, q_ptr, ADD_OUTFIT_EQUIP);
	}

	if (creature_ptr->cls_idx == CLASS_RANGER)
	{
		q_ptr = &forge;
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_BOW, SV_SHORT_BOW), ITEM_FREE_SIZE);
		add_outfit(creature_ptr, q_ptr, ADD_OUTFIT_EQUIP);
	}

	else if (creature_ptr->cls_idx == CLASS_ARCHER)
	{
		q_ptr = &forge;
		/* Hack -- Give the player some arrows */
		object_prep(q_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);
		add_outfit(creature_ptr, q_ptr, ADD_OUTFIT_EQUIP);
	}
	else if (creature_ptr->cls_idx == CLASS_HIGH_MAGE)
	{
		/* Hack -- Give the player some arrows */
		q_ptr = &forge;
		object_prep(q_ptr, lookup_kind(TV_WAND, SV_WAND_MAGIC_MISSILE), ITEM_FREE_SIZE);
		q_ptr->number = 1;
		q_ptr->pval = (byte)rand_range(25, 30);

		add_outfit(creature_ptr, q_ptr, 0);
	}
	else if (creature_ptr->cls_idx == CLASS_SORCERER)
	{
		for (i = TV_LIFE_BOOK; i <= TV_LIFE_BOOK + MAX_MAGIC - 1; i++)
		{
			q_ptr = &forge;
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(i, 0), ITEM_FREE_SIZE);
			q_ptr->number = (byte)rand_range(1, creature_ptr->lev / 8);
			add_outfit(creature_ptr, q_ptr, 0);

		}
	}
	else if (creature_ptr->cls_idx == CLASS_TOURIST)
	{
		if (creature_ptr->chara_idx != CHARA_SEXY)
		{
			q_ptr = &forge;
			/* Hack -- Give the player some arrows */
			object_prep(q_ptr, lookup_kind(TV_SHOT, SV_AMMO_LIGHT), ITEM_FREE_SIZE);
			q_ptr->number = (byte)rand_range(15, 20);

			add_outfit(creature_ptr, q_ptr, 0);
		}

		q_ptr = &forge;
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_BISCUIT), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);
		add_outfit(creature_ptr, q_ptr, 0);

		q_ptr = &forge;
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_WAYBREAD), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);
		add_outfit(creature_ptr, q_ptr, 0);

		q_ptr = &forge;
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_JERKY), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(1, 3);
		add_outfit(creature_ptr, q_ptr, 0);

		q_ptr = &forge;
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_ALE), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);
		add_outfit(creature_ptr, q_ptr, 0);

		q_ptr = &forge;
		object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_WINE), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(2, 4);
		add_outfit(creature_ptr, q_ptr, 0);
	}
	else if (creature_ptr->cls_idx == CLASS_NINJA)
	{
		/* Hack -- Give the player some arrows */
		q_ptr = &forge;
		object_prep(q_ptr, lookup_kind(TV_SPIKE, 0), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(creature_ptr, q_ptr, 0);
	}
	else if (creature_ptr->cls_idx == CLASS_SNIPER)
	{
		/* Hack -- Give the player some bolts */
		q_ptr = &forge;
		object_prep(q_ptr, lookup_kind(TV_BOLT, SV_AMMO_NORMAL), ITEM_FREE_SIZE);
		q_ptr->number = (byte)rand_range(15, 20);

		add_outfit(creature_ptr, q_ptr, ADD_OUTFIT_EQUIP);
	}

	/* Hack -- Give the player three useful objects */
	if(creature_ptr->cls_idx != INDEX_NONE && has_cf_creature(creature_ptr, CF_HUMANOID))
	{
		for (i = 0; i < 10; i++)
		{
			/* Look up standard equipment */
			tv = class_equipment_init[creature_ptr->cls_idx][i][0];
			sv = class_equipment_init[creature_ptr->cls_idx][i][1];
			if(tv == 0) continue;

			if (has_cf_creature(creature_ptr, CF_ANDROID) && ((tv == TV_SOFT_ARMOR) || (tv == TV_HARD_ARMOR))) continue;

			/* Hack to initialize spellbooks */
			if (tv == TV_SORCERY_BOOK && creature_ptr->realm1)
				tv = TV_LIFE_BOOK + creature_ptr->realm1 - 1;
			else if (tv == TV_DEATH_BOOK && creature_ptr->realm2)
				tv = TV_LIFE_BOOK + creature_ptr->realm2 - 1;

			else if (tv == TV_RING && sv == SV_RING_RES_FEAR &&
			    IS_PURE_RACE(creature_ptr, RACE_BARBARIAN))
				/* Barbarians do not need a ring of resist fear */
				sv = SV_RING_SUSTAIN_STR;

			else if (tv == TV_RING && sv == SV_RING_SUSTAIN_INT &&
			    IS_PURE_RACE(creature_ptr, RACE_MIND_FLAYER))
			{
				tv = TV_POTION;
				sv = SV_POTION_RESTORE_MANA;
			}

			/* Get local object */
			q_ptr = &forge;

			/* Hack -- Give the player an object */
			object_prep(q_ptr, lookup_kind(tv, sv), creature_ptr->size);

			/* Assassins begin the game with a poisoned dagger */
			if ((tv == TV_SWORD || tv == TV_HAFTED) && (creature_ptr->cls_idx == CLASS_ROGUE &&
				creature_ptr->realm1 == REALM_DEATH)) /* Only assassins get a poisoned weapon */
			{
				q_ptr->name2 = EGO_BRAND_POIS;
			}

			add_outfit(creature_ptr, q_ptr, ADD_OUTFIT_EQUIP);
		}
	}

	// Item depend on Character

	if(creature_ptr->chara_idx == CHARA_SEXY)
	{
		class_equipment_init[creature_ptr->cls_idx][2][0] = TV_HAFTED;
		class_equipment_init[creature_ptr->cls_idx][2][1] = SV_WHIP;
	}

	/* Hack -- make aware of the water */
	k_info[lookup_kind(TV_POTION, SV_POTION_WATER)].aware = TRUE;


	/* Apply Magic */
	for(i = 0; i < INVEN_TOTAL; i++)
	{
		if(creature_ptr->inventory[i].k_idx && creature_ptr->equip_now[i])
		{
			if(!creature_ptr->inventory[i].name1 && !creature_ptr->inventory[i].name2)
			apply_magic(creature_ptr, &creature_ptr->inventory[i], creature_ptr->lev * 2,
				calc_deal_item_rank(creature_ptr, &creature_ptr->inventory[i]), 0);
		}
	}


	return;

}

static int place_monster_one(creature_type *summoner_ptr, int y, int x, int species_idx, int monster_ego_idx, u32b mode)
{
	/* Access the location */
	cave_type		*c_ptr = &cave[y][x];

	creature_type	*m_ptr;

	species_type	*r_ptr = &species_info[species_idx];
	monster_ego		*re_ptr;
	race_type		*rpr_ptr;

	cptr		name = (species_name + r_ptr->name);

	int re_selected, rpr_selected, rpc_selected, rps_selected;


	/* Select Ego */
	re_ptr = NULL;
	rpr_ptr = NULL;
	re_selected = MONEGO_NONE;
	rpr_selected = INDEX_NONE;
	rpc_selected = INDEX_NONE;
	rps_selected = INDEX_NONE;

	if(monster_ego_idx == MONEGO_NORMAL)
	{
		if(is_force_lesser_species(r_ptr)){
			int n;
			n = rand_range(MONEGO_LESSER_FROM, MONEGO_LESSER_TO);
			re_ptr = &re_info[n];
			re_selected = n;
		}
		else if (is_variable_size_species(r_ptr))
		{
			re_selected = MONEGO_VARIABLE_SIZE;
		}
	}
	else{
		re_selected = monster_ego_idx;
	}

	// set intelligence race
	if (is_variable_race_species(r_ptr))
	{
		int n;
		n = rand_range(RACE_HUMAN, RACE_GNOME);
		rpr_ptr = &race_info[n];
		rpr_selected = n;
	}
	else
	{
		rpr_ptr = &race_info[r_ptr->race_idx1];
		rpr_selected = r_ptr->race_idx1;
	}

	// set class
	if (is_variable_class_species(r_ptr))
	{
		int n;
		n = rand_range(CLASS_WARRIOR, CLASS_PALADIN);
		rpc_selected = n;
	}
	else
	{
		rpc_selected = r_ptr->cls_idx;
	}

	// set character
	if (is_variable_chara_species(r_ptr))
	{
		int n;
		n = rand_range(CHARA_FUTUU, CHARA_NAMAKE);
		rps_selected = n;
	}
	else
	{
		rps_selected = r_ptr->chara_idx;
	}

	/* DO NOT PLACE A MONSTER IN THE SMALL SCALE WILDERNESS !!! */
	if (wild_mode){
		if (cheat_hear)
		{
			msg_format("[max_m_idx: Wild mode]");
		}
		return max_m_idx;
	}

	/* Verify location */
	if (!in_bounds(y, x)){
		if (cheat_hear)
		{
			msg_format("[max_m_idx: Invalid Location]");
		}
		return (max_m_idx);
	}

	/* Paranoia */
	if (!species_idx)
	{
		if (cheat_hear)
		{
			msg_format("[max_m_idx: Invalid Monster Race]");
		}
		return (max_m_idx);
	}

	/* Paranoia */
	if (!r_ptr->name)
	{
		if (cheat_hear)
		{
			msg_format("[max_m_idx: Invalid Monster Name]");
		}
		return (max_m_idx);
	}

	if (!(mode & PM_IGNORE_TERRAIN))
	{
		/* Not on the Pattern */
		if (pattern_tile(y, x)) return max_m_idx;

		/* Require empty space (if not ghostly) */
		if (!monster_can_enter(y, x, r_ptr, 0)){
			if (cheat_hear)
			{
				msg_format("[max_m_idx: Monster Can't Enter]");
			}
			return max_m_idx;
		}
	}

	// TO DO DEBUG.
	if (!inside_battle)
	{
		/* Hack -- "unique" monsters must be "unique" */
		if (((is_unique_species(r_ptr)) || has_cf(&r_ptr->flags, CF_NAZGUL)) &&
		    (r_ptr->cur_num >= r_ptr->max_num))
		{
			if (cheat_hear)
			{
				msg_format("[max_m_idx: Unique monster must be unique.]");
			}

			/* Cannot create */
			return (max_m_idx);
		}

		if (is_sub_unique_species(r_ptr) &&
		    (r_ptr->cur_num >= 1))
		{
			if (cheat_hear)
			{
				msg_format("[max_m_idx: Unique monster must be unique.]");
			}
			return (max_m_idx);
		}

		if (species_idx == MON_BANORLUPART)
		{
			if (species_info[MON_BANOR].cur_num > 0) return max_m_idx;
			if (species_info[MON_LUPART].cur_num > 0) return max_m_idx;
			if (cheat_hear)
			{
				msg_format("[max_m_idx: Unique monster must be unique.]");
			}

		}

		/* Depth monsters may NOT be created out of depth, unless in Nightmare mode */
		if (is_force_depth_species(r_ptr) && (dun_level < r_ptr->level) &&
		    (!curse_of_Iluvatar || (is_quest_species(r_ptr))))
		{
			if (cheat_hear)
			{
				msg_format("[max_m_idx: No Nightmare mode.]");
			}
			/* Cannot create */
			return (max_m_idx);
		}
	}

	if (quest_number(dun_level))
	{
		int hoge = quest_number(dun_level);
		if ((quest[hoge].type == QUEST_TYPE_KILL_LEVEL) || (quest[hoge].type == QUEST_TYPE_RANDOM))
		{
			if(species_idx == quest[hoge].species_idx)
			{
				int number_mon, i2, j2;
				number_mon = 0;

				/* Count all quest monsters */
				for (i2 = 0; i2 < cur_wid; ++i2)
					for (j2 = 0; j2 < cur_hgt; j2++)
						if (cave[j2][i2].m_idx > 0)
							if (creature_list[cave[j2][i2].m_idx].species_idx == quest[hoge].species_idx)
								number_mon++;
				if(number_mon + quest[hoge].cur_num >= quest[hoge].max_num)
					return max_m_idx;
			}
		}
	}

	if (is_glyph_grid(c_ptr))
	{
		if (randint1(BREAK_GLYPH) < (r_ptr->level+20))
		{
			/* Describe observable breakage */
			if (c_ptr->info & CAVE_MARK)
			{
#ifdef JP
msg_print("守りのルーンが壊れた！");
#else
				msg_print("The rune of protection is broken!");
#endif

			}

			/* Forget the rune */
			c_ptr->info &= ~(CAVE_MARK);

			/* Break the rune */
			c_ptr->info &= ~(CAVE_OBJECT);
			c_ptr->mimic = 0;

			/* Notice */
			//TODO note_spot(y, x);
		}
		else return max_m_idx;
	}


	if ((is_unique_species(r_ptr)) || has_cf(&r_ptr->flags, CF_NAZGUL) || (r_ptr->level < 10)) mode &= ~PM_KAGE;

	/* Make a new monster */
	c_ptr->m_idx = m_pop();
	hack_m_idx_ii = c_ptr->m_idx;

	/* Mega-Hack -- catch "failure" */
	if (!c_ptr->m_idx) return (max_m_idx);

	/* Get a new monster record */
	m_ptr = &creature_list[c_ptr->m_idx];

	if(is_unique_species(r_ptr))
	{
		int i;
		for(i = 0; i < max_unique; i++)
			if(species_idx == u_info[i].species_idx)
				*m_ptr = u_info[i];
	}
	else
	{
		creature_type cr;
		generate_creature(m_ptr, species_idx, &cr, GC_AUTO); 
	}

	/* No flags */
	m_ptr->mflag = 0;
	m_ptr->mflag2 = 0;

	/* Hack -- Appearance transfer */
	if ((mode & PM_MULTIPLY) && !is_player(summoner_ptr) && !is_original_ap(summoner_ptr))
	{
		m_ptr->ap_species_idx = summoner_ptr->ap_species_idx;

		/* Hack -- Shadower spawns Shadower */
		if (summoner_ptr->mflag2 & MFLAG2_KAGE) m_ptr->mflag2 |= MFLAG2_KAGE;
	}

	/* Sub-alignment of a monster */
	if (summoner_ptr && !is_player(summoner_ptr) && !(is_enemy_of_evil_creature(summoner_ptr) && is_enemy_of_good_creature(summoner_ptr)))
		m_ptr->sub_align = summoner_ptr->sub_align;
	else
	{
		m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
		if (is_enemy_of_good_creature(m_ptr)) m_ptr->sub_align |= SUB_ALIGN_EVIL;
		if (is_enemy_of_evil_creature(m_ptr)) m_ptr->sub_align |= SUB_ALIGN_GOOD;
	}

	/* Place the monster at the location */
	m_ptr->fy = y;
	m_ptr->fx = x;

	/* Unknown distance */
	m_ptr->cdis = 0;
	reset_target(m_ptr);
	m_ptr->nickname = 0;

	/* Your pet summons its pet. */
	if (summoner_ptr && !is_player(summoner_ptr) && is_pet(player_ptr, summoner_ptr))
	{
		mode |= PM_FORCE_PET;
		//TODO Parent Set
		m_ptr->parent_m_idx = 0;
	}
	else
	{
		m_ptr->parent_m_idx = 0;
	}

	if (is_chameleon_species(r_ptr))
	{
		choose_new_monster(c_ptr->m_idx, TRUE, 0, MONEGO_NONE);
		r_ptr = &species_info[m_ptr->species_idx];
		m_ptr->mflag2 |= MFLAG2_CHAMELEON;

		/* Hack - Set sub_align to neutral when the Chameleon Lord is generated as "GUARDIAN" */
		if (summoner_ptr && (is_unique_species(r_ptr)) && is_player(summoner_ptr))
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
	}
	else if ((mode & PM_KAGE) && !(mode & PM_FORCE_PET))
	{
		m_ptr->ap_species_idx = MON_KAGE;
		m_ptr->mflag2 |= MFLAG2_KAGE;
	}

	if (mode & PM_NO_PET) m_ptr->mflag2 |= MFLAG2_NOPET;

	/* Not visible */
	m_ptr->ml = FALSE;

	/* Pet? */
	if (mode & PM_FORCE_PET)
	{
		set_pet(summoner_ptr, m_ptr);
	}
	/* Friendly? */
	else if (is_friendly_species(r_ptr) ||
		 (mode & PM_FORCE_FRIENDLY)) //TODO || is_friendly_idx(who))
	{
		if (!creature_has_hostile_align(NULL, summoner_ptr, 0, -1, r_ptr)) set_friendly(m_ptr);
	}

	/* Assume no sleeping */
	m_ptr->paralyzed = 0;

	/* Enforce sleeping if needed */
	if ((mode & PM_ALLOW_SLEEP) && r_ptr->sleep && !curse_of_Iluvatar)
	{
		int val = r_ptr->sleep;
		(void)set_paralyzed(&creature_list[c_ptr->m_idx], (val * 2) + randint1(val * 10));
	}

	if (mode & PM_HASTE) (void)set_fast(&creature_list[c_ptr->m_idx], 100, FALSE);

	/* Give a random starting energy */
	if (!curse_of_Iluvatar)
	{
		m_ptr->energy_need = ENERGY_NEED() - (s16b)randint0(100);
	}
	else
	{
		/* Nightmare monsters are more prepared */
		m_ptr->energy_need = ENERGY_NEED() - (s16b)randint0(100) * 2;
	}

	/* Force monster to wait for player, unless in Nightmare mode */
	if (has_cf_creature(m_ptr, CF_FORCE_SLEEP) && !curse_of_Iluvatar)
	{
		/* Monster is still being nice */
		m_ptr->mflag |= (MFLAG_NICE);

		/* Must repair monsters */
		repair_monsters = TRUE;
	}

	/* Hack -- see "process_monsters()" */
	if (c_ptr->m_idx < hack_m_idx)
	{
		/* Monster is still being born */
		m_ptr->mflag |= (MFLAG_BORN);
	}

/*TODO
	if (is_self_ld_creature(m_ptr))
		update |= (PU_MON_LITE);
	else if (is_has_ld_creature(m_ptr) && !m_ptr->paralyzed)
		update |= (PU_MON_LITE);
*/

	/* Update the monster */
	update_mon(m_ptr, c_ptr->m_idx, TRUE);

	/* Count the monsters on the level */
	real_species_ptr(m_ptr)->cur_num++;

	/*
	 * Memorize location of the unique monster in saved floors.
	 * A unique monster move from old saved floor.
	 */
/*TODO
	if (character_dungeon &&
	    ((is_unique_species(r_ptr)) || has_cf(&r_ptr->flags, CF_NAZGUL)))
		real_species_ptr(m_ptr)->floor_id = watcher_ptr->floor_id;
*/

	/* Hack -- Count the number of "reproducers" */
	if (has_cf_creature(m_ptr, CF_MULTIPLY)) num_repro++;

	/* Hack -- Notice new multi-hued monsters */
	{
		if (has_cf_creature(m_ptr, CF_ATTR_MULTI) || has_cf_creature(m_ptr, CF_SHAPECHANGER))
			shimmer_monsters = TRUE;
	}

/* TODO
	if (watcher_ptr->warning && character_dungeon)
	{
		if (is_unique_species(r_ptr))
		{
			cptr color;
			object_type *o_ptr;
			char o_name[MAX_NLEN];

			if (r_ptr->level > watcher_ptr->lev + 30)
#ifdef JP
				color = "黒く";
#else
				color = "black";
#endif
			else if (r_ptr->level > watcher_ptr->lev + 15)
#ifdef JP
				color = "紫色に";
#else
				color = "purple";
#endif
			else if (r_ptr->level > watcher_ptr->lev + 5)
#ifdef JP
				color = "ルビー色に";
#else
				color = "deep red";
#endif
			else if (r_ptr->level > watcher_ptr->lev - 5)
#ifdef JP
				color = "赤く";
#else
				color = "red";
#endif
			else if (r_ptr->level > watcher_ptr->lev - 15)
#ifdef JP
				color = "ピンク色に";
#else
				color = "pink";
#endif
			else
#ifdef JP
				color = "白く";
#else
				color = "white";
#endif

			o_ptr = choose_warning_item(watcher_ptr);
			if (o_ptr)
			{
				object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
#ifdef JP
				msg_format("%sは%s光った。", o_name, color);
#else
				msg_format("%s glows %s.", o_name, color);
#endif
			}
			else
			{
#ifdef JP
				msg_format("s%光る物が頭に浮かんだ。", color);
#else
				msg_format("An %s image forms in your mind.");
#endif
			}
		}
	}
*/

	if (is_explosive_rune_grid(c_ptr))
	{
		/* Break the ward */
		if (randint1(BREAK_MINOR_GLYPH) > r_ptr->level)
		{
			/* Describe observable breakage */
			if (c_ptr->info & CAVE_MARK)
			{
#ifdef JP
//				msg_print("ルーンが爆発した！");
#else
//				msg_print("The rune explodes!");
#endif

//TODO				project(watcher_ptr, 2, y, x, 2 * (watcher_ptr->lev + damroll(7, 7)), GF_MANA, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}
		}
		else
		{
#ifdef JP
msg_print("爆発のルーンは解除された。");
#else
			msg_print("An explosive rune was disarmed.");
#endif
		}

		/* Forget the rune */
		c_ptr->info &= ~(CAVE_MARK);

		/* Break the rune */
		c_ptr->info &= ~(CAVE_OBJECT);
		c_ptr->mimic = 0;

		//TODO note_spot(y, x);
		lite_spot(y, x);
	}

	//strcpy(m_ptr->name, species_name + r_ptr->name);
	creature_desc(m_ptr->name, m_ptr, MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

	/* Info for Wizard Mode*/
	if (cheat_hear)
	{
		msg_format("[%s L%d AC%d HW%d/%d S%d]", m_ptr->name, m_ptr->lev,
			m_ptr->ac + m_ptr->to_a, m_ptr->ht, m_ptr->wt, m_ptr->sex);
	}

	/* Success */
	return c_ptr->m_idx;
}


#define MON_SCAT_MAXD 10

static bool mon_scatter(int species_idx, int *yp, int *xp, int y, int x, int max_dist)
{
	int place_x[MON_SCAT_MAXD];
	int place_y[MON_SCAT_MAXD];
	int num[MON_SCAT_MAXD];
	int i;
	int nx, ny;

	if (max_dist >= MON_SCAT_MAXD)
		return FALSE;

	for (i = 0; i < MON_SCAT_MAXD; i++)
		num[i] = 0;

	for (nx = x - max_dist; nx <= x + max_dist; nx++)
	{
		for (ny = y - max_dist; ny <= y + max_dist; ny++)
		{
			/* Ignore annoying locations */
			if (!in_bounds(ny, nx)) continue;

			/* Require "line of projection" */
			if (!projectable(y, x, ny, nx)) continue;

			if (species_idx > 0)
			{
				species_type *r_ptr = &species_info[species_idx];

				/* Require empty space (if not ghostly) */
				if (!monster_can_enter(ny, nx, r_ptr, 0))
					continue;
			}
			else
			{
				/* Walls and Monsters block flow */
				if (!cave_empty_bold2(ny, nx)) continue;

				/* ... nor on the Pattern */
				if (pattern_tile(ny, nx)) continue;
			}

			i = distance(y, x, ny, nx);

			if (i > max_dist)
				continue;

			num[i]++;

			/* random swap */
			if (one_in_(num[i]))
			{
				place_x[i] = nx;
				place_y[i] = ny;
			}
		}
	}

	i = 0;
	while (i < MON_SCAT_MAXD && 0 == num[i])
		i++;
	if (i >= MON_SCAT_MAXD)
		return FALSE;

	*xp = place_x[i];
	*yp = place_y[i];

	return TRUE;
}


/*
 * Maximum size of a group of monsters
 */
#define GROUP_MAX	32


/*
 * Attempt to place a "group" of monsters around the given location
 */
static bool place_monster_group(creature_type *summoner_ptr, int y, int x, int species_idx, u32b mode)
{
	species_type *r_ptr = &species_info[species_idx];

	int n, i;
	int total = 0, extra = 0;

	int hack_n = 0;

	byte hack_y[GROUP_MAX];
	byte hack_x[GROUP_MAX];


	/* Pick a group size */
	total = randint1(10);

	/* Hard monsters, small groups */
	if (r_ptr->level > dun_level)
	{
		extra = r_ptr->level - dun_level;
		extra = 0 - randint1(extra);
	}

	/* Easy monsters, large groups */
	else if (r_ptr->level < dun_level)
	{
		extra = dun_level - r_ptr->level;
		extra = randint1(extra);
	}

	/* Hack -- limit group reduction */
	if (extra > 9) extra = 9;

	/* Modify the group size */
	total += extra;

	/* Minimum size */
	if (total < 1) total = 1;

	/* Maximum size */
	if (total > GROUP_MAX) total = GROUP_MAX;


	/* Start on the monster */
	hack_n = 1;
	hack_x[0] = x;
	hack_y[0] = y;

	/* Puddle monsters, breadth first, up to total */
	for (n = 0; (n < hack_n) && (hack_n < total); n++)
	{
		/* Grab the location */
		int hx = hack_x[n];
		int hy = hack_y[n];

		/* Check each direction, up to total */
		for (i = 0; (i < 8) && (hack_n < total); i++)
		{
			int mx, my;

			scatter(&my, &mx, hy, hx, 4, 0);

			/* Walls and Monsters block flow */
			if (!cave_empty_bold2(my, mx)) continue;

			/* Attempt to place another monster */
			if (place_monster_one(summoner_ptr, my, mx, species_idx, MONEGO_NORMAL, mode) != max_m_idx)
			{
				/* Add it to the "hack" set */
				hack_y[hack_n] = my;
				hack_x[hack_n] = mx;
				hack_n++;
			}
		}
	}


	/* Success */
	return (TRUE);
}


/*
 * Hack -- help pick an escort type
 */
static int place_species_idx = 0;
static int place_monster_m_idx = 0;

/*
 * Hack -- help pick an escort type
 */
static bool place_monster_okay(int species_idx)
{
	species_type *r_ptr = &species_info[place_species_idx];
	creature_type *m_ptr = &creature_list[place_monster_m_idx];

	species_type *z_ptr = &species_info[species_idx];

	/* Hack - Escorts have to have the same dungeon flag */
	if (mon_hook_dungeon(place_species_idx) != mon_hook_dungeon(species_idx)) return (FALSE);

	/* Require similar "race" */
	if (z_ptr->d_char != r_ptr->d_char) return (FALSE);

	/* Skip more advanced monsters */
	if (z_ptr->level > r_ptr->level) return (FALSE);

	/* Skip unique monsters */
	if (is_unique_species(z_ptr)) return (FALSE);

	/* Paranoia -- Skip identical monsters */
	if (place_species_idx == species_idx) return (FALSE);

	/* Skip different alignment */
	if (creature_has_hostile_align(m_ptr, p_ptr, 0, 0, z_ptr)) return FALSE;

	if (is_friendly_species(r_ptr))
	{
		if (creature_has_hostile_align(NULL, p_ptr, 1, -1, z_ptr)) return FALSE;
	}

	if (is_chameleon_species(r_ptr) && !is_chameleon_species(z_ptr))
		return FALSE;

	/* Okay */
	return (TRUE);
}


/*
 * Attempt to place a monster of the given race at the given location
 *
 * Note that certain monsters are now marked as requiring "friends".
 * These monsters, if successfully placed, and if the "grp" parameter
 * is TRUE, will be surrounded by a "group" of identical monsters.
 *
 * Note that certain monsters are now marked as requiring an "escort",
 * which is a collection of monsters with similar "race" but lower level.
 *
 * Some monsters induce a fake "group" flag on their escorts.
 *
 * Note the "bizarre" use of non-recursion to prevent annoying output
 * when running a code profiler.
 *
 * Note the use of the new "monster allocation table" code to restrict
 * the "get_mon_num()" function to "legal" escort types.
 */
bool place_monster_aux(creature_type *summoner_ptr, int y, int x, int species_idx, u32b mode)
{
	int             i, j;
	species_type    *r_ptr = &species_info[species_idx];
	creature_type   *m_ptr;

	if (!(mode & PM_NO_KAGE) && one_in_(333))
		mode |= PM_KAGE;

	/* Place one monster, or fail */
	i = place_monster_one(summoner_ptr, y, x, species_idx, MONEGO_NORMAL, mode);
	if (i == max_m_idx) return (FALSE);

	m_ptr = &creature_list[i];

	i = 0;
	while(i < MAX_UNDERLINGS && m_ptr->underling_id[i])
	{
		int n = 0; 
		for(j = 0; j < m_ptr->underling_num[i]; j++)
		{
			int nx, ny, d = 8;

			/* Pick a location */
			scatter(&ny, &nx, y, x, d, 0);

			/* Require empty grids */
			if (!cave_empty_bold2(ny, nx)) continue;

			/* Prepare allocation table */
			get_mon_num_prep(place_monster_okay, get_creature_hook2(ny, nx));
			if(place_monster_one(summoner_ptr, ny, nx, m_ptr->underling_id[i], MONEGO_NORMAL, mode) == max_m_idx);
				n++;
		}
		m_ptr->underling_num[i] -= n;
		i++;
	}



	/* Require the "group" flag */
	if (!(mode & PM_ALLOW_GROUP)) return (TRUE);

	place_monster_m_idx = hack_m_idx_ii;

	/* Friends for certain monsters */
	if (is_friends_species(r_ptr))
	{
		/* Attempt to place a group */
		(void)place_monster_group(summoner_ptr, y, x, species_idx, mode);
	}


	/* Escorts for certain monsters */
	if (is_escort_species(r_ptr))
	{
		/* Set the escort index */
		place_species_idx = species_idx;

		/* Try to place several "escorts" */
		for (i = 0; i < 32; i++)
		{
			int nx, ny, z, d = 3;

			/* Pick a location */
			scatter(&ny, &nx, y, x, d, 0);

			/* Require empty grids */
			if (!cave_empty_bold2(ny, nx)) continue;

			/* Prepare allocation table */
			get_mon_num_prep(place_monster_okay, get_creature_hook2(ny, nx));

			/* Pick a random race */
			z = get_mon_num(r_ptr->level);

			/* Handle failure */
			if (!z) break;

			/* Place a single escort */
			(void)place_monster_one(summoner_ptr, ny, nx, z, MONEGO_NORMAL, mode);

			/* Place a "group" of escorts if needed */
			if (is_friends_species(&species_info[z]) || is_escort_species(r_ptr))
			{
				/* Place a group of monsters */
				(void)place_monster_group(&creature_list[place_monster_m_idx], ny, nx, z, mode);
			}
		}
	}

	/* Success */
	return (TRUE);
}


/*
 * Hack -- attempt to place a monster at the given location
 *
 * Attempt to find a monster appropriate to the "monster_level"
 */
bool place_monster(creature_type *summoner_ptr, int y, int x, u32b mode)
{
	int species_idx;

	/* Prepare allocation table */
	get_mon_num_prep(get_creature_hook(), get_creature_hook2(y, x));

	/* Pick a monster */
	species_idx = get_mon_num(monster_level);

	/* Handle failure */
	if (!species_idx) return (FALSE);

	/* Attempt to place the monster */
	if (place_monster_aux(summoner_ptr, y, x, species_idx, mode)) return (TRUE);

	/* Oops */
	return (FALSE);
}


#ifdef MONSTER_HORDES

bool alloc_horde(int y, int x)
{
	species_type *r_ptr = NULL;
	int species_idx = 0;
	int m_idx;
	int attempts = 1000;
	int cy = y;
	int cx = x;

	/* Prepare allocation table */
	get_mon_num_prep(get_creature_hook(), get_creature_hook2(y, x));

	while (--attempts)
	{
		/* Pick a monster */
		species_idx = get_mon_num(monster_level);

		/* Handle failure */
		if (!species_idx) return (FALSE);

		r_ptr = &species_info[species_idx];

		if (is_unique_species(r_ptr)) continue;

		if (species_idx == MON_HAGURE) continue;
		break;
	}
	if (attempts < 1) return FALSE;

	attempts = 1000;

	while (--attempts)
	{
		/* Attempt to place the monster */
		if (place_monster_aux(p_ptr, y, x, species_idx, 0L)) break;
	}

	if (attempts < 1) return FALSE;

	m_idx = cave[y][x].m_idx;

	if (creature_list[m_idx].mflag2 & MFLAG2_CHAMELEON) r_ptr = &species_info[creature_list[m_idx].species_idx];
	summon_kin_type = r_ptr->d_char;

	for (attempts = randint1(10) + 5; attempts; attempts--)
	{
		scatter(&cy, &cx, y, x, 5, 0);

		(void)summon_specific(&creature_list[m_idx], cy, cx, dun_level + 5, SUMMON_KIN, PM_ALLOW_GROUP);

		y = cy;
		x = cx;
	}

	return TRUE;
}

#endif /* MONSTER_HORDES */


/*
 * Put the Guardian
 */
bool alloc_guardian(bool def_val)
{
	int guardian = d_info[dungeon_type].final_guardian;

	if (guardian && (d_info[dungeon_type].maxdepth == dun_level) && (species_info[guardian].cur_num < species_info[guardian].max_num))
	{
		int oy;
		int ox;
		int try = 4000;

		/* Find a good position */
		while (try)
		{
			/* Get a random spot */
			oy = randint1(cur_hgt - 4) + 2;
			ox = randint1(cur_wid - 4) + 2;

			/* Is it a good spot ? */
			if (cave_empty_bold2(oy, ox) && monster_can_cross_terrain(cave[oy][ox].feat, &species_info[guardian], 0))
			{
				/* Place the guardian */
				if (place_monster_aux(p_ptr, oy, ox, guardian, (PM_ALLOW_GROUP | PM_NO_KAGE | PM_NO_PET))) return TRUE;
			}

			/* One less try */
			try--;
		}

		return FALSE;
	}

	return def_val;
}


/*
 * Attempt to allocate a random monster in the dungeon.
 *
 * Place the monster at least "dis" distance from the player.
 *
 * Use "slp" to choose the initial "sleep" status
 *
 * Use "monster_level" for the monster level
 */
bool alloc_monster(creature_type *player_ptr, int dis, u32b mode)
{
	int			y = 0, x = 0;
	int         attempts_left = 10000;

	/* Put the Guardian */
	if (alloc_guardian(FALSE)) return TRUE;

	/* Find a legal, distant, unoccupied, space */
	while (attempts_left--)
	{
		/* Pick a location */
		y = randint0(cur_hgt);
		x = randint0(cur_wid);

		/* Require empty floor grid (was "naked") */
		if (dun_level)
		{
			if (!cave_empty_bold2(y, x)) continue;
		}
		else
		{
			if (!cave_empty_bold(player_ptr, y, x)) continue;
		}

		/* Accept far away grids */
		if (distance(y, x, player_ptr->fy, player_ptr->fx) > dis) break;
	}

	if (!attempts_left)
	{
		if (cheat_xtra || cheat_hear)
		{
#ifdef JP
msg_print("警告！新たなモンスターを配置できません。小さい階ですか？");
#else
			msg_print("Warning! Could not allocate a new monster. Small level?");
#endif

		}

		return (FALSE);
	}


#ifdef MONSTER_HORDES
	if (randint1(5000) <= dun_level)
	{
		if (alloc_horde(y, x))
		{
#ifdef JP
			if (cheat_hear) msg_format("モンスターの大群(%c)", summon_kin_type);
#else
			if (cheat_hear) msg_format("Monster horde (%c).", summon_kin_type);
#endif

			return (TRUE);
		}
	}
	else
	{
#endif /* MONSTER_HORDES */

		/* Attempt to place the monster, allow groups */
		if (place_monster(NULL, y, x, (mode | PM_ALLOW_GROUP))) return (TRUE);

#ifdef MONSTER_HORDES
	}
#endif /* MONSTER_HORDES */

	/* Nope */
	return (FALSE);
}




/*
 * Hack -- help decide if a monster race is "okay" to summon
 */
static bool summon_specific_okay(int species_idx)
{
	species_type *r_ptr = &species_info[species_idx];

	/* Hack - Only summon dungeon monsters */
	if (!mon_hook_dungeon(species_idx)) return (FALSE);

	/* Hack -- identify the summoning monster */
	if (summon_specific_who > 0)
	{
		creature_type *m_ptr = &creature_list[summon_specific_who];

		/* Do not summon enemies */

		/* Friendly vs. opposite aligned normal or pet */
		if (creature_has_hostile_align(m_ptr, p_ptr, 0, 0, r_ptr)) return FALSE;
	}
	/* Use the player's alignment */
	else if (summon_specific_who < 0)
	{
		/* Do not summon enemies of the pets */
		if (creature_has_hostile_align(NULL, p_ptr, 10, -10, r_ptr))
		{
			if (!one_in_(ABS(p_ptr->good_rank - p_ptr->evil_rank) / 2 + 1)) return FALSE;
		}
	}

	/* Hack -- no specific type specified */
	if (!summon_specific_type) return (TRUE);

	if (!summon_unique_okay && ((is_unique_species(r_ptr)) || has_cf(&r_ptr->flags, CF_NAZGUL))) return FALSE;

	if ((summon_specific_who < 0) &&
	    ((is_unique_species(r_ptr)) || has_cf(&r_ptr->flags, CF_NAZGUL)) &&
	    creature_has_hostile_align(NULL, p_ptr, 10, -10, r_ptr))
		return FALSE;

	if (is_chameleon_species(r_ptr) && (d_info[dungeon_type].flags1 & DF1_CHAMELEON)) return TRUE;

	return (summon_specific_aux(species_idx));
}


/*
 * Place a monster (of the specified "type") near the given
 * location.  Return TRUE if a monster was actually summoned.
 *
 * We will attempt to place the monster up to 10 times before giving up.
 *
 * Note: SUMMON_UNIQUE and SUMMON_AMBERITES will summon Unique's
 * Note: SUMMON_HI_UNDEAD and SUMMON_HI_DRAGON may summon Unique's
 * Note: None of the other summon codes will ever summon Unique's.
 *
 * This function has been changed.  We now take the "monster level"
 * of the summoning monster as a parameter, and use that, along with
 * the current dungeon level, to help determine the level of the
 * desired monster.  Note that this is an upper bound, and also
 * tends to "prefer" monsters of that level.  Currently, we use
 * the average of the dungeon and monster levels, and then add
 * five to allow slight increases in monster power.
 *
 * Note that we use the new "monster allocation table" creation code
 * to restrict the "get_mon_num()" function to the set of "legal"
 * monsters, making this function much faster and more reliable.
 *
 * Note that this function may not succeed, though this is very rare.
 */
bool summon_specific(creature_type *cr_ptr, int y1, int x1, int lev, int type, u32b mode)
{
	int x, y, species_idx;

	if (inside_arena) return (FALSE);

	if (!mon_scatter(0, &y, &x, y1, x1, 2)) return FALSE;

	/* Save the summoner */
	//summon_specific_who = who;

	/* Save the "summon" type */
	summon_specific_type = type;

	summon_unique_okay = (mode & PM_ALLOW_UNIQUE) ? TRUE : FALSE;

	/* Prepare allocation table */
	get_mon_num_prep(summon_specific_okay, get_creature_hook2(y, x));

	/* Pick a monster, using the level calculation */
	species_idx = get_mon_num((dun_level + lev) / 2 + 5);

	/* Handle failure */
	if (!species_idx)
	{
		summon_specific_type = 0;
		return (FALSE);
	}

	if ((type == SUMMON_BLUE_HORROR) || (type == SUMMON_DAWN)) mode |= PM_NO_KAGE;

	/* Attempt to place the monster (awake, allow groups) */
	if (!place_monster_aux(cr_ptr, y, x, species_idx, mode))
	{
		summon_specific_type = 0;
		return (FALSE);
	}

	summon_specific_type = 0;
	/* Success */
	return (TRUE);
}

/* A "dangerous" function, creates a pet of the specified type */
bool summon_named_creature(creature_type *cr_ptr, int oy, int ox, int species_idx, u32b mode)
{
	int x, y;

	/* Prevent illegal monsters */
	if (species_idx >= max_species_idx) return FALSE;

	if (inside_arena) return FALSE;

	if (!mon_scatter(species_idx, &y, &x, oy, ox, 2)) return FALSE;

	/* Place it (allow groups) */
	return place_monster_aux(cr_ptr, y, x, species_idx, (mode | PM_NO_KAGE));
}


/*
 * Let the given monster attempt to reproduce.
 *
 * Note that "reproduction" REQUIRES empty space.
 */
bool multiply_monster(int m_idx, bool clone, u32b mode)
{
	creature_type	*m_ptr = &creature_list[m_idx];

	int y, x;

	if (!mon_scatter(m_ptr->species_idx, &y, &x, m_ptr->fy, m_ptr->fx, 1))
		return FALSE;

	if (m_ptr->mflag2 & MFLAG2_NOPET) mode |= PM_NO_PET;

	/* Create a new monster (awake, no groups) */
	if (!place_monster_aux(&creature_list[m_idx], y, x, m_ptr->species_idx, (mode | PM_NO_KAGE | PM_MULTIPLY)))
		return FALSE;

	/* Hack -- Transfer "clone" flag */
	if (clone || (m_ptr->smart & SM_CLONED))
	{
		creature_list[hack_m_idx_ii].smart |= SM_CLONED;
		creature_list[hack_m_idx_ii].mflag2 |= MFLAG2_NOPET;
	}

	return TRUE;
}





/*
 * Dump a message describing a monster's reaction to damage
 *
 * Technically should attempt to treat "Beholder"'s as jelly's
 */
void message_pain(int m_idx, int dam)
{
	long oldhp, newhp, tmp;
	int percentage;

	creature_type *m_ptr = &creature_list[m_idx];
	species_type *r_ptr = &species_info[m_ptr->species_idx];

	char m_name[80];


	/* Get the monster name */
	creature_desc(m_name, m_ptr, 0);

	/* Notice non-damage */
	if (dam == 0)
	{
#ifdef JP
		msg_format("%^sはダメージを受けていない。", m_name);
#else
		msg_format("%^s is unharmed.", m_name);
#endif

		return;
	}

	/* Note -- subtle fix -CFT */
	newhp = (long)(m_ptr->chp);
	oldhp = newhp + (long)(dam);
	tmp = (newhp * 100L) / oldhp;
	percentage = (int)(tmp);


	/* Mushrooms, Eyes, Jellies, Molds, Vortices, Worms, Quylthulgs */
	if (my_strchr(",ejmvwQ", r_ptr->d_char))
	{
#ifdef JP
		if (percentage > 95)
			msg_format("%^sはほとんど気にとめていない。", m_name);
		else if (percentage > 75)
			msg_format("%^sはしり込みした。", m_name);
		else if (percentage > 50)
			msg_format("%^sは縮こまった。", m_name);
		else if (percentage > 35)
			msg_format("%^sは痛みに震えた。", m_name);
		else if (percentage > 20)
			msg_format("%^sは身もだえした。", m_name);
		else if (percentage > 10)
			msg_format("%^sは苦痛で身もだえした。", m_name);
		else
			msg_format("%^sはぐにゃぐにゃと痙攣した。", m_name);
#else
		if (percentage > 95)
			msg_format("%^s barely notices.", m_name);
		else if (percentage > 75)
			msg_format("%^s flinches.", m_name);
		else if (percentage > 50)
			msg_format("%^s squelches.", m_name);
		else if (percentage > 35)
			msg_format("%^s quivers in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s writhes about.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s jerks limply.", m_name);
#endif

	}


	/* Fish */
	else if (my_strchr("l", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%^sはほとんど気にとめていない。", m_name);
#else
			msg_format("%^s barely notices.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%^sはしり込みした。", m_name);
#else
			msg_format("%^s flinches.", m_name);
#endif
		else if (percentage > 50)
#ifdef JP
msg_format("%^sは躊躇した。", m_name);
#else
			msg_format("%^s hesitates.", m_name);
#endif
		else if (percentage > 35)
#ifdef JP
msg_format("%^sは痛みに震えた。", m_name);
#else
			msg_format("%^s quivers in pain.", m_name);
#endif
		else if (percentage > 20)
#ifdef JP
msg_format("%^sは身もだえした。", m_name);
#else
			msg_format("%^s writhes about.", m_name);
#endif
		else if (percentage > 10)
#ifdef JP
msg_format("%^sは苦痛で身もだえした。", m_name);
#else
			msg_format("%^s writhes in agony.", m_name);
#endif
		else
#ifdef JP
msg_format("%^sはぐにゃぐにゃと痙攣した。", m_name);
#else
			msg_format("%^s jerks limply.", m_name);
#endif
	}


	/* Golems, Walls, Doors, Stairs */
	else if (my_strchr("g#+<>", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%sは攻撃を気にとめていない。", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%sは攻撃に肩をすくめた。", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif
		else if (percentage > 50)
#ifdef JP
msg_format("%^sは雷鳴のように吠えた。", m_name);
#else
			msg_format("%^s roars thunderously.", m_name);
#endif
		else if (percentage > 35)
#ifdef JP
msg_format("%^sは苦しげに吠えた。", m_name);
#else
			msg_format("%^s rumbles.", m_name);
#endif
		else if (percentage > 20)
#ifdef JP
msg_format("%^sはうめいた。", m_name);
#else
			msg_format("%^s grunts.", m_name);
#endif
		else if (percentage > 10)
#ifdef JP
msg_format("%^sは躊躇した。", m_name);
#else
			msg_format("%^s hesitates.", m_name);
#endif
		else
#ifdef JP
msg_format("%^sはくしゃくしゃになった。", m_name);
#else
			msg_format("%^s crumples.", m_name);
#endif
	}


	/* Snakes, Hydrae, Reptiles, Mimics */
	else if (my_strchr("JMR", r_ptr->d_char) || !isalpha(r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%^sはほとんど気にとめていない。", m_name);
#else
			msg_format("%^s barely notices.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%^sはシーッと鳴いた。", m_name);
#else
			msg_format("%^s hisses.", m_name);
#endif
		else if (percentage > 50)
#ifdef JP
msg_format("%^sは怒って頭を上げた。", m_name);
#else
			msg_format("%^s rears up in anger.", m_name);
#endif
		else if (percentage > 35)
#ifdef JP
msg_format("%^sは猛然と威嚇した。", m_name);
#else
			msg_format("%^s hisses furiously.", m_name);
#endif
		else if (percentage > 20)
#ifdef JP
msg_format("%^sは身もだえした。", m_name);
#else
			msg_format("%^s writhes about.", m_name);
#endif
		else if (percentage > 10)
#ifdef JP
msg_format("%^sは苦痛で身もだえした。", m_name);
#else
			msg_format("%^s writhes in agony.", m_name);
#endif
		else
#ifdef JP
msg_format("%^sはぐにゃぐにゃと痙攣した。", m_name);
#else
			msg_format("%^s jerks limply.", m_name);
#endif
	}


	/* Felines */
	else if (my_strchr("f", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%sは攻撃に肩をすくめた。", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%^sは吠えた。", m_name);
#else
			msg_format("%^s roars.", m_name);
#endif
		else if (percentage > 50)
#ifdef JP
msg_format("%^sは怒って吠えた。", m_name);
#else
			msg_format("%^s growls angrily.", m_name);
#endif
		else if (percentage > 35)
#ifdef JP
msg_format("%^sは痛みでシーッと鳴いた。", m_name);
#else
			msg_format("%^s hisses with pain.", m_name);
#endif
		else if (percentage > 20)
#ifdef JP
msg_format("%^sは痛みで弱々しく鳴いた。", m_name);
#else
			msg_format("%^s mewls in pain.", m_name);
#endif
		else if (percentage > 10)
#ifdef JP
msg_format("%^sは苦痛にうめいた。", m_name);
#else
			msg_format("%^s hisses in agony.", m_name);
#endif
		else
#ifdef JP
msg_format("%sは哀れな鳴き声を出した。", m_name);
#else
			msg_format("%^s mewls pitifully.", m_name);
#endif
	}


	/* Ants, Centipedes, Flies, Insects, Beetles, Spiders */
	else if (my_strchr("acFIKS", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%sは攻撃を気にとめていない。", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%^sはキーキー鳴いた。", m_name);
#else
			msg_format("%^s chitters.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^sはヨロヨロ逃げ回った。", m_name);
#else
			msg_format("%^s scuttles about.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^sはうるさく鳴いた。", m_name);
#else
			msg_format("%^s twitters.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^sは痛みに痙攣した。", m_name);
#else
			msg_format("%^s jerks in pain.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^sは苦痛で痙攣した。", m_name);
#else
			msg_format("%^s jerks in agony.", m_name);
#endif

		else
#ifdef JP
msg_format("%^sはピクピクひきつった。", m_name);
#else
			msg_format("%^s twitches.", m_name);
#endif

	}


	/* Birds */
	else if (my_strchr("B", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%^sはさえずった。", m_name);
#else
			msg_format("%^s chirps.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%^sはピーピー鳴いた。", m_name);
#else
			msg_format("%^s twitters.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^sはギャーギャー鳴いた。", m_name);
#else
			msg_format("%^s squawks.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^sはギャーギャー鳴きわめいた。", m_name);
#else
			msg_format("%^s chatters.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^sは苦しんだ。", m_name);
#else
			msg_format("%^s jeers.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^sはのたうち回った。", m_name);
#else
			msg_format("%^s flutters about.", m_name);
#endif

		else
#ifdef JP
msg_format("%^sはキーキーと鳴き叫んだ。", m_name);
#else
			msg_format("%^s squeaks.", m_name);
#endif

	}


	/* Dragons, Demons, High Undead */
	else if (my_strchr("duDLUW", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%sは攻撃を気にとめていない。", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%^sはしり込みした。", m_name);
#else
			msg_format("%^s flinches.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^sは痛みでシーッと鳴いた。", m_name);
#else
			msg_format("%^s hisses in pain.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^sは痛みでうなった。", m_name);
#else
			msg_format("%^s snarls with pain.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^sは痛みに吠えた。", m_name);
#else
			msg_format("%^s roars with pain.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^sは苦しげに叫んだ。", m_name);
#else
			msg_format("%^s gasps.", m_name);
#endif

		else
#ifdef JP
msg_format("%^sは弱々しくうなった。", m_name);
#else
			msg_format("%^s snarls feebly.", m_name);
#endif

	}


	/* Skeletons */
	else if (my_strchr("s", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%sは攻撃を気にとめていない。", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%sは攻撃に肩をすくめた。", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^sはカタカタと笑った。", m_name);
#else
			msg_format("%^s rattles.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^sはよろめいた。", m_name);
#else
			msg_format("%^s stumbles.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^sはカタカタ言った。", m_name);
#else
			msg_format("%^s rattles.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^sはよろめいた。", m_name);
#else
			msg_format("%^s staggers.", m_name);
#endif

		else
#ifdef JP
msg_format("%^sはガタガタ言った。", m_name);
#else
			msg_format("%^s clatters.", m_name);
#endif

	}


	/* Zombies */
	else if (my_strchr("z", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%sは攻撃を気にとめていない。", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%sは攻撃に肩をすくめた。", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^sはうめいた。", m_name);
#else
			msg_format("%^s groans.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%sは苦しげにうめいた。", m_name);
#else
			msg_format("%^s moans.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^sは躊躇した。", m_name);
#else
			msg_format("%^s hesitates.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^sはうなった。", m_name);
#else
			msg_format("%^s grunts.", m_name);
#endif

		else
#ifdef JP
msg_format("%^sはよろめいた。", m_name);
#else
			msg_format("%^s staggers.", m_name);
#endif

	}


	/* Ghosts */
	else if (my_strchr("G", r_ptr->d_char))

	{
		if (percentage > 95)
#ifdef JP
msg_format("%sは攻撃を気にとめていない。", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%sは攻撃に肩をすくめた。", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%sはうめいた。", m_name);
#else
			msg_format("%^s moans.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^sは泣きわめいた。", m_name);
#else
			msg_format("%^s wails.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^sは吠えた。", m_name);
#else
			msg_format("%^s howls.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%sは弱々しくうめいた。", m_name);
#else
			msg_format("%^s moans softly.", m_name);
#endif

		else
#ifdef JP
msg_format("%^sはかすかにうめいた。", m_name);
#else
			msg_format("%^s sighs.", m_name);
#endif

	}


	/* Dogs and Hounds */
	else if (my_strchr("CZ", r_ptr->d_char))
	{
#ifdef JP
		if (percentage > 95)
			msg_format("%^sは攻撃に肩をすくめた。", m_name);
		else if (percentage > 75)
			msg_format("%^sは痛みでうなった。", m_name);
		else if (percentage > 50)
			msg_format("%^sは痛みでキャンキャン吠えた。", m_name);
		else if (percentage > 35)
			msg_format("%^sは痛みで鳴きわめいた。", m_name);
		else if (percentage > 20)
			msg_format("%^sは苦痛のあまり鳴きわめいた。", m_name);
		else if (percentage > 10)
			msg_format("%^sは苦痛でもだえ苦しんだ。", m_name);
		else
			msg_format("%^sは弱々しく吠えた。", m_name);
#else
		if (percentage > 95)
			msg_format("%^s shrugs off the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s snarls with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s yelps in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s howls in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s howls in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s yelps feebly.", m_name);
#endif

	}

	/* One type of monsters (ignore,squeal,shriek) */
	else if (my_strchr("Xbilqrt", r_ptr->d_char))
	{
#ifdef JP
		if (percentage > 95)
			msg_format("%^sは攻撃を気にとめていない。", m_name);
		else if (percentage > 75)
			msg_format("%^sは痛みでうなった。", m_name);
		else if (percentage > 50)
			msg_format("%^sは痛みで叫んだ。", m_name);
		else if (percentage > 35)
			msg_format("%^sは痛みで絶叫した。", m_name);
		else if (percentage > 20)
			msg_format("%^sは苦痛のあまり絶叫した。", m_name);
		else if (percentage > 10)
			msg_format("%^sは苦痛でもだえ苦しんだ。", m_name);
		else
			msg_format("%^sは弱々しく叫んだ。", m_name);
#else
		if (percentage > 95)
			msg_format("%^s ignores the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s grunts with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s squeals in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s shrieks in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s shrieks in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s cries out feebly.", m_name);
#endif

	}

	/* Another type of monsters (shrug,cry,scream) */
	else
	{
#ifdef JP
		if (percentage > 95)
			msg_format("%^sは攻撃に肩をすくめた。", m_name);
		else if (percentage > 75)
			msg_format("%^sは痛みでうなった。", m_name);
		else if (percentage > 50)
			msg_format("%^sは痛みで叫んだ。", m_name);
		else if (percentage > 35)
			msg_format("%^sは痛みで絶叫した。", m_name);
		else if (percentage > 20)
			msg_format("%^sは苦痛のあまり絶叫した。", m_name);
		else if (percentage > 10)
			msg_format("%^sは苦痛でもだえ苦しんだ。", m_name);
		else
			msg_format("%^sは弱々しく叫んだ。", m_name);
#else
		if (percentage > 95)
			msg_format("%^s shrugs off the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s grunts with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s cries out in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s screams in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s screams in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s cries out feebly.", m_name);
#endif

	}
}


/*
 * Learn about an "observed" resistance.
 */
void update_smart_learn(creature_type *learner_ptr, int what)
{

	species_type *r_ptr = &species_info[learner_ptr->species_idx];


	/* Not allowed to learn */
	if (!smart_learn) return;

	/* Too stupid to learn anything */
	if (has_cf_creature(learner_ptr, CF_STUPID)) return;

	/* Not intelligent, only learn sometimes */
	if (!has_cf_creature(learner_ptr, CF_SMART) && (randint0(100) < 50)) return;


	/* XXX XXX XXX */

	/* Analyze the knowledge */
	switch (what)
	{
	case DRS_ACID:
		if (player_ptr->resist_acid) learner_ptr->smart |= (SM_RES_ACID);
		if (IS_OPPOSE_ACID(player_ptr)) learner_ptr->smart |= (SM_OPP_ACID);
		if (player_ptr->immune_acid) learner_ptr->smart |= (SM_IMM_ACID);
		break;

	case DRS_ELEC:
		if (player_ptr->resist_elec) learner_ptr->smart |= (SM_RES_ELEC);
		if (IS_OPPOSE_ELEC(player_ptr)) learner_ptr->smart |= (SM_OPP_ELEC);
		if (player_ptr->immune_elec) learner_ptr->smart |= (SM_IMM_ELEC);
		break;

	case DRS_FIRE:
		if (player_ptr->resist_fire) learner_ptr->smart |= (SM_RES_FIRE);
		if (IS_OPPOSE_FIRE(player_ptr)) learner_ptr->smart |= (SM_OPP_FIRE);
		if (player_ptr->immune_fire) learner_ptr->smart |= (SM_IMM_FIRE);
		break;

	case DRS_COLD:
		if (player_ptr->resist_cold) learner_ptr->smart |= (SM_RES_COLD);
		if (IS_OPPOSE_COLD(player_ptr)) learner_ptr->smart |= (SM_OPP_COLD);
		if (player_ptr->immune_cold) learner_ptr->smart |= (SM_IMM_COLD);
		break;

	case DRS_POIS:
		if (player_ptr->resist_pois) learner_ptr->smart |= (SM_RES_POIS);
		if (IS_OPPOSE_POIS(player_ptr)) learner_ptr->smart |= (SM_OPP_POIS);
		break;


	case DRS_NETH:
		if (player_ptr->resist_neth) learner_ptr->smart |= (SM_RES_NETH);
		break;

	case DRS_LITE:
		if (player_ptr->resist_lite) learner_ptr->smart |= (SM_RES_LITE);
		break;

	case DRS_DARK:
		if (player_ptr->resist_dark) learner_ptr->smart |= (SM_RES_DARK);
		break;

	case DRS_FEAR:
		if (player_ptr->resist_fear) learner_ptr->smart |= (SM_RES_FEAR);
		break;

	case DRS_CONF:
		if (player_ptr->resist_conf) learner_ptr->smart |= (SM_RES_CONF);
		break;

	case DRS_CHAOS:
		if (player_ptr->resist_chaos) learner_ptr->smart |= (SM_RES_CHAOS);
		break;

	case DRS_DISEN:
		if (player_ptr->resist_disen) learner_ptr->smart |= (SM_RES_DISEN);
		break;

	case DRS_BLIND:
		if (player_ptr->resist_blind) learner_ptr->smart |= (SM_RES_BLIND);
		break;

	case DRS_NEXUS:
		if (player_ptr->resist_nexus) learner_ptr->smart |= (SM_RES_NEXUS);
		break;

	case DRS_SOUND:
		if (player_ptr->resist_sound) learner_ptr->smart |= (SM_RES_SOUND);
		break;

	case DRS_SHARD:
		if (player_ptr->resist_shard) learner_ptr->smart |= (SM_RES_SHARD);
		break;

	case DRS_FREE:
		if (player_ptr->free_act) learner_ptr->smart |= (SM_IMM_FREE);
		break;

	case DRS_MANA:
		if (!player_ptr->msp) learner_ptr->smart |= (SM_IMM_MANA);
		break;

	case DRS_REFLECT:
		if (player_ptr->reflect) learner_ptr-> smart |= (SM_IMM_REFLECT);
		break;
	}
}

bool creature_place(creature_type *creature_ptr, int y, int x)
{
	/* Paranoia XXX XXX */
	if (cave[y][x].m_idx != 0) return FALSE;

	/* Save player location */
	creature_ptr->fy = y;
	creature_ptr->fx = x;

	/* Success */
	return TRUE;
}


/*
 * Drop all items carried by a monster
 */
void monster_drop_carried_objects(creature_type *m_ptr)
{
	s16b this_o_idx, next_o_idx = 0;
	object_type forge;
	object_type *o_ptr;
	object_type *q_ptr;


	/* Drop objects being carried */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Get local object */
		q_ptr = &forge;

		/* Copy the object */
		object_copy(q_ptr, o_ptr);

		/* Forget monster */
		q_ptr->held_m_idx = 0;

		/* Delete the object */
		delete_object_idx(this_o_idx);

		/* Drop it */
		(void)drop_near(q_ptr, -1, m_ptr->fy, m_ptr->fx);
	}

	/* Forget objects */
	m_ptr->hold_o_idx = 0;
}
