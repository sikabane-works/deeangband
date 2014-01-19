/*!
    @file Dice.h
    @brief Dice�N���X�Ƃ��̑��t���v�f�̒�`
    @date 2014/01/19
    @author Deskull
    2014 Sikabane Works.
*/

#pragma once

/*! @class Dice
 *  @brief �Q�[�����̃_�C�X����
 */
class Dice
{
private:
	int num; //!< �_�C�X�̐�
	int side; //!< �_�C�X�̖ʐ�
public:

	/*! �_�C�X�̐���Ԃ� */
	int getNum(void);

	/*! �_�C�X�̖ʐ���Ԃ� */
	int getSide(void);

	/*! �_�C�X�̎�蓾��ő�l��Ԃ� */
	int maxThrow(void);
};
