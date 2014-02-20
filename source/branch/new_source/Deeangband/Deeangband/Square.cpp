/*!
 * @file Square.cpp
 * @brief ゲーム中のマスを定義する
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Square.h"

MapLength Square::X(void)
{
	return this->x;
}

MapLength Square::Y(void)
{
	return this->y;
}

bool Square::take_effect(Effect *effectPtr, POWER amount)
{
	return false;
}

ID Square::getFeatureID(void)
{
	return this->feature_id;
}

Feature *Square::getFeaturePtr(void)
{
	return NULL;
}



