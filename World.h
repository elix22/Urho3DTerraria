#pragma once


#include "SpriteBatch.h"


enum CellType : unsigned char
{
    CT_VOID = 0,
    CT_STONE,
    CT_DIRT,
    CT_SAND
};


struct Cell
{
    CellType type_;
};


class World : public Object
{
    URHO3D_OBJECT(World, Object);

public:
    World(Context* context);
    virtual ~World();
    void Generate(int width, int height);
    void Clear();
    void Draw(SpriteBatch* sb, Graphics* g, const Vector3& cameraPos);
    Cell* cells_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    Cell* GetCell(int col, int row);
    bool CellIsPassable(int col, int row);
    bool IsValid(int col, int row);
    Vector2 GetMousePosition();
};

#define CELL_SIZE 32
