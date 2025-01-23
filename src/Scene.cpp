#include "Engine.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Log.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Player.h"
#include "Map.h"
#include "Item.h"

Scene::Scene() : Module()
{
	name = "scene";
	img = nullptr;
	uiMenuTexture = nullptr;
	showUIMenu = false;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	// Make sure the renderer is initialized before loading textures
	if (Engine::GetInstance().render.get()->renderer == nullptr) {
		LOG("Renderer not initialized!");
		return false;
	}

	// Load start screen texture
	startTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/start.png");
	if (startTexture == nullptr) {
		LOG("Failed to load start screen texture!");
		return false;
	}

	// Keep other entities inactive until start screen is dismissed
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	player->active = false;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	if (!showStartScreen)
	{
		// Load game resources
		Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");
		uiMenuTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/UIDebug.png");
		Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/background.ogg");
	}
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{

	CameraFollow();
	////L03 TODO 3: Make the camera movement independent of framerate
	/*float camSpeed = 1;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y -= ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.y += ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x -= ceil(camSpeed * dt);

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		Engine::GetInstance().render.get()->camera.x += ceil(camSpeed * dt);*/

	if (showStartScreen)
		{
		// Only try to render if we have a valid texture
		if (startTexture != nullptr)
			{
		 // Get window dimensions
				int w, h;
				Engine::GetInstance().window.get()->GetWindowSize(w, h);

				SDL_Rect fullScreen = { 0, 0, w, h };

				// Draw the start screen
				if (!Engine::GetInstance().render.get()->DrawTexture(startTexture, 0, 0, &fullScreen))
				{
					LOG("Failed to render start screen!");
				}

				// Check for input to start game
				if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
				{
					showStartScreen = false;
					player->active = true;
					Start(); // Initialize the rest of the game
				}
			}
	}
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		ToggleUIMenu();
	}

	if (showUIMenu)
	{
		DrawUIMenu();
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		player->SaveState();
	}

	if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		player->LoadState();
		
	}

	return true;
}

void Scene::CameraFollow()
{
	// Obtener la posición del jugador
	Vector2D playerPos;
	playerPos.setX(player->position.getX());
	playerPos.setY(player->position.getY());

	// Calcular la posición objetivo de la cámara
	float targetCameraX = -playerPos.getX() + cameraCenterX;
	float targetCameraY = -playerPos.getY() + cameraCenterY;

	// Suavizar el movimiento de la cámara
	float smoothness = 0.1f;

	Engine::GetInstance().render.get()->camera.x += (targetCameraX - Engine::GetInstance().render.get()->camera.x) * smoothness;
	Engine::GetInstance().render.get()->camera.y += (targetCameraY - Engine::GetInstance().render.get()->camera.y) * smoothness;

	// Calcular límites del mapa usando los getters
	int mapWidth = Engine::GetInstance().map.get()->GetMapWidth() *
		Engine::GetInstance().map.get()->GetTileWidth();
	int mapHeight = Engine::GetInstance().map.get()->GetMapHeight() *
		Engine::GetInstance().map.get()->GetTileHeight();

	// Obtener dimensiones de la ventana
	int windowWidth, windowHeight;
	Engine::GetInstance().window.get()->GetWindowSize(windowWidth, windowHeight);

	// Aplicar límites
	if (Engine::GetInstance().render.get()->camera.x > 0)
		Engine::GetInstance().render.get()->camera.x = 0;
	else if (Engine::GetInstance().render.get()->camera.x < -mapWidth + windowWidth)
		Engine::GetInstance().render.get()->camera.x = -mapWidth + windowWidth;

	if (Engine::GetInstance().render.get()->camera.y > 0)
		Engine::GetInstance().render.get()->camera.y = 0;
	else if (Engine::GetInstance().render.get()->camera.y < -mapHeight + windowHeight)
		Engine::GetInstance().render.get()->camera.y = -mapHeight + windowHeight;
}
// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

void Scene::ToggleUIMenu()
{
	showUIMenu = !showUIMenu;
}

void Scene::DrawUIMenu()
{
	if (uiMenuTexture != nullptr)
	{
		int windowWidth, windowHeight;
		Engine::GetInstance().window.get()->GetWindowSize(windowWidth, windowHeight);

		// Obtener las dimensiones de la textura
		int textureWidth, textureHeight;
		Engine::GetInstance().textures.get()->GetSize(uiMenuTexture, textureWidth, textureHeight);

		// Calcular la posición en la esquina superior derecha
		int x = windowWidth - textureWidth;
		int y = 0;

		// Dibujar el menú UI
		Engine::GetInstance().render.get()->DrawTexture(uiMenuTexture, x, y);
	}
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Cleaning Up Scene");

	if (startTexture != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(startTexture);
		startTexture = nullptr;
	}
	if (startTexture != nullptr)
	{
		SDL_DestroyTexture(startTexture);
		startTexture = nullptr;
	}
	LOG("Freeing scene");

	SDL_DestroyTexture(img);

	if (uiMenuTexture != nullptr)
	{
		Engine::GetInstance().textures.get()->UnLoad(uiMenuTexture);
		uiMenuTexture = nullptr;
	}

	return true;
}
