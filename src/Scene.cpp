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
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	//L04: TODO 3b: Instantiate the player using the entity manager
	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	item->position = Vector2D(200, 672);
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

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
	//float camSpeed = 1;

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.y -= ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.y += ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.x -= ceil(camSpeed * dt);

	//if(Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	//	Engine::GetInstance().render.get()->camera.x += ceil(camSpeed * dt);

	return true;
}

void Scene::CameraFollow()
{
	// Obtener la posici�n del jugador
	Vector2D playerPos;
	playerPos.setX(player->position.getX());
	playerPos.setY(player->position.getY());

	// Calcular la posici�n objetivo de la c�mara
	float targetCameraX = -playerPos.getX() + cameraCenterX;
	float targetCameraY = -playerPos.getY() + cameraCenterY;

	// Suavizar el movimiento de la c�mara
	float smoothness = 0.1f;

	Engine::GetInstance().render.get()->camera.x += (targetCameraX - Engine::GetInstance().render.get()->camera.x) * smoothness;
	Engine::GetInstance().render.get()->camera.y += (targetCameraY - Engine::GetInstance().render.get()->camera.y) * smoothness;

	// Calcular l�mites del mapa usando los getters
	int mapWidth = Engine::GetInstance().map.get()->GetMapWidth() *
		Engine::GetInstance().map.get()->GetTileWidth();
	int mapHeight = Engine::GetInstance().map.get()->GetMapHeight() *
		Engine::GetInstance().map.get()->GetTileHeight();

	// Obtener dimensiones de la ventana
	int windowWidth, windowHeight;
	Engine::GetInstance().window.get()->GetWindowSize(windowWidth, windowHeight);

	// Aplicar l�mites
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

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	SDL_DestroyTexture(img);

	return true;
}
