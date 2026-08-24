#include "GridContainer.h"

void GridContainer::_Update(float dt) {
	if (scenes.empty()) return;
	auto cellSize = Vector2D(_size.x / _matrixSize.x, _size.y / _matrixSize.y);
	for (size_t i = 0; i < grid_scenes.size(); i++) {
		if (auto scene = grid_scenes[i].lock()) {
			scene->SetPosition(Vector2D((i % static_cast<size_t>(_matrixSize.x)) * cellSize.x, (i / static_cast<size_t>(_matrixSize.x)) * cellSize.y));
		}
	}
	DebuggerSetProperty("GridContainer_Size", std::to_string(grid_scenes.size()));
}