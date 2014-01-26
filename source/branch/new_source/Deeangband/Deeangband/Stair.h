/*!
 * @file Stair.h
 * @brief ゲーム中の階段のプロトタイプ
 * @date 2014/01/26
 * @author Deskull
 * 2014 Sikabane Works.
 */

#include "stdafx.h"
#include "Coordinates.h"
#include "GameElement.h"

class Stair : public GameElement
{
private:
	ID floorID1; //!<接続フロアID1
	ID floorID2; //!<接続フロアID2
	Coordinates position1; //!<接続座標1
	Coordinates position1; //!<接続座標2
public:

};