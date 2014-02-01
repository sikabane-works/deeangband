/*!
    @file GameWorld.h
    @brief ゲーム世界のプロトタイプ宣言
    @date 2013/12/22
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Creature.h"
#include "Feature.h"
#include "Floor.h"

/*! @class GameWorld
    @brief ゲーム世界のクラス
*/
class GameWorld
{
private:
	map<ID, Tag, Creature> creatureList; /*!<ゲーム世界中の全クリーチャーリスト*/
	map<ID, Tag, Creature> itemList; /*!<ゲーム世界中の全アイテムリスト*/
	map<ID, Tag, Floor> floorList; /*!<ゲーム世界中の全フロアリスト*/
	map<ID, Tag, Feature> featureList; /*!<ゲーム世界中の全地形リスト*/
public:

	GameWorld()
	{
	}

	~GameWorld()
	{
	}

};
