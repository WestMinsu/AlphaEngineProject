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
	m_hitboxOffset = { m_size.x * 0.05f, -m_size.y * 0.3f };
	m_characterSpeed = 100.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_element = ElementType::DARK;

	m_currentAnimState = CharacterAnimationState::IDLE;
	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 500.0f;
	m_attackRange = 150.0f;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 2.0f;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_isGrounded = false;
	killScore = 2500;

	m_strafeTimer = 0.0f;
	m_strafeDuration = 0.0f;
	m_strafeDirection = 1.0f;

	m_sfxAttack = LoadSoundAsset("Assets/Sounds/attack_monster.wav");
	m_sfxHurt = LoadSoundAsset("Assets/Sounds/hurt_monster.wav");
	m_sfxDeath = LoadSoundAsset("Assets/Sounds/hurt_monster.wav");
	m_attackSoundFrame = 10;
	m_visualPivotOffset = 25.0f;

	m_meleeHitboxes.clear();
	m_meleeHitboxes.resize(12, { 0, 0 });
	m_meleeHitboxes[6] = { {m_size.x * 0.5f, 0.f}, {m_size.x * 0.7f, m_size.y * 0.3f} };
	m_meleeHitboxes[7] = { {m_size.x * 0.5f, 0.f}, {m_size.x * 0.7f, m_size.y * 0.3f} };
}

NightBorneEnemyCharacter::NightBorneEnemyCharacter(const NightBorneEnemyCharacter& prototype)
{
	m_size = prototype.m_size;
	m_healthPoint = prototype.m_healthPoint;
	m_hitboxSize = prototype.m_hitboxSize;
	m_hitboxOffset = prototype.m_hitboxOffset;
	m_characterSpeed = prototype.m_characterSpeed * (0.8f + static_cast<float>(rand() % 41) / 100.0f);
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

NightBorneEnemyCharacter* NightBorneEnemyCharacter::Clone()
{
	return new NightBorneEnemyCharacter(*this);
}
