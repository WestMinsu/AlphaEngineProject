#pragma once
#include "AGameState.h"
#include "PlayerCharacter.h"
#include "MeleeEnemyCharacter.h"
#include "MageEnemyCharacter.h"
#include "Projectile.h"
#include <vector>
#include <memory>
#include "TileMap.h"
#include "Background.h"
#include "BossCharacter.h"
#include "VisualEffect.h"

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
	void DrawUI();
	ACharacter* FindClosestEnemyInFront();
	 
	PlayerCharacter m_Player;
	MeleeEnemyCharacter m_MeleeEnemy; 
	MageEnemyCharacter m_MageEnemy;
	BossCharacter m_Boss;

	std::vector<Projectile> m_playerProjectiles; 
	std::vector<Projectile> m_enemyProjectiles;
	std::vector<VisualEffect> m_visualEffects;

	TileMap m_TileMap;
	std::vector<TileMap> m_TileMaps;

	Background m_Background;

	AEGfxTexture* m_pUiSlot;      
	std::map<WeaponType, AEGfxTexture*> m_weaponIconMap;
	AnimData m_lightningEffectData;

	AEGfxTexture* m_pHealthBarFrame;
	AEGfxTexture* m_pHealthBar;

	AEGfxTexture* m_pBossMessageTexture;
	float m_bossMessageTimer;
	const float m_bossMessageDuration = 2.5f;
	BossAIState m_previousBossAIState;
};