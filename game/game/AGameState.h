#pragma once
#include "AEEngine.h"

enum class GameState
{
	NONE,
	INTRO,
	MAIN_MENU,
	MAIN_GAME
};

class AGameState
{
public: 
	virtual ~AGameState() {};

	virtual void Init() = 0;
	virtual void Update(f32 dt) = 0;
	virtual void Draw() = 0;
	virtual void Exit() = 0;
};