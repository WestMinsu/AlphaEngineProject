#pragma once 
#include "RangedEnemyCharacter.h"

class MageEnemyCharacter : public RangedEnemyCharacter
{
public:
	MageEnemyCharacter();
	MageEnemyCharacter(const MageEnemyCharacter&);
	~MageEnemyCharacter();

	void Init(AEVec2 position, PlayerCharacter* player);
	MageEnemyCharacter* Clone() override;
	bool isReadytoFireRange() override;
};