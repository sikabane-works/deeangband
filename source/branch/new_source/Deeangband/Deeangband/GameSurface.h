/*!
 * @file GameSurface.h
 * @brief GameSurface�N���X�Ƃ��̑��t���v�f�̃v���g�^�C�v
 * @date 2013/12/17
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Deeangband.h"
#include "Floor.h"

/*! 
 * @class GameSurface
 * @brief �Q�[���̃��C���C���^�[�t�F�C�X�N���X
 */
class GameSurface
{
private:
	/*! �t���A�̏󋵂�`�悷�� */
	bool viewFloor(Floor *floor_ptr, int y, int x, int width, int height);

public:
	GameSurface();
	virtual ~GameSurface();

	/*!
	 * @brief �Q�[���R�}���h���󂯎��
	 * @return GameCommand�Œ�`���ꂽ�R�}���h���e
	 */
	virtual GameCommand GetCommand()
	{
		return GAME_COMMAND_REDRAW;
	}

	virtual void Redraw()
	{
		return;
	}

};

