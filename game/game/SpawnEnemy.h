#pragma once
#include <AEEngine.h>
#include "EnemyFactory.h"
#include <vector>
#include "ACharacter.h"
#include "BossCharacter.h"

class SpawnEnemy
{
public:
	SpawnEnemy();
	SpawnEnemy(AEVec2 pos, EnemyFactory* factory, std::string name, s32 spawnTimes);
	~SpawnEnemy();

	void Init(AEVec2 pos, EnemyFactory* factory, std::string name, s32 spawnTimes);
	
	void Update(f32 dt, AEVec2 positionPlayer, s32 moveTileMapCount, std::vector<ACharacter*>& enemies);
	void Update(f32 dt, AEVec2 positionPlayer, s32 moveTileMapCount, std::vector<BossCharacter*>& bosses);
	void Destroy();

	void Reset();

private:
	AEVec2 m_position;
	s32 m_spawnTimes;

	f32 m_spawnCurrentTime;
	f32 m_spawnTerm;
	std::string m_enemyName;
	EnemyFactory* m_EnemyFactory;

	s32 m_resetCount;
};

