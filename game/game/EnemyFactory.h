#pragma once
#include "ACharacter.h"
#include <map>
#include <string>

class EnemyFactory
{
public:
	EnemyFactory();
	~EnemyFactory();

	void RegisterPrototype(std::string& name, ACharacter* prototype);
	ACharacter* Create(const std::string& name);

private:
	std::map<std::string, ACharacter*> m_enemyPrototypes;
};

