/*!
    @file Dice.h
    @brief Diceクラスとその他付随要素の定義
    @date 2014/01/19
    @author Deskull
    2014 Sikabane Works.
*/

#pragma once

/*! @class Dice
 *  @brief ゲーム中のダイス処理
 */
class Dice
{
private:
	int num; //!< ダイスの数
	int side; //!< ダイスの面数
public:

	/*! ダイスの数を返す */
	int getNum(void);

	/*! ダイスの面数を返す */
	int getSide(void);

	/*! ダイスの取り得る最大値を返す */
	int maxThrow(void);
};
