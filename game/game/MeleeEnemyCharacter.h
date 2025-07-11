#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include "AIState.h" 
#include <map>
#include <vector>

class PlayerCharacter;

class MeleeEnemyCharacter : public ACharacter
{
public:
	MeleeEnemyCharacter();
	MeleeEnemyCharacter(const MeleeEnemyCharacter&);
	~MeleeEnemyCharacter();

	void Init(AEVec2 position) override;
	void Init(AEVec2 position, PlayerCharacter* player);
	void Update(f32 dt) override;
	void Move(f32 dt) override;
	void Attack() override;
	void Draw() override;
	void Destroy() override;
	virtual void TakeDamage(s32 damage, DamageType damageType) override;

	bool IsAttackHitboxActive() const;
	const AttackHitbox& GetCurrentMeleeHitbox() const;
	bool HasHitPlayerThisAttack() const { return m_hasHitPlayerThisAttack; }
	void RegisterPlayerHit() { m_hasHitPlayerThisAttack = true; }
protected:
	Animation m_animation;
	CharacterAnimationState m_currentAnimState;
	std::map<CharacterAnimationState, AnimData> m_animDataMap;
	AEAudio m_sfxAttack;
	AEAudio m_sfxHurt;
	AEAudio m_sfxDeath;

	EnemyAIState m_currentAIState;
	PlayerCharacter* m_pPlayer;

	f32 m_detectionRange;
	f32 m_attackRange;

	f32 m_attackCooldownTimer;
	f32 m_attackCooldownDuration;

	f32 m_velocityX;
	f32 m_velocityY;
	f32 m_gravity;
	bool m_isGrounded;

	s32 m_attackSoundFrame;
	bool m_hasPlayedAttackSound;

	std::vector<AttackHitbox> m_meleeHitboxes;
	bool m_hasHitPlayerThisAttack;
	bool m_isMeleeAttackHitboxActive;

	AEVec2 m_prevPosition;
	f32 m_stuckTimer;
	const float m_jumpStrength = 500.f; 
};