#include <fstream>
#include <iostream>
#include "tinyxml2.h"
#include "TileMap.h"
#include "AssetManager.h"
#include "Constants.h"

TileMap::TileMap()
{
	m_tileSize = 16;
	m_tileScale = 2.f;
}

TileMap::TileMap(std::string mapfileDir, f32 tileScale, f32 x, f32 y)
{
	m_tileSize = 16;
	m_tileScale = tileScale;
	m_offset = AEVec2{ x, y };
	LoadJson(mapfileDir.c_str());
	LoadTilesets("Assets/FreeCuteTileset/");
	PrepareLayerData();
}

TileMap::~TileMap()
{
}

void TileMap::Update(f32 dt)
{
	float mapPixelWidth = m_mapWidth * m_tileSize * m_tileScale;
	f32 xCAM, yCAM;
	AEGfxGetCamPosition(&xCAM, &yCAM);

	if (m_offset.x + mapPixelWidth < xCAM ) {
		m_offset.x += mapPixelWidth * 2;
	}
	else if (m_offset.x > xCAM + kWindowWidth) {
		m_offset.x -= mapPixelWidth * 2;
	}

	//std::cout << m_offset.x << std::endl;
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

			int x = (i % m_mapWidth) * m_tileSize * m_tileScale - kHalfWindowWidth + m_tileSize * m_tileScale / 2.f + m_offset.x;
			int y = (m_mapHeight - 1 - (i / m_mapWidth)) * m_tileSize*m_tileScale - kHalfWindowHeight + m_tileSize * m_tileScale / 2.f;

			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			AEGfxTextureSet(usedTileset->tilesetTexture, u0, v0);

			AEMtx33 scale = { 0 };
			AEMtx33Scale(&scale, m_tileSize * m_tileScale, m_tileSize * m_tileScale);
			//AEMtx33Scale(&scale, 32.f, 32.f);

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

bool TileMap::checkCollisionTileMap(AEVec2 position, AEVec2 size)
{
	bool result = false;

	int tileX = (int)((position.x + kHalfWindowWidth) / (m_tileSize*m_tileScale)) % m_mapWidth;
	int tileY = (int)((position.y + kHalfWindowHeight) / (m_tileSize * m_tileScale)) % m_mapHeight;
	if (m_offset.x / (m_tileSize*m_tileScale) <= tileX
		&& m_offset.x / (m_tileSize*m_tileScale) + m_mapWidth >= tileX)

	{
		f32 tileID = m_layers[0][tileX+tileY*m_mapWidth];
		const TilesetInfo* tilesetInfo = nullptr;
		for (const auto& ts : m_tilesets)
		{
			if (ts.contains(tileID))
			{
				tilesetInfo = &ts;
				break;
			}
		}
		
		if (tilesetInfo && tilesetInfo->collisions.find(tileID) != tilesetInfo->collisions.end())
		{
			for (auto& box : tilesetInfo->collisions.at(tileID))
			{
				if ( tileX + size.x < box.x ) {
					result = true;
					break;
				}
			}
		}
	}


	return result;
}

s32 TileMap::GetMapTotalWidth()
{
	return m_mapWidth*m_tileSize*m_tileScale;
}

s32 TileMap::GetMapHeight()
{
	return m_mapHeight;
}

std::vector<int> TileMap::GetLayer(s32 idx)
{
	return m_layers[idx];
}

AEVec2 TileMap::GetOffset()
{
	return m_offset;
}

void TileMap::SetOffset(f32 offsetX, f32 offsetY)
{
	m_offset = AEVec2{offsetX, offsetY};
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
		tilesetInfo.tilesetTexture = LoadImageAsset(imagePath);
		std::pair<f32, f32> keyPair{ tilesetInfo.imageWidth , tilesetInfo.imageHeight};

		if (m_meshes.find(keyPair) == m_meshes.end()) {
			AEGfxVertexList* m_mesh;
			AEGfxTriAdd(
				-0.5f, -0.5f, 0xFFFFFFFF, 0.f, m_tileSize/ (f32)tilesetInfo.imageHeight,
				0.5f, -0.5f, 0xFFFFFFFF, m_tileSize / (f32)tilesetInfo.imageWidth, m_tileSize / (f32)tilesetInfo.imageHeight,
				0.5f, 0.5f, 0xFFFFFFFF, m_tileSize / (f32)tilesetInfo.imageWidth, 0.f);

			AEGfxTriAdd(
				0.5f, 0.5f, 0xFFFFFFFF, m_tileSize / (f32)tilesetInfo.imageWidth, 0.f,
				-0.5f, 0.5f, 0xFFFFFFFF, 0.f, 0.f,
				-0.5f, -0.5f, 0xFFFFFFFF, 0.f, m_tileSize / (f32)tilesetInfo.imageHeight);

			m_mesh = AEGfxMeshEnd();

			m_meshes[keyPair] = m_mesh;
		}

		for (auto* tile = tileset->FirstChildElement("tile");tile;tile = tile->NextSiblingElement("tile"))
		{
			s32 tileID = tile->IntAttribute("id");

			auto* objGroup = tile->FirstChildElement("objectgroup");
			if (!objGroup) continue;

			for (auto* obj = objGroup->FirstChildElement("object"); obj; obj = obj->NextSiblingElement("object"))
			{
				f32 x = obj->FloatAttribute("x");
				f32 y= obj->FloatAttribute("y");
				f32 w = obj->FloatAttribute("width");
				f32 h = obj->FloatAttribute("height");

				tilesetInfo.collisions[tileID].push_back({x, y, w*2, h*2});
			}
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

