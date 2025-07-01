#pragma once
#include "AEEngine.h"
#include "WeaponData.h"
#include "GameTypes.h"

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
	virtual void TakeDamage(s32 damage, DamageType damageType) = 0;
	virtual ACharacter* Clone() = 0;

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
	const AEVec2& GetHitboxSize() const
	{
		return m_hitboxSize;
	}
	const AEVec2& GetHitboxOffset() const
	{
		return m_hitboxOffset;
	}
	ElementType GetElement() const 
	{ 
		return m_element; 
	} 
protected:
	AEVec2 m_position;
	AEVec2 m_size;
	AEVec2 m_hitboxSize;
	AEVec2 m_hitboxOffset;

	s32 m_healthPoint;
	f32 m_characterSpeed;
	f32 m_airAcceleration;

	CharacterDirection m_currentDirection;
	ElementType m_element;
};