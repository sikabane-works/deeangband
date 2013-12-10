/*!
    @file GameElement.h
    @brief GameElementクラスとその他付随要素のプロトタイプ
    @date 2013/12/11
    @author Deskull
    2013 Sikabane Works.
*/

typedef int ID; /*!<ゲーム要素ID*/

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
