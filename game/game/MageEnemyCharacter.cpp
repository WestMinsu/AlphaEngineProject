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
	m_element = ElementType::NONE;

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

MageEnemyCharacter::MageEnemyCharacter(const MageEnemyCharacter& prototype)
{
	m_size = prototype.m_size;
	m_healthPoint = prototype.m_healthPoint;
	m_characterSpeed = prototype.m_characterSpeed;
	m_currentDirection = prototype.m_currentDirection;
	m_currentAnimState = prototype.m_currentAnimState;
	m_animation = prototype.m_animation;
	m_animDataMap = prototype.m_animDataMap;
	m_element = prototype.m_element;

	m_currentAIState = prototype.m_currentAIState;
	m_pPlayer = prototype.m_pPlayer;
	m_detectionRange = prototype.m_detectionRange;
	m_attackRange = prototype.m_attackRange;

	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = prototype.m_attackCooldownDuration;
	m_hasFiredProjectile = false;

	m_hitboxSize = prototype.m_hitboxSize;
	m_hitboxOffset = prototype.m_hitboxOffset;
	m_isHurt = false;

	m_projectileData = prototype.m_projectileData;
}

MageEnemyCharacter::~MageEnemyCharacter() {}

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

bool MageEnemyCharacter::isReadytoFireRange()
{
	return m_currentAnimState == CharacterAnimationState::RANGED_ATTACK
	&& (m_animation.GetCurrentFrame() == 11 && !m_hasFiredProjectile);
}

MageEnemyCharacter* MageEnemyCharacter::Clone()
{
	return new MageEnemyCharacter(*this);
}
