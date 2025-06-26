#pragma once
#include "AGameState.h"

class GameClearState : public AGameState
{
public:
	GameClearState();
	~GameClearState();

	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;
};