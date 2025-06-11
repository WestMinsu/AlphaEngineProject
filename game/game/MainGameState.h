#pragma once
#include "AGameState.h"

enum class CharacterAnimationState
{
	IDLE,
	WALK,
	JUMP,
	DEATH
};

enum class CharacterDirection
{
	LEFT,
	RIGHT
};


class MainGameState : public AGameState
{
public:
	MainGameState();

	~MainGameState();

	// Override Functions
	void Init() override;
	void Update(f32 dt) override;
	void Draw() override;
	void Exit() override;

private:
    AEGfxVertexList* m_mesh;

    AEGfxTexture* m_pTexIdle;
    AEGfxTexture* m_pTexWalk;
    AEGfxTexture* m_pTexJump;
    AEGfxTexture* m_pTexDeath;
    AEGfxTexture* m_currentPTex;

    CharacterAnimationState m_currentAnimState;
    CharacterDirection m_currentDirection;

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

    f32 characterWidth = 500.0f;
    f32 characterHeight = 500.0f;
    AEVec2 m_characterPosition;
    f32 m_characterSpeed = 300.0f;
};