#pragma once
#include "../Asset.h"
#include <Math/Vector2D.h>

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
	const Vector2D GetSize() const;
};