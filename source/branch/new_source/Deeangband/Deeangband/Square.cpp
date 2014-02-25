/*!
 * @file Square.cpp
 * @brief ゲーム中のマスを定義する
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Square.h"

Square::Square(Floor *floor_ptr)
{
	this->floor_ptr = floor_ptr;
	feature_id = 0;
}

Square::~Square()
{
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



