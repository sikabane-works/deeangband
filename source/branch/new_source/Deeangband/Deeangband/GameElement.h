/*!
    @file GameElement.h
    @brief GameElement�N���X�Ƃ��̑��t���v�f�̃v���g�^�C�v
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

typedef int ID; /*!<�Q�[���v�fID*/

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
