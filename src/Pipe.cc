#include "Pipe.hh"
#include "GameManager.hh"

Pipe::Pipe(const Vector2 p, const float r, const float s) :
    GameObject(p, r, s) 
{
    _state = MENU;

    _steamValue = PIPE_STEAM_MIN_VALUE;
    _rotation = 0.0f;
    _steamEmitterAnimationFrame = 0.0f;
    _steamLifetimeMultiplier = 1.0f;

    _steamIndicatorOffsetX = 0.0f;
    _steamIndicatorOffsetY = 0.0f;

    _steamMaxTimer = 0.0f;

    _valveIndicatorRotation = 0.0f;
    _isValveIndicatorVisible = false;
    _valveIndicatorColorT = 0.0f;

    _timeForNextSteamCloud = PIPE_STEAM_SPAWN_TIME;

    _steamClouds = std::vector<SteamCloud>();

	_steamLoopSound = GameManager::instance->audioManager->GetPipeSteamLoopSound();
	_steamHissLoopSound = GameManager::instance->audioManager->GetPipeSteamHissLoopSound();

    SetSoundVolume(_steamLoopSound, 0.0f);
    PlaySound(_steamLoopSound);
    SetSoundVolume(_steamHissLoopSound, 0.0f);
    PlaySound(_steamHissLoopSound);
}

Pipe::~Pipe() 
{
    GameManager::instance->audioManager->UnloadPipeSteamLoopSound(&_steamLoopSound);
    GameManager::instance->audioManager->UnloadPipeSteamHissLoopSound(&_steamHissLoopSound);
}

void Pipe::Update_Menu(const float deltaTime) 
{
    _steamLifetimeMultiplier = 5.0f;

    _isValveIndicatorVisible = false;
    _steamMaxTimer = 0.0f;

    _steamValue = PIPE_STEAM_MIN_VALUE;
}

void Pipe::Update_Game(const float deltaTime) 
{
    _steamLifetimeMultiplier = 1.0f;

    _steamValue += PIPE_STEAM_INCREMENT_SPEED * deltaTime;
    if (_steamValue > PIPE_STEAM_MAX_VALUE) _steamValue = PIPE_STEAM_MAX_VALUE;

	if (_steamValue >= PIPE_STEAM_MAX_VALUE) _steamMaxTimer += deltaTime;
	else _steamMaxTimer = 0.0f;

	if (_steamMaxTimer >= PIPE_STEAM_MAX_TIMER_THRESHOLD) _isValveIndicatorVisible = true;
	else _isValveIndicatorVisible = false;
}

void Pipe::Update_GameGrabbed(const float deltaTime) 
{
    _steamLifetimeMultiplier = 5.0f;

    if (_steamValue <= PIPE_STEAM_MIN_VALUE) return;
    
    Vector2 mousePos = GameManager::instance->GetWorldMousePos();

    float angle = atan2f(mousePos.y - _position.y, mousePos.x - _position.x);
    float prevAngle = atan2f(_mousePositionLastFrame.y - _position.y, _mousePositionLastFrame.x - _position.x);
    float rotationDelta = angle - prevAngle;
    float angleDiff = fabsf(rotationDelta);
    if (angleDiff > PI) angleDiff = 2.0f * PI - angleDiff;

    if (rotationDelta > PI)  rotationDelta -= 2.0f * PI;
    if (rotationDelta < -PI) rotationDelta += 2.0f * PI;

    _rotation += rotationDelta * RAD2DEG;

    int prevSteamValue = (int)_steamValue;

    _steamValue -= PIPE_STEAM_DECREMENT_SPEED * angleDiff * deltaTime;
    if (_steamValue > PIPE_STEAM_MAX_VALUE) _steamValue = PIPE_STEAM_MAX_VALUE;
    if (_steamValue < PIPE_STEAM_MIN_VALUE) _steamValue = PIPE_STEAM_MIN_VALUE;
    _mousePositionLastFrame = mousePos;

    if (prevSteamValue != (int)_steamValue && prevSteamValue % 3 == 0) GameManager::instance->audioManager->PlayPipeValveRotateSound();

    if (_isValveIndicatorVisible)
    {
        if (_steamValue < PIPE_VALVE_INDICATOR_STEAM_DISAPPEAR_THRESHOLD) _isValveIndicatorVisible = false;
    }
}

