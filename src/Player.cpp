#include "Player.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Player::Player() :
	Entity(EntityType::PLAYER),
	spawnPoint(96, 500)  // Inicializar spawnPoint en la lista de inicialización
{
	name = "Player";
	godMode = false;
	isDead = false;
	position = Vector2D(96, 500);
	lives = 3; // Inicializamos con 3 vidas
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: TODO 2: Initialize Player parameters
	position = Vector2D(96, 500);
	return true;
}

bool Player::Start() {

	heartTexture = Engine::GetInstance().textures.get()->Load("Assets/Textures/heart.png");

	//L03: TODO 2: Initialize Player parameters
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/player.png");

	// L08 TODO 5: Add physics to the player - initialize physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX(), (int)position.getY(), texW / 2, bodyType::DYNAMIC);


	// L08 TODO 6: Assign player class (using "this") to the listener of the pbody. This makes the Physics module to call the OnCollision method
	pbody->listener = this;

	// L08 TODO 7: Assign collider type
	pbody->ctype = ColliderType::PLAYER;

	//initialize audio effect
	pickCoinFxId = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/retro-video-game-coin-pickup-38299.ogg");
	jumpFxId = Engine::GetInstance().audio->LoadFx("Assets/Audio/Fx/jump.ogg");

	isDead = false;

	return true;
}

bool Player::Update(float dt)
{
	// L08 TODO 5: Add physics to the player - updated player position using physics
	b2Vec2 velocity = b2Vec2(0, pbody->body->GetLinearVelocity().y);

	// Move left
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		velocity.x = -0.2 * 16;
	}

	// Move right
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		velocity.x = 0.2 * 16;
	}
	
	//Jump
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && isJumping == false) {
		// Apply an initial upward force
		pbody->body->ApplyLinearImpulseToCenter(b2Vec2(0, -jumpForce), true);
		isJumping = true;
		Engine::GetInstance().audio->PlayFx(jumpFxId);
	}

	// If the player is jumpling, we don't want to apply gravity, we use the current velocity prduced by the jump
	if(isJumping == true)
	{
		velocity.y = pbody->body->GetLinearVelocity().y;
	}

	// Apply the velocity to the player
	pbody->body->SetLinearVelocity(velocity);

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());

	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		godMode = !godMode;
	}

	if (godMode)
	{
		// Implementar lógica de invencibilidad
		// Por ejemplo, ignorar colisiones o daño
	}

	if (isDead)
	{
		pbody->body->SetTransform({ PIXEL_TO_METERS(96),PIXEL_TO_METERS(500) }, 0);
		isDead = false;
	}
	else
	{
		pbody->body->SetLinearVelocity(velocity);
	}

	if (godMode)
	{
		if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		{
			velocity.y = -4;
		}
		else if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			velocity.y = 4;
		}
		else
		{
			velocity.y = 0;
		}
		pbody->body->SetLinearVelocity(velocity);
	}
	if (pendingRespawn) {
		Respawn();
		pendingRespawn = false;
	}

	return true;
}

bool Player::CleanUp()
{
	LOG("Cleanup player");
	Engine::GetInstance().textures.get()->UnLoad(texture);
	return true;
}

// L08 TODO 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		//reset the jump flag when touching the ground
		isJumping = false;
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;

	if (physB->ctype == ColliderType::CHECKPOINT) {
        SetCheckpoint(Vector2D(position.getX(), position.getY()));
    }
	case ColliderType::SPIKE:
		LOG("Collision SPIKE");
		if (!godMode)
		{
			isDead = true;
			LoseLife();
			pendingRespawn = true;// Call respawn immediately when player dies
		}

	default:
		break;
	}

	
}

void Player::OnCollisionEnd(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("End Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("End Collision ITEM");
		Engine::GetInstance().audio.get()->PlayFx(pickCoinFxId);
		break;
	case ColliderType::SPIKE:
		LOG("Is dead");
		break;
	default:
		break;
	}
}

bool Player::SaveState() {
	pugi::xml_document saveDoc;
	pugi::xml_node rootNode = saveDoc.append_child("game_state");

	rootNode.append_child("player_x").append_child(pugi::node_pcdata).set_value(std::to_string(position.getX()).c_str());
	rootNode.append_child("player_y").append_child(pugi::node_pcdata).set_value(std::to_string(position.getY()).c_str());

	return saveDoc.save_file("save_game.xml");
}

bool Player::LoadState() {
	pugi::xml_document loadDoc;
	if (loadDoc.load_file("save_game.xml")) {
		pugi::xml_node rootNode = loadDoc.child("game_state");
		float x = std::stof(rootNode.child("player_x").child_value());
		float y = std::stof(rootNode.child("player_y").child_value());

		position = Vector2D(x, y);
		if (pbody != nullptr) {
			pbody->body->SetTransform(b2Vec2(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y)), 0.0f);
		}
		return true;
	}
	return false;
}

void Player::SetCheckpoint(Vector2D pos) {
	lastCheckpoint = pos;
	hasCheckpoint = true;
	// Opcional: Añadir efecto de sonido
	/*Engine::GetInstance().audio->PlayFx(checkpointFxId);*/
}

// Modifica la función Respawn existente
void Player::Respawn() {

	if (!gameOver) {
		if (hasCheckpoint) {
			position = lastCheckpoint;
		}
		else {
			position = spawnPoint;
		}

		if (pbody != nullptr) {
			pbody->body->SetTransform(
				b2Vec2(PIXEL_TO_METERS(position.getX()),
					PIXEL_TO_METERS(position.getY())), 0.0f);
		}
		isDead = false;
	}
}

void Player::LoseLife() {
	if (!godMode) {
		lives--;
		if (lives <= 0) {
			gameOver = true;
		}
	}
}