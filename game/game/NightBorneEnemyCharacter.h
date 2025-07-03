#pragma once 
#include "MeleeEnemyCharacter.h"

class NightBorneEnemyCharacter : public MeleeEnemyCharacter
{
public:
	NightBorneEnemyCharacter();
	~NightBorneEnemyCharacter();

	void Init(AEVec2 position, PlayerCharacter* player);
	void TakeDamage(s32 damage, DamageType damageType) override;
};