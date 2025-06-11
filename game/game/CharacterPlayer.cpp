#include "CharacterPlayer.h"
#include "Constants.h"

CharacterPlayer::CharacterPlayer()
{
	// TO DO change 
	// Maybe in Constants
	m_position = { 0,0 };
	m_size = { 500.f, 500.f };
	m_healthPoint = 100;
	m_characterSpeed = 300.f;
	m_currentAnimState = CharacterAnimationState::IDLE;
	m_currentDirection = CharacterDirection::RIGHT;

}


CharacterPlayer::~CharacterPlayer()
{
}

void CharacterPlayer::Init(AEVec2 position)
{
	m_position = position;
	m_animation.Init();
}

void CharacterPlayer::Update(f32 dt)
{
	
	if (m_currentAnimState == CharacterAnimationState::DEATH && m_animation.getAnimationFinished())
	{
		m_animation.UpdateDeathTime(dt);
		if (m_animation.isRestartAnimDeathtoIDLE())
		{
			m_currentAnimState = CharacterAnimationState::IDLE;
			m_animation.ChangeAnimState(m_currentAnimState);
			m_currentDirection = CharacterDirection::RIGHT;
		}
		return;
	}

	if (m_currentAnimState == CharacterAnimationState::JUMP && m_animation.getAnimationFinished())
	{
		m_currentAnimState = CharacterAnimationState::IDLE;
		m_animation.ChangeAnimState(CharacterAnimationState::IDLE);
	}

	if (m_currentAnimState != CharacterAnimationState::JUMP &&
		m_currentAnimState != CharacterAnimationState::DEATH)
	{
		Move(dt);
	}
		
	m_animation.Update(m_currentAnimState, dt);

	const f32 characterHalfWidth = m_size.x / 2.0f;
	const f32 characterHalfHeight = m_size.y / 2.0f;
	m_position.x = AEClamp(m_position.x, -kWindowWidth / 2.0f + characterHalfWidth, kWindowWidth / 2.0f - characterHalfWidth);
}

void CharacterPlayer::Move(f32 dt)
{
	if (!AEInputCheckCurr(AEVK_LEFT) && AEInputCheckCurr(AEVK_RIGHT))
	{
		if (m_currentAnimState != CharacterAnimationState::WALK)
		{
			m_currentAnimState = CharacterAnimationState::WALK;
			m_animation.ChangeAnimState(m_currentAnimState);

		}
		m_currentDirection = CharacterDirection::RIGHT;
		m_position.x += m_characterSpeed * dt;
	}
	else if (!AEInputCheckCurr(AEVK_RIGHT) && AEInputCheckCurr(AEVK_LEFT))
	{
		if (m_currentAnimState != CharacterAnimationState::WALK)
		{
			m_currentAnimState = CharacterAnimationState::WALK;
			m_animation.ChangeAnimState(m_currentAnimState);
		}
		m_currentDirection = CharacterDirection::LEFT;
		m_position.x -= m_characterSpeed * dt;
	}
	else if (AEInputCheckTriggered(AEVK_SPACE))
	{
		if (m_currentAnimState != CharacterAnimationState::JUMP)
		{
			m_currentAnimState = CharacterAnimationState::JUMP;
			m_animation.ChangeAnimState(m_currentAnimState);

		}
		//AEAudioPlay(gameManager->m_jumpSound, gameManager->m_sfxGroup, 1.0f, 1.0f, 0);
	}
	else if (AEInputCheckTriggered(AEVK_K))
	{
		if (m_currentAnimState != CharacterAnimationState::DEATH)
		{
			m_currentAnimState = CharacterAnimationState::DEATH;
			m_animation.ChangeAnimState(m_currentAnimState);
		}
	}
	else
	{
		if (m_currentAnimState != CharacterAnimationState::IDLE)
		{
			m_currentAnimState = CharacterAnimationState::IDLE;
			m_animation.ChangeAnimState(m_currentAnimState);
		}
	}
}

void CharacterPlayer::Attack()
{

}

void CharacterPlayer::Draw()
{
	AEMtx33 scale = { 0 };
	if (m_currentDirection == CharacterDirection::LEFT)
		AEMtx33Scale(&scale, -m_size.x, m_size.y);
	else
		AEMtx33Scale(&scale, m_size.x, m_size.y);

	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, 0);

	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, m_position.x, m_position.y);

	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	m_animation.Draw(transform);
}

void CharacterPlayer::Destroy()
{
	m_animation.Destroy();
}

