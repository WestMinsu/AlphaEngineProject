#pragma once
#include <AEEngine.h>
#include "ACharacter.h"

class Animation
{
public:
	Animation();
	~Animation();

	void Init();
	void Update(CharacterAnimationState currentAnimState, f32 dt);
	void ChangeAnimState(CharacterAnimationState newAnimState);
	void Draw(AEMtx33 transform);
	void Destroy();

	bool getAnimationFinished();
	void UpdateDeathTime(f32 dt);
	bool isRestartAnimDeathtoIDLE();

private:
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