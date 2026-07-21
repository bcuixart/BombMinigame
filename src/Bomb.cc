#include "Bomb.hh"
#include "GameManager.hh"

Bomb::Bomb(const Vector2 p, const float r, const float s, const BombType t) : 
	GameObject(p, r, s)
{
	_radius = s / 2;
	_radiusVisual = _radius * (float(BOMB_SPRITE_HITBOX_SIZE) / float(BOMB_SPRITE_SIZE));
	_grabbedScaleMultiplier = 1;

	_movementDirection = { 1, 1 };

	_type = t;
	if (_type == BOMB_BLACK) _color = { 55, 55, 55, 255 };
	else if (_type == BOMB_RED) _color = { 200, 0, 0, 255 };
	else if (_type == BOMB_BLUE) _color = { 0, 0, 200, 255 };
	else if (_type == BOMB_GREEN) _color = { 0, 200, 0, 255 };
	_state = RANDOM_MOVEMENT;

	_timeToExplode = BOMB_EXPLODE_TIME;
	if (_type == BOMB_REVIVE) _timeToExplode = BOMB_EXPLODE_TIME_REVIVE;

	_didStepSound001 = false;
	_didStepSound002 = false;

	_windUpLoopSound = GameManager::instance->audioManager->GetBombWindUpLoopSound();
	SetSoundVolume(_windUpLoopSound, BOMB_WINDUP_LOOP_SOUND_VOLUME);
	PlaySound(_windUpLoopSound);

	_fuseLoopSound = GameManager::instance->audioManager->GetBombFuseLoopSound();
	SetSoundVolume(_fuseLoopSound, 0);
	PlaySound(_fuseLoopSound);

	float cycleLength = (float)ANIMATION_FRAMES / 33.75f;
	float musicTime = GameManager::instance->audioManager->GetMusicTime() + BOMB_MUSIC_TIME_OFFSET;
	_animationTime = fmodf(musicTime, cycleLength);
	_animationFrame = fmodf(musicTime * 33.75f, (float)ANIMATION_FRAMES);

	_didGameOver = false;
	_collidedThisFrame = false;
	_wasAboutToExplodeLastFrame = false;

	_alpha = 0;
}

Bomb::~Bomb()
{
	GameManager::instance->audioManager->UnloadBombWindUpLoopSound(&_windUpLoopSound);
	GameManager::instance->audioManager->UnloadBombFuseLoopSound(&_fuseLoopSound);
}

BombType Bomb::GetType() const 
{
	return _type;
}

void Bomb::Update(float deltaTime)
{
	float prevFrame = _animationFrame;

	_animationTime += deltaTime;
	_animationFrame = fmodf(_animationTime * 33.75f, (float)ANIMATION_FRAMES);

	_alpha = Clamp(_alpha + 800.0f * deltaTime, 0.0f, 255.0f);

	if (_animationFrame < prevFrame)
	{
		_didStepSound001 = false;
		_didStepSound002 = false;
		_animationFrame = 0;
	}

	SetSoundPan(_windUpLoopSound, GameManager::instance->GetPan(_position));
	if (!IsSoundPlaying(_windUpLoopSound)) PlaySound(_windUpLoopSound);

	SetSoundPan(_fuseLoopSound, GameManager::instance->GetPan(_position));
	float fuseSoundMultiplier = Clamp(1 - (_timeToExplode / BOMB_EXPLODE_FUSE_TIME), 0, 1);
	SetSoundVolume(_fuseLoopSound, min(1.0f, BOMB_FUSE_LOOP_SOUND_VOLUME * fuseSoundMultiplier));
	if (!IsSoundPlaying(_fuseLoopSound)) PlaySound(_fuseLoopSound);

	_collidedThisFrame = false;

	switch (_state) 
	{
		case RANDOM_MOVEMENT: 
		{
			Update_RandomMovement(deltaTime);
			break;
		}
		case GRABBED: 
		{
			Update_Grabbed(deltaTime);
			break;
		}		
		case PLACED: 
		{
			Update_Placed(deltaTime);
			break;
		}
	}
}

