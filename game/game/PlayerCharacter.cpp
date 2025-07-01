#include "PlayerCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include "AssetManager.h"
#include <iostream>
#include "TileMap.h"

PlayerCharacter::PlayerCharacter()
{
	m_position = { 0, };
	m_size = { 200.f, 200.f };
	m_hitboxSize = { m_size.x * 0.4f, m_size.y * 0.6f };
	m_hitboxOffset = { 0.0f, -40.0f };
	m_healthPoint = 100;
	m_characterSpeed = 300.f;
	m_airAcceleration = 1200.f;
	m_currentDirection = CharacterDirection::RIGHT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_jumpStrength = 800.0f;
	m_isGrounded = false;
	m_groundLevel = -kHalfWindowHeight + 100.0f;

	m_isMeleeAttacking = false;
	m_isMeleeAttackHitboxActive = false;
	m_hasHitEnemyThisAttack = false;

	m_isProjectileAttacking = false;
	m_hasFiredProjectile = false;

	m_isDashing = false;
	m_dashSpeed = 800.f;

	m_currentWeapon = DamageType::FIRE;
	m_currentWeaponIndex = 0;

	m_isInvincible = false;
	m_invincibilityTimer = 0.0f;
	m_isHurt = false;
}

PlayerCharacter::~PlayerCharacter()
{
}

void PlayerCharacter::Init(AEVec2 position)
{
	m_position = position;
	m_animation.Init();

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Idle/Battlemage Idle.png", nullptr, 8, SpriteSheetOrientation::VERTICAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Running/Battlemage Run.png", nullptr, 10, SpriteSheetOrientation::VERTICAL, 0.08f, true };
	m_animDataMap[CharacterAnimationState::JUMP] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Jump Neutral/Battlemage Jump Neutral.png", nullptr, 12, SpriteSheetOrientation::VERTICAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::MELEE_ATTACK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Attack 1/Battlemage Attack 1.png", nullptr, 8, SpriteSheetOrientation::VERTICAL, 0.08f, false };
	m_animDataMap[CharacterAnimationState::RANGED_ATTACK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Sustain Magic/Battlemage Sustain Magic.png", nullptr, 11, SpriteSheetOrientation::VERTICAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::DASH] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Dash/Battlemage Dash.png", nullptr, 7, SpriteSheetOrientation::VERTICAL, 0.07f, false };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Death/Battlemage Death.png", nullptr, 12, SpriteSheetOrientation::VERTICAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::HURT] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Hurt/hurt.png", nullptr, 2, SpriteSheetOrientation::HORIZONTAL, 0.2f, false };
	
	ProjectileData fireData;
	fireData.speed = 1000.0f;
	fireData.damage = 10;
	fireData.size = { 200.f, 200.f };
	fireData.animData = { "Assets/MagicArrow/fire.png", nullptr, 15, SpriteSheetOrientation::HORIZONTAL, 0.05f, true };
	fireData.type = { DamageType::FIRE };
	m_projectileDataMap[fireData.type] = fireData;

	ProjectileData iceData;
	iceData.speed = 1000.0f;
	iceData.damage = 100;
	iceData.size = { 200.f, 200.f };
	iceData.animData = { "Assets/MagicArrow/ice.png", nullptr, 15, SpriteSheetOrientation::HORIZONTAL, 0.05f, true };
	iceData.type = { DamageType::ICE };
	m_projectileDataMap[iceData.type] = iceData;


	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath.c_str());
	}
	for (auto& pair : m_projectileDataMap)
	{
		pair.second.animData.pTexture = LoadImageAsset(pair.second.animData.texturePath.c_str());
	}

	m_availableWeapons.push_back(DamageType::FIRE);
	m_availableWeapons.push_back(DamageType::ICE);
	m_availableWeapons.push_back(DamageType::LIGHTNING);

	m_attackHitboxes.resize(8);
	m_attackHitboxes[3] = { { m_size.x * 0.25f, m_size.y * -0.15f },  { m_size.x * 0.40f, m_size.y * 0.70f } };
	m_attackHitboxes[4] = { { m_size.x * 0.28f, m_size.y * -0.3f },   { m_size.x * 0.40f, m_size.y * 0.20f } };
	m_attackHitboxes[5] = { { m_size.x * 0.28f, m_size.y * -0.4f },  { m_size.x * 0.40f, m_size.y * 0.10f } };
	m_attackHitboxes[6] = { { m_size.x * 0.28f, m_size.y * -0.4f },  { m_size.x * 0.40f, m_size.y * 0.10f } };

	m_weaponUseCounts[DamageType::FIRE] = 10;
	m_weaponUseCounts[DamageType::ICE] = 10;
	m_weaponUseCounts[DamageType::LIGHTNING] = 5;

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}

