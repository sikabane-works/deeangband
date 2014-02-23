/*!
    @file Creature.h
    @brief ゲーム中のクリーチャーと付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

#include "stdafx.h"
#include "Coordinates.h"
#include "Effect.h"
#include "Floor.h"
#include "Item.h"
#include "GameElement.h"

typedef int Status; /*!< 基礎能力値の変数型 */
typedef int HP;     /*!< HPの変数型 */
typedef int MP;     /*!< MPの変数型 */
typedef int SOUL;   /*!< ソウルの変数型 */

/*!
 * @brief クリーチャーの基礎能力値6種の列挙体
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

/*!
 * @struct StatusTable
 * @brief ステータス値の構造体
 */
struct StatusTable
{
	Status current[CS_MAX]; /*!<現在のステータス*/
	Status max[CS_MAX]; /*!<通常のステータス*/
	Status maxMax[CS_MAX]; /*!<ステータスの基本成長限界*/
};

/*!
 * @class Creature
 * @brief クリーチャーのクラス
 */
class Creature : public GameElement
{
private:

	string name;      /*!< クリーチャーの名前 */
	HP currentHp;     /*!< 現在のHP */
	HP max_hp;        /*!< 現在の最大HP */
	HP maxMaxHp;      /*!< 本来の最大HP */
	MP currentMp;     /*!< 現在のMP */
	MP maxMp;         /*!< 現在の最大MP */
	MP maxMaxMp;      /*!< 本来の最大MP */
	SOUL currentSoul; /*!< 現在のソウル値 */
	SOUL maxSoul;     /*!< ソウル最大到達値 */

	StatusTable status; /*!< ステータス */
	ID floorID; /*!< 現在いるフロアID */
	Coordinates position; /*!< 現座標 */
	map<ID, Tag, Item> itemList; /*!< 所持アイテムリスト */
	char symbol; /*!< シンボルキャラ */
	Color symbolColor; /*!< シンボルカラー */
	ID tileID ;/*!< タイルID */

	/*!
	 * @brief クリーチャー死亡時の処理を行う
	 */
	void dead(void);

	/*!
	 * @brief クリーチャーの現HPを増減させる
	 */
	void calc_hp(HP amount);

	/*!
	 * @brief クリーチャーの現MPを増減させる
	 */
	void calc_mp(MP amount);

	/*!
	 * @brief クリーチャーの現HPを設定する
	 */
	void set_hp(HP amount);

	/*!
	 * @brief クリーチャーの現MPを設定する
	 */
	void set_mp(MP amount);

	/*!
	 * @brief クリーチャーを所定フロアの所定座標に移動する
	 * @param floorID 移動先フロアID
	 * @param x 移動先x座標
	 * @param y 移動先y座標
	 * @return 移動できなかった場合のエラーID
	 */
	int move(ID floor_ID, int x, int y);

	/*!
	 * @brief クリーチャーを所定フロアの所定座標に移動する
	 * @param floorID 移動先フロアID
	 * @param coord 移動先座標
	 * @return 移動できなかった場合のエラーID
	 */
	int move(ID floor_ID, Coordinates coord);

public:

	/*!
	 * @brief クリーチャーのダミー生成処理コンストラクタ
	 */
	Creature::Creature(void);

	/*!
	 * @brief クリーチャーの名前を返す
	 * @return クリーチャー名
	 */
	string Creature::GetName(void);

	/*!
	 * @brief クリーチャーが生存している状態かを返す
	 * @return 生存しているならばtrueを返す
	 */
	bool Creature::is_alive(void);

	/*!
	 * @brief 受けた属性効果を処理する
	 * @param effectPtr
	 * @param amount
	 */
	bool Creature::take_effect(Effect* effectPtr, POWER amount);

	/*!
	 * @brief クリーチャーの現HPを返す
	 * @return クリーチャーの現HP
	 */
	HP Creature::GetCurHP(void);

	/*!
	 * @brief クリーチャーの現最大HPを返す
	 * @return クリーチャーの現最大HP
	 */
	HP Creature::GetMaxHP(void);

	/*!
	 * @brief クリーチャーの通常最大HPを返す
	 * @return クリーチャーの通常最大HP
	 */
	HP Creature::GetNorMaxHP(void);

	/*!
	 * @brief クリーチャーの現MPを返す
	 * @return クリーチャーの現MP
	 */
	MP Creature::GetCurMP(void);

	/*!
	 * @brief クリーチャーの現最大MPを返す
	 * @return クリーチャーの現最大MP
	 */
	MP Creature::GetMaxMP(void);

	/*!
	 * @brief クリーチャーの通常最大MPを返す
	 * @return クリーチャーの通常最大MP
	 */
	MP Creature::GetNorMaxMP(void);

	/*!
	 * @brief クリーチャーの現ACを返す
	 * @return クリーチャーの現AC
	 */
	AC Creature::GetAC(void);

	/*!
	 * @brief クリーチャーの現EVを返す
	 * @return クリーチャーの現EV
	 */
	EV Creature::GetEV(void);

	/*!
	 * @brief クリーチャーの現VOを返す
	 * @return クリーチャーの現VO
	 */
	VO Creature::GetVO(void);

	/*!
	 * @brief クリーチャーの現在位置を返す
	 * @return クリーチャー現在位置を示すCoordinates
	 */
	Coordinates *Creature::getCorrdinates(void);

	/*!
	 * @brief クリーチャーの所持品リストを返す
	 * @return クリーチャー所持品リストマップ
	 */
	map<ID, Tag, Item> *Creature::getItemList(void);

	/*!
	 * @brief クリーチャーの現ソウル値を返す
	 * @return クリーチャーの現ソウル値
	 */
	SOUL Creature::GetCurrentSoul(void);

	/*!
	 * @brief クリーチャーのソウル最大到達値を返す
	 * @return クリーチャーのソウル最大到達値
	 */
	SOUL Creature::GetMaxSoul(void);

	/*!
	 * @brief クリーチャーの現基礎能力値を返す
	 * @param stat 取得したい基礎能力値ID
	 * @return 基礎能力値の現在値
	 */
	Status Creature::GetCurrentStatus(CREATURE_STATUS stat);

	/*!
	 * @brief クリーチャーの平常基礎能力値を返す
	 * @param stat 取得したい平常能力値ID
	 * @return 基礎能力値の平常値
	 */
	Status Creature::GetMaxStatus(CREATURE_STATUS stat);

	/*!
	 * @brief クリーチャーの限界基礎能力値を返す
	 * @param stat 取得したい限界能力値ID
	 * @return 基礎能力値の限界値
	 */
	Status Creature::GetMaxMAXStatus(CREATURE_STATUS stat);

};

