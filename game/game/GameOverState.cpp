#include "GameOverState.h"
#include "GameManager.h"
#include <cstdio>
#include "Utility.h"
#include "Constants.h"

GameOverState::GameOverState() {}
GameOverState::~GameOverState() {}

void GameOverState::Init() 
{
	GameManager::PlayBGM(BGMTrack::OVER);
}

void GameOverState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
	}
}

void GameOverState::Draw()
{
	AEGfxTexture* gameOverTex = AEGfxTextureLoad("Assets/gameover.png");
	DrawRect(0.f, 0.f, kWindowWidth, kWindowHeight, 1.f, 1.f, 1.f, 1.f, gameOverTex);
	
	char strBuffer[100];
	sprintf_s(strBuffer, "SCORE: %d", GameManager::GetFinalScore());
	AEGfxPrint(GameManager::m_font, strBuffer, -0.3f, -0.3f, 1.2f, 1.f, 1.f, 1.f, 1.f);
}

void GameOverState::Exit() {}