/*!
 *  @file Dice.h
 *  @brief Dice�N���X�Ƃ��̑��t���v�f�̒�`
 *  @date 2014/01/19
 *  @author Deskull
 *  2014 Sikabane Works.
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

	/*!
	 * @brief �_�C�X�̐���Ԃ�
	 * @return �_�C�X�̐�
	 */
	int getNum(void);

	/*!
	 * @brief �_�C�X�̖ʐ���Ԃ�
	 * @return �_�C�X�̖ʐ�
	 */
	int getSide(void);

	/*!
	 * @brief �_�C�X��U��
	 * @return �_�C�X�̑��v�l
	 */
	int cast(void);

	/*!
	 * @brief �_�C�X��U��
	 * @param num �_�C�X�̐�
	 * @param side �_�C�X�̖ʐ�
	 * @return �_�C�X�̑��v�l
	 */
	static int cast(int num, int side);

	/*!
	 * @brief �_�C�X�̎�蓾��ő�l��Ԃ�
	 * @return �_�C�X�̍ő�l
	 */
	int maxCast(void);

	/*!
	 * @brief �_�C�X�̎�蓾��ő�l��Ԃ�
	 * @param num �_�C�X�̐�
	 * @param side �_�C�X�̖ʐ�
	 * @return �_�C�X�̍ő�l
	 */
	static int maxCast(int num, int side);

	/*!
	 * @brief 0���ŏ��l�Ƃ���������Ԃ�
     * @param max �����͈̔�(0�`max-1)
	 * @return �����̒l
	 */
	static int rand0(int max);

	/*!
	 * @brief 1���ŏ��l�Ƃ���������Ԃ�
     * @param max �����̍ő�l(1�`max)
	 * @return �����̒l
	 */
	static int rand1(int max);
};