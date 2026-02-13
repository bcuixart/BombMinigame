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

	_currentBombStepSoundIndex = 0;
	_currentBombCollisionSoundIndex = 0;
}

AudioManager::~AudioManager()
{
	for (int i = 0; i < ASSET_SOUND_BOMB_STEP_SOUNDS; i++) UnloadSound(_bombStepSounds[i]);
	for (int i = 0; i < ASSET_SOUND_BOMB_COLLISION_SOUNDS; i++) UnloadSound(_bombCollisionSounds[i]);
}

void AudioManager::Update(const float deltaTime)
{

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
