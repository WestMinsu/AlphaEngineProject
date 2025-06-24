#include "MainGameState.h"
#include "GameManager.h"
#include "Constants.h"
#include <iostream>
#include <algorithm>
#include "Utility.h"
#include "AssetManager.h"
#include "WeaponData.h"
#include "VisualEffect.h"
#include "BossCharacter.h"

MainGameState::MainGameState()
{
	m_pUiSlot = nullptr;
}

MainGameState::~MainGameState() {}

void MainGameState::Init()
{
	AEGfxSetCamPosition(0.f, 0.f);
	m_Player.Init({ -kHalfWindowWidth + 200.f, 0.f });
	m_MeleeEnemy.Init({ kHalfWindowWidth - 200.f, 0.f }, &m_Player);
	m_MageEnemy.Init({ kHalfWindowWidth - 500.f, 0.f }, &m_Player);
	m_Boss.Init({ kHalfWindowWidth - 300.f, 100.f }, &m_Player);

	m_TileMaps.push_back(TileMap("Assets/Maps/test0_32.tmj"));
	m_TileMaps.push_back(TileMap("Assets/Maps/test1_32.tmj", m_TileMaps[0].GetMapWidth(), 0.f));
	m_TileMaps.push_back(TileMap("Assets/Maps/test1_32.tmj", m_TileMaps[0].GetMapWidth(), 0.f));
	m_Background.Init();

	m_pUiSlot = LoadImageAsset("Assets/UI/slot.png");
	m_weaponIconMap[WeaponType::FIRE] = LoadImageAsset("Assets/MagicArrow/fire_icon.png");
	m_weaponIconMap[WeaponType::ICE] = LoadImageAsset("Assets/MagicArrow/ice_icon.png");
	m_weaponIconMap[WeaponType::LIGHTNING] = LoadImageAsset("Assets/Thunder Strike/lightning_icon.png");

	m_lightningEffectData.texturePath = "Assets/Thunder Strike/Thunderstrike w blur.png";
	m_lightningEffectData.pTexture = LoadImageAsset(m_lightningEffectData.texturePath.c_str());
	m_lightningEffectData.frameCount = 8;
	m_lightningEffectData.orientation = SpriteSheetOrientation::HORIZONTAL;
	m_lightningEffectData.frameDuration = 0.12f;
	m_lightningEffectData.loop = false;

	m_pHealthBarFrame = LoadImageAsset("Assets/UI/healthbar_frame.png");
	m_pHealthBar = LoadImageAsset("Assets/UI/healthbar_fill.png");
}

