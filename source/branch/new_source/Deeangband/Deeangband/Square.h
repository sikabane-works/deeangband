/*!
    @file Square.h
    @brief �Q�[�����̃}�X�ƕt���v�f�̃v���g�^�C�v
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"

#include "Effect.h"
#include "Floor.h"
#include "Feature.h"
#include "GameElement.h"

/*! @class Square
    @brief �}�X�̃N���X
*/
class Square : public GameElement
{
private:
	ID *floor_id; //!<�����t���AID
	ID *feature_id; //!<�n�`ID

public:

	/*! ��������t���A�̃|�C���^��Ԃ��B*/
	Floor* getFloorPtr(void);

	/*! �Ή�����n�`�̃|�C���^��Ԃ��B*/
	Feature* getFeaturePtr(void);

	/*! �󂯂����ʂ��������� */
	bool Square::take_effect(POWER amount, Effect *effectPtr);

};
