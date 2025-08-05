#include "VisualEffect.h"
#include "Utility.h"

VisualEffect::VisualEffect()
{
	m_position = { 0,0 };
	m_size = { 200.f, 200.f }; 
	m_isActive = true;
}

VisualEffect::~VisualEffect()
{
}

void VisualEffect::Init(AEVec2 position, const AnimData& data)
{
	m_position = position;
	m_animation.Init();
	m_animation.Play(CharacterAnimationState::IDLE, data); 
}

void VisualEffect::Update(f32 dt)
{
	if (!m_isActive)
		return;

	m_animation.Update(dt);

	if (m_animation.IsFinished())
	{
		m_isActive = false;
	}
}

void VisualEffect::Draw()
{
	if (!m_isActive)
		return;

	AEMtx33 scale = { 0 };
	AEMtx33 rotate = { 0 };
	AEMtx33 translate = { 0 };
	AEMtx33 transform = { 0 };

	AEMtx33Trans(&translate, m_position.x, m_position.y);
	AEMtx33Rot(&rotate, 0);
	AEMtx33Scale(&scale, m_size.x, m_size.y);

	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	m_animation.Draw(transform);
}