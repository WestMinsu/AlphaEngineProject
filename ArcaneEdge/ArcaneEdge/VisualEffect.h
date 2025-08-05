#pragma once
#include "AEEngine.h"
#include "Animation.h"

class VisualEffect
{
public:
	VisualEffect();
	~VisualEffect();

	void Init(AEVec2 position, const AnimData& data);
	void Update(f32 dt);
	void Draw();

	bool IsActive() const { return m_isActive; }

	Animation& GetAnimation()
	{
		return m_animation;
	}
	const AEVec2& GetPosition() const
	{
		return m_position;
	}
	const AEVec2& GetSize() const
	{
		return m_size;
	}

private:
	AEVec2 m_position;
	AEVec2 m_size;
	Animation m_animation;
	bool m_isActive;
};