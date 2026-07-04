#include "Explosion.hh"
#include "GameManager.hh"

Explosion::Explosion(const Vector2 p, const float r, const float s) :
	GameObject(p, r, s)
{
	_elapsedLifetime = 0;

	for (int i = 0; i < EXPLOSION_PROPS_NUMBER; ++i) 
	{
		_propsEnabled[i] = GetRandomValue(0,1) == 0;
		_propsPositions[i] = _position;
		_propsVelocities[i] = { float(GetRandomValue(-600, 600)), float(GetRandomValue(-200, -1000)) };
		_propsRotations[i] = float(GetRandomValue(0, 360));
		_propsRotationSpeeds[i] = float(GetRandomValue(-180, 180));
	} 

	_explosionSound = GameManager::instance->audioManager->GetBombExplosionSound();
	SetSoundPan(_explosionSound, GameManager::instance->GetPan(_position));
	SetSoundPitch(_explosionSound, float(GetRandomValue(90, 110)) / 100.0f);

	PlaySound(_explosionSound);
}

Explosion::~Explosion()
{
	UnloadSound(_explosionSound);
}

void Explosion::Update(float deltaTime)
{
	_elapsedLifetime += deltaTime;

	for (int i = 0; i < EXPLOSION_PROPS_NUMBER; ++i) 
	{
		if (_propsEnabled[i]) 
		{
			_propsPositions[i].x += _propsVelocities[i].x * deltaTime;
			_propsPositions[i].y += _propsVelocities[i].y * deltaTime;

			_propsVelocities[i].y += 1600 * deltaTime;
			_propsRotations[i] += _propsRotationSpeeds[i] * deltaTime;
		}
	}

	// Twice as long for the sound to play out, since the sound is longer than the visual effect
	if (_elapsedLifetime >= EXPLOSION_DURATION * 2.0f) 
	{
		GameManager::instance->DestroyExplosion(this);
	}
}

void Explosion::Render(const float deltaTime)
{
	Rectangle dest = { _position.x, _position.y, _scale, _scale };
	Vector2 origin = { _scale / 2, _scale / 2 };
	
	float animProgress = (_elapsedLifetime / EXPLOSION_DURATION);
	animProgress = fminf(animProgress, 1.0f);

	const float propSize = _scale / 4;
	Vector2 propOrigin = { propSize / 2, propSize / 2 };
	float propAlpha = 1.0f - animProgress * 2.0f;

	for (int i = 0; i < EXPLOSION_PROPS_NUMBER; ++i) 
	{
		if (_propsEnabled[i]) 
		{
			DrawTexturePro
			(GameManager::instance->sprExplosionProps,
				{ float(i * EXPLOSION_PROPS_SPRITE_SIZE), 0, EXPLOSION_PROPS_SPRITE_SIZE, EXPLOSION_PROPS_SPRITE_SIZE },
				{ _propsPositions[i].x, _propsPositions[i].y, propSize, propSize },
				propOrigin, _propsRotations[i], Fade(WHITE, propAlpha)
			);
		}
	}

	DrawTexturePro
	(GameManager::instance->sprExplosion,
		{ float(int(animProgress * EXPLOSION_ANIMATION_SPRITES) * EXPLOSION_SPRITE_SIZE), 0, EXPLOSION_SPRITE_SIZE, EXPLOSION_SPRITE_SIZE }, // SOURCE
		dest, origin, 0, WHITE
	);
	
}
