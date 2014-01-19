/*!
    @file GameElement.h
    @brief GameElementクラスと付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#pragma once

typedef int ID; //!<ゲーム要素ID
typedef string StrID; //! ゲーム要素文字列ID
typedef int Weight; //!< 重量
typedef int AC; //!< 頑健性
typedef int EV; //!< 回避力
typedef int VO; //!< 意志力


/*! @class GameElement
    @brief ゲームの諸要素の管理を行うスーパークラス
*/
class GameElement
{
private:
	ID id; /*!<ゲーム要素のID*/
public:
	void set_id(); /*!<IDを設定する*/
	ID get_id();   /*!<IDを取得する*/
};