void MainGameState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_R))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
		return;
	}

	m_Player.Update(dt);
	m_MeleeEnemy.Update(dt);
	m_MageEnemy.Update(dt);
	m_Boss.Update(dt);

	int tileX = m_Player.GetPosition().x / 32;
	int tileY = m_Player.GetPosition().y / 32;

	// To Do 
	// TileMap Collision

	if (m_Player.GetPosition().x > 0.f)
	{
		AEGfxSetCamPosition(m_Player.GetPosition().x, 0.f);
	}

	bool isPlayerCasting = m_Player.GetCurrentAnimState() == CharacterAnimationState::RANGED_ATTACK;
	bool canPlayerFire = m_Player.GetAnimation().GetCurrentFrame() == 4 && !m_Player.HasFiredProjectile();
	if (isPlayerCasting && canPlayerFire)
	{
		WeaponType currentWeapon = m_Player.GetCurrentWeaponType();
		if (currentWeapon == WeaponType::FIRE || currentWeapon == WeaponType::ICE)
		{
			m_playerProjectiles.emplace_back();
			Projectile& newProjectile = m_playerProjectiles.back();
			AEVec2 spawnPos;
			const AEVec2& playerPos = m_Player.GetPosition();
			const AEVec2& playerSize = m_Player.GetSize();
			CharacterDirection playerDir = m_Player.GetDirection();
			const f32 offsetX_Ratio = 0.3f;
			const f32 offsetY_Ratio = 0.1f;
			spawnPos.x = playerPos.x + (playerDir == CharacterDirection::RIGHT ? playerSize.x * offsetX_Ratio : -playerSize.x * offsetX_Ratio);
			spawnPos.y = playerPos.y + playerSize.y * offsetY_Ratio;
			const ProjectileData& projData = m_Player.GetCurrentProjectileData();
			newProjectile.Init(spawnPos, playerDir, projData);
			m_Player.SetFiredProjectile(true);
		}
		else if (currentWeapon == WeaponType::LIGHTNING)
		{
			ACharacter* target = FindClosestEnemyInFront();
			if (target)
			{
				target->TakeDamage(25);
				m_visualEffects.emplace_back();
				VisualEffect& newEffect = m_visualEffects.back();
				newEffect.Init(target->GetPosition(), m_lightningEffectData);
			}
			m_Player.SetFiredProjectile(true);
		}
	}

	bool isMageCasting = m_MageEnemy.GetCurrentAnimState() == CharacterAnimationState::RANGED_ATTACK;
	bool canMageFire = m_MageEnemy.GetAnimation().GetCurrentFrame() == 15 && !m_MageEnemy.HasFiredProjectile();

	if (isMageCasting && canMageFire)
	{
		m_enemyProjectiles.emplace_back();
		Projectile& newProjectile = m_enemyProjectiles.back();

		const ProjectileData& projData = m_MageEnemy.GetProjectileData();

		newProjectile.Init(m_MageEnemy.GetPosition(), m_MageEnemy.GetDirection(), projData);

		m_MageEnemy.SetFiredProjectile(true);
	}

	bool isBossCastingRanged = m_Boss.GetCurrentAnimState() == CharacterAnimationState::RANGED_ATTACK;
	if (isBossCastingRanged && m_Boss.GetAnimation().IsFinished() && !m_Boss.HasFired())
	{
		m_Boss.SetFired(true);
		const ProjectileData& projData = m_Boss.GetProjectileData();

		if (m_Boss.IsBuffed())
		{
			// 3-way shot	
		}
		else
		{
			m_enemyProjectiles.emplace_back();
			Projectile& newProjectile = m_enemyProjectiles.back();
			newProjectile.Init(m_Boss.GetPosition(), m_Boss.GetDirection(), projData);
		}
	}

	if (m_Boss.GetCurrentAIState() == BossAIState::LASER_BEAM &&
		m_Boss.GetLaserAnimation().GetCurrentFrame() >= 8 && 
		!m_Boss.IsUnbeatable() && m_Player.GetHealth() > 0)
	{
		const AttackHitbox& laserHitbox = m_Boss.GetLaserHitbox();
		AEVec2 bossPos = m_Boss.GetPosition();
		AEVec2 hitboxPos;
		hitboxPos.x = bossPos.x + (m_Boss.GetDirection() == CharacterDirection::LEFT ? -laserHitbox.offset.x : laserHitbox.offset.x);
		hitboxPos.y = bossPos.y + laserHitbox.offset.y;

		if (CheckAABBCollision(hitboxPos, laserHitbox.size, m_Player.GetPosition(), m_Player.GetSize()))
		{
			m_Player.TakeDamage(1);
		}
	}

	for (auto proj = m_playerProjectiles.begin(); proj != m_playerProjectiles.end(); )
	{
		proj->Update(dt);
		bool hit = false;
		if (proj->IsActive() && m_MeleeEnemy.GetHealth() > 0)
		{
			if (CheckAABBCollision(proj->GetPosition(), proj->GetSize(), m_MeleeEnemy.GetPosition(), m_MeleeEnemy.GetSize()))
			{
				m_MeleeEnemy.TakeDamage(proj->GetDamage());
				proj->Deactivate();
				hit = true;
			}
		}
		if (!hit && proj->IsActive() && m_MageEnemy.GetHealth() > 0)
		{
			if (CheckAABBCollision(proj->GetPosition(), proj->GetSize(), m_MageEnemy.GetPosition(), m_MageEnemy.GetSize()))
			{
				m_MageEnemy.TakeDamage(proj->GetDamage());
				proj->Deactivate();
			}
		}

		if (!hit && proj->IsActive() && m_Boss.GetHealth() > 0 && m_Boss.IsAttackable())
		{
			if (CheckAABBCollision(proj->GetPosition(), proj->GetSize(), m_Boss.GetPosition(), m_Boss.GetSize()))
			{
				m_Boss.TakeDamage(proj->GetDamage());
				proj->Deactivate();
			}
		}

		if (!proj->IsActive())
		{
			proj->Destroy();
			proj = m_playerProjectiles.erase(proj);
		}
		else
			++proj;
	}

	for (auto& proj : m_enemyProjectiles)
	{
		proj.Update(dt);
		if (proj.IsActive() && m_Player.GetHealth() > 0 && CheckAABBCollision(proj.GetPosition(), proj.GetSize(), m_Player.GetPosition(), m_Player.GetSize()))
		{
			m_Player.TakeDamage(proj.GetDamage());
			proj.Deactivate();
		}
	}

	if (m_Player.IsMeleeAttackHitboxActive() && !m_Player.HasHitEnemyThisAttack())
	{
		const AttackHitbox& currentHitbox = m_Player.GetCurrentAttackHitbox();
		const AEVec2& playerPos = m_Player.GetPosition();
		AEVec2 hitboxPos;
		hitboxPos.x = playerPos.x + (m_Player.GetDirection() == CharacterDirection::RIGHT ? currentHitbox.offset.x : -currentHitbox.offset.x);
		hitboxPos.y = playerPos.y + currentHitbox.offset.y;

		if (m_MeleeEnemy.GetHealth() > 0 && CheckAABBCollision(hitboxPos, currentHitbox.size, m_MeleeEnemy.GetPosition(), m_MeleeEnemy.GetSize()))
		{
			m_MeleeEnemy.TakeDamage(10);
			m_Player.RegisterHit();
		}
		else if (m_MageEnemy.GetHealth() > 0 && CheckAABBCollision(hitboxPos, currentHitbox.size, m_MageEnemy.GetPosition(), m_MageEnemy.GetSize()))
		{
			m_MageEnemy.TakeDamage(10);
			m_Player.RegisterHit();
		}
		else if (m_Boss.GetHealth() > 0 && m_Boss.IsAttackable() && CheckAABBCollision(hitboxPos, currentHitbox.size, m_Boss.GetPosition(), m_Boss.GetSize()))
		{
			m_Boss.TakeDamage(10);
			m_Player.RegisterHit();
		}
	}

	for (auto& tm : m_TileMaps)
	{
		tm.Update(dt);
	}

	for (auto& effect : m_visualEffects)
	{
		effect.Update(dt);
	}
}

