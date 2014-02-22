/*!
 * @file GameSurface.h
 * @brief GameSurfaceクラスとその他付随要素のプロトタイプ
 * @date 2013/12/17
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Deeangband.h"
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
	GameSurface();
	virtual ~GameSurface();

	/*!
	 * @brief ゲームコマンドを受け取る
	 * @return GameCommandで定義されたコマンド内容
	 */
	virtual GameCommand GetCommand()
	{
		return GAME_COMMAND_REDRAW;
	}

	virtual void Redraw()
	{
		return;
	}

};

