/*!
 * @file GameSurface.h
 * @brief GameSurface�N���X�Ƃ��̑��t���v�f�̃v���g�^�C�v
 * @date 2013/12/17
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Creature.h"
#include "Deeangband.h"
#include "Floor.h"

/*! 
 * @class GameSurface
 * @brief �Q�[���̃��C���C���^�[�t�F�C�X�N���X
 */
class GameSurface
{
private:

public:
	GameSurface(void);
	virtual ~GameSurface(void);

	/*!
	 * @brief �Q�[���R�}���h���󂯎��
	 * @return GameCommand�Œ�`���ꂽ�R�}���h���e
	 */
	virtual GameCommand GetCommand(void)
	{
		return GAME_COMMAND_REDRAW;
	}

	/*!
	 * @brief �Q�[����ʕ\�����X�V����
	 * @return �Ȃ�
	 */
	virtual void Redraw(void)
	{
		return;
	}

	/*!
	 * @brief �Q�[����ʂɃN���[�`���[����\������
	 * @param creaturePtr �\���������N���[�`���[�̍\���̎Q�ƃ|�C���^
	 * @return �Ȃ�
	 */
	virtual void ViewCreatureStatus(Creature *creaturePtr)
	{
		return;
	}

	/*!
	 * @brief �t���A�̏󋵂�`�悷��
	 * @param floorPtr �\���������t���A�̍\���̎Q�ƃ|�C���^
	 * @param x ����N�_��X���W
	 * @param y ����N�_��Y���W
	 * @param w �\���̃}�X��
	 * @param h �\���̃}�X����
	 * @return �Ȃ�
	 */
	virtual void ViewFloor(Floor *floorPtr, int x, int y, int w, int h)
	{
		return;
	}

};

