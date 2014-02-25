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
	Floor *floor_ptr; //!<�����t���A�Q�ƃ|�C���^
	ID feature_id; //!<�n�`ID
public:

	Square::Square(Floor *floor_ptr);

	Square::~Square();

	/*!
	 * @brief ��������t���A�̃|�C���^��Ԃ��B
	 */
	Floor* getFloorPtr(void);

	/*!
	 * @brief �Ή�����n�`��ID��Ԃ��B
	 */
	ID getFeatureID(void);

	/*!
	 * @brief �Ή�����n�`�̃|�C���^��Ԃ��B
	 */
	Feature* getFeaturePtr(void);

	/*!
	 * @brief �󂯂����ʂ���������
	 */
	bool Square::take_effect(Effect *effectPtr, POWER amount);

};
