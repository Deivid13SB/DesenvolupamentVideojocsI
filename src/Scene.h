#pragma once

#include "Module.h"
#include "Player.h"
#include "SDL2/SDL.h"

struct SDL_Texture;

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:
private:
	SDL_Texture* startTexture = nullptr;
	bool showStartScreen = true;
	void CameraFollow();
	void ToggleUIMenu();
	void DrawUIMenu();
	void DrawHearts();

	int cameraCenterX = 400;
	int cameraCenterY = 300;

	SDL_Texture* img;

	//L03: TODO 3b: Declare a Player attribute
	Player* player;

	SDL_Texture* uiMenuTexture;
	bool showUIMenu;
	SDL_Texture* heartTexture = nullptr;
};