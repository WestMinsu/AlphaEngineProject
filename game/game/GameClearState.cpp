#include "GameClearState.h"
#include "GameManager.h"
#include <cstdio>

GameClearState::GameClearState() {}
GameClearState::~GameClearState() {}

void GameClearState::Init()
{
}

void GameClearState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_R))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
	}
}

void GameClearState::Draw()
{
	char strBuffer[100];
	sprintf_s(strBuffer, "GAME CLEAR");
	AEGfxPrint(GameManager::m_font, strBuffer, -0.3f, 0, 1.2f, 1, 1, 1, 1);
}

void GameClearState::Exit()
{
}