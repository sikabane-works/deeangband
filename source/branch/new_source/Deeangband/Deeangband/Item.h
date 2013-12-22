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
public:
	
	/*! 受けた効果を処理する */
	bool Item::take_effect(POWER amount, Effect *effectPtr);

};
