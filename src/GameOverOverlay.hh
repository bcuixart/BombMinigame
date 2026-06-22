#ifndef GAMEOVEROVERLAY_HH
#define GAMEOVEROVERLAY_HH

#include "GameObject.hh"

using namespace std;

class GameOverOverlay : public GameObject {
public:
	GameOverOverlay(const Vector2 p, const float r, const float s);
	virtual ~GameOverOverlay() {};

	virtual void Update(const float deltaTime);
	virtual void Render(const float deltaTime);

	virtual void GameOver();

protected:

private:
	bool _didGameOver;

	float _gameOverTimeElapsed;
};

#endif