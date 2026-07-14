#include "Pipe.hh"
#include "GameManager.hh"

Pipe::Pipe(const Vector2 p, const float r, const float s) :
    GameObject(p, r, s) 
{
    _state = MENU;

    _steamValue = PIPE_STEAM_MIN_VALUE;
    _rotation = 0.0f;

    _timeForNextSteamCloud = PIPE_STEAM_SPAWN_TIME;

    _steamClouds = std::vector<SteamCloud>();
}

Pipe::~Pipe() 
{

}

void Pipe::Update_Menu(const float deltaTime) 
{
    _steamValue = PIPE_STEAM_MIN_VALUE;
}

void Pipe::Update_Game(const float deltaTime) 
{
    _steamValue += PIPE_STEAM_INCREMENT_SPEED * deltaTime;
    if (_steamValue > PIPE_STEAM_MAX_VALUE) _steamValue = PIPE_STEAM_MAX_VALUE;
}

void Pipe::Update_GameGrabbed(const float deltaTime) 
{
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

    _steamValue -= PIPE_STEAM_DECREMENT_SPEED * angleDiff * deltaTime;
    if (_steamValue > PIPE_STEAM_MAX_VALUE) _steamValue = PIPE_STEAM_MAX_VALUE;
    if (_steamValue < PIPE_STEAM_MIN_VALUE) _steamValue = PIPE_STEAM_MIN_VALUE;
    _mousePositionLastFrame = mousePos;
}

void Pipe::UpdateSteamCloud(SteamCloud& cloud, const float deltaTime) 
{
    cloud.lifetime += deltaTime;
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

    _timeForNextSteamCloud -= deltaTime;
    if (_timeForNextSteamCloud <= 0 && _steamValue > PIPE_STEAM_SPAWN_THRESHOLD) SpawnSteamCloud();

    for (SteamCloud& cloud : _steamClouds) UpdateSteamCloud(cloud, deltaTime);

    _steamClouds.erase(std::remove_if(_steamClouds.begin(), _steamClouds.end(),
        [](const SteamCloud& cloud) { return cloud.markedForDestroy; }),
        _steamClouds.end());
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

    float baseTLx = _position.x - PIPE_STEAM_INDICATOR_BASE_SPRITE_SIZE_X / 2.0f + PIPE_STEAM_INDICATOR_BASE_SPRITE_OFFSET_X;
    float baseTLy = _position.y + PIPE_VALVE_SPRITE_SIZE / 2.0f + PIPE_STEAM_INDICATOR_BASE_SPRITE_OFFSET_Y;
    float angle = (_steamValue - PIPE_STEAM_MIN_VALUE) / (PIPE_STEAM_MAX_VALUE - PIPE_STEAM_MIN_VALUE)
              * (PIPE_STEAM_INDICATOR_NEEDLE_ANGLE_MAX - PIPE_STEAM_INDICATOR_NEEDLE_ANGLE_MIN)
              + PIPE_STEAM_INDICATOR_NEEDLE_ANGLE_MIN;

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
    cloud.position = { (float)GetRandomValue(-boundsX, boundsX), (float)GetRandomValue(BOMBHOUSE_COORD_TOP_VER_POS, BOMBHOUSE_COORD_BOT_VER_POS) };
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

    _timeForNextSteamCloud = PIPE_STEAM_SPAWN_TIME;
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