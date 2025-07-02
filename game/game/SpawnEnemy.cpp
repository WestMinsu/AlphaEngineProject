#include "SpawnEnemy.h"


SpawnEnemy::SpawnEnemy()
{
	m_position = {0,0};
	m_spawnTimes = 1;
	m_spawnCurrentTime = 0.0f;
	m_spawnTerm = 2.0f;
	m_EnemyFactory = nullptr;
	m_enemyName = "";
}

SpawnEnemy::SpawnEnemy(AEVec2 pos, EnemyFactory* factory, std::string name)
{
	m_position = pos;
	m_spawnTimes = 1;
	m_spawnCurrentTime = 0.0f;
	m_spawnTerm = 2.0f;
	m_EnemyFactory = factory;
	m_enemyName = name;
}

SpawnEnemy::~SpawnEnemy()
{

}

void SpawnEnemy::Init(AEVec2 pos, EnemyFactory* factory, std::string name)
{
	m_position = pos;
	m_EnemyFactory = factory;
	m_enemyName = name;
}

void SpawnEnemy::Update(f32 dt, std::vector<ACharacter*>& enemies)
{
	m_spawnCurrentTime += dt;
	if (m_spawnTimes > 0 && m_spawnCurrentTime > m_spawnTerm)
	{
		ACharacter* newEnemy = m_EnemyFactory->Create(m_enemyName);
		newEnemy->Init(m_position);
		enemies.push_back(newEnemy);
		m_spawnTimes--;
		m_spawnCurrentTime = 0.f;
	}
}

void SpawnEnemy::Destroy()
{

}

