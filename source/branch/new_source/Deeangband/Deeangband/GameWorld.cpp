/*!
 * @file GameWorld.cpp
 * @brief �Q�[�����E���`����
 * @date 2013/12/22
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "GameWorld.h"
#include "Deeangband.h"

GameWorld::GameWorld(GameSurface *gSurface)
{
	gameSurface = gSurface;
}

GameWorld::~GameWorld(void)
{
}

PlayExitCode GameWorld::PlayLoop(void)
{
	return PLAY_EXIT_QUIT;
}

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
