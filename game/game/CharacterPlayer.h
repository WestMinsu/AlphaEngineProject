#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include <vector>
#include <map>
#include "WeaponData.h"

struct AttackHitbox
{
	AEVec2 offset;
	AEVec2 size;
};

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
	void TakeDamage(s32 damage) override;

	Animation& GetAnimation() { return m_animation; }
	CharacterAnimationState GetCurrentAnimState() const { return m_currentAnimState; }
	s32 GetCurrentAnimationFrame() const;
	const AttackHitbox& GetCurrentAttackHitbox() const;

	bool HasFiredProjectile() const { return m_hasFiredProjectile; }
	void SetFiredProjectile(bool fired) { m_hasFiredProjectile = fired; }

	bool IsMeleeAttackHitboxActive() const { return m_isMeleeAttackHitboxActive; }
	void RegisterHit() { m_hasHitEnemyThisAttack = true; }
	bool HasHitEnemyThisAttack() const { return m_hasHitEnemyThisAttack; }

	const ProjectileData& GetCurrentProjectileData() const;
	WeaponType GetCurrentWeaponType() const { return m_currentWeapon; }
	const std::vector<WeaponType>& GetAvailableWeapons() const { return m_availableWeapons; }
private:
	Animation m_animation;
	std::map<CharacterAnimationState, AnimData> m_animDataMap;

	f32 m_velocityX;
	f32 m_velocityY;
	f32 m_gravity;
	f32 m_jumpStrength;
	f32 m_groundLevel;
	bool m_isGrounded;

	bool m_isMeleeAttacking;
	bool m_isMeleeAttackHitboxActive;
	bool m_hasHitEnemyThisAttack;
	std::vector<AttackHitbox> m_attackHitboxes;

	bool m_isProjectileAttacking;
	bool m_hasFiredProjectile;

	bool m_isDashing;
	f32 m_dashSpeed;

	CharacterAnimationState m_currentAnimState;

	std::map<WeaponType, ProjectileData> m_projectileDataMap;
	std::vector<WeaponType> m_availableWeapons;
	WeaponType m_currentWeapon;
	int m_currentWeaponIndex;
};