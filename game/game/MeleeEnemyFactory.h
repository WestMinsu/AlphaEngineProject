#pragma once
#include "AEnemyFactory.h"

class MeleeEnemyFactory : public AEnemyFactory
{
public:
	ACharacter* CreateEnemy(AEVec2 position) override;
};

