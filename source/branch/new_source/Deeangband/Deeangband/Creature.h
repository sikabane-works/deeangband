#include "GameElement.h"

typedef int Status;

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
public:

	/*! クリーチャーが生存している状態かを返す */
	bool is_alive(void);
};

