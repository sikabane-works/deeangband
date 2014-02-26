/*!
 * @file GameElement.cpp
 * @brief ゲーム中要素のスーパークラス実装
 * @date 2014/01/19
 * @author Deskull
 * 2014 Sikabane Works.
 */

#include "stdafx.h"
#include "GameElement.h"

ID GameElement::GetID(void)
{
	return this->id;
}

void GameElement::SetID(ID newID)
{
	this->id = newID;
}

Tag GameElement::GetTag(void)
{
	return this->tag;
}

void GameElement::SetTag(Tag newTag)
{
	this->tag = newTag;
}

GameWorld *GameElement::GetGameWorld(void)
{
	return gameWorld;
}
