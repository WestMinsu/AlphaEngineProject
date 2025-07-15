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
	m_hasHitPlayerThisAttack = false;

	m_hitboxSize = { m_size.x * 0.5f, m_size.y * 0.5f };
	m_hitboxOffset = { m_size.x * 0.01f, m_size.y * 0.02f };
	killScore = 20000;
	m_pBossHealthBar = LoadImageAsset("Assets/UI/hpBar.png");
}

BossCharacter::BossCharacter(const BossCharacter& bossCopy)
{
	m_size = bossCopy.m_size;
	m_maxHealth = bossCopy.m_maxHealth;
	m_healthPoint = bossCopy.m_healthPoint;
	m_characterSpeed = bossCopy.m_characterSpeed;
	m_currentDirection = bossCopy.m_currentDirection;
	m_currentAnimState = bossCopy.m_currentAnimState;
	m_element = bossCopy.m_element;

	m_currentAIState = bossCopy.m_currentAIState;
	m_pPlayer = bossCopy.m_pPlayer;


	m_animation = bossCopy.m_animation;
	m_animDataMap = bossCopy.m_animDataMap;
	m_hitboxSize = bossCopy.m_hitboxSize;
	m_hitboxOffset = bossCopy.m_hitboxOffset;

	m_laserVisualSize = bossCopy.m_laserVisualSize;
	m_laserVisualOffset = bossCopy.m_laserVisualOffset;
	m_laserAnimation = bossCopy.m_laserAnimation;

	m_isAttackable = bossCopy.m_isAttackable;
	m_hasGlowed = false;
	m_hasBuffed = false;
	m_isInBuffState = false;
	m_aiTimer = 0.0f;
	m_meleeAttackRange = bossCopy.m_meleeAttackRange;
	m_rangedAttackRange = bossCopy.m_rangedAttackRange;
	m_cooldownDuration = bossCopy.m_cooldownDuration;
	m_hasFired = false;
	m_hasHitPlayerThisAttack = false;

	m_projectileData = bossCopy.m_projectileData;
	m_meleeHitboxes = bossCopy.m_meleeHitboxes;
	m_laserHitbox = bossCopy.m_laserHitbox;
	m_pBossHealthBar = bossCopy.m_pBossHealthBar;
}

BossCharacter::~BossCharacter() {}

