#pragma once
#include <memory>
#include "AEEngine.h"
#include "AGameState.h"
#include "IntroState.h"
#include "MainMenuState.h"
#include "MainGameState.h"

class GameManager
{
public:
	// constructor
	GameManager();

	// destructor
	~GameManager();

	void Init();
	void Update(f32 dt);

	//static member variable


	// GameState variables to change
	static GameState m_nextState;

	// Game Font
	static s8 m_font;

	// static function
	// Change m_nextState
	static void ChangeState(GameState newGameState);

private:
	bool m_isGameRunning;
	const char* m_kTextTitle;

	std::unique_ptr<AGameState> m_GameState;

};