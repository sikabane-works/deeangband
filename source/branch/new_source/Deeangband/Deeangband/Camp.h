/*!
 * @file Camp.h
 * @brief ゲーム中の陣営と付随要素のプロトタイプ
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "GameElement.h"

#pragma once

class Camp : public GameElement
{

private:

public:

	Camp::Camp(ID id, Tag tag, GameWorld *gameWorld);

	Camp::Camp(void);

	Camp::~Camp(void);

};