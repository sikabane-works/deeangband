/*!
 * @file GameWorld.h
 * @brief �Q�[�����E�̃v���g�^�C�v�錾
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
 *  @brief �Q�[�����E�̃N���X
 */
class GameWorld
{
private:
	map<ID, Tag, Creature> creatureList;            /*!<�Q�[�����E���̑S�N���[�`���[���X�g*/
	map<ID, Tag, Item> itemList;                    /*!<�Q�[�����E���̑S�A�C�e�����X�g*/
	map<ID, Tag, ItemEgo> itemEgoList;              /*!<�Q�[�����E���̑S�A�C�e���G�S���X�g*/
	map<ID, Tag, FixedArtifact> fixedArtifactList;  /*!<�Q�[�����E���̑S�Œ�A�[�e�B�t�@�N�g���X�g*/
	map<ID, Tag, Feature> featureList;              /*!<�Q�[�����E���̑S�n�`���X�g*/
	map<ID, Tag, Floor> floorList;                  /*!<�Q�[�����E���̑S�t���A���X�g*/
public:

	GameWorld()
	{
	}

	~GameWorld()
	{
	}

};
