#include "ProjectionGridContainer.h"
#include <Debugger/Debugger.h>
#include <cmath>
#include <algorithm>

Vector2D ProjectionGridContainer::GridToScreenPosition(double gx, double gy) const {
	if (_matrixSize.x <= 0 || _matrixSize.y <= 0) return Vector2D(0, 0);

	Vector2D cellSize(_size.x / _matrixSize.x, _size.y / _matrixSize.y);
	Vector2D step = cellSize + _space;

	switch (_projectionType) {
	case ProjectionType::Orthographic: {
		return Vector2D(gx * step.x, gy * step.y);
	}
	case ProjectionType::Isometric: {
		double posX = (gx - gy) * (step.x * 0.5);
		double posY = (gx + gy) * (step.y * 0.5);
		return Vector2D(posX, posY);
	}
	case ProjectionType::Perspective: {
		double baseX = (gx - (_matrixSize.x - 1.0) * 0.5) * step.x;
		double baseY = gy * step.y;
		double depth = baseY;
		double dist = (std::max)(1.0f, _cameraDistance);
		double factor = dist / (dist + depth);

		double posX = _vanishingPoint.x + (baseX - _vanishingPoint.x) * factor;
		double posY = _vanishingPoint.y + (baseY - _vanishingPoint.y) * factor;
		return Vector2D(posX, posY);
	}
	case ProjectionType::Oblique: {
		Vector2D basePos(gx * step.x, gy * step.y);
		return basePos + _obliqueOffset * gy;
	}
	case ProjectionType::Custom: {
		return _axisX * (gx * step.x) + _axisY * (gy * step.y);
	}
	}
	return Vector2D(0, 0);
}

std::vector<Vector2D> ProjectionGridContainer::GetCellVertices(double gx, double gy) const {
	Vector2D v0 = GridToScreenPosition(gx, gy);
	Vector2D v1 = GridToScreenPosition(gx + 1.0, gy);
	Vector2D v2 = GridToScreenPosition(gx + 1.0, gy + 1.0);
	Vector2D v3 = GridToScreenPosition(gx, gy + 1.0);
	return { v0, v1, v2, v3 };
}

Vector2D ProjectionGridContainer::ScreenToGridPosition(const Vector2D& screenPos) const {
	if (_matrixSize.x <= 0 || _matrixSize.y <= 0) return Vector2D(0, 0);

	Vector2D cellSize(_size.x / _matrixSize.x, _size.y / _matrixSize.y);
	Vector2D step = cellSize + _space;

	switch (_projectionType) {
	case ProjectionType::Orthographic: {
		double gx = screenPos.x / (step.x != 0 ? step.x : 1.0);
		double gy = screenPos.y / (step.y != 0 ? step.y : 1.0);
		return Vector2D(gx, gy);
	}
	case ProjectionType::Isometric: {
		double u = screenPos.x / (step.x * 0.5 != 0 ? step.x * 0.5 : 1.0);
		double v = screenPos.y / (step.y * 0.5 != 0 ? step.y * 0.5 : 1.0);
		double gx = (u + v) * 0.5;
		double gy = (v - u) * 0.5;
		return Vector2D(gx, gy);
	}
	case ProjectionType::Oblique: {
		double stepYEff = step.y + _obliqueOffset.y;
		double gy = screenPos.y / (stepYEff != 0 ? stepYEff : 1.0);
		double posXEff = screenPos.x - _obliqueOffset.x * gy;
		double gx = posXEff / (step.x != 0 ? step.x : 1.0);
		return Vector2D(gx, gy);
	}
	default: {
		double gx = screenPos.x / (step.x != 0 ? step.x : 1.0);
		double gy = screenPos.y / (step.y != 0 ? step.y : 1.0);
		return Vector2D(gx, gy);
	}
	}
}

ProjectionGridContainer* ProjectionGridContainer::SetCellColor(size_t gx, size_t gy, unsigned int color) {
	if (gx < static_cast<size_t>(_matrixSize.x) && gy < static_cast<size_t>(_matrixSize.y)) {
		size_t index = gy * static_cast<size_t>(_matrixSize.x) + gx;
		if (_cellColors.size() <= index) {
			_cellColors.resize(static_cast<size_t>(_matrixSize.x * _matrixSize.y), _defaultCellColor);
		}
		_cellColors[index] = color;
		if (index < _gridPolygons.size() && _gridPolygons[index]) {
			_gridPolygons[index]->SetColor(color);
		}
	}
	return this;
}

std::shared_ptr<ColorPolygon> ProjectionGridContainer::GetCellPolygon(size_t gx, size_t gy) const {
	if (gx < static_cast<size_t>(_matrixSize.x) && gy < static_cast<size_t>(_matrixSize.y)) {
		size_t index = gy * static_cast<size_t>(_matrixSize.x) + gx;
		if (index < _gridPolygons.size()) {
			return _gridPolygons[index];
		}
	}
	return nullptr;
}

void ProjectionGridContainer::_SyncGridPolygons() {
	size_t targetCount = static_cast<size_t>(_matrixSize.x * _matrixSize.y);
	if (targetCount == 0) return;

	if (_gridPolygons.size() != targetCount) {
		// Clean up previous polygons from scene
		for (auto& poly : _gridPolygons) {
			if (poly) {
				DeleteScene(poly);
			}
		}
		_gridPolygons.clear();
		_gridPolygons.reserve(targetCount);
		_cellColors.resize(targetCount, _defaultCellColor);

		for (size_t i = 0; i < targetCount; ++i) {
			auto poly = CreateScene<ColorPolygon>();
			poly->SetColor(_cellColors[i]);
			_gridPolygons.push_back(poly);
		}
	}
}

void ProjectionGridContainer::_Update(float dt) {
	if (_matrixSize.x <= 0 || _matrixSize.y <= 0) return;

	Vector2D cellSize(_size.x / _matrixSize.x, _size.y / _matrixSize.y);
	Vector2D step = cellSize + _space;

	// Sync and update base ColorPolygons for each grid cell
	if (_showGridBase) {
		_SyncGridPolygons();
		size_t totalCells = static_cast<size_t>(_matrixSize.x * _matrixSize.y);
		for (size_t i = 0; i < totalCells; ++i) {
			if (i < _gridPolygons.size() && _gridPolygons[i]) {
				size_t gx = i % static_cast<size_t>(_matrixSize.x);
				size_t gy = i / static_cast<size_t>(_matrixSize.x);

				_gridPolygons[i]->Show();
				_gridPolygons[i]->SetVertices(GetCellVertices(static_cast<double>(gx), static_cast<double>(gy)));
				if (i < _cellColors.size()) {
					_gridPolygons[i]->SetColor(_cellColors[i]);
				}
			}
		}
	} else {
		for (auto& poly : _gridPolygons) {
			if (poly) poly->Hide();
		}
	}

	// Update positions of child scenes (placed items on grid)
	for (size_t i = 0; i < _gridScenes.size(); ++i) {
		if (auto scene = _gridScenes[i].lock()) {
			size_t gx = i % static_cast<size_t>(_matrixSize.x);
			size_t gy = i / static_cast<size_t>(_matrixSize.x);

			Vector2D projectedPos = GridToScreenPosition(gx, gy);
			scene->SetPosition(projectedPos);

			if (_projectionType == ProjectionType::Perspective && _applyPerspectiveScale) {
				double depth = gy * step.y;
				double dist = (std::max)(1.0f, _cameraDistance);
				double factor = dist / (dist + depth);
				scene->SetScale(factor);
			}
		}
	}
}
