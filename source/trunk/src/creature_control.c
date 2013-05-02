/* File: creature2.c */

/*
* Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research,
* and not for profit purposes provided that this copyright and statement
* are included in all such copies.  Other copyrights may also apply.
*/

/* Purpose: misc code for creatures */

#include "angband.h"
#include "artifact.h"
#include "birth.h"
#include "command.h"
#include "creature_melee.h"
#include "diary.h"
#include "files.h"
#include "floors.h"
#include "quest.h"
#include "riding.h"

#define HORDE_NOGOOD 0x01
#define HORDE_NOEVIL 0x02

// Determine if a "boundary" grid is "floor mimic"
#define boundary_floor(C, F, MF) \
	((C)->mimic && permanent_wall(F) && \
	 (have_flag((MF)->flags, FF_MOVE) || have_flag((MF)->flags, FF_CAN_FLY)) && \
	 have_flag((MF)->flags, FF_PROJECT) && !have_flag((MF)->flags, FF_OPEN))

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
#ifdef JP // nuke me
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
static byte class_equipment_init[MAX_CLASS][CLASS_INIT_EQUIPMENT][2] =
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
 * Determine if a trap affects the player.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match trap power against player armor.
 */
static int check_hit(creature_type *creature_ptr, POWER power)
{
	int k, ac;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- 5% hit, 5% miss */
	if(k < 10) return (k < 5);

	if(creature_ptr->chara_idx == CHARA_NAMAKE || one_in_(20)) return TRUE;
	if(power <= 0) return FALSE;

	/* Total armor */
	ac = creature_ptr->ac + creature_ptr->to_ac;

	/* Power competes against Armor */
	if(randint1(power) > ((ac * 3) / 4)) return TRUE;

	/* Assume miss */
	return FALSE;
}


/* Handle player hitting a real trap */
void hit_trap(creature_type *creature_ptr, bool break_trap)
{
	int i, num, dam;
	COODINATES x = creature_ptr->fx, y = creature_ptr->fy;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	/* Get the cave grid */
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

	cptr name = COD_TRAP;

	disturb(player_ptr, 0, 0);
	cave_alter_feat(floor_ptr, y, x, FF_HIT_TRAP);

	switch (trap_feat_type)
	{
		case TRAP_TRAPDOOR:
			if(has_trait(creature_ptr, TRAIT_CAN_FLY))
				msg_print(MES_TRAP_DOOR_AVOID);
			else
			{
				msg_print(MES_TRAP_DOOR);
#ifdef JP
				if(has_trait(creature_ptr, TRAIT_ECHIZEN_TALK))
					msg_print("くっそ～！");
				if(has_trait(creature_ptr, TRAIT_CHARGEMAN_TALK))
					msg_print("ジュラル星人の仕業に違いない！");
#endif
				sound(SOUND_FALL);
				dam = diceroll(2, 8);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, COD_TRAP_DOOR, NULL, -1);

				/* Still alive and autosave enabled */
				if(autosave_l && (creature_ptr->chp >= 0)) do_cmd_save_game(TRUE);
				write_diary(DIARY_BUNSHOU, 0, DIARY_TRAP_DOOR);
				move_floor(creature_ptr, floor_ptr->dungeon_id, creature_ptr->wy, creature_ptr->wx, creature_ptr->depth + 1, floor_ptr, CFM_RAND_SEED | CFM_RAND_CONNECT);

				/* Leaving */
				subject_change_floor = TRUE;
			}
			break;

		case TRAP_PIT:
			if(has_trait(creature_ptr, TRAIT_CAN_FLY)) msg_print(MES_TRAP_PIT_AVOID);
			else
			{
				msg_print(MES_TRAP_PIT);
				name = COD_PIT_TRAP;
				dam = diceroll(2, 6);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;

		case TRAP_SPIKED_PIT:
			if(has_trait(creature_ptr, TRAIT_CAN_FLY)) msg_print(MES_TRAP_SPIKED_PIT_AVOID);
			else
			{
				msg_print(MES_TRAP_SPIKED_PIT);
				name = COD_PIT_TRAP;
				dam = diceroll(2, 6); // Base damage

				if(PERCENT(50)) // Extra spike damage
				{
					msg_print(MES_TRAP_INPILED);
					name = COD_SPIKED_PIT_TRAP;
					dam = dam * 2;
					(void)add_timed_trait(creature_ptr, TRAIT_CUT, randint1(dam), TRUE);
				}

				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;

		case TRAP_POISON_PIT:
		{
			if(has_trait(creature_ptr, TRAIT_CAN_FLY)) msg_print(MES_TRAP_SPIKED_PIT_AVOID);
			else
			{
				msg_print(MES_TRAP_SPIKED_PIT);
				name = COD_PIT_TRAP;
				dam = diceroll(2, 6); // Base damage

				// Extra spike damage
				if(PERCENT(50))
				{
					msg_print(MES_TRAP_POISON_INPILED);
					name = COD_SPIKED_PIT_TRAP;
					dam = dam * 2;
					(void)add_timed_trait(creature_ptr, TRAIT_CUT, randint1(dam), TRUE);

					if(!has_trait(creature_ptr, TRAIT_RES_POIS))
					{
						dam = dam * 2;
						(void)add_timed_trait(creature_ptr, TRAIT_POISONED, randint1(dam), TRUE);
					}
				}
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_NOESCAPE, dam, name, NULL, -1);
			}
			break;
		}

		case TRAP_TY_CURSE:
		{
			num = 2 + randint1(3);
			for (i = 0; i < num; i++)
				(void)summoning(0, y, x, floor_ptr->depth, 0, (PC_ALLOW_GROUP | PC_ALLOW_UNIQUE | PC_NO_PET));

			if(floor_ptr->depth > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;
				do stop_ty = activate_ty_curse(creature_ptr, stop_ty, &count);
				while (one_in_(6));
			}
			break;
		}

		case TRAP_TELEPORT:
		{
			msg_print(MES_TRAP_TELEPORT);
			teleport_creature(creature_ptr, 100, TELEPORT_PASSIVE);
			break;
		}

		case TRAP_FIRE:
		{
			msg_print(MES_TRAP_FIRE);
			dam = diceroll(4, 6);
			//TODO damage
			break;
		}

		case TRAP_ACID:
		{
			msg_print(MES_TRAP_ACID);
			dam = diceroll(4, 6);
			//TODO damage
			break;
		}

		case TRAP_SLOW:
		{
			if(check_hit(creature_ptr, 125) && !(has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1)))
			{
				msg_print(MES_TRAP_DARTS);
				dam = diceroll(1, 4);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, dam, COD_DART_TRAP, NULL, -1);
				add_timed_trait(creature_ptr, TRAIT_SLOW, randint0(20) + 20, TRUE);
			}
			else msg_print(MES_TRAP_DARTS_MISSED);
			break;
		}

		case TRAP_LOSE_STR:
		{
			if(check_hit(creature_ptr, 125))
			{
				msg_print(MES_TRAP_DARTS);
				dam = diceroll(1, 4);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, dam, COD_DART_TRAP, NULL, -1);
				if(!(has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1))) (void)do_dec_stat(creature_ptr, STAT_STR);
			}
			else
			{
				msg_print(MES_TRAP_DARTS_MISSED);
			}
			break;
		}

		case TRAP_LOSE_DEX:
		{
			if(check_hit(creature_ptr, 125))
			{
				msg_print(MES_TRAP_DARTS);
				dam = diceroll(1, 4);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, dam, COD_DART_TRAP, NULL, -1);
				if(!(has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1))) (void)do_dec_stat(creature_ptr, STAT_DEX);
			}
			else
			{
				msg_print(MES_TRAP_DARTS_MISSED);
			}
			break;
		}

		case TRAP_LOSE_CON:
		{
			if(check_hit(creature_ptr, 125))
			{
				msg_print(MES_TRAP_DARTS);
				dam = diceroll(1, 4);
				take_damage_to_creature(NULL, creature_ptr, DAMAGE_ATTACK, dam, COD_DART_TRAP, NULL, -1);
				if(!(has_trait(creature_ptr, TRAIT_MULTI_SHADOW) && (game_turn & 1))) (void)do_dec_stat(creature_ptr, STAT_CON);
			}
			else
			{
				msg_print(MES_TRAP_DARTS_MISSED);
			}
			break;
		}

		case TRAP_BLIND:
			msg_print(MES_TRAP_BLIND);
			if(!has_trait(creature_ptr, TRAIT_NO_BLIND)) (void)add_timed_trait(creature_ptr, TRAIT_BLIND, randint0(50) + 25, TRUE);
			break;

		case TRAP_CONFUSE:
			msg_print(MES_TRAP_CONFUSE);
			if(!has_trait(creature_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(creature_ptr, TRAIT_CONFUSED, randint0(20) + 10, TRUE);
			break;

		case TRAP_POISON:
			msg_print(MES_TRAP_POISON);
			if(!has_trait(creature_ptr, TRAIT_RES_POIS)) (void)add_timed_trait(creature_ptr, TRAIT_POISONED, randint0(20) + 10, TRUE);
			break;

		case TRAP_SLEEP:
			msg_print(MES_TRAP_SLEPT);
			if(!has_trait(creature_ptr, TRAIT_FREE_ACTION))
				(void)add_timed_trait(creature_ptr, TRAIT_SLEPT, randint0(10) + 5, TRUE);
			break;

		case TRAP_TRAPS:
			msg_print(MES_FEATURE_KILLED);
			project(NULL, 0, 1, y, x, 0, DO_EFFECT_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);
			break;

		case TRAP_ALARM:
			msg_print(MES_TRAP_ALARM);
			aggravate_creatures(creature_ptr);
			break;

		case TRAP_OPEN:
			msg_print(MES_TRAP_OPEN);
			(void)project(NULL, 0, 3, y, x, 0, DO_EFFECT_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(NULL, 0, 3, y, x - 4, 0, DO_EFFECT_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(NULL, 0, 3, y, x + 4, 0, DO_EFFECT_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			aggravate_creatures(creature_ptr);
			break;

		case TRAP_ARMAGEDDON:
		{
			static int levs[10] = {0, 0, 20, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;

			int lev;
			msg_print(MES_TRAP_ARMAGEDDON);

			/* Summon Demons and Angels */
			for (lev = floor_ptr->depth; lev >= 20; lev -= 1 + lev/16)
			{
				num = levs[MIN(lev/10, 9)];
				for (i = 0; i < num; i++)
				{
					COODINATES x1 = (COODINATES)rand_spread(x, 7);
					COODINATES y1 = (COODINATES)rand_spread(y, 5);

					/* Skip illegal grids */
					if(!IN_BOUNDS(floor_ptr, y1, x1)) continue;

					/* Require line of projection */
					if(!projectable(floor_ptr, MAX_RANGE, creature_ptr->fy, creature_ptr->fx, y1, x1)) continue;

					if(summoning(0, y1, x1, lev, TRAIT_S_ARMAGE_EVIL, (PC_NO_PET)))
						evil_idx = hack_m_idx_ii;

					if(summoning(0, y1, x1, lev, TRAIT_S_ARMAGE_GOOD, (PC_NO_PET)))
						good_idx = hack_m_idx_ii;

					/* Let them fight each other */
					if(evil_idx && good_idx)
					{
						creature_type *evil_ptr = &creature_list[evil_idx];
						creature_type *good_ptr = &creature_list[good_idx];
						evil_ptr->target_y = good_ptr->fy;
						evil_ptr->target_x = good_ptr->fx;
						good_ptr->target_y = evil_ptr->fy;
						good_ptr->target_x = evil_ptr->fx;
					}
				}
			}
			break;
		}

		case TRAP_PIRANHA:
			msg_print(MES_TRAP_PIRANHAS);
			/* Water fills room */
			cast_ball_hide(creature_ptr, DO_EFFECT_WATER_FLOW, MAX_RANGE_SUB, 1, 10);
			/* Summon Piranhas */
			num = 1 + floor_ptr->depth/20;
			for (i = 0; i < num; i++) (void)summoning(0, y, x, floor_ptr->depth, TRAIT_S_PIRANHAS, (PC_ALLOW_GROUP | PC_NO_PET));
			break;

		case TRAP_ACID_FLOW:
			msg_print(MES_TRAP_ACID_FLOW);
			cast_ball_hide(creature_ptr, DO_EFFECT_ACID_FLOW, MAX_RANGE_SUB, 1, 10);
			break;

		case TRAP_POISON_FLOW:
			msg_print(MES_TRAP_POISON_FLOW);
			cast_ball_hide(creature_ptr, DO_EFFECT_POISON_FLOW, MAX_RANGE_SUB, 1, 10);
			break;
	}

	if(break_trap && is_trap(c_ptr->feat))
	{
		cave_alter_feat(floor_ptr, y, x, FF_DISARM);
		msg_print(MES_TRAP_DESTROY);
	}
}


static void exit_area(creature_type *creature_ptr, COODINATES x, COODINATES y)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];

	if(!floor_ptr->depth && !floor_ptr->global_map && ((x == 0) || (x == MAX_WID - 1) || (y == 0) || (y == MAX_HGT - 1)))
	{
		COODINATES tmp_wx = 0, tmp_wy = 0, tmp_px = 0, tmp_py = 0;

		// Can the player enter the grid?//
		if(c_ptr->mimic && creature_can_cross_terrain(creature_ptr, c_ptr->mimic, 0))
		{
			/* Hack: move to new area */
			if((y == 0) && (x == 0))
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = floor_ptr->height - 2;
				tmp_px = floor_ptr->width - 2;
			}

			else if((y == 0) && (x == MAX_WID - 1))
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = floor_ptr->height - 2;
				tmp_px = 1;
			}

			else if((y == MAX_HGT - 1) && (x == 0))
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = 1;
				tmp_px = floor_ptr->width - 2;
			}

			else if((y == MAX_HGT - 1) && (x == MAX_WID - 1))
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = 1;
				tmp_px = 1;
			}

			else if(y == 0)
			{
				tmp_wy = creature_ptr->wy - 1;
				tmp_wx = creature_ptr->wx;
				tmp_py = floor_ptr->height - 2;
				tmp_px = x;
			}

			else if(y == MAX_HGT - 1)
			{
				tmp_wy = creature_ptr->wy + 1;
				tmp_wx = creature_ptr->wx;
				tmp_py = 1;
				tmp_px = x;
			}

			else if(x == 0)
			{
				tmp_wy = creature_ptr->wy;
				tmp_wx = creature_ptr->wx - 1;
				tmp_py = y;
				tmp_px = floor_ptr->width - 2;
			}

			else if(x == MAX_WID - 1)
			{
				tmp_wy = creature_ptr->wy;
				tmp_wx = creature_ptr->wx + 1;
				tmp_py = y;
				tmp_px = 1;
			}

			if(wilderness[tmp_wy][tmp_wx].terrain != TERRAIN_CHAOS)
			{
				creature_ptr->wy = tmp_wy;
				creature_ptr->wx = tmp_wx;
				creature_ptr->oldpy = tmp_py;
				creature_ptr->oldpx = tmp_px;
				subject_change_floor = TRUE;

				reveal_wilderness(tmp_wy, tmp_wx);
				cost_tactical_energy(creature_ptr, 100);
				return;
			}
			else if(get_check(MES_FIELD_ASK_CHAOS))
			{
				creature_ptr->wy = tmp_wy;
				creature_ptr->wx = tmp_wx;
				creature_ptr->oldpy = tmp_py;
				creature_ptr->oldpx = tmp_px;
				subject_change_floor = TRUE;
				reveal_wilderness(tmp_wy, tmp_wx);
				cost_tactical_energy(creature_ptr, 100);
				return;
			}

			//cancel_tactical_action(creature_ptr);
			//oktomove = FALSE;

		}

		/* "Blocked" message appears later */
		//can_enter = FALSE;
	}
}

static void plus_move_cost(creature_type *creature_ptr, int x, int y)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	creature_type *steed_ptr = &creature_list[creature_ptr->riding];
	species_type *riding_r_ptr = &species_info[creature_ptr->riding ? steed_ptr->species_idx : 0]; 

	bool can_kill_walls = has_trait(creature_ptr, TRAIT_KILL_WALL) && have_flag(f_ptr->flags, FF_HURT_DISI) &&
		(!have_flag(f_ptr->flags, FF_LOS)) && !have_flag(f_ptr->flags, FF_PERMANENT);

	cost_tactical_energy(creature_ptr, 100);

	if(have_flag(f_ptr->flags, FF_TREE) && !can_kill_walls)
		if((creature_ptr->class_idx != CLASS_RANGER) && !has_trait(creature_ptr, TRAIT_CAN_FLY) &&
			(!creature_ptr->riding || !has_trait_species(riding_r_ptr, TRAIT_WILD_WOOD)))
				cost_tactical_energy(creature_ptr, 30);
}


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
*  Extract creature race pointer of a creature's true form
*/
species_type *real_species_ptr(creature_type *m_ptr)
{
	species_type *species_ptr = &species_info[m_ptr->species_idx];
	/* Extract real race */
	if(m_ptr->sc_flag2 & SC_FLAG2_CHAMELEON)
	{
		if(has_trait_species(species_ptr, TRAIT_UNIQUE))
			return &species_info[SPECIES_CHAMELEON_K];
		else
			return &species_info[SPECIES_CHAMELEON];
	}
	else
	{
		return species_ptr;
	}
}


/*
* Delete a creature by index.
*
* When a creature is deleted, all of its objects are deleted.
*/
void delete_species_idx(creature_type *creature_ptr)
{
	int x, y;
	floor_type *floor_ptr = &floor_list[creature_ptr->floor_idx];

	// Get location
	y = creature_ptr->fy;
	x = creature_ptr->fx;

	real_species_ptr(creature_ptr)->cur_num--;	// Hack -- Reduce the racial counter
	if(has_trait(creature_ptr, TRAIT_MULTIPLY)) floor_ptr->num_repro--;	// Hack -- count the number of "reproducers"

	reset_timed_trait(creature_ptr);

	if(creature_ptr == &creature_list[npc_status_id]) health_track(0);	// Hack -- remove tracked creature

	if(&creature_list[pet_t_m_idx] == creature_ptr) pet_t_m_idx = 0;
	if(&creature_list[riding_t_m_idx] == creature_ptr) riding_t_m_idx = 0;
	if(&creature_list[creature_ptr->riding] == creature_ptr) creature_ptr->riding = 0;

	floor_ptr->cave[y][x].creature_idx = 0;	// Creature is gone

	if(is_pet(player_ptr, creature_ptr)) check_pets_num_and_align(player_ptr, creature_ptr, FALSE);

	(void)WIPE(creature_ptr, creature_type);	// Wipe the Creature
	creature_cnt--;	// Count creatures

	lite_spot(floor_ptr, y, x);	// Visual update
	if(is_lighting_creature(creature_ptr) || is_darken_creature(creature_ptr)) prepare_update(creature_ptr, PU_SPECIES_LITE);	// Update some things
}


/*
* Delete the creature, if any, at a given location
*/
void delete_creature(floor_type *floor_ptr, int y, int x)
{
	cave_type *c_ptr;


	if(!IN_BOUNDS(floor_ptr, y, x)) return;

	/* Check the grid */
	c_ptr = &floor_ptr->cave[y][x];

	/* Delete the creature (if any) */
	if(c_ptr->creature_idx) delete_species_idx(&creature_list[c_ptr->creature_idx]);
}


/*
 * Move an object from index i1 to index i2 in the object list
 */
static void move_creature_object(int to, int from)
{
	creature_list[to] = creature_list[from];
	creature_wipe(&creature_list[from]);
}


