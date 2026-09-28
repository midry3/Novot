#pragma once
#include "../Asset.h"

class Texture : public Asset {
protected:
	fs::path file;
	int _handle = -1;
public:
	Texture() = default;
	Texture(const fs::path& file) : Asset(file) {}
	~Texture() = default;
	int Load() override;
	void Release() override;
};