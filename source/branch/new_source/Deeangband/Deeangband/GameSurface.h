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
	GameSurface(void);
	virtual ~GameSurface(void);

	/*!
	 * @brief ゲームコマンドを受け取る
	 * @return GameCommandで定義されたコマンド内容
	 */
	virtual GameCommand GetCommand(void)
	{
		return GAME_COMMAND_REDRAW;
	}

	/*!
	 * @brief ゲーム画面表示を更新する
	 */
	virtual void Redraw(void)
	{
		return;
	}

	/*!
	 * @brief ゲーム画面にクリーチャー情報を表示する
	 * @param creaturePtr 表示したいクリーチャーの構造体参照ポインタ
	 */
	virtual void ViewCreatureStatus(Creature *creaturePtr)
	{
		return;
	}

};

