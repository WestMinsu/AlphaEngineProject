#pragma once
#include "AGameState.h"
#include "CharacterPlayer.h"
#include "CharacterMeleeEnemy.h"
#include "CharacterMageEnemy.h"
#include "Projectile.h"
#include "EnemyProjectile.h"
#include <vector>
#include <memory>
#include "TileMap.h"
#include "Background.h"

class MainGameState : public AGameState
{
public:
	MainGameState();
	~MainGameState();

	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;

private:
	CharacterPlayer m_Player;
	CharacterMeleeEnemy m_MeleeEnemy; 
	CharacterMageEnemy m_MageEnemy;

	std::vector<Projectile> m_playerProjectiles; 
	std::vector<EnemyProjectile> m_enemyProjectiles;

	TileMap m_TileMap;
	Background m_Background;
};