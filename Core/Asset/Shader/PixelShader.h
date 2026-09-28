#pragma once
#include <Asset/Asset.h>

class PixelShader : public Asset {
public:
	PixelShader() = default;
	PixelShader(const fs::path& file) : Asset(file) {}
	~PixelShader() {
		Release();
	}
	int Load() override;
	void Release() override;
};