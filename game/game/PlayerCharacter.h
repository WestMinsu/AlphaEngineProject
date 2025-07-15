#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include <vector>
#include <map>
#include "WeaponData.h"

class PlayerCharacter : public ACharacter
{
public:
	PlayerCharacter();
	~PlayerCharacter();

	void Init(AEVec2 position) override;
	void Update(f32 dt) override;
	void Move(f32 dt) override;
	void Attack() override;
	void Draw() override;
	void Destroy() override;
	void TakeDamage(s32 damage, DamageType damageType) override;
	bool IsCompletelyDead() const;
	bool IsInvincible() const;
	// DUMP Do Not Use
	PlayerCharacter* Clone() override;

	int GetWeaponUseCount(DamageType type) const;
	void ConsumeCurrentWeapon();

	Animation& GetAnimation()
	{
		return m_animation;
	}
	CharacterAnimationState GetCurrentAnimState() const
	{
		return m_currentAnimState;
	}
	s32 GetCurrentAnimationFrame() const;
	const AttackHitbox& GetCurrentMeleeHitbox() const;

	bool HasFiredProjectile() const
	{
		return m_hasFiredProjectile;
	}
	void SetFiredProjectile(bool fired)
	{
		m_hasFiredProjectile = fired;
	}
	bool IsMeleeAttackHitboxActive() const
	{
		return m_isMeleeAttackHitboxActive;
	}
	void RegisterHit()
	{
		m_hasHitEnemyThisAttack = true;
	}
	bool HasHitEnemyThisAttack() const
	{
		return m_hasHitEnemyThisAttack;
	}
	const ProjectileData& GetCurrentProjectileData() const;
	DamageType GetCurrentWeaponType() const
	{
		return m_currentWeapon;
	}
	const std::vector<DamageType>& GetAvailableWeapons() const
	{
		return m_availableWeapons;
	}

	s32 GetScore() const 
	{ 
		return m_score; 
	}
	
	int GetMeleeAttackDamage() const
	{
		return m_meleeAttackDamage;
	}

	void AddScore(s32 amount);
	void BuyMagic(DamageType type);
	const AEVec2& GetHitboxSize() const override;
	const AEVec2& GetHitboxOffset() const override;

private:
	Animation m_animation;
	std::map<CharacterAnimationState, AnimData> m_animDataMap;

	int m_maxHealth;
	f32 m_velocityX;
	f32 m_velocityY;
	f32 m_gravity;
	f32 m_jumpStrength;
	int m_meleeAttackDamage;
	bool m_isGrounded;

	bool m_isMeleeAttacking;
	bool m_isMeleeAttackHitboxActive;
	bool m_hasHitEnemyThisAttack;
	std::vector<AttackHitbox> m_attackHitboxes;

	bool m_isSkillAttacking;
	bool m_hasFiredProjectile;

	bool m_isDashing;
	f32 m_dashSpeed;

	CharacterAnimationState m_currentAnimState;

	std::map<DamageType, ProjectileData> m_projectileDataMap;
	std::vector<DamageType> m_availableWeapons;
	DamageType m_currentWeapon;
	int m_currentWeaponIndex;
	std::map<DamageType, int> m_weaponUseCounts;

	s32 m_score;

	AEAudio* m_sfxMeleeAttack;
	AEAudio* m_sfxFireAttack;
	AEAudio* m_sfxIceAttack;
	AEAudio* m_sfxLightningAttack;
	AEAudio* m_sfxHurt;
	AEAudio* m_sfxDeath;

	bool m_isCrouching;

	bool m_hasPlayedAttackSound;
	AEVec2 m_crouchingHitboxSize;
	AEVec2 m_crouchingHitboxOffset;

	float m_dashCooldownTimer;
	const float m_dashCooldownDuration = 1.5f;
};