/*!
    @file Floor.h
    @brief ゲーム中のダンジョンと付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#include "stdafx.h"
#include "GameElement.h"

typedef int MapLength; /*!<マップ距離*/

/*! @class Floor
    @brief フロアのクラス
*/
class Floor : public GameElement
{
private:

	MapLength width; /*!< フロアの横サイズ*/
	MapLength height; /*!< フロアの縦サイズ*/

public:

	/*! フロアの横サイズを得る */
	MapLength get_width(void);

	/*! フロアの縦サイズを得る */
	MapLength get_height(void);

};
