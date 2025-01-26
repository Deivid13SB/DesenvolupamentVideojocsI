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
	uiMenuTexture = nullptr;
	showUIMenu = false;
	heartTexture = nullptr;
	introTexture = nullptr;
	titleTexture = nullptr;
	pauseTexture = nullptr;
	dieTexture = nullptr;
	endTexture = nullptr;
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
	
	//L08 Create a new item using the entity manager and set the position to (200, 672) to test
	//Item* item = (Item*) Engine::GetInstance().entityManager->CreateEntity(EntityType::ITEM);
	//item->position = Vector2D(200, 672);
	return ret;
}

// Called before the first frame
bool Scene::Start()
{
	introTexture = Engine::GetInstance().textures->Load("Assets/Textures/IntroScreen.png");
	titleTexture = Engine::GetInstance().textures->Load("Assets/Textures/TitleScreen.png");
	pauseTexture = Engine::GetInstance().textures->Load("Assets/Textures/PauseScreen.png");
	dieTexture = Engine::GetInstance().textures->Load("Assets/Textures/DieScreen.png");
	endTexture = Engine::GetInstance().textures->Load("Assets/Textures/EndScreen.png");

	//L06 TODO 3: Call the function to load the map. 
	Engine::GetInstance().map->Load("Assets/Maps/", "MapTemplate.tmx");

	uiMenuTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/UIDebug.png");
	
	Engine::GetInstance().audio->PlayMusic("Assets/Audio/Music/background.ogg");

	heartTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/heart.png");
	if (heartTexture == nullptr) {
		LOG("Failed to load heart texture!");
	}

	player = (Player*)Engine::GetInstance().entityManager->CreateEntity(EntityType::PLAYER);


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
	switch (currentState)
	{
	case SceneState::INTRO:
		DrawIntroScreen();
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			ChangeState(SceneState::TITLE);
		}
		break;

	case SceneState::TITLE:
		DrawTitleScreen();
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			ChangeState(SceneState::GAME);
		}
		break;

	case SceneState::GAME:
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
			ChangeState(SceneState::PAUSE);
		}
		if (player->IsGameOver())
		{
			ChangeState(SceneState::DIE);
		}
		break;

	case SceneState::PAUSE:
		DrawPauseScreen();
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			ChangeState(SceneState::GAME);
		}
		break;

	case SceneState::DIE:
		DrawDieScreen();
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			ChangeState(SceneState::TITLE);
		}
		break;

	case SceneState::END:
		DrawEndScreen();
		if (Engine::GetInstance().input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			ChangeState(SceneState::TITLE);
		}
		break;
	}

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

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	{
		ToggleUIMenu();
	}

	if (player->IsGameOver()) {
		// Aquí puedes añadir lo que quieras que suceda cuando se acabe el juego
		// Por ejemplo:
		LOG("GAME OVER - No more lives!");
		return false;
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

	DrawHearts();

	return true;
}

void Scene::DrawHearts() {

	LOG("Drawing hearts. Player lives: %d", player->lives);
	LOG("Heart texture pointer: %p", heartTexture);
	if (heartTexture == nullptr || player == nullptr) return;

	// Obtener dimensiones de la ventana
	int windowWidth, windowHeight;
	Engine::GetInstance().window.get()->GetWindowSize(windowWidth, windowHeight);

	// Tamaño de cada corazón
	const int heartSize = 32; // Ajusta este valor según el tamaño de tu textura
	const int padding = 10;   // Espacio entre corazones

	// Posición base para los corazones (esquina superior derecha)
	int baseX = windowWidth - ((heartSize + padding) * 3);
	int baseY = 100;

	// Dibujar los corazones basados en las vidas del jugador
	for (int i = 0; i < player->lives; i++) {
		Engine::GetInstance().render.get()->DrawTexture(
			heartTexture,
			baseX + (i * (heartSize + padding)),
			baseY,
			nullptr,  // section
			0.0f,     // speed (0 para que no se muevan con la cámara)
			0.0       // angle
		);
	}
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

void Scene::ChangeState(SceneState newState)
{
	currentState = newState;
}

void Scene::DrawIntroScreen()
{
	Engine::GetInstance().render->DrawTexture(introTexture, 0, 0);
}

void Scene::DrawTitleScreen()
{
	Engine::GetInstance().render->DrawTexture(titleTexture, 0, 0);
}

void Scene::DrawPauseScreen()
{
	Engine::GetInstance().render->DrawTexture(pauseTexture, 0, 0);
}

void Scene::DrawDieScreen()
{
	Engine::GetInstance().render->DrawTexture(dieTexture, 0, 0);
}

void Scene::DrawEndScreen()
{
	Engine::GetInstance().render->DrawTexture(endTexture, 0, 0);
}


// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	SDL_DestroyTexture(img);

	if (uiMenuTexture != nullptr) {
		LOG("Unloading uiMenuTexture...");
		Engine::GetInstance().textures->UnLoad(uiMenuTexture);
		uiMenuTexture = nullptr;
	}

	if (heartTexture != nullptr) {
		LOG("Unloading heartTexture...");
		Engine::GetInstance().textures->UnLoad(heartTexture);
		heartTexture = nullptr;
	}

	if (introTexture != nullptr) {
		LOG("Unloading introTexture...");
		Engine::GetInstance().textures->UnLoad(introTexture);
		introTexture = nullptr;
	}

	if (titleTexture != nullptr) {
		LOG("Unloading titleTexture...");
		Engine::GetInstance().textures->UnLoad(titleTexture);
		titleTexture = nullptr;
	}

	if (pauseTexture != nullptr) {
		LOG("Unloading pauseTexture...");
		Engine::GetInstance().textures->UnLoad(pauseTexture);
		pauseTexture = nullptr;
	}

	if (dieTexture != nullptr) {
		LOG("Unloading dieTexture...");
		Engine::GetInstance().textures->UnLoad(dieTexture);
		dieTexture = nullptr;
	}

	if (endTexture != nullptr) {
		LOG("Unloading endTexture...");
		Engine::GetInstance().textures->UnLoad(endTexture);
		endTexture = nullptr;
	}

	return true;
}