/*
* Compact and Reorder the creature list
*
* This function can be very dangerous, use with caution!
*
* When actually "compacting" creatures, we base the saving throw
* on a combination of creature level, distance from player, and
* current "desperation".
*
* After "compacting" (if needed), we "reorder" the creatures into a more
* compact order, and we reset the allocation info, and the "live" array.
*/
void compact_creatures(int size)
{
	int		i, num, cnt;
	int		cur_lev, cur_dis, chance;

	/* Message (only if compacting) */
#ifdef JP
	if(size) msg_print("クリーチャー情報を圧縮しています...");
#else
	if(size) msg_print("Compacting creatures...");
#endif

	/* Compact at least 'size' objects */
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Check all the creatures */
		for (i = 1; i < creature_max; i++)
		{
			creature_type *m_ptr = &creature_list[i];
			species_type *species_ptr = &species_info[m_ptr->species_idx];
			if(!is_valid_creature(m_ptr)) continue;

			/* Hack -- High level creatures start out "immune" */
			if(species_ptr->level > cur_lev) continue;

			if(creature_list[i].ridden || creature_list[i].riding) continue;

			/* Ignore nearby creatures */
			if((cur_dis > 0) && (m_ptr->cdis < cur_dis)) continue;

			/* Saving throw chance */
			chance = 90;

			/* Only compact "Quest" Creatures in emergencies */
			if((has_trait_species(species_ptr, TRAIT_QUESTOR)) && (cnt < 1000)) chance = 100;

			/* Try not to compact Unique Creatures */
			if(has_trait_species(species_ptr, TRAIT_UNIQUE)) chance = 100;

			/* All creatures get a saving throw */
			if(PERCENT(chance)) continue;

			if(record_named_pet && is_pet(player_ptr, m_ptr) && m_ptr->nickname)
			{
				char m_name[MAX_NLEN];
				creature_desc(m_name, m_ptr, CD_INDEF_VISIBLE);
				write_diary(DIARY_NAMED_PET, RECORD_NAMED_PET_COMPACT, m_name);
			}

			/* Delete the creature */
			delete_species_idx(m_ptr);

			/* Count the creature */
			num++;
		}
	}

	/* Excise dead creatures (backwards!) */
	for (i = creature_max - 1; i >= 1; i--)
	{
		/* Get the i'th creature */
		creature_type *m_ptr = &creature_list[i];

		/* Skip real creatures */
		if(is_valid_creature(m_ptr)) continue;

		/* Move last creature into open hole */
		move_creature_object(creature_max - 1, i);

		/* Compress "creature_max" */
		creature_max--;
	}
}

void birth_uniques(void)
{
	int i;
	char buf[80];

	// Init Unique Count
	unique_max = 0;

	for(i = 0; i < max_species_idx; i++)
	{
		sprintf(buf, "Please Wait ... Unique Making[%d]", i);
		prt(buf, 0, 0);
		Term_fresh();

		if(has_trait_species(&species_info[i], TRAIT_UNIQUE))
		{
			unique_max++;
			generate_creature(NULL, i, GC_AUTO);
		}
	}

}


/*
* Delete/Remove all the creatures when the player leaves the level
*
* This is an efficient method of simulating multiple calls to the
* "delete_creature()" function, with no visual effects.
*/
void wipe_creature_list(FLOOR_ID floor_idx)
{
	int i;

	/* Hack -- if Banor or Lupart dies, stay another dead */
	if(!species_info[SPECIES_BANORLUPART].max_num)
	{
		if(species_info[SPECIES_BANOR].max_num)
		{
			species_info[SPECIES_BANOR].max_num = 0;
			species_info[SPECIES_BANOR].r_pkills++;
			species_info[SPECIES_BANOR].r_akills++;
			if(species_info[SPECIES_BANOR].r_tkills < MAX_SHORT) species_info[SPECIES_BANOR].r_tkills++;
		}
		if(species_info[SPECIES_LUPART].max_num)
		{
			species_info[SPECIES_LUPART].max_num = 0;
			species_info[SPECIES_LUPART].r_pkills++;
			species_info[SPECIES_LUPART].r_akills++;
			if(species_info[SPECIES_LUPART].r_tkills < MAX_SHORT) species_info[SPECIES_LUPART].r_tkills++;
		}
	}

	/* Delete all the creatures */
	for (i = creature_max - 1; i >= 1; i--)
	{
		creature_type *creature_ptr = &creature_list[i];
		floor_type *floor_ptr = &floor_list[creature_ptr->floor_idx];
		if(!creature_ptr->species_idx) continue; // Skip dead creature
		if(floor_idx && creature_ptr->floor_idx != floor_idx) continue; // Skip other floor  creature
		floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].creature_idx = 0; // Creature is gone
		(void)WIPE(creature_ptr, creature_type); // Wipe the Creature
	}

	/*
	* Wiping racial counters of all creatures and incrementing of racial
	* counters of creatures in party_mon[] are required to prevent multiple
	* generation of unique creature who is the minion of player.
	*/

	/* Hack -- Wipe the racial counter of all creature races */
	for (i = 1; i < max_species_idx; i++) species_info[i].cur_num = 0;

	/* Reset "creature_max" and "creature_cnt" */
	creature_max = 1;
	creature_cnt = 0;

	/* Hack -- no more target */
	pet_t_m_idx = 0;
	riding_t_m_idx = 0;

	/* Hack -- no more tracking */
	health_track(0);
}

// Acquires and returns the index of a "free" creature.
// This routine should almost never fail, but it *can* happen.
CREATURE_ID creature_pop(void)
{
	CREATURE_ID i;
	creature_type *creature_ptr;

	for (i = 1; i < creature_max; i++) // Recycle dead creatures
	{
		creature_ptr = &creature_list[i]; // Acquire creature
		if(creature_ptr->species_idx || has_trait(creature_ptr, TRAIT_UNIQUE) || is_player(creature_ptr))
			continue; // Skip live creatures and player

		creature_cnt++; // Count creature and return using id
		return i;
	}

	if(creature_max < max_creature_idx) // Normal allocation
	{
		i = creature_max; // Access the next hole

		// Expand the array and count creature
		creature_max++;
		creature_cnt++;
		return i;

		// Return the index
	}

	msg_warning("クリーチャーが多すぎる！");

	return 0; // Try not to crash
}


static void set_species_list_bias_summoning(PROB **prob_list_ptr, TRAIT_ID summoning_type)
{
	int n;
	species_type *species_ptr;
	PROB *prob_list = *prob_list_ptr;
	bool okay;
	int i;

	for(n = 0; n < max_species_idx; n++)
	{
		species_ptr = &species_info[n];

		// Check our requirements
		switch (summoning_type)
		{
		case TRAIT_S_ANT:
			if(!IS_RACE(species_ptr, RACE_ANT)) prob_list[n] = 0;
			break;

		case TRAIT_S_SPIDER:
			if(!IS_RACE(species_ptr, RACE_SPIDER)) prob_list[n] = 0;
			break;

		case TRAIT_S_HOUND:
			if(!IS_RACE(species_ptr, RACE_CANINE) && !IS_RACE(species_ptr, RACE_ZEPHYR_HOUND)) prob_list[n] = 0;
			break;

		case TRAIT_S_HYDRA:
			if(!IS_RACE(species_ptr, RACE_HYDRA)) prob_list[n] = 0;
			break;

		case TRAIT_S_ANGEL:
			if(!IS_RACE(species_ptr, RACE_ANGEL)) prob_list[n] = 0;
			break;

		case TRAIT_S_DEMON:
		case TRAIT_S_HI_DEMON:
			if(!has_trait_species(species_ptr, TRAIT_DEMON)) prob_list[n] = 0;
			break;

		case TRAIT_S_UNDEAD:
		case TRAIT_S_HI_UNDEAD:
			if(!has_trait_species(species_ptr, TRAIT_UNDEAD)) prob_list[n] = 0;
			break;

		case TRAIT_S_DRAGON:
		case TRAIT_S_HI_DRAGON:
			if(!has_trait_species(species_ptr, TRAIT_DRAGON)) prob_list[n] = 0;
			break;

		case TRAIT_S_AMBERITES:
			if(!IS_RACE(species_ptr, RACE_AMBERITE)) prob_list[n] = 0;
			break;

		case TRAIT_S_UNIQUE:
			if(!has_trait_species(species_ptr, TRAIT_UNIQUE)) prob_list[n] = 0;
			break;

		case TRAIT_S_MOLD:
			if(!IS_RACE(species_ptr, RACE_MOLD)) prob_list[n] = 0;
			break;

		case TRAIT_S_BAT:
			if(!IS_RACE(species_ptr, RACE_BAT)) prob_list[n] = 0;
			break;

		case TRAIT_S_QUYLTHULG:
			if(!IS_RACE(species_ptr, RACE_QUYLTHLUG)) prob_list[n] = 0;
			okay = IS_RACE(species_ptr, RACE_QUYLTHLUG);
			break;

		case TRAIT_S_CREEPING_COIN:
			if(!IS_RACE(species_ptr, RACE_MIMIC) || !(species_ptr->d_char == '$')) prob_list[n] = 0;
			break;

		case TRAIT_S_MIMIC:
			if(!IS_RACE(species_ptr, RACE_MIMIC)) prob_list[n] = 0;
			break;

		case TRAIT_S_GOLEM:
			if(!has_trait_species(species_ptr, TRAIT_GOLEM)) prob_list[n] = 0;
			break;

		case TRAIT_S_CYBER:
			if(!has_trait_species(species_ptr, TRAIT_GOLEM) || has_trait_raw(&species_ptr->flags, TRAIT_ROCKET)) prob_list[n] = 0;
			break;

		case TRAIT_S_KIN:
			//TODO okay = IS_RACE(species_ptr, species_ptr->race_idx1) || IS_RACE(species_ptr, species_ptr->race_idx2);
			break;

		case TRAIT_S_DAWN_LEGION:
			if(species_ptr->species_idx != SPECIES_DAWN) prob_list[n] = 0;
			break;

		case TRAIT_S_ANIMAL:
			if(!has_trait_species(species_ptr, TRAIT_ANIMAL)) prob_list[n] = 0;
			break;

		case TRAIT_S_ANIMAL_RANGER:
			if(!has_trait_species(species_ptr, TRAIT_ANIMAL)) prob_list[n] = 0;
			if(!my_strchr("abcflqrwBCHIJKMRS", species_ptr->d_char)) prob_list[n] = 0;
			if(has_trait_species(species_ptr, TRAIT_DRAGON)) prob_list[n] = 0;
			if(has_trait_species(species_ptr, TRAIT_UNDEAD)) prob_list[n] = 0;
			if(has_trait_species(species_ptr, TRAIT_DEMON)) prob_list[n] = 0;
			if(has_trait_species(species_ptr, TRAIT_MULTIPLY)) prob_list[n] = 0;
			break;

		case TRAIT_S_HI_DRAGON_LIVING:
			if(!has_trait_species(species_ptr, TRAIT_DRAGON)) prob_list[n] = 0;
			if(!species_living(species_ptr)) prob_list[n] = 0;
			break;

		case TRAIT_S_LIVING:
			if(!has_trait_species(species_ptr, TRAIT_DRAGON)) prob_list[n] = 0; 
			break;

		case TRAIT_S_PHANTOM:
			if(!species_ptr->species_idx != SPECIES_PHANTOM_B && !species_ptr->species_idx != SPECIES_PHANTOM_W);
			break;

		case TRAIT_S_BLUE_HORROR:
			if(species_ptr->species_idx != SPECIES_BLUE_HORROR) prob_list[n] = 0;
			break;

		case TRAIT_S_ELEMENTAL:
			if(!IS_RACE(species_ptr, RACE_ELEMENTAL)) prob_list[n] = 0;
			break;

		case TRAIT_S_VORTEX:
			if(!IS_RACE(species_ptr, RACE_VORTEX)) prob_list[n] = 0;
			break;

		case TRAIT_S_HYBRID:
			if(species_ptr->d_char == 'H');
			break;

		case TRAIT_S_BIRD:
			if(!IS_RACE(species_ptr, RACE_BIRD)) prob_list[n] = 0;
			break;

		case TRAIT_S_KAMIKAZE:
			for (i = 0; i < MAX_SPECIAL_BLOWS; i++)
				if(species_ptr->blow[i].method == RBM_EXPLODE) okay = TRUE;
			break;

		case TRAIT_S_KAMIKAZE_LIVING:
			for (i = 0; i < MAX_SPECIAL_BLOWS; i++)
				if(species_ptr->blow[i].method == RBM_EXPLODE) okay = TRUE;
			okay = (okay && species_living(species_ptr));
			break;

		case TRAIT_S_MANES:
			if(species_ptr->species_idx != SPECIES_MANES) prob_list[n] = 0;
			break;

		case TRAIT_S_LOUSE:
			if(species_ptr->species_idx != SPECIES_LOUSE) prob_list[n] = 0;
			break;

		case TRAIT_S_GUARDIANS:
			if(has_trait_species(species_ptr, TRAIT_GUARDIAN)) prob_list[n] = 0;
			break;

		case TRAIT_S_KNIGHTS:
			if(species_ptr->class_idx != CLASS_PALADIN) prob_list[n] = 0;
			break;

		case TRAIT_S_EAGLES:
			if(!IS_RACE(species_ptr, RACE_BIRD)) prob_list[n] = 0;
			if(!has_trait_species(species_ptr, TRAIT_WILD_MOUNTAIN)) prob_list[n] = 0;
			if(!has_trait_species(species_ptr, TRAIT_WILD_ONLY)) prob_list[n] = 0;
			break;

		case TRAIT_S_ARMAGE_GOOD:
			if(!IS_RACE(species_ptr, RACE_ANGEL)) prob_list[n] = 0;
			if(!is_enemy_of_evil_species(species_ptr)) prob_list[n] = 0;
			break;

		case TRAIT_S_ARMAGE_EVIL:
			if(!IS_RACE(species_ptr, RACE_DEMON)) prob_list[n] = 0;
			if(!is_enemy_of_good_species(species_ptr)) prob_list[n] = 0;
			break;
		}

	}
}


static int chameleon_change_m_idx = 0;

// Some dungeon types restrict the possible creatures.
// Return TRUE is the creature is OK and FALSE otherwise
static bool restrict_creature_to_dungeon(SPECIES_ID species_idx)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(player_ptr);
	dungeon_type *d_ptr = &dungeon_info[floor_ptr->dungeon_id];
	species_type *species_ptr = &species_info[species_idx];
	//byte a;

	if(d_ptr->flags1 & DF1_CHAMELEON) if(chameleon_change_m_idx) return TRUE;

	if(d_ptr->flags1 & DF1_NO_MAGIC)
	{
		if(species_idx != SPECIES_CHAMELEON && species_ptr->freq_spell && has_non_magic_skill_flags(&species_ptr->flags)) return FALSE;
	}

	if(d_ptr->flags1 & DF1_NO_MELEE)
	{
		if(species_idx == SPECIES_CHAMELEON) return TRUE;
		if(!(has_bolt_flags(&species_ptr->flags) || has_beam_flags(&species_ptr->flags) || has_ball_flags(&species_ptr->flags) || 
			has_trait_raw(&species_ptr->flags, TRAIT_CAUSE_1) || 
			has_trait_raw(&species_ptr->flags, TRAIT_CAUSE_2) || 
			has_trait_raw(&species_ptr->flags, TRAIT_CAUSE_3) || 
			has_trait_raw(&species_ptr->flags, TRAIT_CAUSE_4) || 
			has_trait_raw(&species_ptr->flags, TRAIT_MIND_BLAST) || 
			has_trait_raw(&species_ptr->flags, TRAIT_BRAIN_SMASH)))
			return FALSE;
	}
	if(d_ptr->flags1 & DF1_BEGINNER)
	{
		if(species_ptr->level > floor_ptr->depth)
			return FALSE;
	}

	if(d_ptr->special_div >= 64) return TRUE;

	//TODO if(summoning_type && !(d_ptr->flags1 & DF1_CHAMELEON)) return TRUE;

	/* TODO
	switch (d_ptr->mode)
	{
	case DUNGEON_MODE_AND:
	for (a = 0; a < MAX_TRAITS; a++)
	{
	if(has_trait_raw(&d_ptr->c_flags, a)
	&& !has_trait_raw(&species_ptr->flags, a)) return FALSE; 
	}
	for (a = 0; a < 5; a++)
	if(d_ptr->r_char[a] && (d_ptr->r_char[a] != species_ptr->d_char)) return FALSE;

	return TRUE;

	case DUNGEON_MODE_NAND:
	for (a = 0; a < MAX_TRAITS; a++)
	{
	if(has_trait_raw(&d_ptr->c_flags, a)
	&& !has_trait_raw(&species_ptr->flags, a)) return TRUE; 
	}
	for (a = 0; a < 5; a++)
	if(d_ptr->r_char[a] && (d_ptr->r_char[a] != species_ptr->d_char)) return TRUE;

	return FALSE;

	case DUNGEON_MODE_OR:
	for (a = 0; a < MAX_TRAITS; a++)
	{
	if(has_trait_raw(&d_ptr->c_flags, a)
	&& has_trait_raw(&species_ptr->flags, a)) return TRUE; 
	}
	for (a = 0; a < 5; a++)
	if(d_ptr->r_char[a] == species_ptr->d_char) return TRUE;

	return FALSE;

	case DUNGEON_MODE_NOR:
	for (a = 0; a < MAX_TRAITS; a++)
	{
	if(has_trait_raw(&d_ptr->c_flags, a)
	&& has_trait_raw(&species_ptr->flags, a)) return FALSE; 
	}
	for (a = 0; a < 5; a++)
	if(d_ptr->r_char[a] == species_ptr->d_char) return FALSE;

	return TRUE;
	}
	*/

	return TRUE;
}

void reset_species_preps(void)
{
	int i;
	for (i = 0; i < alloc_species_size; i++) // Scan the allocation table
	{
		alloc_entry *entry = &alloc_species_table[i];	// Get the entry
		entry->prob2 = entry->prob1; // Accept this creature
	}
}

void alloc_species_list(PROB **prob_list_ptr)
{
	int id;
	species_type *species_ptr;
	PROB *prob_list;
	C_MAKE(*prob_list_ptr, max_species_idx, PROB);

	prob_list = *prob_list_ptr;

	for(id = 0; id < max_species_idx; id++)
	{
		species_ptr = &species_info[id];
		prob_list[id] = (species_ptr->rarity != 0 ? 10000 / species_ptr->rarity : 0);
		if(has_trait_species(species_ptr, TRAIT_QUESTOR)) prob_list[id] = 0;
		if(has_trait_species(species_ptr, TRAIT_GUARDIAN)) prob_list[id] = 0;
	}
	return;
}

void forbid_species_list(PROB **prob_list_ptr, bool (*hook_func)(SPECIES_ID species_idx))
{
	int n;
	PROB *prob_list = *prob_list_ptr;
	for(n = 0; n < max_species_idx; n++) if(!hook_func(n)) prob_list[n] = 0;
	return;
}

void free_species_list(PROB **prob_list_ptr)
{
	C_KILL(*prob_list_ptr, max_species_idx, PROB);
}


