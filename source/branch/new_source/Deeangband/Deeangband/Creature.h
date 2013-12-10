#include "GameElement.h"

typedef int Status;

enum CREATURE_STATUS
{
	CS_STR,
	CS_INT,
	CS_WIS,
	CS_DEX,
	CS_CON,
	CS_CHR,
	CS_MAX
};


struct status_table
{
	Status current[CS_MAX];
	Status max[CS_MAX];
	Status max_max[CS_MAX];
};

class Creature : public GameElement
{
private:
public:
	bool is_alive(void);
};

