/*!
 * @file Creature.cpp
 * @brief ゲーム中のクリーチャーを定義する
 * @date 2013/12/10
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Creature.h"

Creature::Creature(ID id, Tag tag, GameWorld *gameWorld) : GameElement(id, tag, gameWorld)
{
	Creature();
	return;
}

Creature::Creature(void)
{
	int statID;
	name = "ダミー";
	currentHp = max_hp = maxMaxHp = 15;
	currentMp = maxMp = maxMaxMp = 10;
	currentSoul = maxSoul = 0;

	for(statID = 0; statID < CS_MAX; statID++)
	{
		status.current[statID] = status.max[statID] = 8;
		status.maxMax[statID] = 20;
	}
}

Creature::~Creature(void)
{
	return;
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
	this->currentHp += amount;
	if(this->currentHp > this->max_hp) this->currentHp = this->max_hp;
	if(this->currentHp < 1) this->dead();
}

void Creature::calc_mp(MP amount)
{
	this->currentMp += amount;
	if(this->currentMp > this->maxMp) this->currentMp = this->maxMp;
	if(this->currentMp < 0) this->currentMp = 0;
}

void Creature::set_hp(HP amount)
{
	this->currentHp = amount;
	if(this->currentHp < this->max_hp) this->currentHp = this->max_hp;
	if(this->currentHp < 1) this->dead();
}

void Creature::set_mp(MP amount)
{
	this->currentMp += amount;
	if(this->currentMp > this->maxMp) this->currentMp = this->maxMp;
	if(this->currentMp < 0) this->currentMp = 0;
}

string Creature::GetName(void)
{
	return name;
}

HP Creature::GetCurHP(void)
{
	return currentHp;
}

HP Creature::GetMaxHP(void)
{
	return max_hp;
}

HP Creature::GetNorMaxHP(void)
{
	return maxMaxHp;
}

MP Creature::GetCurMP(void)
{
	return currentMp;
}

MP Creature::GetMaxMP(void)
{
	return maxMp;
}

MP Creature::GetNorMaxMP(void)
{
	return maxMaxMp;
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

SOUL Creature::GetCurrentSoul(void)
{
	return currentSoul;
}

SOUL Creature::GetMaxSoul(void)
{
	return maxSoul;
}

Status Creature::GetCurrentStatus(CREATURE_STATUS stat)
{
	return status.current[stat];
}

Status Creature::GetMaxStatus(CREATURE_STATUS stat)
{
	return status.max[stat];
}

Status Creature::GetMaxMAXStatus(CREATURE_STATUS stat)
{
	return status.maxMax[stat];
}
