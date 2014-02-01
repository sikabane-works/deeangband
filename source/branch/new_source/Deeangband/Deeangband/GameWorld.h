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
#include "FixedArtifact.h"
#include "Feature.h"
#include "Floor.h"
#include "Item.h"
#include "ItemEgo.h"

/*! @class GameWorld
 *  @brief ゲーム世界のクラス
 */
class GameWorld
{
private:
	map<ID, Tag, Creature> creatureList;            /*!<ゲーム世界中の全クリーチャーリスト*/
	map<ID, Tag, Item> itemList;                    /*!<ゲーム世界中の全アイテムリスト*/
	map<ID, Tag, ItemEgo> itemEgoList;              /*!<ゲーム世界中の全アイテムエゴリスト*/
	map<ID, Tag, FixedArtifact> fixedArtifactList;  /*!<ゲーム世界中の全固定アーティファクトリスト*/
	map<ID, Tag, Feature> featureList;              /*!<ゲーム世界中の全地形リスト*/
	map<ID, Tag, Floor> floorList;                  /*!<ゲーム世界中の全フロアリスト*/
public:

	GameWorld()
	{
	}

	~GameWorld()
	{
	}

};
