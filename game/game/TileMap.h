#pragma once
#include <AEEngine.h>
#include <unordered_map>
#include <string>
#include "json.hpp"

struct Tile {
    s32 tileId;
};

struct TileCoord {
    s32 x, y;
    bool operator==(const TileCoord& other) const {
        return x == other.x && y == other.y;
    }
};

struct TileCoordHash {
    std::size_t operator()(const TileCoord& coord) const {
        return std::hash<int>()(coord.x) ^ (std::hash<int>()(coord.y) << 1);
    }
};

using SparseLayer = std::unordered_map<TileCoord, Tile, TileCoordHash>;
using SparseTileMap = std::unordered_map<std::string, SparseLayer>;

SparseTileMap loadSparseTileMapByLayer(const std::string& filename);