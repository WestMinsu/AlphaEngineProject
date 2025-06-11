#pragma once
#include "ACharacter.h"
#include "MainGameState.h"

class CharacterPlayer : public ACharacter
{
public:
	CharacterPlayer();
	~CharacterPlayer();

	void Init(AEVec2 m_position) override;
	void Update() override;
	void Move() override;
	void Attack() override;
	void Draw() override;

	//getter 
	CharacterAnimationState getCurrentAnimState();

	//setter
	void setCurrentAnimState(CharacterAnimationState newAnimState);

private:
	// Weapon
	CharacterAnimationState m_currentAnimState;
	
};

