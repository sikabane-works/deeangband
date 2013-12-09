#include "stdafx.h"
#include "Creature.h"
#include "GameElement.h"

class Creature : public GameElement
{
private:
public:
	bool is_alive(void)
	{
		return true;
	}
};

