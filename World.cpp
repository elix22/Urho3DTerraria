#include "Urho3DAll.h"
#include "World.h"
#include "Global.h"
#include "SpriteBatch.h"


World::World(Context* context) : Object(context)
{
}


World::~World()
{
}


void World::Clear()
{
    if (cells_)
    {
        delete[] cells_;
        cells_ = nullptr;
    }
}

bool World::CellIsPassable(int col, int row)
{
    Cell* cell = GetCell(col, row);
    if (cell->type_ == CT_VOID)
        return true;
    return false;
}

void World::Generate(int width, int height)
{
    width_ = width;
    height_ = height;
    cells_ = new Cell[width * height];
    for (int i = 0; i < width * height; i++)
        cells_[i].type_ = CT_VOID;
    for (int y = 0; y < height_; y++)
    {
        for (int x = 0; x < width_; x++)
        {
            if (x == 0 || y == 0 || x == width_ - 1 || y == height_ - 1)
            {
                int r = Random(2);
                if (r == 0)
                    GetCell(x, y)->type_ = CT_VOID;
                else if (r == 1)
                    GetCell(x, y)->type_ = CT_STONE;
                continue;
            }
        }
    }
    for (int y = 10; y < height_ - 1; y++)
    {
        for (int x = 1; x < width_ - 1; x++)
        {
            int r = Random(2);
            if (r == 0)
                GetCell(x, y)->type_ = CT_VOID;
            else if (r == 1)
                GetCell(x, y)->type_ = CT_SAND;
            continue;
        }
    }
}


void World::Draw(SpriteBatch* sb, Graphics* g, const Vector3& cameraPos)
{
    IntRect viewport = g->GetViewport();
    int halfWidth = viewport.Width() / 2;
    int halfHeight = viewport.Height() / 2;

    int colStart = int((cameraPos.x_ - halfWidth) / CELL_SIZE);
    int colEnd = int((cameraPos.x_ + halfWidth) / CELL_SIZE) + 1;
    int rowStart = int((-cameraPos.y_ - halfHeight) / CELL_SIZE);   // в массиве Y возрастает вниз,
    int rowEnd = int((-cameraPos.y_ + halfHeight) / CELL_SIZE) + 1; // а в пространстве вверх

    if (colStart < 0)
        colStart = 0;
    if (rowStart < 0)
        rowStart = 0;
    if (colEnd >= width_)
        colEnd = width_ - 1;
    if (rowEnd >= height_)
        rowEnd = height_ - 1;

    for (int row = rowStart; row <= rowEnd; row++)
    {
        for (int col = colStart; col <= colEnd; col++)
        {
            Rect dest(col * CELL_SIZE, -row * CELL_SIZE, col * CELL_SIZE + CELL_SIZE, -row * CELL_SIZE - CELL_SIZE);
            Rect src(0, 0, CELL_SIZE, CELL_SIZE);

            if (GetCell(col, row)->type_ == CT_SAND)
                sb->Draw(CellSand, dest, src, Color::WHITE);
            else if (GetCell(col, row)->type_ == CT_STONE)
                sb->Draw(CellStone, dest, src, Color::WHITE);
        }
    }
}


Cell* World::GetCell(int col, int row)
{
    assert(col >= 0 && row >= 0 && col < width_ && row < height_);
    return &cells_[row * width_ + col];
}


bool World::IsValid(int col, int row)
{
    if (col < 0 || row < 0)
        return false;
    if (col >= width_ || row >= height_)
        return false;
    return true;
}


Vector2 World::GetMousePosition() // world position of cursor
{
    Input* input = GetSubsystem<Input>();
    IntVector2 pos = input->GetMousePosition();
    Graphics* gr = GetSubsystem<Graphics>();
    float mx = cameraNode->GetPosition().x_ + pos.x_ - gr->GetWidth() / 2;
    float my = cameraNode->GetPosition().y_ - pos.y_ + gr->GetHeight() / 2;
    return Vector2(mx, my);
}