void MainGameState::Draw()
{
	m_Background.Draw();
	for (auto tm : m_TileMaps)
	{
		tm.Draw();
	}

	m_MeleeEnemy.Draw();
	m_MageEnemy.Draw();
	
	if (!m_Boss.IsCompletelyDead())
	{
		m_Boss.Draw();
	}

	m_Player.Draw();

	for (auto& projectile : m_playerProjectiles)
		projectile.Draw();
	for (auto& projectile : m_enemyProjectiles)
		projectile.Draw();
	for (auto& effect : m_visualEffects) 
		effect.Draw();

	DrawUI();
}

void MainGameState::Exit()
{
	for (auto& tm : m_TileMaps)
	{
		tm.Destroy();
	}
	m_Background.Destroy();
	m_Player.Destroy();
	m_MeleeEnemy.Destroy();
	m_MageEnemy.Destroy();
	for (auto& projectile : m_playerProjectiles) projectile.Destroy();
	for (auto& projectile : m_enemyProjectiles) projectile.Destroy();
	m_playerProjectiles.clear();
	m_enemyProjectiles.clear();

	AEGfxSetCamPosition(0.f, 0.f);
}

void MainGameState::DrawUI()
{
	const f32 slotSize = 80.0f;
	const f32 slotMargin = 10.0f;
	const auto& availableWeapons = m_Player.GetAvailableWeapons();
	const size_t totalSlots = availableWeapons.size();
	if (totalSlots == 0)
		return;

	const f32 totalWidth = (slotSize * totalSlots) + (slotMargin * (totalSlots - 1));
	const f32 startX = -(totalWidth / 2.0f) + (slotSize / 2.0f);
	const f32 posY = -(kHalfWindowHeight - 60.0f);

	f32 xCam, yCam;
		AEGfxGetCamPosition(&xCam, &yCam);

	WeaponType currentWeapon = m_Player.GetCurrentWeaponType();

	for (size_t i = 0; i < totalSlots; ++i)
	{
		float posX = startX + i * (slotSize + slotMargin) + xCam;
		WeaponType slotWeaponType = availableWeapons[i];

		DrawRect(posX, posY, slotSize, slotSize, 1.f, 1.f, 1.f, 0.7f, m_pUiSlot);

		if (m_weaponIconMap.count(slotWeaponType))
		{
			DrawRect(posX, posY, slotSize * 0.8f, slotSize * 0.8f, 1.f, 1.f, 1.f, 1.f, m_weaponIconMap.at(slotWeaponType));
		}

		if (slotWeaponType == currentWeapon)
		{
			DrawHollowRect(posX, posY, slotSize, slotSize, 1.0f, 1.0f, 0.0f, 1.0f);
		}
	}

	if (m_Boss.IsAttackable()) // 보스가 공격 가능한 상태일 때만 체력바 표시
	{
		const float barWidth = 500.f;
		const float barHeight = 125.f;
		const float barX = 0;
		const float barY = kHalfWindowHeight - 150.f;

		DrawRect(barX, barY, barWidth, barHeight, 0.1f, 0.1f, 0.1f, 1.f);
		float healthRatio = static_cast<float>(m_Boss.GetHealth()) / m_Boss.getMaxHealth();
		float currentHealthWidth = barWidth * healthRatio;
		// 남은 체력 그리기
		DrawRect(barX - (barWidth - currentHealthWidth) / 2.0f, barY, currentHealthWidth, barHeight, 1.0f, 0.0f, 0.0f, 1.f);
		// 체력바 테두리
		DrawHollowRect(barX, barY, barWidth, barHeight, 1.f, 1.f, 0.f, 1.f);
	}

	if (m_Boss.GetCurrentAnimState() == CharacterAnimationState::RANGED_ATTACK && m_Boss.GetAnimation().IsFinished() && !m_Boss.HasFired())
	{
		m_Boss.SetFired(true);
		// 버프 상태에 따른 분기
		if (m_Boss.IsBuffed())
		{
			// 3-way shot
			// ... 로직 추가 ...
		}
		else
		{
			// 단일 샷
			// ... 로직 추가 ...
		}
	}

	// 레이저 공격 충돌
	if (m_Boss.GetCurrentAnimState() == CharacterAnimationState::LASER_SHEET && m_Boss.GetAnimation().GetCurrentFrame() >= 9)
	{
		// AABB Check
	}
}

