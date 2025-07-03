#include "MainGameState.h"
#include "GameManager.h"
#include "Constants.h"
#include <iostream>
#include <algorithm>
#include <string>
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
	//m_MeleeEnemy.Init({ kHalfWindowWidth - 700.f, 0.f }, &m_Player);
	//m_MageEnemy.Init({ kHalfWindowWidth - 550.f, 0.f }, &m_Player);
	//m_FireWormEnemy.Init({ kHalfWindowWidth - 550.f, 0.f }, &m_Player);
	m_Boss.Init({ kHalfWindowWidth - 300.f, 100.f }, &m_Player);

	TileMaps.push_back(TileMap("Assets/Maps/test0_32.tmj", 2.f));
	TileMaps.push_back(TileMap("Assets/Maps/test1_32.tmj", 2.f, TileMaps[0].GetMapTotalWidth()));

	m_Background.Init();
	MeleeEnemyCharacter* melee = new MeleeEnemyCharacter();
	melee->Init({ kHalfWindowWidth - 700.f, 0.f }, &m_Player);
	MageEnemyCharacter* mage = new MageEnemyCharacter();
	mage->Init({ kHalfWindowWidth - 550.f, 0.f }, &m_Player);
	FireWormEnemyCharacter* fire = new FireWormEnemyCharacter();
	fire->Init({ kHalfWindowWidth - 550.f, 0.f }, &m_Player);

	//m_factory = std::make_shared<EnemyFactory>();
	m_factory.RegisterPrototype("Melee", melee);
	m_factory.RegisterPrototype("Mage", mage);
	m_factory.RegisterPrototype("Fire", fire);

	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 1350, -kHalfWindowHeight + 270 }, &m_factory, "Melee"));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 1660, -kHalfWindowHeight + 680 }, &m_factory, "Mage"));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 2150, -kHalfWindowHeight + 330}, &m_factory, "Melee"));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 2820, -kHalfWindowHeight + 740 }, &m_factory, "Mage"));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 2950, -kHalfWindowHeight + 425 }, &m_factory, "Melee"));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 3425, -kHalfWindowHeight + 585}, &m_factory, "Fire"));

	m_Enemies.push_back(&m_Boss);

	m_pUiSlot = LoadImageAsset("Assets/UI/slot.png");
	m_weaponIconMap[DamageType::FIRE] = LoadImageAsset("Assets/MagicArrow/fire_icon.png");
	m_weaponIconMap[DamageType::ICE] = LoadImageAsset("Assets/MagicArrow/ice_icon.png");
	m_weaponIconMap[DamageType::LIGHTNING] = LoadImageAsset("Assets/Thunder Strike/lightning_icon.png");

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
	if (m_Boss.IsCompletelyDead())
	{
		GameManager::ChangeState(GameState::GAME_CLEAR);
		return;
	}
	if (m_Player.IsCompletelyDead())
	{
		GameManager::ChangeState(GameState::GAME_OVER);
		return;
	}
	if (AEInputCheckTriggered(AEVK_R))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
		return;
	}

	for (auto& tm : TileMaps)
	{
		tm.Update(dt);
	}

	m_Player.Update(dt);
	//m_MeleeEnemy.Update(dt);
	//m_MageEnemy.Update(dt);
	m_Boss.Update(dt);
	//m_FireWormEnemy.Update(dt);

	for (auto& spawn : m_Spawns)
	{
		spawn->Update(dt, m_Enemies);
	}

	for (auto enemy : m_Enemies)
	{
		enemy->Update(dt);
	}

	BossAIState currentBossState = m_Boss.GetCurrentAIState();
	if (currentBossState != m_previousBossAIState)
	{
		if (currentBossState == BossAIState::GLOWING)
		{
			m_pBossMessageTexture = LoadImageAsset("Assets/UI/healtext.png"); 
			m_bossMessageTimer = m_bossMessageDuration;
		}
		else if (currentBossState == BossAIState::BUFF)
		{
			m_pBossMessageTexture = LoadImageAsset("Assets/UI/enragetext.png");
			m_bossMessageTimer = m_bossMessageDuration;
		}
		m_previousBossAIState = currentBossState;
	}

	if (m_bossMessageTimer > 0.0f)
	{
		m_bossMessageTimer -= dt;
		if (m_bossMessageTimer <= 0.0f)
		{
			m_pBossMessageTexture = nullptr; 
		}
	}

	if (!m_Player.IsInvincible() && m_Player.GetHealth() > 0)
	{
		//std::vector<ACharacter*> enemies;
		//if (m_MeleeEnemy.GetHealth() > 0) enemies.push_back(&m_MeleeEnemy);
		//if (m_MageEnemy.GetHealth() > 0) enemies.push_back(&m_MageEnemy);
		//if (m_FireWormEnemy.GetHealth() > 0) enemies.push_back(&m_FireWormEnemy);
		//if (m_Boss.GetHealth() > 0 && m_Boss.IsAttackable()) enemies.push_back(&m_Boss);

		AEVec2 playerHitboxPos = m_Player.GetPosition();
		playerHitboxPos.x += m_Player.GetHitboxOffset().x;
		playerHitboxPos.y += m_Player.GetHitboxOffset().y;
		const AEVec2& playerHitboxSize = m_Player.GetHitboxSize();

		//for (auto* enemy : enemies)
		for (auto* enemy : m_Enemies)

		{
			if (CheckAABBCollision(playerHitboxPos, playerHitboxSize, enemy->GetPosition(), enemy->GetHitboxSize()))
			{
				std::cout << "collsion" << std::endl;
				m_Player.TakeDamage(1, DamageType::NONE); 
				break; 
			}
		}
	}

	if (m_Player.GetPosition().x > 0.f)
	{
		AEGfxSetCamPosition(m_Player.GetPosition().x, 0.f);
	}

	bool isPlayerCasting = m_Player.GetCurrentAnimState() == CharacterAnimationState::RANGED_ATTACK;
	bool canPlayerFire = m_Player.GetAnimation().GetCurrentFrame() == 4 && !m_Player.HasFiredProjectile();
	if (isPlayerCasting && canPlayerFire)
	{
		DamageType currentWeapon = m_Player.GetCurrentWeaponType();
		CharacterDirection playerDir = m_Player.GetDirection();
		AEVec2 directionVec = { (playerDir == CharacterDirection::RIGHT ? 1.0f : -1.0f), 0.0f };

		if (currentWeapon == DamageType::FIRE || currentWeapon == DamageType::ICE)
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
			newProjectile.Init(spawnPos, directionVec, projData);
			m_Player.ConsumeCurrentWeapon();
			m_Player.SetFiredProjectile(true);
		}
		else if (currentWeapon == DamageType::LIGHTNING)
		{
			ACharacter* target = FindClosestEnemyInFront();
			if (target)
			{
				target->TakeDamage(25, currentWeapon);
				m_visualEffects.emplace_back();
				VisualEffect& newEffect = m_visualEffects.back();
				newEffect.Init(target->GetPosition(), m_lightningEffectData);
			}
			m_Player.ConsumeCurrentWeapon();
			m_Player.SetFiredProjectile(true);
		}
	}

	for (auto enemy : m_Enemies)
	{
		RangedEnemyCharacter* rangeEnemy = dynamic_cast<RangedEnemyCharacter*>(enemy);
		if (rangeEnemy)
		{
			if (rangeEnemy->isReadytoFireRange())
			{

				m_enemyProjectiles.emplace_back();
				Projectile& newProjectile = m_enemyProjectiles.back();
				const ProjectileData& projData = rangeEnemy->GetProjectileData();
				AEVec2 directionVec = { (rangeEnemy->GetDirection() == CharacterDirection::RIGHT ? 1.0f : -1.0f), 0.0f };

				newProjectile.Init(rangeEnemy->GetPosition(), directionVec, projData);

				rangeEnemy->SetFiredProjectile(true);
			}
		}
	}


	bool isBossMeleeAttacking = m_Boss.GetCurrentAIState() == BossAIState::MELEE_ATTACK;
	bool isBossHitboxActive = m_Boss.GetAnimation().GetCurrentFrame() >= 3;
	if (isBossMeleeAttacking && isBossHitboxActive && !m_Boss.HasHitPlayerThisAttack() && m_Player.GetHealth() > 0)
	{
		const AttackHitbox& bossHitbox = m_Boss.GetCurrentMeleeHitbox();
		AEVec2 bossPos = m_Boss.GetPosition();
		CharacterDirection bossDir = m_Boss.GetDirection();
		AEVec2 bossHitboxPos;
		bossHitboxPos.x = bossPos.x + (bossDir == CharacterDirection::RIGHT ? bossHitbox.offset.x : -bossHitbox.offset.x);
		bossHitboxPos.y = bossPos.y + bossHitbox.offset.y;

		AEVec2 playerHitboxPos = m_Player.GetPosition();
		playerHitboxPos.x += m_Player.GetHitboxOffset().x;
		playerHitboxPos.y += m_Player.GetHitboxOffset().y;

		if (CheckAABBCollision(bossHitboxPos, bossHitbox.size, playerHitboxPos, m_Player.GetHitboxSize()))
		{
			m_Player.TakeDamage(15, DamageType::NONE);
			m_Boss.RegisterPlayerHit();
		}
	}

	bool isBossCastingRanged = m_Boss.GetCurrentAnimState() == CharacterAnimationState::RANGED_ATTACK;
	if (isBossCastingRanged && m_Boss.GetAnimation().IsFinished() && !m_Boss.HasFired())
	{
		m_Boss.SetFired(true);
		const ProjectileData& projData = m_Boss.GetProjectileData();
		AEVec2 bossPos = m_Boss.GetPosition();
		AEVec2 playerPos = m_Player.GetPosition();

		if (m_Boss.IsBuffed())
		{
			// 3-Way Shot
			float baseAngle = atan2(playerPos.y - bossPos.y, playerPos.x - bossPos.x);
			AEVec2 dirMid = { cosf(baseAngle), sinf(baseAngle) };
			AEVec2 dirUp = { cosf(baseAngle - 0.3f), sinf(baseAngle - 0.3f) };
			AEVec2 dirDown = { cosf(baseAngle + 0.3f), sinf(baseAngle + 0.3f) };

			m_enemyProjectiles.emplace_back().Init(bossPos, dirUp, projData);
			m_enemyProjectiles.emplace_back().Init(bossPos, dirMid, projData);
			m_enemyProjectiles.emplace_back().Init(bossPos, dirDown, projData);
		}
		else
		{
			AEVec2 finalDir = { (m_Boss.GetDirection() == CharacterDirection::RIGHT ? 1.f : -1.f), 0.f };
			float yDiff = playerPos.y - bossPos.y;
			if (yDiff > 150.f) finalDir.y = 0.5f;
			else if (yDiff < -150.f) finalDir.y = -0.5f;

			AEVec2Normalize(&finalDir, &finalDir);
			m_enemyProjectiles.emplace_back().Init(bossPos, finalDir, projData);
		}
	}

	bool isBossLaserAttacking = m_Boss.GetCurrentAIState() == BossAIState::LASER_BEAM;
	bool isLaserHitboxActive = m_Boss.GetLaserAnimation().GetCurrentFrame() >= 8;

	if (isBossLaserAttacking && isLaserHitboxActive && !m_Boss.IsUnbeatable() && m_Player.GetHealth() > 0)
	{
		const AttackHitbox& laserData = m_Boss.GetLaserHitbox();
		float offsetDir = (m_Boss.GetDirection() == CharacterDirection::RIGHT) ? 1.0f : -1.0f;
		AEVec2 laserBasePos = { m_Boss.GetPosition().x + (laserData.offset.x * offsetDir), m_Boss.GetPosition().y + laserData.offset.y };

		AEVec2 playerHitboxPos = m_Player.GetPosition();
		playerHitboxPos.x += m_Player.GetHitboxOffset().x;
		playerHitboxPos.y += m_Player.GetHitboxOffset().y;

		if (m_Boss.IsBuffed())
		{
			// 3-Way Laser
			float yOffsets[] = { 150.f, 0.f, -150.f };
			for (float yOffset : yOffsets)
			{
				AEVec2 currentLaserPos = { laserBasePos.x, laserBasePos.y + yOffset };
				if (CheckAABBCollision(currentLaserPos, laserData.size, playerHitboxPos, m_Player.GetHitboxSize()))
				{
					m_Player.TakeDamage(1, DamageType::NONE);
					break;
				}
			}
		}
		else
		{
			AEVec2 finalLaserPos = laserBasePos;
			float yDiff = m_Player.GetPosition().y - m_Boss.GetPosition().y;
			if (yDiff > 150.f) finalLaserPos.y += 100.f;
			else if (yDiff < -150.f) finalLaserPos.y -= 100.f;

			if (CheckAABBCollision(finalLaserPos, laserData.size, playerHitboxPos, m_Player.GetHitboxSize()))
			{
				m_Player.TakeDamage(1, DamageType::NONE);
			}
		}
	}

	for (auto proj = m_playerProjectiles.begin(); proj != m_playerProjectiles.end(); )
	{
		proj->Update(dt);
		for (auto enemy : m_Enemies)
		{
			if (proj->IsActive() && enemy->GetHealth() > 0)
			{
				if (CheckAABBCollision(proj->GetPosition(), proj->GetSize(), enemy->GetPosition(), enemy->GetHitboxSize()))
				{
					enemy->TakeDamage(proj->GetDamage(), proj->GetType());
					proj->Deactivate();
				}
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
		if (proj.IsActive() && m_Player.GetHealth() > 0)
		{
			AEVec2 playerHitboxPos = m_Player.GetPosition();
			playerHitboxPos.x += m_Player.GetHitboxOffset().x;
			playerHitboxPos.y += m_Player.GetHitboxOffset().y;

			if (CheckAABBCollision(proj.GetPosition(), proj.GetSize(), playerHitboxPos, m_Player.GetHitboxSize()))
			{
				m_Player.TakeDamage(proj.GetDamage(), DamageType::NONE);
				proj.Deactivate();
			}
		}
	}

	if (m_Player.IsMeleeAttackHitboxActive() && !m_Player.HasHitEnemyThisAttack())
	{
		const AttackHitbox& currentHitbox = m_Player.GetCurrentMeleeHitbox();
		const AEVec2& playerPos = m_Player.GetPosition();
		AEVec2 hitboxPos;
		hitboxPos.x = playerPos.x + (m_Player.GetDirection() == CharacterDirection::RIGHT ? currentHitbox.offset.x : -currentHitbox.offset.x);
		hitboxPos.y = playerPos.y + currentHitbox.offset.y;

		for (auto enemy : m_Enemies)
		{
			if ( enemy->GetHealth() > 0 && CheckAABBCollision(hitboxPos, currentHitbox.size, enemy->GetPosition(), enemy->GetHitboxSize()))
			{
				enemy->TakeDamage(10, DamageType::NONE);
				m_Player.RegisterHit();
			}
		}
	}

	for (auto& effect : m_visualEffects)
	{
		effect.Update(dt);
	}
}

void MainGameState::Draw()
{
	m_Background.Draw();
	//m_MeleeEnemy.Draw();
	//m_MageEnemy.Draw();
	//m_FireWormEnemy.Draw();

	for (auto tm : TileMaps)

	{
		tm.Draw();
	}

	//m_MeleeEnemy.Draw();
	//m_MageEnemy.Draw();

	for (auto enemy : m_Enemies)
	{
		enemy->Draw();
	}

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

	if (m_pBossMessageTexture)
	{
		float xCam, yCam;
		AEGfxGetCamPosition(&xCam, &yCam);

		float imgWidth = 500.f;
		float imgHeight = 100.f;
		float imgY = 200.f;

		DrawRect(xCam, imgY, imgWidth, imgHeight, 1.f, 1.f, 1.f, 1.f, m_pBossMessageTexture);
	}
}

void MainGameState::Exit()
{
	for (auto& tm : TileMaps)
	{
		tm.Destroy();
	}

	//for (auto enemy : m_Enemies)
	//{
	//	enemy->Destroy();
	//}

	TileMaps.clear();
	m_Background.Destroy();
	m_Player.Destroy();
	m_Boss.Destroy();
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

	DamageType currentWeapon = m_Player.GetCurrentWeaponType();

	char countStr[10];
	for (size_t i = 0; i < totalSlots; ++i)
	{
		float posX = startX + i * (slotSize + slotMargin) + xCam;
		DamageType slotWeaponType = availableWeapons[i];

		DrawRect(posX, posY, slotSize, slotSize, 1.f, 1.f, 1.f, 0.7f, m_pUiSlot);

		if (m_weaponIconMap.count(slotWeaponType))
		{
			DrawRect(posX, posY, slotSize * 0.8f, slotSize * 0.8f, 1.f, 1.f, 1.f, 1.f, m_weaponIconMap.at(slotWeaponType));
		}

		if (slotWeaponType == currentWeapon)
		{
			DrawHollowRect(posX, posY, slotSize, slotSize, 1.0f, 1.0f, 0.0f, 1.0f);
		}

		f32 TextScale = 0.5f;
		int useCount = m_Player.GetWeaponUseCount(slotWeaponType);
		sprintf_s(countStr, "%d", useCount);
		AEVec2 fontPos = GetNormalizedCoords(posX-xCam, posY);
		AEGfxPrint(GameManager::m_font, countStr, fontPos.x, fontPos.y, TextScale, 1, 1, 1, 1);
	}

	if (m_Boss.IsAttackable())
	{
		const float barWidth = 500.f;
		const float barHeight = 125.f;
		const float barX = 0;
		const float barY = kHalfWindowHeight - 150.f;
		DrawRect(barX + xCam, barY, barWidth, barHeight, 0.1f, 0.1f, 0.1f, 1.f);
		float healthRatio = static_cast<float>(m_Boss.GetHealth()) / m_Boss.getMaxHealth();
		float currentHealthWidth = barWidth * healthRatio;
		DrawRect(barX + xCam - (barWidth - currentHealthWidth) / 2.0f, barY, currentHealthWidth, barHeight, 1.0f, 0.0f, 0.0f, 1.f);
		DrawHollowRect(barX + xCam, barY, barWidth, barHeight, 1.f, 1.f, 0.f, 1.f);
	}
}

ACharacter* MainGameState::FindClosestEnemyInFront()
{
	ACharacter* closestEnemy = nullptr;
	float minDistance = FLT_MAX;

	AEVec2 playerPos = m_Player.GetPosition();
	CharacterDirection playerDir = m_Player.GetDirection();

	for (ACharacter* enemy : m_Enemies)
	{
		const AEVec2& enemyPos = enemy->GetPosition();

		bool isInFront = (playerDir == CharacterDirection::RIGHT && enemyPos.x > playerPos.x) ||
			(playerDir == CharacterDirection::LEFT && enemyPos.x < playerPos.x);

		const float yTolerance = m_Player.GetHitboxSize().y / 2.0f;
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