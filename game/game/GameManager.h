#pragma once
#include "AEEngine.h"
#include "AGameState.h"

class GameManager
{
public:
	// constructor
	GameManager();

	// destructor
	~GameManager();

	void Init();
	void Update();

	//static member variable
	// width & height of the window
	static s32 m_widthWindow;
	static s32 m_heightWindow;

	// GameState variables to change
	static AGameState* m_currentState; 
	static AGameState* m_nextState;

	// Game Font
	static s8 m_font;

	// static function
	static void ChangeState();

private:
	bool isGameRunning;

};