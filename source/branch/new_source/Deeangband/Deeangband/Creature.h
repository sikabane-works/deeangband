/*!
    @file Creature.h
    @brief ゲーム中のクリーチャーと付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Effect.h"
#include "GameElement.h"

typedef int Status; /*!<基礎能力値*/
typedef int HP; /*!<HP*/
typedef int MP; /*!<MP*/

/*! @enum CREATURE_STATUS
    @brief クリーチャーの基礎能力値6種の列挙体
*/
enum CREATURE_STATUS
{
	CS_STR, /*!<腕力*/
	CS_INT, /*!<知力*/
	CS_WIS, /*!<賢さ*/
	CS_DEX, /*!<器用*/
	CS_CON, /*!<耐久*/
	CS_CHR, /*!<魅力*/
	CS_MAX  /*!<総数:6*/
};


/*! @struct status_table
    @brief ステータス値の構造体
*/
struct status_table
{
	Status current[CS_MAX]; /*!<現在のステータス*/
	Status max[CS_MAX]; /*!<通常のステータス*/
	Status max_max[CS_MAX]; /*!<ステータスの基本成長限界*/
};

/*! @class Creature
    @brief クリーチャーのクラス
*/
class Creature : public GameElement
{
private:

	string name; /*!< クリーチャーの名前*/
	HP current_hp; /*!< 現在のHP*/
	HP max_hp; /*!< 現在の最大HP*/
	HP max_max_hp; /*!< 本来の最大HP*/
	MP current_mp; /*!< 現在のHP*/
	MP max_mp; /*!< 現在の最大HP*/
	MP max_max_mp; /*!< 本来の最大HP*/
	status_table status; /*!< クリーチャーのステータス */
	ID floorID; /*!< 現在いるフロアID */

	/*! クリーチャー死亡時の処理を行う */
	void dead(void);

	/*! クリーチャーの現HPを増減させる */
	void calc_hp(HP amount);

	/*! クリーチャーの現MPを増減させる */
	void calc_mp(MP amount);

	/*! クリーチャーの現HPを設定する */
	void set_hp(HP amount);

	/*! クリーチャーの現MPを設定する */
	void set_mp(MP amount);

public:

	/*!
	 * @brief クリーチャーが生存している状態かを返す
	 * @return 生存しているならばtrueを返す
	 */
	bool is_alive(void);

	/*!
	 * @brief 受けた属性効果を処理する
	 * @param effectPtr
	 * @param amount
	 */
	bool take_effect(Effect* effectPtr, POWER amount);

	/*!
	 * @brief クリーチャーの現HPを返す
	 * @return クリーチャーの現HP
	 */
	HP getCurHP(void);

	/*!
	 * @brief クリーチャーの現最大HPを返す
	 * @return クリーチャーの現最大HP
	 */
	HP getMaxHP(void);

	/*!
	 * @brief クリーチャーの通常最大HPを返す
	 * @return クリーチャーの通常最大HP
	 */
	HP getNorMaxHP(void);

	/*!
	 * @brief クリーチャーの現MPを返す
	 * @return クリーチャーの現MP
	 */
	MP getCurMP(void);

	/*!
	 * @brief クリーチャーの現最大MPを返す
	 * @return クリーチャーの現最大MP
	 */
	MP getMaxMP(void);

	/*!
	 * @brief クリーチャーの通常最大MPを返す
	 * @return クリーチャーの通常最大MP
	 */
	MP getNorMaxMP(void);

	/*!
	 * @brief クリーチャーの現ACを返す
	 * @return クリーチャーの現AC
	 */
	AC getAC(void);

	/*!
	 * @brief クリーチャーの現EVを返す
	 * @return クリーチャーの現EV
	 */
	EV getEV(void);

	/*!
	 * @brief クリーチャーの現VOを返す
	 * @return クリーチャーの現VO
	 */
	VO getVO(void);

};

