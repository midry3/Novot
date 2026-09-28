#pragma once
#include <Asset/Asset.h>

class EffekseerAsset: public Asset {
public:
	EffekseerAsset() = default;
	EffekseerAsset(const fs::path& file) : Asset(file) {}
	~EffekseerAsset() = default;
	int Load() override;
	void Release() override;
};