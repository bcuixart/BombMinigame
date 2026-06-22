#include "AudioManager.hh"

AudioManager::AudioManager()
{
	for (int i = 0; i < ASSET_SOUND_BOMB_STEP_SOUNDS; i++)
	{
		_bombStepSounds[i] = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_STEP_PREFIX + 
		((i < 10) ? "00" : "0") + std::to_string(i) + ASSET_SOUND_BOMB_STEP_SUFFIX).c_str());
	}
	
	for (int i = 0; i < ASSET_SOUND_BOMB_COLLISION_SOUNDS; i++)
	{
		_bombCollisionSounds[i] = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_COLLISION_PREFIX +
		((i < 10) ? "00" : "0") + std::to_string(i) + ASSET_SOUND_BOMB_COLLISION_SUFFIX).c_str());
	}

	_dramaticDrumSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_DRAMATIC_DRUM).c_str());
	_bombExplosionSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_EXPLOSION).c_str());
	_gameOverAlertSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_GAMEOVER_ALERT).c_str());
	_gameOverJingleSound = LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_GAMEOVER_JINGLE).c_str());

	_gameMusic = LoadMusicStream((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_MUSIC).c_str());
	_factoryAmbience = LoadMusicStream((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_FACTORY_AMBIENCE).c_str());

	_currentBombStepSoundIndex = 0;
	_currentBombCollisionSoundIndex = 0;

	_playingMusic = false;

	PlayMusicStream(_factoryAmbience);
}

AudioManager::~AudioManager()
{
	for (int i = 0; i < ASSET_SOUND_BOMB_STEP_SOUNDS; i++) UnloadSound(_bombStepSounds[i]);
	for (int i = 0; i < ASSET_SOUND_BOMB_COLLISION_SOUNDS; i++) UnloadSound(_bombCollisionSounds[i]);

	UnloadSound(_dramaticDrumSound);
	UnloadSound(_bombExplosionSound);
	UnloadSound(_gameOverAlertSound);
	UnloadSound(_gameOverJingleSound);

	UnloadMusicStream(_gameMusic);
	UnloadMusicStream(_factoryAmbience);
}

void AudioManager::Update(const float deltaTime)
{
	if (_playingMusic)
	{
		UpdateMusicStream(_gameMusic);

		float musicTime = GetMusicTimePlayed(_gameMusic);
		if (musicTime >= ASSET_SOUND_MUSIC_LOOP_END + 1)
		{
			SeekMusicStream(_gameMusic, musicTime - ASSET_SOUND_MUSIC_LOOP_LENGTH);
		}
	}

	UpdateMusicStream(_factoryAmbience);

	float factoryAmbienceTime = GetMusicTimePlayed(_factoryAmbience);
	if (factoryAmbienceTime >= ASSET_SOUND_MUSIC_LOOP_END + 1)
	{
		SeekMusicStream(_factoryAmbience, factoryAmbienceTime - ASSET_SOUND_MUSIC_LOOP_LENGTH);
	}
}

void AudioManager::PlayMusic()
{
	PlayMusicStream(_gameMusic);
	_playingMusic = true;
}

void AudioManager::StopMusic()
{
	StopMusicStream(_gameMusic);
	_playingMusic = false;
}

float AudioManager::GetMusicTime() const
{
	if (_playingMusic) return GetMusicTimePlayed(_gameMusic);

	return GetMusicTimePlayed(_factoryAmbience);
}

void AudioManager::PlayBombStepSound(const float pan)
{
	SetSoundPan(_bombStepSounds[_currentBombStepSoundIndex], pan);
	PlaySound(_bombStepSounds[_currentBombStepSoundIndex]);

	_currentBombStepSoundIndex = (_currentBombStepSoundIndex + 1) % ASSET_SOUND_BOMB_STEP_SOUNDS;
}

void AudioManager::PlayBombCollisionSound(const float pan)
{
	SetSoundPan(_bombCollisionSounds[_currentBombCollisionSoundIndex], pan);
	PlaySound(_bombCollisionSounds[_currentBombCollisionSoundIndex]);

	_currentBombCollisionSoundIndex = (_currentBombCollisionSoundIndex + 1) % ASSET_SOUND_BOMB_COLLISION_SOUNDS;
}

void AudioManager::PlayBombExplosionSound(const float pan)
{
	SetSoundPan(_bombExplosionSound, pan);
	PlaySound(_bombExplosionSound);
}

void AudioManager::PlayDramaticDrum()
{
	PlaySound(_dramaticDrumSound);
}

void AudioManager::PlayGameOverAlertSound()
{
	PlaySound(_gameOverAlertSound);
}

void AudioManager::PlayGameOverJingleSound()
{
	PlaySound(_gameOverJingleSound);
}

Sound AudioManager::GetBombWindUpLoopSound()
{
	return LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_WINDUP_LOOP).c_str());
}

void AudioManager::UnloadBombWindUpLoopSound(Sound* sound)
{
	UnloadSound(*sound);
}

Sound AudioManager::GetBombFuseLoopSound()
{
	return LoadSound((std::string(ASSETS_PATH) + ASSET_SOUNDS_PATH + ASSET_SOUND_BOMB_FUSE_LOOP).c_str());
}

void AudioManager::UnloadBombFuseLoopSound(Sound* sound)
{
	UnloadSound(*sound);
}
