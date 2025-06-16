#include "AGameState.h"
#include "GameManager.h"
#include "MainGameState.h"
#include "Constants.h"
#include "TileMap.h"
#include "AssetManager.h"

MainGameState::MainGameState()
{
}

MainGameState::~MainGameState()
{

}

void MainGameState::Init()
{
	m_Player.Init({-100.f,0.f});
	m_TileMap = loadSparseTileMapByLayer("Assets/Maps/testMap..tmj");
	m_tileSet = assetManager.LoadImageAsset("Assets/FreeCuteTileset/Tileset.png");
}

void MainGameState::Update(f32 dt)
{
	if (AEInputCheckTriggered(AEVK_R))
	{
		GameManager::ChangeState(GameState::MAIN_MENU);
		return;
	}
	m_Player.Update(dt);
}

void MainGameState::Draw()
{
	m_Player.Draw();
}

void MainGameState::Exit()
{

}
