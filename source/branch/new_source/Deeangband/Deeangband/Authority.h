/*!
 * @file Authority.h
 * @brief ���\�̃v���g�^�C�v�錾
 * @date 2013/12/22
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "CreatureTemplate.h"

/*! 
 * @class Authority
 * @brief ���\�̃N���X
 */
class Authority : public CreatureTemplate
{
private:

public:
	Authority::Authority(ID id, Tag tag, GameWorld *gameWorld);
	Authority::Authority(void);
	Authority::~Authority(void);
};
