#include "Novot.h"
#include <DxLib.h>
#include "NovotCore.h"
#include <SceneManager.h>
#include "Core/Timer.h"
#include <Mouse/Mouse.h>
#include <chrono>
#include <EffekseerForDXLib.h>
#include <Graphic/2D.h>

namespace Novot {
	int Init() {
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

	void End() {
		Effkseer_End();
		DxLib_End();
	}

	void Update(float dt) {
#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
		DebuggerProcessCommands();
#endif
		KeyInput.Update();
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

	bool Draw() {
		if (ClearDrawScreen() != 0) {
			return false;
		}
		SceneManager.Draw();
		return true;
	}

	void SetBackgroundColor(const Vector3D& color) {
		DxLib::SetBackgroundColor(static_cast<int>(color.x), static_cast<int>(color.y), static_cast<int>(color.z));
	}

	void SetScreenMaterial(const std::shared_ptr<MV1>& material) {
		_screenMaterial = material;
	}

	void SetScreenPixelShader(const std::shared_ptr<PixelShader>& shader) {
		_screenPixelShader = shader;
	}

	void SetConstantBufferToScreenPixelShader(int handle, int slot) {
		_constantBufferHandle = handle;
		_constantBufferSlot = slot;
	}

	int StartGame() {
		Init();

		const float iframe = PropertyManager.GetFrameInterval();
		const Vector2D wsize = PropertyManager.GetSettingWindowSize();
		const int screen = MakeScreen(wsize.x, wsize.y);

		float dt = iframe;
		int prevTime = GetNowCount();
		int currentTime;

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
			Update(dt / 1000.0f);
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
			if (ClearDrawScreen() != 0) {
				break;
			}
			if (_screenPixelShader) {
				SetShaderConstantBuffer(_constantBufferHandle, DX_SHADERTYPE_PIXEL, _constantBufferSlot);
				NovotRenderer::DrawGraphShader(
					0,
					0,
					screen,
					TRUE,
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
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	return Novot::Main();
}