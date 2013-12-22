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
#include "Feature.h"
#include "Floor.h"

/*! @class GameWorld
    @brief �Q�[�����E�̃N���X
*/
class GameWorld
{
private:
	map<ID, StrID, Creature> creatureList; /*!<�Q�[�����E���̑S�N���[�`���[���X�g*/
	map<ID, StrID, Creature> itemList; /*!<�Q�[�����E���̑S�A�C�e�����X�g*/
	map<ID, StrID, Floor> floorList; /*!<�Q�[�����E���̑S�t���A���X�g*/
	map<ID, StrID, Feature> featureList; /*!<�Q�[�����E���̑S�n�`���X�g*/
public:

	GameWorld()
	{
	}

	~GameWorld()
	{
	}

};