void BossCharacter::Init(AEVec2 position)
{
	m_position = position;
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
	m_animDataMap[CharacterAnimationState::LASER_SHEET] = { "Assets/Boss/laser_sheet.png", nullptr, 34, SpriteSheetOrientation::VERTICAL, 0.12f, false };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Boss/death.png", nullptr, 14, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::BUFF] = { "Assets/Boss/buff.png", nullptr, 10, SpriteSheetOrientation::HORIZONTAL, 0.1f, false };


	for (auto& pair : m_animDataMap)
	{
		pair.second.pTexture = LoadImageAsset(pair.second.texturePath.c_str());
	}

	m_projectileData.speed = 1200.0f;
	m_projectileData.damage = 10;
	m_projectileData.size = { 150.f, 50.f };
	m_projectileData.animData = { "Assets/Boss/arm_projectile.png", nullptr, 1, SpriteSheetOrientation::HORIZONTAL, 1.0f, true };
	m_projectileData.animData.pTexture = LoadImageAsset(m_projectileData.animData.texturePath.c_str());

	m_meleeHitboxes.resize(7);
	m_meleeHitboxes[3] = { { m_size.x * 0.4f, m_size.y * 0.1f}, { m_size.x * 0.6f, m_size.y * 0.3f } };
	m_meleeHitboxes[4] = { { m_size.x * 0.4f, m_size.y * 0.0f}, { m_size.x * 0.6f, m_size.y * 0.3f } };

	m_laserVisualSize = { m_size.x * 3.0f, m_size.y * 0.5f };
	m_laserVisualOffset = { m_size.x, -m_size.y * 0.15f };

	float laserSizeX = m_size.x * 3.0f;
	float laserSizeY = m_size.y * 0.1f;
	float laserOffsetX = m_laserVisualOffset.x;
	float laserOffsetY = m_laserVisualOffset.y + laserSizeY * 0.8f;

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

	if (m_currentAIState == BossAIState::GLOWING || m_currentAIState == BossAIState::BUFF)
	{
		if (m_animation.IsFinished())
		{
			m_isAttackable = true;
			m_currentAIState = BossAIState::IDLE;
		}
		m_animation.Update(dt);
		return;
	}

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
			m_hasHitPlayerThisAttack = false;
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
			m_laserTargetPos = m_pPlayer->GetPosition();
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
		const AttackHitbox& currentHitbox = GetCurrentMeleeHitbox();
		AEVec2 hitboxPos;
		hitboxPos.x = m_position.x + (m_currentDirection == CharacterDirection::LEFT ? -currentHitbox.offset.x : currentHitbox.offset.x);
		hitboxPos.y = m_position.y + currentHitbox.offset.y;
		DrawHollowRect(hitboxPos.x, hitboxPos.y, currentHitbox.size.x, currentHitbox.size.y, 1.0f, 0.0f, 0.0f, 0.5f);
	}


	if (m_currentAIState == BossAIState::LASER_BEAM)
	{
		const AttackHitbox& laserData = GetLaserHitbox();
		float offsetDir = (m_currentDirection == CharacterDirection::RIGHT) ? 1.0f : -1.0f;

		AEVec2 visualPos = { m_position.x + (m_laserVisualOffset.x * offsetDir), m_position.y + m_laserVisualOffset.y };
		const AttackHitbox& laserHitbox = GetLaserHitbox();
		AEVec2 hitboxPos = { m_position.x + (laserHitbox.offset.x * offsetDir), m_position.y + laserHitbox.offset.y };

		if (m_isInBuffState)
		{
			float yOffsets[] = { 150.f, 0.f, -150.f }; 
			for (float yOffset : yOffsets)
			{
				AEMtx33 laserTransform;
				AEMtx33Trans(&translate, visualPos.x, visualPos.y + yOffset);
				AEMtx33Rot(&rotate, 0);
				AEMtx33Scale(&scale, m_laserVisualSize.x * offsetDir, m_laserVisualSize.y);
				AEMtx33Concat(&laserTransform, &rotate, &scale);
				AEMtx33Concat(&laserTransform, &translate, &laserTransform);
				m_laserAnimation.Draw(laserTransform);
				if (m_laserAnimation.GetCurrentFrame() >= 22)
					DrawHollowRect(hitboxPos.x, hitboxPos.y + yOffset, laserData.size.x, laserData.size.y, 1.f, 0.f, 1.f, 0.5f);
			}
		}
		else
		{
			AEVec2 finalVisualPos = visualPos;
			AEVec2 finalHitboxPos = hitboxPos;
			float yDiff = m_laserTargetPos.y - m_position.y;
			if (yDiff > 150.f)
			{
				finalVisualPos.y += 100.f;
				finalHitboxPos.y += 100.f;
			}
			else if (yDiff < -150.f)
			{
				finalVisualPos.y -= 100.f;
				finalHitboxPos.y -= 100.f;
			}

			AEMtx33 laserTransform;
			AEMtx33Trans(&translate, finalVisualPos.x, finalVisualPos.y);
			AEMtx33Rot(&rotate, 0);
			AEMtx33Scale(&scale, m_laserVisualSize.x * offsetDir, m_laserVisualSize.y);
			AEMtx33Concat(&laserTransform, &rotate, &scale);
			AEMtx33Concat(&laserTransform, &translate, &laserTransform);
			m_laserAnimation.Draw(laserTransform);
			if (m_laserAnimation.GetCurrentFrame() >= 22)
				DrawHollowRect(finalHitboxPos.x, finalHitboxPos.y, laserHitbox.size.x, laserHitbox.size.y, 1.f, 0.f, 1.f, 0.5f);
		}
	}

	DrawHollowRect(m_position.x + m_hitboxOffset.x, m_position.y + m_hitboxOffset.y, m_hitboxSize.x, m_hitboxSize.y, 1.0f, 0.0f, 0.0f, 1.f);
}

