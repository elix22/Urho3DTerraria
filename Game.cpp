#include "Urho3DAll.h"
#include "Game.h"
#include "World.h"
#include "Global.h"
#include "Player.h"
#include "MainMenu.h"


URHO3D_DEFINE_APPLICATION_MAIN(Game)


Game::Game(Context* context) :
Application(context),
yaw_(0.0f),
pitch_(0.0f)
{
}


void Game::Setup()
{
    engineParameters_["WindowTitle"] = GetTypeName();
    //engineParameters_["LogName"] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
    engineParameters_["FullScreen"] = false;
    engineParameters_["Headless"] = false;
    engineParameters_["WindowWidth"] = 800;
    engineParameters_["WindowHeight"] = 600;
    engineParameters_["ResourcePaths"] = "Data;CoreData;MyData";
}


void Game::Start()
{
    context_->RegisterFactory<Player>();
    context_->RegisterFactory<MainMenu>();

    GetSubsystem<Localization>()->LoadJSONFile("Strings.json");
    SetRandomSeed(Time::GetSystemTime());
    LoadResources(GetSubsystem<ResourceCache>());

    world_ = new World(context_);
    world_->Generate(300, 300);

    mainMenu_ = new MainMenu(context_);
    

    CreateScene();
    SetupViewport();
    SubscribeToEvents();
    spriteBatch_ = new SpriteBatch(context_);
    spriteBatch_->Init(3000, cameraNode->GetComponent<Camera>());
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);

    // OS Cursor первый тип курсора
    GetSubsystem<Input>()->SetMouseVisible(true, true); // используем это
    //GetSubsystem<Input>()->SetMouseMode(MouseMode::MM_WRAP);
    
    // Internal cursor заменяет курсор ОС, если включен
    /*UI* ui = GetSubsystem<UI>();
    SharedPtr<Cursor> cursor(new Cursor(context_));
    cursor->SetStyleAuto(xmlFile);
    ui->SetCursor(cursor);
    //cursor->SetVisible(false);
    cursor->SetTexture(CellAtlas); // IT WORKS
    Graphics* graphics = GetSubsystem<Graphics>();
    cursor->SetPosition(graphics->GetWidth() / 2, graphics->GetHeight() / 2);
  */  

    //mainMenu_->SetCurrent();

}


void Game::SetupViewport()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode->GetComponent<Camera>()));
    renderer->SetViewport(0, viewport);
}


void Game::CreateScene()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    Node* planeNode = scene_->CreateChild("Plane");
    planeNode->SetScale(Vector3(100.0f, 1.0f, 100.0f));
    StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
    planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
    planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));

    Node* lightNode = scene_->CreateChild("DirectionalLight");
    lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
    Light* light = lightNode->CreateComponent<Light>();
    light->SetColor(Color(0.5f, 0.5f, 0.5f));
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);
    light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
    light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));
    //light->SetShadowIntensity(0.5f);

    Node* zoneNode = scene_->CreateChild("Zone");
    Zone* zone = zoneNode->CreateComponent<Zone>();
    zone->SetBoundingBox(BoundingBox(-1000000.0f, 1000000.0f));
    zone->SetAmbientColor(Color(0.5f, 0.5f, 0.5f));
    zone->SetFogColor(Color(0.4f, 0.5f, 0.8f));
    zone->SetFogStart(1000.0f);
    zone->SetFogEnd(1000.0f);

    const unsigned NUM_OBJECTS = 0;
    for (unsigned i = 0; i < NUM_OBJECTS; ++i)
    {
        Node* mushroomNode = scene_->CreateChild("Mushroom");
        mushroomNode->SetPosition(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));
        mushroomNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
        mushroomNode->SetScale(32);
        StaticModel* mushroomObject = mushroomNode->CreateComponent<StaticModel>();
        mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
        mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
        mushroomObject->SetCastShadows(true);
    }


    cameraNode = scene_->CreateChild("Camera");
    Camera* camera = cameraNode->CreateComponent<Camera>();
    cameraNode->SetPosition(Vector3(0.0f, 0.0f, -100.0f));
    cameraNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));
    camera->SetNearClip(0.01f);
    camera->SetFarClip(30000);
    camera->SetUseClipping(false);
    camera->SetOrthographic(true);
    Graphics* graphics = GetSubsystem<Graphics>();
    camera->SetOrthoSize((float)graphics->GetHeight());
    //camera->SetZoom(2.0f);

    playerNode_ = scene_->CreateChild("Player");
    playerNode_->SetPosition(Vector3(200, -200, -20));
    AnimatedModel* playerObject = playerNode_->CreateComponent<AnimatedModel>();
    playerObject->SetModel(cache->GetResource<Model>("Models/Jack.mdl"));
    playerObject->SetMaterial(cache->GetResource<Material>("Materials/Jack.xml"));
    //playerNode_->SetScale(0.5f);
    playerNode_->SetScale(32.0f);
    playerNode_->SetRotation(Quaternion(0.0f, 90.0f, 0.0f));
    AnimationController* ac = playerNode_->CreateComponent<AnimationController>();
    //ac->Play("Models/Jack_Walk.ani", 0, true);
    Player* player = playerNode_->CreateComponent<Player>();
    player->Init(world_);

}



void Game::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Game, HandleUpdate));
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Game, HandlePostUpdate));
    SubscribeToEvent(E_ENDVIEWRENDER, URHO3D_HANDLER(Game, HandleEndViewRender));
}


void Game::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float timeStep = eventData[P_TIMESTEP].GetFloat();

    cameraNode->SetPosition(playerNode_->GetPosition() + Vector3(0.0f, 0.0f, -100.0f));
}


void Game::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float timeStep = eventData[P_TIMESTEP].GetFloat();

    Input* input = GetSubsystem<Input>();
    if (input->GetKeyPress(KEY_F2))
        GetSubsystem<DebugHud>()->ToggleAll();
}


void Game::HandleEndViewRender(StringHash eventType, VariantMap& eventData)
{
    //if (gameState != GameState::GAME)
    //    return;
    //Vector2 mPos = world_->GetMousePosition();
    spriteBatch_->Begin();
    world_->Draw(spriteBatch_, GetSubsystem<Graphics>(), cameraNode->GetPosition());
    //spriteBatch_->Draw(CellSand, Rect(mPos.x_, mPos.y_, mPos.x_ + 32, mPos.y_ - 32), Rect(0, 0, 32, 32), Color::WHITE);
    spriteBatch_->End();
}
