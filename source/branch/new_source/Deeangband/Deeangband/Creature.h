/*!
    @file Creature.h
    @brief �Q�[�����̃N���[�`���[���`����
    @date 2013/12/10
    @author Deskull
    2013 Sikabane Works.
*/

#include "stdafx.h"
#include "GameElement.h"

typedef int Status;

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
	Status current[CS_MAX];
	Status max[CS_MAX];
	Status max_max[CS_MAX];
};

/*! @class Creature
    @brief �N���[�`���[�̃N���X
*/
class Creature : public GameElement
{
private:
	string name;
	int current_hp; /*!< ���݂�HP*/
	int max_hp; /*!< ���݂̍ő�HP*/
	int max_max_hp; /*!< �{���̍ő�HP*/
	int current_mp; /*!< ���݂�HP*/
	int max_mp; /*!< ���݂̍ő�HP*/
	int max_max_mp; /*!< �{���̍ő�HP*/

public:

	/*!< �N���[�`���[���������Ă����Ԃ���Ԃ� */
	bool is_alive(void);
	
};

