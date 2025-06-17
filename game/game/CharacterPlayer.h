#pragma once
#include "ACharacter.h"
#include "Animation.h"
#include <vector>

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

	bool IsAttacking() const;
	bool IsAttackHitboxActive() const;
	bool HasHitEnemyThisAttack() const;

	void RegisterHit();
	s32 GetCurrentAnimationFrame() const;
	const AttackHitbox& GetCurrentAttackHitbox() const;

private:
	Animation m_animation;

	f32 m_velocityX;
	f32 m_velocityY;
	f32 m_gravity;
	f32 m_jumpStrength;
	f32 m_groundLevel;
	bool m_isGrounded;

	bool m_isAttacking;
	f32 m_attackTimer;
	bool m_isAttackHitboxActive;
	bool m_hasHitEnemyThisAttack;

	bool m_isDashing;
	f32 m_dashTimer;
	f32 m_dashSpeed;

	CharacterAnimationState m_currentAnimState;
	std::vector<AttackHitbox> m_attackHitboxes;
};