#pragma once
#include "AGameState.h"

class GameOverState : public AGameState
{
public:
	GameOverState();
	~GameOverState();

	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;
};