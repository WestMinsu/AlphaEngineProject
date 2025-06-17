#include "MainGameState.h"
#include "GameManager.h"
#include "Constants.h"
#include <iostream>

MainGameState::MainGameState() {}

MainGameState::~MainGameState() {}

void MainGameState::Init()
{
	m_Player.Init({ -kHalfWindowWidth + 200.f, 0.f });
	m_Enemy.Init({ kHalfWindowWidth - 200.f, 0.f });
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

	if (m_Player.IsAttackHitboxActive() && !m_Player.HasHitEnemyThisAttack())
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

			std::cout << "Enemy Hit! Remaining HP: " << m_Enemy.GetHealth() << std::endl;
		}
	}
}

void MainGameState::Draw()
{
	m_Enemy.Draw();
	m_Player.Draw();
}

void MainGameState::Exit()
{
	m_Player.Destroy();
	m_Enemy.Destroy();
}