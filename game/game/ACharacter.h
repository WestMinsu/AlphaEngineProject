#pragma once
#include "AEEngine.h"

class ACharacter
{
public:
	~ACharacter() {};
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Move() = 0;
	virtual void Attack() = 0;

private:
	AEVec2 m_position;
	int m_healthPoint;
};

