#pragma once
#include <AEEngine.h>
#include "EnemyFactory.h"

class SpawnEnemy
{
public:
	SpawnEnemy(AEVec2 pos);
	~SpawnEnemy();

	void Update(f32 dt);
	void Draw();
	void Destroy();

private:
	AEVec2 m_position;
	s32 m_spawnTimes;

	f32 m_spawnCurrentTime;
	f32 m_spawnTerm;
	EnemyFactory* m_EnemyFactory;
};