static int mysqrt(int n)
{
	int tmp = n >> 1;
	int tasu = 10;
	int kaeriti = 1;

	if(!tmp)
	{
		if(n) return 1;
		else return 0;
	}

	while(tmp)
	{
		if((n/tmp) < tmp)
		{
			tmp >>= 1;
		}
		else break;
	}
	kaeriti = tmp;
	while(tasu)
	{
		if((n/tmp) < tmp)
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
* Choose a creature race that seems "appropriate" to the given level
*
* This function uses the "prob2" field of the "creature allocation table",
* and various local information, to calculate the "prob3" field of the
* same table, which is then used to choose an "appropriate" creature, in
* a relatively efficient manner.
*
* Note that "town" creatures will *only* be created in the town, and
* "normal" creatures will *never* be created in the town, unless the
* "level" is "modified", for example, by polymorph or summoning.
*
* There is a small chance (1/50) of "boosting" the given depth by
* a small amount (up to four levels), except in the town.
*
* It is (slightly) more likely to acquire a creature of the given level
* than one of a lower level.  This is done by choosing several creatures
* appropriate to the given level and keeping the "hardest" one.
*
* Note that if no creatures are "appropriate", then this function will
* fail, and return zero, but this should *almost* never happen.
*/
SPECIES_ID get_species_num(floor_type *floor_ptr, FLOOR_LEV level)
{
	int i;
	SPECIES_ID species_idx;
	//TODO int p, j;
	long value, total;

	species_type *species_ptr;
	alloc_entry *table = alloc_species_table;

	int pls_kakuritu, pls_level;
	int hoge = mysqrt(level * 10000L);

	if(level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;
	pls_kakuritu = MAX(NASTY_ENEMY_MAX, NASTY_ENEMY_BASE - ((floor_ptr->floor_turn / (TURNS_PER_TICK * 2500L) - hoge / 10)));
	pls_level = MIN(NASTY_ENEMY_PLUS_MIN, 3 + floor_ptr->floor_turn / (TURNS_PER_TICK * 20000L) - hoge / 40 + MIN(5, level/10));

	if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_MAZE)
	{
		pls_kakuritu = MIN(pls_kakuritu / 2, pls_kakuritu - 10);
		if(pls_kakuritu < 2) pls_kakuritu = 2;
		pls_level += 2;
		level += 3;
	}

	// Boost the level
	if(!floor_ptr->gamble_arena_mode && !(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_BEGINNER))
	{
		// Nightmare mode allows more out-of depth creatures
		if(has_trait(player_ptr, TRAIT_CURSE_OF_ILUVATAR) && !randint0(pls_kakuritu))
			level = 1 + (level * MAX_DEPTH / randint1(MAX_DEPTH));
		else
			if(!randint0(pls_kakuritu)) level += pls_level;
	}

	total = 0L; // Reset total

	// Process probabilities
	for (i = 0; i < alloc_species_size; i++)
	{
		if(table[i].level > level) break;			// Creatures are sorted by depth
		table[i].prob3 = 0;							// Default
		species_idx = table[i].index;				// Access the "species_idx" of the chosen creature
		species_ptr = &species_info[species_idx];			// Access the actual race
		if(has_trait_species(species_ptr, TRAIT_CITIZEN)) continue;	// Citizens doesn't wander.

		if(!floor_ptr->gamble_arena_mode && !chameleon_change_m_idx)
		{
			// Hack -- "unique" creatures must be "unique"
			if((has_trait_species(species_ptr, TRAIT_UNIQUE)) && (species_ptr->cur_num >= species_ptr->max_num)) continue;
			if(has_trait_species(species_ptr, TRAIT_UNIQUE2) && (species_ptr->cur_num >= 1)) continue;
			if(species_idx == SPECIES_BANORLUPART)
			{
				if(species_info[SPECIES_BANOR].cur_num > 0) continue;
				if(species_info[SPECIES_LUPART].cur_num > 0) continue;
			}
		}
		table[i].prob3 = table[i].prob2; // Accept
		total += table[i].prob3; // Total
	}

	if(total <= 0)
	{
		msg_warning("No selection in get_species_num()");
		return SUCCESS; // No legal creatures
	}

	value = randint0(total); // Pick a creature

	for (i = 0; i < alloc_species_size; i++) // Find the creature
	{
		if(value < table[i].prob3) break; // Found the entry
		value = value - table[i].prob3; // Decrement
	}

	/*
	p = randint0(100); // Power boost

	if(p < 60) // Try for a "harder" creature once (50%) or twice (10%)
	{
	j = i; // Save old
	value = randint0(total); // Pick a creature
	for (i = 0; i < alloc_species_size; i++) // Find the creature
	{
	if(value < table[i].prob3) break;	// Found the entry
	value = value - table[i].prob3;		// Decrement
	}

	// Keep the "best" one
	if(table[i].level < table[j].level) i = j;
	}

	if(p < 10) // Try for a "harder" creature twice (10%)
	{
	j = i; // Save old
	value = randint0(total); // Pick a creature
	for (i = 0; i < alloc_species_size; i++) // Find the creature
	{
	if(value < table[i].prob3) break; // Found the entry
	value = value - table[i].prob3; // Decrement
	}

	if(table[i].level < table[j].level) i = j; // Keep the "best" one
	}
	*/

	return (table[i].index); // Result
}





/*
* Build a string describing a creature in some way.
*
* We can correctly describe creatures based on their visibility.
* We can force all creatures to be treated as visible or invisible.
* We can build nominatives, objectives, possessives, or reflexives.
* We can selectively pronominalize hidden, visible, or all creatures.
* We can use definite or indefinite descriptions for hidden creatures.
* We can use definite or indefinite descriptions for visible creatures.
*
* Pronominalization involves the gender whenever possible and allowed,
* so that by cleverly requesting pronominalization / visibility, you
* can get messages like "You hit someone.  She screams in agony!".
*
* Reflexives are acquired by requesting Objective plus Possessive.
*
* If no m_ptr arg is given (?), the creature is assumed to be hidden,
* unless the "Assume Visible" mode is requested.
*
* If no species_ptr arg is given, it is extracted from m_ptr and species_info
* If neither m_ptr nor species_ptr is given, the creature is assumed to
* be neuter, singular, and hidden (unless "Assume Visible" is set),
* in which case you may be in trouble... :-)
*
* I am assuming that no creature name is more than 70 characters long,
* so that "char desc[80];" is sufficiently large for any result.
*
* Mode Flags:
*  CD_OBJECTIVE       --> Objective (or Reflexive)
*  CD_POSSESSIVE      --> Possessive (or Reflexive)
*  CD_INDEF_HIDDEN    --> Use indefinites for hidden creatures ("something")
*  CD_INDEF_VISIBLE   --> Use indefinites for visible creatures ("a kobold")
*  CD_PRON_HIDDEN     --> Pronominalize hidden creatures
*  CD_PRON_VISIBLE    --> Pronominalize visible creatures
*  CD_ASSUME_HIDDEN   --> Assume the creature is hidden
*  CD_ASSUME_VISIBLE  --> Assume the creature is visible
*  CD_TRUE_NAME       --> Chameleon's true name
*  CD_IGNORE_HALLU    --> Ignore hallucination, and penetrate shape change
*  CD_IGNORE_EGO_DESC --> Ignore Ego description
*
* Useful Modes:
*  0x00 --> Full nominative name ("the kobold") or "it"
*  CD_INDEF_HIDDEN --> Full nominative name ("the kobold") or "something"
*  CD_ASSUME_VISIBLE --> Genocide resistance name ("the kobold")
*  CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE --> Killing name ("a kobold")
*  CD_PRON_VISIBLE | CD_POSSESSIVE
*    --> Possessive, genderized if visable ("his") or "its"
*  CD_PRON_VISIBLE | CD_POSSESSIVE | CD_OBJECTIVE
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

	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);

	desc[0] = '\0';

	if(!creature_ptr) return;
	if(is_player(creature_ptr))
	{
		(void)strcpy(desc, KW_YOU);
		return;
	}

	species_ptr = &species_info[creature_ptr->ap_species_idx];

	/* Mode of CD_TRUE_NAME will reveal Chameleon's true name */
	if(mode & CD_TRUE_NAME) name = (species_name + real_species_ptr(creature_ptr)->name);
	else name = (species_name + species_ptr->name);

	/* Are we hallucinating? (Idea from Nethack...) */
	if(has_trait(player_ptr, TRAIT_HALLUCINATION) && !(mode & CD_IGNORE_HALLU))
	{
		if(one_in_(2))
		{
			if(!get_rnd_line(TEXT_FILES_SILLY, creature_ptr->species_idx, silly_name))
				named = TRUE;
		}

		if(!named)
		{
			species_type *hallu_race;

			do
			{
				hallu_race = &species_info[randint1(max_species_idx - 1)];
			}
			while (!hallu_race->name || has_trait_species(hallu_race, TRAIT_UNIQUE));

			strcpy(silly_name, (species_name + hallu_race->name));
		}

		/* Better not strcpy it, or we could corrupt species_info... */
		name = silly_name;
	}


	/* Can we "see" it (exists + forced, or visible + not unforced) */
	seen = (creature_ptr && ((mode & CD_ASSUME_VISIBLE) || (!(mode & CD_ASSUME_HIDDEN) && creature_ptr->see_others)));

	/* Sexed Pronouns (seen and allowed, or unseen and allowed) */
	pron = (creature_ptr && ((seen && (mode & CD_PRON_VISIBLE)) || (!seen && (mode & CD_PRON_HIDDEN))));


	/* First, try using pronouns, or describing hidden creatures */
	if(!seen || pron)
	{
		/* an encoding of the creature "sex" */
		int kind = 0x00;

		/* Extract the gender (if applicable) */
		if(IS_FEMALE(creature_ptr)) kind = 0x20;
		else if(IS_MALE(creature_ptr)) kind = 0x10;

		/* Ignore the gender (if desired) */
		if(!creature_ptr || !pron) kind = 0x00;

		/* Assume simple result */
		res = KW_IT;

		/* Brute force: split on the possibilities */
		switch (kind + (mode & (CD_INDEF_HIDDEN | CD_POSSESSIVE | CD_OBJECTIVE)))
		{
			/* Neuter, or unknown */
#ifdef JP
			case 0x00:                                                    res = "何か"; break;
			case 0x00 + (CD_OBJECTIVE):                                   res = "何か"; break;
			case 0x00 + (CD_POSSESSIVE):                                  res = "何かの"; break;
			case 0x00 + (CD_POSSESSIVE | CD_OBJECTIVE):                   res = "何か自身"; break;
			case 0x00 + (CD_INDEF_HIDDEN):                                res = "何か"; break;
			case 0x00 + (CD_INDEF_HIDDEN | CD_OBJECTIVE):                 res = "何か"; break;
			case 0x00 + (CD_INDEF_HIDDEN | CD_POSSESSIVE):                res = "何か"; break;
			case 0x00 + (CD_INDEF_HIDDEN | CD_POSSESSIVE | CD_OBJECTIVE): res = "それ自身"; break;
#else
			case 0x00:                                                    res = "it"; break;
			case 0x00 + (CD_OBJECTIVE):                                   res = "it"; break;
			case 0x00 + (CD_POSSESSIVE):                                  res = "its"; break;
			case 0x00 + (CD_POSSESSIVE | CD_OBJECTIVE):                   res = "itself"; break;
			case 0x00 + (CD_INDEF_HIDDEN):                                res = "something"; break;
			case 0x00 + (CD_INDEF_HIDDEN | CD_OBJECTIVE):                 res = "something"; break;
			case 0x00 + (CD_INDEF_HIDDEN | CD_POSSESSIVE):                res = "something's"; break;
			case 0x00 + (CD_INDEF_HIDDEN | CD_POSSESSIVE | CD_OBJECTIVE): res = "itself"; break;
#endif


				/* Male (assume human if vague) */
#ifdef JP
			case 0x10:                                                    res = "彼"; break;
			case 0x10 + (CD_OBJECTIVE):                                   res = "彼"; break;
			case 0x10 + (CD_POSSESSIVE):                                  res = "彼の"; break;
			case 0x10 + (CD_POSSESSIVE | CD_OBJECTIVE):                   res = "彼自身"; break;
			case 0x10 + (CD_INDEF_HIDDEN):                                res = "誰か"; break;
			case 0x10 + (CD_INDEF_HIDDEN | CD_OBJECTIVE):                 res = "誰か"; break;
			case 0x10 + (CD_INDEF_HIDDEN | CD_POSSESSIVE):                res = "誰かの"; break;
			case 0x10 + (CD_INDEF_HIDDEN | CD_POSSESSIVE | CD_OBJECTIVE): res = "彼自身"; break;
#else
			case 0x10:                                                    res = "he"; break;
			case 0x10 + (CD_OBJECTIVE):                                   res = "him"; break;
			case 0x10 + (CD_POSSESSIVE):                                  res = "his"; break;
			case 0x10 + (CD_POSSESSIVE | CD_OBJECTIVE):                   res = "himself"; break;
			case 0x10 + (CD_INDEF_HIDDEN):                                res = "someone"; break;
			case 0x10 + (CD_INDEF_HIDDEN | CD_OBJECTIVE):                 res = "someone"; break;
			case 0x10 + (CD_INDEF_HIDDEN | CD_POSSESSIVE):                res = "someone's"; break;
			case 0x10 + (CD_INDEF_HIDDEN | CD_POSSESSIVE | CD_OBJECTIVE): res = "himself"; break;
#endif


				/* Female (assume human if vague) */
#ifdef JP
			case 0x20:                                                    res = "彼女"; break;
			case 0x20 + (CD_OBJECTIVE):                                   res = "彼女"; break;
			case 0x20 + (CD_POSSESSIVE):                                  res = "彼女の"; break;
			case 0x20 + (CD_POSSESSIVE | CD_OBJECTIVE):                   res = "彼女自身"; break;
			case 0x20 + (CD_INDEF_HIDDEN):                                res = "誰か"; break;
			case 0x20 + (CD_INDEF_HIDDEN | CD_OBJECTIVE):                 res = "誰か"; break;
			case 0x20 + (CD_INDEF_HIDDEN | CD_POSSESSIVE):                res = "誰かの"; break;
			case 0x20 + (CD_INDEF_HIDDEN | CD_POSSESSIVE | CD_OBJECTIVE): res = "彼女自身"; break;
#else
			case 0x20:                                                    res = "she"; break;
			case 0x20 + (CD_OBJECTIVE):                                   res = "her"; break;
			case 0x20 + (CD_POSSESSIVE):                                  res = "her"; break;
			case 0x20 + (CD_POSSESSIVE | CD_OBJECTIVE):                   res = "herself"; break;
			case 0x20 + (CD_INDEF_HIDDEN):                                res = "someone"; break;
			case 0x20 + (CD_INDEF_HIDDEN | CD_OBJECTIVE):                 res = "someone"; break;
			case 0x20 + (CD_INDEF_HIDDEN | CD_POSSESSIVE):                res = "someone's"; break;
			case 0x20 + (CD_INDEF_HIDDEN | CD_POSSESSIVE | CD_OBJECTIVE): res = "herself"; break;
#endif
		}

		/* Copy the result */
		(void)strcpy(desc, res);
	}


	/* Handle visible creatures, "reflexive" request */
	else if((mode & (CD_POSSESSIVE | CD_OBJECTIVE)) == (CD_POSSESSIVE | CD_OBJECTIVE))
	{
		/* The creature is visible, so use its gender */
#ifdef JP
		if(IS_FEMALE(creature_ptr)) strcpy(desc, "彼女自身");
		else if(IS_MALE(creature_ptr)) strcpy(desc, "彼自身");
		else strcpy(desc, "それ自身");
#else
		if(IS_FEMALE(creature_ptr)) strcpy(desc, "herself");
		else if(IS_MALE(creature_ptr)) strcpy(desc, "himself");
		else strcpy(desc, "itself");
#endif
	}


	/* Handle all other visible creature requests */
	else
	{
		/* Tanuki? */
		if(is_pet(player_ptr, creature_ptr) && !is_original_ap(creature_ptr))
		{
#ifdef JP
			char *t;
			strcpy(buf, name);
			t = buf;
			while(strncmp(t, "』", 2) && *t) t++;
			if(*t)
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
			if((has_trait_species(species_ptr, TRAIT_UNIQUE)) && !(has_trait(player_ptr, TRAIT_HALLUCINATION) && !(mode & CD_IGNORE_HALLU)))
			{
				/* Start with the name (thus nominative and objective) */
				if((creature_ptr->sc_flag2 & SC_FLAG2_CHAMELEON) && !(mode & CD_TRUE_NAME))
				{
#ifdef JP
					char *t;
					strcpy(buf, name);
					t = buf;
					while (strncmp(t, "』", 2) && *t) t++;
					if(*t)
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

				/* Inside creature arena, and it is not your mount */
				else if(floor_ptr->gamble_arena_mode && !(player_ptr->riding && (&creature_list[player_ptr->riding] == creature_ptr)))
				{
					/* It is a fake unique creature */
#ifdef JP
					(void)sprintf(desc, "%sもどき", name);
#else
					(void)sprintf(desc, "fake %s", name);
#endif
				}

				else
				{
					if(!(mode & CD_IGNORE_EGO_DESC)) creature_desc_ego_pre(desc, creature_ptr, species_ptr, mode);
					(void)strcat(desc, species_name + species_ptr->name);
					if(!(mode & CD_IGNORE_EGO_DESC)
						) creature_desc_ego_post(desc, creature_ptr, species_ptr);
				}
			}

			/* It could be an indefinite creature */
			else if(mode & CD_INDEF_VISIBLE)
			{
				/* XXX Check plurality for "some" */

				/* Indefinite creatures need an indefinite article */
#ifdef JP
				(void)strcpy(desc, "");
#else
				(void)strcpy(desc, is_a_vowel(name[0]) ? "an " : "a ");
#endif

				if(!(mode & CD_IGNORE_EGO_DESC)) creature_desc_ego_pre(desc, creature_ptr, species_ptr, mode);
				(void)strcat(desc, species_name + species_ptr->name);
				if(!(mode & CD_IGNORE_EGO_DESC)) creature_desc_ego_post(desc, creature_ptr, species_ptr);
			}

			/* It could be a normal, definite, creature */
			else
			{
				/* Definite creatures need a definite article */
				if(is_pet(player_ptr, creature_ptr))
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

				if(!(mode & CD_IGNORE_EGO_DESC)) creature_desc_ego_pre(desc, creature_ptr, species_ptr, mode);
				(void)strcat(desc, species_name + species_ptr->name);
				if(!(mode & CD_IGNORE_EGO_DESC)) creature_desc_ego_post(desc, creature_ptr, species_ptr);
			}



			if(creature_ptr->nickname)
			{
#ifdef JP
				sprintf(buf,"「%s」",quark_str(creature_ptr->nickname));
#else
				sprintf(buf," called %s",quark_str(creature_ptr->nickname));
#endif
				strcat(desc,buf);
			}

			if(player_ptr->riding && (&creature_list[player_ptr->riding] == creature_ptr))
				strcat(desc, MES_CR_DESC_RIDING);

			if((mode & CD_IGNORE_HALLU) && (creature_ptr->sc_flag2 & SC_FLAG2_CHAMELEON))
			{
				if(has_trait_species(species_ptr, TRAIT_UNIQUE))
					strcat(desc, MES_CR_DESC_CHAMELEON_LORD);
				else
					strcat(desc, MES_CR_DESC_CHAMELEON);
			}

			if((mode & CD_IGNORE_HALLU) && !is_original_ap(creature_ptr))
				strcat(desc, format("(%s)", species_name + species_info[creature_ptr->species_idx].name));

			/* Handle the Possessive as a special afterthought */
			if(mode & CD_POSSESSIVE)
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


void creature_desc_ego_pre(char* desc, creature_type *creature_ptr, species_type *species_ptr, FLAGS_32 mode)
{
	char buf[100];

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
		strcpy(buf, desc_race_name(creature_ptr, mode));
		if(strlen(buf))
		{
			(void)strcat(desc, buf);
#ifdef JP
			(void)strcat(desc, "の");
#else
			(void)strcat(desc, "'s ");
#endif
		}
	}

	if(has_trait(creature_ptr, TRAIT_VARIABLE_SIZE)){
		char tmp[80];
		tmp[0] = '\0';
#ifdef JP
		sprintf(tmp, "サイズ%dの");
#else
		sprintf(tmp, "Size:%d ");
#endif
		(void)strcat(desc, tmp);
	}

	else if(creature_ptr->creature_ego_idx != MONEGO_NONE){
		(void)strcat(desc, re_name + re_info[creature_ptr->creature_ego_idx].name);
	}

}

void creature_desc_ego_post(char* desc, creature_type *creature_ptr, species_type *species_ptr)
{

	if(creature_ptr->class_idx == INDEX_VARIABLE){
#ifdef JP
		(void)strcat(desc, "の");
		(void)strcat(desc, class_info[species_ptr->class_idx].title);
#else
		(void)strcat(desc, class_info[species_ptr->class_idx].title);
#endif
	}

}



/*
* Learn about a creature (by "probing" it)
*
* Return the number of new flags learnt.  -Mogami-
*/
int lore_do_probe(SPECIES_ID species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	int n = 0;
	byte tmp_byte;

	/* Maximal info about awareness */
	if(species_ptr->r_wake != MAX_UCHAR) n++;
	if(species_ptr->r_ignore != MAX_UCHAR) n++;
	species_ptr->r_wake = species_ptr->r_ignore = MAX_UCHAR;

	/* Maximal drops */
	tmp_byte = 0;

	/* Observe many spells */
	if(species_ptr->r_cast_spell != MAX_UCHAR) n++;
	species_ptr->r_cast_spell = MAX_UCHAR;

	/* Know about evolution */
	if(!(species_ptr->r_xtra1 & MR1_SINKA)) n++;
	species_ptr->r_xtra1 |= MR1_SINKA;

	/* Update creature recall window */
	if(species_window_idx == species_idx)
	{
		prepare_window(PW_MONSTER);
	}

	/* Return the number of new flags learnt */
	return n;
}

void sanity_blast(creature_type *watcher_ptr, creature_type *eldritch_ptr)
{
	int difficulty = 100;
	floor_type *floor_ptr = GET_FLOOR_PTR(watcher_ptr);
	char            eldritch_name[80];
	//species_type    *species_ptr = &species_info[eldritch_ptr->ap_species_idx];

	if(floor_ptr->gamble_arena_mode || !floor_ptr->generated) return;


	difficulty = eldritch_ptr->lev;
	creature_desc(eldritch_name, eldritch_ptr, 0);

	if(!IS_IN_THIS_FLOOR(eldritch_ptr)) return;
	if(!eldritch_ptr->see_others) return; // Cannot see it for some reason
	if(!has_trait(eldritch_ptr, TRAIT_ELDRITCH_HORROR)) return; // oops
	if(is_pet(player_ptr, eldritch_ptr)) return; // Pet eldritch horrors are safe most of the time
	if(saving_throw(watcher_ptr, SAVING_VO, difficulty, 0)) return;

	//TODO saving_throw if(saving_throw(watcher_ptr->skill_rob - difficulty)) return; // Save, no adverse effects

	if(has_trait(watcher_ptr, TRAIT_HALLUCINATION))
	{
		// Something silly happens...
		msg_format(MES_INSANITY_FACED(funny_desc[randint0(MAX_SAN_FUNNY)], eldritch_name));
		if(one_in_(3))
		{
			msg_print(funny_comments[randint0(MAX_SAN_COMMENT)]);
			add_timed_trait(watcher_ptr, TRAIT_HALLUCINATION, (s16b)randint1(difficulty), TRUE); 
		}
		return;
	}

	// Something frightening happens...

	if(has_trait(watcher_ptr, TRAIT_DEMON))
		msg_format(MES_INSANITY_GLANCE(delight_desc[randint0(MAX_SAN_DELIGHT)], eldritch_name));
	else
		msg_format(MES_INSANITY_FACED(horror_desc[randint0(MAX_SAN_HORROR)], eldritch_name));

	reveal_creature_info(eldritch_ptr, TRAIT_ELDRITCH_HORROR);

	// Demon characters are unaffected
	if(has_trait(watcher_ptr, TRAIT_DEMON)) return;
	if(wizard) return;

	// Undead characters are 50% likely to be unaffected
	if(has_trait(watcher_ptr, TRAIT_UNDEAD)) if(saving_throw(watcher_ptr, SAVING_VO, 40, 0)) return;

	sanity_blast_aux(watcher_ptr, difficulty);
}

void sanity_blast_aux(creature_type *watcher_ptr, POWER power)
{
	bool happened = FALSE;
	/* Mind blast */
	if(!saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
		if(!has_trait(watcher_ptr, TRAIT_NO_CONF))
			(void)add_timed_trait(watcher_ptr, TRAIT_CONFUSED, randint0(4) + 4, FALSE);
		if(!has_trait(watcher_ptr, TRAIT_RES_CHAO) && one_in_(3) && !has_trait(watcher_ptr, TRAIT_NO_HALLUCINATION))
			(void)add_timed_trait(watcher_ptr, TRAIT_HALLUCINATION, randint0(250) + 150, FALSE);
		return;
	}

	// Lose int & wis
	if(!saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
		do_dec_stat(watcher_ptr, STAT_INT);
		do_dec_stat(watcher_ptr, STAT_WIS);
		return;
	}

	// Brain smash
	if(!saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
		if(!has_trait(watcher_ptr, TRAIT_NO_CONF)) (void)add_timed_trait(watcher_ptr, TRAIT_CONFUSED, randint0(4) + 4, FALSE);
		if(!has_trait(watcher_ptr, TRAIT_FREE_ACTION)) (void)add_timed_trait(watcher_ptr, TRAIT_PARALYZED, randint0(4) + 4, FALSE);
		while (!saving_throw(watcher_ptr, SAVING_VO, power, 0)) (void)do_dec_stat(watcher_ptr, STAT_INT);
		while (!saving_throw(watcher_ptr, SAVING_VO, power, 0)) (void)do_dec_stat(watcher_ptr, STAT_WIS);
		if(!has_trait(watcher_ptr, TRAIT_RES_CHAO)) (void)add_timed_trait(watcher_ptr, TRAIT_HALLUCINATION, GET_TIMED_TRAIT(watcher_ptr, TRAIT_HALLUCINATION) + randint0(250) + 150, FALSE);
		return;
	}

	// Amnesia
	if(!saving_throw(watcher_ptr, SAVING_VO, power, 0))
	{
		if(lose_all_info(watcher_ptr)) msg_print(MES_INSANITY_AMNESIA);
		return;
	}

	/* Else gain permanent insanity */
	if(has_trait(watcher_ptr, TRAIT_MORONIC) && has_trait(watcher_ptr, TRAIT_BERS_RAGE) &&
		(has_trait(watcher_ptr, TRAIT_COWARDICE) || has_trait(watcher_ptr, TRAIT_FEARLESS)) &&
		(has_trait(watcher_ptr, TRAIT_HALLU) || has_trait(watcher_ptr, TRAIT_RES_CHAO)))
		return;

	while (!happened)
	{
		switch (randint1(4))
		{
		case 1:
			{
				if(!has_trait(watcher_ptr, TRAIT_MORONIC))
				{
					if(has_trait(watcher_ptr, TRAIT_HYPER_INT)) lose_mutative_trait(watcher_ptr, TRAIT_HYPER_INT);

					get_mutative_trait(watcher_ptr, TRAIT_MORONIC);
					happened = TRUE;
				}
				break;
			}
		case 2:
			{
				if(!has_trait(watcher_ptr, TRAIT_COWARDICE) && !has_trait(watcher_ptr, TRAIT_FEARLESS))
				{
					if(has_trait(watcher_ptr, TRAIT_FEARLESS)) lose_mutative_trait(watcher_ptr, TRAIT_FEARLESS);
					get_mutative_trait(watcher_ptr, TRAIT_COWARDICE);
					happened = TRUE;
				}
				break;
			}
		case 3:
			{
				if(!has_trait(watcher_ptr, TRAIT_HALLU) && !has_trait(watcher_ptr, TRAIT_RES_CHAO))
				{
					get_mutative_trait(watcher_ptr, TRAIT_HALLU);
					happened = TRUE;
				}
				break;
			}
		default:
			{
				if(!has_trait(watcher_ptr, TRAIT_BERS_RAGE))
				{
					get_mutative_trait(watcher_ptr, TRAIT_BERS_RAGE);
					happened = TRUE;
				}
				break;
			}
		}
	}

	prepare_update(watcher_ptr, CRU_BONUS);
	handle_stuff(watcher_ptr);

	prepare_update(watcher_ptr, CRU_BONUS);
	handle_stuff(watcher_ptr);
}


/*
* This function updates the creature record of the given creature
*
* This involves extracting the distance to the player (if requested),
* and then checking for visibility (natural, infravision, see-invis,
* telepathy), updating the creature visibility flag, redrawing (or
* erasing) the creature when its visibility changes, and taking note
* of any interesting creature flags (cold-blooded, invisible, etc).
*
* Note the new "sc_flag" field which encodes several creature state flags,
* including "view" for when the creature is currently in line of sight,
* and "mark" for when the creature is currently visible via detection.
*
* The only creature fields that are changed here are "cdis" (the
* distance from the player), "ml" (visible to the player), and
* "sc_flag" (to maintain the "SC_FLAG_VIEW" flag).
*
* Note the special "update_creatures()" function which can be used to
* call this function once for every creature.
*
* Note the "full" flag which requests that the "cdis" field be updated,
* this is only needed when the creature (or the player) has moved.
*
* Every time a creature moves, we must call this function for that
* creature, and update the distance, and the visibility.  Every time
* the player moves, we must call this function for every creature, and
* update the distance, and the visibility.  Whenever the player "state"
* changes in certain ways ("blindness", "infravision", "telepathy",
* and "see invisible"), we must call this function for every creature,
* and update the visibility.
*
* Routines that change the "illumination" of a grid must also call this
* function for any creature in that grid, since the "visibility" of some
* creatures may be based on the illumination of their grid.
*
* Note that this function is called once per creature every time the
* player moves.  When the player is running, this function is one
* of the primary bottlenecks, along with "update_view()" and the
* "process_creatures()" code, so efficiency is important.
*
* Note the optimized "inline" version of the "distance()" function.
*
* A creature is "visible" to the player if(1) it has been detected
* by the player, (2) it is close to the player and the player has
* telepathy, or (3) it is close to the player, and in line of sight
* of the player, and it is "illuminated" by some combination of
* infravision, torch light, or permanent light (invisible creatures
* are only affected by "light" if the player can see invisible).
*
* Creatures which are not on the current panel may be "visible" to
* the player, and their descriptions will include an "offscreen"
* reference.  Currently, offscreen creatures cannot be targetted
* or viewed directly, but old targets will remain set.  
*
* The player can choose to be disturbed by several things, including
* "disturb_move" (creature which is viewable moves in some way), and
* "disturb_near" (creature which is "easily" viewable moves in some
* way).  Note that "moves" includes "appears" and "disappears".
*/
//TODO  Marge to set_creature_bonuses
void update_creature_view(creature_type *creature_ptr, CREATURE_ID creature_idx, bool full)
{
	creature_type *target_ptr = &creature_list[creature_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(target_ptr);

	bool do_disturb = disturb_move;

	int d;

	/* Current location */
	int fy = target_ptr->fy;
	int fx = target_ptr->fx;

	/* Seen at all */
	bool flag = FALSE;

	/* Seen by vision */
	bool easy = FALSE;

	/* Non-Ninja player in the darkness */
	bool in_darkness = (dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_DARKNESS) && !has_trait(creature_ptr, TRAIT_SEE_DARKNESS);

	/* Do disturb? */
	if(disturb_high)
	{
		species_type *ap_r_ptr = &species_info[target_ptr->ap_species_idx];

		if(ap_r_ptr->r_tkills && ap_r_ptr->level >= creature_ptr->lev)
			do_disturb = TRUE;
	}

	/* Compute distance */
	if(full)
	{
		/* Distance components */
		int dy = (creature_ptr->fy > fy) ? (creature_ptr->fy - fy) : (fy - creature_ptr->fy);
		int dx = (creature_ptr->fx > fx) ? (creature_ptr->fx - fx) : (fx - creature_ptr->fx);

		/* Approximate distance */
		d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

		/* Restrict distance */
		if(d > 255) d = 255;

		if(!d) d = 1;

		/* Save the distance */
		target_ptr->cdis = d;
	}

	/* Extract distance */
	else
	{
		/* Extract the distance */
		d = target_ptr->cdis;
	}


	/* Detected */
	if(target_ptr->sc_flag2 & (SC_FLAG2_MARK)) flag = TRUE;


	/* Nearby */
	if(d <= (in_darkness ? MAX_SIGHT / 2 : MAX_SIGHT))
	{
		if(!in_darkness || (d <= MAX_SIGHT / 4))
		{
			if(GET_TIMED_TRAIT(creature_ptr, TRAIT_POSTURE_MUSOU))
			{
				/* Detectable */
				flag = TRUE;

				if(is_original_ap(target_ptr) && !has_trait(creature_ptr, TRAIT_HALLUCINATION))
				{
					if(has_trait(target_ptr, TRAIT_SMART)) reveal_creature_info(target_ptr, TRAIT_SMART);
					if(has_trait(target_ptr, TRAIT_SMART)) reveal_creature_info(target_ptr, TRAIT_STUPID);
				}
			}

			/* Basic telepathy */
			/* Snipers get telepathy when they concentrate deeper */
			else if(has_trait(creature_ptr, TRAIT_ESP))
			{
				/* Empty mind, no telepathy */
				if(has_trait(target_ptr, TRAIT_EMPTY_MIND))
				{
					/* Memorize flags */
					if(is_original_ap_and_seen(creature_ptr, target_ptr)) reveal_creature_info(target_ptr, TRAIT_EMPTY_MIND);
				}

				/* Weird mind, occasional telepathy */
				else if(has_trait(target_ptr, TRAIT_WEIRD_MIND))
				{
					/* One in ten individuals are detectable */
					if((creature_idx % 10) == 5)
					{
						/* Detectable */
						flag = TRUE;

						if(is_original_ap(target_ptr) && !has_trait(creature_ptr, TRAIT_HALLUCINATION))
						{
							/* Memorize flags */
							reveal_creature_info(target_ptr, TRAIT_WEIRD_MIND);
							reveal_creature_info(target_ptr, TRAIT_SMART);
							reveal_creature_info(target_ptr, TRAIT_STUPID);
						}
					}
				}

				/* Normal mind, allow telepathy */
				else
				{
					/* Detectable */
					flag = TRUE;

					if(is_original_ap(target_ptr) && !has_trait(creature_ptr, TRAIT_HALLUCINATION))
					{
						/* Hack -- Memorize mental flags */
						reveal_creature_info(target_ptr, TRAIT_SMART);
						reveal_creature_info(target_ptr, TRAIT_STUPID);
					}
				}
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_ANIMAL) &&  has_trait(target_ptr, TRAIT_ANIMAL))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_UNDEAD) && has_trait(target_ptr, TRAIT_UNDEAD))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_DEMON) && has_trait(target_ptr, TRAIT_DEMON))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_ORC) && has_trait(target_ptr, TRAIT_ORC))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_TROLL) && has_trait(target_ptr, TRAIT_TROLL))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_GIANT) && has_trait(target_ptr, TRAIT_GIANT))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_DRAGON) && has_trait(target_ptr, TRAIT_DRAGON))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_HUMAN) && has_trait(target_ptr, TRAIT_HUMAN))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_RACE);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_EVIL) && is_enemy_of_good_creature(target_ptr))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_GOOD) && is_enemy_of_evil_creature(target_ptr))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, INFO_TYPE_ALIGNMENT);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_NONLIVING) && has_trait(target_ptr, TRAIT_NONLIVING) && !has_trait(target_ptr, TRAIT_UNDEAD)) 
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, TRAIT_NONLIVING);
			}

			/* Magical sensing */
			if(has_trait(creature_ptr, TRAIT_SENSE_UNIQUE) && has_trait(target_ptr, TRAIT_UNIQUE))
			{
				flag = TRUE;
				reveal_creature_info(target_ptr, TRAIT_UNIQUE);
			}
		}

		/* Normal line of sight, and not blind */
		if(player_has_los_bold(fy, fx) && !has_trait(creature_ptr, TRAIT_BLIND))
		{
			bool do_invisible = FALSE;
			bool do_cold_blood = FALSE;

			/* Snipers can see targets in darkness when they concentrate deeper */
			if(creature_ptr->concent >= CONCENT_RADAR_THRESHOLD)
			{
				/* Easy to see */
				easy = flag = TRUE;
			}

			/* Use "infravision" */
			if(d <= creature_ptr->see_infra)
			{
				/* Handle "cold blooded" creatures */
				if(has_trait(target_ptr, TRAIT_COLD_BLOOD) || !has_trait(target_ptr, TRAIT_AURA_FIRE))
				{
					do_cold_blood = TRUE;
				}

				/* Handle "warm blooded" creatures */
				else
				{
					/* Easy to see */
					easy = flag = TRUE;
				}
			}

			/* Use "illumination" */
			if(creature_can_see_bold(creature_ptr, fy, fx))
			{
				/* Handle "invisible" creatures */
				if(has_trait(target_ptr, TRAIT_INVISIBLE))
				{
					do_invisible = TRUE;

					/* See invisible */
					if(!has_trait(creature_ptr, TRAIT_INVISIBLE))
					{
						/* Easy to see */
						easy = flag = TRUE;
					}
				}

				/* Handle "normal" creatures */
				else
				{
					/* Easy to see */
					easy = flag = TRUE;
				}
			}

			/* Visible */
			if(flag)
			{
				if(is_original_ap(target_ptr) && !has_trait(creature_ptr, TRAIT_HALLUCINATION))
				{
					/* Memorize flags */
					reveal_creature_info(target_ptr, TRAIT_INVISIBLE);
					reveal_creature_info(target_ptr, TRAIT_COLD_BLOOD);
				}
			}
		}
	}


	/* The creature is now visible */
	if(flag)
	{
		/* It was previously unseen */
		if(!target_ptr->see_others)
		{
			/* Mark as visible */
			target_ptr->see_others = TRUE;

			/* Draw the creature */
			lite_spot(floor_ptr, fy, fx);

			/* Update health bar as needed */
			if(npc_status_id == creature_idx) prepare_redraw(PR_HEALTH);
			if(creature_ptr->riding == creature_idx) prepare_redraw(PR_UHEALTH);

			/* Hack -- Count "fresh" sightings */
			if(!has_trait(creature_ptr, TRAIT_HALLUCINATION))
			{
				//if((target_ptr->ap_species_idx == SPECIES_KAGE) && (species_info[SPECIES_KAGE].r_sights < MAX_SHORT))
				//	species_info[SPECIES_KAGE].r_sights++;
				//else if(is_original_ap(target_ptr) && (species_ptr->r_sights < MAX_SHORT))
				//	species_ptr->r_sights++;
			}

			/* Eldritch Horror */
			if(has_trait_species(&species_info[target_ptr->ap_species_idx], TRAIT_ELDRITCH_HORROR))
				sanity_blast(creature_ptr, target_ptr);

			/* Disturb on appearance */
			if(disturb_near && (projectable(floor_ptr, MAX_RANGE, creature_ptr->fy, creature_ptr->fx, target_ptr->fy, target_ptr->fx)))
			{
				if(disturb_pets || is_hostile(target_ptr)) disturb(creature_ptr, 1, 0);
			}
		}
	}

	/* The creature is not visible */
	else
	{
		/* It was previously seen */
		if(target_ptr->see_others)
		{
			/* Mark as not visible */
			target_ptr->see_others = FALSE;

			/* Erase the creature */
			lite_spot(floor_ptr, fy, fx);

			/* Update health bar as needed */
			if(npc_status_id == creature_idx) prepare_redraw(PR_HEALTH);
			if(creature_ptr->riding == creature_idx) prepare_redraw(PR_UHEALTH);

			/* Disturb on disappearance */
			if(do_disturb)
			{
				if(disturb_pets || is_hostile(target_ptr))
					disturb(creature_ptr, 1, 0);
			}
		}
	}


	/* The creature is now easily visible */
	if(easy)
	{
		/* Change */
		if(!(target_ptr->sc_flag & (SC_FLAG_VIEW)))
		{
			/* Mark as easily visible */
			target_ptr->sc_flag |= (SC_FLAG_VIEW);

			/* Disturb on appearance */
			if(do_disturb)
			{
				if(disturb_pets || is_hostile(target_ptr))
					disturb(creature_ptr, 1, 0);
			}
		}
	}

	/* The creature is not easily visible */
	else
	{
		/* Change */
		if(target_ptr->sc_flag & (SC_FLAG_VIEW))

		{
			/* Mark as not easily visible */
			target_ptr->sc_flag &= ~(SC_FLAG_VIEW);

			/* Disturb on disappearance */
			if(do_disturb)
			{
				if(disturb_pets || is_hostile(target_ptr))
					disturb(creature_ptr, 1, 0);
			}
		}
	}
}


