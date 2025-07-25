#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include <map>
#include "PlayerCharacter.h"
#include "AIState.h" 

class RangedEnemyCharacter : public ACharacter
{
public:
	RangedEnemyCharacter();
	~RangedEnemyCharacter();

	void Init(AEVec2 position) override;
	void Init(AEVec2 position, PlayerCharacter* player);
	void Update(f32 dt) override;
	void Move(f32 dt) override;
	void Attack() override;
	void Draw() override;
	void Destroy() override;
	virtual void TakeDamage(s32 damage, DamageType damageType) override;
	
	virtual bool isReadytoFireRange();

	Animation& GetAnimation() { return m_animation; }
	CharacterAnimationState GetCurrentAnimState() const { return m_currentAnimState; }
	bool HasFiredProjectile() const { return m_hasFiredProjectile; }
	void SetFiredProjectile(bool fired) { m_hasFiredProjectile = fired; }
	const ProjectileData& GetProjectileData() const { return m_projectileData; }
	const AEVec2& GetProjectileSpawnOffset() const { return m_projectileSpawnOffset; }
protected:
	Animation m_animation;
	CharacterAnimationState m_currentAnimState;
	std::map<CharacterAnimationState, AnimData> m_animDataMap;
	ProjectileData m_projectileData;
	AEAudio* m_sfxAttack;
	AEAudio* m_sfxHurt;
	AEAudio* m_sfxDeath;

	EnemyAIState m_currentAIState;
	PlayerCharacter* m_pPlayer;

	f32 m_detectionRange;
	f32 m_attackRange;

	f32 m_attackCooldownTimer;
	f32 m_attackCooldownDuration;
	bool m_hasFiredProjectile;

	f32 m_velocityX;
	f32 m_velocityY;
	f32 m_gravity;
	bool m_isGrounded;
	AEVec2 m_projectileSpawnOffset;

	s32 m_attackSoundFrame;
	bool m_hasPlayedAttackSound;

	AEVec2 m_prevPosition;
	f32 m_stuckTimer;
	const float m_jumpStrength = 500.f;
};