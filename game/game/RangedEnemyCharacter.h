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

	Animation& GetAnimation() { return m_animation; }
	CharacterAnimationState GetCurrentAnimState() const { return m_currentAnimState; }
	bool HasFiredProjectile() const { return m_hasFiredProjectile; }
	void SetFiredProjectile(bool fired) { m_hasFiredProjectile = fired; }
	const ProjectileData& GetProjectileData() const { return m_projectileData; }
protected:
	Animation m_animation;
	CharacterAnimationState m_currentAnimState;
	std::map<CharacterAnimationState, AnimData> m_animDataMap;
	ProjectileData m_projectileData;

	EnemyAIState m_currentAIState;
	PlayerCharacter* m_pPlayer;

	f32 m_detectionRange;
	f32 m_attackRange;
	f32 m_idealRange;

	f32 m_attackCooldownTimer;
	f32 m_attackCooldownDuration;
	bool m_hasFiredProjectile;
	bool m_isHurt;
};