#include "Scene.h"
#include <SceneManager.h>
#include <Animation/Tween.h>
#include <Timer.h>
#include <Camera.h>
#include <Graphic/2D.h>

bool Scene::IsShouldDraw() const {
	if (not visible || not _isEnabled) return false;
	if (parent.expired()) {
		return true;
	}
	else {
		return parent.lock()->IsShouldDraw();
	}
}

void Scene::Clear() {
	KillAllTimers();
	KillAllTweenGroups();
	_scenes.clear();
}

void Scene::Load() {
	if (_isLoaded) return;
	ID = SceneManager.GetID();
	_Load();
	auto scenesCopy = _scenes;
	for (const auto& s : scenesCopy) {
		s->SetParent(weak_from_this());
		s->Load();
	}
	_isLoaded = true;
}

void Scene::Release() {
	_Release();
	
	const auto scenesCopy = _scenes;
	for (const auto& s : scenesCopy) {
		s->Release();
	}
}

void Scene::Update(float dt) {
	if (SceneManager.IsCancelUpdate() || not _isEnabled) {
		return;
	}
	_Update(dt);
	if (SceneManager.IsCancelUpdate()) {
		return;
	}
	const auto scenesCopy = _scenes;
	for (auto it=scenesCopy.rbegin(); it!=scenesCopy.rend(); ++it) {
		(*it)->Update(dt);
	}
	if (IsShouldDraw()) {
		SceneManager.AddDrawTargetFront(shared_from_this());
	}
}

void Scene::Draw() const {
	if (not IsShouldDraw()) return;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, GetAlpha());
	SetDrawBright(modulate.x, modulate.y, modulate.z);
	_Draw();
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

Vector2D Scene::GetParentGlobalPosition() const {
	if (parent.expired()) {
		return Vector0;
	}
	else {
		return parent.lock()->GetGlobalPosition();
	}
}
Vector2D Scene::GetParentScale() const {
	if (parent.expired()) {
		return Vector1;
	}
	else {
		return parent.lock()->GetScale();
	}
}

Vector2D Scene::GetGlobalPosition() const {
	return GetParentGlobalPosition() + (position + offset) * GetParentScale();
}

Vector2D Scene::GetDrawPosition() const {
	return (GetGlobalPosition() * Camera.GetZoom() - Camera.GetPoint());
}

Scene* Scene::SetGlobalPosition(const Vector2D& pos) {
	if (parent.expired()) {
		position = pos - offset;
	}
	else {
		const auto p = parent.lock();
		const Vector2D pScale = p->GetScale();
		const Vector2D delta = pos - p->GetGlobalPosition();
		position = Vector2D(
			pScale.x != 0 ? delta.x / pScale.x : 0,
			pScale.y != 0 ? delta.y / pScale.y : 0
		) - offset;
	}
	return this;
}

Vector2D Scene::GetScale() const {
	if (parent.expired()) {
		return scale;
	}
	else {
		return parent.lock()->GetScale() * scale;
	}
}

Vector3D Scene::GetModulate() const {
	if (parent.expired()) {
		return modulate;
	}
	else {
		const auto p = parent.lock();
		const Vector3D pModulate = p->GetModulate();
		return Vector3D(
			modulate.x * pModulate.x / 255.0,
			modulate.y * pModulate.y / 255.0,
			modulate.z * pModulate.z / 255.0
		);
	}
}

std::shared_ptr<Timer> Scene::CreateTimer(double time, const std::function<void()> callback, int loop) {
	auto timer = CreateScene<Timer>();
	timer->SetTime(time)
		->SetLoop(loop)
		->OnTimeout(callback)
		->OnCompleted([timer]() {
			timer->Free();
		});
	_timers.push_back(timer);
	return timer;
}

std::shared_ptr<TweenGroup> Scene::CreateTweenGroup() {
	auto tweenGroup = CreateScene<TweenGroup>();
	auto ptr = tweenGroup->weak_from_this();
	tweenGroup->OnComplete([ptr]() {
		if (auto tg = ptr.lock()) {
			tg->Free();
		}
	});
	_tweenGroups.push_back(tweenGroup);
	return tweenGroup;
}
void Scene::KillAllTweenGroups() {
	for (auto& tweenGroup : _tweenGroups) {
		if (auto tg = tweenGroup.lock()) {
			tg->Kill();
		}
	}
	_tweenGroups.clear();
}

void Scene::SkipAllTweenGroups() {
	for (auto& tweenGroup : _tweenGroups) {
		if (auto tg = tweenGroup.lock()) {
			tg->Skip();
		}
	}
	_tweenGroups.clear();
}

void Scene::KillAllTimers() {
	for (auto& timer : _timers) {
		if (auto t = timer.lock()) {
			t->Kill();
		}
	}
	_timers.clear();
}