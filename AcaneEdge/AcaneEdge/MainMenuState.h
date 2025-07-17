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
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;

private:
	bool m_isHoveringStart;
	bool m_isHoveringLeaderboard;
	bool m_isHoveringExit;
};