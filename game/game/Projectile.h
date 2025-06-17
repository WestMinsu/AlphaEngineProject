#pragma once
#include "AEEngine.h"
#include "Animation.h"
#include "ACharacter.h"

class Projectile
{
public:
	Projectile();
	~Projectile();

	void Init(AEVec2 startPos, CharacterDirection dir);
	void Update(f32 dt);
	void Draw();
	void Destroy();

	bool IsActive() const { return m_isActive; }
	void Deactivate() { m_isActive = false; }

	const AEVec2& GetPosition() const { return m_position; }
	const AEVec2& GetSize() const { return m_size; }

private:
	AEVec2 m_position;
	AEVec2 m_velocity;
	AEVec2 m_size;
	Animation m_animation;
	AnimData m_animData;
	bool m_isActive;
};