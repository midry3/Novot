#pragma once
#include <Scene.h>

class GridContainer : public Scene {
protected:
	Vector2D _matrixSize, _size, _space;
	std::vector<std::weak_ptr<Scene>> grid_scenes;
	void _Update(float dt) override;
public:
	GridContainer() = default;
	~GridContainer() = default;
	GridContainer* SetMatrixSize(const Vector2D& size) {
		_matrixSize = size;
		grid_scenes.resize(static_cast<size_t>(size.x * size.y));
		return this;
	}
	GridContainer* SetSize(const Vector2D& size) {
		_size = size;
		return this;
	}
	GridContainer* SetSpace(const Vector2D& space) {
		_space = space;
		return this;
	}
	void AddScene(std::shared_ptr<Scene> scene) {
		scene->Load();
		auto n = static_cast<size_t>(_scenes.size());
		_scenes.emplace_back(scene);
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
		return _scenes.size() >= static_cast<size_t>(_matrixSize.x * _matrixSize.y);
	}
};