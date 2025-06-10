#pragma once

enum class GameState
{
	NONE,
	INTRO,
	MAINMENU,
	MAINNGAME
};

class AGameState
{
public: 
	virtual ~AGameState() {};

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Exit() = 0;
};