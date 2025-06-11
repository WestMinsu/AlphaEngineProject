#pragma once
#include "ACharacter.h"

class CharacterPlayer : public ACharacter
{
public:
	CharacterPlayer();
	~CharacterPlayer();

	void Init(AEVec2 position) override;
	void Update(f32 dt) override;
	void Move() override;
	void Attack() override;
	void Draw() override;
	void Destroy() override;

private:
	// Weapon
	CharacterAnimationState m_currentAnimState;
	
	AEGfxVertexList* m_mesh;

	AEGfxTexture* m_pTexIdle;
	AEGfxTexture* m_pTexWalk;
	AEGfxTexture* m_pTexJump;
	AEGfxTexture* m_pTexDeath;
	AEGfxTexture* m_currentPTex;

	s32 m_subImageIndex;
	f32 m_offset;
	f32 m_elapsedTime;

	s32 m_idleFrames = 8;
	s32 m_walkFrames = 8;
	s32 m_jumpFrames = 8;
	s32 m_deathFrames = 8;

	bool m_animationFinished = false;
	f32 m_deathTimer;
	const f32 m_restartDelay = 3.0f;

};

