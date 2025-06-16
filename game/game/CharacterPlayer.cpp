#include "CharacterPlayer.h"
#include "Constants.h"
#include "Utility.h"
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

	m_velocityY = 0.0f;
	m_gravity = -1200.0f;  
	m_jumpStrength = 600.0f; 
	m_isGrounded = true;     
	m_groundLevel = 0.0f;    
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
	if (!m_isGrounded)
	{
		m_velocityY += m_gravity * dt;     
		m_position.y += m_velocityY * dt; 
	}

	if (m_position.y <= m_groundLevel && m_velocityY <= 0.0f)
	{
		m_position.y = m_groundLevel; 
		m_velocityY = 0.0f;
		m_isGrounded = true;

		if (m_currentAnimState == CharacterAnimationState::JUMP)
		{
			m_currentAnimState = CharacterAnimationState::IDLE;
			m_animation.ChangeAnimState(m_currentAnimState);
		}
	}

	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		if (m_animation.getAnimationFinished())
		{
			m_animation.UpdateDeathTime(dt);
			if (m_animation.isRestartAnimDeathtoIDLE())
			{
				m_currentAnimState = CharacterAnimationState::IDLE;
				m_animation.ChangeAnimState(m_currentAnimState);
				m_currentDirection = CharacterDirection::RIGHT;
			}
		}
	}
	else 
	{
		if (m_currentAnimState == CharacterAnimationState::JUMP && m_animation.getAnimationFinished())
		{
			m_currentAnimState = CharacterAnimationState::IDLE;
			m_animation.ChangeAnimState(m_currentAnimState);
		}
		Move(dt);
	}
	const f32 halfCharWidth = m_size.x / 2.0f;
	const f32 halfCharHeight = m_size.y / 2.0f;

	if (m_position.x < -kHalfWindowWidth + halfCharWidth)
	{
		m_position.x = -kHalfWindowWidth + halfCharWidth;
	}

	if (m_position.x > kHalfWindowWidth - halfCharWidth)
	{
		m_position.x = kHalfWindowWidth - halfCharWidth;
	}

	if (m_position.y > kHalfWindowHeight - halfCharHeight)
	{
		m_position.y = kHalfWindowHeight - halfCharHeight;
		m_velocityY = 0;
	}

	m_animation.Update(dt);
}

void CharacterPlayer::Move(f32 dt)
{
	bool isMoving = false;
	if (AEInputCheckCurr(AEVK_LEFT))
	{
		m_position.x -= m_characterSpeed * dt;
		m_currentDirection = CharacterDirection::LEFT;
		isMoving = true;
	}

	else if (AEInputCheckCurr(AEVK_RIGHT))
	{
		m_position.x += m_characterSpeed * dt;
		m_currentDirection = CharacterDirection::RIGHT;
		isMoving = true;
	}

	if (AEInputCheckTriggered(AEVK_SPACE))
	{
		if (m_isGrounded)
		{
			m_isGrounded = false;
			m_velocityY = m_jumpStrength;

			m_currentAnimState = CharacterAnimationState::JUMP;
			m_animation.ChangeAnimState(m_currentAnimState);
		}
	}

	if (m_currentAnimState != CharacterAnimationState::JUMP && m_currentAnimState != CharacterAnimationState::DEATH)
	{
		if (isMoving)
		{
			if (m_currentAnimState != CharacterAnimationState::WALK)
			{
				m_currentAnimState = CharacterAnimationState::WALK;
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

	if (AEInputCheckTriggered(AEVK_K))
	{
		if (m_currentAnimState != CharacterAnimationState::DEATH)
		{
			m_currentAnimState = CharacterAnimationState::DEATH;
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
	DrawHollowRect(m_position.x, m_position.y, m_size.x, m_size.y, 0.f, 1.f, 0.f);
}

void CharacterPlayer::Destroy()
{
	m_animation.Destroy();
}

