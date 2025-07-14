#include "Projectile.h"
#include "Constants.h"
#include <iostream>
#include "Utility.h"
#include "AssetManager.h"

Projectile::Projectile()
{
	m_position = { 0,0 };
	m_velocity = { 0,0 };
	m_size = { 0,0 };
	m_damage = 0;
	m_isActive = true;
	m_type = DamageType::NONE;
	m_startPosition = { 0,0 };
	m_maxRange = 800.f;
}

Projectile::~Projectile()
{
}

void Projectile::Init(AEVec2 startPos, AEVec2 direction, const ProjectileData& data)
{
	m_position = startPos;
	m_startPosition = startPos;
	m_size = data.size;
	m_damage = data.damage;
	m_type = data.type;

	AEVec2Normalize(&direction, &direction);
	m_velocity.x = direction.x * data.speed;
	m_velocity.y = direction.y * data.speed;

	m_hitboxSize = { data.size.x * 0.2f, data.size.y * 0.8f }; 
	m_hitboxOffset = { data.size.x * 0.15f, 0.0f };

	m_animation.Init();
	m_animation.Play(CharacterAnimationState::IDLE, data.animData);
}

void Projectile::Update(f32 dt)
{
	if (!m_isActive)
		return;

	m_position.x += m_velocity.x * dt;
	m_position.y += m_velocity.y * dt;
	m_animation.Update(dt);

	if (AEVec2Distance(&m_position, &m_startPosition) > m_maxRange)
	{
		m_isActive = false;
	}
}

void Projectile::Draw()
{
	if (!m_isActive)
		return;

	AEMtx33 scale = { 0 };
	AEMtx33 rotate = { 0 };
	AEMtx33 translate = { 0 };
	AEMtx33 transform = { 0 };

	AEMtx33Trans(&translate, m_position.x, m_position.y);
	AEMtx33Rot(&rotate, 0);
	AEMtx33Scale(&scale, m_velocity.x > 0 ? m_size.x : -m_size.x, m_size.y);
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	m_animation.Draw(transform);
	AEVec2 hitboxWorldPos;
	float dirMultiplier = (m_velocity.x >= 0) ? 1.0f : -1.0f;
	hitboxWorldPos.x = m_position.x + (m_hitboxOffset.x * dirMultiplier);
	hitboxWorldPos.y = m_position.y + m_hitboxOffset.y;

	DrawHollowRect(hitboxWorldPos.x, hitboxWorldPos.y, m_hitboxSize.x, m_hitboxSize.y, 0.0f, 1.0f, 1.0f, 0.5f);
}

void Projectile::Destroy()
{
	m_animation.Destroy();
}