void BossCharacter::TakeDamage(s32 damage, DamageType damageType)
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
		m_currentAnimState = CharacterAnimationState::BUFF;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
	else if (m_healthPoint <= m_maxHealth * 0.5f && !m_hasGlowed)
	{
		m_hasGlowed = true;
		m_healthPoint += static_cast<s32>(m_maxHealth * 0.15f);
		m_isAttackable = false;
		m_currentAIState = BossAIState::GLOWING;
		m_currentAnimState = CharacterAnimationState::GLOWING;
		m_animation.Play(m_currentAnimState, m_animDataMap.at(m_currentAnimState));
	}
}

void BossCharacter::Destroy()
{
}

BossCharacter* BossCharacter::Clone()
{
	return new BossCharacter(*this);
}

bool BossCharacter::IsUnbeatable() const
{
	return m_currentAIState == BossAIState::GLOWING || m_currentAIState == BossAIState::BUFF || !m_isAttackable;
}

const AttackHitbox& BossCharacter::GetCurrentMeleeHitbox() const
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

std::vector<AttackHitbox> BossCharacter::GetCurrentActiveHitboxes()
{
	return std::vector<AttackHitbox>();
}

void BossCharacter::GetBossPattern(AEGfxTexture*& pBossMessageTexture, float& bossMessageTimer, BossAIState& previousBossAIState)
{
	if (m_currentAIState != previousBossAIState)
	{
		if (m_currentAIState == BossAIState::GLOWING)
		{
			pBossMessageTexture = LoadImageAsset("Assets/UI/healtext.png");
			bossMessageTimer = m_bossMessageDuration;
		}
		else if (m_currentAIState == BossAIState::BUFF)
		{
			pBossMessageTexture = LoadImageAsset("Assets/UI/enragetext.png");
			bossMessageTimer = m_bossMessageDuration;
		}
		previousBossAIState = m_currentAIState;
	}
}

void BossCharacter::AttackMelee(PlayerCharacter& player)
{
	bool isBossHitboxActive = m_animation.GetCurrentFrame() >= 3;
	if (m_currentAIState == BossAIState::MELEE_ATTACK 
		&& isBossHitboxActive 
		&& !m_hasHitPlayerThisAttack
		&& player.GetHealth() > 0)
	{
		const AttackHitbox& bossHitbox = GetCurrentMeleeHitbox();
		AEVec2 bossHitboxPos;
		bossHitboxPos.x = m_position.x + (m_currentDirection == CharacterDirection::RIGHT ? bossHitbox.offset.x : -bossHitbox.offset.x);
		bossHitboxPos.y = m_position.y + bossHitbox.offset.y;

		AEVec2 playerHitboxPos = player.GetPosition();
		playerHitboxPos.x += player.GetHitboxOffset().x;
		playerHitboxPos.y += player.GetHitboxOffset().y;

		if (CheckAABBCollision(bossHitboxPos, bossHitbox.size, playerHitboxPos, player.GetHitboxSize()))
		{
			player.TakeDamage(15, DamageType::NONE);
			RegisterPlayerHit();
			m_hasHitPlayerThisAttack = true;
		}
	}

}

void BossCharacter::AttackRange(PlayerCharacter& player, std::vector<Projectile>& enemyProjectiles)
{
	if (m_currentAIState == BossAIState::RANGED_ATTACK
		&& m_animation.IsFinished() 
		&& !m_hasFired)
	{
		m_hasFired = true;
		AEVec2 playerPos = player.GetPosition();

		if (m_isInBuffState)
		{
			// 3-Way Shot
			float baseAngle = atan2(playerPos.y - m_position.y, playerPos.x - m_position.x);
			AEVec2 dirMid = { cosf(baseAngle), sinf(baseAngle) };
			AEVec2 dirUp = { cosf(baseAngle - 0.3f), sinf(baseAngle - 0.3f) };
			AEVec2 dirDown = { cosf(baseAngle + 0.3f), sinf(baseAngle + 0.3f) };

			enemyProjectiles.emplace_back().Init(m_position, dirUp, m_projectileData);
			enemyProjectiles.emplace_back().Init(m_position, dirMid, m_projectileData);
			enemyProjectiles.emplace_back().Init(m_position, dirDown, m_projectileData);
		}
		else
		{
			AEVec2 finalDir = { (m_currentDirection == CharacterDirection::RIGHT ? 1.f : -1.f), 0.f };
			float yDiff = playerPos.y - m_position.y;
			if (yDiff > 150.f) finalDir.y = 0.5f;
			else if (yDiff < -150.f) finalDir.y = -0.5f;

			AEVec2Normalize(&finalDir, &finalDir);
			enemyProjectiles.emplace_back().Init(m_position, finalDir, m_projectileData);
		}
	}
}

