#include "Background.h"
#include "AssetManager.h"

Background::Background()
	: m_position{0.f, 0.f}
{
	m_meshBG = nullptr;
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

void Background::Update()
{

}

void Background::Draw()
{



}
