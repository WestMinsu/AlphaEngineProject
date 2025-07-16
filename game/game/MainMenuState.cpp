#include "AGameState.h"
#include "GameManager.h"
#include "MainMenuState.h"
#include "Constants.h"
#include "Utility.h"
#include <string>

const f32 buttonWidth = 350.f;
const f32 buttonHeight = 100.f;

const f32 startButtonX = -kHalfWindowWidth / 2.0f;
const f32 startButtonY = kHalfWindowHeight / 4.0f;

const f32 leaderBoardButtonX = -kHalfWindowWidth / 2.0f;
const f32 leaderBoardButtonY = -kHalfWindowHeight / 5.0f;

const f32 exitButtonX = -kHalfWindowWidth / 2.0f;
const f32 exitButtonY = -kHalfWindowHeight / 1.5f;

MainMenuState::MainMenuState()
{
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::Init()
{
	GameManager::PlayBGM(BGMTrack::MENU);
}

void MainMenuState::Update(f32 dt)
{
	s32 pX, pY;
	AEInputGetCursorPosition(&pX, &pY);
	pX -= static_cast<s32>(kHalfWindowWidth);
	pY = static_cast<s32>(kHalfWindowHeight) - pY;
	if (AEInputCheckReleased(AEVK_LBUTTON))
	{
		if ((startButtonX - buttonWidth / 2 <= pX && pX <= startButtonX + buttonWidth / 2) && (startButtonY - buttonHeight / 2 <= pY && pY <= startButtonY + buttonHeight / 2))
			GameManager::ChangeState(GameState::MAIN_GAME);
		if ((leaderBoardButtonX - buttonWidth / 2 <= pX && pX <= leaderBoardButtonX + buttonWidth / 2) && (leaderBoardButtonY - buttonHeight / 2 <= pY && pY <= leaderBoardButtonY + buttonHeight / 2))
			GameManager::ChangeState(GameState::LEADERBOARD);
		if ((exitButtonX - buttonWidth / 2 <= pX && pX <= exitButtonX + buttonWidth / 2) && (exitButtonY - buttonHeight / 2 <= pY && pY <= exitButtonY + buttonHeight / 2))
			GameManager::m_isGameRunning = false;
	}
}

void MainMenuState::Draw()
{
	AEGfxTexture* mainMenuTex = AEGfxTextureLoad("Assets/title.jpg");
	DrawRect(0.f, 0.f, kWindowWidth, kWindowHeight, 1.f, 1.f, 1.f, 1.f, mainMenuTex);

	AEGfxSetBackgroundColor(0.1f, 0.1f, 0.1f);

	AEGfxTexture* startButtonTex = AEGfxTextureLoad("Assets/start.png");
	AEGfxTexture* leaderBoardButtonTex = AEGfxTextureLoad("Assets/leaderboard.png");
	AEGfxTexture* exitButtonTex = AEGfxTextureLoad("Assets/exit.png");
	DrawRect(startButtonX, startButtonY, buttonWidth, buttonHeight, 1, 1, 1, 1, startButtonTex);
	DrawRect(leaderBoardButtonX, leaderBoardButtonY, buttonWidth, buttonHeight, 1, 1, 1, 1, leaderBoardButtonTex);
	DrawRect(exitButtonX, exitButtonY, buttonWidth, buttonHeight, 1, 1, 1, 1, exitButtonTex);
}

void MainMenuState::Exit()
{
}
