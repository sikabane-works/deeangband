/*!
 *  @file GameElement.h
 *  @brief GameElementクラスと付随要素のプロトタイプ
 *  @date 2014/02/01
 *  @author Deskull
 *  2013-2014 Sikabane Works.
 */

#pragma once

#include "Deeangband.h"

typedef int ID;     //!< ゲーム要素ID
typedef string Tag; //!< ゲーム要素文字列ID
typedef int Weight; //!< 重量
typedef int AC;     //!< 頑健性
typedef int EV;     //!< 回避力
typedef int VO;     //!< 意志力
typedef int DEPTH;  //!< 階層深度
typedef int NUMBER; //!< 個数
typedef int PPM;    //!< 百万分率
typedef int MapLength; //!<マップ距離
typedef int POWER;     //!<効力

/*!
 * @struct color
 * @brief 色指定の構造体
 */
typedef struct 
{
	int r;
	int g;
	int b;
	int a;
} Color;

/*! 
 * @class GameElement
 * @brief ゲームの諸要素の管理を行うスーパークラス
 */
class GameElement
{
private:

	ID id;   //!< ID
	Tag tag; //!< タグ
	GameWorld *gameWorld; //!< 所属元のGameWorld

public:

	GameElement::GameElement(void); 

	GameElement::GameElement(ID id, Tag tag, GameWorld *gameWorld); 

	GameElement::~GameElement(void); 


	/*!
	 * @brief IDを設定する
	 */
	void GameElement::SetID(ID newID); 

	/*!
	 * @brief IDを取得する
	 */
	ID GameElement::GetID();

	/*!
	 * @brief タグを設定する
	 */
	void GameElement::SetTag(Tag newTag);

	/*!
	 * @brief タグを取得する
	 */
	Tag GameElement::GetTag();

	/*!
	 * @brief 所属元のGameWorldインスタンスを返す
	 * @return GameWorldインスタンスの参照ポインタ
	 */
	GameWorld *GameElement::GetGameWorld();
};