void BossCharacter::AttackLaser(PlayerCharacter& player)
{
	bool isLaserHitboxActive = m_laserAnimation.GetCurrentFrame() >= 22;

	if (m_currentAIState == BossAIState::LASER_BEAM
		&& isLaserHitboxActive 
		&& !IsUnbeatable() 
		&& player.GetHealth() > 0)
	{
		float offsetDir = (m_currentDirection == CharacterDirection::RIGHT) ? 1.0f : -1.0f;
		AEVec2 laserBasePos = { m_position.x + (m_laserHitbox.offset.x * offsetDir), m_position.y + m_laserHitbox.offset.y };

		AEVec2 playerHitboxPos = player.GetPosition();
		playerHitboxPos.x += player.GetHitboxOffset().x;
		playerHitboxPos.y += player.GetHitboxOffset().y;

		if (m_isInBuffState)
		{
			// 3-Way Laser
			float yOffsets[] = { 150.f, 0.f, -150.f };
			for (float yOffset : yOffsets)
			{
				AEVec2 currentLaserPos = { laserBasePos.x, laserBasePos.y + yOffset };
				if (CheckAABBCollision(currentLaserPos, m_laserHitbox.size, playerHitboxPos, player.GetHitboxSize()))
				{
					player.TakeDamage(1, DamageType::NONE);
					break;
				}
			}
		}
		else
		{
			AEVec2 finalLaserPos = laserBasePos;
			float yDiff = m_laserTargetPos.y - m_position.y;
			if (yDiff > 150.f) finalLaserPos.y += 100.f;
			else if (yDiff < -150.f) finalLaserPos.y -= 100.f;

			if (CheckAABBCollision(finalLaserPos, m_laserHitbox.size, playerHitboxPos, player.GetHitboxSize()))
			{
				player.TakeDamage(1, DamageType::NONE);
			}
		}
	}
}

void BossCharacter::DrawBossHPUI()
{
	//if (m_isAttackable)
	//{
	//	f32 xCam, yCam;
	//	AEGfxGetCamPosition(&xCam, &yCam);
	//	const float barWidth = 500.f;
	//	const float barHeight = 125.f;
	//	const float barX = 0;
	//	const float barY = kHalfWindowHeight - 150.f;
	//	DrawRect(barX + xCam, barY, barWidth, barHeight, 0.1f, 0.1f, 0.1f, 1.f);
	//	float healthRatio = static_cast<float>(m_healthPoint) / m_maxHealth;
	//	float currentHealthWidth = barWidth * healthRatio;
	//	DrawRect(barX + xCam - (barWidth - currentHealthWidth) / 2.0f, barY, currentHealthWidth, barHeight, 1.0f, 0.0f, 0.0f, 1.f);
	//	DrawHollowRect(barX + xCam, barY, barWidth, barHeight, 1.f, 1.f, 0.f, 1.f);
	//}

	if (m_isAttackable)
	{
		f32 xCam, yCam;
		AEGfxGetCamPosition(&xCam, &yCam);
		const float barWidth = 600.f;
		const float barHeight = 120.f;
		const float barX = 0;
		const float barY = kHalfWindowHeight - 150.f;
		
		DrawRect(barX + xCam, barY, barWidth, barHeight, 1.f, 1.f, 1.f, 1.f, m_pBossHealthBar);
		float healthRatio = static_cast<float>(m_healthPoint) / m_maxHealth;
		float fillWidth = barWidth * 0.70f;
		float fillHeight = barHeight * 0.42f;
		float currentHealthWidth = fillWidth * healthRatio;

		DrawRect(barX + xCam - (fillWidth - currentHealthWidth) / 2.0f, barY, currentHealthWidth, fillHeight, 1.f, 0.2f, 0.2f, 1.f);
	}
}