void Bomb::Update_RandomMovement(const float deltaTime) 
{
	_timeToExplode -= deltaTime;
	if (_timeToExplode <= 0) GameManager::instance->ExplodeBomb(this);

	_grabbedScaleMultiplier = 1;

	_position.x += _movementDirection.x * BOMB_MOVEMENT_SPEED * deltaTime;
	_position.y += _movementDirection.y * BOMB_MOVEMENT_SPEED * deltaTime;

	// Flip at bounds
	Vector2 boundsHor = GameManager::instance->GetHorizontalBounds();
	if (_position.x < boundsHor.x) _movementDirection.x = 1;
	else if (_position.x > boundsHor.y) _movementDirection.x = -1;	
	if (_position.y < MAP_COORD_VER_MIN) _movementDirection.y = 1;
	else if (_position.y > MAP_COORD_VER_MAX) _movementDirection.y = -1;

	// Constraint to max horizontal bounds
	if (_position.x < MAP_COORD_HOR_MIN) _position.x = MAP_COORD_HOR_MIN;
	if (_position.x > MAP_COORD_HOR_MAX) _position.x = MAP_COORD_HOR_MAX;

	_animationIndex = BOMB_ANIM_INDEX_WALK_BOT_LEFT;
	if (_movementDirection.x == 1 && _movementDirection.y == 1) _animationIndex = BOMB_ANIM_INDEX_WALK_BOT_RIGHT;
	else if (_movementDirection.x == 1 && _movementDirection.y == -1) _animationIndex = BOMB_ANIM_INDEX_WALK_TOP_RIGHT;
	else if (_movementDirection.x == -1 && _movementDirection.y == -1) _animationIndex = BOMB_ANIM_INDEX_WALK_TOP_LEFT;

	// Sounds
	if (int(_animationFrame) >= BOMB_STEP_SOUND_ANIM_FRAME_001 && !_didStepSound001) { GameManager::instance->audioManager->PlayBombStepSound(GameManager::instance->GetPan(_position)); _didStepSound001 = true; }
	else if (int(_animationFrame) >= BOMB_STEP_SOUND_ANIM_FRAME_002 && !_didStepSound002) { GameManager::instance->audioManager->PlayBombStepSound(GameManager::instance->GetPan(_position)); _didStepSound002 = true; }
}

void Bomb::Update_Grabbed(const float deltaTime) 
{
	_timeToExplode -= deltaTime;
	if (_timeToExplode <= 0) GameManager::instance->ExplodeBomb(this);

	_grabbedScaleMultiplier = BOMB_GRABBED_SCALE_INDEX;

	_animationIndex = BOMB_ANIM_INDEX_GRABBED;

	_position = GameManager::instance->GetWorldMousePos();
}

void Bomb::Update_Placed(const float deltaTime) 
{
	_timeToExplode = max(_timeToExplode, 5.0f);
	_animationIndex = (_placedDirection == BOMB_PLACED_TOP) ? BOMB_ANIM_INDEX_PLACED_TOP : BOMB_ANIM_INDEX_PLACED_BOT;

	_grabbedScaleMultiplier = 1;

	if (_position.x < BOMBHOUSE_COORD_HOR_MIN) _position.x = BOMBHOUSE_COORD_HOR_MIN;
	else if (_position.x > BOMBHOUSE_COORD_HOR_MAX) _position.x = BOMBHOUSE_COORD_HOR_MAX;

	float volumeMultiplier;
	if (_placedDirection == BOMB_PLACED_TOP)
	{
		volumeMultiplier = abs((-MAP_COORD_RADIUS) - _position.y);
		volumeMultiplier /= abs(BOMBHOUSE_COORD_TOP_VER_POS - -MAP_COORD_RADIUS);

		_position.y -= BOMB_MOVEMENT_SPEED * deltaTime;
		_position.y = max(_position.y, (float) -MAP_COORD_RADIUS);
	}
	else
	{
		volumeMultiplier = abs(_position.y - (MAP_COORD_RADIUS));
		volumeMultiplier /= abs(BOMBHOUSE_COORD_BOT_VER_POS - (MAP_COORD_RADIUS));

		_position.y += BOMB_MOVEMENT_SPEED * deltaTime;
		_position.y = min(_position.y, (float) MAP_COORD_RADIUS);
	}

	volumeMultiplier = Clamp(volumeMultiplier, 0, 1);
	SetSoundVolume(_windUpLoopSound, min(1.0f, BOMB_WINDUP_LOOP_SOUND_VOLUME * volumeMultiplier));

	if (_position.y <= -MAP_COORD_RADIUS || _position.y >= MAP_COORD_RADIUS) GameManager::instance->BombEntered(this, this->_placedDirection);
}

