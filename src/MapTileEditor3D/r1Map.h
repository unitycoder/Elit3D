#pragma once
#include "Resource.h"

#include <unordered_map>
#include <vector>

#include "m1MapEditor.h"

#include "int2.h"
#include "TypeVar.h"
#include "MapLayer.h"
#include "p1Tools.h"
#include "ExternalTools/JSON/json.hpp"

struct Node {
    Node(int x, int y) : x(x), y(y) {}
    int x;
    int y;

    inline bool operator==(const Node& n) const {
        return x == n.x && y == n.y;
    }
};

class r1Map :
    public Resource
{
    friend class m1MapEditor;
public:
    r1Map(const uint64_t& _uid);
    ~r1Map();

    void Save(const uint64_t& tileset);
    void Export(const uint64_t& tileset, Layer::DataTypeExport d, m1MapEditor::MapTypeExport t);
    void SaveInImage();
    void Load() override;
    void Unload() override;
    void Resize(int width, int height);
    void Edit(int layer, int row, int col, int brushSize, p1Tools::Tools tool, p1Tools::Shape shape, TILE_DATA_TYPE id, unsigned char g, unsigned char b);

    static void CreateNewMap(int width, int height, const char* path);

    void OnInspector() override;

    bool CheckBoundaries(const int2& point, int brushSize, p1Tools::Tools tool, p1Tools::Shape shape) const;

private:
    void LoadLayers(nlohmann::json& file);

private:
    int2 size = { -1, -1 };

    Properties properties;

    std::vector<Layer*> layers;
};

