#pragma once
#include "AEEngine.h"
#include "MainGameState.h"

class ACharacter
{
public:
	~ACharacter() {};
	virtual void Init(AEVec2 position) = 0;
	virtual void Update() = 0;
	virtual void Move() = 0;
	virtual void Attack() = 0;
	virtual void Draw() = 0;

protected:
	AEVec2 m_position;
	AEVec2 m_size;

	int m_healthPoint;
	f32 m_characterSpeed = 300.0f;

	CharacterDirection m_currentDirection;
};

