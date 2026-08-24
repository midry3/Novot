#pragma once
#include <Asset/Asset.h>
#include <Scene.h>
#include <memory>

enum StretchType {
	NONE,
	FIT
};

class Sprite2D : public Scene {
private:
	std::shared_ptr<Asset> texture;
	StretchType stretch = StretchType::NONE;
	Vector2D size;
	int _handle = -1;
	void AdjustScale();
protected:
	void _Load() override;
	void _Release() override;
	void _Update(float dt) override;
	void _Draw() const override;
public:
	Vector2D scale;
	double rotation = 0.0;
	Sprite2D() : scale(1, 1){}
	Sprite2D(const std::shared_ptr<Asset>& texture) : texture(texture), scale(1, 1) {
		AdjustSize();
	}
	Sprite2D(int handle) : _handle(handle), scale(1, 1) {}
	~Sprite2D() = default;
	void SetTexture(const std::shared_ptr<Asset>& texture) {
		this->texture = texture;
		AdjustSize();
	}
	void ChangeStretch(StretchType type);
	void SetScale(double x) {
		scale = Vector2D(x, x);
	}
	int GetHandle() {
		if (_handle == -1) {
			_handle = texture->Load();
		}
		return _handle;
	}
	void AdjustSize() {
		DebuggerWriteLine("Load Sprite2D...");
		int x = 0, y = 0;
		GetGraphSize(GetHandle(), &x, &y);
		size = Vector2D(x, y);
		DebuggerWriteLine("Loaded texture size: " + std::to_string(size.x) + ", " + std::to_string(size.y));
		_handle = GetHandle();
		AdjustScale();
	}
	void RotateDegree(double degree) {
		rotation = degree * 3.14159265358979323846 / 180.0;
	}
	Vector2D GetSize() const {
		return size * scale;
	}
	Vector2D GetRealSize() const {
		return size;
	}
};