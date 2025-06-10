#pragma once
#include "AGameState.h"
class IntroState: public AGameState
{
public:
	IntroState();

	~IntroState();

	// Override Functions
	void Init() override;
	void Update() override;
	void Draw() override;
	void Exit() override;

private:

};