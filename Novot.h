#pragma once
#include <Singleton.h>
#include <NovotCore.h>
#include <SceneManager.h>
#include <Asset/Shader/PixelShader.h>
#include <Asset/AssetManager.h>
#include <Windows.h>



class _Novot : public Singleton<_Novot> {
private:
	int _constantBufferHandle=-1, _constantBufferSlot=-1;
	std::shared_ptr<PixelShader> _screenPixelShader;
public:
	int Init();
	void AdditionalInit();
	void Setup();
	void End();
	void Update(float dt);
	bool Draw() const;
	int StartGame();

	void SetScreenPixelShader(const std::shared_ptr<PixelShader>& shader);
	template <std::derived_from<PixelShader> T>
	void SetScreenPixelShader() {
		SetScreenPixelShader(std::make_shared<T>());
	}
	void SetConstantBufferToScreenPixelShader(int handle, int slot);
};

#define Novot _Novot::GetInstance()