#pragma once
#include "AGameState.h"
class MainGameState : public AGameState
{
public:
	MainGameState();

	~MainGameState();

	// Override Functions
	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;

private:

};