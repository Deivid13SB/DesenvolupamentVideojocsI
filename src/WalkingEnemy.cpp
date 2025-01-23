#include "WalkingEnemy.h"
#include "Engine.h"
#include "Log.h"
#include "Scene.h"
#include "Textures.h"

WalkingEnemy::WalkingEnemy() : Entity(EntityType::WALKING_ENEMY)
{
    name = "WalkingEnemy";
    position = Vector2D(300, 400); // Ajusta la posición inicial
}

WalkingEnemy::~WalkingEnemy() {}

bool WalkingEnemy::Awake() { return true; }

bool WalkingEnemy::Start()
{
    texture = Engine::GetInstance().textures->Load("Assets/Textures/walkingenemy.png");
    pbody = Engine::GetInstance().physics->CreateRectangle((int)position.getX(), (int)position.getY(), 32, 32, bodyType::DYNAMIC);
    pbody->listener = this;
    pbody->ctype = ColliderType::SPIKE; // Usar un tipo que represente al enemigo
    
    return true;
}

bool WalkingEnemy::Update(float dt)
{
    auto* scene = Engine::GetInstance().scene.get();
    if (!scene->pathfinding)
    {
        LOG("Pathfinding system not ready yet.");
        return false;
    }

    Vector2D playerPos = scene->GetPlayer()->position;
    std::vector<Vector2D> path = scene->pathfinding->FindPath(position, playerPos);

    if (!path.empty())
    {
        Vector2D nextStep = path[0];
        Vector2D direction = nextStep - position;
        direction.normalized();

        pbody->body->SetLinearVelocity(b2Vec2(direction.getX() * speed, pbody->body->GetLinearVelocity().y));
    }

    if (pbody == nullptr)
    {
        LOG("Error: pbody no está inicializado en WalkingEnemy::Update.");
        return true;
    }

    // Actualiza la posición física del enemigo
    int x, y;
    pbody->GetPosition(x, y);
    position.setX(x);
    position.setY(y);

    Engine::GetInstance().render->DrawTexture(texture, position.getX(), position.getY());

    return true;
}

void WalkingEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
    if (physB->ctype == ColliderType::PLAYER)
    {
        LOG("WalkingEnemy collided with PLAYER");
    }
}