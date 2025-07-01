#include "SpawnEnemy.h"
#include <AEEngine.h>
#include <vector>
#include "EnemyFactory.h"
#include "ACharacter.h"

SpawnEnemy::SpawnEnemy(AEVec2 pos)
{
	m_position = pos;
	m_spawnTimes = 5;
	m_spawnCurrentTime = 0.0f;
	m_spawnTerm = 2.0f;
}

SpawnEnemy::~SpawnEnemy()
{

}

void SpawnEnemy::Update(f32 dt, std::vector<ACharacter*>& enemies)
{
	
}

void SpawnEnemy::Destroy()
{
}

