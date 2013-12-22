/*!
    @file Square.h
    @brief ゲーム中のマスと付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"

#include "Effect.h"
#include "Floor.h"
#include "Feature.h"
#include "GameElement.h"

/*! @class Square
    @brief マスのクラス
*/
class Square : public GameElement
{
private:
	ID *floor_id; //!<所属フロアID
	ID *feature_id; //!<地形ID

public:

	/*! 所属するフロアのポインタを返す。*/
	Floor* getFloorPtr(void);

	/*! 対応する地形のポインタを返す。*/
	Feature* getFeaturePtr(void);

	/*! 受けた効果を処理する */
	bool Square::take_effect(POWER amount, Effect *effectPtr);

};
