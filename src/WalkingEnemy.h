#pragma once

#include "Physics.h"
#include "Scene.h"
#include "Entity.h"
#include "Vector2D.h"

class WalkingEnemy : public Entity
{
public:
    WalkingEnemy();
    ~WalkingEnemy();

    bool Awake();
    bool Start();
    bool Update(float dt);
    void OnCollision(PhysBody* physA, PhysBody* physB);

private:
    float speed = 2.0f;
    Vector2D direction;
    PhysBody* pbody;
    SDL_Texture* texture = nullptr;
};

