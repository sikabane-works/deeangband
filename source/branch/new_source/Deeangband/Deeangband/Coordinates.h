/*!
    @file Coordinates.h
    @brief Coordinatesクラスとその他付随要素の定義
    @date 2014/01/26
    @author Deskull
    2014 Sikabane Works.
*/

#pragma once

/*! @class Coordinates
 *  @brief ゲーム世界座標のクラス
 */
class Coordinates
{
private:
	int fx; //!< フロアX座標
	int fy; //!< フロアY座標
	int wx; //!< 広域マップX座標
	int wy; //!< 広域マップY座標
public:

	/*
	 * @brief フロアX座標を得る
	 * @return フロアX座標
	 */
	int getFloorX(void);

	/*
	 * @brief フロアY座標を得る
	 * @return フロアY座標を得る
	 */
	int getFloorY(void);

	/*
	 * @brief 広域マップX座標を得る
	 * @return 広域マップX座標
	 */
	int getWorldX(void);

	/*
	 * @brief 広域マップY座標を得る
	 * @return 広域マップY座標
	 */
	int getWorldY(void);
};
