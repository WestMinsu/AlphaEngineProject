#include "MeleeEnemyCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>
#include "PlayerCharacter.h"
#include "AssetManager.h"
#include "TileMap.h"
#include "GameManager.h"

MeleeEnemyCharacter::MeleeEnemyCharacter()
{
	m_size = { 200.f, 200.f };
	m_healthPoint = 50;
	m_characterSpeed = 100.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_currentAIState = EnemyAIState::IDLE;
	m_pPlayer = nullptr;
	m_detectionRange = 800.0f;
	m_attackRange = 100.0f;
	m_attackCooldownTimer = 0.0f;
	m_attackCooldownDuration = 2.0f;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_isGrounded = false;
	killScore = 1000;
	m_isDamageEffectActive = false;
	m_isHurt = false;
	m_damageEffectTimer = 0.0f;
	m_damageEffectDuration = 0.3f;
	m_damageEffectTimer = 0.0f;

	m_strafeTimer = 0.0f;
	m_strafeDuration = 0.0f;
	m_strafeDirection = 1.0f;

	m_attackSoundFrame = 5;
	m_hasPlayedAttackSound = false;
	m_visualPivotOffset = 0.0f;

	m_hasPlayedAttackSound = false;
	m_hasHitPlayerThisAttack = false;
	m_isMeleeAttackHitboxActive = false;
}

MeleeEnemyCharacter::~MeleeEnemyCharacter() 
{
}

void MeleeEnemyCharacter::Init(AEVec2 position)
{
	ACharacter::Init(position);
}

void MeleeEnemyCharacter::Init(AEVec2 position, PlayerCharacter* player)
{
	m_position = position;
	m_pPlayer = player; 
	m_animation.Init();
}

void MeleeEnemyCharacter::Update(f32 dt)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		m_animation.Update(dt);
		return;
	}

	if (!m_pPlayer) 
		return;

	if (m_isDamageEffectActive)
	{
		m_damageEffectTimer += dt;
		if (m_damageEffectTimer >= m_damageEffectDuration)
		{
			m_isDamageEffectActive = false;
			m_damageEffectTimer = 0.0f;
		}
	}

	if (m_isHurt && m_animation.IsFinished())
	{
		m_isHurt = false;
	}

	m_prevPosition = m_position;

	AEVec2 playerPos = m_pPlayer->GetPosition();
	float distanceToPlayer = AEVec2Distance(&m_position, &playerPos);
	float xDistanceToPlayer = std::abs(m_position.x - playerPos.x);

	AEVec2 groundCheckPos = m_position;
	groundCheckPos.y -= 1.0f;

	if (!checkCollisionTileMap(groundCheckPos, m_size))
	{
		m_isGrounded = false;
	}

	if (!m_isGrounded)
	{
		m_velocityY += m_gravity * dt;
	}

	switch (m_currentAIState)
	{
	case EnemyAIState::IDLE:
	{
		if (distanceToPlayer < m_detectionRange)
			m_currentAIState = EnemyAIState::CHASE;
		break;
	}
	case EnemyAIState::CHASE:
		if (distanceToPlayer < m_attackRange)
		{
			m_currentAIState = EnemyAIState::ATTACK;
			m_hasPlayedAttackSound = false; 
			m_hasHitPlayerThisAttack = false;
		}
		else if (xDistanceToPlayer < m_attackRange + 100.f) 
		{
			m_currentAIState = EnemyAIState::STRAFING;
			m_strafeDuration = 0.5f + static_cast<float>(rand() % 10) / 10.0f;
			m_strafeTimer = 0.0f;
			m_strafeDirection = (rand() % 2 == 0) ? 1.0f : -1.0f;
		}
		else if (distanceToPlayer > m_detectionRange)
			m_currentAIState = EnemyAIState::IDLE;
		break;
	case EnemyAIState::STRAFING:
		m_strafeTimer += dt;
		if (m_strafeTimer >= m_strafeDuration)
		{
			m_currentAIState = EnemyAIState::CHASE;
		}
		break;
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
			m_currentAIState = EnemyAIState::IDLE;
		break;
	}
	}

	m_isMeleeAttackHitboxActive = (m_currentAIState == EnemyAIState::ATTACK) && (m_animation.GetCurrentFrame() >= 5 && m_animation.GetCurrentFrame() <= 7);

	if (m_currentAIState == EnemyAIState::ATTACK && !m_hasPlayedAttackSound && m_animation.GetCurrentFrame() >= m_attackSoundFrame)
	{
		GameManager::PlaySFX(*m_sfxAttack);
		m_hasPlayedAttackSound = true;
	}

	f32 m_velocityX = 0.0f;
	CharacterAnimationState desiredAnimState = CharacterAnimationState::IDLE;

	if (m_currentAIState == EnemyAIState::CHASE)
	{
		float xOffset = playerPos.x - m_position.x;
		float directionChangeThreshold = 5.0f;

		if (xOffset < -directionChangeThreshold)
		{
			m_currentDirection = CharacterDirection::LEFT;
			m_velocityX = -m_characterSpeed;
		}
		else if (xOffset > directionChangeThreshold)
		{
			m_currentDirection = CharacterDirection::RIGHT;
			m_velocityX = m_characterSpeed;
		}
		desiredAnimState = CharacterAnimationState::WALK;
	}
	else if (m_currentAIState == EnemyAIState::STRAFING)
	{
		m_velocityX = m_characterSpeed * m_strafeDirection;
		desiredAnimState = CharacterAnimationState::WALK;
	}
	else if (m_currentAIState == EnemyAIState::ATTACK)
	{
		desiredAnimState = CharacterAnimationState::MELEE_ATTACK;
	}
	else if (m_isHurt) 
	{
		desiredAnimState = CharacterAnimationState::HURT;
	}

	//m_position.x += m_velocityX * dt;

	AEVec2 hitboxPosition{ m_position.x + m_hitboxOffset.x, m_position.y + m_hitboxOffset.y };
	AEVec2 tempPosition{ hitboxPosition.x + m_velocityX * dt, hitboxPosition.y + m_velocityY * dt };

	while (m_velocityY >= 0 ? hitboxPosition.y < tempPosition.y : hitboxPosition.y > tempPosition.y)
	{
		//if (checkCollisionTileMap(m_position, m_size)) break;
		if (checkCollisionTileMap(hitboxPosition, m_hitboxSize)) break;

		hitboxPosition.y += std::copysign(1.0f, m_velocityY);
		//std::cout << "Call Y" << std::endl;
	}

	if (checkCollisionTileMap(hitboxPosition, m_hitboxSize))
	{
		hitboxPosition.y -= std::copysign(1.0f, m_velocityY);
		if (m_velocityY < 0) m_isGrounded = true;
	}

	while (m_velocityX >= 0 ? hitboxPosition.x < tempPosition.x : hitboxPosition.x > tempPosition.x)
	{
		if (checkCollisionTileMap(hitboxPosition, m_hitboxSize)) break;
		hitboxPosition.x += std::copysign(1.0f, m_velocityX);
		//std::cout << "Call X" << std::endl;
	}

	if (checkCollisionTileMap(hitboxPosition, m_hitboxSize))
	{
		hitboxPosition.x -= std::copysign(1.0f, m_velocityX);
	}

	m_position = { hitboxPosition.x - m_hitboxOffset.x, hitboxPosition.y - m_hitboxOffset.y };

	float movedDistance = std::abs(m_position.x - m_prevPosition.x);
	if (m_isGrounded && m_currentAIState == EnemyAIState::CHASE && m_velocityX != 0.f && movedDistance < 0.1f)
	{
		m_stuckTimer += dt;
		if (m_stuckTimer > 0.01f)
		{
			m_velocityY = m_jumpStrength;
			m_isGrounded = false;
			m_stuckTimer = 0.0f;
		}
	}
	else
	{
		m_stuckTimer = 0.0f; 
	}

	if (m_currentAnimState != desiredAnimState)
	{
		m_currentAnimState = desiredAnimState;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
	m_animation.Update(dt);
}