ACharacter* MainGameState::FindClosestEnemyInFront()
{
	ACharacter* closestEnemy = nullptr;
	float minDistance = FLT_MAX;

	AEVec2 playerPos = m_Player.GetPosition();
	CharacterDirection playerDir = m_Player.GetDirection();

	std::vector<ACharacter*> enemies;
	if (m_MeleeEnemy.GetHealth() > 0)
		enemies.push_back(&m_MeleeEnemy);
	if (m_MageEnemy.GetHealth() > 0)
		enemies.push_back(&m_MageEnemy);
	if (m_Boss.GetHealth() > 0)
		enemies.push_back(&m_Boss);

	for (ACharacter* enemy : enemies)
	{
		const AEVec2& enemyPos = enemy->GetPosition();

		bool isInFront = (playerDir == CharacterDirection::RIGHT && enemyPos.x > playerPos.x) ||
			(playerDir == CharacterDirection::LEFT && enemyPos.x < playerPos.x);

		const float yTolerance = m_Player.GetSize().y / 2.0f;
		float yDistance = std::abs(playerPos.y - enemyPos.y);

		if (isInFront && yDistance <= yTolerance)
		{
			AEVec2 distVec = { enemyPos.x - playerPos.x, enemyPos.y - playerPos.y };
			float distance = AEVec2Length(&distVec);
			if (distance < minDistance)
			{
				minDistance = distance;
				closestEnemy = enemy;
			}
		}
	}
	return closestEnemy;
}