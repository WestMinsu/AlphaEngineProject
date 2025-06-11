#include "Animation.h"

Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Init()
{
	AEGfxMeshStart();

	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
		0.5f, -0.5f, 0xFFFFFFFF, 0.125f, 1.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, 0.125f, 1.0f,
		0.5f, 0.5f, 0xFFFFFFFF, 0.125f, 0.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

	m_mesh = AEGfxMeshEnd();

	m_pTexIdle = AEGfxTextureLoad("Assets/idle_right_down.png");
	m_pTexWalk = AEGfxTextureLoad("Assets/walk_right_down.png");
	m_pTexJump = AEGfxTextureLoad("Assets/Jump_Right_Down.png");
	m_pTexDeath = AEGfxTextureLoad("Assets/death_normal_right_down.png");

	m_currentPTex = m_pTexIdle;
	m_subImageIndex = 0;
	m_offset = 0.0f;
	m_elapsedTime = 0.0f;
}

void Animation::Update(CharacterAnimationState currentAnimState, f32 dt)
{
	s32 totalFramesForCurrentAnim = 0;
	switch (currentAnimState)
	{
	case CharacterAnimationState::IDLE:
		totalFramesForCurrentAnim = m_idleFrames;
		break;
	case CharacterAnimationState::WALK:
		totalFramesForCurrentAnim = m_walkFrames;
		break;
	case CharacterAnimationState::JUMP:
		totalFramesForCurrentAnim = m_jumpFrames;
		break;
	case CharacterAnimationState::DEATH:
		totalFramesForCurrentAnim = m_deathFrames;
		break;
	}

	m_elapsedTime += dt;
	if (m_elapsedTime >= 0.1f)
	{
		switch (currentAnimState)
		{
		case CharacterAnimationState::IDLE:
			totalFramesForCurrentAnim = m_idleFrames;
			break;
		case CharacterAnimationState::WALK:
			totalFramesForCurrentAnim = m_walkFrames;
			break;
		case CharacterAnimationState::JUMP:
			totalFramesForCurrentAnim = m_jumpFrames;
			break;
		case CharacterAnimationState::DEATH:
			totalFramesForCurrentAnim = m_deathFrames;
			break;
		default:
			totalFramesForCurrentAnim = 1;
			break;
		}

		m_subImageIndex++;
		if (m_subImageIndex >= totalFramesForCurrentAnim)
		{
			m_animationFinished = true;

			if (currentAnimState == CharacterAnimationState::DEATH)
			{
				m_subImageIndex = totalFramesForCurrentAnim - 1;
			}
			else
				m_subImageIndex = 0;
		}
		m_offset = static_cast<f32>(m_subImageIndex) / totalFramesForCurrentAnim;

		m_elapsedTime = 0.0f;
	}
}

void Animation::ChangeAnimState(CharacterAnimationState newAnimState)
{
	switch (newAnimState)
	{
	case CharacterAnimationState::IDLE:
		m_currentPTex = m_pTexIdle;
		break;
	case CharacterAnimationState::WALK:
		m_currentPTex = m_pTexWalk;
		break;
	case CharacterAnimationState::JUMP:
		m_currentPTex = m_pTexJump;
		break;
	case CharacterAnimationState::DEATH:
		m_currentPTex = m_pTexDeath;
		m_deathTimer = 0.0f;
		break;
	}
	m_subImageIndex = 0;
	m_offset = 0.0f;
	m_elapsedTime = 0.0f;
	m_animationFinished = false;
}

void Animation::Draw(AEMtx33 transform)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	AEGfxTextureSet(m_currentPTex, m_offset, 0);
	AEGfxSetTransform(transform.m);

	AEGfxMeshDraw(m_mesh, AE_GFX_MDM_TRIANGLES);
}

void Animation::Destroy()
{
	AEGfxMeshFree(m_mesh);
	if (m_pTexIdle) AEGfxTextureUnload(m_pTexIdle);
	if (m_pTexWalk) AEGfxTextureUnload(m_pTexWalk);
	if (m_pTexJump) AEGfxTextureUnload(m_pTexJump);
	if (m_pTexDeath) AEGfxTextureUnload(m_pTexDeath);
}

bool Animation::getAnimationFinished()
{
	return m_animationFinished;
}

void Animation::UpdateDeathTime(f32 dt)
{
	m_deathTimer += dt;
}

bool Animation::isRestartAnimDeathtoIDLE()
{
	return m_deathTimer >= m_restartDelay;
}
