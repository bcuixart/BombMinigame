#include "Pipe.hh"
#include "GameManager.hh"

Pipe::Pipe(const Vector2 p, const float r, const float s) :
    GameObject(p, r, s) 
{
    _state = MENU;

    _steamValue = PIPE_STEAM_MIN_VALUE;
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
    Vector2 mousePos = GameManager::instance->GetWorldMousePos();

    float angle = atan2f(mousePos.y - _position.y, mousePos.x - _position.x);
    float prevAngle = atan2f(_mousePositionLastFrame.y - _position.y, _mousePositionLastFrame.x - _position.x);
    float angleDiff = fabsf(angle - prevAngle);
    if (angleDiff > PI) angleDiff = 2.0f * PI - angleDiff;

    _steamValue -= PIPE_STEAM_DECREMENT_SPEED * angleDiff * deltaTime;
    if (_steamValue > PIPE_STEAM_MAX_VALUE) _steamValue = PIPE_STEAM_MAX_VALUE;
    if (_steamValue < PIPE_STEAM_MIN_VALUE) _steamValue = PIPE_STEAM_MIN_VALUE;
    _mousePositionLastFrame = mousePos;
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
}

void Pipe::Render(const float deltaTime) 
{
    DrawRectangleLines(_position.x - PIPE_VALVE_RADIUS / 2, _position.y - PIPE_VALVE_RADIUS / 2, PIPE_VALVE_RADIUS, PIPE_VALVE_RADIUS, { 255, 255, 255, 255 });

    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%.1f", _steamValue);
    DrawText(buffer, _position.x - PIPE_VALVE_RADIUS / 2, _position.y - PIPE_VALVE_RADIUS / 2 - 20, 20, { 255, 255, 255, 255 });
}

void Pipe::GameOver() 
{
    _state = MENU;
    _steamValue = PIPE_STEAM_MIN_VALUE;
}

bool Pipe::WasClicked(const Vector2 mousePos) const 
{
    if (_state == MENU || _state == GAME_GRABBED) return false;

    return CheckCollisionPointRec(mousePos,
    {	_position.x - PIPE_VALVE_RADIUS / 2,
        _position.y - PIPE_VALVE_RADIUS / 2,
        PIPE_VALVE_RADIUS,
        PIPE_VALVE_RADIUS
    });
}

void Pipe::StartGame() 
{
    _state = GAME;
    _steamValue = 50; // TEST VALUE, MUST BE SET TO PIPE_STEAM_MIN_VALUE
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