#include "CharacterPlayer.h"
#include "Constants.h"
#include "Utility.h"
#include <iostream>

CharacterPlayer::CharacterPlayer()
{
	m_position = { 0,0 };
	m_size = { 500.f, 500.f };
	m_healthPoint = 100;
	m_characterSpeed = 300.f;
	m_airAcceleration = 1200.f;
	m_currentDirection = CharacterDirection::RIGHT;
	m_currentAnimState = CharacterAnimationState::IDLE;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_gravity = -1200.0f;
	m_jumpStrength = 600.0f;
	m_isGrounded = true;
	m_groundLevel = 0.0f;

	m_isMeleeAttacking = false;
	m_isMeleeAttackHitboxActive = false;
	m_hasHitEnemyThisAttack = false;

	m_isProjectileAttacking = false;
	m_hasFiredProjectile = false;

	m_isDashing = false;
	m_dashSpeed = 800.0f;
}

CharacterPlayer::~CharacterPlayer() {}

void CharacterPlayer::Init(AEVec2 position)
{
	m_position = position;
	m_animation.Init();

	m_animDataMap[CharacterAnimationState::IDLE] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Idle/Battlemage Idle.png", nullptr, 8, SpriteSheetOrientation::VERTICAL, 0.1f, true };
	m_animDataMap[CharacterAnimationState::WALK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Running/Battlemage Run.png", nullptr, 10, SpriteSheetOrientation::VERTICAL, 0.08f, true };
	m_animDataMap[CharacterAnimationState::JUMP] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Jump Neutral/Battlemage Jump Neutral.png", nullptr, 12, SpriteSheetOrientation::VERTICAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::MELEE_ATTACK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Attack 1/Battlemage Attack 1.png", nullptr, 8, SpriteSheetOrientation::VERTICAL, 0.08f, false };
	m_animDataMap[CharacterAnimationState::PROJECTILE_ATTACK] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Sustain Magic/Battlemage Sustain Magic.png", nullptr, 11, SpriteSheetOrientation::VERTICAL, 0.1f, false };
	m_animDataMap[CharacterAnimationState::DASH] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Dash/Battlemage Dash.png", nullptr, 7, SpriteSheetOrientation::VERTICAL, 0.11f, false };
	m_animDataMap[CharacterAnimationState::DEATH] = { "Assets/Character/Battlemage Complete (Sprite Sheet)/Death/Battlemage Death.png", nullptr, 12, SpriteSheetOrientation::VERTICAL, 0.1f, false };

	for (auto& pair : m_animDataMap) {
		pair.second.pTexture = AEGfxTextureLoad(pair.second.texturePath.c_str());
	}

	m_attackHitboxes.resize(8);
	m_attackHitboxes[0] = { {0, 0}, {0, 0} };
	m_attackHitboxes[1] = { {0, 0}, {0, 0} };
	m_attackHitboxes[2] = { {0, 0}, {0, 0} };

	m_attackHitboxes[3] = { { m_size.x * 0.25f, m_size.y * -0.15f },  { m_size.x * 0.40f, m_size.y * 0.70f } };
	m_attackHitboxes[4] = { { m_size.x * 0.28f, m_size.y * -0.3f },  { m_size.x * 0.40f, m_size.y * 0.20f } };
	m_attackHitboxes[5] = { { m_size.x * 0.28f, m_size.y * -0.4f }, { m_size.x * 0.40f, m_size.y * 0.10f } };
	m_attackHitboxes[6] = { { m_size.x * 0.28f, m_size.y * -0.4f }, { m_size.x * 0.40f, m_size.y * 0.10f } };

	m_attackHitboxes[7] = { {0, 0}, {0, 0} };

	m_animation.Play(CharacterAnimationState::IDLE, m_animDataMap.at(CharacterAnimationState::IDLE));
}

