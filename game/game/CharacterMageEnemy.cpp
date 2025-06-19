#include "CharacterMageEnemy.h"
#include "CharacterPlayer.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "AIState.h"

CharacterMageEnemy::CharacterMageEnemy()
{
	m_size = { 200.f, 200.f };
	m_healthPoint = 40;
	m_characterSpeed = 80.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 800.0f;
	m_attackRange = 600.0f;
	m_idealRange = 500.0f;

	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 3.0f;
	m_hasFiredProjectile = false;
}

CharacterMageEnemy::~CharacterMageEnemy() {}

void CharacterMageEnemy::Init(AEVec2 position) {}

void CharacterMageEnemy::Init(AEVec2 position, CharacterPlayer* player)
{
	m_position = position;
	m_pPlayer = player;
	m_animation.Init();

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Fantasy Skeleton Enemies/mage/idle.PNG", nullptr, 8, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Fantasy Skeleton Enemies/mage/death.PNG", nullptr, 18, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/Fantasy Skeleton Enemies/mage/walk.PNG", nullptr, 6, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::PROJECTILE_ATTACK] = { "Assets/Fantasy Skeleton Enemies/mage/attack.PNG", nullptr, 21, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };

	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = AEGfxTextureLoad(pair.second.texturePath.c_str());
	}

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}

void CharacterMageEnemy::Update(f32 dt)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH && m_animation.IsFinished())
	{
		return;
	}
	if (!m_pPlayer) return;

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
		{
			m_currentAIState = EnemyAIState::ATTACK;
			m_hasFiredProjectile = false;
		}
		else if (distanceToPlayer > m_detectionRange)
		{
			m_currentAIState = EnemyAIState::IDLE;
		}
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
			m_currentAIState = EnemyAIState::CHASE;
		break;
	}
	}

	f32 velocityX = 0.0f;
	CharacterAnimationState desiredAnimState = CharacterAnimationState::IDLE;

	if (m_pPlayer->GetPosition().x < m_position.x)
	{
		m_currentDirection = CharacterDirection::LEFT;
	}
	else
	{
		m_currentDirection = CharacterDirection::RIGHT;
	}

	if (m_currentAIState == EnemyAIState::CHASE)
	{
		if (distanceToPlayer > m_idealRange)
		{
			velocityX = (m_currentDirection == CharacterDirection::RIGHT ? m_characterSpeed : -m_characterSpeed);
			desiredAnimState = CharacterAnimationState::WALK;
		}
	}
	else if (m_currentAIState == EnemyAIState::ATTACK)
	{
		desiredAnimState = CharacterAnimationState::PROJECTILE_ATTACK;
	}

	m_position.x += velocityX * dt;

	if (m_currentAnimState != desiredAnimState)
	{
		m_currentAnimState = desiredAnimState;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}

	m_animation.Update(dt);
}

void CharacterMageEnemy::Move(f32 dt) {}
void CharacterMageEnemy::Attack() {}

void CharacterMageEnemy::Draw()
{
	AEMtx33 scale = { 0 };
	AEMtx33 rotate = { 0 };
	AEMtx33 translate = { 0 };
	AEMtx33 transform = { 0 };

	AEMtx33Trans(&translate, m_position.x, m_position.y);
	AEMtx33Rot(&rotate, 0);
	AEMtx33Scale(&scale, m_currentDirection == CharacterDirection::RIGHT ? m_size.x : -m_size.x, m_size.y);

	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	m_animation.Draw(transform);
	DrawHollowRect(m_position.x, m_position.y, m_size.x, m_size.y, 0.f, 1.f, 0.f, 0.5f);
}

void CharacterMageEnemy::Destroy()
{
	for (auto& pair : m_animDataMap)
	{
		if (pair.second.pTexture)
			AEGfxTextureUnload(pair.second.pTexture);
	}
	m_animation.Destroy();
}

void CharacterMageEnemy::TakeDamage(s32 damage)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;
	std::cout << "Mage Enemy takes damage! HP: " << m_healthPoint << std::endl;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}