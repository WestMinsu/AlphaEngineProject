#pragma once
#include "AEEngine.h"

enum class CharacterAnimationState
{
	IDLE,
	WALK,
	JUMP,
	MELEE_ATTACK,
	PROJECTILE_ATTACK,
	DASH,
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
	virtual ~ACharacter() {};
	virtual void Init(AEVec2 position) = 0;
	virtual void Update(f32 dt) = 0;
	virtual void Move(f32 dt) = 0;
	virtual void Attack() = 0;
	virtual void Draw() = 0;
	virtual void Destroy() = 0;
	virtual void TakeDamage(s32 damage) = 0;

	const AEVec2& GetPosition() const 
	{ 
		return m_position;
	}
	const AEVec2& GetSize() const 
	{
		return m_size;
	}
	CharacterDirection GetDirection() const 
	{
		return m_currentDirection; 
	}
	s32 GetHealth() const 
	{ 
		return m_healthPoint;
	}	

protected:
	AEVec2 m_position;
	AEVec2 m_size;

	s32 m_healthPoint;
	f32 m_characterSpeed;
	f32 m_airAcceleration;

	CharacterDirection m_currentDirection;
};