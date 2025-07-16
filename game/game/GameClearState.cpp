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
	m_playerName = "";
}

void GameClearState::Update(f32 dt)
{
	for (char c = 'A'; c <= 'Z'; ++c)
	{
		if (AEInputCheckTriggered(c))
		{
			if (m_playerName.length() < 10) // 최대 10글자
				m_playerName += c;
		}
	}
	if (AEInputCheckTriggered(AEVK_BACK))
	{
		if (!m_playerName.empty())
			m_playerName.pop_back();
	}
	if (AEInputCheckTriggered(AEVK_RETURN))
	{
		if (m_playerName.empty())
			m_playerName = "PLAYER";

		GameManager::GetInstance()->GetScoreManager().AddScore(m_playerName, GameManager::GetFinalScore());
		GameManager::ChangeState(GameState::MAIN_MENU);
	}
	//if (AEInputCheckTriggered(AEVK_ESCAPE))
	//{
	//	GameManager::ChangeState(GameState::MAIN_MENU);
	//}
}

void GameClearState::Draw()
{
	AEGfxTexture* gameClearTex = AEGfxTextureLoad("Assets/gameclear.jpg");
	DrawRect(0.f, 0.f, kWindowWidth, kWindowHeight, 1.f, 1.f, 1.f, 1.f, gameClearTex);

	char strBuffer[100];
	sprintf_s(strBuffer, "Score: %d", GameManager::GetFinalScore());

	float outlineThickness = 0.005f;
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.1f + outlineThickness, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.1f - outlineThickness, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f + outlineThickness, -0.1f, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f - outlineThickness, -0.1f, 1.2f, 0.f, 0.f, 0.f, 1.f);

	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.1f, 1.2f, 1.f, 1.f, 1.f, 1.f);

	sprintf_s(strBuffer, "Enter Your Name: %s_", m_playerName.c_str());
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.3f + outlineThickness, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.3f - outlineThickness, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f + outlineThickness, -0.3f, 1.2f, 0.f, 0.f, 0.f, 1.f);
	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f - outlineThickness, -0.3f, 1.2f, 0.f, 0.f, 0.f, 1.f);

	AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, -0.3f, 1.2f, 1.f, 1.f, 1.f, 1.f);
}

void GameClearState::Exit()
{
}