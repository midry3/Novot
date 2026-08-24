#pragma once
#include "Singleton.h"

class _GameState : public Singleton<_GameState> {
private:
	bool isPaused;
	bool stepFrameRequested = false;

public:
	float timeScale = 1.0f;
	_GameState() : isPaused(false), stepFrameRequested(false) {}
	bool IsPaused() const {
		return isPaused;
	}
	void SetPaused(bool paused) {
		isPaused = paused;
	}
	void Pause() {
		SetPaused(true);
	}
	void Resume() {
		SetPaused(false);
	}
	void SetTimeScale(float scale) {
		timeScale = scale;
	}
	float GetTimeScale() const {
		return timeScale;
	}

	// Step frame API
	void RequestStepFrame() {
		stepFrameRequested = true;
	}
	bool IsStepFrameRequested() const {
		return stepFrameRequested;
	}
	void ConsumeStepFrameRequest() {
		stepFrameRequested = false;
	}
};

#define GameState _GameState::GetInstance()