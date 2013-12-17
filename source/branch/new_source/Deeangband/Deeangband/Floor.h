/*!
    @file Floor.h
    @brief ゲーム中のダンジョンと付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

#include "stdafx.h"
#include "GameElement.h"

/*! @class Floor
    @brief フロアのクラス
*/
class Floor : public GameElement
{
private:

	int width; /*!< フロアの横サイズ*/
	int height; /*!< フロアの縦サイズ*/

public:

	/*! フロアの横サイズを得る */
	int get_width(void);

	/*! フロアの縦サイズを得る */
	int get_height(void);
};
