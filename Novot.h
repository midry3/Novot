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
	static std::function<void()> _setupFunction = nullptr;
	static std::shared_ptr<MV1> _screenMaterial = nullptr;

	int Init();

	void End();
	void Update(float dt);
	bool Draw();
	int StartGame();
	void RegisterSetupFunction(const std::function<void()>& func);

	void SetScreenMaterial(const std::shared_ptr<MV1>& material);
	void SetScreenPixelShader(const std::shared_ptr<PixelShader>& shader);
	template <std::derived_from<PixelShader> T>
	void SetScreenPixelShader() {
		SetScreenPixelShader(std::make_shared<T>());
	}
	void SetConstantBufferToScreenPixelShader(int handle, int slot);
};
