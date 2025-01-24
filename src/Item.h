#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

public:

	bool isPicked = false;
	void Collect(); // Nueva función
	bool isCollected = false; // Nueva variable para rastrear si el ítem fue recogido

private:

	SDL_Texture* texture;
	const char* texturePath;
	int texW, texH;

	//L08 TODO 4: Add a physics to an item
	PhysBody* pbody;
};
