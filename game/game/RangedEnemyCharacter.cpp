#include "RangedEnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "AssetManager.h"

RangedEnemyCharacter::RangedEnemyCharacter()
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

	m_hitboxSize = { m_size.x * 0.7f, m_size.y * 0.9f };
	m_hitboxOffset = { 0.f, 0.f };
	m_isHurt = false;
}

RangedEnemyCharacter::~RangedEnemyCharacter() {}

void RangedEnemyCharacter::Init(AEVec2 position) 
{
	ACharacter::Init(position);
}

void RangedEnemyCharacter::Init(AEVec2 position, PlayerCharacter* player)
{
	m_position = position;
	m_pPlayer = player;
	m_animation.Init();
}

void RangedEnemyCharacter::Update(f32 dt)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		m_animation.Update(dt);
		return;
	}
	if (!m_pPlayer)
		return;

	if (m_isHurt && m_animation.IsFinished())
	{
		m_isHurt = false;
	}

	AEVec2 playerPos = m_pPlayer->GetPosition();
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
		desiredAnimState = CharacterAnimationState::RANGED_ATTACK;
	}
	else if (m_isHurt)
	{
		desiredAnimState = CharacterAnimationState::HURT;
	}

	m_position.x += velocityX * dt;

	if (m_currentAnimState != desiredAnimState)
	{
		m_currentAnimState = desiredAnimState;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}

	m_animation.Update(dt);
}

void RangedEnemyCharacter::Move(f32 dt) {}
void RangedEnemyCharacter::Attack() {}

void RangedEnemyCharacter::Draw()
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
	DrawHollowRect(m_position.x + m_hitboxOffset.x, m_position.y + m_hitboxOffset.y, m_hitboxSize.x, m_hitboxSize.y, 1.0f, 0.0f, 0.0f, 1.f);
}

void RangedEnemyCharacter::Destroy()
{
	m_animation.Destroy();
}

void RangedEnemyCharacter::TakeDamage(s32 damage, DamageType damageType)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;
	std::cout << "Enemy takes damage! HP: " << m_healthPoint << std::endl;

	m_isHurt = true;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}