void PlayerCharacter::Update(f32 dt)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		m_animation.Update(dt);
		return; 
	}

	if (m_isInvincible)
	{
		m_invincibilityTimer += dt;
		if (m_invincibilityTimer >= m_invincibilityDuration)
		{
			m_isInvincible = false;
			m_invincibilityTimer = 0.0f;
		}
	}

	if (m_isHurt && m_animation.IsFinished())
	{
		m_isHurt = false;
	}

	bool isAttacking = m_isMeleeAttacking || m_isProjectileAttacking;

	if (AEInputCheckTriggered(AEVK_1))
	{
		m_currentWeaponIndex = 0;
		m_currentWeapon = m_availableWeapons[m_currentWeaponIndex];
	}
	if (AEInputCheckTriggered(AEVK_2))
	{
		if (m_availableWeapons.size() > 1)
		{
			m_currentWeaponIndex = 1;
			m_currentWeapon = m_availableWeapons[m_currentWeaponIndex];
		}
	}
	if (AEInputCheckTriggered(AEVK_3))
	{
		if (m_availableWeapons.size() > 2)
		{
			m_currentWeaponIndex = 2;
			m_currentWeapon = m_availableWeapons[m_currentWeaponIndex];
		}
	}
	if (AEInputCheckCurr(AEVK_A) && !isAttacking)
	{
		m_isMeleeAttacking = true;
		m_hasHitEnemyThisAttack = false;
	}
	if (AEInputCheckCurr(AEVK_S) && !isAttacking && (m_weaponUseCounts.at(m_currentWeapon) > 0))
	{
		m_isProjectileAttacking = true;
		m_hasFiredProjectile = false;
	}
	if (AEInputCheckTriggered(AEVK_LSHIFT) && !m_isDashing && !isAttacking)
	{
		m_isDashing = true;

		m_isInvincible = true;
		m_invincibilityTimer = 0.0f;
	}
	if (AEInputCheckTriggered(AEVK_SPACE) && m_isGrounded && !isAttacking && !m_isDashing)
	{
		m_isGrounded = false;
		m_velocityY = m_jumpStrength;
	}

	if (m_animation.IsFinished())
	{
		if (m_isMeleeAttacking) m_isMeleeAttacking = false;
		if (m_isProjectileAttacking) m_isProjectileAttacking = false;
		if (m_isDashing) m_isDashing = false;
	}

	if (m_isDashing && isAttacking)
	{
		m_isDashing = false;
	}

	m_isMeleeAttackHitboxActive = m_isMeleeAttacking && (m_animation.GetCurrentFrame() >= 3);

	if (m_isDashing)
	{
		if (isAttacking)
		{
			m_velocityX = 0.0f;
		}
		else
		{
			m_velocityX = (m_currentDirection == CharacterDirection::RIGHT) ? m_dashSpeed : -m_dashSpeed;
		}
	}
	else if (m_isGrounded && isAttacking)
	{
		m_velocityX = 0.0f;
	}
	else
	{
		float currentMaxSpeed = m_characterSpeed;
		if (m_isGrounded)
		{
			if (AEInputCheckCurr(AEVK_LEFT))
				m_velocityX = -currentMaxSpeed;
			else if (AEInputCheckCurr(AEVK_RIGHT))
				m_velocityX = currentMaxSpeed;
			else
				m_velocityX = 0.0f;
		}
		else
		{
			if (AEInputCheckCurr(AEVK_LEFT))
				m_velocityX -= m_airAcceleration * dt;
			else if (AEInputCheckCurr(AEVK_RIGHT))
				m_velocityX += m_airAcceleration * dt;
		}

		if (m_velocityX > currentMaxSpeed)
			m_velocityX = currentMaxSpeed;
		if (m_velocityX < -currentMaxSpeed)
			m_velocityX = -currentMaxSpeed;
	}

	if (!m_isMeleeAttacking && !m_isProjectileAttacking)
	{
		if (AEInputCheckCurr(AEVK_LEFT))
			m_currentDirection = CharacterDirection::LEFT;
		else if (AEInputCheckCurr(AEVK_RIGHT))
			m_currentDirection = CharacterDirection::RIGHT;
	}

	if (!m_isGrounded)
		m_velocityY += m_gravity * dt;

	if (m_position.y <= m_groundLevel && m_velocityY <= 0.0f)
	{
		m_position.y = m_groundLevel;
		m_velocityY = 0.0f;
		if (!m_isGrounded)
		{
			m_isGrounded = true;
			if (!AEInputCheckCurr(AEVK_LEFT) && !AEInputCheckCurr(AEVK_RIGHT))
			{
				m_velocityX = 0.0f;
			}
		}
	}

	AEVec2 tempPosition{ m_position.x + m_velocityX * dt, m_position.y + m_velocityY * dt };

	while (m_velocityY >= 0 ? m_position.y < tempPosition.y : m_position.y > tempPosition.y)
	{
		if (checkCollisionTileMap(m_position, m_size)) break;
		m_position.y += std::copysign(1.0f, m_velocityY);
		//std::cout << "Call Y" << std::endl;
	}

	if (checkCollisionTileMap(m_position, m_size))
	{
		m_position.y -= std::copysign(1.0f, m_velocityY);
		if(m_velocityY < 0) m_isGrounded = true;
	}

	while (m_velocityX >= 0 ? m_position.x < tempPosition.x : m_position.x > tempPosition.x)
	{
		if (checkCollisionTileMap(m_position, m_size)) break;
		m_position.x += std::copysign(1.0f, m_velocityX);
		//std::cout << "Call X" << std::endl;
	}

	if (checkCollisionTileMap(m_position, m_size))
	{
		m_position.x -= std::copysign(1.0f, m_velocityX);
	}

	CharacterAnimationState desiredState;
	if (m_isMeleeAttacking)
		desiredState = CharacterAnimationState::MELEE_ATTACK;
	else if (m_isProjectileAttacking)
		desiredState = CharacterAnimationState::RANGED_ATTACK;
	else if (m_isDashing)
		desiredState = CharacterAnimationState::DASH;
	else if (m_isHurt)
		desiredState = CharacterAnimationState::HURT;
	else if (!m_isGrounded)
		desiredState = CharacterAnimationState::JUMP;
	else
	{
		if (m_velocityX == 0.0f)
			desiredState = CharacterAnimationState::IDLE;
		else
			desiredState = CharacterAnimationState::WALK;
	}

	m_animation.Play(desiredState, m_animDataMap.at(desiredState));
	m_currentAnimState = desiredState;

	const f32 halfCharWidth = m_size.x / 2.0f;
	const f32 halfCharHeight = m_size.y / 2.0f;
	if (m_position.x < -kHalfWindowWidth + halfCharWidth)
		m_position.x = -kHalfWindowWidth + halfCharWidth;

	if (m_position.y > kHalfWindowHeight - halfCharHeight)
	{
		m_position.y = kHalfWindowHeight - halfCharHeight;
		m_velocityY = 0;
	}

	m_animation.Update(dt);
}

