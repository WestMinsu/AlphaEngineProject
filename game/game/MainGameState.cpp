#include "MainGameState.h"
#include "GameManager.h"
#include "Constants.h"
#include <iostream>
#include <algorithm>
#include "Utility.h"
#include "AssetManager.h"
#include "WeaponData.h"

MainGameState::MainGameState() 
{
	MainGameState::m_pUiSlot = nullptr;
	MainGameState::m_pFireIcon = nullptr;
}
MainGameState::~MainGameState() {}

void MainGameState::Init()
{
	AEGfxSetCamPosition(0.f, 0.f);
	m_Player.Init({ -kHalfWindowWidth + 200.f, 0.f });
	m_MeleeEnemy.Init({ kHalfWindowWidth - 200.f, 0.f }, &m_Player);
	m_MageEnemy.Init({ kHalfWindowWidth - 500.f, 0.f }, &m_Player);

	m_TileMaps.push_back(TileMap("Assets/Maps/test0_32.tmj"));
	m_TileMaps.push_back(TileMap("Assets/Maps/test1_32.tmj", m_TileMaps[0].GetMapWidth() * m_TileMaps[0].GetTileSize(), 0.f));
	m_Background.Init();

	m_pUiSlot = LoadImageAsset("Assets/UI/slot.png");
	m_weaponIconMap[WeaponType::FIRE] = LoadImageAsset("Assets/MagicArrow/fire_icon.png");
	m_weaponIconMap[WeaponType::ICE] = LoadImageAsset("Assets/MagicArrow/ice_icon.png");
}

void MainGameState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_R))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
		return;
	}

	m_oldPositionPlayer = m_Player.GetPosition();

	m_Player.Update(dt);
	m_MeleeEnemy.Update(dt);
	m_MageEnemy.Update(dt);

	int tileX = m_Player.GetPosition().x / 32;
	int tileY = m_Player.GetPosition().y / 32;

	for (auto& tileMap : m_TileMaps)
	{
		tileMap.checkCollisionTileMap(m_Player.GetPosition(), m_Player.GetSize());
	}


	if (m_Player.GetPosition().x > 0.f)
	{
		AEGfxSetCamPosition(m_Player.GetPosition().x, 0.f);
	}

	if (m_Player.GetCurrentAnimState() == CharacterAnimationState::PROJECTILE_ATTACK &&
		m_Player.GetAnimation().GetCurrentFrame() == 4 &&
		!m_Player.HasFiredProjectile())
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

	if (m_MageEnemy.GetCurrentAnimState() == CharacterAnimationState::PROJECTILE_ATTACK &&
		m_MageEnemy.GetAnimation().GetCurrentFrame() == 15 &&
		!m_MageEnemy.HasFiredProjectile())
	{
		m_enemyProjectiles.emplace_back();
		Projectile& newProjectile = m_enemyProjectiles.back();

		const ProjectileData& projData = m_MageEnemy.GetProjectileData();

		newProjectile.Init(m_MageEnemy.GetPosition(), m_MageEnemy.GetDirection(), projData);

		m_MageEnemy.SetFiredProjectile(true);
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
	}

	for (auto& tm : m_TileMaps)
	{
		tm.Update(dt);
	}
}

void MainGameState::Draw()
{
	m_Background.Draw();
	m_MeleeEnemy.Draw();
	m_MageEnemy.Draw();

	for (auto tm : m_TileMaps)
	{
		tm.Draw();
	}

	m_Player.Draw();

	for (auto& projectile : m_playerProjectiles)
		projectile.Draw();
	for (auto& projectile : m_enemyProjectiles) 
		projectile.Draw();

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
	if (totalSlots == 0) return;

	const f32 totalWidth = (slotSize * totalSlots) + (slotMargin * (totalSlots - 1));
	const f32 startX = -(totalWidth / 2.0f) + (slotSize / 2.0f);
	const f32 posY = kHalfWindowHeight - 60.0f;
	
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
}