/*
* This function simply updates all the (non-dead) creatures (see above).
*/
void update_creatures(bool full)
{
	int i;

	/* Update each (live) creature */
	for (i = 1; i < creature_max; i++)
	{
		creature_type *m_ptr = &creature_list[i];
		if(!is_valid_creature(m_ptr)) continue;
		update_creature_view(player_ptr, i, full);
	}
}


/*
* Hack -- the index of the summoning creature
*/
static bool creature_hook_chameleon_lord(SPECIES_ID species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	creature_type *m_ptr = &creature_list[chameleon_change_m_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(m_ptr);

	if(!(has_trait_species(species_ptr, TRAIT_UNIQUE))) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_FRIENDLY) || has_trait_species(species_ptr, TRAIT_CHAMELEON)) return FALSE;
	if(ABS(species_ptr->level - species_info[SPECIES_CHAMELEON_K].level) > 5) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_SUICIDE_BOMBER)) return FALSE;
	if(!species_can_cross_terrain(floor_ptr->cave[m_ptr->fy][m_ptr->fx].feat, species_ptr, 0)) return FALSE;
	return TRUE;
}

static bool creature_hook_chameleon(SPECIES_ID species_idx)
{
	species_type *species_ptr = &species_info[species_idx];
	creature_type *m_ptr = &creature_list[chameleon_change_m_idx];
	species_type *old_r_ptr = &species_info[m_ptr->species_idx];
	floor_type *floor_ptr = GET_FLOOR_PTR(m_ptr);

	if(has_trait_species(species_ptr, TRAIT_UNIQUE)) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_MULTIPLY)) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_FRIENDLY) || has_trait_species(species_ptr, TRAIT_CHAMELEON)) return FALSE;
	if(has_trait_species(species_ptr, TRAIT_SUICIDE_BOMBER)) return FALSE;

	if(!species_can_cross_terrain(floor_ptr->cave[m_ptr->fy][m_ptr->fx].feat, species_ptr, 0)) return FALSE;

	/* Not born */
	if(!has_trait_species(old_r_ptr, TRAIT_CHAMELEON))
	{
		if(is_enemy_of_evil_species(old_r_ptr) && !is_enemy_of_evil_species(species_ptr)) return FALSE;
		if(is_enemy_of_good_species(old_r_ptr) && !is_enemy_of_good_species(species_ptr)) return FALSE;
		if(!(is_enemy_of_evil_species(old_r_ptr) && is_enemy_of_good_species(old_r_ptr)) && (is_enemy_of_evil_species(species_ptr) && is_enemy_of_good_species(species_ptr))) return FALSE;
	}

	return TRUE;
}

