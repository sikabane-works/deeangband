/*!
 * @file Dice.cpp
 * @brief ゲーム中のダイス処理を実装する
 * @date 2014/01/19
 * @author Deskull
 * 2014 Sikabane Works.
 */

#include "stdafx.h"
#include "Dice.h"

int Dice::getNum(void)
{
	return this->num;
}

int Dice::getSide(void)
{
	return this->side;
}

int Dice::maxThrow(void)
{
	return this->num * this->side;
}

int Dice::rand0(int max)
{
	return rand() % max;
}

int Dice::rand1(int max)
{
	return rand() % max + 1;
}
