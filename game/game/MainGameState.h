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
	AEVec2 m_oldPositionPlayer;
	CharacterEnemy m_Enemy;
	std::vector<Projectile> m_projectiles;
	std::vector<TileMap> m_TileMaps;
	Background m_Background;
};