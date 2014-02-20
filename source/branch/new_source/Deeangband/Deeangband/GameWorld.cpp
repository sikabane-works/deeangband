/*!
 * @file GameWorld.cpp
 * @brief ゲーム世界を定義する
 * @date 2013/12/22
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "GameWorld.h"

void GameWorld::DoGameCommand(GameCommand command)
{
	switch(command)
	{
	case GAME_COMMAND_REDRAW:
		break;
	case GAME_COMMAND_EXIT:
		break;
	}
}
