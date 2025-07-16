#include "LeaderBoardState.h"
#include "GameManager.h"
#include <cstdio>
#include "Utility.h"
#include "Constants.h"

LeaderBoardState::LeaderBoardState() {}
LeaderBoardState::~LeaderBoardState() {}

void LeaderBoardState::Init()
{
}

void LeaderBoardState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_R) || AEInputCheckTriggered(AEVK_ESCAPE))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
	}
}

void LeaderBoardState::Draw()
{
	AEGfxTexture* leaderBoardTex = AEGfxTextureLoad("Assets/leaderboard.jpg");
	DrawRect(0.f, 0.f, kWindowWidth, kWindowHeight, 1.f, 1.f, 1.f, 1.f, leaderBoardTex);

	const auto& scores = GameManager::GetInstance()->GetScoreManager().GetScores();
	float yPos = 0.5f;
	char strBuffer[100];
	int rank = 1;

	float outlineThickness = 0.005f;
	for (const auto& entry : scores)
	{
		sprintf_s(strBuffer, "%d. %s - %d\n", rank, entry.playerName.c_str(), entry.score);

		AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, yPos + outlineThickness, 0.8f, 0.f, 0.f, 0.f, 1.f);
		AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, yPos - outlineThickness, 0.8f, 0.f, 0.f, 0.f, 1.f);
		AEGfxPrint(GameManager::m_font, strBuffer, -0.7f + outlineThickness, yPos, 0.8f, 0.f, 0.f, 0.f, 1.f);
		AEGfxPrint(GameManager::m_font, strBuffer, -0.7f - outlineThickness, yPos, 0.8f, 0.f, 0.f, 0.f, 1.f);
		AEGfxPrint(GameManager::m_font, strBuffer, -0.7f, yPos, 0.8f, 1.f, 1.f, 1.f, 1.f);
		yPos -= 0.12f;
		rank++;
	}
}

void LeaderBoardState::Exit() {}