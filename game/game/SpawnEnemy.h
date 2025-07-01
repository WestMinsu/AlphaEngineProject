#pragma once
#include <AEEngine.h>
#include "EnemyFactory.h"
#include <vector>
#include "ACharacter.h"

class SpawnEnemy
{
public:
	SpawnEnemy(AEVec2 pos);
	~SpawnEnemy();

	void Update(f32 dt, std::vector<ACharacter*>&);
	void Destroy();

private:
	AEVec2 m_position;
	s32 m_spawnTimes;

	f32 m_spawnCurrentTime;
	f32 m_spawnTerm;
	EnemyFactory* m_EnemyFactory;
};

