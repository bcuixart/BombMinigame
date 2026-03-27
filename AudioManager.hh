#ifndef AUDIOMANAGER_HH
#define AUDIOMANAGER_HH

#include <iostream>
#include <string>
#include <raylib.h>
#include <raymath.h>

#include "Constants.hh"

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	void Update(const float deltaTime);

	void PlayMusic();
	void StopMusic();

	float GetMusicTime() const;

	void PlayBombStepSound(const float pan);
	void PlayBombCollisionSound(const float pan);

	void PlayDramaticDrum();

	Sound GetBombWindUpLoopSound();
	void UnloadBombWindUpLoopSound(Sound* sound);

	Sound GetBombFuseLoopSound();
	void UnloadBombFuseLoopSound(Sound* sound);

protected:

private:
	Sound _bombStepSounds[ASSET_SOUND_BOMB_STEP_SOUNDS];
	Sound _bombCollisionSounds[ASSET_SOUND_BOMB_COLLISION_SOUNDS];

	Sound _dramaticDrumSound;

	Music _gameMusic;
	bool _playingMusic;

	Music _factoryAmbience;

	int _currentBombStepSoundIndex;
	int _currentBombCollisionSoundIndex;
};

#endif