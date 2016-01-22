#include "Global.h"


GameState gameState = GameState::MAIN_MENU;


Node* cameraNode = nullptr;


Texture2D* CellAtlas = nullptr;
Texture2D* CellSand = nullptr;
Texture2D* CellStone = nullptr;


void LoadResources(ResourceCache* cache)
{
    CellAtlas = cache->GetResource<Texture2D>("Urho2D/Box.png");
    CellSand = cache->GetResource<Texture2D>("Urho2D/Box.png");
    CellStone = cache->GetResource<Texture2D>("Urho2D/Ball.png");
}
