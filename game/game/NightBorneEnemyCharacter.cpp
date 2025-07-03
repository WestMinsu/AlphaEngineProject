#include "NightBorneEnemyCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "PlayerCharacter.h"
#include "AssetManager.h"

NightBorneEnemyCharacter::NightBorneEnemyCharacter()
{
	m_size = { 250.f, 250.f };
	m_healthPoint = 50;
	m_hitboxSize = { m_size.x * 0.45f, m_size.y * 0.4f };
	m_hitboxOffset = { m_size.x * 0.05f, -m_size.y * 0.25f };
	m_characterSpeed = 100.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 500.0f;
	m_attackRange = 100.0f;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 2.0f;
	
	m_isHurt = false;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_isGrounded = false;
}

NightBorneEnemyCharacter::~NightBorneEnemyCharacter()
{
}

void NightBorneEnemyCharacter::Init(AEVec2 position, PlayerCharacter* player)
{
	MeleeEnemyCharacter::Init(position, player);

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/NightBorne/idle.PNG", nullptr, 9, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/NightBorne/death.PNG", nullptr, 23, SpriteSheetOrientation::HORIZONTAL, 0.07f, false };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/NightBorne/run.PNG", nullptr, 6, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::MELEE_ATTACK] = { "Assets/NightBorne/attack.PNG", nullptr, 12, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::HURT] = { "Assets/NightBorne/hurt.PNG", nullptr, 5, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };

	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath);
	}

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}

void NightBorneEnemyCharacter::TakeDamage(s32 damage, DamageType damageType)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;
	std::cout << "NightBorne takes damage! HP: " << m_healthPoint << std::endl;

	m_isHurt = true;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}
