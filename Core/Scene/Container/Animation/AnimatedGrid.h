#pragma once
#include <Scene/Container/Grid/GridContainer.h>
#include <Animation/EasingFunction.h>

enum Direction {
	Horizontal,
	Vertical
};

class AnimatedGrid : public GridContainer {
protected:
	Direction _direction = Horizontal;
	Easing::EasingFunction _easing = Easing::Linear;
	double _animationTime = 0.5;
	size_t _bookedCount = 0;
	bool _isSkipUpdate = false;

	void _Update(float dt) override;

	void UpdateMatrixSize() {
		if (_direction == Horizontal) {
			_matrixSize = Vector2D(static_cast<double>(_gridScenes.size()), 1);
		}
		else {
			_matrixSize = Vector2D(1, static_cast<double>(_gridScenes.size()));
		}
	}

	void UpdateCellSize() override {
		if (_direction == Horizontal) {
			_cellSize = Vector2D(_size.x / (_matrixSize.x + _bookedCount), _size.y);
		}
		else {
			_cellSize = Vector2D(_size.x, _size.y / (_matrixSize.y + _bookedCount));
		}
	}

	void AnimateReflow();
public:
	Signal<> BookCompleted;
	Vector2D GetNextCellPosition() const {
		return GetPositionAt(_gridScenes.size());
	}

	GridContainer* SetMatrixSize(const Vector2D& size) = delete;
	
	AnimatedGrid* SetDirection(Direction dir) {
		_direction = dir;
		UpdateMatrixSize();
		UpdateCellSize();
		return this;
	}

	AnimatedGrid* SetEasing(Easing::EasingFunction easing) {
		_easing = easing;
		return this;
	}

	AnimatedGrid* SetAnimationTime(double time) {
		_animationTime = time;
		return this;
	}

	AnimatedGrid* AddScene(std::shared_ptr<Scene> scene) override {
		scene->Load();
		if (0 < _bookedCount) {
			_bookedCount--;
		}
		_scenes.emplace_back(scene);
		_gridScenes.emplace_back(scene);
		UpdateMatrixSize();
		UpdateCellSize();
		scene->SetParent(weak_from_this());
		return this;
	}

	void DeleteScene(int index) override;
	void DeleteScene(std::shared_ptr<Scene> scene) override;
	void Book(int num);
};