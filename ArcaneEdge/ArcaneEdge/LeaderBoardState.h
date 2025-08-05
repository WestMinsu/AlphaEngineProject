#pragma once
#include "AGameState.h"

class LeaderBoardState : public AGameState
{
public:
	LeaderBoardState();
	~LeaderBoardState();

	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;
};