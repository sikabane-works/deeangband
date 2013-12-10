/*!
    @file Creature.h
    @brief ゲーム中のクリーチャーと付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#include "stdafx.h"
#include "GameElement.h"

typedef int Status; /*!<基礎能力値*/
typedef int HP; /*!<HP*/
typedef int MP; /*!<MP*/
typedef int POWER; /*!<効力*/

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
	Status current[CS_MAX];
	Status max[CS_MAX];
	Status max_max[CS_MAX];
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

	/*! クリーチャーが生存している状態かを返す */
	bool is_alive(void);

	/*! クリーチャーに属性効果を与える */
	bool take_effect(POWER amount);

};

