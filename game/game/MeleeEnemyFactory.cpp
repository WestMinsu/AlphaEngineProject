#include "MeleeEnemyFactory.h"
#include "MeleeEnemyCharacter.h"

ACharacter* MeleeEnemyFactory::CreateEnemy(AEVec2 position)
{
    return new MeleeEnemyCharacter();
}
