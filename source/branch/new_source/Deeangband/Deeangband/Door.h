/*!
 * @file Door.h
 * @brief ゲーム中のトラップのプロトタイプ定義
 * @date 2014/01/26
 * @author Deskull
 * 2014 Sikabane Works.
 */

#include "stdafx.h"
#include "Coordinates.h"
#include "GameElement.h"

class Door : public GameElement
{
private:
	ID floorID; //!<所在フロアID
	Coordinates position; //!<所在座標
public:

};