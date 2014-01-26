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

	/*!
	 * @brief ダイスの数を返す
	 * @return ダイスの数
	 */
	int getNum(void);

	/*!
	 * @brief ダイスの面数を返す
	 * @return ダイスの面数
	 */
	int getSide(void);

	/*!
	 * @brief ダイスを振る
	 * @return ダイスの総計値
	 */
	int cast(void);

	/*!
	 * @brief ダイスを振る
	 * @param num ダイスの数
	 * @param side ダイスの面数
	 * @return ダイスの総計値
	 */
	static int cast(int num, int side);

	/*!
	 * @brief ダイスの取り得る最大値を返す
	 * @return ダイスの最大値
	 */
	int maxCast(void);

	/*!
	 * @brief ダイスの取り得る最大値を返す
	 * @param num ダイスの数
	 * @param side ダイスの面数
	 * @return ダイスの最大値
	 */
	static int maxCast(int num, int side);

	/*!
	 * @brief 0を最小値とした乱数を返す
     * @param max 乱数の範囲(0～max-1)
	 * @return 乱数の値
	 */
	static int rand0(int max);

	/*!
	 * @brief 1を最小値とした乱数を返す
     * @param max 乱数の最大値(1～max)
	 * @return 乱数の値
	 */
	static int rand1(int max);
};
