/*!
    @file Item.h
    @brief ゲーム中のアイテムと付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Effect.h"
#include "GameElement.h"

/*! @class Item
    @brief アイテムのクラス
*/
class Item : public GameElement
{
private:
	Weight weight; //!< アイテムの重量
	AC baseAC; //!< 基本装甲修正
	EV baseEV; //!< 基本回避修正
	VO baseVO; //!< 基本意志修正
	AC plusAC; //!< 追加装甲修正
	EV plusEV; //!< 追加回避修正
	VO plusVO; //!< 追加意志修正
public:
	
	/*! 受けた効果を処理する */
	bool Item::take_effect(Effect *effectPtr, POWER amount);

	/*! 基本装甲修正を返す */
	AC getBaseAC(void);

	/*! 追加装甲修正を返す */
	AC getPlusAC(void);

	/*! 基本回避修正を返す */
	EV getBaseEV(void);

	/*! 追加回避修正を返す */
	EV getPlusEV(void);

	/*! 基本意志修正を返す */
	VO getBaseVO(void);

	/*! 追加意志修正を返す */
	VO getPlusVO(void);

};
