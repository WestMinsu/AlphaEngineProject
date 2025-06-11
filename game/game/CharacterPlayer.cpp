#include "CharacterPlayer.h"

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
}

void CharacterPlayer::Update()
{

}

void CharacterPlayer::Move()
{

}

void CharacterPlayer::Attack()
{
}

void CharacterPlayer::Draw()
{


}

CharacterAnimationState CharacterPlayer::getCurrentAnimState()
{
	return m_currentAnimState;
}

void CharacterPlayer::setCurrentAnimState(CharacterAnimationState newAnimState)
{
	m_currentAnimState = newAnimState;
}
