#pragma once
#include "AGameState.h"
class MainMenuState : public AGameState
{
public: 
	// Constructor
	MainMenuState();

	// Destructor
	~MainMenuState();

	// Override Functions
	void Init() override;
	void Update() override;
	void Draw() override;
	void Exit() override;

private:

	bool StartMainGame();
};