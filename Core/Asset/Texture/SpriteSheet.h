#pragma once
#include <Math/Vector2D.h>
#include <Event/Signal.h>
#include "Texture.h"
#include <vector>
#include <memory>

class SpriteSheet : public Asset {
protected:
	bool isLoop = true;
	bool isPaused = false;
	std::vector<int> frames;
	std::shared_ptr<Texture> texture;
	bool _isEmittedAnimationFinished = false;
	int rows=0, columns=0, currentFrameNumber=0, frameCount=0, fps=0, prevTime=0, width=0, height=0;
	double frameInterval, elapsedTime=0.0, _scale=1.0;
public:
	Signal<int> FrameChanged;
	Signal<> AnimationFinished;
	SpriteSheet() = default;
	SpriteSheet(const fs::path file);
	~SpriteSheet() = default;
	SpriteSheet(std::shared_ptr<Texture> texture, int rows, int columns, int fps, bool isLoop=true) : texture(texture), rows(rows), columns(columns), fps(fps), isLoop(isLoop), frameInterval(1000.0 / fps), currentFrameNumber(0), elapsedTime(0), prevTime(0) {}
	int Load() override;
	int UpdateFrame(float dt);
	[[nodiscard]] int GetCurrentFrame() const {
		if (frames.empty()) return -1;
		return frames[currentFrameNumber];
	}
	[[nodiscard]] int GetCurrentFrameNumber() const {
		return currentFrameNumber;
	}
	[[nodiscard]] double GetScale() const {
		return _scale;
	}
	[[nodiscard]] Vector2D GetFrameSize() const {
		return Vector2D(width / columns, height / rows);
	}
	[[nodiscard]] Vector2D GetSheetSize() const {
		return Vector2D(width, height);
	}
	[[nodiscard]] int GetFrameCount() const {
		return frameCount;
	}
	[[nodiscard]] int GetFPS() const {
		return fps;
	}
	[[nodiscard]] bool IsLoop() const {
		return isLoop;
	}
	void GetUVRect(float& uMin, float& vMin, float& uMax, float& vMax) const;
	void SetLoop(bool loop) {
		isLoop = loop;
	}
	void Release() override;
	void Init();
	void Pause() {
		isPaused = true;
	}
	void Play() {
		if (frames.empty() || currentFrameNumber < 0 ||
			currentFrameNumber >= static_cast<int>(frames.size())) {
			return;
		}
		isPaused = false;
		FrameChanged.Emit(frames[currentFrameNumber]);
	}
	bool IsPaused() const {
		return isPaused;
	}
};
