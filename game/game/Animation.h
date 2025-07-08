#pragma once
#include "AEEngine.h"
#include <string>
#include <map>
#include "GameTypes.h" 

class Animation	
{
public:
	static int COUNT;
	Animation();
	~Animation();

	void Init();
	void Update(f32 dt);
	void Draw(AEMtx33 transform, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	void Destroy();

	void Play(CharacterAnimationState state, const AnimData& clipData);

	s32 GetCurrentFrame() const { return m_subImageIndex; }
	bool IsFinished() const { return m_isFinished; }
	CharacterAnimationState GetCurrentState() const { return m_currentState; }

private:
	AEGfxVertexList* m_mesh;
	AnimData m_currentClipData;
	CharacterAnimationState m_currentState;

	s32 m_subImageIndex;
	f32 m_elapsedTime;
	bool m_isFinished;
	int id;
};