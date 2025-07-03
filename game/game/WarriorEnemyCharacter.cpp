#include "WarriorEnemyCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "PlayerCharacter.h"
#include "AssetManager.h"

WarriorEnemyCharacter::WarriorEnemyCharacter()
{
	m_size = { 200.f, 200.f };
	m_healthPoint = 50;
	m_hitboxSize = { m_size.x * 0.45f, m_size.y * 0.75f };
	m_hitboxOffset = { m_size.x * 0.05f, -m_size.y * 0.1f };
	m_characterSpeed = 100.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 1000.0f;
	m_attackRange = 100.0f;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 2.0f;

	m_isHurt = false;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_isGrounded = false;
}

WarriorEnemyCharacter::WarriorEnemyCharacter(const WarriorEnemyCharacter& prototype)
{
	m_size = prototype.m_size;
	m_healthPoint = prototype.m_healthPoint;
	m_characterSpeed = prototype.m_characterSpeed;
	m_currentDirection = prototype.m_currentDirection;
	m_currentAnimState = prototype.m_currentAnimState;

	m_currentAIState = prototype.m_currentAIState;
	m_pPlayer = prototype.m_pPlayer;
	m_detectionRange = prototype.m_detectionRange;
	m_attackRange = prototype.m_attackRange;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = prototype.m_attackCooldownDuration;

	m_animation = prototype.m_animation;
	m_animDataMap = prototype.m_animDataMap;
	m_hitboxSize = { m_size.x * 0.7f, m_size.y * 0.9f };
	m_hitboxOffset = { 0.f, 0.f };
	m_isHurt = false;

	m_velocityX = prototype.m_velocityX;
	m_velocityY = prototype.m_velocityY;
	m_gravity = prototype.m_gravity;
	m_isGrounded = prototype.m_isGrounded;
}

WarriorEnemyCharacter::~WarriorEnemyCharacter()
{
}

void WarriorEnemyCharacter::Init(AEVec2 position, PlayerCharacter* player)
{
	MeleeEnemyCharacter::Init(position, player);

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Fantasy Skeleton Enemies/warrior/idle.PNG", nullptr, 8, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Fantasy Skeleton Enemies/warrior/death.PNG", nullptr, 13, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/Fantasy Skeleton Enemies/warrior/walk.PNG", nullptr, 6, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::MELEE_ATTACK] = { "Assets/Fantasy Skeleton Enemies/warrior/attack.PNG", nullptr, 13, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::HURT] = { "Assets/Fantasy Skeleton Enemies/warrior/hurt.PNG", nullptr, 5, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };

	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath);
	}

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}

void WarriorEnemyCharacter::TakeDamage(s32 damage, DamageType damageType)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}
		
	m_healthPoint -= damage;
	std::cout << "Warrior Enemy takes damage! HP: " << m_healthPoint << std::endl;

	m_isHurt = true;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}

WarriorEnemyCharacter* WarriorEnemyCharacter::Clone()
{
	return new WarriorEnemyCharacter(*this);
}
