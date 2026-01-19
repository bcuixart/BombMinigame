#include "Explosion.hh"
#include "GameManager.hh"

Explosion::Explosion(const Vector2 p, const float r, const float s) :
	GameObject(p, r, s)
{
	_elapsedLifetime = 0;
}

void Explosion::Update(float deltaTime)
{
	_elapsedLifetime += deltaTime;

	if (_elapsedLifetime >= EXPLOSION_DURATION) GameManager::instance->DestroyExplosion(this);
}

void Explosion::Render(const float deltaTime)
{
	Rectangle dest = { _position.x, _position.y, _scale, _scale };
	Vector2 origin = { _scale / 2, _scale / 2 };
	// Body
	DrawTexturePro
	(GameManager::instance->sprExplosion,
		{ float(int((_elapsedLifetime / EXPLOSION_DURATION) *EXPLOSION_ANIMATION_SPRITES) * EXPLOSION_SPRITE_SIZE), 0, EXPLOSION_SPRITE_SIZE, EXPLOSION_SPRITE_SIZE }, // SOURCE
		dest, origin, 0, WHITE
	);
}