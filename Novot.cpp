#include "Novot.h"
#include <DxLib.h>
#include "NovotCore.h"
#include <SceneManager.h>
#include "Core/Timer.h"
#include <Mouse/Mouse.h>
#include <Camera.h>
#include <chrono>
#include <EffekseerForDXLib.h>
#include <Graphic/2D.h>

int _Novot::Init() {
	AdditionalInit();
	SetUseDirect3DVersion(DX_DIRECT3D_11);
	ChangeWindowMode(TRUE);
	SetDrawScreen(DX_SCREEN_BACK);
	SetAlwaysRunFlag(TRUE);
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
	auto wsize = PropertyManager.GetSettingWindowSize();
	SetGraphMode(wsize.x, wsize.y, 32);
	SetWindowSize(wsize.x, wsize.y);
	SetWindowSizeChangeEnableFlag(FALSE);
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);
	if (DxLib_Init() == -1) return -1;
	if (Effekseer_Init(8000) == -1) {
		DxLib_End();
		return -1;
	}
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();
	Effekseer_Set2DSetting(wsize.x, wsize.y);

	SceneManager.Init();

	Setup();

#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
	TimerManager.New(2.0, []() {
		DebuggerUpdate();
		}, -1);
	DebuggerWriteLine("\r\nDebugger initialized.");
#endif
	return 0;
}

void _Novot::End() {
	Effkseer_End();
	DxLib_End();
}

void _Novot::Update(float dt) {
#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
	DebuggerProcessCommands();
#endif
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
		Mouse.UpdateMouseEvent();
		SceneManager.Update(dt);
		UpdateEffekseer2D(dt);
	}
}

bool _Novot::Draw() const {
	if (ClearDrawScreen() != 0) {
		return false;
	}
	SceneManager.Draw();
	return true;
}

void _Novot::SetScreenPixelShader(const std::shared_ptr<PixelShader>& shader) {
	_screenPixelShader = shader;
}

void _Novot::SetConstantBufferToScreenPixelShader(int handle, int slot) {
	_constantBufferHandle = handle;
	_constantBufferSlot = slot;
}

int _Novot::StartGame() {
	Init();

	int prevTime = GetNowCount();
	int currentTime;
	float dt = 0.0f;
	const float iframe = PropertyManager.GetFrameInterval();
	const Vector2D wsize = PropertyManager.GetSettingWindowSize();
	const int screen = MakeScreen(wsize.x, wsize.y);

	while (ProcessMessage() == 0) {
		Effekseer_Sync3DSetting();
		currentTime = GetNowCount();
		dt += (currentTime - prevTime);
		prevTime = currentTime;
		if (dt < iframe) {
			WaitTimer(iframe - dt);
			dt = iframe;
		}

		// 更新======
#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
		auto t0 = std::chrono::high_resolution_clock::now();
#endif
		Update(dt/1000.0f);
#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
		auto t1 = std::chrono::high_resolution_clock::now();
#endif
		// ======

		// 描画======
		SetDrawScreen(screen);
		if (not Draw()) {
			break;
		}
		SetDrawScreen(DX_SCREEN_BACK);
		if (_screenPixelShader) {
			SetShaderConstantBuffer(_constantBufferHandle, DX_SHADERTYPE_PIXEL, _constantBufferSlot);
			NovotRenderer::DrawRotaGraph3Shader(
				wsize.x / 2,
				wsize.y / 2,
				wsize.x / 2,
				wsize.y / 2,
				1.0f, 1.0f, 0.0f,
				screen, TRUE,
				_screenPixelShader->GetHandle()
			);
		}
		else {
			DrawExtendGraph(
				0,
				0,
				wsize.x,
				wsize.y,
				screen,
				TRUE
			);
		}

#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
		auto t2 = std::chrono::high_resolution_clock::now();

		double updateMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
		double drawMs = std::chrono::duration<double, std::milli>(t2 - t1).count();
		DebuggerRecordFrameTime(updateMs, drawMs);
		if (updateMs + drawMs > 16.67) {
			DebuggerWriteLine("[WARN] Frame spike detected: " + std::to_string(updateMs + drawMs) + " ms");
		}
#endif
		ScreenFlip();
		// ======

#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
		DebuggerSetProperty("FPS", std::to_string(static_cast<int>(std::round(1000.0f / dt))));
#endif
		dt = 0.0f;
	}

	End();
	return 0;
}