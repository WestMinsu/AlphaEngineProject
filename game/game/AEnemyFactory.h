#pragma once
#include "ACharacter.h"

// Abstract Enemy Factory Class
class AEnemyFactory
{
public:
	~AEnemyFactory() {};

	virtual ACharacter* CreateEnemy(AEVec2 position)=0;
};

