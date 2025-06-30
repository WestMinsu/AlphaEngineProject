#pragma once
#include "AEnemyFactory.h"
#include "PlayerCharacter.h"

class MageEnemyFactory : public AEnemyFactory
{
public:
	ACharacter* CreateEnemy(AEVec2 position, PlayerCharacter* player) override;
};

