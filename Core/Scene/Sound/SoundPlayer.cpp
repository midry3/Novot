#include "SoundPlayer.h"

void SoundPlayer::_Load() {
	// Do nothing
}

SoundPlayer* SoundPlayer::SetSound(const std::shared_ptr<Sound>& sound) {
	_sound = sound;
	return this;
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