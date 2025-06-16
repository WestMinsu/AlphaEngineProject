#pragma once
#include "AEEngine.h"

enum class CharacterAnimationState
{
	IDLE,
	WALK,
	JUMP,
	ATTACK,
	DEATH
};

enum class CharacterDirection
{
	LEFT,
	RIGHT
};


class ACharacter
{
public:
	~ACharacter() {};
	virtual void Init(AEVec2 position) = 0;
	virtual void Update(f32 dt) = 0;
	virtual void Move(f32 dt) = 0;
	virtual void Attack() = 0;
	virtual void Draw() = 0;
	virtual void Destroy() = 0;

protected:
	AEVec2 m_position;
	AEVec2 m_size;

	s32 m_healthPoint;
	f32 m_characterSpeed;

	CharacterDirection m_currentDirection;
};

