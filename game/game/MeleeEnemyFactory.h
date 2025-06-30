#pragma once
#include "AEnemyFactory.h"
#include "PlayerCharacter.h"

class MeleeEnemyFactory : public AEnemyFactory
{
public:
	ACharacter* CreateEnemy(AEVec2 position, PlayerCharacter* player) override;
};

