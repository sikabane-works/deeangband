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
#include "Effect.h"
#include "Feature.h"
#include "FixedArtifact.h"
#include "Floor.h"
#include "Item.h"
#include "ItemBase.h"
#include "ItemEgo.h"
#include "ItemTrait.h"
#include "Karma.h"
#include "Skill.h"
#include "Species.h"
#include "TrapBase.h"

/*! @class GameWorld
 *  @brief ゲーム世界のクラス
 */
class GameWorld
{
private:

	//静的データリスト
	map<ID, Tag, Authority> authorityList;          /*!<全権能リスト*/
	map<ID, Tag, CreatureTrait> creatureTraitList;  /*!<全クリーチャー特性リスト*/
	map<ID, Tag, Effect> effectList;                /*!<全効果属性リスト*/
	map<ID, Tag, FixedArtifact> fixedArtifactList;  /*!<全固定アーティファクトリスト*/
	map<ID, Tag, Feature> featureList;              /*!<全地形リスト*/
	map<ID, Tag, ItemBase> itemBaseList;            /*!<全アイテムベースリスト*/
	map<ID, Tag, ItemEgo> itemEgoList;              /*!<全アイテムエゴリスト*/
	map<ID, Tag, ItemTrait> itemTraitList;          /*!<全アイテム特性リスト*/
	map<ID, Tag, Karma> karmatList;                 /*!<全カルマリスト*/
	map<ID, Tag, Skill> skillList;                  /*!<全スキルリスト*/
	map<ID, Tag, Species> speciesList;              /*!<全クリーチャー種別リスト*/
	map<ID, Tag, TrapBase> trapbaseList;            /*!<全トラップベース種別リスト*/

	//動的データリスト
	map<ID, Tag, Building> buildingList;            /*!<全施設リスト*/
	map<ID, Tag, Camp> campList;                    /*!<全陣営リスト*/
	map<ID, Tag, Creature> creatureList;            /*!<全クリーチャーリスト*/
	map<ID, Tag, Dungeon> dungeonList;              /*!<全ダンジョンリスト*/
	map<ID, Tag, Item> itemList;                    /*!<全アイテムリスト*/
	map<ID, Tag, Floor> floorList;                  /*!<全フロアリスト*/

public:

	GameWorld()
	{
	}

	~GameWorld()
	{
	}

};
