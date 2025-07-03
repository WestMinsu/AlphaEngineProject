#include "MageEnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "AssetManager.h"

MageEnemyCharacter::MageEnemyCharacter()
{
	m_size = { 200.f, 200.f };
	m_hitboxSize = { m_size.x * 0.5f, m_size.y * 0.9f };
	m_hitboxOffset = { m_size.x * 0.05f, -m_size.y * 0.05f };
	m_healthPoint = 40;
	m_characterSpeed = 80.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 800.0f;
	m_attackRange = 600.0f;

	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 3.0f;
	m_hasFiredProjectile = false;
	m_isHurt = false;
}

MageEnemyCharacter::~MageEnemyCharacter() 
{
}

void MageEnemyCharacter::Init(AEVec2 position, PlayerCharacter* player)
{
	RangedEnemyCharacter::Init(position, player);

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

void MageEnemyCharacter::TakeDamage(s32 damage, DamageType damageType)
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