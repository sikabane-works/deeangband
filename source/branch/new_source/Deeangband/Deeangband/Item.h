/*!
    @file Item.h
    @brief �Q�[�����̃A�C�e���ƕt���v�f�̃v���g�^�C�v
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Effect.h"
#include "GameElement.h"

/*! @class Item
    @brief �A�C�e���̃N���X
*/
class Item : public GameElement
{
private:
public:
	
	/*! �󂯂����ʂ��������� */
	bool Item::take_effect(POWER amount, Effect *effectPtr);

};