void set_new_species(creature_type *creature_ptr, bool born, SPECIES_ID species_idx, CREATURE_EGO_ID creature_ego_idx)
{
	int i, m_idx = 0;
	int oldmhp;
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	species_type *species_ptr;
	char old_m_name[MAX_NLEN];
	bool old_unique = FALSE;
	int old_species_idx = creature_ptr->species_idx;

	if(has_trait_species(&species_info[creature_ptr->species_idx], TRAIT_UNIQUE))
		old_unique = TRUE;
	if(old_unique && (species_idx == SPECIES_CHAMELEON)) species_idx = SPECIES_CHAMELEON_K;
	species_ptr = &species_info[species_idx];
	creature_desc(old_m_name, creature_ptr, 0);

	if(!species_idx)
	{
		int level;
		PROB *prob_list;

		alloc_species_list(&prob_list);
		if(has_trait(creature_ptr, TRAIT_UNIQUE)) forbid_species_list(&prob_list, creature_hook_chameleon_lord);
		else forbid_species_list(&prob_list, creature_hook_chameleon);

		level = species_info[creature_ptr->species_idx].level;
		if(dungeon_info[floor_ptr->dungeon_id].flags1 & DF1_CHAMELEON) level += 2+randint1(3);
		set_species_list_bias_level_limitation(&prob_list, level, level+10);

		species_idx = species_rand(prob_list);
		free_species_list(&prob_list);
		species_ptr = &species_info[species_idx];

		if(!species_idx) return;
	}

	if(is_pet(player_ptr, creature_ptr)) check_pets_num_and_align(player_ptr, creature_ptr, FALSE);

	creature_ptr->species_idx = species_idx;
	creature_ptr->ap_species_idx = species_idx;
	update_creature_view(player_ptr, m_idx, FALSE);
	lite_spot(floor_ptr, creature_ptr->fy, creature_ptr->fx);

	if(creature_ego_idx == MONEGO_NONE) creature_ptr->creature_ego_idx = 0;
	else creature_ptr->creature_ego_idx = creature_ego_idx;

	if(is_lighting_species(&species_info[old_species_idx]) || is_darken_species(&species_info[old_species_idx]) || (is_lighting_species(species_ptr) || is_darken_species(species_ptr)))
	{
		prepare_update(creature_ptr, PU_SPECIES_LITE);
	}

	if(is_pet(player_ptr, creature_ptr)) check_pets_num_and_align(player_ptr, creature_ptr, TRUE);

	if(born)
	{
		/* Sub-alignment of a chameleon */
		if(is_enemy_of_evil_species(species_ptr) && is_enemy_of_good_species(species_ptr))
		{
			creature_ptr->sub_align = SUB_ALIGN_NEUTRAL;
			if(is_enemy_of_good_species(species_ptr)) creature_ptr->sub_align |= SUB_ALIGN_EVIL;
			if(is_enemy_of_evil_species(species_ptr)) creature_ptr->sub_align |= SUB_ALIGN_GOOD;
		}
		return;
	}

	if(creature_ptr->ridden)
	{
		char m_name[MAX_NLEN];
		creature_desc(m_name, creature_ptr, 0);
		msg_format(MES_POLYMORPH_DONE(old_m_name));
		if(!has_trait(creature_ptr, TRAIT_RIDING))
			if(do_thrown_from_riding(&creature_list[creature_ptr->ridden], 0, TRUE)) msg_print(MES_FALL_RIDING);
	}

	oldmhp = creature_ptr->mmhp;

	creature_ptr->mhp = (long)(creature_ptr->mhp * creature_ptr->mmhp) / oldmhp;
	if(creature_ptr->mhp < 1) creature_ptr->mhp = 1;
	creature_ptr->chp = (long)(creature_ptr->chp * creature_ptr->mmhp) / oldmhp;

	for(i = 0; i < STAT_MAX; i++)
		creature_ptr->stat_use[i] = species_ptr->stat_max[i];

	if(creature_ptr->creature_ego_idx != MONEGO_NONE)
		for(i = 0; i < STAT_MAX; i++)
			creature_ptr->stat_use[i] += re_info[creature_ego_idx].stat[i];

	//TODO Reset Status
}

static void deal_magic_book_aux(creature_type *creature_ptr, REALM_ID realm)
{
	int min, max;
	TVAL tv;
	object_type forge;
	object_type *quest_ptr;

	quest_ptr = &forge;
	tv = (TVAL)(TV_LIFE_BOOK + realm - 1);

	// First Book
	min = (creature_ptr->lev > 15 ? 2 : 1);
	max = (min + creature_ptr->lev / 10);
	object_prep(quest_ptr, lookup_kind(tv, 0));
	quest_ptr->number = (byte)rand_range(min, max);
	add_item_to_creature(creature_ptr, quest_ptr, 0);

	// Second Book
	if(creature_ptr->lev > 10 || one_in_(50))
	{
		min = (creature_ptr->lev > 22 ? 2 : 1);
		max = (min + creature_ptr->lev / 17);
		object_prep(quest_ptr, lookup_kind(tv, 1));
		quest_ptr->number = (byte)rand_range(min, max);
		add_item_to_creature(creature_ptr, quest_ptr, 0);
	}

	// Third Book
	if(creature_ptr->lev > 32 || one_in_(300))
	{
		min = (creature_ptr->lev > 41 ? 2 : 1);
		max = (min + creature_ptr->lev / 37);
		object_prep(quest_ptr, lookup_kind(tv, 2));
		quest_ptr->number = (byte)rand_range(min, max);
		add_item_to_creature(creature_ptr, quest_ptr, 0);
	}

	// Fourth Book
	if(creature_ptr->lev > 44 || one_in_(1000))
	{
		min = (creature_ptr->lev > 48 ? 2 : 1);
		max = (min + creature_ptr->lev / 50);
		object_prep(quest_ptr, lookup_kind(tv, 3));
		quest_ptr->number = (byte)rand_range(min, max);
		add_item_to_creature(creature_ptr, quest_ptr, 0);
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
	object_type *quest_ptr;
	quest_ptr = &forge;

	if((creature_ptr->lev >= 3 || one_in_(5)) && creature_ptr->lev <= 12)
	{
		min = (1 + creature_ptr->lev / 10);
		max = (min + creature_ptr->lev * 2 / 3);
		object_prep(quest_ptr, lookup_kind(TV_POTION, SV_POTION_CURE_LIGHT));
		quest_ptr->number = (byte)rand_range(min, max);
		add_item_to_creature(creature_ptr, quest_ptr, 0);
	}

}

static void deal_food(creature_type *creature_ptr)
{
	object_type *quest_ptr;
	object_type forge;
	int i;
	quest_ptr = &forge;

	if(has_trait(creature_ptr, TRAIT_FOOD_EATER))
	{
		/* Food rations */
		object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));
		quest_ptr->number = (byte)rand_range(3, 7);

		add_item_to_creature(creature_ptr, quest_ptr, 0);
	}

	else if(has_trait(creature_ptr, TRAIT_CORPSE_EATER))
	{
		PROB *prob_list;
		alloc_species_list(&prob_list);
		set_species_list_bias_level_limitation(&prob_list, 0, 0);

		for (i = rand_range(3, 4); i > 0; i--)
		{
			object_prep(quest_ptr, lookup_kind(TV_CORPSE, SV_CORPSE));
			quest_ptr->creator_idx = species_rand(prob_list);
			quest_ptr->number = 1;
			add_item_to_creature(creature_ptr, quest_ptr, 0);
		}
		free_species_list(&prob_list);

	}
	else if(has_trait(creature_ptr, TRAIT_WATER_DRINKER))
	{
		/* Potions of Water */
		object_prep(quest_ptr, lookup_kind(TV_POTION, SV_POTION_WATER));
		quest_ptr->number = (byte)rand_range(15, 23);
		add_item_to_creature(creature_ptr, quest_ptr, 0);
	}
	else if(has_trait(creature_ptr, TRAIT_FLASK_DRINKER))
	{
		/* Flasks of oil */
		object_prep(quest_ptr, lookup_kind(TV_FLASK, SV_ANY));

		/* Fuel with oil */
		apply_magic(creature_ptr, quest_ptr, 1, AM_NO_FIXED_ART, 0);
		quest_ptr->number = (byte)rand_range(7, 12);
		add_item_to_creature(creature_ptr, quest_ptr, 0);
	}

}

static void deal_lite(creature_type *creature_ptr)
{
	object_type *quest_ptr;
	object_type forge;
	quest_ptr = &forge;

	if(has_trait(creature_ptr, TRAIT_HUMANOID))
	{
		if(has_trait(creature_ptr, TRAIT_VAMPIRE) && (creature_ptr->class_idx != CLASS_NINJA))
		{
			// Hack -- Give the player scrolls of DARKNESS!
			object_prep(quest_ptr, lookup_kind(TV_SCROLL, SV_SCROLL_DARKNESS));
			quest_ptr->number = (byte)rand_range(2, 5);
			add_item_to_creature(creature_ptr, quest_ptr, 0);
		}

		else if(creature_ptr->class_idx != CLASS_NINJA)
		{
			// Hack -- Give the player some torches
			object_prep(quest_ptr, lookup_kind(TV_LITE, SV_LITE_TORCH));
			quest_ptr->number = (byte)rand_range(3, 7);
			quest_ptr->fuel = (s16b)rand_range(7, 10) * 500;
			add_item_to_creature(creature_ptr, quest_ptr, 0);
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
	TVAL tv;
	SVAL sv;
	object_type	forge;
	object_type	*quest_ptr;
	TVAL i;
	QUANTITY number;
	species_type *species_ptr = &species_info[creature_ptr->species_idx];
	int object_level;

	creature_ptr->carrying_weight = 0;
	creature_ptr->equipping_weight = 0;
	creature_ptr->inven_cnt = 0;
	creature_ptr->equip_cnt = 0;

	for(i = 0; i < INVEN_TOTAL; i++) object_wipe(&creature_ptr->inventory[i]);
	object_level = creature_ptr->lev * 2;

	/* inventory */
	number = 0;
	// TODO Inventory Count


	quest_ptr = &forge;

	for(i = 0; i < creature_ptr->sc / 10; i++)
	{
		make_random_object(quest_ptr, AM_GOOD, 0, object_level, 0);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}

	// Item depend on species_info
	for(i = 0; i < INVEN_TOTAL; i++)
	{
		if(!(&creature_ptr->inventory[i])) break;
		if(!species_ptr->artifact_prob[i]) break;

		if(species_ptr->artifact_id[i])
		{
			artifact_type *a_ptr = &artifact_info[species_ptr->artifact_id[i]];
			if((species_ptr->artifact_id[i] > 0) && ((randint0(100) < species_ptr->artifact_prob[i]) || wizard))
			{
				if(!a_ptr->cur_num)
				{
					object_type ob;
					// Equip the artifact
					create_named_art(creature_ptr, &ob, species_ptr->artifact_id[i]);
					a_ptr->cur_num = 1;
					add_item_to_creature(creature_ptr, &ob, TRUE);
				}
			}

		}
		else
		{
			object_type ob;
			object_prep(&ob, lookup_kind(species_ptr->artifact_tval[i], species_ptr->artifact_sval[i]));
			create_ego(&ob, object_level, species_ptr->artifact_ego[i]);
			add_item_to_creature(creature_ptr, &ob, TRUE);
		}
	}

	deal_magic_book(creature_ptr);							// Dealing MagicBook
	if(is_player(creature_ptr)) deal_potion(creature_ptr);	// Dealing Potion

	//TODO
	// Food depend on traits_precondition
	if(is_player(creature_ptr)) deal_food(creature_ptr);
	if(is_player(creature_ptr)) deal_lite(creature_ptr);

	if(IS_RACE(creature_ptr, RACE_BALROG))
	{
		object_prep(quest_ptr, lookup_kind(TV_LITE, SV_LITE_UDUN));
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}

	if(IS_RACE(creature_ptr, RACE_ISTARI))
	{
		object_prep(quest_ptr, lookup_kind(TV_HAFTED, SV_ISTARISTAFF));
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}

	// Item depend on Class
	if((creature_ptr->class_idx == CLASS_RANGER) || (creature_ptr->class_idx == CLASS_CAVALRY))
	{
		/* Hack -- Give the player some arrows */
		object_prep(quest_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL));
		quest_ptr->number = (byte)rand_range(15, 20);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}

	if(creature_ptr->class_idx == CLASS_RANGER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(quest_ptr, lookup_kind(TV_BOW, SV_SHORT_BOW));
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}

	else if(creature_ptr->class_idx == CLASS_ARCHER)
	{
		/* Hack -- Give the player some arrows */
		object_prep(quest_ptr, lookup_kind(TV_ARROW, SV_AMMO_NORMAL));
		quest_ptr->number = (byte)rand_range(15, 20);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}
	else if(creature_ptr->class_idx == CLASS_HIGH_MAGE)
	{
		/* Hack -- Give the player some arrows */
		object_prep(quest_ptr, lookup_kind(TV_WAND, SV_WAND_MAGIC_MISSILE));
		quest_ptr->number = 1;
		quest_ptr->pval = (byte)rand_range(25, 30);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}
	else if(creature_ptr->class_idx == CLASS_SORCERER)
	{
		for (i = TV_LIFE_BOOK; i <= TV_LIFE_BOOK + MAX_REALMS - 1; i++)
		{
			/* Hack -- Give the player some arrows */
			object_prep(quest_ptr, lookup_kind(i, 0));
			quest_ptr->number = (byte)rand_range(1, creature_ptr->lev / 8);
			add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
		}
	}
	else if(creature_ptr->class_idx == CLASS_TOURIST)
	{
		if(creature_ptr->chara_idx != CHARA_SEXY)
		{
			/* Hack -- Give the player some arrows */
			object_prep(quest_ptr, lookup_kind(TV_SHOT, SV_AMMO_LIGHT));
			quest_ptr->number = (byte)rand_range(15, 20);
			add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
		}

		object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_BISCUIT));
		quest_ptr->number = (byte)rand_range(2, 4);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);

		object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_WAYBREAD));
		quest_ptr->number = (byte)rand_range(2, 4);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);

		object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_JERKY));
		quest_ptr->number = (byte)rand_range(1, 3);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);

		object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_ALE));
		quest_ptr->number = (byte)rand_range(2, 4);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);

		object_prep(quest_ptr, lookup_kind(TV_FOOD, SV_FOOD_PINT_OF_WINE));
		quest_ptr->number = (byte)rand_range(2, 4);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}
	else if(creature_ptr->class_idx == CLASS_NINJA)
	{
		object_prep(quest_ptr, lookup_kind(TV_SPIKE, 0));
		quest_ptr->number = (byte)rand_range(15, 20);
		add_item_to_creature(creature_ptr, quest_ptr, 0);
	}
	else if(creature_ptr->class_idx == CLASS_SNIPER)
	{
		object_prep(quest_ptr, lookup_kind(TV_BOLT, SV_AMMO_NORMAL));
		quest_ptr->number = (byte)rand_range(15, 20);
		add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
	}

	/* Hack -- Give the player three useful objects */
	if(creature_ptr->class_idx != INDEX_NONE && has_trait(creature_ptr, TRAIT_HUMANOID))
	{
		for (i = 0; i < CLASS_INIT_EQUIPMENT; i++)
		{
			/* Look up standard equipment */
			tv = class_equipment_init[creature_ptr->class_idx][i][0];
			sv = class_equipment_init[creature_ptr->class_idx][i][1];
			if(tv == 0) continue;

			if(has_trait(creature_ptr, TRAIT_ANDROID) && ((tv == TV_SOFT_ARMOR) || (tv == TV_HARD_ARMOR))) continue;

			/* Hack to initialize spellbooks */
			if(tv == TV_SORCERY_BOOK && creature_ptr->realm1)
				tv = (TVAL)(TV_LIFE_BOOK + creature_ptr->realm1 - 1);
			else if(tv == TV_DEATH_BOOK && creature_ptr->realm2)
				tv = (TVAL)(TV_LIFE_BOOK + creature_ptr->realm2 - 1);

			else if(tv == TV_RING && sv == SV_RING_RES_FEAR &&
				IS_PURE_RACE(creature_ptr, RACE_BARBARIAN))
				/* Barbarians do not need a ring of resist fear */
				sv = SV_RING_SUSTAIN_STR;

			else if(tv == TV_RING && sv == SV_RING_SUSTAIN_INT && IS_PURE_RACE(creature_ptr, RACE_MIND_FLAYER))
			{
				tv = TV_POTION;
				sv = SV_POTION_RESTORE_MANA;
			}

			/* Hack -- Give the player an object */
			object_prep(quest_ptr, lookup_kind(tv, sv));

			/* Assassins begin the game with a poisoned dagger */
			if((tv == TV_SWORD || tv == TV_HAFTED) && (creature_ptr->class_idx == CLASS_ROGUE && creature_ptr->realm1 == REALM_DEATH)) // Only assassins get a poisoned weapon
				quest_ptr->name2 = EGO_BRAND_POIS;

			add_item_to_creature(creature_ptr, quest_ptr, ADD_OUTFIT_EQUIP);
		}
	}

	// Item depend on Character

	if(creature_ptr->chara_idx == CHARA_SEXY)
	{
		class_equipment_init[creature_ptr->class_idx][2][0] = TV_HAFTED;
		class_equipment_init[creature_ptr->class_idx][2][1] = SV_WHIP;
	}

	/* Hack -- make aware of the water */
	object_kind_info[lookup_kind(TV_POTION, SV_POTION_WATER)].aware = TRUE;


	// Apply Magic
	for(i = 0; i < INVEN_TOTAL; i++)
	{
		if(creature_ptr->inventory[i].k_idx && IS_EQUIPPED(&creature_ptr->inventory[i]))
		{
			if(!creature_ptr->inventory[i].name1 && !creature_ptr->inventory[i].name2)
				apply_magic(creature_ptr, &creature_ptr->inventory[i], creature_ptr->lev * 2,
				calc_deal_item_rank(creature_ptr, &creature_ptr->inventory[i]), 0);
		}
	}

	return;
}

