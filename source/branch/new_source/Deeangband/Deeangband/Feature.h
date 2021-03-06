/*!
 *  @file Feature.h
 *  @brief ゲーム中の地形情報のプロトタイプ
 *  @date 2014/02/20
 *  @author Deskull
 *  2014 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "GameElement.h"

/*!
 * @class Feature
 * @brief 地形情報のクラス
 */
class Feature : public GameElement
{
private:
	string name; /*!< 地形の名前 */
	ID tileID; /*!< 地形タイルID */
	char symbol; /*!< 地形タイルシンボル */
	Color symColor; /*!< 地形タイルシンボル */

public:

	Feature::Feature(ID id, Tag tag, GameWorld *gameWorld);
	Feature::Feature(void);
	Feature::~Feature(void);

};
