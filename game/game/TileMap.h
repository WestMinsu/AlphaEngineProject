#pragma once
#include <AEEngine.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "json.hpp"

struct CollisionBox
{
    f32 x, y, width, height;
};

struct TilesetInfo {
    s32 firstGId;
    s32 tileWidth, tileHeight;
    s32 columns, tileCount;
    AEGfxTexture* tilesetTexture;
    s32 imageWidth, imageHeight;
    std::unordered_map<f32, std::vector<CollisionBox>> collisions;
    std::string name;
    

    bool contains(s32 globalID) const
    {
        return globalID >= firstGId && globalID < firstGId + tileCount;
    }
};

class TileMap
{
public:
    TileMap();
    TileMap(const std::string& mapfileDir, f32 tileScale = 1.f, f32 offsetX = 0.f, f32 offsetY = 0.f);
    ~TileMap();

    void Update(f32 dt);
    void Draw();
    void Destroy();

    friend bool checkCollisionTileMap(AEVec2 position, AEVec2 size);

    AEVec2 GetOffset();
    void SetOffset(f32 offsetX, f32 offsetY);

    s32 GetMapTotalWidth();
    s32 GetMapHeight();
    std::vector<std::vector<int>> GetLayer(s32 idx);
    
private:
    AEVec2 m_offset;
    s32 m_offsetCount;
    s32 m_mapWidth, m_mapHeight;
    s32 m_tileSize;
    f32 m_tileScale;
    
    nlohmann::json m_mapJson;

    AEGfxTexture* wireframe;
    AEGfxVertexList* wireframe_mesh;
    
    std::vector<TilesetInfo> m_tilesets;
    std::vector<std::vector<std::vector<int>>> m_layers;
    std::vector<CollisionBox> m_collisionBoxes;
    std::map<std::pair<f32, f32>, AEGfxVertexList*> m_meshes;
    
    void LoadJson(const char*);
    void LoadTilesets(const char*);
    void PrepareLayerData();
    void ExtractWorldColliders();
};

bool checkCollisionTileMap(AEVec2 position, AEVec2 size);
extern std::vector<TileMap> TileMaps;