void PlayerCharacter::Move(f32 dt)
{
}

void PlayerCharacter::Draw()
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

	if (m_isMeleeAttackHitboxActive)
	{
		const AttackHitbox& currentHitbox = GetCurrentMeleeHitbox();
		AEVec2 hitboxPos;
		hitboxPos.x = m_position.x + (m_currentDirection == CharacterDirection::RIGHT ? currentHitbox.offset.x : -currentHitbox.offset.x);
		hitboxPos.y = m_position.y + currentHitbox.offset.y;
		DrawHollowRect(hitboxPos.x, hitboxPos.y, currentHitbox.size.x, currentHitbox.size.y, 1.0f, 0.0f, 0.0f, 0.5f);
	}

	DrawHollowRect(m_position.x + m_hitboxOffset.x, m_position.y + m_hitboxOffset.y, m_hitboxSize.x, m_hitboxSize.y, 0.0f, 0.8f, 1.0f, 0.5f);
}

void PlayerCharacter::Destroy()
{
	m_animDataMap.clear();
	m_animation.Destroy();
}

s32 PlayerCharacter::GetCurrentAnimationFrame() const
{
	return m_animation.GetCurrentFrame();
}

const AttackHitbox& PlayerCharacter::GetCurrentMeleeHitbox() const
{
	s32 currentFrame = GetCurrentAnimationFrame();
	if (currentFrame >= 0 && currentFrame < m_attackHitboxes.size())
	{
		return m_attackHitboxes[currentFrame];
	}
	return m_attackHitboxes[0];
}

void PlayerCharacter::Attack()
{
}

void PlayerCharacter::TakeDamage(s32 damage, DamageType damageType)
{
	if (m_isInvincible || m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;
	std::cout << "Player takes damage! HP: " << m_healthPoint << std::endl;

	m_isInvincible = true;
	m_invincibilityTimer = 0.0f;
	m_isHurt = true;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}

bool PlayerCharacter::IsCompletelyDead() const
{
	return (m_currentAnimState == CharacterAnimationState::DEATH && m_animation.IsFinished());
}

const ProjectileData& PlayerCharacter::GetCurrentProjectileData() const
{
	return m_projectileDataMap.at(m_currentWeapon);
}

bool PlayerCharacter::IsInvincible() const
{
	return m_isInvincible;
}

PlayerCharacter* PlayerCharacter::Clone()
{
	return nullptr;
}

int PlayerCharacter::GetWeaponUseCount(DamageType type) const
{
	if (m_weaponUseCounts.count(type))
	{
		return m_weaponUseCounts.at(type);
	}
	return 0;
}

void PlayerCharacter::ConsumeCurrentWeapon()
{
	if (m_weaponUseCounts.count(m_currentWeapon))
	{
		if (m_weaponUseCounts[m_currentWeapon] > 0)
		{
			m_weaponUseCounts[m_currentWeapon]--;
		}
	}
}

