#include "MainGameState.h"
#include "GameManager.h"
#include "Constants.h"
#include <iostream>
#include <algorithm>
#include "Utility.h"

MainGameState::MainGameState() {}
MainGameState::~MainGameState() {}

void MainGameState::Init()
{
	m_Player.Init({ -kHalfWindowWidth + 200.f, 0.f });
	m_MeleeEnemy.Init({ kHalfWindowWidth - 200.f, 0.f }, &m_Player);
	m_MageEnemy.Init({ kHalfWindowWidth - 500.f, 0.f }, &m_Player);
	m_TileMap.Init();
	m_Background.Init();
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
		newProjectile.Init(spawnPos, playerDir);
		m_Player.SetFiredProjectile(true);
	}

	if (m_MageEnemy.GetCurrentAnimState() == CharacterAnimationState::PROJECTILE_ATTACK &&
		m_MageEnemy.GetAnimation().GetCurrentFrame() == 15 &&
		!m_MageEnemy.HasFiredProjectile())
	{
		m_enemyProjectiles.emplace_back();
		EnemyProjectile& newProjectile = m_enemyProjectiles.back();
		newProjectile.Init(m_MageEnemy.GetPosition(), m_MageEnemy.GetDirection());
		m_MageEnemy.SetFiredProjectile(true);
	}

	for (auto it = m_playerProjectiles.begin(); it != m_playerProjectiles.end(); )
	{
		it->Update(dt);
		bool hit = false;
		if (it->IsActive() && m_MeleeEnemy.GetHealth() > 0)
		{
			if (CheckAABBCollision(it->GetPosition(), it->GetSize(), m_MeleeEnemy.GetPosition(), m_MeleeEnemy.GetSize()))
			{
				m_MeleeEnemy.TakeDamage(15);
				it->Deactivate();
				hit = true;
			}
		}
		if (!hit && it->IsActive() && m_MageEnemy.GetHealth() > 0)
		{
			if (CheckAABBCollision(it->GetPosition(), it->GetSize(), m_MageEnemy.GetPosition(), m_MageEnemy.GetSize()))
			{
				m_MageEnemy.TakeDamage(15);
				it->Deactivate();
			}
		}
		if (!it->IsActive()) { it->Destroy(); it = m_playerProjectiles.erase(it); }
		else { ++it; }
	}

	for (auto it = m_enemyProjectiles.begin(); it != m_enemyProjectiles.end(); )
	{
		it->Update(dt);
		if (it->IsActive() && m_Player.GetHealth() > 0)
		{
			if (CheckAABBCollision(it->GetPosition(), it->GetSize(), m_Player.GetPosition(), m_Player.GetSize()))
			{
				m_Player.TakeDamage(5);
				it->Deactivate();
			}
		}
		if (!it->IsActive()) { it->Destroy(); it = m_enemyProjectiles.erase(it); }
		else { ++it; }
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
}

void MainGameState::Draw()
{
	m_Background.Draw();
	m_TileMap.Draw();

	m_Player.Draw();
	m_MeleeEnemy.Draw();
	m_MageEnemy.Draw();

	for (auto& projectile : m_playerProjectiles) projectile.Draw();
	for (auto& projectile : m_enemyProjectiles) projectile.Draw();
}

void MainGameState::Exit()
{
	m_Player.Destroy();
	m_MeleeEnemy.Destroy();
	m_MageEnemy.Destroy();
	for (auto& projectile : m_playerProjectiles) projectile.Destroy();
	for (auto& projectile : m_enemyProjectiles) projectile.Destroy();
	m_playerProjectiles.clear();
	m_enemyProjectiles.clear();
}