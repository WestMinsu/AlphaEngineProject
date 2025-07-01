#pragma once 
#include "RangedEnemyCharacter.h"

class FireWormEnemyCharacter : public RangedEnemyCharacter
{
public:
	FireWormEnemyCharacter();
	FireWormEnemyCharacter(FireWormEnemyCharacter*);
	~FireWormEnemyCharacter();

	void Init(AEVec2 position, PlayerCharacter* player);
	void TakeDamage(s32 damage, DamageType damageType) override;
	FireWormEnemyCharacter* Clone() override;
};