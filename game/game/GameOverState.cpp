#include "GameOverState.h"
#include "GameManager.h"
#include <cstdio>

GameOverState::GameOverState() {}
GameOverState::~GameOverState() {}

void GameOverState::Init() 
{
	GameManager::PlayBGM(BGMTrack::OVER);
}

void GameOverState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_R))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
	}
}

void GameOverState::Draw()
{
	char strBuffer[100];
	sprintf_s(strBuffer, "GAME OVER");
	AEGfxPrint(GameManager::m_font, strBuffer, -0.3f, 0, 1.2f, 1, 1, 1, 1);
}

void GameOverState::Exit() {}