#pragma once
#include "Singleton.h"
#include <Event/Signal.h>

class _GameState : public Singleton<_GameState> {
private:
	bool isPaused;
	bool stepFrameRequested = false;

public:
	Signal<> Paused;
	Signal<> Resumed;
	float timeScale = 1.0f;
	_GameState() : isPaused(false), stepFrameRequested(false) {}
	bool IsPaused() const {
		return isPaused;
	}
	void SetPaused(bool paused) {
		isPaused = paused;
		if (isPaused) {
			Paused.Emit();
		}
		else {
			Resumed.Emit();
		}
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