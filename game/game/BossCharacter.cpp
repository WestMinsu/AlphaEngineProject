#include "BossCharacter.h"
#include "PlayerCharacter.h"
#include "Constants.h"
#include "Utility.h"
#include "AssetManager.h"
#include <iostream>

BossCharacter::BossCharacter()
{
	m_size = { 400.f, 400.f };
	m_maxHealth = 500.f;
	m_healthPoint = static_cast<s32>(m_maxHealth);
	m_characterSpeed = 0.f;
	m_currentDirection = CharacterDirection::LEFT;
	m_currentAnimState = CharacterAnimationState::IDLE;
	m_currentAIState = BossAIState::APPEARING;

	m_pPlayer = nullptr;
	m_isAttackable = false;
	m_hasGlowed = false;
	m_hasBuffed = false;
	m_isInBuffState = false;
	m_aiTimer = 0.0f;
	m_meleeAttackRange = 300.f;
	m_rangedAttackRange = 800.f;
	m_cooldownDuration = 1.5f;
	m_hasFired = false;
}

BossCharacter::~BossCharacter() {}

void BossCharacter::Init(AEVec2 position)
{
}

void BossCharacter::Init(AEVec2 position, PlayerCharacter* player)
{
	m_position = position;
	m_pPlayer = player;
	m_animation.Init();
	m_laserAnimation.Init();

	m_animDataMap[CharacterAnimationState::APPEARANCE] = { "Assets/Boss/appearance.png", nullptr, 14, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Boss/idle.png", nullptr, 4, SpriteSheetOrientation::HORIZONTAL, 0.15f, true };
	m_animDataMap[CharacterAnimationState::GLOWING] = { "Assets/Boss/glowing.png", nullptr, 8, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::MELEE_ATTACK] = { "Assets/Boss/meleeattack.png", nullptr, 7, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::RANGED_ATTACK] = { "Assets/Boss/rangedattack.png", nullptr, 9, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::LASER_CAST] = { "Assets/Boss/lasercast.png", nullptr, 7, SpriteSheetOrientation::HORIZONTAL, 0.12f, false };
	m_animDataMap[CharacterAnimationState::LASER_SHEET] = { "Assets/Boss/laser_sheet.png", nullptr, 14, SpriteSheetOrientation::VERTICAL, 0.08f, false };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Boss/death.png", nullptr, 14, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::BUFF] = { "Assets/Boss/buff.png", nullptr, 10, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };

	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath.c_str());
	}

	m_projectileData.speed = 1200.0f;
	m_projectileData.damage = 10;
	m_projectileData.size = { 100.f, 50.f };
	m_projectileData.animData = { "Assets/Boss/arm_projectile.png", nullptr, 1, SpriteSheetOrientation::HORIZONTAL, 1.0f, true };
	m_projectileData.animData.pTexture = LoadImageAsset(m_projectileData.animData.texturePath.c_str());

	m_meleeHitboxes.resize(7);
	m_meleeHitboxes[3] = { { m_size.x * 0.4f, m_size.y * 0.1f}, { m_size.x * 0.6f, m_size.y * 0.3f } };
	m_meleeHitboxes[4] = { { m_size.x * 0.4f, m_size.y * 0.0f}, { m_size.x * 0.6f, m_size.y * 0.3f } };

	float laserOffsetX = m_size.x;
	float laserOffsetY = m_size.y * 0.01f;
	float laserSizeX = m_size.x * 3.0f;
	float laserSizeY = m_size.y;
	m_laserHitbox = { { laserOffsetX, laserOffsetY }, { laserSizeX, laserSizeY } };

	m_currentAnimState = CharacterAnimationState::APPEARANCE;
	m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
}

