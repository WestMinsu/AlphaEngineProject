#pragma once
#include "ACharacter.h"

// Abstract Enemy Factory Class
class AEnemyFactory
{
	~AEnemyFactory() {};

	virtual ACharacter* CreateEnemy()=0;
};

