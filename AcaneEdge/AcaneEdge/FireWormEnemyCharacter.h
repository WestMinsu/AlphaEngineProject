#pragma once 
#include "RangedEnemyCharacter.h"

class FireWormEnemyCharacter : public RangedEnemyCharacter
{
public:
	FireWormEnemyCharacter();
	FireWormEnemyCharacter(const FireWormEnemyCharacter&);
	~FireWormEnemyCharacter();

	void Init(AEVec2 position, PlayerCharacter* player);
	FireWormEnemyCharacter* Clone() override;
	bool isReadytoFireRange() override;
};