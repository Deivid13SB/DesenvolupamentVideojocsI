#include "FlyingEnemy.h"
#include "Engine.h"
#include "Log.h"
#include "Scene.h"
#include "Textures.h"

FlyingEnemy::FlyingEnemy() : Entity(EntityType::FLYING_ENEMY)
{
    name = "FlyingEnemy";
    position = Vector2D(500, 300); // Ajusta la posición inicial
}

FlyingEnemy::~FlyingEnemy() {}

bool FlyingEnemy::Awake()
{
    return true;
}

bool FlyingEnemy::Start()
{
    // Configurar el cuerpo físico
    pbody = Engine::GetInstance().physics->CreateRectangle((int)position.getX(), (int)position.getY(), 32, 32, bodyType::KINEMATIC);
    pbody->listener = this;
    pbody->ctype = ColliderType::SPIKE; // Tipo de colisionador

    // Cargar la textura
    if (Engine::GetInstance().textures == nullptr)
    {
        LOG("Error: El sistema de texturas no está inicializado.");
        return false;
    }

    texture = Engine::GetInstance().textures->Load("Assets/Textures/flyingenemy.png");
    if (texture == nullptr)
    {
        LOG("Error: No se pudo cargar la textura del Flying Enemy.");
        return false;
    }

    return true;
}

bool FlyingEnemy::Update(float dt)
{
    // Obtener la posición del jugador
    auto* scene = Engine::GetInstance().scene.get();
    Vector2D playerPos = scene->GetPlayer()->position;

    // Calcular el camino hacia el jugador usando el sistema de pathfinding
    std::vector<Vector2D> path = scene->pathfinding->FindPath(position, playerPos);

    if (!path.empty())
    {
        // Sigue el primer paso del camino
        Vector2D nextStep = path[0];
        direction = nextStep - position;
        direction.normalized();

        // Mover al enemigo volador hacia la dirección
        pbody->body->SetLinearVelocity(b2Vec2(direction.getX() * speed, direction.getY() * speed));
    }
    else
    {
        LOG("FlyingEnemy: No se encontró un camino hacia el jugador.");
    }

    if (pbody == nullptr)
    {
        LOG("Error: pbody no está inicializado en WalkingEnemy::Update.");
        return true;
    }
    // Actualizar la posición física
    int x, y;
    pbody->GetPosition(x, y);
    position.setX(x);
    position.setY(y);

    // Dibujar la textura
    Engine::GetInstance().render->DrawTexture(texture, position.getX(), position.getY());

    return true;
}

void FlyingEnemy::OnCollision(PhysBody* physA, PhysBody* physB)
{
    if (physB->ctype == ColliderType::PLAYER)
    {
        LOG("FlyingEnemy colisionó con PLAYER.");
    }
}