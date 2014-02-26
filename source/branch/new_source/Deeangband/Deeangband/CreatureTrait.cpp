/*!
 *  @file CreatureTrait.cpp
 *  @brief ゲーム中のクリーチャー特性を定義する
 *  @date 2013/12/22
 *  @author Deskull
 *  2013 Sikabane Works.
 */

#include "stdafx.h"
#include "CreatureTrait.h"

CreatureTrait::CreatureTrait(ID id, Tag tag, GameWorld *gameWorld) : CreatureTemplate(id, tag, gameWorld)
{
	return;
}

CreatureTrait::CreatureTrait(void)
{
	return;
}

CreatureTrait::~CreatureTrait(void)
{
	return;
}
