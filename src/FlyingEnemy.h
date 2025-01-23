#pragma once

#include "Entity.h"
#include "Physics.h"
#include "Vector2D.h"

class FlyingEnemy : public Entity
{
public:
    FlyingEnemy();
    ~FlyingEnemy();

    bool Awake();
    bool Start();
    bool Update(float dt);
    void OnCollision(PhysBody* physA, PhysBody* physB);

private:
    float speed = 2.5f; // Velocidad de vuelo
    Vector2D direction;
    PhysBody* pbody;
    SDL_Texture* texture = nullptr; // Textura del enemigo
};
