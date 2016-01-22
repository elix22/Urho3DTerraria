#include "Urho3DAll.h"
#include "Player.h"
#include "World.h"


Player::Player(Context* context) : LogicComponent(context)
{
    SetUpdateEventMask(USE_UPDATE);
}


float Player::GetSpeed()
{
    return 200.0f;
}


bool Player::CanMoveTo(float x, float y)
{
    if (x < size_.x_ / 2)
        return false;
    if (y > -size_.y_)
        return false;
    if (x >= world_->width_ * CELL_SIZE - size_.x_ / 2)
        return false;
    if (y <= -world_->height_ * CELL_SIZE)
        return false;

    float xLeft = x - size_.x_ / 2;
    float xRight = x + size_.x_ / 2;
    float yDown = y;
    float yUp = y + size_.y_;
    float yHalf = y + size_.y_ / 2;

    int col = int(xLeft / CELL_SIZE);
    int row = int(-yDown / CELL_SIZE);
    if (!world_->CellIsPassable(col, row))
        return false;

    col = int(x / CELL_SIZE);
    row = int(-yDown / CELL_SIZE);
    if (!world_->CellIsPassable(col, row))
        return false;

    col = int(xRight / CELL_SIZE);
    row = int(-yDown / CELL_SIZE);
    if (!world_->CellIsPassable(col, row))
        return false;

    col = int(xRight / CELL_SIZE);
    row = int(-yHalf / CELL_SIZE);
    if (!world_->CellIsPassable(col, row))
        return false;

    col = int(xRight / CELL_SIZE);
    row = int(-yUp / CELL_SIZE);
    if (!world_->CellIsPassable(col, row))
        return false;

    col = int(x / CELL_SIZE);
    row = int(-yUp / CELL_SIZE);
    if (!world_->CellIsPassable(col, row))
        return false;

    col = int(xLeft / CELL_SIZE);
    row = int(-yUp / CELL_SIZE);
    if (!world_->CellIsPassable(col, row))
        return false;

    col = int(xLeft / CELL_SIZE);
    row = int(-yHalf / CELL_SIZE);
    if (!world_->CellIsPassable(col, row))
        return false;

    return true;
}


void Player::Update(float timeStep)
{
    Input* input = GetSubsystem<Input>();

    Vector3 oldPos = node_->GetWorldPosition();
    float xPos = oldPos.x_;
    float yPos = oldPos.y_;

    AnimationController* ac = node_->GetComponent<AnimationController>();
    if (input->GetKeyDown('A') && !input->GetKeyDown('D'))
    {
        xPos -= GetSpeed() * timeStep;
        if (!CanMoveTo(xPos, yPos))
            xPos = oldPos.x_;
        node_->SetRotation(Quaternion(0, -90, 0));
    }
    if (input->GetKeyDown('D') && !input->GetKeyDown('A'))
    {
        xPos += GetSpeed() * timeStep;
        if (!CanMoveTo(xPos, yPos))
            xPos = oldPos.x_;
        node_->SetRotation(Quaternion(0, 90, 0));
    }

    if (xPos != oldPos.x_)
    {
        if (!ac->IsPlaying("Models/Jack_Walk.ani"))
            ac->PlayExclusive("Models/Jack_Walk.ani", 0, true, 0.1f);
    }
    else
    {
        ac->Stop("Models/Jack_Walk.ani", 0.3f);
        ac->SetTime("Models/Jack_Walk.ani", 0);
    }


    if (input->GetKeyDown('W') && !input->GetKeyDown('S') && onGround_)
    {
        verticalSpeed_ = 200.0f;
    }
    else
    {
        verticalSpeed_ -= 9.8f * timeStep * 32.0f;
    }

    if (verticalSpeed_ < -400.0f) // на слишком большой скорости будет проскакивать свозь преграды
        verticalSpeed_ = -400.0f;

    yPos += verticalSpeed_ * timeStep;

    if (!CanMoveTo(xPos, yPos))
    {
        yPos = oldPos.y_;
        if (verticalSpeed_ < 0)
            onGround_ = true;
        verticalSpeed_ = 0.0f;
    }
    else
    {
        onGround_ = false;
    }

    node_->SetWorldPosition(Vector3(xPos, yPos, oldPos.z_));

    if (input->GetMouseButtonDown(MOUSEB_LEFT))
    {
        Vector2 mPos = world_->GetMousePosition();
        int col = int(mPos.x_ / CELL_SIZE);
        int row = int(-mPos.y_ / CELL_SIZE);
        if (world_->IsValid(col, row))
        {
            world_->GetCell(col, row)->type_ = CT_VOID;
        }
    }
}


void Player::Init(World* world)
{
    world_ = world;
}
