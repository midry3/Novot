#pragma once
#include <Scene.h>

class GridContainer : public Scene {
protected:
	Vector2D _matrixSize, _cellSize, _size, _space;
	std::vector<std::weak_ptr<Scene>> _gridScenes;

	void _Update(float dt) override;

	virtual void UpdateCellSize() {
		if (_size.x == 0 && _size.y == 0) {
			_cellSize = Vec2D_0;

			for (const auto& scene : _scenes) {
				if (scene) {
					const auto& size = scene->GetRawSize();
					_cellSize.x = Max(_cellSize.x, size.x);
					_cellSize.y = Max(_cellSize.y, size.y);
				}
			}
		}
		else {
			_cellSize = Vector2D(_size.x / _matrixSize.x, _size.y / _matrixSize.y);
		}
	}
public:
	GridContainer() = default;
	~GridContainer() = default;
	Vector2D GetPositionAt(size_t index) const;
	GridContainer* SetMatrixSize(const Vector2D& size) {
		_matrixSize = size;
		UpdateCellSize();
		_gridScenes.resize(static_cast<size_t>(size.x * size.y));
		return this;
	}
	[[nodiscard]] const Vector2D& GetMatrixSize() const {
		return _matrixSize;
	}
	[[nodiscard]] double GetMaxChildCount() const {
		return _matrixSize.x * _matrixSize.y;
	}
	GridContainer* SetSize(const Vector2D& size) {
		_size = size;
		UpdateCellSize();
		return this;
	}
	GridContainer* SetSpace(const Vector2D& space) {
		_space = space;
		return this;
	}
	GridContainer* AddScene(std::shared_ptr<Scene> scene) override {
		scene->Load();
		auto n = static_cast<size_t>(_scenes.size());
		_scenes.push_back(scene);
		_gridScenes[n] = scene;
		scene->SetParent(weak_from_this());
		UpdateCellSize();
		return this;
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