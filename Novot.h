#pragma once
#include <Singleton.h>
#include <NovotCore.h>
#include <SceneManager.h>
#include <Asset/Shader/PixelShader.h>
#include <Asset/AssetManager.h>
#include <Windows.h>
#include <Asset/Model/MV1.h>

namespace Novot {
	static int _constantBufferHandle = -1, _constantBufferSlot = -1;
	static std::shared_ptr<PixelShader> _screenPixelShader;
	static std::shared_ptr<MV1> _screenMaterial = nullptr;

	/// <summary>
	///  @brief ゲームの初期化処理を行う関数。オーバーライドして使用する。
	/// </summary>
	void Setup();

	/// <summary>
	///  @brief ゲームのメイン関数。オーバーライドして使用する。最後に return StartGame(); を行うこと。
	/// </summary>
	int Main();

	int Init();

	/// <summary>
	/// @brief ゲームのメインループを開始する関数。Main関数の返り値にする。
	/// </summary>
	/// <returns></returns>
	int StartGame();
	void End();

	void Update(float dt);
	bool Draw();
	void RegisterSetupFunction(const std::function<void()>& func);

	void SetScreenMaterial(const std::shared_ptr<MV1>& material);
	void SetScreenPixelShader(const std::shared_ptr<PixelShader>& shader);
	template <std::derived_from<PixelShader> T>
	void SetScreenPixelShader() {
		SetScreenPixelShader(std::make_shared<T>());
	}
	void SetConstantBufferToScreenPixelShader(int handle, int slot);
};

using namespace Novot;

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int);