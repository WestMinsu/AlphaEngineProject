#include "MageEnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "AssetManager.h"

MageEnemyCharacter::MageEnemyCharacter()
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

MageEnemyCharacter::~MageEnemyCharacter() {}

void MageEnemyCharacter::Init(AEVec2 position) {}

void MageEnemyCharacter::Init(AEVec2 position, PlayerCharacter* player)
{
	m_position = position;
	m_pPlayer = player;
	m_animation.Init();

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Fantasy Skeleton Enemies/mage/idle.PNG", nullptr, 8, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Fantasy Skeleton Enemies/mage/death.PNG", nullptr, 18, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/Fantasy Skeleton Enemies/mage/walk.PNG", nullptr, 6, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::RANGED_ATTACK] = { "Assets/Fantasy Skeleton Enemies/mage/attack.PNG", nullptr, 21, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::HURT] = { "Assets/Fantasy Skeleton Enemies/mage/hurt.PNG", nullptr, 5, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath);
	}

	m_projectileData.speed = 800.0f;
	m_projectileData.damage = 5;
	m_projectileData.size = { 150.f, 150.f };
	m_projectileData.animData = { "Assets/MagicArrow/enemyarrow.png", nullptr, 15, SpriteSheetOrientation::HORIZONTAL, 0.05f, true };
	m_projectileData.animData.pTexture = LoadImageAsset(m_projectileData.animData.texturePath.c_str());

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}

void MageEnemyCharacter::Update(f32 dt)
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

void MageEnemyCharacter::Move(f32 dt) {}
void MageEnemyCharacter::Attack() {}

void MageEnemyCharacter::Draw()
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

void MageEnemyCharacter::Destroy()
{
	m_animation.Destroy();
}

void MageEnemyCharacter::TakeDamage(s32 damage)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;
	std::cout << "Mage Enemy takes damage! HP: " << m_healthPoint << std::endl;

	m_isHurt = true;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}