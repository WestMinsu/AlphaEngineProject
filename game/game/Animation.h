#pragma once
#include <AEEngine.h>
#include "ACharacter.h"
#include <map>

enum class SpriteSheetOrientation
{
	HORIZONTAL, 
	VERTICAL   
};

struct AnimData
{
	AEGfxTexture* pTexture = nullptr;
	s32 frameCount = 0;
	SpriteSheetOrientation orientation = SpriteSheetOrientation::VERTICAL;
	f32 frameDuration = 0.1f;
};

class Animation
{
public:
	Animation();
	~Animation();

	void Init();
	void Update(f32 dt);
	void ChangeAnimState(CharacterAnimationState newAnimState);
	void Draw(AEMtx33 transform);
	void Destroy();

	bool getAnimationFinished();
	void UpdateDeathTime(f32 dt);
	bool isRestartAnimDeathtoIDLE();

private:
	AEGfxVertexList* m_mesh;

	std::map<CharacterAnimationState, AnimData> m_animDataMap;
	CharacterAnimationState m_currentAnimState; 

	s32 m_subImageIndex;
	f32 m_offset;
	f32 m_elapsedTime;

	bool m_animationFinished = false;
	f32 m_deathTimer;
	const f32 m_restartDelay = 3.0f;
};