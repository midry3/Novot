#include "Scene.h"
#include <SceneManager.h>
#include <Timer.h>
#include <Tween.h>

bool Scene::IsShouldDraw() const {
	if (!visible) return false;
	if (parent.expired()) {
		return true;
	}
	else {
		return parent.lock()->IsShouldDraw();
	}
}

void Scene::Load() {
	if (_isLoaded) return;
	ID = SceneManager.GetID();
	_Load();
	auto scenesCopy = scenes;
	for (const auto& s : scenesCopy) {
		s->Load();
	}
	_isLoaded = true;
}

void Scene::Release() {
	_Release();
	AssetLoader.ReleaseAsset(ID);
	auto scenesCopy = scenes;
	for (const auto& s : scenesCopy) {
		s->Release();
	}
}

void Scene::Update(float dt) {
	if (SceneManager.IsCancelUpdate()) {
		return;
	}
	_Update(dt);
	if (SceneManager.IsCancelUpdate()) {
		return;
	}
	auto scenesCopy = scenes;
	for (const auto& s : scenesCopy) {
		s->Update(dt);
	}
}

void Scene::Draw() const {
	if (!IsShouldDraw()) return;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	SetDrawBright(modulate.x, modulate.y, modulate.z);
	_Draw();
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	auto scenesCopy = scenes;
	for (const auto& s : scenesCopy) {
		s->Draw();
	}
}

Vector2D Scene::GetGlobalPosition() const {
	if (parent.expired()) {
		return position + transform;
	}
	else {
		return parent.lock()->GetGlobalPosition() + position + transform;
	}
}

Vector2D Scene::GetDrawPosition() const {
	return GetGlobalPosition() - Camera.GetPoint();
}

std::shared_ptr<Timer> Scene::CreateTimer(double time, const std::function<void()> callback, int loop) {
	auto timer = CreateScene<Timer>();
	timer->SetTime(time)
		->SetLoop(loop)
		->OnTimeout(callback)
		->OnCompleted([timer]() {
			timer->Free();
		});
	return timer;
}

std::shared_ptr<TweenGroup> Scene::CreateTweenGroup() {
	auto tweenGroup = CreateScene<TweenGroup>();
	tweenGroup->OnComplete([tweenGroup, this]() {
		tweenGroup->Free();
		});
	return tweenGroup;
}