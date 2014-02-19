/*!
 * @file GameSurface.h
 * @brief GameSurfaceクラスとその他付随要素のプロトタイプ
 * @date 2013/12/17
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Floor.h"

/*! 
 * @class GameSurface
 * @brief ゲームのメインインターフェイスクラス
 */
class GameSurface
{
private:

	/*! フロアの状況を描画する */
	bool viewFloor(Floor *floor_ptr, int y, int x, int width, int height);

public:

};
