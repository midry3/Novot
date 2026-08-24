#include "SpriteSheet.h"

int SpriteSheet::Load() {
	frames.resize(rows * columns);
	GetGraphSize(texture->Load(), &width, &height);
	auto size = GetFrameSize();
	auto data = texture->GetData();
	if (CreateDivGraphFromMem(data.data(), (int)data.size(), rows * columns, columns, rows, size.x, size.y, frames.data()) == -1) {
		_handle = -2;
		return -1;
	}
	currentFrameNumber = 0;
	elapsedTime = 0;
	prevTime = GetNowCount();
	_handle = frames[0];
	return _handle;
}

int SpriteSheet::UpdateFrame(float dt) {
	if (IsLoaded()) {
		if (isPaused) {
			return _handle;
		}
		elapsedTime += dt * 1000;
		if (frameInterval <= elapsedTime) {
			if (isLoop) {
				currentFrameNumber = (currentFrameNumber + 1) % frames.size();
			} else {
				currentFrameNumber = min(currentFrameNumber + 1, static_cast<int>(frames.size() - 1));
			}
			elapsedTime = 0;
		}
		_handle = frames[currentFrameNumber];
		return _handle;
	}
	return -1;
}

void SpriteSheet::Release() {
	for (auto handle : frames) {
		DeleteGraph(handle);
	}
	frames.clear();
	_handle = -1;
}