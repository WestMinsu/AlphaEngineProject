#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include "WeaponData.h"
#include <vector>
#include <map>
#include "PlayerCharacter.h" //for debugging
#include "Projectile.h"

// class PlayerCharacter;

enum class BossAIState
{
	APPEARING,
	APPEARANCE_WAIT,
	IDLE,
	MELEE_ATTACK,
	RANGED_ATTACK,
	LASER_CAST,
	LASER_BEAM,
	GLOWING,
	BUFF,
	COOLDOWN,
	DEATH
};

class BossCharacter : public ACharacter
{
public:
	BossCharacter();
	BossCharacter(const BossCharacter& bossCopy);
	~BossCharacter();

	void Init(AEVec2 position) override;
	void Init(AEVec2 position, PlayerCharacter* player);
	void Update(f32 dt) override;
	void Move(f32 dt) override;
	void Attack() override;
	void Draw() override;
	void TakeDamage(s32 damage, DamageType damageType) override;
	void Destroy() override;
	// DUMP, Do Not Use
	BossCharacter* Clone();

	Animation& GetAnimation() { return m_animation; }
	CharacterAnimationState GetCurrentAnimState() const { return m_currentAnimState; }
	BossAIState GetCurrentAIState() const { return m_currentAIState; }
	const AnimData& GetAnimData(CharacterAnimationState state) const { return m_animDataMap.at(state); }
	bool IsAttackable() const { return m_isAttackable; }
	bool IsUnbeatable() const; 
	bool IsBuffed() const { return m_isInBuffState; }
	const ProjectileData& GetProjectileData() const { return m_projectileData; }
	const AttackHitbox& GetCurrentMeleeHitbox() const;
	bool HasFired() const { return m_hasFired; }
	void SetFired(bool fired) { m_hasFired = fired; }
	AEVec2 GetLaserTargetPos() const { return m_laserTargetPos; }
	Animation& GetLaserAnimation() { return m_laserAnimation; }
	const AttackHitbox& GetLaserHitbox() const { return m_laserHitbox; }
	const f32 GetMaxHealth() const { return m_maxHealth; }
	bool IsCompletelyDead() const;
	bool HasHitPlayerThisAttack() const { return m_hasHitPlayerThisAttack; }
	void RegisterPlayerHit() { m_hasHitPlayerThisAttack = true; }
	std::vector<AttackHitbox> GetCurrentActiveHitboxes();

	void GetBossPattern(AEGfxTexture*& pBossMessageTexture, float& bossMessageTimer, BossAIState& previousBossAIState);
	void AttackMelee(PlayerCharacter& player);
	void AttackRange(PlayerCharacter& player, std::vector<Projectile>& enemyProjectiles);
	void AttackLaser(PlayerCharacter& player);

	void DrawBossHPUI();

private:
	Animation m_animation;
	CharacterAnimationState m_currentAnimState;
	std::map<CharacterAnimationState, AnimData> m_animDataMap;
	ProjectileData m_projectileData;
	std::vector<AttackHitbox> m_meleeHitboxes;
	AttackHitbox m_laserHitbox;
	AEVec2 m_laserVisualSize;
	AEVec2 m_laserVisualOffset;

	Animation m_laserAnimation;

	BossAIState m_currentAIState;
	PlayerCharacter* m_pPlayer;
		
	f32 m_maxHealth;
	bool m_isAttackable;
	bool m_hasGlowed;
	bool m_hasBuffed;
	bool m_isInBuffState;

	f32 m_aiTimer;

	f32 m_meleeAttackRange;
	f32 m_rangedAttackRange;

	f32 m_cooldownDuration;

	bool m_hasFired;
	AEVec2 m_laserTargetPos;
	bool m_hasHitPlayerThisAttack;

	const float m_bossMessageDuration = 2.5;
	AEGfxTexture* m_pBossHealthBar;
};