void Bomb::Render(const float deltaTime) 
{
	bool aboutToExplode = GetIsAboutToExplode();
	if (aboutToExplode && !_wasAboutToExplodeLastFrame) GameManager::instance->audioManager->PlayBombWarningSound(GameManager::instance->GetPan(_position));
	_wasAboutToExplodeLastFrame = aboutToExplode;
	
	Rectangle dest = { _position.x, _position.y, _scale * _grabbedScaleMultiplier, _scale * _grabbedScaleMultiplier };
	Vector2 origin = { _radius * _grabbedScaleMultiplier, _radius * _grabbedScaleMultiplier };

	// Body
	DrawTexturePro
	(	GameManager::instance->sprBombBody,
		{   
			float(int((_state == RANDOM_MOVEMENT) ? _animationFrame : 0) * BOMB_SPRITE_SIZE), 
			float(int((aboutToExplode) ? BOMB_ABOUT_TO_EXPLODE : (_type == BOMB_MENU || _type == BOMB_REVIVE || _type == BOMB_POINT_TALLY) ? 0 : _type) * BOMB_SPRITE_SIZE),
			BOMB_SPRITE_SIZE, 
			BOMB_SPRITE_SIZE 
		}, // SOURCE
		dest, origin, 0, { 255, 255, 255, (unsigned char)_alpha }
	);

	// Deco
	DrawTexturePro
	(	GameManager::instance->sprBombDeco,
		{ float(int(_animationFrame) * BOMB_SPRITE_SIZE), float(_animationIndex * BOMB_SPRITE_SIZE), BOMB_SPRITE_SIZE, BOMB_SPRITE_SIZE }, // SOURCE
		dest, origin, 0, { 255, 255, 255, (unsigned char)_alpha }
	);

	// Fuse
	float fuseMultiplier = Clamp((_timeToExplode / BOMB_EXPLODE_FUSE_TIME), 0, 1);

	if (fuseMultiplier < 1)
	{
		BeginBlendMode(BLEND_ADDITIVE);

		float fuseOffsetY = BOMB_FUSE_SPRITE_OFFSET_Y_MIN + (BOMB_FUSE_SPRITE_OFFSET_Y_MAX - BOMB_FUSE_SPRITE_OFFSET_Y_MIN) * fuseMultiplier;

		Rectangle destFuse = { _position.x, _position.y + fuseOffsetY, _scale * _grabbedScaleMultiplier / 4.f, _scale * _grabbedScaleMultiplier / 4.f };
		Vector2 originFuse = { _radius * _grabbedScaleMultiplier / 4.f, _radius * _grabbedScaleMultiplier / 4.f };

		DrawTexturePro
		(GameManager::instance->sprBombFuse,
			{ float((int(_animationFrame) % BOMB_FUSE_SPRITES) * BOMB_FUSE_SPRITE_SIZE), float(_animationIndex * BOMB_FUSE_SPRITE_SIZE), BOMB_FUSE_SPRITE_SIZE, BOMB_FUSE_SPRITE_SIZE }, // SOURCE
			destFuse, originFuse, 0, { 255, 255, 255, (unsigned char)(255 * (1 - fuseMultiplier)) }
		);

		EndBlendMode();
	}

	if (DEBUG_BOMB_HITBOX_DRAW) 
	{
		DrawCircleLinesV(_position, _radiusVisual, _color);

		DrawRectangleLines(
			_position.x - _radiusVisual * BOMB_GRAB_MOUSE_SCALE_INDEX,
			_position.y - _radiusVisual * BOMB_GRAB_MOUSE_SCALE_INDEX,
			_radiusVisual * 2 * BOMB_GRAB_MOUSE_SCALE_INDEX,
			_radiusVisual * 2 * BOMB_GRAB_MOUSE_SCALE_INDEX + BOMB_GRAB_MOUSE_HITBOX_OFFSET_Y,
			BLUE);
	}
}

void Bomb::GameOver() 
{
	float newTimeToExplode = GetRandomValue(100, 300) / 100.f;
	_timeToExplode = min(_timeToExplode, newTimeToExplode);

	_didGameOver = true;
}

bool Bomb::WasClicked(const Vector2 mousePos) const
{
	if (_state == GRABBED) return false;

	return CheckCollisionPointRec(mousePos,
	{	_position.x - _radiusVisual * BOMB_GRAB_MOUSE_SCALE_INDEX,
		_position.y - _radiusVisual * BOMB_GRAB_MOUSE_SCALE_INDEX,
		_radiusVisual * 2 * BOMB_GRAB_MOUSE_SCALE_INDEX,
		_radiusVisual * 2 * BOMB_GRAB_MOUSE_SCALE_INDEX + BOMB_GRAB_MOUSE_HITBOX_OFFSET_Y 
	});
}

