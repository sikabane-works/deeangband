/*!
    @file GameWorld.h
    @brief �Q�[�����E�̃v���g�^�C�v�錾
    @date 2013/12/22
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Creature.h"
#include "Floor.h"

/*! @class GameWorld
    @brief �Q�[�����E�̃N���X
*/
class GameWorld
{
private:
	map<ID, StrID, Creature> CreatureList;
	map<ID, StrID, Floor> FloorList;

public:
};
