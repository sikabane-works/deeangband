/*!
 * @file Floor.cpp
 * @brief �Q�[�����̃t���A���`����
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Floor.h"

Floor::Floor()
{
	int i, j;
	width = 50;
	height = 50;

	squares = new vector<vector<Square>> (height, vector<Square>(width));
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
