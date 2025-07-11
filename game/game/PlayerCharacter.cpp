#include "PlayerCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include "AssetManager.h"
#include <iostream>
#include "TileMap.h"
#include "GameManager.h"

PlayerCharacter::PlayerCharacter()
{
	m_position = { 0, };
	m_size = { 200.f, 200.f };
	m_hitboxSize = { m_size.x * 0.25f, m_size.y * 0.6f };
	m_hitboxOffset = { 0.0f, -40.0f };
	m_crouchingHitboxSize = { m_size.x * 0.25f, m_size.y * 0.3f };
	m_crouchingHitboxOffset = { 0.0f, -40.0f };
	m_healthPoint = 100;
	m_characterSpeed = 300.f;
	m_airAcceleration = 1200.f;
	m_currentDirection = CharacterDirection::RIGHT;
	m_element = ElementType::NONE;

	m_currentAnimState = CharacterAnimationState::IDLE;
	m_maxHealth = 100;
	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_jumpStrength = 800.0f;
	m_isGrounded = false;

	m_isMeleeAttacking = false;
	m_isMeleeAttackHitboxActive = false;
	m_hasHitEnemyThisAttack = false;

	m_isSkillAttacking = false;
	m_hasFiredProjectile = false;

	m_isDashing = false;
	m_dashSpeed = 800.f;

	m_currentWeapon = DamageType::FIRE;
	m_currentWeaponIndex = 0;

	m_isHurt = false;
	m_isDamageEffectActive = false;
	m_damageEffectTimer = 0.0f;
	m_damageEffectDuration = 1.5f;
	m_score = 0;
	m_hasPlayedAttackSound = false;
}

PlayerCharacter::~PlayerCharacter()
{
}

