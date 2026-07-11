#include "GameOverOverlay.hh"
#include "GameManager.hh"

GameOverOverlay::GameOverOverlay(const Vector2 p, const float r, const float s) :
	GameObject(p, r, s)
{
	_didGameOver = false;
	_gameOverTimeElapsed = 0;
}

void GameOverOverlay::Update(float deltaTime)
{
	if (!_didGameOver) return;

	_gameOverTimeElapsed += deltaTime;

	if (_gameOverTimeElapsed < GAMEOVER_OVERLAY_CUTSCENE_FADEIN_TIME)
	{
		_scale = (_gameOverTimeElapsed / GAMEOVER_OVERLAY_CUTSCENE_FADEIN_TIME) * (GAMEOVER_OVERLAY_CUTSCENE_SIZE_END - GAMEOVER_OVERLAY_CUTSCENE_SIZE_START)
			+ GAMEOVER_OVERLAY_CUTSCENE_SIZE_START;
	}
	else if (_gameOverTimeElapsed < GAMEOVER_OVERLAY_CUTSCENE_FADEIN_TIME + GAMEOVER_OVERLAY_CUTSCENE_OSCILATE_TIME)
	{
		_scale = GAMEOVER_OVERLAY_CUTSCENE_SIZE_END + sin((_gameOverTimeElapsed - GAMEOVER_OVERLAY_CUTSCENE_FADEIN_TIME) * GAMEOVER_OVERLAY_CUTSCENE_OSCILATE_VELOCITY) 
			* GAMEOVER_OVERLAY_CUTSCENE_OSCILATE_SCALE;
	}
	else if (_gameOverTimeElapsed < GAMEOVER_OVERLAY_CUTSCENE_TIME)
	{
		float phaseStartTime = GAMEOVER_OVERLAY_CUTSCENE_FADEIN_TIME + GAMEOVER_OVERLAY_CUTSCENE_OSCILATE_TIME;
		float phaseDuration = GAMEOVER_OVERLAY_CUTSCENE_TIME - phaseStartTime;
		float phaseProgress = (_gameOverTimeElapsed - phaseStartTime) / phaseDuration;
		
		_scale = phaseProgress * (GAMEOVER_OVERLAY_CUTSCENE_SIZE_START - GAMEOVER_OVERLAY_CUTSCENE_SIZE_END) + GAMEOVER_OVERLAY_CUTSCENE_SIZE_END;
	}
	else
	{
		_didGameOver = false;
	}
}

void GameOverOverlay::Render(const float deltaTime)
{
	if (!_didGameOver) return;

	float halfScale = _scale / 2.0f;
	float left = _position.x - halfScale;
	float right = _position.x + halfScale;
	float top = _position.y - halfScale;
	float bottom = _position.y + halfScale;

	const float EXTENT = 5000.0f;
	DrawRectangleRec({ -EXTENT,  -EXTENT,  EXTENT * 2, EXTENT + top }, GAMEOVER_OVERLAY_SPRTE_COLOR);
	DrawRectangleRec({ -EXTENT,  bottom,   EXTENT * 2, EXTENT - bottom }, GAMEOVER_OVERLAY_SPRTE_COLOR);
	DrawRectangleRec({ -EXTENT,  top,  EXTENT + left,  _scale }, GAMEOVER_OVERLAY_SPRTE_COLOR);
	DrawRectangleRec({ right,   top,  EXTENT - right, _scale }, GAMEOVER_OVERLAY_SPRTE_COLOR);

	Rectangle dest = { _position.x, _position.y, _scale, _scale };
	Vector2 origin = { _scale / 2, _scale / 2 };
	
	DrawTexturePro
	(GameManager::instance->sprGameOverOverlay,
		{ 0, 0, GAMEOVER_OVERLAY_SPRTE_SIZE, GAMEOVER_OVERLAY_SPRTE_SIZE }, // SOURCE
		dest, origin, 0, GAMEOVER_OVERLAY_SPRTE_COLOR
	);
}

void GameOverOverlay::GameOver()
{
	_didGameOver = true;
	_gameOverTimeElapsed = 0;

	_scale = GAMEOVER_OVERLAY_CUTSCENE_SIZE_START;
}