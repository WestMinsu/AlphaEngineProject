#include "Projectile.h"
#include "Constants.h"
#include <iostream>
#include "Utility.h"

Projectile::Projectile()
{
	m_position = { 0,0 };
	m_velocity = { 0,0 };
	m_size = { 0,0 };
	m_damage = 0;
	m_isActive = true;
}

Projectile::~Projectile()
{
}



void Projectile::Init(AEVec2 startPos, CharacterDirection dir, const ProjectileData& data)
{
	m_position = startPos;
	m_size = data.size;
	m_damage = data.damage;

	if (dir == CharacterDirection::RIGHT)
	{
		m_velocity.x = data.speed;
	}
	else
	{
		m_velocity.x = -data.speed;
	}

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

	if (m_position.x < -kHalfWindowWidth - m_size.x || m_position.x > kHalfWindowWidth + m_size.x)
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
	DrawHollowRect(m_position.x, m_position.y, m_size.x, m_size.y, 0.0f, 0.0f, 1.0f, 0.5f);
}

void Projectile::Destroy()
{
	if (m_animData.pTexture)
	{
		AEGfxTextureUnload(m_animData.pTexture);
	}
	m_animation.Destroy();
}
