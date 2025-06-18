#include <fstream>
#include <iostream>
#include "tinyxml2.h"
#include "TileMap.h"
#include "AssetManager.h"
#include "Constants.h"

TileMap::TileMap()
{
	m_tileSize = 16;
}

TileMap::~TileMap()
{
}

void TileMap::Init()
{
	LoadJson("Assets/Maps/testDemo.tmj");
	LoadTilesets("Assets/FreeCuteTileset/");
	PrepareLayerData();

}

// To Do 
void TileMap::Update(f32 dt)
{
	//
}

void TileMap::Draw()
{
	for (const auto& layer : m_layers)
	{
		for (int i = 0; i < layer.size(); i++)
		{
			s32 globalID = layer[i];
			if (globalID == 0) continue;
		
			const TilesetInfo* usedTileset = nullptr;
			for (const auto& ts : m_tilesets)
			{
				if (ts.contains(globalID))
				{
					usedTileset = &ts;
					break;
				}
			}
			if (!usedTileset) continue;

			s32 localID = globalID - usedTileset->firstGId;
			s32 tx = localID % usedTileset->columns;
			s32 ty = localID / usedTileset->columns;

			float u0 = (tx * usedTileset->tileWidth) / (float)usedTileset->imageWidth;
			float v0 = (ty * usedTileset->tileHeight) / (float)usedTileset->imageHeight;
			float u1 = ((tx + 1) * usedTileset->tileWidth) / (float)usedTileset->imageWidth;
			float v1 = ((ty + 1) * usedTileset->tileHeight) / (float)usedTileset->imageHeight;

			int x = (i % m_mapWidth) * m_tileSize - kHalfWindowWidth + m_tileSize/2.0f;
			int y = (m_mapHeight - 1 - (i / m_mapWidth)) * m_tileSize - kHalfWindowHeight + m_tileSize / 2.0f;

			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			AEGfxTextureSet(usedTileset->tilesetTexture, u0, v0);

			AEMtx33 scale = { 0 };
			AEMtx33Scale(&scale, m_tileSize, m_tileSize);

			AEMtx33 rotate = { 0 };
			AEMtx33Rot(&rotate, 0);

			AEMtx33 translate = { 0 };
			AEMtx33Trans(&translate, x, y);

			AEMtx33 transform = { 0 };
			AEMtx33Concat(&transform, &rotate, &scale);
			AEMtx33Concat(&transform, &translate, &transform);

			AEGfxSetTransform(transform.m);

			std::pair<f32, f32> keyPiar{usedTileset->imageWidth, usedTileset->imageHeight};
			AEGfxMeshDraw(m_meshes[keyPiar], AE_GFX_MDM_TRIANGLES);

		}
	}
}

void TileMap::Destroy()
{
	for (auto& m_mesh : m_meshes) 
	{
		AEGfxMeshFree(m_mesh.second);
	}
	m_meshes.clear();
}

void TileMap::LoadJson(const char* jsonfile)
{
	std::ifstream ifs(jsonfile);
	if (!ifs) throw std::runtime_error("Fail to read");
	ifs >> m_mapJson;

	m_mapWidth = m_mapJson["width"];
	m_mapHeight = m_mapJson["height"];
}

void TileMap::LoadTilesets(const char* tilesetDir)
{
	for (const auto& ts : m_mapJson["tilesets"])
	{
		TilesetInfo tilesetInfo;
		tilesetInfo.firstGId = ts["firstgid"];

		std::string tsxPath = tilesetDir + ts["source"].get<std::string>();

		tinyxml2::XMLDocument tilesetXML;

		if (tilesetXML.LoadFile(tsxPath.c_str()) != tinyxml2::XML_SUCCESS) {
			throw std::runtime_error("Fail to read xml " + tsxPath);
		}

		tinyxml2::XMLElement* tileset = tilesetXML.FirstChildElement("tileset");
		if (!tileset) throw std::runtime_error("Fail tileset: " + tsxPath);

		tileset->QueryIntAttribute("tilewidth", &tilesetInfo.tileWidth);
		tileset->QueryIntAttribute("tileheight", &tilesetInfo.tileHeight);
		tileset->QueryIntAttribute("tilecount", &tilesetInfo.tileCount);
		tileset->QueryIntAttribute("columns", &tilesetInfo.columns);
		tilesetInfo.name = tileset->Attribute("name");

		tinyxml2::XMLElement* image = tileset->FirstChildElement("image");
		if (!image) throw std::runtime_error("Fail image: " + tsxPath);
		std::string imagePath = tilesetDir + std::string(image->Attribute("source"));
		image->QueryIntAttribute("width", &tilesetInfo.imageWidth);
		image->QueryIntAttribute("height", &tilesetInfo.imageHeight);

		s32 channels;
		tilesetInfo.tilesetTexture = assetManager.LoadImageAsset(imagePath);
		std::pair<f32, f32> keyPair{ tilesetInfo.imageWidth , tilesetInfo.imageHeight};

		if (m_meshes.find(keyPair) == m_meshes.end()) {
			AEGfxVertexList* m_mesh;
			AEGfxTriAdd(
				-0.5f, -0.5f, 0xFFFFFFFF, 0.f, m_tileSize/ (f32)tilesetInfo.imageHeight,
				0.5f, -0.5f, 0xFFFFFFFF, m_tileSize/ (f32)tilesetInfo.imageWidth, m_tileSize / (f32)tilesetInfo.imageHeight,
				0.5f, 0.5f, 0xFFFFFFFF, m_tileSize / (f32)tilesetInfo.imageWidth, 0.f);

			AEGfxTriAdd(
				0.5f, 0.5f, 0xFFFFFFFF, m_tileSize / (f32)tilesetInfo.imageWidth, 0.f,
				-0.5f, 0.5f, 0xFFFFFFFF, 0.f, 0.f,
				-0.5f, -0.5f, 0xFFFFFFFF, 0.f, m_tileSize / (f32)tilesetInfo.imageHeight);

			m_mesh = AEGfxMeshEnd();

			m_meshes[keyPair] = m_mesh;
		}

		m_tilesets.push_back(tilesetInfo);
	}
}

void TileMap::PrepareLayerData()
{
	for (const auto& layer : m_mapJson["layers"])
	{
		if (layer["type"] != "tilelayer") continue;
		std::vector<int> layerData = layer["data"].get<std::vector<int>>();
		m_layers.push_back(layerData);
	}
}
