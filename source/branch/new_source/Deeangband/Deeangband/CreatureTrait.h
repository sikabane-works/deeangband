/*!
 * @file CreatureTrait.h
 * @brief ゲーム中のクリーチャー特性のプロトタイプ
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
 * @brief クリーチャー特性のクラス
 */
class CreatureTrait : public CreatureTemplate
{
private:
public:

	CreatureTrait::CreatureTrait(ID id, Tag tag, GameWorld *gameWorld);

	CreatureTrait::CreatureTrait(void);

	CreatureTrait::~CreatureTrait(void);

};