void Pipe::UpdateSteamCloud(SteamCloud& cloud, const float deltaTime) 
{
    cloud.lifetime += deltaTime * _steamLifetimeMultiplier;
    float lifetimeNormalized = cloud.lifetime / PIPE_STEAM_CLOUD_LIFETIME;

    cloud.animationFrame += deltaTime * cloud.animationSpeed;
    if (cloud.animationFrame >= PIPE_STEAM_CLOUD_FRAMES) cloud.animationFrame = 0.0f;

    float alphaFunction = -4.0f * lifetimeNormalized * (lifetimeNormalized - 1.0f);
    cloud.alpha = cloud.maxAlpha * alphaFunction;

    if (cloud.lifetime >= PIPE_STEAM_CLOUD_LIFETIME) cloud.markedForDestroy = true;
}

void Pipe::Update(const float deltaTime) 
{
    switch (_state)
    {
        case MENU:
        {
            Update_Menu(deltaTime);
            break;
        }
        case GAME:
        {
            Update_Game(deltaTime);
            break;
        }
        case GAME_GRABBED:
        {
            Update_GameGrabbed(deltaTime);
            break;
        }
    }

    float steamT = Clamp((_steamValue - PIPE_STEAM_SOUND_STEAM_VALUE_MIN) /
        (PIPE_STEAM_SOUND_STEAM_VALUE_MAX - PIPE_STEAM_SOUND_STEAM_VALUE_MIN), 0.0f, 1.0f);
    float steamHissT = Clamp((_steamValue - PIPE_STEAM_HISS_SOUND_STEAM_VALUE_MIN) /
        (PIPE_STEAM_HISS_SOUND_STEAM_VALUE_MAX - PIPE_STEAM_HISS_SOUND_STEAM_VALUE_MIN), 0.0f, 1.0f);

	if (GameManager::instance->IsPaused())
	{
		steamT = 0.0f;
		steamHissT = 0.0f;
	}

    SetSoundVolume(_steamLoopSound, steamT);
    SetSoundVolume(_steamHissLoopSound, steamHissT);

    if (!IsSoundPlaying(_steamLoopSound)) PlaySound(_steamLoopSound);
    if (!IsSoundPlaying(_steamHissLoopSound)) PlaySound(_steamHissLoopSound);

    float _steamIndicatorOffsetMultiplier = Clamp((_steamValue - PIPE_STEAM_SPAWN_THRESHOLD) /
        (PIPE_STEAM_MAX_VALUE - PIPE_STEAM_SPAWN_THRESHOLD), 0.0f, 1.0f);
	_steamIndicatorOffsetX = GetRandomValue(-1, 1) * _steamIndicatorOffsetMultiplier;
	_steamIndicatorOffsetY = GetRandomValue(-1, 1) * _steamIndicatorOffsetMultiplier;

    _timeForNextSteamCloud -= deltaTime;
    if (_timeForNextSteamCloud <= 0 && _steamValue > PIPE_STEAM_SPAWN_THRESHOLD) SpawnSteamCloud();

    _steamEmitterAnimationFrame += PIPE_STEAM_EMITTER_ANIMATION_SPEED * deltaTime;
    if (_steamEmitterAnimationFrame >= PIPE_STEAM_EMITTER_SPRITES) _steamEmitterAnimationFrame = 0.0f;

    for (SteamCloud& cloud : _steamClouds) UpdateSteamCloud(cloud, deltaTime);

    _steamClouds.erase(std::remove_if(_steamClouds.begin(), _steamClouds.end(),
        [](const SteamCloud& cloud) { return cloud.markedForDestroy; }),
        _steamClouds.end());

	_valveIndicatorRotation += PIPE_VALVE_INDICATOR_ROTATION_SPEED * deltaTime;
    if (_valveIndicatorRotation >= 360.0f) _valveIndicatorRotation -= 360.0f;
    else if (_valveIndicatorRotation < 0.0f) _valveIndicatorRotation += 360.0f;

    _valveIndicatorColorT += deltaTime / PIPE_VALVE_INDICATOR_COLOR_CYCLE_TIME;
    _valveIndicatorColorT = fmodf(_valveIndicatorColorT, 1.0f);
}

