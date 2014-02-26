/*!
 * @file Floor.cpp
 * @brief ゲーム中のフロアを定義する
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Floor.h"

Floor::Floor(ID id, Tag tag, GameWorld *gameWorld) : GameElement(id, tag, gameWorld)
{
	Floor();
	return;
}

Floor::Floor()
{
	width = 50;
	height = 50;
	squares = new vector<vector<Square>> (height, vector<Square>(width, this));
}

Floor::~Floor()
{
}

MapLength Floor::GetWidth(void)
{
	return width;
}

MapLength Floor::GetHeight(void)
{
	return height;
}

Square *Floor::getSquare(MapLength x, MapLength y)
{
	return &(*squares)[x][y];
}

