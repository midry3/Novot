#include "SoundPlayer.h"

void SoundPlayer::_Update(float dt) {
	if (_sound) {
		ChangeVolumeSoundMem(_volume, _sound->GetHandle());
	}
}

void SoundPlayer::_Release() {
	Stop();
}

SoundPlayer* SoundPlayer::SetSound(const std::shared_ptr<Sound>& sound) {
	_sound = sound;
	return this;
}

SoundPlayer* SoundPlayer::SetVolume(int volume) {
	_volume = Clamp(volume, 0, NORMAL_VOLUME);
	ChangeVolumeSoundMem(_volume, _sound->GetHandle());
	return this;
}

SoundPlayer* SoundPlayer::SetVolume(float volume) {
	SetVolume(static_cast<int>(volume * NORMAL_VOLUME));
	return this;
}
SoundPlayer* SoundPlayer::SetVolume(double volume) {
	SetVolume(static_cast<int>(volume * NORMAL_VOLUME));
	return this;
}

SoundPlayer* SoundPlayer::SetLoop(bool loop) {
	_sound->SetLoop(loop);
	return this;
}

int SoundPlayer::GetVolume() const {
	return _volume;
}

float SoundPlayer::GetVolumeRatio() const {
	return static_cast<float>(_volume) / NORMAL_VOLUME;
}

bool SoundPlayer::IsPlaying() const {
	return _sound ? CheckSoundMem(_sound->GetHandle()) == TRUE : false;
}

void  SoundPlayer::Play() {
	if (_sound) {
		_sound->Play();
	}
}

void SoundPlayer::Pause() {
	if (_sound) {
		_sound->Pause();
	}
}

void SoundPlayer::Stop() {
	if (_sound) {
		_sound->Stop();
	}
}