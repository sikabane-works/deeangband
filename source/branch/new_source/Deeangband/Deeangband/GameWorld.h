/*!
 * @file GameWorld.h
 * @brief ゲーム世界のプロトタイプ宣言
 * @date 2013/02/01
 * @author Deskull
 * 2013-2014 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Authority.h"
#include "Building.h"
#include "Camp.h"
#include "Creature.h"
#include "CreatureTrait.h"
#include "Dungeon.h"
#include "Deeangband.h"
#include "Effect.h"
#include "Feature.h"
#include "FixedArtifact.h"
#include "Floor.h"
#include "GameSurface.h"
#include "Item.h"
#include "ItemBase.h"
#include "ItemEgo.h"
#include "ItemTrait.h"
#include "Karma.h"
#include "Skill.h"
#include "Species.h"
#include "TrapBase.h"

/*!
 * @class GameWorld
 * @brief ゲーム世界のクラス
 */
class GameWorld
{
private:
	GameSurface *gameSurface;

	//静的データリスト
	map<Tag, Authority> authorityList;          /*!<全権能リスト*/
	map<Tag, CreatureTrait> creatureTraitList;  /*!<全クリーチャー特性リスト*/
	map<Tag, Effect> effectList;                /*!<全効果属性リスト*/
	map<Tag, FixedArtifact> fixedArtifactList;  /*!<全固定アーティファクトリスト*/
	map<Tag, Feature> featureList;              /*!<全地形リスト*/
	map<Tag, ItemBase> itemBaseList;            /*!<全アイテムベースリスト*/
	map<Tag, ItemEgo> itemEgoList;              /*!<全アイテムエゴリスト*/
	map<Tag, ItemTrait> itemTraitList;          /*!<全アイテム特性リスト*/
	map<Tag, Karma> karmatList;                 /*!<全カルマリスト*/
	map<Tag, Skill> skillList;                  /*!<全スキルリスト*/
	map<Tag, Species> speciesList;              /*!<全クリーチャー種別リスト*/
	map<Tag, TrapBase> trapbaseList;            /*!<全トラップベース種別リスト*/

	//動的データリスト
	map<ID, Building> buildingList;            /*!<全施設リスト*/
	map<ID, Camp> campList;                    /*!<全陣営リスト*/
	map<ID, Creature> creatureList;            /*!<全クリーチャーリスト*/
	map<ID, Dungeon> dungeonList;              /*!<全ダンジョンリスト*/
	map<ID, Item> itemList;                    /*!<全アイテムリスト*/
	map<ID, Floor> floorList;                  /*!<全フロアリスト*/

public:

	GameWorld(GameSurface *gSurface);
	~GameWorld();

	Authority *GameWorld::GetAuthority(Tag tag);
	CreatureTrait *GameWorld::GetCreatureTrait(Tag tag);
	Effect *GameWorld::GetEffect(Tag tag);
	FixedArtifact *GameWorld::GetFixedArtifact(Tag tag);
	Feature *GameWorld::GetFeature(Tag tag);
	ItemBase *GameWorld::GetItemBase(Tag tag);
	ItemEgo *GameWorld::GetItemEgo(Tag tag);
	ItemTrait *GameWorld::GetItemTrait(Tag tag);
	Karma *GameWorld::GetKarma(Tag tag);
	Skill *GameWorld::GetSkill(Tag tag);
	Species *GameWorld::GetSpecies(Tag tag);
	TrapBase *GameWorld::GetTrapBase(Tag tag);

	Building *GameWorld::GetTrapBase(ID id);
	Camp *GameWorld::GetCamp(ID id);
	Creature *GameWorld::GetCreature(ID id);
	Dungeon *GameWorld::GetDungeon(ID id);
	Item *GameWorld::GetItem(ID id);
	Floor *GameWorld::GetFloor(ID id);

	PlayExitCode PlayLoop(void);

	/*
	 * @brief 受け取ったゲームコマンドを処理する
	 */
	PlayExitCode DoGameCommand(GameCommand command);
};
