#pragma once
#include <Event/Signal.h>
#include <Asset/Texture/Texture.h>
#include <Scene.h>
#include <Math/Constants.h>
#include <Backend/Backend.h>

enum StretchType {
	NONE,
	FIT
};

class Sprite2D : public Scene {
protected:
	std::shared_ptr<Texture> _texture;
	StretchType stretch = StretchType::NONE;
	Vector2D _size;
	int _handle = -1;
	void _Release() override;
	void _Draw() const override;
public:
	Signal<> TextureChanged;
	virtual void GetUVRect(float& uMin, float& vMin, float& uMax, float& vMax) const {
		uMin = 0.0f;
		vMin = 0.0f;
		uMax = 1.0f;
		vMax = 1.0f;
	}
	Sprite2D() = default;
	Sprite2D(const std::shared_ptr<Texture>& texture) : _texture(texture) {
		AdjustSize();
	}
	Sprite2D(int handle) : _handle(handle) {}
	~Sprite2D() = default;
	virtual Sprite2D* SetTexture(const std::shared_ptr<Texture>& texture) {
		this->_texture = texture;
		_handle = -1;
		AdjustSize();
		TextureChanged.Emit();
		return this;
	}
	Sprite2D* SetTexture(const fs::path& path) {
		SetTexture(LoadAsset<Texture>(path));
		return this;
	}
	Sprite2D* SetTexture(int handle) {
		_handle = handle;
		AdjustSize();
		return this;
	}
	void ChangeStretch(StretchType type);
	int GetHandle() {
		if (_handle == -1) {
			_handle = _texture->Load();
		}
		return _handle;
	}
	void AdjustScale();
	void AdjustSize();
	const Vector2D& GetRawSize() const override {
		return _size;
	}
};