#include "CharacterPlayer.h"
#include "Constants.h"

CharacterPlayer::CharacterPlayer()
{
	// TO DO change 
	// Maybe in Constants
	m_position = { 0,0 };
	m_size = { 500.f, 500.f };
	m_healthPoint = 100;
	m_characterSpeed = 300.f;
	m_currentAnimState = CharacterAnimationState::IDLE;
	m_currentDirection = CharacterDirection::RIGHT;

}


CharacterPlayer::~CharacterPlayer()
{
}

void CharacterPlayer::Init(AEVec2 position)
{
	m_position = position;

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

void CharacterPlayer::Update(f32 dt)
{
	s32 totalFramesForCurrentAnim = 0;
	switch (m_currentAnimState)
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

	if (m_currentAnimState == CharacterAnimationState::DEATH && m_animationFinished)
	{
		m_deathTimer += dt;
		if (m_deathTimer >= m_restartDelay)
		{
			m_currentAnimState = CharacterAnimationState::IDLE;
			m_currentPTex = m_pTexIdle;
			m_subImageIndex = 0;
			m_offset = 0.0f;
			m_elapsedTime = 0.0f;
			m_animationFinished = false;
			m_deathTimer = 0.0f;
			m_currentDirection = CharacterDirection::RIGHT;
		}
		return;
	}

	if (m_currentAnimState == CharacterAnimationState::JUMP && m_animationFinished)
	{
		m_currentAnimState = CharacterAnimationState::IDLE;
		m_currentPTex = m_pTexIdle;
		m_subImageIndex = 0;
		m_offset = 0.0f;
		m_elapsedTime = 0.0f;
		m_animationFinished = false;
	}

	if (m_currentAnimState != CharacterAnimationState::JUMP &&
		m_currentAnimState != CharacterAnimationState::DEATH)
	{
		if (!AEInputCheckCurr(AEVK_LEFT) && AEInputCheckCurr(AEVK_RIGHT))
		{
			if (m_currentAnimState != CharacterAnimationState::WALK)
			{
				m_currentAnimState = CharacterAnimationState::WALK;
				m_currentPTex = m_pTexWalk;
				m_subImageIndex = 0;
				m_offset = 0.0f;
				m_elapsedTime = 0.0f;
			}
			m_currentDirection = CharacterDirection::RIGHT;
			m_position.x += m_characterSpeed * dt;
		}
		else if (!AEInputCheckCurr(AEVK_RIGHT) && AEInputCheckCurr(AEVK_LEFT))
		{
			if (m_currentAnimState != CharacterAnimationState::WALK)
			{
				m_currentAnimState = CharacterAnimationState::WALK;
				m_currentPTex = m_pTexWalk;
				m_subImageIndex = 0;
				m_offset = 0.0f;
				m_elapsedTime = 0.0f;
			}
			m_currentDirection = CharacterDirection::LEFT;
			m_position.x -= m_characterSpeed * dt;
		}
		else if (AEInputCheckTriggered(AEVK_SPACE))
		{
			if (m_currentAnimState != CharacterAnimationState::JUMP)
			{
				m_currentAnimState = CharacterAnimationState::JUMP;
				m_currentPTex = m_pTexJump;
				m_subImageIndex = 0;
				m_offset = 0.0f;
				m_elapsedTime = 0.0f;
				m_animationFinished = false;
			}
			//AEAudioPlay(gameManager->m_jumpSound, gameManager->m_sfxGroup, 1.0f, 1.0f, 0);
		}
		else if (AEInputCheckTriggered(AEVK_K))
		{
			if (m_currentAnimState != CharacterAnimationState::DEATH)
			{
				m_currentAnimState = CharacterAnimationState::DEATH;
				m_currentPTex = m_pTexDeath;
				m_subImageIndex = 0;
				m_offset = 0.0f;
				m_elapsedTime = 0.0f;
				m_animationFinished = false;
				m_deathTimer = 0.0f;
			}
		}
		else
		{
			if (m_currentAnimState != CharacterAnimationState::IDLE)
			{
				m_currentAnimState = CharacterAnimationState::IDLE;
				m_currentPTex = m_pTexIdle;
				m_subImageIndex = 0;
				m_offset = 0.0f;
				m_elapsedTime = 0.0f;
			}
		}
	}

	const f32 characterHalfWidth = m_size.x / 2.0f;
	const f32 characterHalfHeight = m_size.y / 2.0f;
	m_position.x = AEClamp(m_position.x, -kWindowWidth / 2.0f + characterHalfWidth, kWindowWidth / 2.0f - characterHalfWidth);

	m_elapsedTime += dt;
	if (m_elapsedTime >= 0.1f)
	{
		switch (m_currentAnimState)
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

			if (m_currentAnimState == CharacterAnimationState::DEATH)
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

void CharacterPlayer::Move()
{

}

void CharacterPlayer::Attack()
{
}

void CharacterPlayer::Draw()
{
	AEMtx33 scale = { 0 };
	if (m_currentDirection == CharacterDirection::LEFT)
		AEMtx33Scale(&scale, -m_size.x, m_size.y);
	else
		AEMtx33Scale(&scale, m_size.x, m_size.y);

	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, 0);

	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, m_position.x, m_position.y);

	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	AEGfxTextureSet(m_currentPTex, m_offset, 0);

	AEGfxSetTransform(transform.m);
	AEGfxMeshDraw(m_mesh, AE_GFX_MDM_TRIANGLES);

}

void CharacterPlayer::Destroy()
{
	AEGfxMeshFree(m_mesh);
	if (m_pTexIdle) AEGfxTextureUnload(m_pTexIdle);
	if (m_pTexWalk) AEGfxTextureUnload(m_pTexWalk);
	if (m_pTexJump) AEGfxTextureUnload(m_pTexJump);
	if (m_pTexDeath) AEGfxTextureUnload(m_pTexDeath);
}

