#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include "AIState.h" 
#include <map>

class PlayerCharacter;

class MeleeEnemyCharacter : public ACharacter
{
public:
	MeleeEnemyCharacter();
	MeleeEnemyCharacter(MeleeEnemyCharacter*);
	~MeleeEnemyCharacter();

	void Init(AEVec2 position) override;
	void Init(AEVec2 position, PlayerCharacter* player);
	void Update(f32 dt) override;
	void Move(f32 dt) override;
	void Attack() override;
	void Draw() override;
	void Destroy() override;
	void TakeDamage(s32 damage, DamageType damageType) override;
	MeleeEnemyCharacter* Clone() override;



private:
	Animation m_animation;
	CharacterAnimationState m_currentAnimState;
	std::map<CharacterAnimationState, AnimData> m_animDataMap;

	EnemyAIState m_currentAIState;
	PlayerCharacter* m_pPlayer;

	f32 m_detectionRange;
	f32 m_attackRange;

	f32 m_attackCooldownTimer;
	f32 m_attackCooldownDuration;
	bool m_isHurt;
};