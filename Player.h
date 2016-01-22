// Центр игрока внизу посередине.

#pragma once

#include "World.h"

class Player : public LogicComponent
{
    URHO3D_OBJECT(Player, LogicComponent);

public:
    Player(Context* context);
    virtual void Update(float timeStep);
    void Init(World* world);

private:
    World* world_ = nullptr;
    Vector2 size_ = Vector2(28, 62);
    float verticalSpeed_ = 0.0f;
    bool onGround_ = false;

    float GetSpeed();
    bool CanMoveTo(float x, float y);
};

