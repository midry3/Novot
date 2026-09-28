#include "Sound.h"
#include <NovotCore.h>

void Sound::Play() {
	Load();
	if (IsLoaded()) {
		//SetVolumeSoundMem(volume, _handle);
		if (isLoop) {
			PlaySoundMem(_handle, DX_PLAYTYPE_LOOP, TRUE);
		}
		else {
			PlaySoundMem(_handle, DX_PLAYTYPE_BACK, TRUE);
		}
	}
}

int Sound::Load() {
	if (_handle != -1) {
		return _handle;
	}
	auto data = GetData();
	_handle = LoadSoundMemByMemImage(
		data.data(),
		data.size()
	);
	if (_handle == -1) {
		_handle = -2;
		DebuggerWriteLine("Failed to load sound: " + GetPath().string());
		return -1;
	}
	return _handle;
}

void Sound::Pause() {
	if (IsLoaded()) {
		StopSoundMem(_handle);
	}
}

void Sound::Release() {
	if (IsLoaded()) {
		StopSoundMem(_handle);
		DeleteSoundMem(_handle);
		_handle = 0;
	}
}