void MeleeEnemyCharacter::Move(f32 dt)
{

}

void MeleeEnemyCharacter::Attack()
{
	
}


void MeleeEnemyCharacter::Draw()
{
	AEMtx33 scale = { 0 };

	f32 finalDrawPosX = m_position.x;

	if (m_currentDirection == CharacterDirection::LEFT)
		AEMtx33Scale(&scale, -m_size.x, m_size.y);
	else
	{
		AEMtx33Scale(&scale, m_size.x, m_size.y);
		finalDrawPosX += m_visualPivotOffset;
	}

	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, 0);

	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, finalDrawPosX, m_position.y);

	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	if (m_isDamageEffectActive && m_isHurt && m_animation.GetCurrentState() != CharacterAnimationState::DEATH)
		m_animation.Draw(transform, 1.0f, 0.0f, 0.0f, 0.7f);
	else
		m_animation.Draw(transform);

	if (IsAttackHitboxActive())
	{
		const AttackHitbox& currentHitbox = GetCurrentMeleeHitbox();
		AEVec2 hitboxPos;
		hitboxPos.x = m_position.x + (m_currentDirection == CharacterDirection::RIGHT ? currentHitbox.offset.x : -currentHitbox.offset.x);
		hitboxPos.y = m_position.y + currentHitbox.offset.y;
		//DrawHollowRect(hitboxPos.x, hitboxPos.y, currentHitbox.size.x, currentHitbox.size.y, 1.0f, 0.0f, 0.0f, 0.5f);
	}
	//DrawHollowRect(m_position.x + m_hitboxOffset.x, m_position.y + m_hitboxOffset.y, m_hitboxSize.x, m_hitboxSize.y, 1.0f, 0.0f, 0.0f, 1.f);
}

void MeleeEnemyCharacter::Destroy()
{
	m_animation.Destroy();
}

void MeleeEnemyCharacter::TakeDamage(s32 damage, DamageType damageType)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;
	//std::cout << "Melee Enemy takes damage! HP: " << m_healthPoint << std::endl;
	m_isDamageEffectActive = true;
	m_damageEffectTimer = 0.0f;
	m_isHurt = true;
	GameManager::PlaySFX(*m_sfxHurt);

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
		GameManager::PlaySFX(*m_sfxDeath);
	}
}

bool MeleeEnemyCharacter::IsAttackHitboxActive() const
{
	if (m_currentAnimState == CharacterAnimationState::MELEE_ATTACK)
		return true;
	return false;
}

const AttackHitbox& MeleeEnemyCharacter::GetCurrentMeleeHitbox() const
{
	s32 currentFrame = m_animation.GetCurrentFrame();
	if (currentFrame >= 0 && currentFrame < m_meleeHitboxes.size())
	{
		return m_meleeHitboxes[currentFrame];
	}
	return m_meleeHitboxes[0];
}