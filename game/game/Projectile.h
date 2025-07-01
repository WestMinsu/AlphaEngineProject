#pragma once
#include "AEEngine.h"
#include "Animation.h"
#include "ACharacter.h"
#include "WeaponData.h"
#include "GameTypes.h"

class Projectile
{
public:
	Projectile();
	~Projectile();

	void Init(AEVec2 startPos, AEVec2 direction, const ProjectileData& data);
	void Update(f32 dt);
	void Draw();
	void Destroy();

	bool IsActive() const { return m_isActive; }
	void Deactivate() { m_isActive = false; }

	const AEVec2& GetPosition() const { return m_position; }
	const AEVec2& GetSize() const { return m_size; }
	s32 GetDamage() const { return m_damage; }
	DamageType GetType() const { return m_type; }
private:
	AEVec2 m_position;
	AEVec2 m_velocity;
	AEVec2 m_size;
	s32 m_damage;
	Animation m_animation;
	AnimData m_animData;
	DamageType m_type;
	bool m_isActive;
};