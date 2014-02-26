/*!
 * @file CreatureTrait.h
 * @brief �Q�[�����̃N���[�`���[�����̃v���g�^�C�v
 * @date 2013/12/22
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "CreatureTemplate.h"
#include "Deeangband.h"
#include "GameElement.h"

/*! 
 * @class CreatureTrait
 * @brief �N���[�`���[�����̃N���X
 */
class CreatureTrait : public CreatureTemplate
{
private:
public:

	CreatureTrait::CreatureTrait(ID id, Tag tag, GameWorld *gameWorld);

	CreatureTrait::CreatureTrait(void);

	CreatureTrait::~CreatureTrait(void);

};
