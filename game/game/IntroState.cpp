#include "AGameState.h"
#include "GameManager.h"
#include "IntroState.h"
#include "Constants.h"
#include "Utility.h"

IntroState::IntroState(): 
m_pTex{AEGfxTextureLoad("Assets/Digipen_logo.png")}, 
m_alphaTime{ 0.0f },
m_alphaSpeed{ 1.0f },
m_currentAlpha{ 0.0f }
{
}

IntroState::~IntroState()
{
}

void IntroState::Init()
{

}

void IntroState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_SPACE))
		GameManager::ChangeState(GameState::MAIN_MENU);

	m_alphaTime += dt * m_alphaSpeed;
	m_currentAlpha = (sinf(m_alphaTime));
	if (m_alphaTime >= PI)
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
	}
}

void IntroState::Draw()
{
	AEGfxSetBackgroundColor(0.1f, 0.1f, 0.1f);
	DrawRect(0, 0, kWindowWidth, kWindowHeight, 1.0f, 1.0f, 1.0f, m_currentAlpha, m_pTex);
}

void IntroState::Exit()
{
}
