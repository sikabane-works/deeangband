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

int Dice::cast(void)
{
	int i, n = 0;
	for(i = 0; i < this->num; i++) n += rand1(this->side);
	return n;
}

int Dice::cast(int num, int side)
{
	int i, n = 0;
	for(i = 0; i < num; i++) n += rand1(side);
	return n;
};

int Dice::maxCast(void)
{
	return this->num * this->side;
}

int Dice::maxCast(int num, int side)
{
	return num * side;
};

int Dice::rand0(int max)
{
	return rand() % max;
}

int Dice::rand1(int max)
{
	return rand() % max + 1;
}
