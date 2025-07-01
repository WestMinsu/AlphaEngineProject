#pragma once 
#include "RangedEnemyCharacter.h"

class MageEnemyCharacter : public RangedEnemyCharacter
{
public:
	MageEnemyCharacter();
	MageEnemyCharacter(MageEnemyCharacter*);
	~MageEnemyCharacter();

	void Init(AEVec2 position, PlayerCharacter* player);
	MageEnemyCharacter* Clone() override;
	void TakeDamage(s32 damage, DamageType damageType) override;
};