/*!
 * @file Dungeon.h
 * @brief ゲーム中のダンジョンと付随要素のプロトタイプ
 * @date 2013/12/11
 * @author Deskull
 * 2013 Sikabane Works.
 */

#include "stdafx.h"
#include "Camp.h"
#include "Species.h"
#include "Feature.h"
#include "GameElement.h"

#pragma once

/*! 
 * @class Dungeon
 * @brief ダンジョンのクラス
 */
class Dungeon : public GameElement
{
private:
	DEPTH minDepth;      //!< 開始階層
	DEPTH maxDepth;      //!< 最深階層
	ID outerWallFeature; //!< 基本外壁地形ID
	ID innerWallFeature; //!< 基本内壁地形ID
	ID controlCamp;      //!< 所有陣営ID
	ID gurdianSpecies;   //!< ダンジョンの守護者ID
public:

	DEPTH GetMinDepth(void);

	DEPTH GetMaxDepth(void);

	ID GetOuterWallFeatureID(void);

	Feature *GetOuterWallFeaturePtr(void);

	ID GetInnerWallFeatureID(void);

	Feature *GetInnerWallFeaturePtr(void);

	ID GetControlCampID(void);

	Camp *GetControlCampPtr(void);

	ID GetGurdianSpeciesID(void);

	Species *GetGurdianSpeciesPtr(void);

};