void Pipe::RenderSteamCloud(const SteamCloud& cloud, const float deltaTime) const
{
    DrawTexturePro(
        GameManager::instance->sprSteamCloud, 
        { (float)(PIPE_STEAM_CLOUD_SPRITE_SIZE * (int)cloud.animationFrame), 0, PIPE_STEAM_CLOUD_SPRITE_SIZE, PIPE_STEAM_CLOUD_SPRITE_SIZE },
        { cloud.position.x - cloud.scaleX / 2, cloud.position.y - cloud.scaleY / 2, cloud.scaleX, cloud.scaleY },
        { cloud.scaleX / 2, cloud.scaleY / 2 }, cloud.rotation, { 255, 255, 255, (unsigned char)(cloud.alpha * 255) }
    );
}

void Pipe::Render(const float deltaTime) 
{
    if (DEBUG_PIPE_DRAW_GRAB_RADIUS)
    {
        DrawRectangleLines(
            _position.x - PIPE_VALVE_GRAB_RADIUS / 2,
            _position.y - PIPE_VALVE_GRAB_RADIUS / 2,
            PIPE_VALVE_GRAB_RADIUS,
            PIPE_VALVE_GRAB_RADIUS,
            { 255, 255, 255, 255 });
    }

    if (DEBUG_PIPE_DRAW_STEAM_VALUE)
    {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%.1f", _steamValue);
        DrawText(buffer, _position.x - PIPE_VALVE_SPRITE_SIZE / 2, _position.y - PIPE_VALVE_SPRITE_SIZE / 2 - 20, 20, { 255, 255, 255, 255 });
    }

    float emitterAlpha = (_steamValue - PIPE_STEAM_SPAWN_THRESHOLD) / (PIPE_STEAM_MAX_VALUE - PIPE_STEAM_SPAWN_THRESHOLD) 
                * (PIPE_STEAM_CLOUD_ALPHA_MAX - PIPE_STEAM_CLOUD_ALPHA_MIN) + PIPE_STEAM_CLOUD_ALPHA_MIN;

    if (emitterAlpha < 0.0f) emitterAlpha = 0.0f;
    DrawTexturePro(
        GameManager::instance->sprSteamEmitter,
        { (float)(PIPE_STEAM_EMITTER_SPRITE_SIZE_X * (int)_steamEmitterAnimationFrame), 0, PIPE_STEAM_EMITTER_SPRITE_SIZE_X, PIPE_STEAM_EMITTER_SPRITE_SIZE_Y },
        { PIPE_STEAM_EMITTER_POSITION_X - PIPE_STEAM_EMITTER_SPRITE_SIZE_X / 2, PIPE_STEAM_EMITTER_POSITION_Y - PIPE_STEAM_EMITTER_SPRITE_SIZE_Y / 2, PIPE_STEAM_EMITTER_SPRITE_SIZE_X, PIPE_STEAM_EMITTER_SPRITE_SIZE_Y },
        { 0, 0 }, PIPE_STEAM_EMITTER_ROTATION, { 255, 255, 255, (unsigned char)(emitterAlpha * 255) }
    );

    float baseTLx = _position.x - PIPE_STEAM_INDICATOR_BASE_SPRITE_SIZE_X / 2.0f + PIPE_STEAM_INDICATOR_BASE_SPRITE_OFFSET_X;
    float baseTLy = _position.y + PIPE_VALVE_SPRITE_SIZE / 2.0f + PIPE_STEAM_INDICATOR_BASE_SPRITE_OFFSET_Y;
    float angle = (_steamValue - PIPE_STEAM_MIN_VALUE) / (PIPE_STEAM_MAX_VALUE - PIPE_STEAM_MIN_VALUE)
              * (PIPE_STEAM_INDICATOR_NEEDLE_ANGLE_MAX - PIPE_STEAM_INDICATOR_NEEDLE_ANGLE_MIN)
              + PIPE_STEAM_INDICATOR_NEEDLE_ANGLE_MIN;

	baseTLx += _steamIndicatorOffsetX;
	baseTLy += _steamIndicatorOffsetY;

    DrawTexturePro(
        GameManager::instance->sprSteamIndicatorBase,
        { 0, 0, PIPE_STEAM_INDICATOR_BASE_SPRITE_SIZE_X, PIPE_STEAM_INDICATOR_BASE_SPRITE_SIZE_Y },
        { baseTLx, baseTLy, PIPE_STEAM_INDICATOR_BASE_SPRITE_SIZE_X, PIPE_STEAM_INDICATOR_BASE_SPRITE_SIZE_Y },
        { 0, 0 }, 0.0f, WHITE
    );

    DrawTexturePro(
        GameManager::instance->sprSteamIndicatorNeedle,
        { 0, 0, PIPE_STEAM_INDICATOR_NEEDLE_SPRITE_SIZE, PIPE_STEAM_INDICATOR_NEEDLE_SPRITE_SIZE },
        { baseTLx + PIPE_STEAM_INDICATOR_NEEDLE_OFFSET_X,
        baseTLy + PIPE_STEAM_INDICATOR_NEEDLE_OFFSET_Y,
        PIPE_STEAM_INDICATOR_NEEDLE_SPRITE_SIZE,
        PIPE_STEAM_INDICATOR_NEEDLE_SPRITE_SIZE },
        { PIPE_STEAM_INDICATOR_NEEDLE_SPRITE_SIZE / 2.0f, PIPE_STEAM_INDICATOR_NEEDLE_SPRITE_SIZE / 2.0f },
        angle,
        WHITE
    );

    DrawTexturePro(
        GameManager::instance->sprPipeValve, 
        { 0, 0, PIPE_VALVE_SPRITE_SIZE, PIPE_VALVE_SPRITE_SIZE },
        { _position.x, _position.y, PIPE_VALVE_SPRITE_SIZE, PIPE_VALVE_SPRITE_SIZE },
        { PIPE_VALVE_SPRITE_SIZE / 2.0f, PIPE_VALVE_SPRITE_SIZE / 2.0f }, 
        _rotation, 
        WHITE
    );

    for (const SteamCloud& cloud : _steamClouds) RenderSteamCloud(cloud, deltaTime);

    if (_isValveIndicatorVisible)
    {
        float t = (sinf(_valveIndicatorColorT * 2.0f * PI) + 1.0f) / 2.0f; // 0..1 suau

        Color c1 = PIPE_VALVE_INDICATOR_COLOR_1;
        Color c2 = PIPE_VALVE_INDICATOR_COLOR_2;
        Color indicatorColor = {
            (unsigned char)(c1.r + t * (c2.r - c1.r)),
            (unsigned char)(c1.g + t * (c2.g - c1.g)),
            (unsigned char)(c1.b + t * (c2.b - c1.b)),
			(unsigned char)((_steamValue - PIPE_VALVE_INDICATOR_STEAM_DISAPPEAR_THRESHOLD) / (PIPE_STEAM_MAX_VALUE - PIPE_VALVE_INDICATOR_STEAM_DISAPPEAR_THRESHOLD) * 255)
        };

        DrawTexturePro(
            GameManager::instance->sprPipeValveIndicator,
            { 0, 0, PIPE_VALVE_INDICATOR_SPRITE_SIZE, PIPE_VALVE_INDICATOR_SPRITE_SIZE },
            { _position.x, _position.y, PIPE_VALVE_INDICATOR_SPRITE_SIZE, PIPE_VALVE_INDICATOR_SPRITE_SIZE },
            { PIPE_VALVE_INDICATOR_SPRITE_SIZE / 2.0f, PIPE_VALVE_INDICATOR_SPRITE_SIZE / 2.0f },
            _valveIndicatorRotation,
            indicatorColor
        );
    }
}

