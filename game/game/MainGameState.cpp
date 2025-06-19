#include "MainGameState.h"
#include "GameManager.h"
#include "Constants.h"
#include <iostream>
#include <algorithm>
#include "TileMap.h"
#include "AssetManager.h"

MainGameState::MainGameState()
{
}

MainGameState::~MainGameState() 
{
}

void MainGameState::Init()
{
	AEGfxSetCamPosition(0.f, 0.f);
	m_Player.Init({ -kHalfWindowWidth + 200.f, 0.f });
	m_Enemy.Init({ kHalfWindowWidth - 200.f, 0.f });

	m_TileMap0.Init("Assets/Maps/test0_32.tmj");
	std::cout << m_TileMap0.GetMapWidth()  << std::endl;
	m_TileMap1.Init("Assets/Maps/test1_32.tmj", m_TileMap0.GetMapWidth(), 0.f);
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
	m_Enemy.Update(dt);

	if (m_Player.GetPosition().x > 0.f)
	{
		AEGfxSetCamPosition(m_Player.GetPosition().x, 0.f);
	}

	if (m_Player.GetCurrentAnimState() == CharacterAnimationState::PROJECTILE_ATTACK &&
		m_Player.GetAnimation().GetCurrentFrame() == 4 &&
		!m_Player.HasFiredProjectile())
	{
		const AEVec2& playerPos = m_Player.GetPosition();
		const AEVec2& playerSize = m_Player.GetSize();
		CharacterDirection playerDir = m_Player.GetDirection();

		const f32 offsetX_Ratio = 0.3f;
		const f32 offsetY_Ratio = 0.1f;

		AEVec2 spawnPos;
		spawnPos.x = playerPos.x + (playerDir == CharacterDirection::RIGHT ? playerSize.x * offsetX_Ratio : -playerSize.x * offsetX_Ratio);
		spawnPos.y = playerPos.y - playerSize.y * offsetY_Ratio;

		m_projectiles.emplace_back();
		Projectile& newProjectile = m_projectiles.back();

		newProjectile.Init(spawnPos, playerDir);

		m_Player.SetFiredProjectile(true);
	}

	for (auto it = m_projectiles.begin(); it != m_projectiles.end(); )
	{
		it->Update(dt);

		bool destroyed = false;
		if (!it->IsActive()) {
			it->Destroy();
			it = m_projectiles.erase(it);
			destroyed = true;
		}

		if (!destroyed && m_Enemy.GetHealth() > 0)
		{
			AEVec2 projPos = it->GetPosition();
			AEVec2 projSize = it->GetSize();
			AEVec2 enemyPos = m_Enemy.GetPosition();
			AEVec2 enemySize = m_Enemy.GetSize();

			float pLeft = projPos.x - projSize.x / 2;
			float pRight = projPos.x + projSize.x / 2;
			float pTop = projPos.y + projSize.y / 2;
			float pBottom = projPos.y - projSize.y / 2;

			float eLeft = enemyPos.x - enemySize.x / 2;
			float eRight = enemyPos.x + enemySize.x / 2;
			float eTop = enemyPos.y + enemySize.y / 2;
			float eBottom = enemyPos.y - enemySize.y / 2;

			if (pRight > eLeft && pLeft < eRight && pTop > eBottom && pBottom < eTop)
			{
				m_Enemy.TakeDamage(15);
				it->Deactivate();
			}
		}

		if (!destroyed)
		{
			++it;
		}
	}

	if (m_Player.IsMeleeAttackHitboxActive() && !m_Player.HasHitEnemyThisAttack() && m_Enemy.GetHealth() > 0)
	{
		const AttackHitbox& currentHitbox = m_Player.GetCurrentAttackHitbox();
		AEVec2 playerPos = m_Player.GetPosition();
		AEVec2 hitboxPos;
		hitboxPos.x = playerPos.x + (m_Player.GetDirection() == CharacterDirection::RIGHT ? currentHitbox.offset.x : -currentHitbox.offset.x);
		hitboxPos.y = playerPos.y + currentHitbox.offset.y;

		AEVec2 hitboxSize = currentHitbox.size;
		AEVec2 enemyPos = m_Enemy.GetPosition();
		AEVec2 enemySize = m_Enemy.GetSize();

		float pLeft = hitboxPos.x - hitboxSize.x / 2;
		float pRight = hitboxPos.x + hitboxSize.x / 2;
		float pTop = hitboxPos.y + hitboxSize.y / 2;
		float pBottom = hitboxPos.y - hitboxSize.y / 2;

		float eLeft = enemyPos.x - enemySize.x / 2;
		float eRight = enemyPos.x + enemySize.x / 2;
		float eTop = enemyPos.y + enemySize.y / 2;
		float eBottom = enemyPos.y - enemySize.y / 2;

		if (pRight > eLeft && pLeft < eRight && pTop > eBottom && pBottom < eTop)
		{
			m_Enemy.TakeDamage(10);
			m_Player.RegisterHit();
		}
	}

	
	m_TileMap0.Update(dt);
	m_TileMap1.Update(dt);
}

void MainGameState::Draw()
{
	m_Background.Draw();
	m_TileMap0.Draw();
	m_TileMap1.Draw();
	m_Player.Draw();
	m_Enemy.Draw();
	for (auto& projectile : m_projectiles)
	{
		projectile.Draw();
	}
}

void MainGameState::Exit()
{
	m_TileMap0.Destroy();
	m_TileMap1.Destroy();
	m_Background.Destroy();
	m_Player.Destroy();
	m_Enemy.Destroy();
	for (auto& projectile : m_projectiles)
	{
		projectile.Destroy();
	}
	m_projectiles.clear();
	AEGfxSetCamPosition(0.f, 0.f);
}