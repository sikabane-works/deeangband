/*!
 * @file Floor.h
 * @brief �Q�[�����̃_���W�����ƕt���v�f�̃v���g�^�C�v
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "GameElement.h"
#include "Square.h"

/*!
 * @class Floor
 * @brief �t���A�̃N���X
 */
class Floor : public GameElement
{
private:

	map<MapLength, MapLength, Square> squares;
	MapLength width; /*!< �t���A�̉��T�C�Y*/
	MapLength height; /*!< �t���A�̏c�T�C�Y*/

public:

	/*!
	 * @brief �t���A�̉��T�C�Y�𓾂�
	 * @return �t���A�̉��T�C�Y
	 */
	MapLength GetWidth(void);

	/*!
	 * @brief �t���A�̏c�T�C�Y�𓾂�
	 * @return �t���A�̏c�T�C�Y
	 */
	MapLength GetHeight(void);

	/*!
	 * @brief �t���A�̊Y�����W�̃}�X��Ԃ��B
	 * @return �Y������}�X�C���X�^���X�̎Q�ƃ|�C���^
	 */
	Square *getSquare(MapLength x, MapLength y);
};
