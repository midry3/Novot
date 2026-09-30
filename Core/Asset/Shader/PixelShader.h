#pragma once
#include <Asset/Asset.h>

constexpr int NOVOT_CONSTANT_BUFFER_SLOT_START = 4;

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