#include "FireWormEnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "AssetManager.h"

FireWormEnemyCharacter::FireWormEnemyCharacter()
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

FireWormEnemyCharacter::FireWormEnemyCharacter(const FireWormEnemyCharacter& prototype)
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
	m_idealRange = prototype.m_idealRange;

	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = prototype.m_attackCooldownDuration;
	m_hasFiredProjectile = false;

	m_animation = prototype.m_animation;
	m_animDataMap = prototype.m_animDataMap;
	m_hitboxSize = { m_size.x * 0.7f, m_size.y * 0.9f };
	m_hitboxOffset = { 0.f, 0.f };
	m_isHurt = false;
}

FireWormEnemyCharacter::~FireWormEnemyCharacter()
{
}

void FireWormEnemyCharacter::Init(AEVec2 position, PlayerCharacter* player)
{
	RangedEnemyCharacter::Init(position, player);

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Fire Worm/Sprites/Worm/Idle.PNG", nullptr, 9, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Fire Worm/Sprites/Worm/Death.png", nullptr, 8, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/Fire Worm/Sprites/Worm/Walk.png", nullptr, 9, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::RANGED_ATTACK] = { "Assets/Fire Worm/Sprites/Worm/Attack.png", nullptr, 16, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::HURT] = { "Assets/Fire Worm/Sprites/Worm/Get Hit.png", nullptr, 3, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath);
	}

	m_projectileData.speed = 800.0f;
	m_projectileData.damage = 5;
	m_projectileData.size = { 150.f, 150.f };
	m_projectileData.animData = { "Assets/Fire Worm/Sprites/Fire Ball/Move.png", nullptr, 6, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_projectileData.animData.pTexture = LoadImageAsset(m_projectileData.animData.texturePath.c_str());

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}

void FireWormEnemyCharacter::TakeDamage(s32 damage, DamageType damageType)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	if (damageType == DamageType::FIRE)
	{
		std::cout << "Fire Mage is IMMUNE to fire!" << std::endl;
		return;
	}

	m_healthPoint -= damage;
	std::cout << "Fire Worm Enemy takes damage! HP: " << m_healthPoint << std::endl;

	m_isHurt = true;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}

FireWormEnemyCharacter* FireWormEnemyCharacter::Clone()
{
	return new FireWormEnemyCharacter(*this);
}
