#include "Background.h"
#include "AssetManager.h"
#include "Utility.h"
#include "Constants.h"

Background::Background()
	: m_position{0.f, 0.f}
{
}

Background::~Background()
{
	
}

void Background::Init()
{
	m_backgrounds.push_back(assetManager.LoadImageAsset("Assets/FreeCuteTileset/BG1.png"));
	m_backgrounds.push_back(assetManager.LoadImageAsset("Assets/FreeCuteTileset/BG2.png"));
	m_backgrounds.push_back(assetManager.LoadImageAsset("Assets/FreeCuteTileset/BG3.png"));

}

void Background::Update(f32 dt)
{

}

void Background::Draw()
{
	for (auto& texBG : m_backgrounds)
	{
		DrawRect(m_position.x, m_position.y, kWindowWidth, kWindowHeight, 1.f, 1.f, 1.f, 1.f, texBG);
	}

}

void Background::Destroy()
{

}
