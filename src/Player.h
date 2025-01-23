#pragma once

#include "Entity.h"
#include "SDL2/SDL.h"
#include "Box2D/Box2D.h"

struct SDL_Texture;


class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	const Vector2D spawnPoint{ 96, 500 }; // Hacer el spawnPoint constante

	// L08 TODO 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

	void OnCollisionEnd(PhysBody* physA, PhysBody* physB);

	bool SaveState();
	bool LoadState();
	void SetCheckpoint(Vector2D position);
	int lives = 3;  // Número inicial de vidas
	SDL_Texture* heartTexture = nullptr;
	bool gameOver = false;

public:

	//Declare player parameters
	float speed = 7.0f;
	SDL_Texture* texture = NULL;
	int texW, texH;

	bool godMode;

	bool isDead;

	void Respawn();
	void LoseLife();
	int GetLives() const { return lives; }
	bool IsGameOver() const { return gameOver; }


	//Audio fx
	int pickCoinFxId;
	int jumpFxId;

	// L08 TODO 5: Add physics to the player - declare a Physics body
	PhysBody* pbody;
	float jumpForce = 1.9f; // The force to apply when jumping
	bool isJumping = false; // Flag to check if the player is currently jumping
	Vector2D lastCheckpoint;
	bool hasCheckpoint = false;
	bool pendingRespawn = false;
};