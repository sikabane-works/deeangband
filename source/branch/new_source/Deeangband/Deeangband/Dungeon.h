/*!
 * @file Dungeon.h
 * @brief �Q�[�����̃_���W�����ƕt���v�f�̃v���g�^�C�v
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Camp.h"
#include "Species.h"
#include "Feature.h"
#include "GameElement.h"

#pragma once

/*!
 * @struct �Œ�t���A�����e�[�u��
 */
typedef struct FixedFloorTable FixedFloorTable;
struct FixedFloorTable
{
	bool StaticFixed; //!<�ÓI�Œ�
	ID FixedFloorID;  //!<���������Œ�t���A��ID
	DEPTH MinDepth;   //!<�����Œ�K�w
	DEPTH MaxDepth;   //!<�����ő�K�w
	NUMBER MinNum;    //!<�Œᐶ����
	NUMBER MaxNum;    //!<�ő吶����
	PPM Probability;  //!<�����m��
};



/*! 
 * @class Dungeon
 * @brief �_���W�����̃N���X
 */
class Dungeon : public GameElement
{
private:
	list<FixedFloorTable> fixFloorTableList; //!< �Œ�t���A�̃��X�g
	DEPTH minDepth;      //!< �J�n�K�w
	DEPTH maxDepth;      //!< �Ő[�K�w
	ID outerWallFeature; //!< ��{�O�ǒn�`ID
	ID innerWallFeature; //!< ��{���ǒn�`ID
	ID controlCamp;      //!< ���L�w�cID
	ID gurdianSpecies;   //!< �_���W�����̎���ID
public:

	DEPTH GetMinDepth(void);

	DEPTH GetMaxDepth(void);

	ID GetOuterWallFeatureID(void);

	Feature *GetOuterWallFeaturePtr(void);

	ID GetInnerWallFeatureID(void);

	Feature *GetInnerWallFeaturePtr(void);

	ID GetControlCampID(void);

	Camp *GetControlCampPtr(void);

	ID GetGurdianSpeciesID(void);

	Species *GetGurdianSpeciesPtr(void);

};
