#pragma once
#include <Scene.h>
#include "../Asset.h"
#include <vector>
#include <memory>

class SpriteSheet : public Asset {
protected:
	bool isLoop = true;
	bool isPaused = false;
	std::vector<int> frames;
	std::shared_ptr<Asset> texture;
	int rows, columns, currentFrameNumber, fps, prevTime, width=0, height=0;
	double frameInterval, elapsedTime;
public:
	SpriteSheet() = default;
	~SpriteSheet() = default;
	SpriteSheet(std::shared_ptr<Asset> texture, int rows, int columns, int fps, bool isLoop=true) : texture(texture), rows(rows), columns(columns), fps(fps), isLoop(isLoop), frameInterval(1000.0 / fps), currentFrameNumber(0), elapsedTime(0) {}
	SpriteSheet(std::string file, int rows, int columns, int fps, bool isLoop = true) : texture(std::make_shared<Asset>(file)), rows(rows), columns(columns), fps(fps), isLoop(isLoop), frameInterval(1000.0 / fps), currentFrameNumber(0), elapsedTime(0) {}
	int Load() override;
	int UpdateFrame(float dt);
	int GetCurrentFrame() const {
		if (frames.empty()) return -1;
		return frames[currentFrameNumber];
	}
	int GetCurrentFrameNumber() const {
		return currentFrameNumber;
	}
	Vector2D GetFrameSize() const {
		return Vector2D(width / columns, height / rows);
	}
	void Release() override;
	void Pause() {
		isPaused = true;
	}
	void Play() {
		isPaused = false;
	}
	bool IsPaused() const {
		return isPaused;
	}
};
