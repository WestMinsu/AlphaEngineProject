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

	m_isHoveringStart = (pX >= startButtonX - buttonWidth / 2 && pX <= startButtonX + buttonWidth / 2) &&
		(pY >= startButtonY - buttonHeight / 2 && pY <= startButtonY + buttonHeight / 2);

	m_isHoveringLeaderboard = (pX >= leaderBoardButtonX - buttonWidth / 2 && pX <= leaderBoardButtonX + buttonWidth / 2) &&
		(pY >= leaderBoardButtonY - buttonHeight / 2 && pY <= leaderBoardButtonY + buttonHeight / 2);

	m_isHoveringExit = (pX >= exitButtonX - buttonWidth / 2 && pX <= exitButtonX + buttonWidth / 2) &&
		(pY >= exitButtonY - buttonHeight / 2 && pY <= exitButtonY + buttonHeight / 2);

	if (AEInputCheckReleased(AEVK_LBUTTON))
	{
		if (m_isHoveringStart)
			GameManager::ChangeState(GameState::MAIN_GAME);
		else if (m_isHoveringLeaderboard)
			GameManager::ChangeState(GameState::LEADERBOARD);
		else if (m_isHoveringExit)
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

	if (m_isHoveringStart)
		DrawRect(startButtonX, startButtonY, buttonWidth, buttonHeight, 0.6, 0.6f, 0.6f, 1.f, startButtonTex);
	else
		DrawRect(startButtonX, startButtonY, buttonWidth, buttonHeight, 1.f, 1.f, 1.f, 1.f, startButtonTex);

	if (m_isHoveringLeaderboard)
		DrawRect(leaderBoardButtonX, leaderBoardButtonY, buttonWidth, buttonHeight, 0.6, 0.6f, 0.6f, 1.f, leaderBoardButtonTex);
	else
		DrawRect(leaderBoardButtonX, leaderBoardButtonY, buttonWidth, buttonHeight, 1.f, 1.f, 1.f, 1.f, leaderBoardButtonTex);

	if (m_isHoveringExit)
		DrawRect(exitButtonX, exitButtonY, buttonWidth, buttonHeight, 0.6, 0.6f, 0.6f, 1.f, exitButtonTex);
	else
		DrawRect(exitButtonX, exitButtonY, buttonWidth, buttonHeight, 1.f, 1.f, 1.f, 1.f, exitButtonTex);
}

void MainMenuState::Exit()
{
}
