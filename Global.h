#pragma once

#include "Urho3DAll.h"


enum GameState
{
    MAIN_MENU = 0,
    CREATING,
    LOADING,
    GAME
};



extern GameState gameState;


extern Node* cameraNode;


extern Texture2D* CellAtlas;
extern Texture2D* CellSand;
extern Texture2D* CellStone;


void LoadResources(ResourceCache* cache);
