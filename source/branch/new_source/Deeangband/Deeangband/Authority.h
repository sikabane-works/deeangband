/*!
 * @file Authority.h
 * @brief 権能のプロトタイプ宣言
 * @date 2013/12/22
 * @author Deskull
 * 2013 Sikabane Works.
 */

#pragma once

#include "stdafx.h"
#include "CreatureTemplate.h"

/*! 
 * @class Authority
 * @brief 権能のクラス
 */
class Authority : public CreatureTemplate
{
private:

public:
	Authority::Authority(ID id, Tag tag, GameWorld *gameWorld);
	Authority::Authority(void);
	Authority::~Authority(void);
};
