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
	map<Tag, Authority> authorityList;          /*!<�S���\���X�g*/
	map<Tag, CreatureTrait> creatureTraitList;  /*!<�S�N���[�`���[�������X�g*/
	map<Tag, Effect> effectList;                /*!<�S���ʑ������X�g*/
	map<Tag, FixedArtifact> fixedArtifactList;  /*!<�S�Œ�A�[�e�B�t�@�N�g���X�g*/
	map<Tag, Feature> featureList;              /*!<�S�n�`���X�g*/
	map<Tag, ItemBase> itemBaseList;            /*!<�S�A�C�e���x�[�X���X�g*/
	map<Tag, ItemEgo> itemEgoList;              /*!<�S�A�C�e���G�S���X�g*/
	map<Tag, ItemTrait> itemTraitList;          /*!<�S�A�C�e���������X�g*/
	map<Tag, Karma> karmatList;                 /*!<�S�J���}���X�g*/
	map<Tag, Skill> skillList;                  /*!<�S�X�L�����X�g*/
	map<Tag, Species> speciesList;              /*!<�S�N���[�`���[��ʃ��X�g*/
	map<Tag, TrapBase> trapbaseList;            /*!<�S�g���b�v�x�[�X��ʃ��X�g*/

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

	Authority *GameWorld::GetAuthority(Tag tag);
	CreatureTrait *GameWorld::GetCreatureTrait(Tag tag);
	Effect *GameWorld::GetEffect(Tag tag);
	FixedArtifact *GameWorld::GetFixedArtifact(Tag tag);
	Feature *GameWorld::GetFeature(Tag tag);
	ItemBase *GameWorld::GetItemBase(Tag tag);
	ItemEgo *GameWorld::GetItemEgo(Tag tag);
	ItemTrait *GameWorld::GetItemTrait(Tag tag);
	Karma *GameWorld::GetKarma(Tag tag);
	Skill *GameWorld::GetSkill(Tag tag);
	Species *GameWorld::GetSpecies(Tag tag);
	TrapBase *GameWorld::GetTrapBase(Tag tag);

	Building *GameWorld::GetTrapBase(ID id);
	Camp *GameWorld::GetCamp(ID id);
	Creature *GameWorld::GetCreature(ID id);
	Dungeon *GameWorld::GetDungeon(ID id);
	Item *GameWorld::GetItem(ID id);
	Floor *GameWorld::GetFloor(ID id);

	PlayExitCode PlayLoop(void);

	/*
	 * @brief �󂯎�����Q�[���R�}���h����������
	 */
	PlayExitCode DoGameCommand(GameCommand command);
};
