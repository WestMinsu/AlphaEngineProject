#include <fstream>
#include <iostream>
#include "tinyxml2.h"
#include "TileMap.h"
#include "AssetManager.h"
#include "Constants.h"

std::vector<TileMap> TileMaps;

TileMap::TileMap()
{
	m_offsetCount = 1;
	m_tileSize = 16;
	m_tileScale = 2.f;
}

TileMap::TileMap(const std::string& mapfileDir, f32 tileScale, f32 x, f32 y)
{
	m_offsetCount = 1;
	m_tileSize = 16;
	m_tileScale = tileScale;
	m_offset = AEVec2{ x, y };
	LoadJson(mapfileDir.c_str());
	LoadTilesets("Assets/FreeCuteTileset/");
	PrepareLayerData();
	ExtractWorldColliders();

	for (auto& box : m_collisionBoxes)
	{
		std::cout << "Box: " << box.x / 32 << ", " << box.y /32 << std::endl;
	}
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
		m_offsetCount++;
	}
	else if (m_offset.x > xCAM + kWindowWidth) {
		m_offset.x -= mapPixelWidth * 2;
		m_offsetCount--;
	}
}

void TileMap::Draw()
{
	for (const auto& layer : m_layers)
	{
		for (int row = 0; row < m_mapHeight; row++)
		{
			for (int col = 0; col < m_mapWidth; col++)
			{
				s32 globalID = layer[row][col];
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

				int x = col * m_tileSize * m_tileScale - kHalfWindowWidth + m_tileSize * m_tileScale / 2.f + m_offset.x;
				int y = row * m_tileSize * m_tileScale - kHalfWindowHeight + m_tileSize * m_tileScale / 2.f;

				AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
				AEGfxTextureSet(usedTileset->tilesetTexture, u0, v0);

				AEMtx33 scale = { 0 };
				AEMtx33Scale(&scale, m_tileSize * m_tileScale, m_tileSize * m_tileScale);

				AEMtx33 rotate = { 0 };
				AEMtx33Rot(&rotate, 0);

				AEMtx33 translate = { 0 };
				AEMtx33Trans(&translate, x, y);

				AEMtx33 transform = { 0 };
				AEMtx33Concat(&transform, &rotate, &scale);
				AEMtx33Concat(&transform, &translate, &transform);

				AEGfxSetTransform(transform.m);

				std::pair<f32, f32> keyPiar{ usedTileset->imageWidth, usedTileset->imageHeight };
				AEGfxMeshDraw(m_meshes[keyPiar], AE_GFX_MDM_TRIANGLES);
			}
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

bool checkCollisionTileMap(AEVec2 position, AEVec2 size)
{
	bool result = false;

	for (auto& tileMap : TileMaps) {
		int playerLeft = (int)(position.x - (size.x / 4.f) + kHalfWindowWidth) % (2*tileMap.GetMapTotalWidth());
		int playerRight = (int)(position.x + (size.x / 4.f) + kHalfWindowWidth) % (2 * tileMap.GetMapTotalWidth());
		int playerTop = position.y + kHalfWindowHeight;
		int playerBottom = position.y - (size.y * 1/ 2.f) + kHalfWindowHeight;

		
		//if (tileTop > tileMap.GetMapHeight()) return result;

		/*std::cout << "[" << tileLeft << ", "
			<< tileBottom << "] : "
			<< tileMap.m_layers[0][tileBottom][tileLeft] << std::endl;*/

		if (!(tileMap.m_offset.x > playerRight
			|| tileMap.m_offset.x + tileMap.GetMapTotalWidth() < playerLeft))
		{
			int tileLeft = (int)(playerLeft / (tileMap.m_tileSize * tileMap.m_tileScale));
			int tileRight = (int)(playerRight / (tileMap.m_tileSize * tileMap.m_tileScale));
			int tileTop = playerTop / (tileMap.m_tileSize * tileMap.m_tileScale);
			int tileBottom = playerBottom / (tileMap.m_tileSize * tileMap.m_tileScale);

			for (int ty = tileBottom; ty <= tileTop; ty++)
			{
				for (int tx = tileLeft; tx <= tileRight; tx++) {
					if (ty >= tileMap.m_mapHeight ||
						ty < 0 ||
						tx >= tileMap.m_mapWidth ||
						tx < 0 ) continue;
					f32 tileID = tileMap.m_layers[0][ty][tx];
					/*std::cout << "[" << tx << ", "
						<< ty << "] : "
						<< tileMap.m_layers[0][tx + ty * tileMap.m_mapWidth] << std::endl;*/

					const TilesetInfo* tilesetInfo = nullptr;
					for (const auto& ts : tileMap.m_tilesets)
					{
						if (ts.contains(tileID))
						{
							tilesetInfo = &ts;
							break;
						}
					}

					if (tilesetInfo && tilesetInfo->collisions.find(tileID) != tilesetInfo->collisions.end())
					{
						for (auto& box : tileMap.m_collisionBoxes)
						{
							if (!(playerRight < box.x ||
								playerLeft > box.x + box.width ||
								playerTop < box.y ||
								playerBottom > box.y + box.height))
							{
								result = true;
								break;
							}
						}
					}

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

std::vector<std::vector<int>> TileMap::GetLayer(s32 idx)
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

			std::vector<CollisionBox> boxes;

			auto* objGroup = tile->FirstChildElement("objectgroup");
			if (!objGroup) continue;

			for (auto* obj = objGroup->FirstChildElement("object"); obj; obj = obj->NextSiblingElement("object"))
			{
				f32 x = obj->FloatAttribute("x");
				f32 y= obj->FloatAttribute("y");
				f32 w = obj->FloatAttribute("width");
				f32 h = obj->FloatAttribute("height");

				boxes.push_back({ x, y, w * m_tileScale, h * m_tileScale });
			}
			tilesetInfo.collisions[tilesetInfo.firstGId + tileID] = boxes;

		}
		m_tilesets.push_back(tilesetInfo);
	}
}

void TileMap::PrepareLayerData()
{
	for (const auto& layer : m_mapJson["layers"])
	{
		if (layer["type"] != "tilelayer") continue;

		std::vector<std::vector<int>> layerData(m_mapHeight, std::vector<int>(m_mapWidth,0));
		const auto& data = layer["data"];

		for (int i = 0; i < data.size(); i++)
		{
			int x = i % m_mapWidth;
			int y = i / m_mapWidth;
			layerData[m_mapHeight - 1 - y][x] = data[i];
		}

		m_layers.push_back(layerData);
	}
}

void TileMap::ExtractWorldColliders()
{
	for (auto& layer : m_layers)
	{
		for (int y = 0; y < m_mapHeight; y++)
		{
			for (int x = 0; x < m_mapWidth; x++) {
				int globalID = layer[y][x];
				if (globalID == 0) continue;

				for (auto& tileset : m_tilesets)
				{
					auto it = tileset.collisions.find(globalID);
					if (it == tileset.collisions.end()) continue;

					for (auto& box : it->second)
					{
						CollisionBox worldBox;
						worldBox.x = x * m_tileSize * m_tileScale + box.x + m_offset.x;
						worldBox.y = y * m_tileSize * m_tileScale + box.y;
						worldBox.width = box.width;
						worldBox.height = box.height;

						m_collisionBoxes.push_back(worldBox);
					}
				}
			}
		}
	}
}
