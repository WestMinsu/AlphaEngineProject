#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include "AIState.h" 
#include <map>

class CharacterPlayer;

class CharacterEnemy : public ACharacter
{
public:
	CharacterEnemy();
	~CharacterEnemy();

	void Init(AEVec2 position) override;
	void Init(AEVec2 position, CharacterPlayer* player);
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

	EnemyAIState m_currentAIState;
	CharacterPlayer* m_pPlayer;

	f32 m_detectionRange;
	f32 m_attackRange;

	f32 m_attackCooldownTimer;
	f32 m_attackCooldownDuration;
};