#include "CharacterEnemy.h"
#include "Constants.h"
#include "Utility.h"

CharacterEnemy::CharacterEnemy()
{
	m_size = { 500.f, 500.f };
	m_healthPoint = 50; 
	m_characterSpeed = 0.0f; 
	m_currentDirection = CharacterDirection::LEFT; 
	m_currentAnimState = CharacterAnimationState::IDLE;
}

CharacterEnemy::~CharacterEnemy() {}

void CharacterEnemy::Init(AEVec2 position)
{
	m_position = position;
	m_animation.Init(); 
	m_animation.ChangeAnimState(CharacterAnimationState::IDLE);
}

void CharacterEnemy::Update(f32 dt)
{
	// TODO: AI
	m_animation.Update(dt);
}

void CharacterEnemy::Move(f32 dt) 
{
	// TODO
} 

void CharacterEnemy::Attack() 
{
	// TODO
}  

void CharacterEnemy::Draw()
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

void CharacterEnemy::Destroy()
{
	m_animation.Destroy();
}

void CharacterEnemy::TakeDamage(s32 damage)
{
	if (m_currentAnimState == CharacterAnimationState::DEATH)
	{
		return;
	}

	m_healthPoint -= damage;

	if (m_healthPoint <= 0)
	{
		m_currentAnimState = CharacterAnimationState::DEATH;
		m_animation.ChangeAnimState(m_currentAnimState);
	}
}