void Pipe::GameOver() 
{
    _state = MENU;
    _steamValue = PIPE_STEAM_MIN_VALUE;
}

void Pipe::SpawnSteamCloud()
{
    int boundsX = (int)GameManager::instance->GetHorizontalBounds().y;

    SteamCloud cloud;
    cloud.position = { (float)GetRandomValue(-boundsX, boundsX), (float)GetRandomValue(PIPE_STEAM_CLOUD_POSITION_Y_MIN, PIPE_STEAM_CLOUD_POSITION_Y_MAX) };
    cloud.rotation = GetRandomValue(0, 360);
    cloud.scaleX = GetRandomValue(PIPE_STEAM_CLOUD_COORD_SIZE_MIN, PIPE_STEAM_CLOUD_COORD_SIZE_MAX);
    cloud.scaleY = GetRandomValue(PIPE_STEAM_CLOUD_COORD_SIZE_MIN, PIPE_STEAM_CLOUD_COORD_SIZE_MAX);

    cloud.lifetime = 0.0f;
    cloud.animationFrame = 0.0f;
    cloud.animationSpeed = GetRandomValue(PIPE_STEAM_CLOUD_ANIMATION_SPEED_MIN, PIPE_STEAM_CLOUD_ANIMATION_SPEED_MAX);
    cloud.alpha = PIPE_STEAM_CLOUD_ALPHA_MIN;
    cloud.maxAlpha = (_steamValue - PIPE_STEAM_SPAWN_THRESHOLD) / (PIPE_STEAM_MAX_VALUE - PIPE_STEAM_SPAWN_THRESHOLD) 
                * (PIPE_STEAM_CLOUD_ALPHA_MAX - PIPE_STEAM_CLOUD_ALPHA_MIN) + PIPE_STEAM_CLOUD_ALPHA_MIN;
    cloud.markedForDestroy = false;
    _steamClouds.push_back(cloud);

    _timeForNextSteamCloud = PIPE_STEAM_SPAWN_TIME * ((float)MAP_COORD_HOR_MAX / boundsX);
}

bool Pipe::WasClicked(const Vector2 mousePos) const 
{
    if (_state == MENU || _state == GAME_GRABBED) return false;

    return CheckCollisionPointRec(mousePos,
    {	_position.x - PIPE_VALVE_GRAB_RADIUS / 2,
        _position.y - PIPE_VALVE_GRAB_RADIUS / 2,
        PIPE_VALVE_GRAB_RADIUS,
        PIPE_VALVE_GRAB_RADIUS
    });
}

void Pipe::StartGame() 
{
    _state = GAME;
    _steamValue = PIPE_STEAM_MIN_VALUE;

    _rotation = 0.0f;
    
    if (DEBUG_PIPE_USE_START_VALUE) _steamValue = DEBUG_PIPE_START_VALUE;
}

void Pipe::Grab() 
{
    _mousePositionLastFrame = GameManager::instance->GetWorldMousePos();
    _state = GAME_GRABBED;
}

void Pipe::LetGo() 
{
    _state = GAME;
}