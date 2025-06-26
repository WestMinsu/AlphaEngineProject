#pragma once
#include "AGameState.h"
#include "CharacterPlayer.h"

class MainGameStateTest : public AGameState
{
public:
	MainGameStateTest();
	~MainGameStateTest();

	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;

private:
	 
	CharacterPlayer m_Player;
};