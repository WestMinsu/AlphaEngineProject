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
	m_Player.Init({ -kHalfWindowWidth + 200.f, 50.f });

	TileMaps.push_back(TileMap("Assets/Maps/test0_32.tmj", 2.f));
	TileMaps.push_back(TileMap("Assets/Maps/test1_32.tmj", 2.f, TileMaps[0].GetMapTotalWidth()));
}

void MainGameStateTest::Update(f32 dt)
{
	for (auto& tm : TileMaps)
	{
		tm.Update(dt);
	}

	// 플레이어의 타일 좌표를 찾아
	// 그 타일 좌표에 박스를 그려봐
	// 충돌 체크를 해
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

	static bool toggleDebugDraw = false;
	
	if (AEInputCheckTriggered(AEVK_1))
	{
		toggleDebugDraw = !toggleDebugDraw;
	}

	if (toggleDebugDraw)
	{
		f32 green_color[] = { 0, 1, 0 };
		f32 blue_color[] = { 0, 0, 1 };
		f32 red_color[] = { 1, 0, 0 };

		for (auto& curr_tilemap : TileMaps)
		{
			for (int row = 0; row < curr_tilemap.GetMapHeight(); row++)
			{
				for (int col = 0; col < 58; col++)
				{
					AEVec2 tile_position = curr_tilemap.GetTileWorldPosAt(col, row);

					if (curr_tilemap.HasTile(col, row))
					{
						curr_tilemap.DrawRect(tile_position.x, tile_position.y, red_color);
					}
				}
			}
		}
	}

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