void CharacterPlayer::Update(f32 dt)
{
	if (m_animation.GetCurrentState() == CharacterAnimationState::DEATH && m_animation.IsFinished())
	{
		return;
	}

	bool isBusy = m_isMeleeAttacking || m_isProjectileAttacking || m_isDashing;

	if (AEInputCheckTriggered(AEVK_A) && !isBusy) 
	{
			m_isMeleeAttacking = true;
			m_hasHitEnemyThisAttack = false;
	}
	if (AEInputCheckTriggered(AEVK_S) && !isBusy) 
	{
		m_isProjectileAttacking = true;
		m_hasFiredProjectile = false;
	}
	if (AEInputCheckTriggered(AEVK_LSHIFT) && m_isGrounded && !isBusy) {
		m_isDashing = true;
	}
	if (AEInputCheckTriggered(AEVK_SPACE) && m_isGrounded && !isBusy) {
		m_isGrounded = false;
		m_velocityY = m_jumpStrength;
	}

	if (m_isMeleeAttacking) {
		if (m_animation.GetCurrentFrame() >= 3) {
			m_isMeleeAttackHitboxActive = true;
		}
		if (m_animation.IsFinished()) {
			m_isMeleeAttacking = false;
			m_isMeleeAttackHitboxActive = false;
		}
	}
	else {
		m_isMeleeAttackHitboxActive = false;
	}

	if (m_isProjectileAttacking && m_animation.IsFinished()) {
		m_isProjectileAttacking = false;
	}
	if (m_isDashing && m_animation.IsFinished()) {
		m_isDashing = false;
	}

	Move(dt);

	if (m_isGrounded && (m_isMeleeAttacking || m_isProjectileAttacking)) {
		m_velocityX = 0.0f;
	}
	if (m_isDashing) {
		m_velocityX = (m_currentDirection == CharacterDirection::RIGHT) ? m_dashSpeed : -m_dashSpeed;
	}

	if (!m_isGrounded) { m_velocityY += m_gravity * dt; }
	m_position.x += m_velocityX * dt;
	m_position.y += m_velocityY * dt;

	if (m_position.y <= m_groundLevel && m_velocityY <= 0.0f) {
		m_position.y = m_groundLevel;
		m_velocityY = 0.0f;
		if (!m_isGrounded) {
			m_isGrounded = true;
			if (!AEInputCheckCurr(AEVK_LEFT) && !AEInputCheckCurr(AEVK_RIGHT)) {
				m_velocityX = 0.0f;
			}
		}
	}

	CharacterAnimationState desiredState;
	if (m_isDashing) { desiredState = CharacterAnimationState::DASH; }
	else if (m_isProjectileAttacking) { desiredState = CharacterAnimationState::PROJECTILE_ATTACK; }
	else if (m_isMeleeAttacking) { desiredState = CharacterAnimationState::MELEE_ATTACK; }
	else if (!m_isGrounded) { desiredState = CharacterAnimationState::JUMP; }
	else {
		if (m_velocityX == 0.0f) desiredState = CharacterAnimationState::IDLE;
		else desiredState = CharacterAnimationState::WALK;
	}

	m_animation.Play(desiredState, m_animDataMap.at(desiredState));
	m_currentAnimState = desiredState;

	const f32 halfCharWidth = m_size.x / 2.0f;
	const f32 halfCharHeight = m_size.y / 2.0f;
	if (m_position.x < -kHalfWindowWidth + halfCharWidth) { m_position.x = -kHalfWindowWidth + halfCharWidth; }
	if (m_position.x > kHalfWindowWidth - halfCharWidth) { m_position.x = kHalfWindowWidth - halfCharWidth; }
	if (m_position.y > kHalfWindowHeight - halfCharHeight) { m_position.y = kHalfWindowHeight - halfCharHeight; m_velocityY = 0; }

	m_animation.Update(dt);
}

void CharacterPlayer::Move(f32 dt) {
	if (m_isGrounded) {
		if (AEInputCheckCurr(AEVK_LEFT)) { m_velocityX = -m_characterSpeed; }
		else if (AEInputCheckCurr(AEVK_RIGHT)) { m_velocityX = m_characterSpeed; }
		else { m_velocityX = 0.0f; }
	}
	else {
		if (AEInputCheckCurr(AEVK_LEFT)) { m_velocityX -= m_airAcceleration * dt; }
		else if (AEInputCheckCurr(AEVK_RIGHT)) { m_velocityX += m_airAcceleration * dt; }
	}

	if (m_velocityX > m_characterSpeed) { m_velocityX = m_characterSpeed; }
	if (m_velocityX < -m_characterSpeed) { m_velocityX = -m_characterSpeed; }

	if (!m_isMeleeAttacking && !m_isProjectileAttacking) 
	{
		if (AEInputCheckCurr(AEVK_LEFT)) { m_currentDirection = CharacterDirection::LEFT; }
		else if (AEInputCheckCurr(AEVK_RIGHT)) { m_currentDirection = CharacterDirection::RIGHT; }
	}
}

void CharacterPlayer::Draw()
{
	AEMtx33 scale = { 0 };
	AEMtx33Scale(&scale, m_currentDirection == CharacterDirection::RIGHT ? m_size.x : -m_size.x, m_size.y);

	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, 0);

	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, m_position.x, m_position.y);

	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	m_animation.Draw(transform);

	if (m_isMeleeAttackHitboxActive)
	{
		const AttackHitbox& currentHitbox = GetCurrentAttackHitbox();
		AEVec2 hitboxPos;
		hitboxPos.x = m_position.x + (m_currentDirection == CharacterDirection::RIGHT ? currentHitbox.offset.x : -currentHitbox.offset.x);
		hitboxPos.y = m_position.y + currentHitbox.offset.y;
		DrawHollowRect(hitboxPos.x, hitboxPos.y, currentHitbox.size.x, currentHitbox.size.y, 1.0f, 0.0f, 0.0f, 0.5f);
	}
}

void CharacterPlayer::Destroy()
{
	for (auto& pair : m_animDataMap) {
		if (pair.second.pTexture) AEGfxTextureUnload(pair.second.pTexture);
	}
	m_animation.Destroy();
}

s32 CharacterPlayer::GetCurrentAnimationFrame() const
{
	return m_animation.GetCurrentFrame();
}

const AttackHitbox& CharacterPlayer::GetCurrentAttackHitbox() const
{
	s32 currentFrame = GetCurrentAnimationFrame();
	if (currentFrame >= 0 && currentFrame < m_attackHitboxes.size())
	{
		return m_attackHitboxes[currentFrame];
	}
	return m_attackHitboxes[0];
}

void CharacterPlayer::Attack() {}

void CharacterPlayer::TakeDamage(s32 damage)
{
	m_healthPoint -= damage;
}