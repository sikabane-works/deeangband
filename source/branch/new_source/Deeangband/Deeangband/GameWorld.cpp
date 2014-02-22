/*!
 * @file GameWorld.cpp
 * @brief ƒQ[ƒ€¢ŠE‚ð’è‹`‚·‚é
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
	GameCommand commandID;
	PlayExitCode code = PLAY_EXIT_NONE;
	gameSurface->Redraw();

	while(code == PLAY_EXIT_NONE)
	{
		commandID = gameSurface->GetCommand();
		code = DoGameCommand(commandID);
	}

	return PLAY_EXIT_QUIT;
}

PlayExitCode GameWorld::DoGameCommand(GameCommand command)
{
	Creature dammy;
	switch(command)
	{
		case GAME_COMMAND_VIEW_PLAYER_STATUS:
			gameSurface->ViewCreatureStatus(&dammy);
			break;
		case GAME_COMMAND_REDRAW:
			gameSurface->Redraw();
			break;
		case GAME_COMMAND_EXIT:
			return PLAY_EXIT_QUIT;
			break;
	}
	return PLAY_EXIT_NONE;
}
