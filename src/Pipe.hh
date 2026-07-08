#ifndef PIPE_HH
#define PIPE_HH

#include "GameObject.hh"

using namespace std;

enum PipeState 
{
	MENU,
	GAME,
	GAME_GRABBED,
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

    PipeState _state;

    float _steamValue;

    Vector2 _mousePositionLastFrame;
};

#endif