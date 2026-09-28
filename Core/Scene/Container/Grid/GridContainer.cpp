#include "GridContainer.h"

void GridContainer::_Update(float dt) {
	if (_scenes.empty()) return;
	auto cellSize = Vector2D(_size.x / _matrixSize.x, _size.y / _matrixSize.y);
	size_t x, y;
	for (size_t i = 0; i < grid_scenes.size(); i++) {
		if (auto scene = grid_scenes[i].lock()) {
			x = i % static_cast<size_t>(_matrixSize.x);
			y = i / static_cast<size_t>(_matrixSize.x);
			scene->SetPosition(Vector2D(x * cellSize.x, y * cellSize.y) + Vector2D(_space.x * x, _space.y * y));
		}
	}
	DebuggerSetProperty("GridContainer_Size", std::to_string(grid_scenes.size()));
}