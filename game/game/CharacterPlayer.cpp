#include "CharacterPlayer.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>

CharacterPlayer::CharacterPlayer()
{
	m_position = { 0,0 };
	m_size = { 500.f, 500.f };
	m_healthPoint = 100;
	m_characterSpeed = 300.f;
	m_airAcceleration = 1200.f;
	m_currentDirection = CharacterDirection::RIGHT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_jumpStrength = 600.0f;
	m_isGrounded = true;
	m_groundLevel = 0.0f;

	m_isAttacking = false;
	m_attackTimer = 0.0f;
	m_isAttackHitboxActive = false;
	m_hasHitEnemyThisAttack = false;

	m_isDashing = false;
	m_dashTimer = 0.0f;
	m_dashSpeed = 800.0f;
}

CharacterPlayer::~CharacterPlayer() {}

void CharacterPlayer::Init(AEVec2 position)
{
	m_position = position;
	m_animation.Init();
	m_animation.ChangeAnimState(m_currentAnimState);
}

void CharacterPlayer::Update(f32 dt)
{
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
		m_animation.Update(dt);
		return;
	}

	if (AEInputCheckTriggered(AEVK_A) && !m_isAttacking && !m_isDashing)
	{
		bool canAttack = !m_isGrounded || (m_isGrounded && m_velocityX == 0.0f);
		if (canAttack)
		{
			m_isAttacking = true;
			m_attackTimer = 0.0f;
			m_hasHitEnemyThisAttack = false;
		}
	}
	if (AEInputCheckTriggered(AEVK_LSHIFT) && m_isGrounded && !m_isAttacking && !m_isDashing)
	{
		m_isDashing = true;
		m_dashTimer = 0.0f;
	}
	if (AEInputCheckTriggered(AEVK_SPACE) && m_isGrounded && !m_isAttacking && !m_isDashing)
	{
		m_isGrounded = false;
		m_velocityY = m_jumpStrength;
	}

	if (m_isAttacking)
	{
		m_attackTimer += dt;
		const float attackAnimDuration = m_animation.GetAnimationDuration(CharacterAnimationState::ATTACK);
		const float frameDuration = 0.1f;
		const float hitboxActiveTime = 3 * frameDuration;
		if (m_attackTimer >= hitboxActiveTime)
		{
			m_isAttackHitboxActive = true;
		}
		if (attackAnimDuration > 0.0f && m_attackTimer >= attackAnimDuration)
		{
			m_isAttacking = false;
			m_isAttackHitboxActive = false;
		}
	}
	else
	{
		m_isAttackHitboxActive = false;
	}
	if (m_isDashing)
	{
		m_dashTimer += dt;
		const float dashDuration = m_animation.GetAnimationDuration(CharacterAnimationState::DASH);
		if (dashDuration > 0.0f && m_dashTimer >= dashDuration)
		{
			m_isDashing = false;
		}
	}

	Move(dt);
	if (m_isGrounded && m_isAttacking) { m_velocityX = 0.0f; }
	if (m_isDashing)
	{
		m_velocityX = (m_currentDirection == CharacterDirection::RIGHT) ? m_dashSpeed : -m_dashSpeed;
	}

	if (!m_isGrounded) { m_velocityY += m_gravity * dt; }
	m_position.x += m_velocityX * dt;
	m_position.y += m_velocityY * dt;

	if (m_position.y <= m_groundLevel && m_velocityY <= 0.0f)
	{
		m_position.y = m_groundLevel;
		m_velocityY = 0.0f;
		if (!m_isGrounded)
		{
			m_isGrounded = true;
			if (!AEInputCheckCurr(AEVK_LEFT) && !AEInputCheckCurr(AEVK_RIGHT))
			{
				m_velocityX = 0.0f;
			}
		}
	}

	CharacterAnimationState desiredState;
	if (m_isDashing) { desiredState = CharacterAnimationState::DASH; }
	else if (m_isAttacking) { desiredState = CharacterAnimationState::ATTACK; }
	else if (!m_isGrounded) { desiredState = CharacterAnimationState::JUMP; }
	else
	{
		if (m_velocityX == 0.0f) desiredState = CharacterAnimationState::IDLE;
		else desiredState = CharacterAnimationState::WALK;
	}
	if (m_currentAnimState != desiredState)
	{
		m_currentAnimState = desiredState;
		m_animation.ChangeAnimState(m_currentAnimState);
	}

	const f32 halfCharWidth = m_size.x / 2.0f;
	const f32 halfCharHeight = m_size.y / 2.0f;
	if (m_position.x < -kHalfWindowWidth + halfCharWidth) { m_position.x = -kHalfWindowWidth + halfCharWidth; }
	if (m_position.x > kHalfWindowWidth - halfCharWidth) { m_position.x = kHalfWindowWidth - halfCharWidth; }
	if (m_position.y > kHalfWindowHeight - halfCharHeight) { m_position.y = kHalfWindowHeight - halfCharHeight; m_velocityY = 0; }

	m_animation.Update(dt);
}

void CharacterPlayer::Move(f32 dt)
{
	if (m_isGrounded)
	{
		// --- 지상 이동: 즉시 최대 속도로 변경 ---
		if (AEInputCheckCurr(AEVK_LEFT))
		{
			m_velocityX = -m_characterSpeed;
		}
		else if (AEInputCheckCurr(AEVK_RIGHT))
		{
			m_velocityX = m_characterSpeed;
		}
		else
		{
			m_velocityX = 0.0f;
		}
	}

	else 
	{
		if (AEInputCheckCurr(AEVK_LEFT))
		{
			m_velocityX -= m_airAcceleration * dt;
		}
		else if (AEInputCheckCurr(AEVK_RIGHT))
		{
			m_velocityX += m_airAcceleration * dt;
		}
	}

	if (m_velocityX > m_characterSpeed)
	{
		m_velocityX = m_characterSpeed;
	}

	if (m_velocityX < -m_characterSpeed)
	{
		m_velocityX = -m_characterSpeed;
	}

	if (!m_isAttacking)
	{
		if (AEInputCheckCurr(AEVK_LEFT))
		{
			m_currentDirection = CharacterDirection::LEFT;
		}
		else if (AEInputCheckCurr(AEVK_RIGHT))
		{
			m_currentDirection = CharacterDirection::RIGHT;
		}
	}
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

void CharacterPlayer::Attack()
{
}

void CharacterPlayer::TakeDamage(s32 damage)
{
	m_healthPoint -= damage;
}