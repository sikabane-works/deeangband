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
typedef int NUMBER; //!< ��
typedef int PPM;    //!< �S������

/*!
 * @struct color
 * @brief �F�w��̍\����
 */
typedef struct 
{
	int r;
	int g;
	int b;
	int a;
} Color;

/*! 
 * @class GameElement
 * @brief �Q�[���̏��v�f�̊Ǘ����s���X�[�p�[�N���X
 */
class GameElement
{
private:

	ID id;       /*!< ID */
	Tag tag;     /*!< �^�O */

public:

	/*!
	 * @brief ID��ݒ肷��
	 */
	void setID(ID newID); 

	/*!
	 * @brief ID���擾����
	 */
	ID getID();

	/*!
	 * @brief �^�O��ݒ肷��
	 */
	void setTag(Tag newTag);

	/*!
	 * @brief �^�O���擾����
	 */
	Tag getTag();

};
