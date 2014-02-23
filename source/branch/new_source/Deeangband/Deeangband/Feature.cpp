/*!
 * @file Feature.cpp
 * @brief ゲーム中の地形情報を定義する
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Feature.h"

Feature::Feature(void)
{
	name = "未知の地形";
	symbol = 'X';
	symColor.r = 127;
	symColor.g = 127;
	symColor.b = 127;
	symColor.a = 255;
}

Feature::~Feature(void)
{
}
