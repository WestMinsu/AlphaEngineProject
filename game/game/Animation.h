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

	float GetAnimationDuration(CharacterAnimationState state) const;
	bool getAnimationFinished();
	void UpdateDeathTime(f32 dt);
	bool isRestartAnimDeathtoIDLE();
	
	s32 GetCurrentFrame() const;
private:
	AEGfxVertexList* m_mesh;

	std::map<CharacterAnimationState, AnimData> m_animDataMap;
	CharacterAnimationState m_currentAnimState; 

	s32 m_subImageIndex;
	f32 m_offset;
	f32 m_elapsedTime;

	bool m_animationFinished;
	f32 m_deathTimer;
	const f32 m_restartDelay = 3.0f;
};
