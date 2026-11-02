#include "Sound.h"
#include <NovotCore.h>
#include <Backend/Backend.h>

void Sound::Play() {
	Load();
	if (IsLoaded()) {
		if (isLoop) {
			CoreBackend::Sound::Play(_handle, true);
		}
		else {
			CoreBackend::Sound::Play(_handle, false);
		}
	}
}

int Sound::Load() {
	if (_handle != -1) {
		return _handle;
	}
	auto data = GetData();
	_handle = CoreBackend::Sound::Load(
		data.data(),
		static_cast<int>(data.size())
	);
	if (_handle == -1) {
		_handle = -2;
		DebuggerWriteLine("Failed to load sound: " + GetPath().string());
		return -1;
	}
	return _handle;
}

void Sound::Pause() {
	Stop();
}

void Sound::Stop() {
	if (IsLoaded()) {
		CoreBackend::Sound::Stop(_handle);
	}
}

void Sound::Release() {
	if (IsLoaded()) {
		CoreBackend::Sound::Stop(_handle);
		CoreBackend::Sound::Release(_handle);
		_handle = -1;
	}
}