#pragma once
#include "AEnemyFactory.h"

class MageEnemyFactory : public AEnemyFactory
{
public:
	ACharacter* CreateEnemy(AEVec2 position) override;
};

