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
#include "PlayerCharacter.h"

MainGameState::MainGameState()
{
	m_pUiSlot = nullptr;

	m_BossTutorialTexture = nullptr;
	m_isShowingBossTutorial = false;
}

MainGameState::~MainGameState() {}

void MainGameState::Init()
{
	m_isBossMusicPlaying = false;
	GameManager::PlayBGM(BGMTrack::STAGE);

	AEGfxSetCamPosition(0.f, 0.f);
	m_Player.Init({ -kHalfWindowWidth + 200.f, 0.f });

	TileMaps.push_back(TileMap("Assets/Maps/test0_32.tmj", 2.f));
	TileMaps.push_back(TileMap("Assets/Maps/test1_32.tmj", 2.f, TileMaps[0].GetMapTotalWidth()));
	TileMaps.push_back(TileMap("Assets/Maps/test2_32.tmj", 2.f, TileMaps[0].GetMapTotalWidth() * 6));

	m_stones.emplace_back().Init({ TileMaps[0].GetMapTotalWidth() * 6.f - kHalfWindowWidth + 50 * 32.f, -kHalfWindowHeight + 8.5f * 32.f }, "Assets/Boss/stone.PNG");
	m_stones.emplace_back().Init({ TileMaps[0].GetMapTotalWidth() * 6.f - kHalfWindowWidth + 5 * 32.f, -kHalfWindowHeight + 14.5f * 32.f }, "Assets/Boss/stone.PNG");
	m_stones.emplace_back().Init({ TileMaps[0].GetMapTotalWidth() * 6.f - kHalfWindowWidth + 17 * 32.f, -kHalfWindowHeight + 19.5f * 32.f }, "Assets/Boss/stone.PNG");
	m_stones.emplace_back().Init({ TileMaps[0].GetMapTotalWidth() * 6.f - kHalfWindowWidth + 29 * 32.f, -kHalfWindowHeight + 21.5f * 32.f }, "Assets/Boss/stone.PNG");
	m_stones.emplace_back().Init({ TileMaps[0].GetMapTotalWidth() * 6.f - kHalfWindowWidth + 43 * 32.f, -kHalfWindowHeight + 18.5f * 32.f }, "Assets/Boss/stone.PNG");

	m_Background.Init();
	WarriorEnemyCharacter* warrior = new WarriorEnemyCharacter();
	warrior->Init({ kHalfWindowWidth - 700.f, 0.f }, &m_Player);
	MageEnemyCharacter* mage = new MageEnemyCharacter();
	mage->Init({ kHalfWindowWidth - 550.f, 0.f }, &m_Player);
	FireWormEnemyCharacter* fire = new FireWormEnemyCharacter();
	fire->Init({ kHalfWindowWidth - 550.f, 0.f }, &m_Player);
	NightBorneEnemyCharacter* night = new NightBorneEnemyCharacter();
	night->Init({ kHalfWindowWidth - 550.f, 100.f }, &m_Player);
	BossCharacter* boss = new BossCharacter();
	boss->Init({ TileMaps[0].GetMapTotalWidth() * 7.f - kWindowWidth - boss->GetSize().x / 2.f, -150.f }, &m_Player);

	m_factory.RegisterPrototype("Warrior", warrior);
	m_factory.RegisterPrototype("Mage", mage);
	m_factory.RegisterPrototype("Fire", fire);
	m_factory.RegisterPrototype("Night", night);
	m_factory.RegisterPrototype("Boss", boss);

	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 20 * 32.f, -kHalfWindowHeight + 14 * 32.f }, &m_factory, "Warrior", 2));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 36 * 32.f, -kHalfWindowHeight + 9 * 32.f }, &m_factory, "Mage", 1));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 51 * 32.f, -kHalfWindowHeight + 16 * 32.f }, &m_factory, "Fire", 1));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 68 * 32.f, -kHalfWindowHeight + 17 * 32.f }, &m_factory, "Warrior", 2));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 80 * 32.f, -kHalfWindowHeight + 10 * 32.f }, &m_factory, "Mage", 1));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 96 * 32.f, -kHalfWindowHeight + 22 * 32.f }, &m_factory, "Fire", 1));
	m_Spawns.push_back(new SpawnEnemy({ -kHalfWindowWidth + 110 * 32.f, -kHalfWindowHeight + 16 * 32.f }, &m_factory, "Night", 1));

	m_SpawnBoss.Init( { TileMaps[0].GetMapTotalWidth() * 7.f - kWindowWidth - boss->GetSize().x/2.f + 100, -kHalfWindowHeight + 6 * 32.f}, &m_factory, "Boss", 1);

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
	
	m_feedbackText = "";
	m_feedbackTextTimer = 0.0f;

	m_feedbackTextR = 0.0f;
	m_feedbackTextG = 0.0f;
	m_feedbackTextB = 0.0f;

	m_isNextStage = false;
	m_isLeftLocked = false;
	m_clampCameraX = { 0, TileMaps[0].GetMapTotalWidth() * 2.f - kWindowWidth };
	m_currentClampCameraXLeft = m_clampCameraX.x;
	m_moveTileMapCount = 0;

	m_GoMessage = LoadImageAsset("Assets/UI/go.png");
	m_GoMessageTimer = 0.f;
	m_GoMessageSize = {200.f, 100.f};
	m_GoMessageOffset= {kHalfWindowWidth - m_GoMessageSize.x - 30, 0};

	m_borderTexture = LoadImageAsset("Assets/UI/UISlotBorders.png");
	m_scoreBorderTexture = LoadImageAsset("Assets/UI/ScoreBorders.png");
	m_BossTutorialTexture = LoadImageAsset("Assets/boss/bosstutorial.png"); 
	m_controlTexture = LoadImageAsset("Assets/UI/controlguide.png");
	m_helpTexture = LoadImageAsset("Assets/UI/help.png");
}

