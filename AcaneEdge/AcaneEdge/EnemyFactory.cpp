#include "EnemyFactory.h"

EnemyFactory::EnemyFactory()
{
}

EnemyFactory::~EnemyFactory()
{
	for (auto& pair : m_enemyPrototypes)
	{
		delete pair.second;
	}
}

void EnemyFactory::RegisterPrototype(std::string name, ACharacter* prototype)
{
	m_enemyPrototypes[name] = prototype;
}

ACharacter* EnemyFactory::Create(const std::string& name)
{
	auto it = m_enemyPrototypes.find(name);
	if (it != m_enemyPrototypes.end())
	{
		return it->second->Clone();	
	}
	return nullptr;
}

void EnemyFactory::Destroy()
{
	for (auto& enemeyPrototype : m_enemyPrototypes)
	{
		enemeyPrototype.second->Destroy();
	}
	m_enemyPrototypes.clear();
}
