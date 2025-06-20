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
	void DrawUI();
	 
	CharacterPlayer m_Player;
	CharacterMeleeEnemy m_MeleeEnemy; 
	CharacterMageEnemy m_MageEnemy;

	std::vector<Projectile> m_playerProjectiles; 
	std::vector<Projectile> m_enemyProjectiles;

	TileMap m_TileMap;
	Background m_Background;

	AEGfxTexture* m_pUiSlot;      
	AEGfxTexture* m_pFireIcon;    
	AEGfxTexture* m_pIceIcon;
	std::map<WeaponType, AEGfxTexture*> m_weaponIconMap;
};