static int place_creature_one(creature_type *summoner_ptr, floor_type *floor_ptr, COODINATES y, COODINATES x, SPECIES_ID species_idx, CREATURE_EGO_ID creature_ego_idx, FLAGS_32 mode)
{
	cave_type		*c_ptr = &floor_ptr->cave[y][x];
	creature_type	*creature_ptr;
	species_type	*species_ptr = &species_info[species_idx];

	if(has_trait_species(species_ptr, TRAIT_UNIQUE)) mode &= ~PC_KAGE;
	if(creature_ego_idx > 0) creature_ego_idx = 0; //TODO

	// DO NOT PLACE A MONSTER IN THE SMALL SCALE WILDERNESS !!!
	if(floor_ptr->global_map) return max_creature_idx;

	if(!IN_BOUNDS(floor_ptr, y, x)) // Verify location
	{
		if(cheat_hear) msg_warning("[Invalid Location (x=%d, y=%d)]", x, y);
		return max_creature_idx;
	}

	if(!(mode & PC_IGNORE_TERRAIN))
	{
		if(pattern_tile(floor_ptr, y, x)) return max_creature_idx;
		if(CAVE_HAVE_FLAG_BOLD(floor_ptr, y, x, FF_CHAOS_TAINTED)) return max_creature_idx;

		if(!species_can_enter(floor_ptr, y, x, species_ptr, 0)) // Require empty space (if not ghostly)
		{
			if(cheat_hear) msg_format("[max_creature_idx: Creature Can't Enter]");
			return max_creature_idx;
		}
	}

	if(!floor_ptr->gamble_arena_mode) // TO DO DEBUG.
	{
		if(species_info[species_idx].cur_num >= species_info[species_idx].max_num)
		{
			if(cheat_hear) msg_format("[Creature Max Limit ID:%d=%d/%d]", species_info[species_idx].cur_num, species_info[species_idx].max_num);
			return max_creature_idx;
		}
			
		// Depth creatures may NOT be created out of depth, unless in Nightmare mode
		if(has_trait_species(species_ptr, TRAIT_FORCE_DEPTH) && (floor_ptr->depth < species_ptr->level) &&
			(!has_trait(player_ptr, TRAIT_CURSE_OF_ILUVATAR) || (has_trait_species(species_ptr, TRAIT_QUESTOR))))
		{
			if(cheat_hear) msg_format("[max_creature_idx: No Nightmare mode.]");
			return (max_creature_idx);	// Cannot create
		}
	}

	if(quest_number(floor_ptr))
	{
		int hoge = quest_number(floor_ptr);
		if((quest[hoge].type == QUEST_TYPE_KILL_LEVEL) || (quest[hoge].type == QUEST_TYPE_RANDOM))
		{
			if(species_idx == quest[hoge].species_idx)
			{
				int number_mon, i2, j2;
				number_mon = 0;

				/* Count all quest creatures */
				for (i2 = 0; i2 < floor_ptr->width; ++i2)
					for (j2 = 0; j2 < floor_ptr->height; j2++)
						if(floor_ptr->cave[j2][i2].creature_idx > 0)
							if(creature_list[floor_ptr->cave[j2][i2].creature_idx].species_idx == quest[hoge].species_idx)
								number_mon++;
				if(number_mon + quest[hoge].cur_num >= quest[hoge].max_num)
					return max_creature_idx;
			}
		}
	}

	if(is_glyph_grid(c_ptr))
	{
		if(randint1(BREAK_GLYPH) < (species_ptr->level + 20))
		{
			if(c_ptr->info & CAVE_MARK) msg_print(MES_BREAK_P_RUNE);
			c_ptr->info &= ~(CAVE_MARK);	// Forget the rune
			c_ptr->info &= ~(CAVE_OBJECT);	// Break the rune
			c_ptr->mimic = 0;

			//TODO note_spot(y, x);	// Notice
		}
		else return max_creature_idx;
	}

	creature_ptr = generate_creature(c_ptr, species_idx, GC_AUTO); 
	hack_m_idx_ii = c_ptr->creature_idx;

	// Hack -- Appearance transfer
	if(summoner_ptr && (mode & PC_MULTIPLY) && !is_player(summoner_ptr) && !is_original_ap(summoner_ptr))
	{
		creature_ptr->ap_species_idx = summoner_ptr->ap_species_idx;
		if(has_trait(summoner_ptr, TRAIT_KAGE)) set_timed_trait(creature_ptr, TRAIT_KAGE, PERMANENT_TIMED, FALSE);
	}

	// Sub-alignment of a creature
	if(summoner_ptr && !is_player(summoner_ptr) && !(is_enemy_of_evil_creature(summoner_ptr) && is_enemy_of_good_creature(summoner_ptr)))
		creature_ptr->sub_align = summoner_ptr->sub_align;
	else
	{
		creature_ptr->sub_align = SUB_ALIGN_NEUTRAL;
		if(is_enemy_of_good_creature(creature_ptr)) creature_ptr->sub_align |= SUB_ALIGN_EVIL;
		if(is_enemy_of_evil_creature(creature_ptr)) creature_ptr->sub_align |= SUB_ALIGN_GOOD;
	}

	// Place the creature at the location
	creature_ptr->floor_idx = get_floor_idx(floor_ptr);
	creature_ptr->depth = floor_ptr->depth;
	creature_ptr->fy = y;
	creature_ptr->fx = x;

	// Your pet summons its pet.
	if(summoner_ptr && !is_player(summoner_ptr) && is_pet(player_ptr, summoner_ptr))
	{
		mode |= PC_FORCE_PET;	//TODO Parent Set
	}

	if(has_trait_species(species_ptr, TRAIT_CHAMELEON))
	{
		set_new_species(&creature_list[c_ptr->creature_idx], TRUE, 0, MONEGO_NONE);
		species_ptr = &species_info[creature_ptr->species_idx];
		creature_ptr->sc_flag2 |= SC_FLAG2_CHAMELEON;

		/* Hack - Set sub_align to neutral when the Chameleon Lord is generated as "GUARDIAN" */
		if(summoner_ptr && (has_trait_species(species_ptr, TRAIT_UNIQUE)) && is_player(summoner_ptr))
			creature_ptr->sub_align = SUB_ALIGN_NEUTRAL;
	}
	else if((mode & PC_KAGE) && !(mode & PC_FORCE_PET))
	{
		creature_ptr->ap_species_idx = SPECIES_KAGE;
		set_timed_trait(creature_ptr, TRAIT_KAGE, PERMANENT_TIMED, FALSE);
	}

	if(mode & PC_NO_PET) set_timed_trait(creature_ptr, TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
	else if(summoner_ptr) set_pet(summoner_ptr, creature_ptr); // Pet?

	// TODO reimpelment Friendly Creature.

	if((mode & PC_ALLOW_SLEEP) && species_ptr->sleep && !has_trait(player_ptr, TRAIT_CURSE_OF_ILUVATAR)) // Enforce sleeping if needed
	{
		int val = species_ptr->sleep;
		(void)set_timed_trait(creature_ptr, TRAIT_SLEPT, (val * 2) + randint1(val * 10), FALSE);
	}

	if(mode & PC_HASTE) (void)set_timed_trait(creature_ptr, TRAIT_FAST, 100, FALSE);

	// Give a random starting energy
	if(!has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR)) creature_ptr->energy_need = ENERGY_NEED(100) - (s16b)randint0(100);
	// Nightmare creatures are more prepared
	else creature_ptr->energy_need = ENERGY_NEED(100) - (s16b)randint0(100) * 2;

	/* Force creature to wait for player, unless in Nightmare mode */
	if(has_trait(creature_ptr, TRAIT_FORCE_SLEEP) && !has_trait(creature_ptr, TRAIT_CURSE_OF_ILUVATAR))
	{
		creature_ptr->sc_flag |= (SC_FLAG_NICE);
		repair_creatures = TRUE;
	}

	/*TODO
	if(is_self_ld_creature(creature_ptr))
	prepare_update(creature_ptr, PU_SPECIES_LITE);
	else if(is_has_ld_creature(creature_ptr) && !has_trait(creature_ptr, TRAIT_PARALYZED))
	prepare_update(creature_ptr, PU_SPECIES_LITE);
	*/

	/* Update the creature */
	update_creature_view(player_ptr, c_ptr->creature_idx, TRUE);

	/* Count the creatures on the level */
	real_species_ptr(creature_ptr)->cur_num++;

	/*
	* Memorize location of the unique creature in saved floors.
	* A unique creature move from old saved floor.
	*/
	/*TODO
	if(floor_ptr->generated &&
	((has_trait_species(species_ptr, TRAIT_UNIQUE)) || has_trait_species(species_ptr, TRAIT_NAZGUL)))
	real_species_ptr(creature_ptr)->floor_idx = watcher_ptr->floor_idx;
	*/

	/* Hack -- Count the number of "reproducers" */
	if(has_trait(creature_ptr, TRAIT_MULTIPLY)) floor_ptr->num_repro++;

	/* TODO
	if(watcher_ptr->warning && floor_ptr->generated)
	{
	if(has_trait_species(species_ptr, TRAIT_UNIQUE))
	{
	cptr color;
	object_type *object_ptr;
	char object_name[MAX_NLEN];

	if(species_ptr->level > watcher_ptr->lev + 30)
	#ifdef JP
	color = "黒く";
	#else
	color = "black";
	#endif
	else if(species_ptr->level > watcher_ptr->lev + 15)
	#ifdef JP
	color = "紫色に";
	#else
	color = "purple";
	#endif
	else if(species_ptr->level > watcher_ptr->lev + 5)
	#ifdef JP
	color = "ルビー色に";
	#else
	color = "deep red";
	#endif
	else if(species_ptr->level > watcher_ptr->lev - 5)
	#ifdef JP
	color = "赤く";
	#else
	color = "red";
	#endif
	else if(species_ptr->level > watcher_ptr->lev - 15)
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

	object_ptr = choose_warning_item(watcher_ptr);
	if(object_ptr)
	{
	object_desc(object_name, object_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));
	#ifdef JP
	msg_format("%sは%s光った。", object_name, color);
	#else
	msg_format("%s glows %s.", object_name, color);
	#endif
	}
	else
	{
	#ifdef JP
	msg_format("%s光る物が頭に浮かんだ。", color);
	#else
	msg_format("An %s image forms in your mind.");
	#endif
	}
	}
	}
	*/

	if(is_explosive_rune_grid(c_ptr))
	{
		/* Break the ward */
		if(randint1(BREAK_MINOR_GLYPH) > species_ptr->level)
		{
			/* Describe observable breakage */
			if(c_ptr->info & CAVE_MARK)
			{
				msg_print(MES_BREAK_E_RUNE);
				project(NULL, 0, 2, y, x, diceroll(10, 7), DO_EFFECT_MANA, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}
		}
		else
			msg_print(MES_DISARM_E_RUNE);

		/* Forget the rune */
		c_ptr->info &= ~(CAVE_MARK);

		/* Break the rune */
		c_ptr->info &= ~(CAVE_OBJECT);
		c_ptr->mimic = 0;

		note_spot(floor_ptr, y, x);
		lite_spot(floor_ptr, y, x);
	}

	//strcpy(creature_ptr->name, species_name + species_ptr->name);
	creature_desc(creature_ptr->name, creature_ptr, CD_ASSUME_VISIBLE | CD_INDEF_VISIBLE);

	if(cheat_hear) msg_format("[Creature:%s Level:%d]", creature_ptr->name, creature_ptr->lev);
	return c_ptr->creature_idx;
}


#define CREATURE_SCAT_MAXD 10

static bool creature_scatter(SPECIES_ID species_idx, COODINATES *yp, COODINATES *xp, floor_type *floor_ptr, COODINATES y, COODINATES x, COODINATES max_dist)
{
	COODINATES place_x[CREATURE_SCAT_MAXD];
	COODINATES place_y[CREATURE_SCAT_MAXD];
	COODINATES nx, ny;
	int num[CREATURE_SCAT_MAXD];
	int i;

	if(max_dist >= CREATURE_SCAT_MAXD)
		return FALSE;

	for (i = 0; i < CREATURE_SCAT_MAXD; i++)
		num[i] = 0;

	for (nx = x - max_dist; nx <= x + max_dist; nx++)
	{
		for (ny = y - max_dist; ny <= y + max_dist; ny++)
		{
			/* Ignore annoying locations */
			if(!IN_BOUNDS(floor_ptr, ny, nx)) continue;

			/* Require "line of projection" */
			if(!projectable(floor_ptr, MAX_RANGE, y, x, ny, nx)) continue;

			if(species_idx > 0)
			{
				species_type *species_ptr = &species_info[species_idx];

				// Require empty space (if not ghostly)
				if(!species_can_enter(floor_ptr, ny, nx, species_ptr, 0))
					continue;
			}
			else
			{
				/* Walls and Creatures block flow */
				if(!cave_empty_bold2(floor_ptr, ny, nx)) continue;

				/* ... nor on the Pattern */
				if(pattern_tile(floor_ptr, ny, nx)) continue;
			}

			i = distance(y, x, ny, nx);

			if(i > max_dist)
				continue;

			num[i]++;

			/* random swap */
			if(one_in_(num[i]))
			{
				place_x[i] = nx;
				place_y[i] = ny;
			}
		}
	}

	i = 0;
	while (i < CREATURE_SCAT_MAXD && 0 == num[i])
		i++;
	if(i >= CREATURE_SCAT_MAXD)
		return FALSE;

	*xp = place_x[i];
	*yp = place_y[i];

	return TRUE;
}

// Maximum size of a group of creatures
#define GROUP_MAX	32

// Attempt to place a "group" of creatures around the given location
static bool place_creature_group(creature_type *summoner_ptr, floor_type *floor_ptr, int y, int x, SPECIES_ID species_idx, FLAGS_32 mode)
{
	species_type *species_ptr = &species_info[species_idx];

	int n, i;
	int total = 0, extra = 0;

	int hack_n = 0;

	COODINATES hack_y[GROUP_MAX];
	COODINATES hack_x[GROUP_MAX];


	/* Pick a group size */
	total = randint1(10);

	/* Hard creatures, small groups */
	if(species_ptr->level > floor_ptr->depth)
	{
		extra = species_ptr->level - floor_ptr->depth;
		extra = 0 - randint1(extra);
	}

	/* Easy creatures, large groups */
	else if(species_ptr->level < floor_ptr->depth)
	{
		extra = floor_ptr->depth - species_ptr->level;
		extra = randint1(extra);
	}

	/* Hack -- limit group reduction */
	if(extra > 9) extra = 9;

	/* Modify the group size */
	total += extra;

	/* Minimum size */
	if(total < 1) total = 1;

	/* Maximum size */
	if(total > GROUP_MAX) total = GROUP_MAX;

	/* Start on the creature */
	hack_n = 1;
	hack_x[0] = x;
	hack_y[0] = y;

	/* Puddle creatures, breadth first, up to total */
	for (n = 0; (n < hack_n) && (hack_n < total); n++)
	{
		/* Grab the location */
		COODINATES hx = hack_x[n];
		COODINATES hy = hack_y[n];

		/* Check each direction, up to total */
		for (i = 0; (i < 8) && (hack_n < total); i++)
		{
			COODINATES mx, my;

			scatter(floor_ptr, &my, &mx, hy, hx, 4, 0);

			/* Walls and Creatures block flow */
			if(!cave_empty_bold2(floor_ptr, my, mx)) continue;

			/* Attempt to place another creature */
			if(place_creature_one(summoner_ptr, floor_ptr, my, mx, species_idx, MONEGO_NORMAL, mode) != max_creature_idx)
			{
				/* Add it to the "hack" set */
				hack_y[hack_n] = my;
				hack_x[hack_n] = mx;
				hack_n++;
			}
		}
	}

	return TRUE;
}


/*
* Hack -- help pick an escort type
*/
static int place_species_idx = 0;
static int place_creature_m_idx = 0;

/*
* Hack -- help pick an escort type
*/
static bool place_creature_okay(creature_type *summoner_ptr, SPECIES_ID species_idx)
{
	species_type *species_ptr = &species_info[place_species_idx];
	creature_type *m_ptr = &creature_list[place_creature_m_idx];

	species_type *z_ptr = &species_info[species_idx];

	/* Hack - Escorts have to have the same dungeon flag */
	if(species_hook_dungeon(place_species_idx) != species_hook_dungeon(species_idx)) return FALSE;

	/* Require similar "race" */
	if(z_ptr->d_char != species_ptr->d_char) return FALSE;

	/* Skip more advanced creatures */
	if(z_ptr->level > species_ptr->level) return FALSE;

	/* Skip unique creatures */
	if(has_trait_species(z_ptr, TRAIT_UNIQUE)) return FALSE;


	if(place_species_idx == species_idx) return FALSE;

	/* Skip different alignment */
	if(creature_has_hostile_align(m_ptr, summoner_ptr)) return FALSE;

	if(has_trait_species(species_ptr, TRAIT_CHAMELEON) && !has_trait_species(z_ptr, TRAIT_CHAMELEON))
		return FALSE;


	return TRUE;
}


/*
* Attempt to place a creature of the given race at the given location
*
* Note that certain creatures are now marked as requiring "friends".
* These creatures, if successfully placed, and if the "grp" parameter
* is TRUE, will be surrounded by a "group" of identical creatures.
*
* Note that certain creatures are now marked as requiring an "escort",
* which is a collection of creatures with similar "race" but lower level.
*
* Some creatures induce a fake "group" flag on their escorts.
*
* Note the "bizarre" use of non-recursion to prevent annoying output
* when running a code profiler.
*
* Note the use of the new "creature allocation table" code to restrict
* the "get_species_num()" function to "legal" escort types.
*/
bool place_creature_fixed_species(creature_type *summoner_ptr, floor_type *floor_ptr, COODINATES y, COODINATES x, SPECIES_ID species_idx, FLAGS_32 mode)
{
	int             i, j;
	species_type    *species_ptr = &species_info[species_idx];
	creature_type   *servant_ptr;

	if(!(mode & PC_NO_KAGE) && one_in_(SHADOW_GENERATE_RATE)) mode |= PC_KAGE;

	// Place one creature, or fail
	i = place_creature_one(summoner_ptr, floor_ptr, y, x, species_idx, MONEGO_NORMAL, mode);
	if(i == max_creature_idx) return FALSE;

	servant_ptr = &creature_list[i];

	i = 0;
	while(i < MAX_UNDERLINGS && servant_ptr->underling_id[i])
	{
		int n = 0; 
		for(j = 0; j < servant_ptr->underling_num[i]; j++)
		{
			COODINATES nx, ny, d = 8;
			scatter(floor_ptr, &ny, &nx, y, x, d, 0);			// Pick a location
			if(!cave_empty_bold2(floor_ptr, ny, nx)) continue;	// Require empty grids

			if(place_creature_one(summoner_ptr, floor_ptr, ny, nx, servant_ptr->underling_id[i], MONEGO_NORMAL, mode) == max_creature_idx) n++;
		}
		servant_ptr->underling_num[i] -= n;
		i++;
	}

	// Require the "group" flag
	if(!(mode & PC_ALLOW_GROUP)) return TRUE;

	place_creature_m_idx = hack_m_idx_ii;

	// Friends for certain creatures
	if(has_trait_species(species_ptr, TRAIT_FRIENDLY)) (void)place_creature_group(summoner_ptr, floor_ptr, y, x, species_idx, mode);

	//TODO /* Escorts for certain creatures */

	return TRUE;
}


// Hack -- attempt to place a creature at the given location
// Attempt to find a creature appropriate to the "enemy_level"
bool place_floor_spawn_creature(creature_type *summoner_ptr, floor_type *floor_ptr, COODINATES y, COODINATES x, FLAGS_32 mode)
{
	PROB *prob_list;
	SPECIES_ID species_idx;
	
	alloc_species_list(&prob_list);
	//set_species_list_bias_terrain(prob_list, floor_ptr->); //TODO

	set_species_list_bias_level_limitation(&prob_list, floor_ptr->depth / 2, floor_ptr->depth);
	set_species_list_bias_feature(&prob_list, &feature_info[floor_ptr->cave[y][x].feat]);

	// Pick a creature
	//TODO get_creature_list_terrain(), get_creature_list_feature(y, x)
	species_idx = species_rand(prob_list);
	free_species_list(&prob_list);
	if(!species_idx) return FALSE;

	return place_creature_fixed_species(summoner_ptr, floor_ptr, y, x, species_idx, mode); // Attempt to place the creature
}

bool place_creature_horde(creature_type *summoner_ptr, floor_type *floor_ptr, COODINATES y, COODINATES x)
{
	species_type *species_ptr = NULL;
	SPECIES_ID species_idx = 0;
	CREATURE_ID m_idx;
	COODINATES cy = y, cx = x;
	int attempts = SAFE_MAX_ATTEMPTS;

	PROB *prob_list;

	alloc_species_list(&prob_list);
	//TODO set_species_list_bias_terrain(&prob_list, floor_ptr->
	set_species_list_bias_feature(&prob_list, &feature_info[floor_ptr->cave[y][x].feat]);
	species_idx = species_rand(prob_list);
	free_species_list(&prob_list);

	/* Attempt to place the creature */
	attempts = SAFE_MAX_ATTEMPTS;
	while (--attempts) if(place_creature_fixed_species(summoner_ptr, floor_ptr, y, x, species_idx, 0L)) break;
	if(attempts < 1) return FALSE;

	m_idx = floor_ptr->cave[y][x].creature_idx;

	if(creature_list[m_idx].sc_flag2 & SC_FLAG2_CHAMELEON) species_ptr = &species_info[creature_list[m_idx].species_idx];
	//summon_kin_type = species_ptr->d_char;

	for (attempts = randint1(10) + 5; attempts; attempts--)
	{
		scatter(floor_ptr, &cy, &cx, y, x, 5, 0);
		(void)summoning(&creature_list[m_idx], cy, cx, floor_ptr->depth + 5, TRAIT_S_KIN, PC_ALLOW_GROUP);
		y = cy;
		x = cx;
	}

	return TRUE;
}

/*
* Put the Guardian
*/
bool alloc_guardian(floor_type *floor_ptr, bool def_val)
{
	int guardian = dungeon_info[floor_ptr->dungeon_id].final_guardian;

	if(guardian && (dungeon_info[floor_ptr->dungeon_id].maxdepth == floor_ptr->depth) && (species_info[guardian].cur_num < species_info[guardian].max_num))
	{
		COODINATES oy, ox;
		int try = 4000;

		/* Find a good position */
		while (try)
		{
			/* Get a random spot */
			oy = (COODINATES)randint1(floor_ptr->height - 4) + 2;
			ox = (COODINATES)randint1(floor_ptr->width - 4) + 2;

			/* Is it a good spot ? */
			if(cave_empty_bold2(floor_ptr, oy, ox) && species_can_cross_terrain(floor_ptr->cave[oy][ox].feat, &species_info[guardian], 0))
			{
				/* Place the guardian */
				if(place_creature_fixed_species(NULL, floor_ptr, oy, ox, guardian, (PC_ALLOW_GROUP | PC_NO_KAGE | PC_NO_PET))) return TRUE;
			}

			/* One less try */
			try--;
		}

		return FALSE;
	}

	return def_val;
}


/*
* Attempt to allocate a random creature in the dungeon.
* Place the creature at least "dis" distance from the player.
* Use "slp" to choose the initial "sleep" status
* Use "enemy_level" for the creature level
*/
bool alloc_creature(floor_type *floor_ptr, creature_type *player_ptr, int dis, FLAGS_32 mode)
{
	COODINATES y = 0, x = 0;
	int attempts_left = 10000;

	// Put the Guardian
	if(alloc_guardian(floor_ptr, FALSE)) return TRUE;

	// Find a legal, distant, unoccupied, space
	while (attempts_left--)
	{
		// Pick a location
		y = (COODINATES)randint0(floor_ptr->height);
		x = (COODINATES)randint0(floor_ptr->width);

		// Require empty floor grid (was "naked")
		if(floor_ptr->depth)
		{
			if(!cave_empty_bold2(floor_ptr, y, x)) continue;
		}
		else
		{
			if(!cave_empty_bold(floor_ptr, y, x)) continue;
		}

		// Accept far away grids
		if(distance(y, x, player_ptr->fy, player_ptr->fx) > dis) break;
	}

	if(!attempts_left)
	{
		if(cheat_xtra || cheat_hear)
		{
#ifdef JP
			msg_warning("新たなクリーチャーを配置できません。小さい階ですか？");
#else
			msg_warning("Could not allocate a new creature. Small level?");
#endif
		}
		return FALSE;
	}

	if(randint1(5000) <= floor_ptr->depth)
	{
		if(place_creature_horde(NULL, floor_ptr, y, x))
		{
#ifdef JP
			if(cheat_hear) msg_format("クリーチャーの大群(%c)");
#else
			if(cheat_hear) msg_format("Creature horde (%c).");
#endif
			return TRUE;
		}
	}
	else
	{
		/* Attempt to place the creature, allow groups */
		if(place_floor_spawn_creature(NULL, floor_ptr, y, x, (mode | PC_ALLOW_GROUP))) return TRUE;

	}

	return FALSE;
}

// Hack -- help decide if a creature race is "okay" to summon
static bool summoning_okay(creature_type *summoner_ptr, SPECIES_ID species_idx)
{
	if(!species_hook_dungeon(species_idx)) return FALSE; // Hack - Only summon dungeon creatures

	if(summoner_ptr > 0)
	{
	//	if(creature_has_hostile_align(m_ptr, summoner_ptr)) return FALSE; // Friendly vs. opposite aligned normal or pet
	}

	return TRUE;
}


/*
* Place a creature (of the specified "type") near the given
* location.  Return TRUE if a creature was actually summoned.
*
* We will attempt to place the creature up to 10 times before giving up.
*
* Note: TRAIT_S_UNIQUE and TRAIT_S_AMBERITES will summon Unique's
* Note: TRAIT_S_HI_UNDEAD and TRAIT_S_HI_DRAGON may summon Unique's
* Note: None of the other summon codes will ever summon Unique's.
*
* This function has been changed.  We now take the "creature level"
* of the summoning creature as a parameter, and use that, along with
* the current dungeon level, to help determine the level of the
* desired creature.  Note that this is an upper bound, and also
* tends to "prefer" creatures of that level.  Currently, we use
* the average of the dungeon and creature levels, and then add
* five to allow slight increases in creature power.
*
* Note that we use the new "creature allocation table" creation code
* to restrict the "get_species_num(floor_ptr, )" function to the set of "legal"
* creatures, making this function much faster and more reliable.
*
* Note that this function may not succeed, though this is very rare.
*/
bool summoning(creature_type *summoner_ptr, COODINATES y1, COODINATES x1, FLOOR_LEV lev, int type, FLAGS_32 mode)
{
	COODINATES x, y;
	SPECIES_ID species_idx;
	floor_type *floor_ptr;
	PROB *prob_list;

	if(summoner_ptr) floor_ptr = GET_FLOOR_PTR(summoner_ptr);
	else floor_ptr = GET_FLOOR_PTR(player_ptr);
	if(floor_ptr) return FALSE;
	if(!creature_scatter(0, &y, &x, floor_ptr, y1, x1, 2)) return FALSE;

	alloc_species_list(&prob_list);
	//TODO set_species_list_bias_terrain(&prob_list, );
	set_species_list_bias_feature(&prob_list, &feature_info[floor_ptr->cave[y1][x1].feat]);
	set_species_list_bias_level_limitation(&prob_list, 0, (floor_ptr->depth + lev) / 2 + 5);
	species_idx = species_rand(prob_list);
	free_species_list(&prob_list);

	if(!species_idx) return FALSE; // Handle failure

	if((type == TRAIT_S_BLUE_HORROR) || (type == TRAIT_S_DAWN_LEGION)) mode |= PC_NO_KAGE;

	/* Attempt to place the creature (awake, allow groups) */
	if(!place_creature_fixed_species(summoner_ptr, floor_ptr, y, x, species_idx, mode)) return FALSE;

	return TRUE;
}

/* A "dangerous" function, creates a pet of the specified type */
bool summon_named_creature(creature_type *creature_ptr, floor_type *floor_ptr, COODINATES oy, COODINATES ox, SPECIES_ID species_idx, FLAGS_32 mode)
{
	COODINATES x, y;

	// Prevent illegal creatures
	if(species_idx >= max_species_idx) return FALSE;
	if(floor_ptr->fight_arena_mode) return FALSE;
	if(!creature_scatter(species_idx, &y, &x, floor_ptr, oy, ox, 2)) return FALSE;

	// Place it (allow groups)
	return place_creature_fixed_species(creature_ptr, floor_ptr, y, x, species_idx, (mode | PC_NO_KAGE));
}



// Let the given creature attempt to reproduce.
// Note that "reproduction" REQUIRES empty space.
bool multiply_creature(creature_type *creature_ptr, bool clone, FLAGS_32 mode)
{
	floor_type *floor_ptr = &floor_list[creature_ptr->floor_idx];

	COODINATES y, x;

	if(!creature_scatter(creature_ptr->species_idx, &y, &x, floor_ptr, creature_ptr->fy, creature_ptr->fx, 1))
		return FALSE;

	if(has_trait(creature_ptr, TRAIT_NO_PET)) mode |= PC_NO_PET;

	// Create a new creature (awake, no groups)
	if(!place_creature_fixed_species(creature_ptr, floor_ptr, y, x, creature_ptr->species_idx, (mode | PC_NO_KAGE | PC_MULTIPLY))) return FALSE;

	/* Hack -- Transfer "clone" flag */
	if(clone || has_trait(creature_ptr, TRAIT_CLONED))
	{
		set_timed_trait(&creature_list[hack_m_idx_ii], TRAIT_CLONED, PERMANENT_TIMED, FALSE);
		set_timed_trait(&creature_list[hack_m_idx_ii], TRAIT_NO_PET, PERMANENT_TIMED, FALSE);
	}

	return TRUE;
}


// Dump a message describing a creature's reaction to damage
// Technically should attempt to treat "Beholder"'s as jelly's
void message_pain(CREATURE_ID creature_idx, POWER dam)
{
	long oldhp, newhp, tmp;
	int percentage;
	creature_type *creature_ptr = &creature_list[creature_idx];
	char creature_name[MAX_NLEN];

	creature_desc(creature_name, creature_ptr, 0); // Get the creature name

	if(dam == 0) // Notice non-damage
	{
#ifdef JP
		msg_format("%^sはダメージを受けていない。", creature_name);
#else
		msg_format("%^s is unharmed.", creature_name);
#endif
		return;
	}

	newhp = (long)(creature_ptr->chp);
	oldhp = newhp + (long)(dam);
	tmp = (newhp * 100L) / oldhp;
	percentage = (int)(tmp);

	if(my_strchr(",ejmvwQ", creature_ptr->d_char)) // Mushrooms, Eyes, Jellies, Molds, Vortices, Worms, Quylthulgs
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sはほとんど気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%^sはしり込みした。", creature_name);
		else if(percentage > 50) msg_format("%^sは縮こまった。", creature_name);
		else if(percentage > 35) msg_format("%^sは痛みに震えた。", creature_name);
		else if(percentage > 20) msg_format("%^sは身もだえした。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦痛で身もだえした。", creature_name);
		else msg_format("%^sはぐにゃぐにゃと痙攣した。", creature_name);
#else
		if(percentage > 95) msg_format("%^s barely notices.", creature_name);
		else if(percentage > 75) msg_format("%^s flinches.", creature_name);
		else if(percentage > 50) msg_format("%^s squelches.", creature_name);
		else if(percentage > 35) msg_format("%^s quivers in pain.", creature_name);
		else if(percentage > 20) msg_format("%^s writhes about.", creature_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", creature_name);
		else msg_format("%^s jerks limply.", creature_name);
#endif
	}

	else if(my_strchr("l", creature_ptr->d_char)) // Fish
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sはほとんど気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%^sはしり込みした。", creature_name);
		else if(percentage > 50) msg_format("%^sは躊躇した。", creature_name);
		else if(percentage > 35) msg_format("%^sは痛みに震えた。", creature_name);
		else if(percentage > 20) msg_format("%^sは身もだえした。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦痛で身もだえした。", creature_name);
		else msg_format("%^sはぐにゃぐにゃと痙攣した。", creature_name);
#else
		if(percentage > 95) msg_format("%^s barely notices.", creature_name);
		else if(percentage > 75) msg_format("%^s flinches.", creature_name);
		else if(percentage > 50) msg_format("%^s hesitates.", creature_name);
		else if(percentage > 35) msg_format("%^s quivers in pain.", creature_name);
		else if(percentage > 20) msg_format("%^s writhes about.", creature_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", creature_name);
		else msg_format("%^s jerks limply.", creature_name);
#endif		
	}

	else if(my_strchr("g#+<>", creature_ptr->d_char)) // Golems, Walls, Doors, Stairs
	{	
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%sは攻撃に肩をすくめた。", creature_name);
		else if(percentage > 50) msg_format("%^sは雷鳴のように吠えた。", creature_name);
		else if(percentage > 35) msg_format("%^sは苦しげに吠えた。", creature_name);
		else if(percentage > 20) msg_format("%^sはうめいた。", creature_name);
		else if(percentage > 10) msg_format("%^sは躊躇した。", creature_name);
		else msg_format("%^sはくしゃくしゃになった。", creature_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s shrugs off the attack.", creature_name);
		else if(percentage > 50) msg_format("%^s roars thunderously.", creature_name);
		else if(percentage > 35) msg_format("%^s rumbles.", creature_name);
		else if(percentage > 20) msg_format("%^s grunts.", creature_name);
		else if(percentage > 10) msg_format("%^s hesitates.", creature_name);
		else msg_format("%^s crumples.", creature_name);
#endif
	}

	else if(my_strchr("JMR", creature_ptr->d_char) || !isalpha(creature_ptr->d_char)) // Snakes, Hydrae, Reptiles, Mimics
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sはほとんど気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%^sはシーッと鳴いた。", creature_name);
		else if(percentage > 50) msg_format("%^sは怒って頭を上げた。", creature_name);
		else if(percentage > 35) msg_format("%^sは猛然と威嚇した。", creature_name);
		else if(percentage > 20) msg_format("%^sは身もだえした。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦痛で身もだえした。", creature_name);
		else msg_format("%^sはぐにゃぐにゃと痙攣した。", creature_name);
#else
		if(percentage > 95) msg_format("%^s barely notices.", creature_name);
		else if(percentage > 75) msg_format("%^s hisses.", creature_name);
		else if(percentage > 50) msg_format("%^s rears up in anger.", creature_name);
		else if(percentage > 35) msg_format("%^s hisses furiously.", creature_name);
		else if(percentage > 20) msg_format("%^s writhes about.", creature_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", creature_name);
		else msg_format("%^s jerks limply.", creature_name);
#endif
	}

	else if(my_strchr("f", creature_ptr->d_char))
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃に肩をすくめた。", creature_name);
		else if(percentage > 75) msg_format("%^sは吠えた。", creature_name);
		else if(percentage > 50) msg_format("%^sは怒って吠えた。", creature_name);
		else if(percentage > 35) msg_format("%^sは痛みでシーッと鳴いた。", creature_name);
		else if(percentage > 20) msg_format("%^sは痛みで弱々しく鳴いた。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦痛にうめいた。", creature_name);
		else msg_format("%sは哀れな鳴き声を出した。", creature_name);
#else
		if(percentage > 95) msg_format("%^s shrugs off the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s roars.", creature_name);
		else if(percentage > 50) msg_format("%^s growls angrily.", creature_name);
		else if(percentage > 35) msg_format("%^s hisses with pain.", creature_name);
		else if(percentage > 20) msg_format("%^s mewls in pain.", creature_name);
		else if(percentage > 10) msg_format("%^s hisses in agony.", creature_name);
		else msg_format("%^s mewls pitifully.", creature_name);
#endif
	}

	else if(my_strchr("acFIKS", creature_ptr->d_char)) // Ants, Centipedes, Flies, Insects, Beetles, Spiders
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%^sはキーキー鳴いた。", creature_name);
		else if(percentage > 50) msg_format("%^sはヨロヨロ逃げ回った。", creature_name);
		else if(percentage > 35) msg_format("%^sはうるさく鳴いた。", creature_name);
		else if(percentage > 20) msg_format("%^sは痛みに痙攣した。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦痛で痙攣した。", creature_name);
		else msg_format("%^sはピクピクひきつった。", creature_name);
#else
		if(percentage > 95)	msg_format("%^s ignores the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s chitters.", creature_name);
		else if(percentage > 50) msg_format("%^s scuttles about.", creature_name);
		else if(percentage > 35) msg_format("%^s twitters.", creature_name);
		else if(percentage > 20) msg_format("%^s jerks in pain.", creature_name);
		else if(percentage > 10) msg_format("%^s jerks in agony.", creature_name);
		else msg_format("%^s twitches.", creature_name);
#endif
	}

	else if(my_strchr("B", creature_ptr->d_char)) // Birds
	{		
#ifdef JP
		if(percentage > 95) msg_format("%^sはさえずった。", creature_name);
		else if(percentage > 75) msg_format("%^sはピーピー鳴いた。", creature_name);
		else if(percentage > 50) msg_format("%^sはギャーギャー鳴いた。", creature_name);
		else if(percentage > 35) msg_format("%^sはギャーギャー鳴きわめいた。", creature_name);
		else if(percentage > 20) msg_format("%^sは苦しんだ。", creature_name);
		else if(percentage > 10) msg_format("%^sはのたうち回った。", creature_name);
		else msg_format("%^sはキーキーと鳴き叫んだ。", creature_name);
#else
		if(percentage > 95)	msg_format("%^s chirps.", creature_name);
		else if(percentage > 75) msg_format("%^s twitters.", creature_name);
		else if(percentage > 50) msg_format("%^s squawks.", creature_name);
		else if(percentage > 35) msg_format("%^s chatters.", creature_name);
		else if(percentage > 20) msg_format("%^s jeers.", creature_name);
		else if(percentage > 10) msg_format("%^s flutters about.", creature_name);
		else msg_format("%^s squeaks.", creature_name);
#endif
	}

	else if(my_strchr("duDLUW", creature_ptr->d_char)) // Dragons, Demons, High Undead
	{	
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%^sはしり込みした。", creature_name);
		else if(percentage > 50) msg_format("%^sは痛みでシーッと鳴いた。", creature_name);
		else if(percentage > 35) msg_format("%^sは痛みでうなった。", creature_name);
		else if(percentage > 20) msg_format("%^sは痛みに吠えた。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦しげに叫んだ。", creature_name);
		else msg_format("%^sは弱々しくうなった。", creature_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s flinches.", creature_name);
		else if(percentage > 50) msg_format("%^s hisses in pain.", creature_name);
		else if(percentage > 35) msg_format("%^s snarls with pain.", creature_name);
		else if(percentage > 20) msg_format("%^s roars with pain.", creature_name);
		else if(percentage > 10) msg_format("%^s gasps.", creature_name);
		else msg_format("%^s snarls feebly.", creature_name);
#endif
	}

	else if(my_strchr("s", creature_ptr->d_char)) // Skeletons
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%sは攻撃に肩をすくめた。", creature_name);
		else if(percentage > 50) msg_format("%^sはカタカタと笑った。", creature_name);
		else if(percentage > 35) msg_format("%^sはよろめいた。", creature_name);
		else if(percentage > 20) msg_format("%^sはカタカタ言った。", creature_name);
		else if(percentage > 10) msg_format("%^sはよろめいた。", creature_name);
		else msg_format("%^sはガタガタ言った。", creature_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s shrugs off the attack.", creature_name);
		else if(percentage > 50) msg_format("%^s rattles.", creature_name);
		else if(percentage > 35) msg_format("%^s stumbles.", creature_name);
		else if(percentage > 20) msg_format("%^s rattles.", creature_name);
		else if(percentage > 10) msg_format("%^s staggers.", creature_name);
		else msg_format("%^s clatters.", creature_name);
#endif
	}

	else if(my_strchr("z", creature_ptr->d_char)) // Zombies
	{		
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%sは攻撃に肩をすくめた。", creature_name);
		else if(percentage > 50) msg_format("%^sはうめいた。", creature_name);
		else if(percentage > 35) msg_format("%sは苦しげにうめいた。", creature_name);
		else if(percentage > 20) msg_format("%^sは躊躇した。", creature_name);
		else if(percentage > 10) msg_format("%^sはうなった。", creature_name);
		else msg_format("%^sはよろめいた。", creature_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s shrugs off the attack.", creature_name);
		else if(percentage > 50) msg_format("%^s groans.", creature_name);
		else if(percentage > 35) msg_format("%^s moans.", creature_name);
		else if(percentage > 20) msg_format("%^s hesitates.", creature_name);
		else if(percentage > 10) msg_format("%^s grunts.", creature_name);
		else msg_format("%^s staggers.", creature_name);
#endif
	}

	else if(my_strchr("G", creature_ptr->d_char)) // Ghosts
	{
#ifdef JP
		if(percentage > 95) msg_format("%sは攻撃を気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%sは攻撃に肩をすくめた。", creature_name);
		else if(percentage > 50) msg_format("%sはうめいた。", creature_name);
		else if(percentage > 35) msg_format("%^sは泣きわめいた。", creature_name);
		else if(percentage > 20) msg_format("%^sは吠えた。", creature_name);
		else if(percentage > 10) msg_format("%sは弱々しくうめいた。", creature_name);
		else msg_format("%^sはかすかにうめいた。", creature_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s shrugs off the attack.", creature_name);
		else if(percentage > 50)  msg_format("%^s moans.", creature_name);
		else if(percentage > 35) msg_format("%^s wails.", creature_name);
		else if(percentage > 20) msg_format("%^s howls.", creature_name);
		else if(percentage > 10) msg_format("%^s moans softly.", creature_name);
		else msg_format("%^s sighs.", creature_name);
#endif
	}

	else if(my_strchr("CZ", creature_ptr->d_char)) // Dogs and Hounds
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sは攻撃に肩をすくめた。", creature_name);
		else if(percentage > 75) msg_format("%^sは痛みでうなった。", creature_name);
		else if(percentage > 50) msg_format("%^sは痛みでキャンキャン吠えた。", creature_name);
		else if(percentage > 35) msg_format("%^sは痛みで鳴きわめいた。", creature_name);
		else if(percentage > 20) msg_format("%^sは苦痛のあまり鳴きわめいた。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦痛でもだえ苦しんだ。", creature_name);
		else msg_format("%^sは弱々しく吠えた。", creature_name);
#else
		if(percentage > 95) msg_format("%^s shrugs off the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s snarls with pain.", creature_name);
		else if(percentage > 50) msg_format("%^s yelps in pain.", creature_name);
		else if(percentage > 35) msg_format("%^s howls in pain.", creature_name);
		else if(percentage > 20) msg_format("%^s howls in agony.", creature_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", creature_name);
		else msg_format("%^s yelps feebly.", creature_name);
#endif
	}

	else if(my_strchr("Xbilqrt", creature_ptr->d_char)) // One type of creatures (ignore,squeal,shriek)
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sは攻撃を気にとめていない。", creature_name);
		else if(percentage > 75) msg_format("%^sは痛みでうなった。", creature_name);
		else if(percentage > 50) msg_format("%^sは痛みで叫んだ。", creature_name);
		else if(percentage > 35) msg_format("%^sは痛みで絶叫した。", creature_name);
		else if(percentage > 20) msg_format("%^sは苦痛のあまり絶叫した。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦痛でもだえ苦しんだ。", creature_name);
		else msg_format("%^sは弱々しく叫んだ。", creature_name);
#else
		if(percentage > 95) msg_format("%^s ignores the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s grunts with pain.", creature_name);
		else if(percentage > 50) msg_format("%^s squeals in pain.", creature_name);
		else if(percentage > 35) msg_format("%^s shrieks in pain.", creature_name);
		else if(percentage > 20) msg_format("%^s shrieks in agony.", creature_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", creature_name);
		else msg_format("%^s cries out feebly.", creature_name);
#endif
	}

	else // Another type of creatures (shrug,cry,scream)
	{
#ifdef JP
		if(percentage > 95) msg_format("%^sは攻撃に肩をすくめた。", creature_name);
		else if(percentage > 75) msg_format("%^sは痛みでうなった。", creature_name);
		else if(percentage > 50) msg_format("%^sは痛みで叫んだ。", creature_name);
		else if(percentage > 35) msg_format("%^sは痛みで絶叫した。", creature_name);
		else if(percentage > 20) msg_format("%^sは苦痛のあまり絶叫した。", creature_name);
		else if(percentage > 10) msg_format("%^sは苦痛でもだえ苦しんだ。", creature_name);
		else msg_format("%^sは弱々しく叫んだ。", creature_name);
#else
		if(percentage > 95) msg_format("%^s shrugs off the attack.", creature_name);
		else if(percentage > 75) msg_format("%^s grunts with pain.", creature_name);
		else if(percentage > 50) msg_format("%^s cries out in pain.", creature_name);
		else if(percentage > 35) msg_format("%^s screams in pain.", creature_name);
		else if(percentage > 20) msg_format("%^s screams in agony.", creature_name);
		else if(percentage > 10) msg_format("%^s writhes in agony.", creature_name);
		else msg_format("%^s cries out feebly.", creature_name);
#endif
	}
}

// Drop all items carried by a creature
void creature_drop_carried_objects(creature_type *creature_ptr)
{
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	int i;
	for(i = 0; i < INVEN_TOTAL; i++)
		if(is_valid_object(&creature_ptr->inventory[i]))
			(void)drop_near(floor_ptr, &creature_ptr->inventory[i], 25, creature_ptr->fy, creature_ptr->fx);
}

// Move the creature
bool move_creature(creature_type *creature_ptr, floor_type *floor_ptr, COODINATES ny, COODINATES nx, u32b mpe_mode)
{
	floor_type *prev_floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &prev_floor_ptr->cave[ny][nx];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	if(!floor_ptr) floor_ptr = GET_FLOOR_PTR(creature_ptr);
	if(floor_ptr->global_map) reveal_wilderness(ny, nx);

	// Creature status adjustment (Remove all mirrors without explosion / Cut supersthealth)
	if(!prev_floor_ptr || prev_floor_ptr != floor_ptr)
	{
		remove_all_mirrors(creature_ptr, prev_floor_ptr, FALSE);
		if(creature_ptr->posture & NINJA_S_STEALTH) set_superstealth(creature_ptr, FALSE);
		creature_ptr->feeling_turn = 0;
		creature_ptr->floor_feeling = 0;
		creature_ptr->floor_idx = get_floor_idx(floor_ptr);
	}

	if(!(mpe_mode & MCE_STAYING))
	{
		COODINATES oy = creature_ptr->fy;
		COODINATES ox = creature_ptr->fx;
		cave_type *oc_ptr = &prev_floor_ptr->cave[oy][ox];
		CREATURE_ID om_idx = oc_ptr->creature_idx;
		CREATURE_ID nm_idx = c_ptr->creature_idx;

		/* Move the player */
		if(ny) creature_ptr->fy = ny;
		if(nx) creature_ptr->fx = nx;

		/* Hack -- For moving creature or riding player's moving */
		if(!(mpe_mode & MCE_DONT_SWAP_MON))
		{
			/* Swap two creatures */
			c_ptr->creature_idx = om_idx;
			oc_ptr->creature_idx = nm_idx;

			if(om_idx > 0) /* Creature on old spot (or creature_ptr->riding) */
			{
				creature_type *om_ptr = &creature_list[om_idx];
				om_ptr->fy = ny;
				om_ptr->fx = nx;
				update_creature_view(player_ptr, om_idx, TRUE);
			}

			if(nm_idx > 0) /* Creature on new spot */
			{
				creature_type *nm_ptr = &creature_list[nm_idx];
				nm_ptr->fy = oy;
				nm_ptr->fx = ox;
				update_creature_view(player_ptr, nm_idx, TRUE);
			}
		}

		if(!(mpe_mode & MCE_NO_SEE))
		{
			lite_spot(prev_floor_ptr, oy, ox);
			lite_spot(prev_floor_ptr, ny, nx);
		}

		// Check for new panel (redraw map)
		verify_panel(creature_ptr);

		if(mpe_mode & MCE_FORGET_FLOW)
		{
			forget_flow(prev_floor_ptr);
			prepare_update(creature_ptr, PU_UN_VIEW);
			prepare_redraw(PR_MAP);
		}

		prepare_update(creature_ptr, PU_VIEW | PU_LITE | PU_FLOW | PU_SPECIES_LITE | PU_DISTANCE);
		prepare_window(PW_OVERHEAD | PW_DUNGEON);

		/* Remove "unsafe" flag */
		if((!has_trait(creature_ptr, TRAIT_BLIND) && !no_lite(creature_ptr)) || !is_trap(c_ptr->feat)) c_ptr->info &= ~(CAVE_UNSAFE);

		/* For get everything when requested hehe I'm *NASTY* */
		if(prev_floor_ptr->depth && (dungeon_info[prev_floor_ptr->dungeon_id].flags1 & DF1_FORGET)) wiz_dark(prev_floor_ptr, creature_ptr);

		if(mpe_mode & MCE_HANDLE_STUFF) handle_stuff(creature_ptr);

		if(creature_ptr->class_idx == CLASS_NINJA)
		{
			if(c_ptr->info & (CAVE_GLOW)) set_superstealth(creature_ptr, FALSE);
			else if(creature_ptr->cur_lite <= 0) set_superstealth(creature_ptr, TRUE);
		}

		if((creature_ptr->action == ACTION_HAYAGAKE) && (!have_flag(f_ptr->flags, FF_PROJECT) ||
		     (!has_trait(creature_ptr, TRAIT_CAN_FLY) && have_flag(f_ptr->flags, FF_DEEP))))
		{

			if(is_player(creature_ptr)) msg_print(MES_HAYAGAKE_PREVENT);
			cost_tactical_energy(creature_ptr, 100);
			set_action(creature_ptr, ACTION_NONE);
		}
	}

	if(mpe_mode & MCE_ENERGY_USE)
	{
		if(MUSIC_SINGING(creature_ptr, MUSIC_WALL))
		{
			(void)project(creature_ptr, 0, 0, creature_ptr->fy, creature_ptr->fx, (60 + creature_ptr->lev), DO_EFFECT_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM, -1);
			if(!CREATURE_BOLD(creature_ptr, ny, nx) || gameover || subject_change_floor) return FALSE;
		}

		/* Spontaneous Searching */
		if((creature_ptr->skill_fos >= 50) || (0 == randint0(50 - creature_ptr->skill_fos))) search(creature_ptr);

		/* Continuous Searching */
		if(creature_ptr->action == ACTION_SEARCH) search(creature_ptr);
	}

	// Handle "objects"
	if(!(mpe_mode & MCE_DONT_PICKUP)) carry(creature_ptr, (mpe_mode & MCE_DO_PICKUP) ? TRUE : FALSE);

	// Handle "store doors"
	if(have_flag(f_ptr->flags, FF_STORE) && !(mpe_mode & MCE_NO_ENTER))
	{
		disturb(creature_ptr, 0, 0);
		cancel_tactical_action(creature_ptr);
		command_new = SPECIAL_KEY_STORE;
	}

	// Handle "building doors" -KMW-
	else if(have_flag(f_ptr->flags, FF_BLDG) && !(mpe_mode & MCE_NO_ENTER))
	{
		disturb(player_ptr, 0, 0);
		cancel_tactical_action(creature_ptr);
		command_new = SPECIAL_KEY_BUILDING;
	}

	// Handle quest areas -KMW-
	else if(have_flag(f_ptr->flags, FF_QUEST_ENTER) && !(mpe_mode & MCE_NO_ENTER))
	{
		disturb(player_ptr, 0, 0);

		cancel_tactical_action(creature_ptr);
		command_new = SPECIAL_KEY_QUEST;
	}

	else if(have_flag(f_ptr->flags, FF_QUEST_EXIT) && !(mpe_mode & MCE_NO_ENTER))
	{
		if(quest[floor_ptr->quest].type == QUEST_TYPE_FIND_EXIT)
		{
			if(record_fix_quest) write_diary(DIARY_FIX_QUEST_C, floor_ptr->quest, NULL);
			quest[floor_ptr->quest].status = QUEST_STATUS_COMPLETED;
			quest[floor_ptr->quest].complev = (byte)creature_ptr->lev;
			msg_print(MES_COMPLETE_QUEST);
			msg_print(NULL);
		}

		leave_quest_check(creature_ptr);

		floor_ptr->quest = c_ptr->quest_idx;
		prev_floor_ptr->depth = 0;
		creature_ptr->oldpx = 0;
		creature_ptr->oldpy = 0;

		subject_change_floor = TRUE;
	}

	/* Set off a trap */
	else if(have_flag(f_ptr->flags, FF_HIT_TRAP) && !(mpe_mode & MCE_STAYING))
	{
		disturb(player_ptr, 0, 0);

		/* Hidden trap */
		if(c_ptr->mimic || have_flag(f_ptr->flags, FF_SECRET))
		{
			msg_print(MES_TRAP_FOUND);

			/* Pick a trap */
			disclose_grid(prev_floor_ptr, creature_ptr->fy, creature_ptr->fx);
		}

		/* Hit the trap */
		hit_trap(creature_ptr, (mpe_mode & MCE_BREAK_TRAP) ? TRUE : FALSE);

		if(!CREATURE_BOLD(creature_ptr, ny, nx) || gameover || subject_change_floor) return FALSE;
	}

	/* Warn when leaving trap detected region */
	if(!(mpe_mode & MCE_STAYING) && (disturb_trap_detect || alert_trap_detect)
	    && detect_trap && !(c_ptr->info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		detect_trap = FALSE;

		/* You are just on the edge */
		if(!(c_ptr->info & CAVE_UNSAFE))
		{
			if(alert_trap_detect) msg_print(MES_TRAP_WARNING);
			if(disturb_trap_detect) disturb(player_ptr, 0, 0);
		}
	}

	creature_ptr->depth = floor_ptr->depth;
	return CREATURE_BOLD(creature_ptr, ny, nx) && !gameover && !subject_change_floor;
}



// Walk player in the given direction, with the given "pickup" flag.
// This routine should (probably) always induce energy expenditure.
// Note that moving will *always* take a turn, and will *always* hit
// any creature which might be in the destination grid.  Previously,
// moving into walls was "free" and did NOT hit invisible creatures.
void walk_creature(creature_type *creature_ptr, DIRECTION dir, bool do_pickup, bool break_trap)
{
	// Find the result of moving
	COODINATES y = creature_ptr->fy + ddy[dir];
	COODINATES x = creature_ptr->fx + ddx[dir];

	// Examine the destination
	floor_type *floor_ptr = GET_FLOOR_PTR(creature_ptr);
	cave_type *c_ptr = &floor_ptr->cave[y][x];
	feature_type *f_ptr = &feature_info[c_ptr->feat];

	creature_type *target_ptr;
	creature_type *steed_ptr = &creature_list[creature_ptr->riding];

	char m_name[MAX_NLEN];

	bool can_enter = creature_can_cross_terrain(creature_ptr, c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);
	bool can_kill_walls = FALSE;
	bool stormbringer = FALSE;

	bool oktomove = TRUE;
	bool do_past = FALSE;

	// Exit the area
	exit_area(creature_ptr, x, y);

	/* Get the creature */
	target_ptr = &creature_list[c_ptr->creature_idx];

	/* Player can not walk through "walls"... */
	/* unless in Shadow Form */
	can_kill_walls = has_trait(creature_ptr, TRAIT_KILL_WALL) && have_flag(f_ptr->flags, FF_HURT_DISI) &&
		(!can_enter || !have_flag(f_ptr->flags, FF_LOS)) && !have_flag(f_ptr->flags, FF_PERMANENT);

	/* Hack -- attack creatures */
	if(c_ptr->creature_idx && (target_ptr->see_others || can_enter || can_kill_walls))
	{
		/* Attack -- only if we can see it OR it is not in a wall */
		if(!is_hostile(target_ptr) &&
		    !(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_HALLUCINATION) || !target_ptr->see_others || has_trait(creature_ptr, TRAIT_STUN) ||
		    has_trait(creature_ptr, TRAIT_BERS_RAGE) && has_trait(creature_ptr, TRAIT_S_HERO)) &&
		    pattern_seq(creature_ptr, creature_ptr->fy, creature_ptr->fx, y, x) && (can_enter || can_kill_walls))
		{
			
			(void)set_timed_trait(target_ptr, TRAIT_PARALYZED, 0, TRUE); // Disturb the creature
			creature_desc(m_name, target_ptr, 0); // Extract creature name (or "it")

			if(target_ptr->see_others)
			{
				/* Auto-Recall if possible and visible */
				if(!has_trait(creature_ptr, TRAIT_HALLUCINATION)) species_type_track(target_ptr->ap_species_idx);

				/* Track a new creature */
				health_track(c_ptr->creature_idx);
			}

			/* displace? */
			if((stormbringer && (randint1(1000) > 666)) || (creature_ptr->class_idx == CLASS_BERSERKER))
			{
				close_combat(creature_ptr, y, x, 0);
				oktomove = FALSE;
			}
			else if(creature_can_cross_terrain(creature_ptr, floor_ptr->cave[creature_ptr->fy][creature_ptr->fx].feat, 0))
				do_past = TRUE;
			else
			{
				msg_format(MES_PREVENT_BY_CREATURE(m_name));
				cancel_tactical_action(creature_ptr);
				oktomove = FALSE;
			}

			/* now continue on to 'movement' */
		}
		else
		{
			close_combat(creature_ptr, y, x, 0);
			oktomove = FALSE;
		}
	}

	if(oktomove && creature_ptr->riding)
	{
		if(has_trait(steed_ptr, TRAIT_NEVER_MOVE))
		{
			msg_print(MES_WALK_NEVER_MOVE);
			cancel_tactical_action(creature_ptr);
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}

		else if(has_trait(steed_ptr, TRAIT_AFRAID))
		{
			char m_name[MAX_NLEN];

			/* Acquire the creature name */
			creature_desc(m_name, steed_ptr, 0);

			msg_format(MES_WALK_STEED_FEAR(m_name));
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if(creature_ptr->riding_two_handed)
		{
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if(have_flag(f_ptr->flags, FF_CAN_FLY) && has_trait(steed_ptr, TRAIT_CAN_FLY))
		{
			// Allow moving
		}
		else if(have_flag(f_ptr->flags, FF_CAN_SWIM) && has_trait(steed_ptr, TRAIT_CAN_SWIM))
		{
			// Allow moving
		}
		else if(have_flag(f_ptr->flags, FF_WATER) &&
			!has_trait(steed_ptr, TRAIT_AQUATIC) &&
			(have_flag(f_ptr->flags, FF_DEEP) || has_trait(creature_ptr, TRAIT_AURA_FIRE)))
		{
			msg_format(MES_WALK_NEVER_SWIM);
			cancel_tactical_action(creature_ptr);
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if(!have_flag(f_ptr->flags, FF_WATER) && has_trait(steed_ptr, TRAIT_AQUATIC))
		{
			msg_format(MES_WALK_NEVER_LAND);
			cancel_tactical_action(creature_ptr);
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
		else if(have_flag(f_ptr->flags, FF_LAVA) && !has_trait(steed_ptr, TRAIT_RES_FIRE))
		{
			msg_format(MES_WALK_TOO_HOT(feature_name + feature_info[get_feat_mimic(c_ptr)].name));
			cancel_tactical_action(creature_ptr);
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}

		if(oktomove && has_trait(steed_ptr, TRAIT_STUN) && one_in_(2))
		{
			char m_name[MAX_NLEN];
			creature_desc(m_name, steed_ptr, 0);
			msg_format(MES_WALK_STEED_STUNED(m_name));
			oktomove = FALSE;
			disturb(player_ptr, 0, 0);
		}
	}

	else if(!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !has_trait(creature_ptr, TRAIT_CAN_FLY))
	{
		msg_format(MES_WALK_MUST_FLY(feature_name + feature_info[get_feat_mimic(c_ptr)].name));
		cancel_tactical_action(creature_ptr);
		creature_ptr->running = 0;
		oktomove = FALSE;
	}

	// Disarm a visible trap
	else if((do_pickup != easy_disarm) && have_flag(f_ptr->flags, FF_DISARM) && !c_ptr->mimic)
	{
		if(!trap_can_be_ignored(creature_ptr, c_ptr->feat))
		{
			(void)do_cmd_disarm_aux(creature_ptr, y, x, dir);
			return;
		}
	}

	// Player can not walk through "walls" unless in wraith form...
	else if(!can_enter && !can_kill_walls)
	{
		/* Feature code (applying "mimic" field) */
		FEATURE_ID feat = get_feat_mimic(c_ptr);
		feature_type *mimic_f_ptr = &feature_info[feat];
		cptr name = feature_name + mimic_f_ptr->name;

		oktomove = FALSE;

		disturb(player_ptr, 0, 0);

		/* Notice things in the dark */
		if(!(c_ptr->info & CAVE_MARK) && !creature_can_see_bold(creature_ptr, y, x))
		{
			/* Boundary floor mimic */
			if(boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
				msg_print(MES_CANNOT_GO_MORE);

			/* Wall (or secret door) */
			else
			{
				msg_format(MES_WALK_BLOCK(name));
				c_ptr->info |= (CAVE_MARK);
				lite_spot(floor_ptr, y, x);
			}
		}

		/* Notice things */
		else
		{
			// Boundary floor mimic
			if(boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
				msg_print(MES_CANNOT_GO_MORE);
				if(!(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_STUN) || has_trait(creature_ptr, TRAIT_HALLUCINATION)))
					cancel_tactical_action(creature_ptr);
			}

			/* Wall (or secret door) */
			else
			{
				/* Closed doors */
				if(is_closed_door(feat) && easy_open_door(creature_ptr, y, x)) return;
				msg_format(MES_WALK_BLOCK(name));

				/*
				 * Well, it makes sense that you lose time bumping into
				 * a wall _if_ you are confused, stunned or blind; but
				 * typing mistakes should not cost you a turn...
				 */
				if(!(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_STUN) || has_trait(creature_ptr, TRAIT_HALLUCINATION)))
					cancel_tactical_action(creature_ptr);
			}
		}

		if(!boundary_floor(c_ptr, f_ptr, mimic_f_ptr)) sound(SOUND_HITWALL);
	}


	if(floor_ptr->global_map && have_flag(f_ptr->flags, FF_CHAOS_TAINTED))
	{
		if(!get_check(MES_WALK_ASK_ENTERING_CHAOS))
		{
			cancel_tactical_action(creature_ptr);
			oktomove = FALSE;
		}
	}


	/* Normal movement */
	if(oktomove && !pattern_seq(creature_ptr, creature_ptr->fy, creature_ptr->fx, y, x))
	{
		if(!(has_trait(creature_ptr, TRAIT_CONFUSED) || has_trait(creature_ptr, TRAIT_STUN) || has_trait(creature_ptr, TRAIT_HALLUCINATION)))
			cancel_tactical_action(creature_ptr);

		/* To avoid a loop with running */
		disturb(player_ptr, 0, 0);

		oktomove = FALSE;
	}

	/* Normal movement */
	if(oktomove)
	{
		u32b mpe_mode = MCE_ENERGY_USE;

		if(has_trait(creature_ptr, TRAIT_WARNING))
		{
			if(!process_warning(creature_ptr, x, y))
			{
				cost_tactical_energy(creature_ptr, 25);
				return;
			}
		}
		if(do_past) msg_format(MES_WALK_PUSH_PAST(m_name));

		/* Change oldpx and oldpy to place the player well when going back to big mode */
		if(floor_ptr->global_map)
		{
			if(ddy[dir] > 0)  creature_ptr->oldpy = 1;
			if(ddy[dir] < 0)  creature_ptr->oldpy = MAX_HGT - 2;
			if(ddy[dir] == 0) creature_ptr->oldpy = MAX_HGT / 2;
			if(ddx[dir] > 0)  creature_ptr->oldpx = 1;
			if(ddx[dir] < 0)  creature_ptr->oldpx = MAX_WID - 2;
			if(ddx[dir] == 0) creature_ptr->oldpx = MAX_WID / 2;
		}

		if(can_kill_walls)
		{
			cave_alter_feat(floor_ptr, y, x, FF_HURT_DISI);

			// Update some things -- similar to DO_EFFECT_KILL_WALL
			prepare_update(creature_ptr, PU_FLOW);
		}

		// Sound
		if(do_pickup != always_pickup) mpe_mode |= MCE_DO_PICKUP;
		if(break_trap) mpe_mode |= MCE_BREAK_TRAP;

		// Move the player
		(void)move_creature(creature_ptr, NULL, y, x, mpe_mode);
	}

	plus_move_cost(creature_ptr, x, y);

	if(!has_trait(creature_ptr, TRAIT_BLIND) && ((c_ptr->info & CAVE_GLOW) || creature_ptr->cur_lite > 0) && strlen(c_ptr->message))
	{
		msg_format(MES_INSCRIPTION_VIEW(feature_name + feature_info[c_ptr->feat].name));
		msg_format("%s", c_ptr->message);
	}
}

