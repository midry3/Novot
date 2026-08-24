#include "SceneManager.h"

void _SceneManager::Update(float dt) {
	if (currentScene != nullptr) {
		ClearCancelUpdate();
		currentScene->Update(dt);
	}
}

void _SceneManager::Draw() const {
	if (currentScene != nullptr) {
		currentScene->Draw();
	}
}