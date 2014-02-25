/*!
 * @file Square.h
 * @brief �Q�[�����̃}�X�ƕt���v�f�̃v���g�^�C�v
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Deeangband.h"
#include "GameElement.h"

/*! 
 * @class Square
 * @brief �}�X�̃N���X
 */
class Square : public GameElement
{
private:
	ID floor_id; //!<�����t���AID
	ID feature_id; //!<�n�`ID
	MapLength x; //!<�t���A���x���W
	MapLength y; //!<�t���A���y���W
public:

	/*!�t���A���x���W��Ԃ�*/
	MapLength X(void);

	/*!�t���A���y���W��Ԃ�*/
	MapLength Y(void);

	/*! ��������t���A�̃|�C���^��Ԃ��B*/
	Floor* getFloorPtr(void);

	/*! �Ή�����n�`��ID��Ԃ��B*/
	ID getFeatureID(void);

	/*! �Ή�����n�`�̃|�C���^��Ԃ��B*/
	Feature* getFeaturePtr(void);

	/*! �󂯂����ʂ��������� */
	bool Square::take_effect(Effect *effectPtr, POWER amount);

};
