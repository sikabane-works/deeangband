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
