#pragma once
#include <memory>
#include <Singleton.h>
#include "Scene.h"
#include <Animation/Tween.h>

class ColorRect;

class _SceneManager : public Singleton<_SceneManager> {
private:
	std::shared_ptr<Scene> currentScene;
	std::shared_ptr<Scene> nextScene;
	std::vector<std::shared_ptr<Scene>> _drawTargets, effectScenes;
	std::shared_ptr<ColorRect> _transEffect;
	int _nextID = 0;
	bool _cancelUpdate = false;
public:
	void Init();
	void Update(float dt);
	void EffectUpdate(float dt);
	void Draw() const;
	void EffectDraw() const;
	void SetCurrentScene(std::shared_ptr<Scene> scene);

	template <std::derived_from<Scene> T>
	std::shared_ptr<T> SetCurrentScene() {
		auto scene = std::make_shared<T>();
		SetCurrentScene(scene);
		return scene;
	}

	void AddDrawTarget(std::shared_ptr<Scene> scene) {
		_drawTargets.emplace_back(scene);
	}

	void AddEffectScene(std::shared_ptr<Scene> scene) {
		effectScenes.emplace_back(scene);
		scene->Load();
	}
	void DeleteEffectScene(std::shared_ptr<Scene> scene) {
		std::erase_if(effectScenes, [&scene](const std::shared_ptr<Scene>& s) { return s == scene; });
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
	void TransSceneTo(std::shared_ptr<Scene> scene, double in = 2.0, double out = 2.0);
	std::shared_ptr<Scene> GetCurrentScene() {
		return currentScene;
	}
	template <std::derived_from<Scene> T>
	std::shared_ptr<T> CreateEffect() {
		auto effect = std::make_shared<T>();
		AddEffectScene(effect);
		return effect;
	}
};

#define SceneManager _SceneManager::GetInstance()