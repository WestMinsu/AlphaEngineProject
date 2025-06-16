#include <fstream>
#include <iostream>
#include "TileMap.h"

SparseTileMap loadSparseTileMapByLayer(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    nlohmann::json j;
    file >> j;

    int width = j["width"];
    SparseTileMap map;

    for (const auto& layer : j["layers"]) {
        std::string layerName = layer["name"];
        const auto& data = layer["data"];

        SparseLayer sparseLayer;

        for (int i = 0; i < data.size(); ++i) {
            int tileID = data[i];
            if (tileID == 0) continue;  

            int x = i % width;
            int y = i / width;

            sparseLayer[{x, y}] = Tile{ tileID };
        }

        map[layerName] = std::move(sparseLayer);
    }

    return map;
}