void MainGameState::Update(f32 dt)
{
	if (m_isHelpScreenVisible)
	{
		if (AEInputCheckTriggered(AEVK_F1))
		{
			m_isHelpScreenVisible = false;
		}
		return;
	}

	if (AEInputCheckTriggered(AEVK_F1))
	{
		m_isHelpScreenVisible = true;
		return; 
	}

	if (m_feedbackTextTimer > 0.0f)
	{
		m_feedbackTextTimer -= dt;
		if (m_feedbackTextTimer <= 0.0f)
		{
			m_feedbackText.clear();
		}
	}

	if (m_Bosses.size() > 0 && m_Bosses[0]->IsCompletelyDead())
	{
		GameManager::SetFinalScore(m_Player.GetScore());
		GameManager::ChangeState(GameState::GAME_CLEAR);
		return;
	}

	if (m_Player.IsCompletelyDead())
	{
		GameManager::SetFinalScore(m_Player.GetScore());
		GameManager::ChangeState(GameState::GAME_OVER);
		return;
	}

	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
		return;
	}

	for (auto& tm : TileMaps)
	{
		tm.Update(dt);
	}

	m_Player.Update(dt);

	for (auto spawn : m_Spawns)
	{
		spawn->Update(dt, m_Player.GetPosition(), m_moveTileMapCount, m_Enemies);
	}

	if (!m_isBossMusicPlaying && m_Bosses.size() > 0 && m_Bosses[0]->IsAttackable())
	{
		GameManager::PlayBGM(BGMTrack::BOSS);
		m_isBossMusicPlaying = true;
	}

	m_SpawnBoss.Update(dt, m_Player.GetPosition(), m_moveTileMapCount, m_Bosses);

	for (auto enemy : m_Enemies)
	{
		enemy->Update(dt);
	}

	if (m_Bosses.size() > 0)
	{
		m_Bosses[0]->Update(dt);
		m_Bosses[0]->GetBossPattern(m_pBossMessageTexture, m_bossMessageTimer, m_previousBossAIState);
		m_Bosses[0]->AttackMelee(m_Player);
		m_Bosses[0]->AttackRange(m_Player, m_enemyProjectiles);
		m_Bosses[0]->AttackLaser(m_Player);
		m_isShowingBossTutorial = true;
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
		AEVec2 playerHitboxPos = m_Player.GetPosition();
		playerHitboxPos.x += m_Player.GetHitboxOffset().x;
		playerHitboxPos.y += m_Player.GetHitboxOffset().y;
		const AEVec2& playerHitboxSize = m_Player.GetHitboxSize();

		for (auto* enemy : m_Enemies)
		{
			if (enemy->GetHealth() > 0 
				&& CheckAABBCollision(playerHitboxPos, playerHitboxSize, enemy->GetPosition(), enemy->GetHitboxSize()))
			{
				//std::cout << "collsion" << std::endl;
				m_Player.TakeDamage(2, DamageType::NONE);
				break;
			}
		}
	}

	bool allStonesCurrentlyDestroyed = true;

	if (m_isShowingBossTutorial)
	{
		for (const auto& stone : m_stones)
		{
			if (stone.IsActive())
			{
				allStonesCurrentlyDestroyed = false;
				break;
			}
		}
	}

	if (m_isShowingBossTutorial && allStonesCurrentlyDestroyed)
	{
		m_isShowingBossTutorial = false;
	}

	for (auto boss : m_Bosses)
	{
		m_Bosses[0]->SetStoneShield(!allStonesCurrentlyDestroyed);
	}

	if (allStonesCurrentlyDestroyed && !m_allStonesWereDestroyed)
	{
		m_stoneRespawnTimer = m_stoneRespawnTime;
		//std::cout << "All stones destroyed! Respawn timer started." << std::endl;
	}

	m_allStonesWereDestroyed = allStonesCurrentlyDestroyed;

	if (m_stoneRespawnTimer > 0.0f)
	{
		m_stoneRespawnTimer -= dt;
		if (m_stoneRespawnTimer <= 0.0f)
		{
			for (auto& stone : m_stones)
			{
				stone.Respawn(); 
			}
			m_stoneRespawnTimer = 0.0f; 
		}
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
			const f32 offsetX_Ratio = -0.2f;
			const f32 offsetY_Ratio = -0.15f;
			spawnPos.x = playerPos.x + (playerDir == CharacterDirection::RIGHT ? playerSize.x * offsetX_Ratio : -playerSize.x * offsetX_Ratio);
			spawnPos.y = playerPos.y + playerSize.y * offsetY_Ratio;
			const ProjectileData& projData = m_Player.GetCurrentProjectileData();
			newProjectile.Init(spawnPos, directionVec, projData);
			m_Player.ConsumeCurrentWeapon();
			m_Player.SetFiredProjectile(true);
		}
		else if (currentWeapon == DamageType::LIGHTNING)
		{
			ACharacter* enemy = FindClosestEnemyInFront();
			if (enemy)
			{
				bool wasAlive = !enemy->IsDead();
				enemy->TakeDamage(25, currentWeapon);
				if (wasAlive && enemy->IsDead())
				{
					m_Player.AddScore(enemy->GetKillScore());
				}
				m_visualEffects.emplace_back();
				VisualEffect& newEffect = m_visualEffects.back();
				newEffect.Init(enemy->GetPosition(), m_lightningEffectData);
			}
			m_Player.ConsumeCurrentWeapon();
			m_Player.SetFiredProjectile(true);
		}
	}

	for (auto* enemy : m_Enemies)
	{
		MeleeEnemyCharacter* meleeEnemy = dynamic_cast<MeleeEnemyCharacter*>(enemy);
		if (meleeEnemy && meleeEnemy->GetHealth() > 0 && m_Player.GetHealth() > 0 && !m_Player.IsInvincible())
		{
			if (meleeEnemy->IsAttackHitboxActive() && !meleeEnemy->HasHitPlayerThisAttack())
			{
				const AttackHitbox& enemyHitbox = meleeEnemy->GetCurrentMeleeHitbox();
				AEVec2 enemyPos = meleeEnemy->GetPosition();
				CharacterDirection enemyDir = meleeEnemy->GetDirection();
				AEVec2 hitboxPos;
				hitboxPos.x = enemyPos.x + (enemyDir == CharacterDirection::RIGHT ? enemyHitbox.offset.x : -enemyHitbox.offset.x);
				hitboxPos.y = enemyPos.y + enemyHitbox.offset.y;

				AEVec2 playerHitboxPos = m_Player.GetPosition();
				playerHitboxPos.x += m_Player.GetHitboxOffset().x;
				playerHitboxPos.y += m_Player.GetHitboxOffset().y;

				if (CheckAABBCollision(hitboxPos, enemyHitbox.size, playerHitboxPos, m_Player.GetHitboxSize()))
				{
					m_Player.TakeDamage(10, DamageType::NONE);
					meleeEnemy->RegisterPlayerHit();
				}
			}
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
				AEVec2 spawnPos = rangeEnemy->GetPosition();
				spawnPos.x += rangeEnemy->GetProjectileSpawnOffset().x;
				spawnPos.y += rangeEnemy->GetProjectileSpawnOffset().y;
				//std::cout << rangeEnemy->GetProjectileSpawnOffset().y << " " << spawnPos.y << std::endl;
				newProjectile.Init(spawnPos, directionVec, projData);

				rangeEnemy->SetFiredProjectile(true);
			}
		}
	}

	UpdateCamera();
	f32 xCam, yCam;
	AEGfxGetCamPosition(&xCam, &yCam);

	//std::cout << "CAM Clamp: " << m_clampCameraX.x << ", " << m_clampCameraX.y << std::endl;

	if (isAllEnemiesDead() 
		&& m_isNextStage)
	{
		for (auto enemy : m_Enemies )
		{
			delete enemy;
		}
		m_Enemies.clear();
		m_GoMessageTimer = 0.f;

		if (m_isNextStage)
		{
			if (m_moveTileMapCount < 2)
			{
				m_clampCameraX.y += TileMaps[0].GetMapTotalWidth() * 2.f;
			}
			else
			{
				m_clampCameraX.y += TileMaps[0].GetMapTotalWidth();
			}
			m_clampCameraX.x += TileMaps[0].GetMapTotalWidth() * 2.f;
			m_moveTileMapCount++;
			m_isNextStage = false;
		}
	}

	for (auto proj = m_playerProjectiles.begin(); proj != m_playerProjectiles.end(); )
	{
		proj->Update(dt);

		AEVec2 projHitboxPos = proj->GetPosition();
		float dirMultiplier = (proj->GetVelocity().x >= 0) ? 1.0f : -1.0f;
		projHitboxPos.x += proj->GetHitboxOffset().x * dirMultiplier;
		projHitboxPos.y += proj->GetHitboxOffset().y;
		const AEVec2& projHitboxSize = proj->GetHitboxSize();

		bool hit = false;
		for (auto& stone : m_stones)
		{
			if (stone.IsActive() && CheckAABBCollision(proj->GetPosition(), proj->GetSize(), stone.GetPosition(), stone.GetSize()))
			{
				stone.TakeDamage(proj->GetDamage());
				proj->Deactivate();
				hit = true;
				break;
			}
		}
		if (hit) 
			continue;

		for (auto boss : m_Bosses)
		{
			AEVec2 enemyHitboxPos = boss->GetPosition();
			enemyHitboxPos.x += boss->GetHitboxOffset().x;	
			enemyHitboxPos.y += boss->GetHitboxOffset().y;
			if (CheckAABBCollision(projHitboxPos, projHitboxSize, enemyHitboxPos, boss->GetHitboxSize()))
			{
				bool wasAlive = !boss->IsDead();
				boss->TakeDamage(proj->GetDamage(), proj->GetType());
				if (wasAlive && boss->IsDead())
				{
					m_Player.AddScore(boss->GetKillScore());
				}
				proj->Deactivate();
				break;
			}
		}

		for (auto enemy : m_Enemies)
		{
			if (proj->IsActive() && enemy->GetHealth() > 0)
			{
				AEVec2 enemyHitboxPos = enemy->GetPosition();
				enemyHitboxPos.x += enemy->GetHitboxOffset().x;
				enemyHitboxPos.y += enemy->GetHitboxOffset().y;
				if (CheckAABBCollision(projHitboxPos, projHitboxSize, enemyHitboxPos, enemy->GetHitboxSize()))
				{
					switch (enemy->GetElement())
					{
					case ElementType::FIRE:
						if (proj->GetType() == DamageType::FIRE)
						{
							m_feedbackText = "Immune to fire";
							m_feedbackTextTimer = 1.0f;
							m_feedbackTextPos = GetNormalizedCoords(enemy->GetPosition().x-xCam, enemy->GetPosition().y);
							m_feedbackTextR = 1.0f;
							m_feedbackTextG = 0.0f;
							m_feedbackTextB = 0.0f;
						}
						else
						{
							bool wasAlive = !enemy->IsDead();
							enemy->TakeDamage(proj->GetDamage(), proj->GetType());
							if (wasAlive && enemy->IsDead())
							{
								m_Player.AddScore(enemy->GetKillScore());
							}
						}
						break;
					case ElementType::ICE:
						if (proj->GetType() == DamageType::ICE)
						{
							m_feedbackText = "Immune to ice";
							m_feedbackTextTimer = 1.0f;
							m_feedbackTextPos = GetNormalizedCoords(enemy->GetPosition().x - xCam, enemy->GetPosition().y);
							m_feedbackTextR = 0.0f;
							m_feedbackTextG = 0.0f;
							m_feedbackTextB = 1.0f;
						}
						else
						{
							bool wasAlive = !enemy->IsDead();
							enemy->TakeDamage(proj->GetDamage(), proj->GetType());
							if (wasAlive && enemy->IsDead())
							{
								m_Player.AddScore(enemy->GetKillScore());
							}
						}
						break;
					case ElementType::DARK:
						if (proj->GetType() == DamageType::FIRE)
						{
							m_feedbackText = "Immune to fire";
							m_feedbackTextTimer = 1.0f;
							m_feedbackTextPos = GetNormalizedCoords(enemy->GetPosition().x - xCam, enemy->GetPosition().y);
							m_feedbackTextR = 1.0f;
							m_feedbackTextG = 0.0f;
							m_feedbackTextB = 0.0f;
						}
						else if (proj->GetType() == DamageType::ICE)
						{
							m_feedbackText = "Immune to ice";
							m_feedbackTextTimer = 1.0f;
							m_feedbackTextPos = GetNormalizedCoords(enemy->GetPosition().x - xCam, enemy->GetPosition().y);
							m_feedbackTextR = 0.0f;
							m_feedbackTextG = 0.0f;
							m_feedbackTextB = 1.0f;
						}
						else
						{
							bool wasAlive = !enemy->IsDead();
							enemy->TakeDamage(proj->GetDamage(), proj->GetType());
							if (wasAlive && enemy->IsDead())
							{
								m_Player.AddScore(enemy->GetKillScore());
							}
						}
						break;
					default:
						bool wasAlive = !enemy->IsDead();
						enemy->TakeDamage(proj->GetDamage(), proj->GetType());
						if (wasAlive && enemy->IsDead())
						{
							m_Player.AddScore(enemy->GetKillScore());
						}
						break;
					}
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

		bool hit = false;
		for (auto& stone : m_stones)
		{
			if (stone.IsActive() && CheckAABBCollision(hitboxPos, currentHitbox.size, stone.GetPosition(), stone.GetSize()))
			{
				stone.TakeDamage(m_Player.GetMeleeAttackDamage());
				m_Player.RegisterHit();
				hit = true;
				break;
			}
		}

		if (!hit)
		{
			for (auto enemy : m_Enemies)
			{
				AEVec2 enemyHitboxPos = enemy->GetPosition();
				enemyHitboxPos.x += enemy->GetHitboxOffset().x;
				enemyHitboxPos.y += enemy->GetHitboxOffset().y;
				if (enemy->GetHealth() > 0 && CheckAABBCollision(hitboxPos, currentHitbox.size, enemyHitboxPos, enemy->GetHitboxSize()))
				{
					bool wasAlive = !enemy->IsDead();
					enemy->TakeDamage(m_Player.GetMeleeAttackDamage(), DamageType::NONE);
					if (wasAlive && enemy->IsDead())
					{
						m_Player.AddScore(enemy->GetKillScore());
					}
					m_Player.RegisterHit();
					break;
				}
			}

			for (auto enemy : m_Bosses)
			{
				AEVec2 enemyHitboxPos = enemy->GetPosition();
				enemyHitboxPos.x += enemy->GetHitboxOffset().x;
				enemyHitboxPos.y += enemy->GetHitboxOffset().y;
				if (enemy->GetHealth() > 0 && CheckAABBCollision(hitboxPos, currentHitbox.size, enemyHitboxPos, enemy->GetHitboxSize()))
				{
					bool wasAlive = !enemy->IsDead();
					enemy->TakeDamage(m_Player.GetMeleeAttackDamage(), DamageType::NONE);
					if (wasAlive && enemy->IsDead())
					{
						m_Player.AddScore(enemy->GetKillScore());
					}
					m_Player.RegisterHit();
					break;
				}
			}
		}
	}

	for (auto& effect : m_visualEffects)
	{
		effect.Update(dt);
	}

	m_GoMessageTimer += dt;
}

void MainGameState::Draw()
{
	m_Background.Draw();

	for (auto& tm : TileMaps)
	{
		tm.Draw();
	}

	for (auto enemy : m_Enemies)
	{
		enemy->Draw();
	}

	for (auto& stone : m_stones)
	{
		stone.Draw();
	}

	if ( m_Bosses.size() > 0 && !m_Bosses[0]->IsCompletelyDead())
	{
		m_Bosses[0]->Draw();
	}

	m_Player.Draw();

	for (auto& projectile : m_playerProjectiles)
		projectile.Draw();
	for (auto& projectile : m_enemyProjectiles)
		projectile.Draw();
	for (auto& effect : m_visualEffects)
		effect.Draw();

	DrawUI();

	if (m_feedbackTextTimer > 0.0f)
	{
		AEGfxPrint(GameManager::m_font, m_feedbackText.c_str(), m_feedbackTextPos.x, m_feedbackTextPos.y, 0.3f, m_feedbackTextR, m_feedbackTextG, m_feedbackTextB, 1.f);
		//std::cout << m_feedbackTextPos.x << m_feedbackTextPos.y << std::endl;
	}

	float xCam, yCam;
	AEGfxGetCamPosition(&xCam, &yCam);
	if (m_pBossMessageTexture)
	{
		float imgWidth = 700.f;
		float imgHeight = 100.f;
		float imgY = 200.f;

		DrawRect(xCam, imgY, imgWidth, imgHeight, 1.f, 1.f, 1.f, 1.f, m_pBossMessageTexture);
	}

	if (m_isShowingBossTutorial)
	{
		float xCam, yCam;
		AEGfxGetCamPosition(&xCam, &yCam);

		float imgWidth = 400.f;
		float imgHeight = 150.f;
		float imgY = 300.f;

		DrawRect(xCam, imgY, imgWidth, imgHeight, 1.f, 1.f, 1.f, 1.f, m_BossTutorialTexture);
	}

	DrawRect(0, 300, 700, 100, 1.f, 1.f, 1.f, 1.f, m_helpTexture);

	if (m_isHelpScreenVisible)
	{
		float xCam, yCam;
		AEGfxGetCamPosition(&xCam, &yCam);

		DrawRect(xCam, yCam, kWindowWidth, kWindowHeight, 0.f, 0.f, 0.f, 0.7f);
		DrawRect(xCam, yCam, 800, 800, 1.f, 1.f, 1.f, 1.f, m_controlTexture);
	}
}

void MainGameState::Exit()
{
	for (auto& tm : TileMaps)
	{
		tm.Destroy();
	}

	TileMaps.clear();
	m_Background.Destroy();
	m_Player.Destroy();

	for (auto spawn : m_Spawns)
	{
		spawn->Destroy();
	}

	m_Spawns.clear();
	m_SpawnBoss.Destroy();
	m_factory.Destroy();

	// To Do
	if (m_Bosses.size() > 0)
	{
		m_Bosses[0]->Destroy();
	}

	for (auto& projectile : m_playerProjectiles) projectile.Destroy();
	for (auto& projectile : m_enemyProjectiles) projectile.Destroy();
	m_playerProjectiles.clear();
	m_enemyProjectiles.clear();

	AEGfxSetCamPosition(0.f, 0.f);
}

void MainGameState::UpdateCamera()
{
	f32 xCam, yCam;
	AEGfxGetCamPosition(&xCam, &yCam);

	if (m_Player.GetPosition().x > m_clampCameraX.x && m_currentClampCameraXLeft >= m_clampCameraX.x)
	{
		m_isLeftLocked = false;
	}
	else if (m_moveTileMapCount > 0)
	{
		if (!m_isLeftLocked)
		{
			m_isLeftLocked = true;
		}
		m_currentClampCameraXLeft = xCam;
	}

	float leftClamp = m_isLeftLocked ? m_currentClampCameraXLeft : m_clampCameraX.x;
	//if (m_Player.GetPosition().x > m_clampCameraX.x)
	//{
	//	xCam = MoveInterpolation(xCam, m_Player.GetPosition().x, 0.1f);
	//}
	//else {
	//	xCam = MoveInterpolation(xCam, m_Player.GetPosition().x, 0.5f);
	//}
	xCam = MoveInterpolation(xCam, m_Player.GetPosition().x, 0.2f);
	xCam = std::clamp(xCam, leftClamp, m_clampCameraX.y);
	AEGfxSetCamPosition(xCam, 0.f);
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
		AEVec2 fontPos = GetNormalizedCoords(posX - xCam, posY);
		AEGfxPrint(GameManager::m_font, countStr, fontPos.x, fontPos.y, TextScale, 1, 1, 1, 1);
	}


	DrawRect(xCam, posY, slotSize*3.5f, slotSize*1.2f, 1.f, 1.f, 1.f, 1.f, m_borderTexture);


	f32 TextScale = 0.5f;
	char scoreBuffer[100];
	sprintf_s(scoreBuffer, "SCORE: %d", m_Player.GetScore());
	AEVec2 fontPos = GetNormalizedCoords(kHalfWindowWidth * 0.5f, kHalfWindowHeight - 70.f);
	AEGfxPrint(GameManager::m_scoreFont, scoreBuffer, fontPos.x, fontPos.y, TextScale, 1.f, 0.5f, 0.f, 1);

	DrawRect(xCam + kHalfWindowWidth * 0.7f, kHalfWindowHeight - 55.f, 400, 80, 1.f, 1.f, 1.f, 1.f, m_scoreBorderTexture);

	if (m_Bosses.size() > 0)
	{
		m_Bosses[0]->DrawBossHPUI();
	}

	if (m_GoMessageTimer < 3.f && m_GoMessageTimer > 0.f)
	{
		f32 sizeRatio = (int)(1 + m_GoMessageTimer) - m_GoMessageTimer;
		DrawRect(xCam + m_GoMessageOffset.x + m_GoMessageSize.x * (1.f - sizeRatio), yCam + m_GoMessageOffset.y, m_GoMessageSize.x * sizeRatio, m_GoMessageSize.y, 1.f, 1.f, 1.f, 1.f, m_GoMessage);
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
		if (enemy->GetHealth() <= 0)
			continue;
		
		const AEVec2& enemyPos = enemy->GetPosition();

		bool isInFront = (playerDir == CharacterDirection::RIGHT && enemyPos.x > playerPos.x) ||
			(playerDir == CharacterDirection::LEFT && enemyPos.x < playerPos.x);

		const float yTolerance = m_Player.GetHitboxSize().y * 2;
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

	for (ACharacter* enemy : m_Bosses)
	{
		if (enemy->GetHealth() <= 0)
			continue;

		const AEVec2& enemyPos = enemy->GetPosition();

		bool isInFront = (playerDir == CharacterDirection::RIGHT && enemyPos.x > playerPos.x) ||
			(playerDir == CharacterDirection::LEFT && enemyPos.x < playerPos.x);

		const float yTolerance = m_Player.GetHitboxSize().y;
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

bool MainGameState::isAllEnemiesDead()
{
	bool result = true;

	if (m_Enemies.size() > 0)
	{
		for (ACharacter* enemy : m_Enemies)
		{
			if (enemy->GetHealth() > 0)
			{
				result = false;
				break;
			}
		}
	}
	else
	{
		result = false;
	}
	
	if (result)
	{
		m_isNextStage = true;
	}

	return result;
}
