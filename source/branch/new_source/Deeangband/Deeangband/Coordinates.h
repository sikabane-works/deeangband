/*!
    @file Coordinates.h
    @brief Coordinates�N���X�Ƃ��̑��t���v�f�̒�`
    @date 2014/01/26
    @author Deskull
    2014 Sikabane Works.
*/

#pragma once

/*! @class Coordinates
 *  @brief �Q�[�����E���W�̃N���X
 */
class Coordinates
{
private:
	int fx; //!< �t���AX���W
	int fy; //!< �t���AY���W
	int wx; //!< �L��}�b�vX���W
	int wy; //!< �L��}�b�vY���W
public:

	/*
	 * @brief �t���AX���W�𓾂�
	 * @return �t���AX���W
	 */
	int getFloorX(void);

	/*
	 * @brief �t���AY���W�𓾂�
	 * @return �t���AY���W�𓾂�
	 */
	int getFloorY(void);

	/*
	 * @brief �L��}�b�vX���W�𓾂�
	 * @return �L��}�b�vX���W
	 */
	int getWorldX(void);

	/*
	 * @brief �L��}�b�vY���W�𓾂�
	 * @return �L��}�b�vY���W
	 */
	int getWorldY(void);
};
