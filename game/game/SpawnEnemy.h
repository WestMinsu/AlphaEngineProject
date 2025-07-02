#pragma once
#include <AEEngine.h>
#include "EnemyFactory.h"
#include <vector>
#include "ACharacter.h"

class SpawnEnemy
{
public:
	SpawnEnemy();
	SpawnEnemy(AEVec2 pos, EnemyFactory* factory, std::string name);
	~SpawnEnemy();

	void Init(AEVec2 pos, EnemyFactory* factory, std::string name);
	void Update(f32 dt, std::vector<ACharacter*>&);
	void Destroy();

private:
	AEVec2 m_position;
	s32 m_spawnTimes;

	f32 m_spawnCurrentTime;
	f32 m_spawnTerm;
	std::string m_enemyName;
	EnemyFactory* m_EnemyFactory;
};

