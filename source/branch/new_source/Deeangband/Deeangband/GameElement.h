/*!
    @file GameElement.h
    @brief GameElement�N���X�ƕt���v�f�̃v���g�^�C�v
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

typedef int ID; //!<�Q�[���v�fID
typedef string StrID; //! �Q�[���v�f������ID
typedef int Weight; //!< �d��
typedef int AC; //!< �挒��
typedef int EV; //!< ����
typedef int VO; //!< �ӎu��


/*! @class GameElement
    @brief �Q�[���̏��v�f�̊Ǘ����s���X�[�p�[�N���X
*/
class GameElement
{
private:
	ID id; /*!<�Q�[���v�f��ID*/
public:
	void set_id(); /*!<ID��ݒ肷��*/
	ID get_id();   /*!<ID���擾����*/
};
