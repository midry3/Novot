#pragma once
#include <deque>
#include <functional>
#include <memory>
#include <Singleton.h>
#include "Scene.h"
#include <Animation/Tween.h>

class ColorRect;

enum class SceneTransitionBehavior {
	Queue,
	QueueLatest,
	Ignore
};

class _SceneManager : public Singleton<_SceneManager> {
private:
	using TransitionRequest = std::function<void()>;

	std::shared_ptr<Scene> currentScene;
	std::shared_ptr<Scene> nextScene;
	std::vector<std::shared_ptr<Scene>> _effectScenes,  _drawTargets, _drawUITargets;
	std::list<std::shared_ptr<Scene>> _ySortDrawTargets;
	std::deque<TransitionRequest> _transitionQueue;
	std::shared_ptr<ColorRect> _transEffect;
	int _nextID = 0;
	bool _cancelUpdate = false;
	bool _isTransitioning = false;

	void ExecuteTransition(TransitionRequest request, SceneTransitionBehavior behavior);
	void CompleteTransition();
	void SetCurrentSceneImmediate(std::shared_ptr<Scene> scene, bool releaseScene);
	void TransSceneToImmediate(std::shared_ptr<Scene> scene, double in, double out, bool releaseScene);
	void NaturalTransSceneToImmediate(std::shared_ptr<Scene> scene, double duration, bool releaseScene);

#ifdef _DEBUG
	std::vector<std::shared_ptr<Scene>> _drawDebugTargets;
#endif
public:
	void Init();
	void Update(float dt);
	void Update(float dt, float unscaledDt);
	void EffectUpdate(float dt);
	void Draw() const;
	void EffectDraw() const;
	void SetCurrentScene(std::shared_ptr<Scene> scene, bool releaseScene = true);

	template <std::derived_from<Scene> T>
	std::shared_ptr<T> SetCurrentScene(
		SceneTransitionBehavior behavior = SceneTransitionBehavior::Queue
	) {
		auto scene = std::make_shared<T>();
		SetCurrentScene(scene, true, behavior);
		return scene;
	}

	void SetCurrentScene(
		std::shared_ptr<Scene> scene,
		bool releaseScene,
		SceneTransitionBehavior behavior
	);

	void AddDrawTarget(std::shared_ptr<Scene> scene);

	void AddDrawUITarget(std::shared_ptr<Scene> scene) {
		_drawUITargets.push_back(scene);
	}

	void AddEffectScene(std::shared_ptr<Scene> scene) {
		_effectScenes.push_back(scene);
		scene->Load();
	}
	void DeleteEffectScene(std::shared_ptr<Scene> scene) {
		std::erase_if(_effectScenes, [&scene](const std::shared_ptr<Scene>& s) { return s == scene; });
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
	void TransSceneTo(
		std::shared_ptr<Scene> scene,
		double in = 2.0,
		double out = 2.0,
		bool releaseScene = true,
		SceneTransitionBehavior behavior = SceneTransitionBehavior::Queue
	);
	void NaturalTransSceneTo(
		std::shared_ptr<Scene> scene,
		double duration,
		bool releaseScene = true,
		SceneTransitionBehavior behavior = SceneTransitionBehavior::Queue
	);
	std::shared_ptr<Scene> GetCurrentScene() {
		return currentScene;
	}
	template <std::derived_from<Scene> T>
	std::shared_ptr<T> CreateEffect() {
		auto effect = std::make_shared<T>();
		AddEffectScene(effect);
		return effect;
	}

#ifdef _DEBUG
	void AddDrawDebugTarget(std::shared_ptr<Scene> scene) {
		_drawDebugTargets.emplace_back(scene);
	}

	void DrawDebug() const {
		for (const auto& scene : _drawDebugTargets) {
			scene->Draw();
		}
	}
#endif
};

#define SceneManager _SceneManager::GetInstance()