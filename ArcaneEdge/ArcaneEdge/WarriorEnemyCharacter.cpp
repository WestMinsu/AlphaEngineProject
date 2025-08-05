#include "WarriorEnemyCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "PlayerCharacter.h"
#include "AssetManager.h"
#include <cmath>
#include <cstdlib>
#include <ctime> 

WarriorEnemyCharacter::WarriorEnemyCharacter()
{
	m_size = { 200.f, 200.f };
	m_healthPoint = 50;
	m_hitboxSize = { m_size.x * 0.45f, m_size.y * 0.75f };
	m_hitboxOffset = { m_size.x * 0.1f, -m_size.y * 0.1f };
	m_characterSpeed = 100.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_element = ElementType::NONE;

	m_currentAnimState = CharacterAnimationState::IDLE;
	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 1000.0f;
	m_attackRange = 150.0f;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 2.0f;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_isGrounded = false;
	killScore = 1000;

	m_strafeTimer = 0.0f;
	m_strafeDuration = 0.0f;
	m_strafeDirection = 1.0f;

	m_sfxAttack = LoadSoundAsset("Assets/Sounds/attack_monster.wav");
	m_sfxHurt = LoadSoundAsset("Assets/Sounds/hurt3_monster.wav");
	m_sfxDeath = LoadSoundAsset("Assets/Sounds/die2_monster.wav");
	m_attackSoundFrame = 5;
	m_visualPivotOffset = 40.0f;
}

WarriorEnemyCharacter::WarriorEnemyCharacter(const WarriorEnemyCharacter& prototype)
{
	m_size = prototype.m_size;
	m_healthPoint = prototype.m_healthPoint;
	m_characterSpeed = prototype.m_characterSpeed * (0.8f + static_cast<float>(rand() % 61) / 100.0f);
	m_currentDirection = prototype.m_currentDirection;
	m_currentAnimState = prototype.m_currentAnimState;
	m_element = prototype.m_element;

	m_currentAIState = prototype.m_currentAIState;
	m_pPlayer = prototype.m_pPlayer;
	m_detectionRange = prototype.m_detectionRange;
	m_attackRange = prototype.m_attackRange;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = prototype.m_attackCooldownDuration;

	m_animation = prototype.m_animation;
	m_animDataMap = prototype.m_animDataMap;
	m_hitboxSize = prototype.m_hitboxSize;
	m_hitboxOffset = prototype.m_hitboxOffset;
	m_isHurt = false;

	m_velocityX = prototype.m_velocityX;
	m_velocityY = prototype.m_velocityY;
	m_gravity = prototype.m_gravity;
	m_isGrounded = prototype.m_isGrounded;
	killScore = prototype.killScore;

	m_sfxAttack = prototype.m_sfxAttack;
	m_sfxHurt = prototype.m_sfxHurt;
	m_sfxDeath = prototype.m_sfxDeath;
	m_attackSoundFrame = prototype.m_attackSoundFrame;
	m_visualPivotOffset = prototype.m_visualPivotOffset;

	m_meleeHitboxes.clear();
	m_meleeHitboxes.resize(13, { 0, 0 });
	m_meleeHitboxes[6] = { { 0.1f, 0.f }, { m_size.x * 1.0f, m_size.y * 0.45f } };
	m_meleeHitboxes[7] = { { 0.1f, 0.f }, { m_size.x * 1.0f, m_size.y * 0.2f } };
	m_meleeHitboxes[8] = { { 0.1f, 0.f }, { m_size.x * 1.0f, m_size.y * 0.25f } };
	m_meleeHitboxes[9] = { {0.1f, 0.f }, { m_size.x * 0.6f, m_size.y * 0.2f } };
	m_meleeHitboxes[10] = { { 0.1f, 0.f }, { m_size.x * 0.6f, m_size.y * 0.2f } };
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

WarriorEnemyCharacter* WarriorEnemyCharacter::Clone()
{
	return new WarriorEnemyCharacter(*this);
}