void BossCharacter::Update(f32 dt)
{
	if (m_currentAIState == BossAIState::DEATH)
	{
		m_animation.Update(dt);
		return;
	}
	if (!m_pPlayer) 
		return;

	AEVec2 playerPos = m_pPlayer->GetPosition();

	if (playerPos.x < m_position.x)
		m_currentDirection = CharacterDirection::LEFT;
	else
		m_currentDirection = CharacterDirection::RIGHT;

	float distanceToPlayer = AEVec2Distance(&m_position, &playerPos);

	switch (m_currentAIState)
	{
	case BossAIState::APPEARING:
		if (m_animation.IsFinished())
		{
			m_currentAIState = BossAIState::APPEARANCE_WAIT;
			m_aiTimer = 0.0f;
		}
		break;
	case BossAIState::APPEARANCE_WAIT:
	{
		m_aiTimer += dt;
		if (m_aiTimer >= 1.0f)
		{
			m_isAttackable = true;
			m_currentAIState = BossAIState::IDLE;
		}
		break;
	}
	case BossAIState::IDLE:
		if ((rand() % 3) == 0) 
		{
			m_currentAIState = BossAIState::LASER_CAST;
			m_hasFired = false;
		}
		else if (distanceToPlayer < m_meleeAttackRange && !m_isInBuffState)
		{
			m_currentAIState = BossAIState::MELEE_ATTACK;
		}
		else if (distanceToPlayer < m_rangedAttackRange)
		{
			m_currentAIState = BossAIState::RANGED_ATTACK;
			m_hasFired = false;
		}
		break;
	case BossAIState::MELEE_ATTACK:
	case BossAIState::RANGED_ATTACK:
		if (m_animation.IsFinished())
		{
			m_aiTimer = 0.0f;
			m_currentAIState = BossAIState::COOLDOWN;
		}
		break;
	case BossAIState::LASER_CAST:
		if (m_animation.IsFinished())
		{
			m_currentAIState = BossAIState::LASER_BEAM;
			m_laserAnimation.Play(CharacterAnimationState::LASER_SHEET, m_animDataMap.at(CharacterAnimationState::LASER_SHEET));
		}
		break;
	case BossAIState::LASER_BEAM:
		if (m_laserAnimation.IsFinished())
		{
			m_aiTimer = 0.0f;
			m_currentAIState = BossAIState::COOLDOWN;
		}
		break;
	case BossAIState::GLOWING:
	case BossAIState::BUFF:
		if (m_animation.IsFinished())
		{
			m_isAttackable = true;
			m_currentAIState = BossAIState::IDLE;
		}
		break;
	case BossAIState::COOLDOWN:
		m_aiTimer += dt;
		if (m_aiTimer >= m_cooldownDuration)
			m_currentAIState = BossAIState::IDLE;
		break;
	case BossAIState::DEATH:
		break;
	}

	CharacterAnimationState desiredAnimState = m_currentAnimState;
	switch (m_currentAIState)
	{
	case BossAIState::IDLE:
	case BossAIState::APPEARANCE_WAIT:
	case BossAIState::COOLDOWN:
		desiredAnimState = CharacterAnimationState::IDLE;
		break;
	case BossAIState::MELEE_ATTACK:
		desiredAnimState = CharacterAnimationState::MELEE_ATTACK;
		break;
	case BossAIState::RANGED_ATTACK:
		desiredAnimState = CharacterAnimationState::RANGED_ATTACK;
		break;
	case BossAIState::LASER_CAST:
	case BossAIState::LASER_BEAM:
		desiredAnimState = CharacterAnimationState::LASER_CAST;
		break;
	case BossAIState::GLOWING:
		desiredAnimState = CharacterAnimationState::GLOWING;
		break;
	case BossAIState::BUFF:
		desiredAnimState = CharacterAnimationState::BUFF;
		break;
	case BossAIState::DEATH:
		desiredAnimState = CharacterAnimationState::DEATH;
		break;
	}

	if (m_animation.GetCurrentState() != desiredAnimState)
	{
		m_animation.Play(desiredAnimState, m_animDataMap.at(desiredAnimState));
	}
	m_currentAnimState = desiredAnimState;

	m_animation.Update(dt);
	if (m_currentAIState == BossAIState::LASER_BEAM)
	{
		m_laserAnimation.Update(dt);
	}
}

void BossCharacter::Move(f32 dt)
{
}

void BossCharacter::Attack()
{
}

