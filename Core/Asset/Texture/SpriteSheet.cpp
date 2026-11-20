#include "SpriteSheet.h"
#include "AnimationLoader.h"
#include <Backend/Backend.h>

SpriteSheet::SpriteSheet(const fs::path file) : Asset(file) {
	texture = std::make_shared<Texture>(file);
	auto info = AnimationLoader::LoadAnimationInfo(file);
	rows = info.rows;
	columns = info.columns;
	fps = info.fps;
	_scale = info.scale;
	frameInterval = 1000.0 / fps;
	isLoop = info.loop;
}

int SpriteSheet::Load() {
	frames.resize(rows * columns);
	CoreBackend::Texture::Size(texture->Load(), &width, &height);
	auto size = GetFrameSize();
	auto data = texture->GetData();
	frameCount = rows * columns;
	if (CoreBackend::Texture::LoadDiv(data.data(), (int)data.size(), rows * columns, columns, rows, size.x, size.y, frames.data()) == -1) {
		_handle = -2;
		return -1;
	}
	Init();
	_handle = frames[0];
	return _handle;
}

void SpriteSheet::Init() {
	currentFrameNumber = isReverse ? frameCount - 1 : 0;
	if (currentFrameNumber < 0) currentFrameNumber = 0;
	elapsedTime = 0;
	_isEmittedAnimationFinished = false;
	prevTime = CoreBackend::Clock::Now();
}

int SpriteSheet::UpdateFrame(float dt) {
	if (IsLoaded() && not frames.empty() && frameCount > 0) {
		if (isPaused) {
			return _handle;
		}
		elapsedTime += dt * 1000;
		if (frameInterval <= elapsedTime) {
			if (isLoop) {
				currentFrameNumber = isReverse
					? (currentFrameNumber - 1 + frameCount) % frameCount
					: (currentFrameNumber + 1) % frameCount;
			} else {
				const int endFrame = isReverse ? 0 : frameCount - 1;
				if (not _isEmittedAnimationFinished && currentFrameNumber == endFrame) {
					AnimationFinished.Emit();
					_isEmittedAnimationFinished = true;
				}
				currentFrameNumber = isReverse
					? (std::max)(currentFrameNumber - 1, 0)
					: (std::min)(currentFrameNumber + 1, frameCount - 1);
			}
			elapsedTime = 0;
			FrameChanged.Emit(frames[currentFrameNumber]);
		}
		_handle = frames[currentFrameNumber];
		return _handle;
	}
	return -1;
}

void SpriteSheet::Release() {
	for (auto handle : frames) {
		CoreBackend::Texture::Release(handle);
	}
	frames.clear();
	_handle = -1;
}

void SpriteSheet::GetUVRect(float& uMin, float& vMin, float& uMax, float& vMax) const {
	if (width <= 0 || height <= 0 || columns <= 0 || rows <= 0) {
		uMin = 0.0f; vMin = 0.0f; uMax = 1.0f; vMax = 1.0f;
		return;
	}
	int frameW = width / columns;
	int frameH = height / rows;
	int col = currentFrameNumber % columns;
	int row = currentFrameNumber / columns;

	uMin = static_cast<float>(col * frameW) / static_cast<float>(width);
	vMin = static_cast<float>(row * frameH) / static_cast<float>(height);
	uMax = static_cast<float>((col + 1) * frameW) / static_cast<float>(width);
	vMax = static_cast<float>((row + 1) * frameH) / static_cast<float>(height);
}