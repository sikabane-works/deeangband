/*!
 * @file GameWorld.cpp
 * @brief ÉQÅ[ÉÄê¢äEÇíËã`Ç∑ÇÈ
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

	authorityList.insert(map<ID, Authority>::value_type(0, Authority()));
	creatureTraitList.insert(map<ID, CreatureTrait>::value_type(0, CreatureTrait()));
	effectList.insert(map<ID, Effect>::value_type(0, Effect()));
	fixedArtifactList.insert(map<ID, FixedArtifact>::value_type(0, FixedArtifact()));
	featureList.insert(map<ID, Feature>::value_type(0, Feature()));
	itemBaseList.insert(map<ID, ItemBase>::value_type(0, ItemBase()));
	itemEgoList.insert(map<ID, ItemEgo>::value_type(0, ItemEgo()));
	itemTraitList.insert(map<ID, ItemTrait>::value_type(0, ItemTrait()));
	karmatList.insert(map<ID, Karma>::value_type(0, Karma()));
	skillList.insert(map<ID, Skill>::value_type(0, Skill()));
	speciesList.insert(map<ID, Species>::value_type(0, Species()));
	trapbaseList.insert(map<ID, TrapBase>::value_type(0, TrapBase()));

	buildingList.insert(map<ID, Building>::value_type(0, Building()));
	campList.insert(map<ID, Camp>::value_type(0, Camp()));
	creatureList.insert(map<ID, Creature>::value_type(0, Creature()));
	dungeonList.insert(map<ID, Dungeon>::value_type(0, Dungeon()));
	itemList.insert(map<ID, Item>::value_type(0, Item()));
	floorList.insert(map<ID, Floor>::value_type(0, Floor()));

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
