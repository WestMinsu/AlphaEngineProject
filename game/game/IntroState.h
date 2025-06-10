#pragma once
#include "AGameState.h"
#include "AEEngine.h"

class IntroState: public AGameState
{
public:
	IntroState();

	~IntroState();

	// Override Functions
	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;

private:
	AEGfxTexture* m_pTex;
	f32 m_alphaTime;
	f32 m_alphaSpeed;
	f32 m_currentAlpha;
};