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
#include "Floor.h"

/*! @class GameWorld
    @brief ゲーム世界のクラス
*/
class GameWorld
{
private:
	map<ID, StrID, Creature> CreatureList;
	map<ID, StrID, Floor> FloorList;

public:
};
