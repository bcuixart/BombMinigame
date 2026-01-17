#ifndef GAMEMANAGER_HH
#define GAMEMANAGER_HH

#include <iostream>
#include <vector> 
#include <algorithm> 

#include <raylib.h>
#include <raymath.h>

#include "GameObject.hh"
#include "Bomb.hh"
#include "BombHouse.hh"
#include "Constants.hh"

using namespace std;

class GameManager {
public:
	GameManager();

	void Update(const float deltaTime);
	void Render(const float deltaTime);

	void GameOver();

	void InstantiateBomb(Bomb* obj);
	void DestroyBomb(Bomb* obj);

	int GetBombReleasedState(Bomb* obj);

	Vector2 GetWorldMousePos() const;

	static GameManager* instance;

	Texture2D sprBombBody = LoadTexture("Assets/Sprites/Spr_Bomb_Body.png");
	Texture2D sprBombDeco = LoadTexture("Assets/Sprites/Spr_Bomb_Deco.png");

protected:

private:
	void StartGame();

	vector<Bomb*> _bombGameObjects;

	Camera2D _cam = { 0 };

	const float BOMB_SPAWN_TIME_START = 3;
	float _bombSpawnTime = 3;

	float _timeToSpawnNextBomb = 0;

	BombHouse* _bombHouseTop;
	BombHouse* _bombHouseBottom;

	bool _didGameOver;

	Texture2D _sprMapBG = LoadTexture("Assets/Sprites/Spr_Map_BG.png");
};

#endif