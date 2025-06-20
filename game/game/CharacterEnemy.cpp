#include "CharacterEnemy.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "AssetManager.h"

CharacterEnemy::CharacterEnemy()
{
	m_size = { 200.f, 200.f };
	m_healthPoint = 50;
	m_characterSpeed = 0.0f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;
}

CharacterEnemy::~CharacterEnemy() {}

void CharacterEnemy::Init(AEVec2 position)
{
	m_position = position;
	m_animation.Init();

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Idle/Battlemage Idle.png", nullptr, 8, SpriteSheetOrientation::VERTICAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Death/Battlemage Death.png", nullptr, 12, SpriteSheetOrientation::VERTICAL, 0.1f, false };

	for (auto& pair : m_animDataMap) {
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath.c_str());
	}

	m_currentAnimState = CharacterAnimationState::IDLE;
	m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
}

void CharacterEnemy::Update(f32 dt)
{
	if (m_animation.GetCurrentState() == CharacterAnimationState::DEATH && m_animation.IsFinished())
	{
		return;
	}

	m_animation.Update(dt);
}

void CharacterEnemy::Move(f32 dt)
{
	// TODO
}

void CharacterEnemy::Attack()
{
	// TODO
}


void CharacterEnemy::Draw()
{
	AEMtx33 scale = { 0 };
	if (m_currentDirection == CharacterDirection::LEFT)
		AEMtx33Scale(&scale, -m_size.x, m_size.y);
	else
		AEMtx33Scale(&scale, m_size.x, m_size.y);

	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, 0);

	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, m_position.x, m_position.y);

	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	m_animation.Draw(transform);
	DrawHollowRect(m_position.x, m_position.y, m_size.x, m_size.y, 0.f, 1.f, 0.f);
}

void CharacterEnemy::Destroy()
{
	for (auto& pair : m_animDataMap) {
		if (pair.second.pTexture) AEGfxTextureUnload(pair.second.pTexture);
	}
	m_animation.Destroy();
}

void CharacterEnemy::TakeDamage(s32 damage)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;
	std::cout << "Enemy takes damage! HP: " << m_healthPoint << std::endl;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}