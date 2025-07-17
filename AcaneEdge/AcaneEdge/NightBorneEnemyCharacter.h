#pragma once 
#include "MeleeEnemyCharacter.h"

class NightBorneEnemyCharacter : public MeleeEnemyCharacter
{
public:
	NightBorneEnemyCharacter();
	NightBorneEnemyCharacter(const NightBorneEnemyCharacter&);
	~NightBorneEnemyCharacter();

	void Init(AEVec2 position, PlayerCharacter* player);
	NightBorneEnemyCharacter* Clone() override;
};