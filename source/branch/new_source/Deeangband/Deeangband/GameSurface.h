/*!
 * @file GameSurface.h
 * @brief GameSurfaceクラスとその他付随要素のプロトタイプ
 * @date 2013/12/17
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Creature.h"
#include "Deeangband.h"
#include "Floor.h"

/*! 
 * @class GameSurface
 * @brief ゲームのメインインターフェイスクラス
 */
class GameSurface
{
private:

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
	 * @return なし
	 */
	virtual void Redraw(void)
	{
		return;
	}

	/*!
	 * @brief ゲーム画面にクリーチャー情報を表示する
	 * @param creaturePtr 表示したいクリーチャーの構造体参照ポインタ
	 * @return なし
	 */
	virtual void ViewCreatureStatus(Creature *creaturePtr)
	{
		return;
	}

	/*!
	 * @brief フロアの状況を描画する
	 * @param floorPtr 表示したいフロアの構造体参照ポインタ
	 * @param x 左上起点のX座標
	 * @param y 左上起点のY座標
	 * @param w 表示のマス幅
	 * @param h 表示のマス高さ
	 * @return なし
	 */
	virtual void ViewFloor(Floor *floorPtr, int x, int y, int w, int h)
	{
		return;
	}

};

