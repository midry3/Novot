#pragma once
#include <Singleton.h>
#include <NovotCore.h>
#include <SceneManager.h>
#include <Asset/Shader/PixelShader.h>
#include <Asset/AssetManager.h>
#include <Windows.h>
#include <Asset/Model/MV1.h>
#include <vector>

namespace Novot {
	struct ScreenPixelShaderEffect {
		int id = -1;
		std::shared_ptr<PixelShader> shader;
		int constantBufferHandle = -1;
		int constantBufferSlot = -1;
	};

	static std::vector<ScreenPixelShaderEffect> _screenPixelShaders;
	static std::vector<int> _screenPixelShaderScreens;
	static int _nextScreenPixelShaderId = 0;
	static Vector2D _screenShakeOffset;
	static std::shared_ptr<MV1> _screenMaterial = nullptr;

	/// <summary>
	///  ゲームの初期化処理を行う関数。オーバーライドして使用する。
	/// </summary>
	void Setup();

	/// <summary>
	///  ゲームのメイン関数。オーバーライドして使用する。最後に return StartGame(); を行うこと。
	/// </summary>
	int Main();

	/// <summary>
	/// ゲームの初期化処理を行う関数。StartGame関数の中で呼ばれる。
	/// </summary>
	/// <returns></returns>
	int Init();

	/// <summary>
	/// ゲームのメインループを開始する関数。Main関数の返り値にする。
	/// </summary>
	/// <returns></returns>
	int StartGame();

	/// <summary>
	/// ゲームの終了処理を行う関数。StartGame関数の中で呼ばれる。
	/// </summary>
	void End();

	/// <summary>
	/// ゲームの更新処理を行う関数。
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	/// <summary>
	/// ゲームの描画処理を行う関数。
	/// </summary>
	/// <returns></returns>
	bool Draw();

	void SetBackgroundColor(const Vector3D& color);

	void SetScreenMaterial(const std::shared_ptr<MV1>& material);
	void SetScreenShake(const Vector2D& offset);
	void ClearScreenShake();
	void SetScreenPixelShader(const std::shared_ptr<PixelShader>& shader);
	template <std::derived_from<PixelShader> T>
	void SetScreenPixelShader() {
		SetScreenPixelShader(std::make_shared<T>());
	}
	int AddScreenPixelShader(const std::shared_ptr<PixelShader>& shader);
	template <std::derived_from<PixelShader> T>
	int AddScreenPixelShader() {
		return AddScreenPixelShader(std::make_shared<T>());
	}
	void SetConstantBufferToScreenPixelShader(int handle, int slot=4);
	void SetConstantBufferToScreenPixelShader(int shaderId, int handle, int slot);
	void RemoveScreenPixelShader(int shaderId);
};

using namespace Novot;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int);

#define NovotMain Novot::Main
#define NovotSetup Novot::Setup