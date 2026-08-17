#ifndef PIPE_HH
#define PIPE_HH

#include <vector>

#include "GameObject.hh"

using namespace std;

enum PipeState 
{
	MENU,
	GAME,
	GAME_GRABBED,
};

struct SteamCloud
{
    Vector2 position;
    float rotation;
    float scaleX;
    float scaleY;

    float lifetime;
    float animationFrame;
    float animationSpeed;
    float alpha;
    float maxAlpha;

    bool markedForDestroy;
};

class Pipe : public GameObject {
public:
	Pipe(const Vector2 p, const float r, const float s);
	virtual ~Pipe();

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

	virtual void GameOver();

	bool WasClicked(const Vector2 mousePos) const;

    void StartGame();
    void Grab();
    void LetGo();

protected:

private:
    void Update_Menu(const float deltaTime);
    void Update_Game(const float deltaTime);
    void Update_GameGrabbed(const float deltaTime);

    void UpdateSteamCloud(SteamCloud& cloud, const float deltaTime);
    void RenderSteamCloud(const SteamCloud& cloud, const float deltaTime) const;

    void SpawnSteamCloud();

    PipeState _state;

    float _steamValue;
    float _timeForNextSteamCloud;
    float _steamEmitterAnimationFrame;
    float _steamLifetimeMultiplier;

    float _steamIndicatorOffsetX;
    float _steamIndicatorOffsetY;

    float _steamMaxTimer;

	float _valveIndicatorRotation;
	bool _isValveIndicatorVisible;
    float _valveIndicatorColorT;

    std::vector<SteamCloud> _steamClouds;

    Vector2 _mousePositionLastFrame;

    Sound _steamLoopSound;
    Sound _steamHissLoopSound;
};

#endif