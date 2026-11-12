#include "SoundPlayer.h"
#include <Backend/Backend.h>
#include <Animation/Tween.h>

void SoundPlayer::_Update(float dt) {
	if (_sound) {
		CoreBackend::Sound::SetVolume(_sound->GetHandle(), _volume);
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
	CoreBackend::Sound::SetVolume(_sound->GetHandle(), _volume);
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

SoundPlayer* SoundPlayer::AnimateVolume(int toVolume, float duration) {
	const int fromVolume = _volume;
	const int volumeDelta = toVolume - fromVolume;
	if (duration <= 0.0f || volumeDelta == 0) {
		SetVolume(toVolume);
		return this;
	}
	CreateTweenGroup()->CreateTween(&_volume, toVolume, duration)
		->OnUpdate([this]() {
		SetVolume(_volume);
			});
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
	return _sound ? CoreBackend::Sound::IsPlaying(_sound->GetHandle()) : false;
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