void BossCharacter::Draw()
{
	AEMtx33 scale = { 0 };
	AEMtx33 rotate = { 0 };
	AEMtx33 translate = { 0 };
	AEMtx33 transform = { 0 };

	AEMtx33Trans(&translate, m_position.x, m_position.y);
	AEMtx33Rot(&rotate, 0);

	float scaleX = (m_currentDirection == CharacterDirection::LEFT) ? m_size.x : -m_size.x;
	if (m_currentAnimState != CharacterAnimationState::APPEARANCE)
	{
		scaleX *= -1.f;
	}
	AEMtx33Scale(&scale, scaleX, m_size.y);

	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	m_animation.Draw(transform);

	if (m_currentAIState == BossAIState::MELEE_ATTACK && m_animation.GetCurrentFrame() >= 3)
	{
		const AttackHitbox& currentHitbox = GetCurrentAttackHitbox();
		AEVec2 hitboxPos;
		hitboxPos.x = m_position.x + (m_currentDirection == CharacterDirection::LEFT ? -currentHitbox.offset.x : currentHitbox.offset.x);
		hitboxPos.y = m_position.y + currentHitbox.offset.y;
		DrawHollowRect(hitboxPos.x, hitboxPos.y, currentHitbox.size.x, currentHitbox.size.y, 1.0f, 0.0f, 0.0f, 0.5f);
	}

	if (m_currentAIState == BossAIState::LASER_BEAM)
	{
		AEVec2 laserPos;
		const AttackHitbox& laserHitbox = GetLaserHitbox();
		laserPos.x = m_position.x + (m_currentDirection == CharacterDirection::LEFT ? -laserHitbox.offset.x : laserHitbox.offset.x);
		laserPos.y = m_position.y + laserHitbox.offset.y;

		AEMtx33Trans(&translate, laserPos.x, laserPos.y);
		float laserScaleX = (m_currentDirection == CharacterDirection::LEFT) ? -laserHitbox.size.x : laserHitbox.size.x;
		AEMtx33Scale(&scale, laserScaleX, laserHitbox.size.y);
		AEMtx33Concat(&transform, &rotate, &scale);
		AEMtx33Concat(&transform, &translate, &transform);

		m_laserAnimation.Draw(transform);
	}
}

void BossCharacter::TakeDamage(s32 damage)
{
	if (!m_isAttackable) 
		return;

	float damageMultiplier = 1.0f;
	if (m_isInBuffState)
	{
		damageMultiplier = 0.4f;
	}
	m_healthPoint -= static_cast<s32>(damage * damageMultiplier);

	std::cout << "Boss takes damage! HP: " << m_healthPoint << std::endl;

	if (m_healthPoint <= 0)
	{
		m_healthPoint = 0;
		m_currentAIState = BossAIState::DEATH;
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
	else if (m_healthPoint <= m_maxHealth * 0.2f && !m_hasBuffed)
	{
		m_hasBuffed = true;
		m_isInBuffState = true;
		m_isAttackable = false;
		m_currentAIState = BossAIState::BUFF;
	}
	else if (m_healthPoint <= m_maxHealth * 0.5f && !m_hasGlowed)
	{
		m_hasGlowed = true;
		m_healthPoint += static_cast<s32>(m_maxHealth * 0.15f);
		m_isAttackable = false;
		m_currentAIState = BossAIState::GLOWING;
	}
}

void BossCharacter::Destroy()
{
}

bool BossCharacter::IsUnbeatable() const
{
	return m_currentAIState == BossAIState::GLOWING || m_currentAIState == BossAIState::BUFF || !m_isAttackable;
}

const AttackHitbox& BossCharacter::GetCurrentAttackHitbox() const
{
	s32 currentFrame = m_animation.GetCurrentFrame();
	if (currentFrame >= 0 && currentFrame < m_meleeHitboxes.size())
	{
		return m_meleeHitboxes[currentFrame];
	}
	return m_meleeHitboxes[0];
}

bool BossCharacter::IsCompletelyDead() const
{
	return (m_currentAIState == BossAIState::DEATH && m_animation.IsFinished());
}