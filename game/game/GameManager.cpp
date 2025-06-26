#include "GameManager.h"
#include "Utility.h"

GameState GameManager::m_nextState = GameState::NONE;
s8 GameManager::m_font;
bool GameManager::m_isGameRunning = true;

GameManager::GameManager()
{
	m_kTextTitle = "Title";

	m_GameState = nullptr;
}

GameManager::~GameManager()
{
	FreeUtilityMeshes();
	AEGfxDestroyFont(m_font);
}

void GameManager::Init()
{
	AESysSetWindowTitle(m_kTextTitle);
	
	m_GameState = std::move(std::make_unique<IntroState>());
	m_GameState->Init();
	m_font = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);
	InitUtilityMeshes();
	AESysReset();
}

void GameManager::Update(f32 dt)
{
	while (m_isGameRunning)
	{
		if (GameManager::m_nextState != GameState::NONE)
		{
			m_GameState->Exit();

			switch (GameManager::m_nextState)
			{
			case GameState::INTRO:
				m_GameState = std::move(std::make_unique<IntroState>());
				break;
			case GameState::MAIN_MENU:
				m_GameState = std::move(std::make_unique<MainMenuState>());
				break;
			case GameState::MAIN_GAME:
				m_GameState = std::move(std::make_unique<MainGameState>());
				break;
			case GameState::GAME_CLEAR:
				m_GameState = std::move(std::make_unique<GameClearState>());
				break;
			case GameState::GAME_OVER:
				m_GameState = std::move(std::make_unique<GameOverState>());
				break;
			}

			m_GameState->Init();
			GameManager::m_nextState = GameState::NONE;
		}
		AESysFrameStart();

		m_GameState->Update(dt);
		m_GameState->Draw();

		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist()) {
			m_isGameRunning = FALSE;
		}
		AESysFrameEnd();
	}
	m_GameState->Exit();
	return;
}

void GameManager::ChangeState(GameState newGameState)
{
	GameManager::m_nextState = newGameState;
}
