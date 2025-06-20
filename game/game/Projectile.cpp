#include "Projectile.h"
#include "Constants.h"
#include <iostream>
#include "AssetManager.h"

Projectile::Projectile()
{
	m_position = { 0,0 };
	m_velocity = { 0,0 };
	m_size = { 200.f, 100.f };
	m_isActive = true;
}

Projectile::~Projectile() {}

void Projectile::Init(AEVec2 startPos, CharacterDirection dir)
{
	m_position = startPos;

	float speed = 1000.0f;
	if (dir == CharacterDirection::RIGHT)
	{
		m_velocity.x = speed;
	}
	else
	{
		m_velocity.x = -speed;
	}

	m_animation.Init();

	m_animData.texturePath = "Assets/MagicArrow/fire.png";
	m_animData.pTexture = LoadImageAsset(m_animData.texturePath.c_str());
	m_animData.frameCount = 15;
	m_animData.orientation = SpriteSheetOrientation::HORIZONTAL;
	m_animData.frameDuration = 0.05f;
	m_animData.loop = true;

	m_animation.Play(CharacterAnimationState::IDLE, m_animData);
}

void Projectile::Update(f32 dt)
{
	if (!m_isActive) return;

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
	if (!m_isActive) return;

	AEMtx33 scale = { 0 };
	if (m_velocity.x < 0)
		AEMtx33Scale(&scale, -m_size.x * 1.5f, m_size.y * 2.5f);
	else
		AEMtx33Scale(&scale, m_size.x * 1.5f, m_size.y * 2.5f);

	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, 0);

	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, m_position.x, m_position.y);

	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	m_animation.Draw(transform);
}

void Projectile::Destroy()
{
	/*if (m_animData.pTexture)
	{
		AEGfxTextureUnload(m_animData.pTexture);
	}*/
	m_animation.Destroy();
}
