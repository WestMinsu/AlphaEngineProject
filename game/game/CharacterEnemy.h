#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include <map>

class CharacterEnemy : public ACharacter
{
public:
	CharacterEnemy();
	~CharacterEnemy();

	void Init(AEVec2 position) override;
	void Update(f32 dt) override;
	void Move(f32 dt) override;
	void Attack() override;
	void Draw() override;
	void Destroy() override;
	void TakeDamage(s32 damage) override;

private:
	Animation m_animation;
	CharacterAnimationState m_currentAnimState;
	std::map<CharacterAnimationState, AnimData> m_animDataMap;
};