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

	authorityList.insert(map<Tag, Authority>::value_type("NONE", Authority()));
	creatureTraitList.insert(map<Tag, CreatureTrait>::value_type("NONE", CreatureTrait()));
	effectList.insert(map<Tag, Effect>::value_type("NONE", Effect()));
	fixedArtifactList.insert(map<Tag, FixedArtifact>::value_type("NONE", FixedArtifact()));
	featureList.insert(map<Tag, Feature>::value_type("NONE", Feature()));
	itemBaseList.insert(map<Tag, ItemBase>::value_type("NONE", ItemBase()));
	itemEgoList.insert(map<Tag, ItemEgo>::value_type("NONE", ItemEgo()));
	itemTraitList.insert(map<Tag, ItemTrait>::value_type("NONE", ItemTrait()));
	karmaList.insert(map<Tag, Karma>::value_type("NONE", Karma()));
	skillList.insert(map<Tag, Skill>::value_type("NONE", Skill()));
	speciesList.insert(map<Tag, Species>::value_type("NONE", Species()));
	trapBaseList.insert(map<Tag, TrapBase>::value_type("NONE", TrapBase()));

	buildingList.insert(map<ID, Building>::value_type(0, Building()));
	campList.insert(map<ID, Camp>::value_type(0, Camp()));
	creatureList.insert(map<ID, Creature>::value_type(0, Creature()));
	dungeonList.insert(map<ID, Dungeon>::value_type(0, Dungeon(0, "DAMMY", this)));
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

Authority *GameWorld::GetAuthority(Tag tag)
{
	return &authorityList[tag];
}

CreatureTrait *GameWorld::GetCreatureTrait(Tag tag)
{
	return &creatureTraitList[tag];
}

Effect *GameWorld::GetEffect(Tag tag)
{
	return &effectList[tag];
}

FixedArtifact *GameWorld::GetFixedArtifact(Tag tag)
{
	return &fixedArtifactList[tag];
}

Feature *GameWorld::GetFeature(Tag tag)
{
	return &featureList[tag];
}

ItemBase *GameWorld::GetItemBase(Tag tag)
{
	return &itemBaseList[tag];
}

ItemEgo *GameWorld::GetItemEgo(Tag tag)
{
	return &itemEgoList[tag];
}

ItemTrait *GameWorld::GetItemTrait(Tag tag)
{
	return &itemTraitList[tag];
}

Karma *GameWorld::GetKarma(Tag tag)
{
	return &karmaList[tag];
}

Skill *GameWorld::GetSkill(Tag tag)
{
	return &skillList[tag];
}

Species *GameWorld::GetSpecies(Tag tag)
{
	return &speciesList[tag];
}

TrapBase *GameWorld::GetTrapBase(Tag tag)
{
	return &trapBaseList[tag];
}

Building *GameWorld::GetBuilding(ID id)
{
	return &buildingList[id];
}

Camp *GameWorld::GetCamp(ID id)
{
	return &campList[id];
}

Creature *GameWorld::GetCreature(ID id)
{
	return &creatureList[id];
}

Dungeon *GameWorld::GetDungeon(ID id)
{
	return &dungeonList[id];
}

Item *GameWorld::GetItem(ID id)
{
	return &itemList[id];
}

Floor *GameWorld::GetFloor(ID id)
{
	return &floorList[id];
}
