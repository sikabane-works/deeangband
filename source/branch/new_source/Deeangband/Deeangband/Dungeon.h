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
 * @struct 固定フロア生成テーブル
 */
typedef struct FixedFloorTable FixedFloorTable;
struct FixedFloorTable
{
	bool StaticFixed; //!<静的固定
	ID FixedFloorID;  //!<生成される固定フロアのID
	DEPTH MinDepth;   //!<生成最低階層
	DEPTH MaxDepth;   //!<生成最大階層
	NUMBER MinNum;    //!<最低生成数
	NUMBER MaxNum;    //!<最大生成数
	PPM Probability;  //!<生成確率
};



/*! 
 * @class Dungeon
 * @brief ダンジョンのクラス
 */
class Dungeon : public GameElement
{
private:
	list<FixedFloorTable> fixFloorTableList; //!< 固定フロアのリスト
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
