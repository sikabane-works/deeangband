/*!
    @file GameView.h
    @brief GameViewクラスとその他付随要素のプロトタイプ
    @date 2013/12/17
    @author Deskull
    2013 Sikabane Works.
*/

#include "stdafx.h"
#include "Floor.h"

/*! @class GameView
    @brief ゲームのメインインターフェイスクラス
*/
class GameView
{
private:

	/*! フロアの状況を描画する */
	bool viewFloor(Floor *floor_ptr, int y, int x, int width, int height);

public:


};
