#include "VisualDebugScene.h"
#include <SceneManager.h>

void VisualDebugScene::Update(float dt) {
	if (SceneManager.IsCancelUpdate() || not _isEnabled) {
		return;
	}
	if (_isUpdateEnabled) {
		_Update(dt);
	}
	if (SceneManager.IsCancelUpdate()) {
		return;
	}
	const auto scenesCopy = _scenes;
	for (auto it = scenesCopy.rbegin(); it != scenesCopy.rend(); ++it) {
		(*it)->Update(dt);
	}
#ifdef _DEBUG
	if (IsShouldDraw()) {
		SceneManager.AddDrawDebugTarget(shared_from_this());
	}
#endif
}