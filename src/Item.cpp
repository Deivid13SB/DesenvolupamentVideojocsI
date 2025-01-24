#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name = "item";
}

Item::~Item() {}

bool Item::Awake() {
	position = Vector2D(196, 500);
	return true;
}

bool Item::Start() {

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/crystal.png");
	
	// L08 TODO 4: Add a physics to an item - initialize the physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::ITEM;

	return true;
}

void Item::Collect() {
	isCollected = true;
	// Desactivar el cuerpo físico cuando se recoge
	if (pbody != nullptr) {
		pbody->body->SetEnabled(false);
	}

	// Aplicar efecto al jugador
	Player* player = dynamic_cast<Player*>(Engine::GetInstance().entityManager->GetEntity(EntityType::PLAYER));
	if (player != nullptr) {
		LOG("Applying buff to the player!");
		player->ApplyBuff(20.0f, 1.5f, 1.5f); // Buff de 20 segundos, +50% velocidad y salto
	}
	else {
		LOG("Failed to find player to apply buff!");
	}
}

bool Item::Update(float dt)
{
	if (!isCollected) {
		// Solo dibuja el ítem si no ha sido recogido
		b2Transform pbodyPos = pbody->body->GetTransform();
		position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
		position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	}
	return true;
}

bool Item::CleanUp()
{
	if (pbody != nullptr) {
		Engine::GetInstance().physics->DestroyBody(pbody);
		pbody = nullptr;
	}
	if (texture != nullptr) {
		Engine::GetInstance().textures->UnLoad(texture);
		texture = nullptr;
	}
	return true;
}