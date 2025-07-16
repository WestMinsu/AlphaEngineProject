#include "DestructibleObject.h"
#include "Utility.h"
#include "AssetManager.h"
#include <iostream>

DestructibleObject::DestructibleObject()
{
	m_position = { 0,0 };
	m_size = { 100.f, 100.f };
	m_health = 10;
	m_pTexture = nullptr;
	m_isActive = true;
}

DestructibleObject::~DestructibleObject()
{
}

void DestructibleObject::Init(AEVec2 position, const char* texturePath)
{
	m_position = position;
	m_pTexture = LoadImageAsset(texturePath);
	m_isActive = true;
}

void DestructibleObject::Update(f32 dt)
{
}

void DestructibleObject::Draw()
{
	if (!m_isActive) return;

	DrawRect(m_position.x, m_position.y, m_size.x, m_size.y, 1, 1, 1, 1, m_pTexture);
}

void DestructibleObject::TakeDamage(s32 damage)
{
	if (!m_isActive) return;

	m_health -= damage;
	if (m_health <= 0)
	{
		m_isActive = false;
		std::cout << "A stone has been destroyed!" << std::endl;
	}
}

void DestructibleObject::Respawn()
{
	m_isActive = true;
	m_health = 10;
	std::cout << "A stone has respawned!" << std::endl;
}
