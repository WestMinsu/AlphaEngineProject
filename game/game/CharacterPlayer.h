#pragma once
#include "ACharacter.h"
#include "Animation.h"

class CharacterPlayer : public ACharacter
{
public:
	CharacterPlayer();
	~CharacterPlayer();

	void Init(AEVec2 position) override;
	void Update(f32 dt) override;
	void Move(f32 dt) override;
	void Attack() override;
	void Draw() override;
	void Destroy() override;

private:
	// Weapon
	CharacterAnimationState m_currentAnimState;

	Animation m_animation;
};