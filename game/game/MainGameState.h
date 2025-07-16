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
#include "FireWormEnemyCharacter.h"
#include "EnemyFactory.h"
#include "SpawnEnemy.h"
#include "WarriorEnemyCharacter.h"
#include "NightBorneEnemyCharacter.h"
#include "DestructibleObject.h"

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
	PlayerCharacter m_Player;

	std::vector<ACharacter*> m_Enemies;
	std::vector<BossCharacter*> m_Bosses;

	std::vector<Projectile> m_playerProjectiles; 
	std::vector<Projectile> m_enemyProjectiles;
	std::vector<VisualEffect> m_visualEffects;
	std::vector<DestructibleObject> m_stones;

	Background m_Background;
	EnemyFactory m_factory;
	std::vector<SpawnEnemy*> m_Spawns;
	SpawnEnemy m_SpawnBoss;

	bool m_isNextStage;
	bool m_isLeftLocked;
	AEVec2 m_clampCameraX;
	f32 m_currentClampCameraXLeft;
	s32 m_moveTileMapCount;

	AEGfxTexture* m_pUiSlot;      
	std::map<DamageType, AEGfxTexture*> m_weaponIconMap;
	AnimData m_lightningEffectData;

	AEGfxTexture* m_pBossMessageTexture;
	float m_bossMessageTimer;
	BossAIState m_previousBossAIState;

	std::string m_feedbackText;
	float m_feedbackTextTimer;
	AEVec2 m_feedbackTextPos;

	AEGfxTexture* m_pGoMessage;
	float m_GoMessageTimer;
	AEVec2 m_GoMessageSize;
	AEVec2 m_GoMessageOffset;

	f32 m_feedbackTextR;
	f32 m_feedbackTextG;
	f32 m_feedbackTextB;

	AEGfxTexture* m_borderTexture;
	AEGfxTexture* m_scoreBorderTexture;

	bool m_isBossMusicPlaying;

	// private Functions
	void UpdateCamera();
	void DrawUI();
	ACharacter* FindClosestEnemyInFront();
	bool isAllEnemiesDead();

	float m_stoneRespawnTimer;
	const float m_stoneRespawnTime = 15.0f;
	bool m_allStonesWereDestroyed;
};