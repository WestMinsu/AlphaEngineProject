#include "CharacterMeleeEnemy.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "CharacterPlayer.h"
#include "AssetManager.h"

CharacterMeleeEnemy::CharacterMeleeEnemy()
{
	m_size = { 200.f, 200.f };
	m_healthPoint = 50;
	m_characterSpeed = 100.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 500.0f;
	m_attackRange = 150.0f;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 2.0f;
}

CharacterMeleeEnemy::~CharacterMeleeEnemy() 
{
}

void CharacterMeleeEnemy::Init(AEVec2 position)
{
}

void CharacterMeleeEnemy::Init(AEVec2 position, CharacterPlayer* player)
{
	m_position = position;
	m_pPlayer = player; 
	m_animation.Init();

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Fantasy Skeleton Enemies/warrior/idle.PNG", nullptr, 8, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Fantasy Skeleton Enemies/warrior/death.PNG", nullptr, 13, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/Fantasy Skeleton Enemies/warrior/walk.PNG", nullptr, 6, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::MELEE_ATTACK] = { "Assets/Fantasy Skeleton Enemies/warrior/attack.PNG", nullptr, 13, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };

	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath);
	}

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}


void CharacterMeleeEnemy::Update(f32 dt)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		m_animation.Update(dt);
		return;
	}
	if (!m_pPlayer) 
		return;

	const AEVec2& playerPosConst = m_pPlayer->GetPosition();
	AEVec2 playerPos = playerPosConst;
	float distanceToPlayer = AEVec2Distance(&m_position, &playerPos);

	switch (m_currentAIState)
	{
	case EnemyAIState::IDLE:
	{
		if (distanceToPlayer < m_detectionRange)
			m_currentAIState = EnemyAIState::CHASE;
		break;
	}
	case EnemyAIState::CHASE:
	{
		if (distanceToPlayer < m_attackRange)
			m_currentAIState = EnemyAIState::ATTACK;
		else if (distanceToPlayer > m_detectionRange)
			m_currentAIState = EnemyAIState::IDLE;
		break;
	}
	case EnemyAIState::ATTACK:
	{
		if (m_animation.IsFinished())
		{
			m_attackCooldownTimer = 0.0f;
			m_currentAIState = EnemyAIState::COOLDOWN;
		}
		break;
	}
	case EnemyAIState::COOLDOWN:
	{
		m_attackCooldownTimer += dt;
		if (m_attackCooldownTimer >= m_attackCooldownDuration)
			m_currentAIState = EnemyAIState::IDLE;
		break;
	}
	}

	f32 velocityX = 0.0f;
	CharacterAnimationState desiredAnimState = CharacterAnimationState::IDLE;

	if (m_currentAIState == EnemyAIState::CHASE)
	{
		if (m_pPlayer->GetPosition().x < m_position.x)
		{
			m_currentDirection = CharacterDirection::LEFT;
			velocityX = -m_characterSpeed;
		}
		else
		{
			m_currentDirection = CharacterDirection::RIGHT;
			velocityX = m_characterSpeed;
		}
		desiredAnimState = CharacterAnimationState::WALK;
	}
	else if (m_currentAIState == EnemyAIState::ATTACK)
	{
		desiredAnimState = CharacterAnimationState::MELEE_ATTACK;
	}

	m_position.x += velocityX * dt;

	if (m_currentAnimState != desiredAnimState)
	{
		m_currentAnimState = desiredAnimState;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}

	m_animation.Update(dt);
}

void CharacterMeleeEnemy::Move(f32 dt)
{
	// TODO
}

void CharacterMeleeEnemy::Attack()
{
	// TODO
}


void CharacterMeleeEnemy::Draw()
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

void CharacterMeleeEnemy::Destroy()
{
	m_animation.Destroy();
}

void CharacterMeleeEnemy::TakeDamage(s32 damage)
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