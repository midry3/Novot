#include "GridContainer.h"

void GridContainer::_Update(float dt) {
	if (_scenes.empty()) return;
	size_t x, y;
	for (size_t i = 0; i < _gridScenes.size(); i++) {
		if (auto scene = _gridScenes[i].lock()) {
			scene->SetPosition(GetPositionAt(i));
		}
	}
}

Vector2D GridContainer::GetPositionAt(size_t index) const {
	if (_scenes.empty()) return Vec2D_0;
	size_t x = index % static_cast<size_t>(_matrixSize.x);
	size_t y = index / static_cast<size_t>(_matrixSize.x);
	return Vector2D(x * _cellSize.x, y * _cellSize.y) + Vector2D(_space.x * x, _space.y * y);
}