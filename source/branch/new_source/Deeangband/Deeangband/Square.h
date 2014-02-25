/*!
 * @file Square.h
 * @brief ゲーム中のマスと付随要素のプロトタイプ
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "Deeangband.h"
#include "GameElement.h"

/*! 
 * @class Square
 * @brief マスのクラス
 */
class Square : public GameElement
{
private:
	Floor *floor_ptr; //!<所属フロア参照ポインタ
	ID feature_id; //!<地形ID
public:

	Square::Square(Floor *floor_ptr);

	Square::~Square();

	/*!
	 * @brief 所属するフロアのポインタを返す。
	 */
	Floor* getFloorPtr(void);

	/*!
	 * @brief 対応する地形のIDを返す。
	 */
	ID getFeatureID(void);

	/*!
	 * @brief 対応する地形のポインタを返す。
	 */
	Feature* getFeaturePtr(void);

	/*!
	 * @brief 受けた効果を処理する
	 */
	bool Square::take_effect(Effect *effectPtr, POWER amount);

};
