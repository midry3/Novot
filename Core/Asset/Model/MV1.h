#pragma once
#include <Asset/Asset.h>

class MV1 : public Asset {
public:
	MV1(const fs::path& path) : Asset(path) {}
	int Load() override;
	void Release() override;
};