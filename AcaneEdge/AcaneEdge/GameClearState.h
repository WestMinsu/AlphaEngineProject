#pragma once
#include "AGameState.h"
#include <string>

class GameClearState : public AGameState
{
public:
	GameClearState();
	~GameClearState();

	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;

private:
	std::string m_playerName;
};