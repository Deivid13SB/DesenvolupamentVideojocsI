#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

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
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/goldCoin.png");
	
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
	return true;
}