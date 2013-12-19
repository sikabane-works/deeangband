/*!
    @file Floor.h
    @brief �Q�[�����̃_���W�����ƕt���v�f�̃v���g�^�C�v
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#include "stdafx.h"
#include "GameElement.h"

typedef int MapLength; /*!<�}�b�v����*/

/*! @class Floor
    @brief �t���A�̃N���X
*/
class Floor : public GameElement
{
private:

	MapLength width; /*!< �t���A�̉��T�C�Y*/
	MapLength height; /*!< �t���A�̏c�T�C�Y*/

public:

	/*! �t���A�̉��T�C�Y�𓾂� */
	MapLength get_width(void);

	/*! �t���A�̏c�T�C�Y�𓾂� */
	MapLength get_height(void);

};
