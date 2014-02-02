/*!
 *  @file GameElement.h
 *  @brief GameElement�N���X�ƕt���v�f�̃v���g�^�C�v
 *  @date 2014/02/01
 *  @author Deskull
 *  2013-2014 Sikabane Works.
 */

#pragma once

typedef int ID;     //!< �Q�[���v�fID
typedef string Tag; //!< �Q�[���v�f������ID
typedef int Weight; //!< �d��
typedef int AC;     //!< �挒��
typedef int EV;     //!< ����
typedef int VO;     //!< �ӎu��
typedef int DEPTH;  //!< �K�w�[�x


/*! 
 * @class GameElement
 * @brief �Q�[���̏��v�f�̊Ǘ����s���X�[�p�[�N���X
 */
class GameElement
{
private:
	ID id; /*!<ID*/
	Tag tag; /*!<�^�O*/
public:
	void setID(ID newID); /*!<ID��ݒ肷��*/
	ID getID();   /*!<ID���擾����*/
	void setTag(Tag newTag); /*!<�^�O��ݒ肷��*/
	Tag getTag();   /*!<�^�O���擾����*/
};
