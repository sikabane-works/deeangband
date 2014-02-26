/*!
 * @file Floor.h
 * @brief ゲーム中のダンジョンと付随要素のプロトタイプ
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Deeangband.h"
#include "GameElement.h"
#include "Square.h"

/*!
 * @class Floor
 * @brief フロアのクラス
 */
class Floor : public GameElement
{
private:
	vector<vector<Square>> *squares;
	MapLength width; /*!< フロアの横サイズ*/
	MapLength height; /*!< フロアの縦サイズ*/

public:

	Floor::Floor(ID id, Tag tag, GameWorld *gameWorld);
	Floor::Floor();
	Floor::~Floor();

	/*!
	 * @brief フロアの横サイズを得る
	 * @return フロアの横サイズ
	 */
	MapLength Floor::GetWidth(void);

	/*!
	 * @brief フロアの縦サイズを得る
	 * @return フロアの縦サイズ
	 */
	MapLength Floor::GetHeight(void);

	/*!
	 * @brief フロアの該当座標のマスを返す。
	 * @return 該当するマスインスタンスの参照ポインタ
	 */
	Square *Floor::getSquare(MapLength x, MapLength y);
};
