#pragma once


#include "Urho3DAll.h"


class MainMenu : public LogicComponent
{
    URHO3D_OBJECT(MainMenu, LogicComponent);


public:


    SharedPtr<Scene> scene_;
    Node* cameraNode_;
    SharedPtr<Viewport> viewport_;


    MainMenu(Context* context) : LogicComponent(context)
    {
        Localization* l10n = GetSubsystem<Localization>();
        ResourceCache* cache = GetSubsystem<ResourceCache>();
        
        scene_ = new Scene(context);
        scene_->CreateComponent<Octree>();

        Zone* zone = scene_->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
        zone->SetAmbientColor(Color(0.5f, 0.5f, 0.5f));
        zone->SetFogColor(Color(0.4f, 0.5f, 0.8f));
        zone->SetFogStart(1.0f);
        zone->SetFogEnd(100.0f);

        Node* planeNode = scene_->CreateChild("Plane");
        planeNode->SetScale(Vector3(300.0f, 1.0f, 300.0f));
        StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
        planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
        planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

        Node* lightNode = scene_->CreateChild("DirectionalLight");
        lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
        Light* light = lightNode->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetColor(Color(0.8f, 0.8f, 0.8f));

        cameraNode_ = scene_->CreateChild("Camera");
        cameraNode_->CreateComponent<Camera>();
        cameraNode_->SetPosition(Vector3(0.0f, 10.0f, -30.0f));

        Node* text3DNode = scene_->CreateChild("Text3D");
        text3DNode->SetPosition(Vector3(0.0f, 0.1f, 30.0f));
        Text3D* text3D = text3DNode->CreateComponent<Text3D>();

        text3D->SetText(l10n->Get("Language"));

        text3D->SetFont(cache->GetResource<Font>("Fonts/Ubuntu-B.ttf"), 30);
        text3D->SetColor(Color::BLACK);
        text3D->SetAlignment(HA_CENTER, VA_BOTTOM);
        text3DNode->SetScale(15);

        viewport_ = new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>());

        SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MainMenu, HandleUpdate));
        SubscribeToEvent(E_CHANGELANGUAGE, URHO3D_HANDLER(MainMenu, HandleChangeLanguage));
    }


    void SetCurrent()
    {
        Renderer* renderer = GetSubsystem<Renderer>();
        renderer->SetViewport(0, viewport_);
    }


    void HandleUpdate(StringHash eventType, VariantMap& eventData)
    {
        using namespace Update;
        float timeStep = eventData[P_TIMESTEP].GetFloat();
        Input* input = GetSubsystem<Input>();
        const float MOUSE_SENSITIVITY = 0.1f;
        IntVector2 mouseMove = input->GetMousePosition();
        float yaw = mouseMove.x_ * 0.1f;
        float pitch = mouseMove.y_ * 0.1f;
        cameraNode_->SetRotation(Quaternion(pitch, yaw, 0.0f));
    }


    void HandleChangeLangButtonPressed(StringHash eventType, VariantMap& eventData)
    {

    }


    void HandleChangeLanguage(StringHash eventType, VariantMap& eventData)
    {
    }
};
