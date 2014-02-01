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
#include "CreatureTrait.h"
#include "FixedArtifact.h"
#include "Feature.h"
#include "Floor.h"
#include "Item.h"
#include "ItemBase.h"
#include "ItemEgo.h"
#include "ItemTrait.h"

/*! @class GameWorld
 *  @brief �Q�[�����E�̃N���X
 */
class GameWorld
{
private:

	//��{�f�[�^���X�g
	map<ID, Tag, ItemBase> itemBaseList;            /*!<�S�A�C�e���x�[�X���X�g*/
	map<ID, Tag, ItemEgo> itemEgoList;              /*!<�S�A�C�e���G�S���X�g*/
	map<ID, Tag, FixedArtifact> fixedArtifactList;  /*!<�S�Œ�A�[�e�B�t�@�N�g���X�g*/
	map<ID, Tag, ItemTrait> itemTraitList;          /*!<�S�A�C�e���������X�g*/

	map<ID, Tag, Feature> featureList;              /*!<�S�n�`���X�g*/

	map<ID, Tag, CreatureTrait> creatureTraitList;  /*!<�S�N���[�`���[�������X�g*/

	//���p�f�[�^���X�g
	map<ID, Tag, Creature> creatureList;            /*!<�S�N���[�`���[���X�g*/
	map<ID, Tag, Item> itemList;                    /*!<�S�A�C�e�����X�g*/
	map<ID, Tag, Floor> floorList;                  /*!<�S�t���A���X�g*/

public:

	GameWorld()
	{
	}

	~GameWorld()
	{
	}

};
