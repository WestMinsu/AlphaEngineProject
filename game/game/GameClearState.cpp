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
	sprintf_s(strBuffer, "Score: %d", GameManager::GetFinalScore());

	float outlineThickness = 0.005f;
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.3f + outlineThickness, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.3f - outlineThickness, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f + outlineThickness, -0.3f, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f - outlineThickness, -0.3f, 1.2f, 0.f, 0.f, 0.f, 1.f);

	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.3f, 1.2f, 1.f, 1.f, 1.f, 1.f);
}

void GameClearState::Exit()
{
}