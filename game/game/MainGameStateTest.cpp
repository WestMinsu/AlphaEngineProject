#include "MainGameStateTest.h"
#include "GameManager.h"
#include "Constants.h"
#include <iostream>
#include <algorithm>
#include "Utility.h"
#include "AssetManager.h"

MainGameStateTest::MainGameStateTest()
{
}
MainGameStateTest::~MainGameStateTest() {}

void MainGameStateTest::Init()
{
	AEGfxSetCamPosition(0.f, 0.f);
	m_Player.Init({ -kHalfWindowWidth + 200.f, 0.f });

	TileMaps.push_back(TileMap("Assets/Maps/test0_32.tmj", 2.f));
	TileMaps.push_back(TileMap("Assets/Maps/test1_32.tmj", 2.f, TileMaps[0].GetMapTotalWidth()));
}

void MainGameStateTest::Update(f32 dt)
{
	for (auto& tm : TileMaps)
	{
		tm.Update(dt);
	}

	m_Player.Update(dt);

	if (m_Player.GetPosition().x > 0.f)
	{
		AEGfxSetCamPosition(m_Player.GetPosition().x, 0.f);
	}
}

void MainGameStateTest::Draw()
{
	for (auto tm : TileMaps)
	{
		tm.Draw();
	}

	m_Player.Draw();
}

void MainGameStateTest::Exit()
{
	for (auto& tm : TileMaps)
	{
		tm.Destroy();
	}
	TileMaps.clear();

	m_Player.Destroy();

	AEGfxSetCamPosition(0.f, 0.f);
}
