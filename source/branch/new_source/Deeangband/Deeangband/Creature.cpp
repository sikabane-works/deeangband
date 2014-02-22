/*!
 * @file Creature.cpp
 * @brief ゲーム中のクリーチャーを定義する
 * @date 2013/12/10
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Creature.h"

Creature::Creature(void)
{
	name = "ダミー";
	current_hp = max_hp = max_max_hp = 15;
	current_mp = max_mp = max_max_mp = 10;
}

bool Creature::is_alive(void)
{
	return true;
}

bool Creature::take_effect(Effect *effectPtr, POWER amount)
{
	return false;
}

void Creature::dead(void)
{
}

int Creature::move(ID floor_ID, int x, int y)
{
	this->floorID = floor_ID;
	return 0;
}

int Creature::move(ID floor_ID, Coordinates coord)
{
	this->move(floor_ID, coord.getFloorX(), coord.getFloorY());
	return 0;
}

void Creature::calc_hp(HP amount)
{
	this->current_hp += amount;
	if(this->current_hp > this->max_hp) this->current_hp = this->max_hp;
	if(this->current_hp < 1) this->dead();
}

void Creature::calc_mp(MP amount)
{
	this->current_mp += amount;
	if(this->current_mp > this->max_mp) this->current_mp = this->max_mp;
	if(this->current_mp < 0) this->current_mp = 0;
}

void Creature::set_hp(HP amount)
{
	this->current_hp = amount;
	if(this->current_hp < this->max_hp) this->current_hp = this->max_hp;
	if(this->current_hp < 1) this->dead();
}

void Creature::set_mp(MP amount)
{
	this->current_mp += amount;
	if(this->current_mp > this->max_mp) this->current_mp = this->max_mp;
	if(this->current_mp < 0) this->current_mp = 0;
}

string Creature::GetName(void)
{
	return name;
}

HP Creature::GetCurHP(void)
{
	return current_hp;
}

HP Creature::GetMaxHP(void)
{
	return max_hp;
}

HP Creature::GetNorMaxHP(void)
{
	return max_max_hp;
}

MP Creature::GetCurMP(void)
{
	return current_mp;
}

MP Creature::GetMaxMP(void)
{
	return max_mp;
}

MP Creature::GetNorMaxMP(void)
{
	return max_max_mp;
}

AC Creature::GetAC(void)
{
	return 10;
}

EV Creature::GetEV(void)
{
	return 10;
}

VO Creature::GetVO(void)
{
	return 10;
}
