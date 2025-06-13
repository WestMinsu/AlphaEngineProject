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

	//AEGfxTriAdd(
	//	-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
	//	0.5f, -0.5f, 0xFFFFFFFF, 0.125f, 1.0f,
	//	-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

	//AEGfxTriAdd(
	//	0.5f, -0.5f, 0xFFFFFFFF, 0.125f, 1.0f,
	//	0.5f, 0.5f, 0xFFFFFFFF, 0.125f, 0.0f,
	//	-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.125f, 
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 0.125f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 0.125f,
		0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
	m_mesh = AEGfxMeshEnd();

	m_animDataMap[CharacterAnimationState::IDLE] = {
		  AEGfxTextureLoad("Assets/Character/Battlemage Complete (Sprite Sheet)/Idle/Battlemage Idle.png"), 8, SpriteSheetOrientation::VERTICAL, 0.1f
	};
	m_animDataMap[CharacterAnimationState::WALK] = {
		AEGfxTextureLoad("Assets/Character/Battlemage Complete (Sprite Sheet)/Running/Battlemage Run.png"), 10, SpriteSheetOrientation::VERTICAL, 0.08f
	};
	m_animDataMap[CharacterAnimationState::JUMP] = {
		AEGfxTextureLoad("Assets/Character/Battlemage Complete (Sprite Sheet)/Jump Neutral/Battlemage Jump Neutral.png"), 12, SpriteSheetOrientation::VERTICAL, 0.1f
	};

	m_currentAnimState = CharacterAnimationState::IDLE;
	m_subImageIndex = 0;
	m_offset = 0.0f;
	m_elapsedTime = 0.0f;
}

void Animation::Update(f32 dt)
{
	const AnimData& currentData = m_animDataMap.at(m_currentAnimState);
	s32 totalFrames = currentData.frameCount;
	f32 frameDuration = currentData.frameDuration;

	m_elapsedTime += dt;
	if (m_elapsedTime >= frameDuration)
	{
		m_subImageIndex++;
		if (m_subImageIndex >= totalFrames)
		{
			m_animationFinished = true;
			m_subImageIndex = (m_currentAnimState == CharacterAnimationState::DEATH) ? totalFrames - 1 : 0;
		}
		m_offset = static_cast<f32>(m_subImageIndex) / totalFrames;
		m_elapsedTime = 0.0f;
	}
}

void Animation::ChangeAnimState(CharacterAnimationState newAnimState)
{
	if (m_currentAnimState == newAnimState && m_mesh != nullptr) 
		return;

	m_currentAnimState = newAnimState;
	m_subImageIndex = 0;
	m_offset = 0.0f;
	m_elapsedTime = 0.0f;
	m_animationFinished = false;

	if (m_mesh)
	{
		AEGfxMeshFree(m_mesh);
	}

	const AnimData& currentData = m_animDataMap.at(m_currentAnimState);
	f32 frameU = 1.0f;
	f32 frameV = 1.0f; 

	if (currentData.orientation == SpriteSheetOrientation::HORIZONTAL)
	{
		frameU = 1.0f / currentData.frameCount;
	}
	else 
	{
		frameV = 1.0f / currentData.frameCount;
	}

	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, frameV,
		0.5f, -0.5f, 0xFFFFFFFF, frameU, frameV,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
	);
	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, frameU, frameV,
		0.5f, 0.5f, 0xFFFFFFFF, frameU, 0.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
	);
	m_mesh = AEGfxMeshEnd();
}


void Animation::Draw(AEMtx33 transform)
{
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	const AnimData& currentData = m_animDataMap.at(m_currentAnimState);
	f32 translateX = 0.0f, translateY = 0.0f;

	if (currentData.orientation == SpriteSheetOrientation::HORIZONTAL)
	{
		translateX = m_offset;
	}
	else 
	{
		translateY = m_offset;
	}

	AEGfxTextureSet(currentData.pTexture, translateX, translateY);
	AEGfxSetTransform(transform.m);

	AEGfxMeshDraw(m_mesh, AE_GFX_MDM_TRIANGLES);
}


void Animation::Destroy()
{
	if (m_mesh)
	{
		AEGfxMeshFree(m_mesh);
		m_mesh = nullptr;
	}

	for (auto const& [key, val] : m_animDataMap)
	{
		if (val.pTexture)
		{
			AEGfxTextureUnload(val.pTexture);
		}
	}
	m_animDataMap.clear();
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
