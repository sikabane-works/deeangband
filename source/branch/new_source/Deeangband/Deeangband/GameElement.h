/*!
 *  @file GameElement.h
 *  @brief GameElement�N���X�ƕt���v�f�̃v���g�^�C�v
 *  @date 2014/02/01
 *  @author Deskull
 *  2013-2014 Sikabane Works.
 */

#pragma once

#include "Deeangband.h"

typedef int ID;     //!< �Q�[���v�fID
typedef string Tag; //!< �Q�[���v�f������ID
typedef int Weight; //!< �d��
typedef int AC;     //!< �挒��
typedef int EV;     //!< ����
typedef int VO;     //!< �ӎu��
typedef int DEPTH;  //!< �K�w�[�x
typedef int NUMBER; //!< ��
typedef int PPM;    //!< �S������
typedef int MapLength; //!<�}�b�v����
typedef int POWER;     //!<����

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

	ID id;   //!< ID
	Tag tag; //!< �^�O
	GameWorld *gameWorld; //!< ��������GameWorld

public:

	GameElement::GameElement(void); 

	GameElement::GameElement(ID id, Tag tag, GameWorld *gameWorld); 

	GameElement::~GameElement(void); 


	/*!
	 * @brief ID��ݒ肷��
	 */
	void GameElement::SetID(ID newID); 

	/*!
	 * @brief ID���擾����
	 */
	ID GameElement::GetID();

	/*!
	 * @brief �^�O��ݒ肷��
	 */
	void GameElement::SetTag(Tag newTag);

	/*!
	 * @brief �^�O���擾����
	 */
	Tag GameElement::GetTag();

	/*!
	 * @brief ��������GameWorld�C���X�^���X��Ԃ�
	 * @return GameWorld�C���X�^���X�̎Q�ƃ|�C���^
	 */
	GameWorld *GameElement::GetGameWorld();
};
