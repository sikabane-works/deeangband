/*!
 *  @file GameElement.h
 *  @brief GameElementクラスと付随要素のプロトタイプ
 *  @date 2014/02/01
 *  @author Deskull
 *  2013-2014 Sikabane Works.
 */

#pragma once

typedef int ID;     //!< ゲーム要素ID
typedef string Tag; //!< ゲーム要素文字列ID
typedef int Weight; //!< 重量
typedef int AC;     //!< 頑健性
typedef int EV;     //!< 回避力
typedef int VO;     //!< 意志力
typedef int DEPTH;  //!< 階層深度
typedef int NUMBER; //!< 個数
typedef int PPM;    //!< 百万分率

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

	ID id;       /*!< ID */
	Tag tag;     /*!< タグ */

public:

	/*!
	 * @brief IDを設定する
	 */
	void setID(ID newID); 

	/*!
	 * @brief IDを取得する
	 */
	ID getID();

	/*!
	 * @brief タグを設定する
	 */
	void setTag(Tag newTag);

	/*!
	 * @brief タグを取得する
	 */
	Tag getTag();

};
