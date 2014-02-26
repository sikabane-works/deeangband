/*!
 * @file FixedArtifact.h
 * @brief ゲーム中の固定アーティファクトのプロトタイプ
 * @date 2013/12/17
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "GameElement.h"

/*!
 * @class FixedArtifact
 * @brief 固定アーティファクトのクラス
 */
class FixedArtifact : public GameElement
{

private:

public:

	FixedArtifact::FixedArtifact(ID id, Tag tag, GameWorld *gameWorld);
	FixedArtifact::FixedArtifact(void);
	FixedArtifact::~FixedArtifact(void);

};
