#include "GameClearState.h"
#include "GameManager.h"
#include <cstdio>
#include "Utility.h"
#include "constants.h"

GameClearState::GameClearState() {}
GameClearState::~GameClearState() {}

void GameClearState::Init()
{
	GameManager::PlayBGM(BGMTrack::CLEAR);
}

void GameClearState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_ESCAPE))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
	}
}

void GameClearState::Draw()
{
	AEGfxTexture* gameClearTex = AEGfxTextureLoad("Assets/gameclear.png");
	DrawRect(0.f, 0.f, kWindowWidth, kWindowHeight, 1.f, 1.f, 1.f, 1.f, gameClearTex);

	char strBuffer[100];
	sprintf_s(strBuffer, "SCORE: %d", GameManager::GetFinalScore());
	AEGfxPrint(GameManager::m_font, strBuffer, -0.5f, -0.3f, 1.2f, 1.f, 1.f, 1.f, 1.f);
}

void GameClearState::Exit()
{
}