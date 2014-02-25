/*!
 * @file GameWorld.h
 * @brief �Q�[�����E�̃v���g�^�C�v�錾
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
 * @brief �Q�[�����E�̃N���X
 */
class GameWorld
{
private:
	GameSurface *gameSurface;

	//�ÓI�f�[�^���X�g
	map<ID, Authority> authorityList;          /*!<�S���\���X�g*/
	map<ID, CreatureTrait> creatureTraitList;  /*!<�S�N���[�`���[�������X�g*/
	map<ID, Effect> effectList;                /*!<�S���ʑ������X�g*/
	map<ID, FixedArtifact> fixedArtifactList;  /*!<�S�Œ�A�[�e�B�t�@�N�g���X�g*/
	map<ID, Feature> featureList;              /*!<�S�n�`���X�g*/
	map<ID, ItemBase> itemBaseList;            /*!<�S�A�C�e���x�[�X���X�g*/
	map<ID, ItemEgo> itemEgoList;              /*!<�S�A�C�e���G�S���X�g*/
	map<ID, ItemTrait> itemTraitList;          /*!<�S�A�C�e���������X�g*/
	map<ID, Karma> karmatList;                 /*!<�S�J���}���X�g*/
	map<ID, Skill> skillList;                  /*!<�S�X�L�����X�g*/
	map<ID, Species> speciesList;              /*!<�S�N���[�`���[��ʃ��X�g*/
	map<ID, TrapBase> trapbaseList;            /*!<�S�g���b�v�x�[�X��ʃ��X�g*/

	//���I�f�[�^���X�g
	map<ID, Building> buildingList;            /*!<�S�{�݃��X�g*/
	map<ID, Camp> campList;                    /*!<�S�w�c���X�g*/
	map<ID, Creature> creatureList;            /*!<�S�N���[�`���[���X�g*/
	map<ID, Dungeon> dungeonList;              /*!<�S�_���W�������X�g*/
	map<ID, Item> itemList;                    /*!<�S�A�C�e�����X�g*/
	map<ID, Floor> floorList;                  /*!<�S�t���A���X�g*/

public:

	GameWorld(GameSurface *gSurface);
	~GameWorld();

	PlayExitCode PlayLoop(void);

	/*
	 * @brief �󂯎�����Q�[���R�}���h����������
	 */
	PlayExitCode DoGameCommand(GameCommand command);
};