void PlayerCharacter::Init(AEVec2 position)
{
	m_position = position;
	m_animation.Init();

	m_sfxMeleeAttack = LoadSoundAsset("Assets/Sounds/attack_knight.wav");
	m_sfxFireAttack = LoadSoundAsset("Assets/Sounds/fire.mp3");
	m_sfxIceAttack = LoadSoundAsset("Assets/Sounds/ice.mp3");
	m_sfxLightningAttack = LoadSoundAsset("Assets/Sounds/lightning.mp3");
	m_sfxHurt = LoadSoundAsset("Assets/Sounds/hurt_player.mp3");
	m_sfxDeath = LoadSoundAsset("Assets/Sounds/death_player.mp3");

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Idle/Battlemage Idle.png", nullptr, 8, SpriteSheetOrientation::VERTICAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Running/Battlemage Run.png", nullptr, 10, SpriteSheetOrientation::VERTICAL, 0.08f, true };
	m_animDataMap[CharacterAnimationState::JUMP] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Jump Neutral/Battlemage Jump Neutral.png", nullptr, 12, SpriteSheetOrientation::VERTICAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::CROUCH] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Crouch/Battlemage Crouch.png", nullptr, 9, SpriteSheetOrientation::VERTICAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::MELEE_ATTACK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Attack 1/Battlemage Attack 1.png", nullptr, 8, SpriteSheetOrientation::VERTICAL, 0.08f, false };
	m_animDataMap[CharacterAnimationState::RANGED_ATTACK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Sustain Magic/Battlemage Sustain Magic.png", nullptr, 11, SpriteSheetOrientation::VERTICAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::DASH] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Dash/Battlemage Dash.png", nullptr, 7, SpriteSheetOrientation::VERTICAL, 0.07f, false };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Death/Battlemage Death.png", nullptr, 12, SpriteSheetOrientation::VERTICAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::HURT] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Hurt/hurt.png", nullptr, 2, SpriteSheetOrientation::HORIZONTAL, 0.2f, false };
	
	ProjectileData fireData;
	fireData.speed = 1000.0f;
	fireData.damage = 34;
	fireData.size = { 200.f, 70.f };
	fireData.animData = { "Assets/MagicArrow/fire.png", nullptr, 15, SpriteSheetOrientation::HORIZONTAL, 0.05f, true };
	fireData.type = { DamageType::FIRE };
	m_projectileDataMap[fireData.type] = fireData;

	ProjectileData iceData;
	iceData.speed = 1000.0f;
	iceData.damage = 25;
	iceData.size = { 200.f, 70.f };
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

	bool isAttacking = m_isMeleeAttacking || m_isSkillAttacking;

	if (AEInputCheckCurr(AEVK_DOWN) && m_isGrounded && !isAttacking && !m_isDashing)
	{
		m_isCrouching = true;
	}
	else
	{
		m_isCrouching = false;
	}

	bool isBusy = isAttacking || m_isCrouching;

	if (AEInputCheckTriggered(AEVK_Q))
	{
		m_currentWeaponIndex = 0;
		m_currentWeapon = m_availableWeapons[m_currentWeaponIndex];
	}
	if (AEInputCheckTriggered(AEVK_W))
	{
		if (m_availableWeapons.size() > 1)
		{
			m_currentWeaponIndex = 1;
			m_currentWeapon = m_availableWeapons[m_currentWeaponIndex];
		}
	}
	if (AEInputCheckTriggered(AEVK_E))
	{
		if (m_availableWeapons.size() > 2)
		{
			m_currentWeaponIndex = 2;
			m_currentWeapon = m_availableWeapons[m_currentWeaponIndex];
		}
	}
	if (AEInputCheckCurr(AEVK_A) && !isBusy)
	{
		m_isMeleeAttacking = true;
		m_hasHitEnemyThisAttack = false;
		m_hasPlayedAttackSound = false;
	}

	if (AEInputCheckCurr(AEVK_S) && !isBusy && (m_weaponUseCounts.at(m_currentWeapon) > 0))
	{
		m_isSkillAttacking = true;
		m_hasFiredProjectile = false;
		m_hasPlayedAttackSound = false;
	}
	if ((AEInputCheckTriggered(AEVK_LSHIFT) 
		|| AEInputCheckTriggered(AEVK_Z)
		|| AEInputCheckTriggered(AEVK_F)
		)
		&& !m_isDashing && !isAttacking)
	{
		m_isDashing = true;

		m_isDamageEffectActive = true;
		m_damageEffectTimer = 0.0f;
	}
	if ((AEInputCheckTriggered(AEVK_SPACE) 
		|| AEInputCheckTriggered(AEVK_UP)) 
		&& m_isGrounded && !isAttacking && !m_isDashing)
	{
		m_isGrounded = false;
		m_velocityY = m_jumpStrength;
	}

	if (AEInputCheckTriggered(AEVK_D))
	{
		switch (m_currentWeapon)
		{
		case DamageType::FIRE:
			BuyMagic(DamageType::FIRE);
			break;
		case DamageType::ICE:
			BuyMagic(DamageType::ICE);
			break;
		case DamageType::LIGHTNING:
			BuyMagic(DamageType::LIGHTNING);
			break;
		default:
			break;
		}
	}

	if (m_isMeleeAttacking && !m_hasPlayedAttackSound && m_animation.GetCurrentFrame() == 4)
	{
		GameManager::PlaySFX(*m_sfxMeleeAttack);
		m_hasPlayedAttackSound = true;
	}

	if (m_isSkillAttacking && !m_hasPlayedAttackSound && m_animation.GetCurrentFrame() == 5) 
	{
		switch (m_currentWeapon)
		{
		case DamageType::FIRE:
			GameManager::PlaySFX(*m_sfxFireAttack);
			break;
		case DamageType::ICE:
			GameManager::PlaySFX(*m_sfxIceAttack);
			break;
		case DamageType::LIGHTNING:
			GameManager::PlaySFX(*m_sfxLightningAttack);
			break;
		}
		m_hasPlayedAttackSound = true;
	}

	if (m_animation.IsFinished())
	{
		if (m_isMeleeAttacking) m_isMeleeAttacking = false;
		if (m_isSkillAttacking) m_isSkillAttacking = false;
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
		if (!m_isCrouching)
		{
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
		}
		else
			m_velocityX = 0.0f;

		if (m_velocityX > currentMaxSpeed)
			m_velocityX = currentMaxSpeed;
		if (m_velocityX < -currentMaxSpeed)
			m_velocityX = -currentMaxSpeed;
	}

	if (!m_isMeleeAttacking && !m_isSkillAttacking)
	{
		if (AEInputCheckCurr(AEVK_LEFT))
			m_currentDirection = CharacterDirection::LEFT;
		else if (AEInputCheckCurr(AEVK_RIGHT))
			m_currentDirection = CharacterDirection::RIGHT;
	}

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

	AEVec2 hitboxPosition{ m_position.x + m_hitboxOffset.x, m_position.y + m_hitboxOffset.y };
	AEVec2 tempPosition{ hitboxPosition.x + m_velocityX * dt, hitboxPosition.y + m_velocityY * dt };

	while (m_velocityY >= 0 ? hitboxPosition.y < tempPosition.y : hitboxPosition.y > tempPosition.y)
	{
		if (checkCollisionTileMap(hitboxPosition, m_hitboxSize)) break;
		hitboxPosition.y += std::copysign(1.0f, m_velocityY);
	}
	if ( checkCollisionTileMap(hitboxPosition, m_hitboxSize))
	{
		hitboxPosition.y -= std::copysign(1.0f, m_velocityY);
		if (m_velocityY < 0)
		{
			m_isGrounded = true;
		}
		m_velocityY = 0;
	}

	while (m_velocityX >= 0 ? hitboxPosition.x < tempPosition.x : hitboxPosition.x > tempPosition.x)
	{
		if (checkCollisionTileMap(hitboxPosition, m_hitboxSize)) break;
		hitboxPosition.x += std::copysign(1.0f, m_velocityX);
	}
	if (checkCollisionTileMap(hitboxPosition, m_hitboxSize))
	{
		hitboxPosition.x -= std::copysign(1.0f, m_velocityX);
		m_velocityX = 0;
	}

	m_position = { hitboxPosition.x - m_hitboxOffset.x, hitboxPosition.y - m_hitboxOffset.y };

	CharacterAnimationState desiredState;
	if (m_isMeleeAttacking)
		desiredState = CharacterAnimationState::MELEE_ATTACK;
	else if (m_isSkillAttacking)
		desiredState = CharacterAnimationState::RANGED_ATTACK;
	else if (m_isDashing)
		desiredState = CharacterAnimationState::DASH;
	else if (m_isHurt)
		desiredState = CharacterAnimationState::HURT;
	else if (!m_isGrounded)
		desiredState = CharacterAnimationState::JUMP;
	else if (m_isCrouching)
		desiredState = CharacterAnimationState::CROUCH;
	else
	{
		if (m_velocityX == 0.0f)
			desiredState = CharacterAnimationState::IDLE;
		else
			desiredState = CharacterAnimationState::WALK;
	}

	m_animation.Play(desiredState, m_animDataMap.at(desiredState));
	m_currentAnimState = desiredState;

	const f32 halfCharWidth = m_hitboxSize.x / 2.0f;
	const f32 halfCharHeight = m_hitboxSize.y / 2.0f;

	f32 camX, camY;
	AEGfxGetCamPosition(&camX, &camY);
	m_position.x = std::clamp(m_position.x, -kHalfWindowWidth + camX + halfCharWidth,camX + kHalfWindowWidth - halfCharWidth);
	//if (m_position.x < camX + halfCharWidth)
	//	m_position.x = -kHalfWindowWidth + halfCharWidth;

	if (m_position.x < -kHalfWindowWidth + halfCharWidth)
		m_position.x = -kHalfWindowWidth + halfCharWidth;

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

	if (m_isDamageEffectActive && m_isHurt && m_animation.GetCurrentState() != CharacterAnimationState::DEATH)
	{
		if (static_cast<int>(m_damageEffectTimer / 0.1f) % 2 == 0)
		{
			m_animation.Draw(transform, 1.0f, 1.0f, 1.0f, 0.7f);
		}
	}
	else
		m_animation.Draw(transform);

	if (m_isMeleeAttackHitboxActive)
	{
		const AttackHitbox& currentHitbox = GetCurrentMeleeHitbox();
		AEVec2 hitboxPos;
		hitboxPos.x = m_position.x + (m_currentDirection == CharacterDirection::RIGHT ? currentHitbox.offset.x : -currentHitbox.offset.x);
		hitboxPos.y = m_position.y + currentHitbox.offset.y;
		DrawHollowRect(hitboxPos.x, hitboxPos.y, currentHitbox.size.x, currentHitbox.size.y, 1.0f, 0.0f, 0.0f, 0.5f);
	}

	if (m_healthPoint > 0)
	{
		float barWidth = m_hitboxSize.x;
		float barHeight = 10.f;
		float barOffsetY = m_hitboxSize.y / 2.0f; 

		float healthRatio = static_cast<float>(m_healthPoint) / m_maxHealth;
		float currentHealthWidth = barWidth * healthRatio;

		DrawRect(m_position.x, m_position.y + barOffsetY, barWidth, barHeight, 0.2f, 0.2f, 0.2f, 1.0f);
		DrawRect(m_position.x - (barWidth - currentHealthWidth) / 2.0f, m_position.y + barOffsetY, currentHealthWidth, barHeight, 0.0f, 0.8f, 0.2f, 1.0f);
	}	

	DrawHollowRect(m_position.x + m_hitboxOffset.x, m_position.y + m_hitboxOffset.y, GetHitboxSize().x, GetHitboxSize().y, 0.0f, 0.8f, 1.0f, 0.5f);
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
	if (m_isDamageEffectActive || m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;
	std::cout << "Player takes damage! HP: " << m_healthPoint << std::endl;

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
	return m_isDamageEffectActive;
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

void PlayerCharacter::AddScore(s32 amount)
{
	m_score += amount;
}

void PlayerCharacter::BuyMagic(DamageType type)
{
	int cost = 1000;
	if (m_score >= cost)
	{
		m_score -= cost;
		if (type == DamageType::LIGHTNING)
			m_weaponUseCounts[type] += 5;
		else
			m_weaponUseCounts[type] += 10;

		std::cout << "Ammo purchased!" << std::endl;
	}
	else
	{
		std::cout << "Not enough score!" << std::endl;
	}
}

const AEVec2& PlayerCharacter::GetHitboxSize() const
{
	if (m_isCrouching)
	{
		return m_crouchingHitboxSize;
	}
	return m_hitboxSize;
}