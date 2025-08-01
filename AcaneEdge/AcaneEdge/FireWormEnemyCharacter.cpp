#include "FireWormEnemyCharacter.h"
#include "PlayerCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "AssetManager.h"

FireWormEnemyCharacter::FireWormEnemyCharacter()
{
	m_size = { 200.f, 200.f };
	m_hitboxSize = { m_size.x * 0.6f, m_size.y * 0.5f };
	m_hitboxOffset = { 0.0f, -m_size.y * 0.25f };
	m_healthPoint = 40;
	m_characterSpeed = 80.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;
	m_element = ElementType::FIRE;

	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 1600.0f;
	m_attackRange = 300.0f;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 3.0f;
	m_hasFiredProjectile = false;
	m_isHurt = false;
	m_isDamageEffectActive = false;
	m_damageEffectTimer = 0.0f;
	m_damageEffectDuration = 0.2f;
	m_projectileSpawnOffset = { 0.f, -25.f };
	killScore = 2000;

	m_strafeTimer = 0.0f;
	m_strafeDuration = 0.0f;
	m_strafeDirection = 1.0f;

	m_sfxAttack = LoadSoundAsset("Assets/Sounds/attack_monster.wav");
	m_sfxHurt = LoadSoundAsset("Assets/Sounds/hurt2_monster.wav");
	m_sfxDeath = LoadSoundAsset("Assets/Sounds/die_monster.wav");
	m_attackSoundFrame = 13;
}

FireWormEnemyCharacter::FireWormEnemyCharacter(const FireWormEnemyCharacter& prototype)
{
	m_size = prototype.m_size;
	m_hitboxSize = prototype.m_hitboxSize;
	m_hitboxOffset = prototype.m_hitboxOffset;
	m_healthPoint = prototype.m_healthPoint;
	m_characterSpeed = prototype.m_characterSpeed * (0.7f + static_cast<float>(rand() % 61) / 100.0f);
	m_currentDirection = prototype.m_currentDirection;
	m_currentAnimState = prototype.m_currentAnimState;
	m_element = prototype.m_element;

	m_currentAIState = prototype.m_currentAIState;
	m_pPlayer = prototype.m_pPlayer;
	m_detectionRange = prototype.m_detectionRange;
	m_attackRange = prototype.m_attackRange;

	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = prototype.m_attackCooldownDuration;
	m_hasFiredProjectile = false;

	m_animation = prototype.m_animation;
	m_animDataMap = prototype.m_animDataMap;
	m_isHurt = false;

	m_projectileData = prototype.m_projectileData;
	m_projectileSpawnOffset = prototype.m_projectileSpawnOffset;
	killScore = prototype.killScore;

	m_sfxAttack = prototype.m_sfxAttack;
	m_sfxHurt = prototype.m_sfxHurt;
	m_sfxDeath = prototype.m_sfxDeath;
	m_attackSoundFrame = prototype.m_attackSoundFrame;
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
	m_projectileData.damage = 15;
	m_projectileData.size = { 150.f, 50.f };
	m_projectileData.animData = { "Assets/Fire Worm/Sprites/Fire Ball/Move.png", nullptr, 6, SpriteSheetOrientation::HORIZONTAL, 0.1f, true };
	m_projectileData.animData.pTexture = LoadImageAsset(m_projectileData.animData.texturePath.c_str());

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}

FireWormEnemyCharacter* FireWormEnemyCharacter::Clone()
{
	return new FireWormEnemyCharacter(*this);
}

bool FireWormEnemyCharacter::isReadytoFireRange()
{
	return m_currentAnimState == CharacterAnimationState::RANGED_ATTACK
		&& (m_animation.GetCurrentFrame() == m_attackSoundFrame && !m_hasFiredProjectile);
}
