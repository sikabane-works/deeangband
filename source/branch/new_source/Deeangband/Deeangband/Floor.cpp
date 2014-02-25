/*!
 * @file Floor.cpp
 * @brief ゲーム中のフロアを定義する
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Floor.h"

Floor::Floor()
{
	width = 50;
	height = 50;
	squares = new vector<vector<Square>> (height, vector<Square>(width, this));
}

Floor::~Floor()
{
	delete(squares);
}

MapLength Floor::GetWidth(void)
{
	return width;
}

MapLength Floor::GetHeight(void)
{
	return height;
}
