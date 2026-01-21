#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include <iostream>
#include <vector> 
#include <algorithm> 
#include <memory> 

#include <raylib.h>
#include <raymath.h>

#include "GameObject.hh"
#include "Bomb.hh"
#include "BombHouse.hh"
#include "Explosion.hh"
#include "Constants.hh"

using namespace std;

class GameManager {
public:
	GameManager();
	~GameManager();

	void Update(const float deltaTime);
	void Render(const float deltaTime);

	void GameOver();

	void InstantiateBomb(std::unique_ptr<Bomb> obj);
	void DestroyBomb(Bomb* obj);

	void InstantiateExplosion(const Vector2 position);
	void DestroyExplosion(Explosion* expl);

	void BombEntered(Bomb* obj, int _placedDirection);

	Vector2 GetWorldMousePos() const;

	static GameManager* instance;

	Texture2D sprBombBody;
	Texture2D sprBombDeco;
	Texture2D sprExplosion;

protected:

private:
	void StartGame();

	void TryGrabBomb(const Vector2 mousePos);
	int GetBombReleasedState(Bomb* obj);

	vector<std::unique_ptr<Bomb>> _bombGameObjects;
	vector<std::unique_ptr<Explosion>> _explosionGameObjects;

	Camera2D _cam = { 0 };

	const float BOMB_SPAWN_TIME_START = 3;
	float _bombSpawnTime = 3;

	float _timeToSpawnNextBomb = 0;

	std::unique_ptr<BombHouse> _bombHouseTop;
	std::unique_ptr<BombHouse> _bombHouseBottom;

	Bomb* _grabbedBomb;

	bool _didGameOver;

	Texture2D _sprMapBG;
};

#endif