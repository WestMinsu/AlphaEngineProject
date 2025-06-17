#include "ACharacter.h"

const AEVec2& ACharacter::GetPosition() const
{
	return m_position;
}

const AEVec2& ACharacter::GetSize() const
{
	return m_size;
}

s32 ACharacter::GetHealth() const
{
	return m_healthPoint;
}

CharacterDirection ACharacter::GetDirection() const
{
	return m_currentDirection;
}
