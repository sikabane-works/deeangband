/*!
    @file Creature.h
    @brief �Q�[�����̃N���[�`���[�ƕt���v�f�̃v���g�^�C�v
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Effect.h"
#include "GameElement.h"

typedef int Status; /*!<��b�\�͒l*/
typedef int HP; /*!<HP*/
typedef int MP; /*!<MP*/

/*! @enum CREATURE_STATUS
    @brief �N���[�`���[�̊�b�\�͒l6��̗񋓑�
*/
enum CREATURE_STATUS
{
	CS_STR, /*!<�r��*/
	CS_INT, /*!<�m��*/
	CS_WIS, /*!<����*/
	CS_DEX, /*!<��p*/
	CS_CON, /*!<�ϋv*/
	CS_CHR, /*!<����*/
	CS_MAX  /*!<����:6*/
};


/*! @struct status_table
    @brief �X�e�[�^�X�l�̍\����
*/
struct status_table
{
	Status current[CS_MAX]; /*!<���݂̃X�e�[�^�X*/
	Status max[CS_MAX]; /*!<�ʏ�̃X�e�[�^�X*/
	Status max_max[CS_MAX]; /*!<�X�e�[�^�X�̊�{�������E*/
};

/*! @class Creature
    @brief �N���[�`���[�̃N���X
*/
class Creature : public GameElement
{
private:

	string name; /*!< �N���[�`���[�̖��O*/
	HP current_hp; /*!< ���݂�HP*/
	HP max_hp; /*!< ���݂̍ő�HP*/
	HP max_max_hp; /*!< �{���̍ő�HP*/
	MP current_mp; /*!< ���݂�HP*/
	MP max_mp; /*!< ���݂̍ő�HP*/
	MP max_max_mp; /*!< �{���̍ő�HP*/
	status_table status; /*!< �N���[�`���[�̃X�e�[�^�X */

	/*! �N���[�`���[���S���̏������s�� */
	void dead(void);

	/*! �N���[�`���[�̌�HP�𑝌������� */
	void calc_hp(HP amount);

	/*! �N���[�`���[�̌�MP�𑝌������� */
	void calc_mp(MP amount);

	/*! �N���[�`���[�̌�HP��ݒ肷�� */
	void set_hp(HP amount);

	/*! �N���[�`���[�̌�MP��ݒ肷�� */
	void set_mp(MP amount);

public:

	/*! �N���[�`���[���������Ă����Ԃ���Ԃ� */
	bool is_alive(void);

	/*! �󂯂����ʂ��������� */
	bool take_effect(POWER amount, Effect* effectPtr);

};

