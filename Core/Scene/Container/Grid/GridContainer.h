#pragma once
#include <Scene.h>

class GridContainer : public Scene {
protected:
	Vector2D _matrixSize, _size;
	std::vector<std::weak_ptr<Scene>> grid_scenes;
	void _Load() override {};
	void _Release() override {};
	void _Update(float dt) override;
	void _Draw() const override {};
public:
	GridContainer() = default;
	~GridContainer() = default;
	void SetMatrixSize(const Vector2D& size) {
		_matrixSize = size;
		grid_scenes.resize(static_cast<size_t>(size.x * size.y));
	}
	void SetSize(const Vector2D& size) {
		_size = size;
	}
	void AddScene(std::shared_ptr<Scene> scene) {
		scene->Load();
		auto n = static_cast<size_t>(scenes.size());
		scenes.emplace_back(scene);
		grid_scenes[n] = scene;
		scene->SetParent(weak_from_this());
	}
	template <std::derived_from<Scene> T>
	std::shared_ptr<T> CreateScene() {
		auto scene = std::make_shared<T>();
		AddScene(scene);
		return scene;
	}
	bool IsFull() const {
		return scenes.size() >= static_cast<size_t>(_matrixSize.x * _matrixSize.y);
	}
};