#include "SpawnEnemy.h"
#include "Constants.h"
#include <iostream>

SpawnEnemy::SpawnEnemy()
{
	m_position = {0,0};
	m_spawnTimes = 1;
	m_spawnCurrentTime = 0.0f;
	m_spawnTerm = 2.0f;
	m_EnemyFactory = nullptr;
	m_enemyName = "";
	m_resetCount = 0;
}

SpawnEnemy::SpawnEnemy(AEVec2 pos, EnemyFactory* factory, std::string name, s32 spawnTimes)
{
	m_position = pos;
	m_spawnTimes = spawnTimes;
	m_spawnCurrentTime = 0.0f;
	m_spawnTerm = 2.0f;
	m_EnemyFactory = factory;
	m_enemyName = name;
	m_resetCount = 0;
}

SpawnEnemy::~SpawnEnemy()
{

}

void SpawnEnemy::Init(AEVec2 pos, EnemyFactory* factory, std::string name, s32 spawnTimes)
{
	m_position = pos;
	m_spawnTimes = spawnTimes;
	m_EnemyFactory = factory;
	m_enemyName = name;
}


void SpawnEnemy::Update(f32 dt, AEVec2 positionPlayer, s32 moveTileMapCount, std::vector<ACharacter*>& enemies)
{
	m_spawnCurrentTime += dt;
	if (m_spawnTimes > 0 
		&& m_spawnCurrentTime >= m_spawnTerm
		&& m_resetCount == moveTileMapCount
		)
	{
		ACharacter* newEnemy = m_EnemyFactory->Create(m_enemyName);
		newEnemy->Init({ m_position.x, m_position.y });
		enemies.push_back(newEnemy);
		m_spawnTimes--;
		m_spawnCurrentTime = 0.f;
	}

	if (m_resetCount < moveTileMapCount
		&& m_resetCount < 2
		//&& m_position.x + 2400 < positionPlayer.x
		&& 2 * 1856 + m_position.x - positionPlayer.x < kHalfWindowWidth + 100) {
		Reset();
	}
}


void SpawnEnemy::Update(f32 dt, AEVec2 positionPlayer, s32 moveTileMapCount, std::vector<BossCharacter*>& bosses)
{
	m_spawnCurrentTime += dt;
	if (m_spawnTimes > 0 
		&& m_position.x - positionPlayer.x < kHalfWindowWidth - 200)
	{
		BossCharacter* newBoss = dynamic_cast<BossCharacter*>(m_EnemyFactory->Create(m_enemyName));
		newBoss->Init(m_position);
		bosses.push_back(newBoss);
		m_spawnTimes--;
		m_spawnCurrentTime = 0.f;
	}
}



void SpawnEnemy::Destroy()
{

}

void SpawnEnemy::Reset()
{
	f32 camX, camY;
	AEGfxGetCamPosition(&camX, &camY);
	//std::cout << m_position.x << "," << camX << std::endl;
	//std::cout << 2 * 1856 * (1 + m_resetCount) - kHalfWindowWidth << std::endl;
	if (camX > 2 * 1856 * (1 + m_resetCount) - kHalfWindowWidth && camX > m_position.x)
	{
		m_position.x += 2 * 1856;
		m_resetCount++;
		m_spawnTimes = 2*m_resetCount;
		m_spawnCurrentTime = 0.f;
		
	}
}

