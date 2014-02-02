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


/*! 
 * @class GameElement
 * @brief ゲームの諸要素の管理を行うスーパークラス
 */
class GameElement
{
private:
	ID id; /*!<ID*/
	Tag tag; /*!<タグ*/
public:
	void setID(ID newID); /*!<IDを設定する*/
	ID getID();   /*!<IDを取得する*/
	void setTag(Tag newTag); /*!<タグを設定する*/
	Tag getTag();   /*!<タグを取得する*/
};
