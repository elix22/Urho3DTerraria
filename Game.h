#pragma once


#include "SpriteBatch.h"
#include "World.h"
#include "MainMenu.h"


class Game : public Application
{
    URHO3D_OBJECT(Game, Application);

public:
    SharedPtr<MainMenu> mainMenu_;

    
    Game(Context* context);

    virtual void Setup();
    virtual void Start();

protected:
    SharedPtr<Scene> scene_;
    float yaw_;
    float pitch_;

private:
    SpriteBatch* spriteBatch_;
    void CreateScene();
    void SetupViewport();
    void SubscribeToEvents();
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    void HandlePostUpdate(StringHash eventType, VariantMap& eventData);
    void HandleEndViewRender(StringHash eventType, VariantMap& eventData);

    World* world_ = nullptr;
    Node* playerNode_ = nullptr;
};