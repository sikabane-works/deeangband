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
	map<ID, Authority> authorityList;          /*!<全権能リスト*/
	map<ID, CreatureTrait> creatureTraitList;  /*!<全クリーチャー特性リスト*/
	map<ID, Effect> effectList;                /*!<全効果属性リスト*/
	map<ID, FixedArtifact> fixedArtifactList;  /*!<全固定アーティファクトリスト*/
	map<ID, Feature> featureList;              /*!<全地形リスト*/
	map<ID, ItemBase> itemBaseList;            /*!<全アイテムベースリスト*/
	map<ID, ItemEgo> itemEgoList;              /*!<全アイテムエゴリスト*/
	map<ID, ItemTrait> itemTraitList;          /*!<全アイテム特性リスト*/
	map<ID, Karma> karmatList;                 /*!<全カルマリスト*/
	map<ID, Skill> skillList;                  /*!<全スキルリスト*/
	map<ID, Species> speciesList;              /*!<全クリーチャー種別リスト*/
	map<ID, TrapBase> trapbaseList;            /*!<全トラップベース種別リスト*/

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

	PlayExitCode PlayLoop(void);

	/*
	 * @brief 受け取ったゲームコマンドを処理する
	 */
	PlayExitCode DoGameCommand(GameCommand command);
};
