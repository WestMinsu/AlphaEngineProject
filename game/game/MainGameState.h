#pragma once
#include "AGameState.h"
#include "CharacterPlayer.h"
#include "CharacterEnemy.h"
#include "Projectile.h"
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
	CharacterEnemy m_Enemy;
	std::vector<Projectile> m_projectiles;
	TileMap m_TileMap0;
	TileMap m_TileMap1;
	Background m_Background;
};