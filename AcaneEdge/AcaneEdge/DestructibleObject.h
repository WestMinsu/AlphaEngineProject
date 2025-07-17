#pragma once
#include "AEEngine.h"
#include "Animation.h"

class DestructibleObject
{
public:
	DestructibleObject();
	~DestructibleObject();

	void Init(AEVec2 position, const char* texturePath);
	void Update(f32 dt);
	void Draw();

	void TakeDamage(s32 damage);
	void Respawn();

	bool IsActive() const { return m_isActive; }
	const AEVec2& GetPosition() const { return m_position; }
	const AEVec2& GetSize() const { return m_size; }

private:
	AEVec2 m_position;
	AEVec2 m_size;
	s32 m_health;
	AEGfxTexture* m_pTexture;

	bool m_isActive;
};