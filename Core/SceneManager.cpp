#include "SceneManager.h"
#include <Camera.h>
#include <Scene/Shape/ColorRect.h>
#include <Algorithm/Search.h>

void _SceneManager::Init() {
	_transEffect = std::make_shared<ColorRect>();
	_transEffect->SetColor(GetColor(0, 0, 0))
		->SetSize(Vector2D(1920, 1080));
}

void _SceneManager::SetCurrentScene(std::shared_ptr<Scene> scene) {
	if (currentScene != nullptr) {
		currentScene->Release();
		_cancelUpdate = true;
	}
	currentScene = scene;
	currentScene->Load();
	DebuggerSetProperty("Current Scene", typeid(*currentScene).name());
	DebuggerRefreshSceneTree();
}

void _SceneManager::TransSceneTo(std::shared_ptr<Scene> scene, double in, double out) {
	_transEffect->SetAlpha(0);
	AddEffectScene(_transEffect);
	auto tg = _transEffect->CreateTweenGroup();
	tg->CreateTween(&_transEffect->alpha, 255, out)
		->OnComplete([this, scene, in]() {
		SetCurrentScene(scene);
		auto tg2 = scene->CreateTweenGroup();
		tg2->CreateTween(&_transEffect->alpha, 0, in)
			->OnComplete([this]() {
			DeleteEffectScene(_transEffect);
				});
		});
}

void _SceneManager::Update(float dt) {
	_drawTargets.clear();
	_ySortDrawTargets.clear();
	_drawUITargets.clear();
#ifdef _DEBUG
	_drawDebugTargets.clear();
#endif
	if (currentScene != nullptr) {
		ClearCancelUpdate();
		currentScene->Update(dt);
	}
	EffectUpdate(dt);
}

void _SceneManager::EffectUpdate(float dt) {
	for (auto& effectScene : _effectScenes) {
		effectScene->Update(dt);
	}
}

void _SceneManager::Draw() const {
	for (auto it = _drawTargets.rbegin(); it != _drawTargets.rend(); ++it) {
		(*it)->Draw();
	}
	for (const auto& scene : _ySortDrawTargets) {
		scene->Draw();
	}
	for (auto it = _drawUITargets.rbegin(); it != _drawUITargets.rend(); ++it) {
		(*it)->Draw();
	}
	EffectDraw();
#ifdef _DEBUG
	DrawDebug();
#endif
}

void _SceneManager::EffectDraw() const {
	for (const auto& effectScene : _effectScenes) {
		effectScene->Draw();
	}
}

void _SceneManager::AddDrawTarget(std::shared_ptr<Scene> scene) {
	if (scene->IsYSortEnabled()) {
		const double y = scene->GetYSortPriority();

		for (auto it = _ySortDrawTargets.begin(); it != _ySortDrawTargets.end(); ++it) {
			if (y <= (*it)->GetYSortPriority()) {
				_ySortDrawTargets.insert(it, scene);
				return;
			}
		}
		_ySortDrawTargets.push_back(scene);
	}
	else {
		_drawTargets.push_back(scene);
	}
}