/*!
 * @file GameWorld.h
 * @brief ゲーム世界のプロトタイプ宣言
 * @date 2013/02/01
 * @author Deskull
 * 2013-2014 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Creature.h"
#include "CreatureTrait.h"
#include "FixedArtifact.h"
#include "Feature.h"
#include "Floor.h"
#include "Item.h"
#include "ItemBase.h"
#include "ItemEgo.h"
#include "ItemTrait.h"

/*! @class GameWorld
 *  @brief ゲーム世界のクラス
 */
class GameWorld
{
private:

	//基本データリスト
	map<ID, Tag, ItemBase> itemBaseList;            /*!<全アイテムベースリスト*/
	map<ID, Tag, ItemEgo> itemEgoList;              /*!<全アイテムエゴリスト*/
	map<ID, Tag, FixedArtifact> fixedArtifactList;  /*!<全固定アーティファクトリスト*/
	map<ID, Tag, ItemTrait> itemTraitList;          /*!<全アイテム特性リスト*/

	map<ID, Tag, Feature> featureList;              /*!<全地形リスト*/

	map<ID, Tag, CreatureTrait> creatureTraitList;  /*!<全クリーチャー特性リスト*/

	//応用データリスト
	map<ID, Tag, Creature> creatureList;            /*!<全クリーチャーリスト*/
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
