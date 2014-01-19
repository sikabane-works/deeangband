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
	Weight weight; //!< �A�C�e���̏d��
	AC baseAC; //!< ��{���b�C��
	EV baseEV; //!< ��{����C��
	VO baseVO; //!< ��{�ӎu�C��
	AC plusAC; //!< �ǉ����b�C��
	EV plusEV; //!< �ǉ�����C��
	VO plusVO; //!< �ǉ��ӎu�C��
public:
	
	/*! �󂯂����ʂ��������� */
	bool Item::take_effect(Effect *effectPtr, POWER amount);

	/*! ��{���b�C����Ԃ� */
	AC getBaseAC(void);

	/*! �ǉ����b�C����Ԃ� */
	AC getPlusAC(void);

	/*! ��{����C����Ԃ� */
	EV getBaseEV(void);

	/*! �ǉ�����C����Ԃ� */
	EV getPlusEV(void);

	/*! ��{�ӎu�C����Ԃ� */
	VO getBaseVO(void);

	/*! �ǉ��ӎu�C����Ԃ� */
	VO getPlusVO(void);

};
