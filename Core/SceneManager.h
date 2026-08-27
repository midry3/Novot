#pragma once
#include <memory>
#include <Singleton.h>
#include "Scene.h"

class _SceneManager : public Singleton<_SceneManager> {
private:
	std::shared_ptr<Scene> currentScene;
	std::shared_ptr<Scene> nextScene;
	int _nextID;
	float inTime = 1.0f;
	float outTime = 0.5f;
	bool _cancelUpdate = false;
public:
	_SceneManager() = default;
	void Update(float dt);
	void Draw() const;
	void SetCurrentScene(std::shared_ptr<Scene> scene) {
		if (currentScene != nullptr) {
			currentScene->Release();
			_cancelUpdate = true;
		}
		currentScene = scene;
		currentScene->Load();
		DebuggerSetProperty("Current Scene", typeid(*currentScene).name());
		DebuggerRefreshSceneTree();
	}
	template <std::derived_from<Scene> T>
	void SetCurrentScene() {
		SetCurrentScene(std::make_shared<T>());
	}
	int GetID() {
		return _nextID++;
	}
	bool IsCancelUpdate() const {
		return _cancelUpdate;
	}
	void ClearCancelUpdate() {
		_cancelUpdate = false;
	}
	void TransToScene(std::shared_ptr<Scene> scene, float in, float out) {
		
	}
	std::shared_ptr<Scene> GetCurrentScene() {
		return currentScene;
	}
};

#define SceneManager _SceneManager::GetInstance()