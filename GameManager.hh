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

enum GameState 
{
	MAIN_MENU,
	ROUND,
	GAME_OVER,
};

struct RoundValues
{
	int score;

	vector<BombType> spawnableBombTypes;

	BombType currentBombHouseTopType;
	BombType currentBombHouseBottomType;

	float roundTimeElapsed;
	
	float timeToSpawnNextBomb;
	float nextBombSpawnTime;

	float timeToChangeBombHouse;
	float nextBombHouseChangeTime;

	bool addedBlue;
	bool addedGreen;
};

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

	void UpdateMainMenu(const float deltaTime);
	void UpdateRound(const float deltaTime);
	void UpdateGameOver(const float deltaTime);

	void TryGrabBomb(const Vector2 mousePos);
	int GetBombReleasedState(Bomb* obj);

	void CheckBombCollisions();

	BombType GetNewBombType() const;
	void ChangeBombHouseTypes();

	GameState _state;
	RoundValues _roundValues;

	vector<std::unique_ptr<Bomb>> _bombGameObjects;
	vector<std::unique_ptr<Explosion>> _explosionGameObjects;

	Camera2D _cam = { 0 };

	std::unique_ptr<BombHouse> _bombHouseTop;
	std::unique_ptr<BombHouse> _bombHouseBottom;

	Bomb* _grabbedBomb;

	Texture2D _sprMapBG;
};

#endif