void Bomb::Grab() 
{
	SetSoundVolume(_windUpLoopSound, BOMB_WINDUP_LOOP_SOUND_VOLUME);

	_state = GRABBED;
}

int Bomb::LetGo(int releasedState)
{
	if (releasedState == BOMB_RELEASED_TOP) { _state = PLACED; _placedDirection = BOMB_PLACED_TOP; return BOMB_PLACED_TOP; }
	else if (releasedState == BOMB_RELEASED_BOT) { _state = PLACED; _placedDirection = BOMB_PLACED_BOT; return BOMB_PLACED_BOT;  }
	else 
	{
		_state = RANDOM_MOVEMENT; 
		return -1;
	}
}

void Bomb::SnapToBombHouseMin()
{
	if (_placedDirection == BOMB_PLACED_TOP) _position.y = BOMBHOUSE_COORD_TOP_VER_POS - 50;
	else if (_placedDirection == BOMB_PLACED_BOT) _position.y = BOMBHOUSE_COORD_BOT_VER_POS + 50;
}

void Bomb::CheckCollisionWith(Bomb& b)
{
	if (b._state != RANDOM_MOVEMENT || _state != RANDOM_MOVEMENT) return;
	if (b._collidedThisFrame || _collidedThisFrame) return;

	Vector2 delta = Vector2Subtract(_position, b._position);
	float dist = delta.x * delta.x + delta.y * delta.y;
	float minDist = _radiusVisual + b._radiusVisual;

	if (dist < minDist * minDist)
	{
		GameManager::instance->audioManager->PlayBombCollisionSound(GameManager::instance->GetPan(_position));
		ResolveCollisionWith(b, delta, sqrt(dist), minDist);
	}
}

void Bomb::ResolveCollisionWith(Bomb& b, Vector2 delta, float dist, float minDist)
{
	if (dist < 0.0001f)
	{
		delta = { 1.0f, 0.0f };
		dist = 1.0f;
	}

	_collidedThisFrame = true;
	b._collidedThisFrame = true;

	Vector2 normal = Vector2Scale(delta, 1.0f / dist);
	float overlap = minDist - dist;

	_position = Vector2Add(_position, Vector2Scale(normal, overlap * 0.5f));
	b._position = Vector2Subtract(b._position, Vector2Scale(normal, overlap * 0.5f));

	_movementDirection.x *= -1;
	b._movementDirection.x *= -1;
	_movementDirection.y *= -1;
	b._movementDirection.y *= -1;
}


bool Bomb::BombLayerSort(const Bomb* a, const Bomb* b)
{
	if (a->_state == GRABBED) return false;
	if (b->_state == GRABBED) return true;

	return a->_position.y < b->_position.y;
}

bool Bomb::GetIsAboutToExplode() const
{
	return _state != PLACED && !GameManager::instance->IsGameOverCutscene() &&
	(
	(_timeToExplode < 10 && _timeToExplode > 9.5f) ||
	(_timeToExplode < 9 && _timeToExplode > 8.5f) ||
	(_timeToExplode < 8 && _timeToExplode > 7.5f) ||
	(_timeToExplode < 7 && _timeToExplode > 6.5f) ||
	(_timeToExplode < 6 && _timeToExplode > 5.5f) ||

	(_timeToExplode < 5 && _timeToExplode > 4.75f) ||
	(_timeToExplode < 4.5 && _timeToExplode > 4.25f) ||
	(_timeToExplode < 4 && _timeToExplode > 3.75f) ||
	(_timeToExplode < 3.5 && _timeToExplode > 3.25f) ||
	(_timeToExplode < 3 && _timeToExplode > 2.75f) ||
	(_timeToExplode < 2.5f && _timeToExplode > 2.25f) ||

	(_timeToExplode < 2 && _timeToExplode > 1.875f) ||
	(_timeToExplode < 1.75 && _timeToExplode > 1.625f) ||
	(_timeToExplode < 1.5 && _timeToExplode > 1.375f) ||
	(_timeToExplode < 1.25 && _timeToExplode > 1.125f) ||

	(_timeToExplode < 1 && _timeToExplode > 0.875f) ||
	(_timeToExplode < 0.75 && _timeToExplode > 0.625f) ||
	(_timeToExplode < 0.5 && _timeToExplode > 0.375f) ||
	(_timeToExplode < 0.25 && _timeToExplode > 0.125f)
	);
}