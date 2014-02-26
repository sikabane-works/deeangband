/*!
 * @file Floor.h
 * @brief �Q�[�����̃_���W�����ƕt���v�f�̃v���g�^�C�v
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Deeangband.h"
#include "GameElement.h"
#include "Square.h"

/*!
 * @class Floor
 * @brief �t���A�̃N���X
 */
class Floor : public GameElement
{
private:
	vector<vector<Square>> *squares;
	MapLength width; /*!< �t���A�̉��T�C�Y*/
	MapLength height; /*!< �t���A�̏c�T�C�Y*/

public:

	Floor::Floor(ID id, Tag tag, GameWorld *gameWorld);
	Floor::Floor();
	Floor::~Floor();

	/*!
	 * @brief �t���A�̉��T�C�Y�𓾂�
	 * @return �t���A�̉��T�C�Y
	 */
	MapLength Floor::GetWidth(void);

	/*!
	 * @brief �t���A�̏c�T�C�Y�𓾂�
	 * @return �t���A�̏c�T�C�Y
	 */
	MapLength Floor::GetHeight(void);

	/*!
	 * @brief �t���A�̊Y�����W�̃}�X��Ԃ��B
	 * @return �Y������}�X�C���X�^���X�̎Q�ƃ|�C���^
	 */
	Square *Floor::getSquare(MapLength x, MapLength y);
};
