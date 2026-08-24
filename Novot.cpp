#include "Novot.h"
#include <chrono>

void _Novot::Update(float dt) {
	ClearDrawScreen();
	KeyMapping.Update();
	TimerManager.Update(dt);

	bool shouldUpdate = !GameState.IsPaused();
	if (GameState.IsStepFrameRequested()) {
		shouldUpdate = true;
		GameState.ConsumeStepFrameRequest();
		dt = PropertyManager.GetFrameInterval();
	}
	else if (GameState.IsPaused()) {
		dt = 0.0f;
	}
	else {
		dt *= GameState.GetTimeScale();
	}

	if (shouldUpdate) {
		TweenManager.Update(dt);
		Mouse.UpdateMouseEvent();
		SceneManager.Update(dt);
	}
}

void _Novot::Draw() {
	SceneManager.Draw();
}

int _Novot::StartGame() {
	ChangeWindowMode(TRUE);
	SetDrawScreen(DX_SCREEN_BACK);
	SetAlwaysRunFlag(TRUE);
	auto wsize = PropertyManager.GetSettingWindowSize();
	SetGraphMode(wsize.x, wsize.y, 32);
	if (DxLib_Init() == -1) return -1;
#ifdef _DEBUG
	TimerManager.New(2.0, []() {
		DebuggerUpdate();
		}, true);
	DebuggerWriteLine("\r\nDebugger initialized.");
#endif

	Setup();

	// メインループ
	int prevTime = GetNowCount();
	int currentTime;
	float dt = 0.0f;
	float iframe = PropertyManager.GetFrameInterval();
	while (ProcessMessage() == 0) {
		currentTime = GetNowCount();
		dt = (std::min)(iframe, dt + (currentTime - prevTime) / 1000.0f);
		prevTime = currentTime;
		if (iframe <= dt) {
			// Profile Update and Draw durations
			auto t0 = std::chrono::high_resolution_clock::now();
			Update(dt);
			auto t1 = std::chrono::high_resolution_clock::now();
			Draw();
			auto t2 = std::chrono::high_resolution_clock::now();

			double updateMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
			double drawMs = std::chrono::duration<double, std::milli>(t2 - t1).count();
			DebuggerRecordFrameTime(updateMs, drawMs);
			if (updateMs + drawMs > 16.67) {
				DebuggerWriteLine("[WARN] Frame spike detected: " + std::to_string(updateMs + drawMs) + " ms");
			}

			ScreenFlip();
			DebuggerSetProperty("FPS", std::to_string(static_cast<int>(std::round(1.0f / dt))));
			dt = 0.0f;
		}
	}
	DxLib_End();
	return 0;
}