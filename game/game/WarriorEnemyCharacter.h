#pragma once 
#include "MeleeEnemyCharacter.h"

class WarriorEnemyCharacter : public MeleeEnemyCharacter
{
public:
	WarriorEnemyCharacter();
	WarriorEnemyCharacter(const WarriorEnemyCharacter&);
	~WarriorEnemyCharacter();

	void Init(AEVec2 position, PlayerCharacter* player);
	void TakeDamage(s32 damage, DamageType damageType) override;
	WarriorEnemyCharacter* Clone() override;
};