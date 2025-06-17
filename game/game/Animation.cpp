#include "Animation.h"

Animation::Animation() : m_mesh(nullptr), m_subImageIndex(0), m_elapsedTime(0.0f), m_isFinished(false), m_currentState(CharacterAnimationState::IDLE)
{
	m_currentClipData.pTexture = nullptr;
}

Animation::~Animation()
{
	Destroy();
}

void Animation::Init()
{
	m_mesh = nullptr;
}

void Animation::Play(CharacterAnimationState state, const AnimData& clipData)
{
	if (m_mesh != nullptr && m_currentState == state && !m_isFinished)
	{
		return;
	}

	m_currentState = state;

	bool needsRebuild = false;
	if (!m_mesh || m_currentClipData.pTexture != clipData.pTexture || m_currentClipData.frameCount != clipData.frameCount || m_currentClipData.orientation != clipData.orientation)
	{
		needsRebuild = true;
	}

	m_currentClipData = clipData;

	if (needsRebuild)
	{
		if (m_mesh)
		{
			AEGfxMeshFree(m_mesh);
		}

		float frameU = 1.0f;
		float frameV = 1.0f;

		if (m_currentClipData.frameCount > 1) {
			if (m_currentClipData.orientation == SpriteSheetOrientation::HORIZONTAL) frameU = 1.0f / m_currentClipData.frameCount;
			else frameV = 1.0f / m_currentClipData.frameCount;
		}

		AEGfxMeshStart();
		AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, frameV, 0.5f, -0.5f, 0xFFFFFFFF, frameU, frameV, -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
		AEGfxTriAdd(0.5f, -0.5f, 0xFFFFFFFF, frameU, frameV, 0.5f, 0.5f, 0xFFFFFFFF, frameU, 0.0f, -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
		m_mesh = AEGfxMeshEnd();
	}

	m_subImageIndex = 0;
	m_elapsedTime = 0.0f;
	m_isFinished = false;
}

void Animation::Update(f32 dt)
{
	if (!m_currentClipData.pTexture) return;
	if (!m_currentClipData.loop && m_isFinished) return;
	if (m_currentClipData.frameCount <= 1) return;

	m_elapsedTime += dt;
	if (m_elapsedTime >= m_currentClipData.frameDuration)
	{
		m_elapsedTime -= m_currentClipData.frameDuration;

		if (!m_isFinished) m_subImageIndex++;

		if (m_subImageIndex >= m_currentClipData.frameCount)
		{
			if (m_currentClipData.loop) {
				m_subImageIndex = 0;
			}
			else {
				m_isFinished = true;
				m_subImageIndex = m_currentClipData.frameCount - 1;
			}
		}
	}
}

void Animation::Draw(AEMtx33 transform)
{
	if (!m_currentClipData.pTexture || !m_mesh) return;

	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	float offsetX = 0.0f;
	float offsetY = 0.0f;

	if (m_currentClipData.frameCount > 0)
	{
		if (m_currentClipData.orientation == SpriteSheetOrientation::HORIZONTAL) {
			offsetX = static_cast<float>(m_subImageIndex) / m_currentClipData.frameCount;
		}
		else {
			offsetY = static_cast<float>(m_subImageIndex) / m_currentClipData.frameCount;
		}
	}

	AEGfxTextureSet(m_currentClipData.pTexture, offsetX, offsetY);
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
}