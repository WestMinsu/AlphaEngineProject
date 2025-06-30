#include "MageEnemyFactory.h"
#include "MageEnemyCharacter.h"

ACharacter* MageEnemyFactory::CreateEnemy(AEVec2 position, PlayerCharacter* player)
{
    MageEnemyCharacter* mage = new MageEnemyCharacter();
    mage->Init(position, player);
    return mage;
}
