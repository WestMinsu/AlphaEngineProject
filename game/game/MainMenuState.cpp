#include "AGameState.h"
#include "GameManager.h"
#include "MainMenuState.h"
#include "Constants.h"
#include "Utility.h"
#include <string>

const f32 buttonWidth = 350.f;
const f32 buttonHeight = 100.f;

const f32 startButtonX = 0.f;
const f32 startButtonY = 200.f;

const f32 exitButtonX = 0.f;
const f32 exitButtonY = -200.f;


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
		if ((exitButtonX - buttonWidth / 2 <= pX && pX <= exitButtonX + buttonWidth / 2) && (exitButtonY - buttonHeight / 2 <= pY && pY <= exitButtonY + buttonHeight / 2))
			GameManager::m_isGameRunning = false;
	}

}

void MainMenuState::Draw()
{
	AEGfxSetBackgroundColor(0.1f, 0.1f, 0.1f);
	DrawRect(startButtonX, startButtonY, buttonWidth, buttonHeight, 0.5, 0.5, 0.5, 1);
	DrawRect(exitButtonX, exitButtonY, buttonWidth, buttonHeight, 0.5, 0.5, 0.5, 1);

	std::string pongButtonText = "Start";
	std::string animationButtonText = "Exit";
	f32 TextScale = 0.9f;

	f32 pongButtonTextWidth, pongButtonTextHeight;
	AEGfxGetPrintSize(GameManager::m_font, pongButtonText.c_str(), TextScale, &pongButtonTextWidth, &pongButtonTextHeight);
	f32 animationButtonTextWidth, animationButtonTextHeight;
	AEGfxGetPrintSize(GameManager::m_font, animationButtonText.c_str(), TextScale, &animationButtonTextWidth, &animationButtonTextHeight);

	AEVec2 pongTextCoordinate = GetNormalizedCoords(startButtonX, startButtonY);
	AEVec2 animationTextCoordinate = GetNormalizedCoords(exitButtonX, exitButtonY);

	AEGfxPrint(GameManager::m_font, pongButtonText.c_str(), pongTextCoordinate.x - pongButtonTextWidth / 2, pongTextCoordinate.y - pongButtonTextHeight / 2, TextScale, 1, 1, 1, 1);
	AEGfxPrint(GameManager::m_font, animationButtonText.c_str(), animationTextCoordinate.x - animationButtonTextWidth / 2, animationTextCoordinate.y - animationButtonTextHeight / 2, TextScale, 1, 1, 1, 1);
}

void MainMenuState::Exit()
{
}
