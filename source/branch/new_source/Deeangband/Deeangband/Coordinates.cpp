/*!
    @file Coordinates.cpp
    @brief CoordinatesƒNƒ‰ƒX‚Ì’è‹`
    @date 2013/12/19
    @author Deskull
    2014 Sikabane Works.
*/

#include "stdafx.h"
#include "Coordinates.h"

int Coordinates::getFloorX(void)
{
	return this->fx;
}

int Coordinates::getFloorY(void)
{
	return this->fy;
}

int Coordinates::getWorldX(void)
{
	return this->wx;
}

int Coordinates::getWorldY(void)
{
	return this->wy;
}
