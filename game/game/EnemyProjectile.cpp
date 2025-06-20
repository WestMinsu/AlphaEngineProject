#include "EnemyProjectile.h"
#include "Constants.h"
#include "AssetManager.h"

EnemyProjectile::EnemyProjectile()
{
	m_position = { 0,0 };
	m_velocity = { 0,0 };
	m_size = { 200.f, 100.f };
	m_isActive = true;
}

EnemyProjectile::~EnemyProjectile()
{
}

void EnemyProjectile::Init(AEVec2 startPos, CharacterDirection dir)
{
	m_position = startPos;

	float speed = 800.0f;
	if (dir == CharacterDirection::RIGHT)
	{
		m_velocity.x = speed;
	}
	else
	{
		m_velocity.x = -speed;
	}

	m_animation.Init();

	m_animData.texturePath = "Assets/MagicArrow/enemyarrow.png";
	m_animData.pTexture = LoadImageAsset(m_animData.texturePath);
	m_animData.frameCount = 15;
	m_animData.orientation = SpriteSheetOrientation::HORIZONTAL;
	m_animData.frameDuration = 0.05f;
	m_animData.loop = true;

	m_animation.Play(CharacterAnimationState::IDLE, m_animData);
}

void EnemyProjectile::Update(f32 dt)
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

void EnemyProjectile::Draw()
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
}

void EnemyProjectile::Destroy()
{
	if (m_animData.pTexture)
	{
		AEGfxTextureUnload(m_animData.pTexture);
	}
	m_animation.Destroy();
}