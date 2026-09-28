#include "SceneManager.h"
#include <Camera.h>
#include <Scene/Shape/ColorRect.h>

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
	currentScene->SetScale(currentScene->GetScale() * Camera.GetZoom());
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
	if (currentScene != nullptr) {
		ClearCancelUpdate();
		currentScene->Update(dt);
	}
	EffectUpdate(dt);
}

void _SceneManager::EffectUpdate(float dt) {
	for (auto& effectScene : effectScenes) {
		effectScene->Update(dt);
	}
}

void _SceneManager::Draw() const {
	for (auto& scene : _drawTargets) {
		scene->Draw();
	}
	EffectDraw();
}

void _SceneManager::EffectDraw() const {
	for (const auto& effectScene : effectScenes) {
		effectScene->Draw();
	}
}