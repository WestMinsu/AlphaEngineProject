#include "MeleeEnemyFactory.h"
#include "MeleeEnemyCharacter.h"

ACharacter* MeleeEnemyFactory::CreateEnemy(AEVec2 position, PlayerCharacter* player)
{
    MeleeEnemyCharacter* melee = new MeleeEnemyCharacter();
    melee->Init(position, player);
    return melee;
}
