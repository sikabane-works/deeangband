/*!
    @file Creature.h
    @brief �Q�[�����̃N���[�`���[�ƕt���v�f�̃v���g�^�C�v
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Coordinates.h"
#include "Effect.h"
#include "Floor.h"
#include "Item.h"
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

/*!
 * @struct status_table
 * @brief �X�e�[�^�X�l�̍\����
 */
struct status_table
{
	Status current[CS_MAX]; /*!<���݂̃X�e�[�^�X*/
	Status max[CS_MAX]; /*!<�ʏ�̃X�e�[�^�X*/
	Status max_max[CS_MAX]; /*!<�X�e�[�^�X�̊�{�������E*/
};

/*!
 * @class Creature
 * @brief �N���[�`���[�̃N���X
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
	status_table status; /*!< �X�e�[�^�X */
	ID floorID; /*!< ���݂���t���AID */
	Coordinates position; /*!< �����W */
	map<ID, Tag, Item> itemList; /*!< �����A�C�e�����X�g */
	char symbol;
	Color symbolColor;

	/*!
	 * @brief �N���[�`���[���S���̏������s��
	 */
	void dead(void);

	/*!
	 * @brief �N���[�`���[�̌�HP�𑝌�������
	 */
	void calc_hp(HP amount);

	/*!
	 * @brief �N���[�`���[�̌�MP�𑝌�������
	 */
	void calc_mp(MP amount);

	/*!
	 * @brief �N���[�`���[�̌�HP��ݒ肷��
	 */
	void set_hp(HP amount);

	/*!
	 * @brief �N���[�`���[�̌�MP��ݒ肷��
	 */
	void set_mp(MP amount);

	/*!
	 * @brief �N���[�`���[������t���A�̏�����W�Ɉړ�����
	 * @param floorID �ړ���t���AID
	 * @param x �ړ���x���W
	 * @param y �ړ���y���W
	 * @return �ړ��ł��Ȃ������ꍇ�̃G���[ID
	 */
	int move(ID floor_ID, int x, int y);

	/*!
	 * @brief �N���[�`���[������t���A�̏�����W�Ɉړ�����
	 * @param floorID �ړ���t���AID
	 * @param coord �ړ�����W
	 * @return �ړ��ł��Ȃ������ꍇ�̃G���[ID
	 */
	int move(ID floor_ID, Coordinates coord);

public:

	/*!
	 * @brief �N���[�`���[���������Ă����Ԃ���Ԃ�
	 * @return �������Ă���Ȃ��true��Ԃ�
	 */
	bool is_alive(void);

	/*!
	 * @brief �󂯂��������ʂ���������
	 * @param effectPtr
	 * @param amount
	 */
	bool take_effect(Effect* effectPtr, POWER amount);

	/*!
	 * @brief �N���[�`���[�̌�HP��Ԃ�
	 * @return �N���[�`���[�̌�HP
	 */
	HP getCurHP(void);

	/*!
	 * @brief �N���[�`���[�̌��ő�HP��Ԃ�
	 * @return �N���[�`���[�̌��ő�HP
	 */
	HP getMaxHP(void);

	/*!
	 * @brief �N���[�`���[�̒ʏ�ő�HP��Ԃ�
	 * @return �N���[�`���[�̒ʏ�ő�HP
	 */
	HP getNorMaxHP(void);

	/*!
	 * @brief �N���[�`���[�̌�MP��Ԃ�
	 * @return �N���[�`���[�̌�MP
	 */
	MP getCurMP(void);

	/*!
	 * @brief �N���[�`���[�̌��ő�MP��Ԃ�
	 * @return �N���[�`���[�̌��ő�MP
	 */
	MP getMaxMP(void);

	/*!
	 * @brief �N���[�`���[�̒ʏ�ő�MP��Ԃ�
	 * @return �N���[�`���[�̒ʏ�ő�MP
	 */
	MP getNorMaxMP(void);

	/*!
	 * @brief �N���[�`���[�̌�AC��Ԃ�
	 * @return �N���[�`���[�̌�AC
	 */
	AC getAC(void);

	/*!
	 * @brief �N���[�`���[�̌�EV��Ԃ�
	 * @return �N���[�`���[�̌�EV
	 */
	EV getEV(void);

	/*!
	 * @brief �N���[�`���[�̌�VO��Ԃ�
	 * @return �N���[�`���[�̌�VO
	 */
	VO getVO(void);

	/*!
	 * @brief �N���[�`���[�̌��݈ʒu��Ԃ�
	 * @return �N���[�`���[���݈ʒu������Coordinates
	 */
	Coordinates *getCorrdinates(void);

	/*!
	 * @brief �N���[�`���[�̏����i���X�g��Ԃ�
	 * @return �N���[�`���[�����i���X�g�}�b�v
	 */
	map<ID, Tag, Item> *getItemList(void);

};

