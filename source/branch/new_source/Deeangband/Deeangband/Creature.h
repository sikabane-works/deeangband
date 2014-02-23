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

typedef int Status; /*!< ��b�\�͒l�̕ϐ��^ */
typedef int HP;     /*!< HP�̕ϐ��^ */
typedef int MP;     /*!< MP�̕ϐ��^ */
typedef int SOUL;   /*!< �\�E���̕ϐ��^ */

/*!
 * @brief �N���[�`���[�̊�b�\�͒l6��̗񋓑�
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
 * @struct StatusTable
 * @brief �X�e�[�^�X�l�̍\����
 */
struct StatusTable
{
	Status current[CS_MAX]; /*!<���݂̃X�e�[�^�X*/
	Status max[CS_MAX]; /*!<�ʏ�̃X�e�[�^�X*/
	Status maxMax[CS_MAX]; /*!<�X�e�[�^�X�̊�{�������E*/
};

/*!
 * @class Creature
 * @brief �N���[�`���[�̃N���X
 */
class Creature : public GameElement
{
private:

	string name;      /*!< �N���[�`���[�̖��O */
	HP currentHp;     /*!< ���݂�HP */
	HP max_hp;        /*!< ���݂̍ő�HP */
	HP maxMaxHp;      /*!< �{���̍ő�HP */
	MP currentMp;     /*!< ���݂�MP */
	MP maxMp;         /*!< ���݂̍ő�MP */
	MP maxMaxMp;      /*!< �{���̍ő�MP */
	SOUL currentSoul; /*!< ���݂̃\�E���l */
	SOUL maxSoul;     /*!< �\�E���ő哞�B�l */

	StatusTable status; /*!< �X�e�[�^�X */
	ID floorID; /*!< ���݂���t���AID */
	Coordinates position; /*!< �����W */
	map<ID, Tag, Item> itemList; /*!< �����A�C�e�����X�g */
	char symbol; /*!< �V���{���L���� */
	Color symbolColor; /*!< �V���{���J���[ */
	ID tileID ;/*!< �^�C��ID */

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
	 * @brief �N���[�`���[�̃_�~�[���������R���X�g���N�^
	 */
	Creature::Creature(void);

	/*!
	 * @brief �N���[�`���[�̖��O��Ԃ�
	 * @return �N���[�`���[��
	 */
	string Creature::GetName(void);

	/*!
	 * @brief �N���[�`���[���������Ă����Ԃ���Ԃ�
	 * @return �������Ă���Ȃ��true��Ԃ�
	 */
	bool Creature::is_alive(void);

	/*!
	 * @brief �󂯂��������ʂ���������
	 * @param effectPtr
	 * @param amount
	 */
	bool Creature::take_effect(Effect* effectPtr, POWER amount);

	/*!
	 * @brief �N���[�`���[�̌�HP��Ԃ�
	 * @return �N���[�`���[�̌�HP
	 */
	HP Creature::GetCurHP(void);

	/*!
	 * @brief �N���[�`���[�̌��ő�HP��Ԃ�
	 * @return �N���[�`���[�̌��ő�HP
	 */
	HP Creature::GetMaxHP(void);

	/*!
	 * @brief �N���[�`���[�̒ʏ�ő�HP��Ԃ�
	 * @return �N���[�`���[�̒ʏ�ő�HP
	 */
	HP Creature::GetNorMaxHP(void);

	/*!
	 * @brief �N���[�`���[�̌�MP��Ԃ�
	 * @return �N���[�`���[�̌�MP
	 */
	MP Creature::GetCurMP(void);

	/*!
	 * @brief �N���[�`���[�̌��ő�MP��Ԃ�
	 * @return �N���[�`���[�̌��ő�MP
	 */
	MP Creature::GetMaxMP(void);

	/*!
	 * @brief �N���[�`���[�̒ʏ�ő�MP��Ԃ�
	 * @return �N���[�`���[�̒ʏ�ő�MP
	 */
	MP Creature::GetNorMaxMP(void);

	/*!
	 * @brief �N���[�`���[�̌�AC��Ԃ�
	 * @return �N���[�`���[�̌�AC
	 */
	AC Creature::GetAC(void);

	/*!
	 * @brief �N���[�`���[�̌�EV��Ԃ�
	 * @return �N���[�`���[�̌�EV
	 */
	EV Creature::GetEV(void);

	/*!
	 * @brief �N���[�`���[�̌�VO��Ԃ�
	 * @return �N���[�`���[�̌�VO
	 */
	VO Creature::GetVO(void);

	/*!
	 * @brief �N���[�`���[�̌��݈ʒu��Ԃ�
	 * @return �N���[�`���[���݈ʒu������Coordinates
	 */
	Coordinates *Creature::getCorrdinates(void);

	/*!
	 * @brief �N���[�`���[�̏����i���X�g��Ԃ�
	 * @return �N���[�`���[�����i���X�g�}�b�v
	 */
	map<ID, Tag, Item> *Creature::getItemList(void);

	/*!
	 * @brief �N���[�`���[�̌��\�E���l��Ԃ�
	 * @return �N���[�`���[�̌��\�E���l
	 */
	SOUL Creature::GetCurrentSoul(void);

	/*!
	 * @brief �N���[�`���[�̃\�E���ő哞�B�l��Ԃ�
	 * @return �N���[�`���[�̃\�E���ő哞�B�l
	 */
	SOUL Creature::GetMaxSoul(void);

	/*!
	 * @brief �N���[�`���[�̌���b�\�͒l��Ԃ�
	 * @param stat �擾��������b�\�͒lID
	 * @return ��b�\�͒l�̌��ݒl
	 */
	Status Creature::GetCurrentStatus(CREATURE_STATUS stat);

	/*!
	 * @brief �N���[�`���[�̕����b�\�͒l��Ԃ�
	 * @param stat �擾����������\�͒lID
	 * @return ��b�\�͒l�̕���l
	 */
	Status Creature::GetMaxStatus(CREATURE_STATUS stat);

	/*!
	 * @brief �N���[�`���[�̌��E��b�\�͒l��Ԃ�
	 * @param stat �擾���������E�\�͒lID
	 * @return ��b�\�͒l�̌��E�l
	 */
	Status Creature::GetMaxMAXStatus(CREATURE_STATUS stat);

};

