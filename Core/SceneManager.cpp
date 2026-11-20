#include "SceneManager.h"
#include <Camera.h>
#include <Scene/Shape/ColorRect.h>
#include <Scene/Texture/Sprite2D.h>
#include <Algorithm/Search.h>
#include <Backend/Backend.h>

namespace {
	class NaturalTransitionScreen : public Sprite2D {
	private:
		int _screenHandle;

	public:
		explicit NaturalTransitionScreen(int screenHandle)
			: Sprite2D(screenHandle), _screenHandle(screenHandle) {
			AdjustSize();
		}

		~NaturalTransitionScreen() override {
			CoreBackend::Texture::Release(_screenHandle);
		}
	};
}

void _SceneManager::Init() {
	_transEffect = std::make_shared<ColorRect>();
	_transEffect->SetColor(CoreBackend::Drawing::Color(0, 0, 0))
		->SetSize(Vector2D(1920, 1080));
}

void _SceneManager::SetCurrentScene(std::shared_ptr<Scene> scene, bool releaseScene) {
	SetCurrentScene(scene, releaseScene, SceneTransitionBehavior::Queue);
}

void _SceneManager::SetCurrentScene(
	std::shared_ptr<Scene> scene,
	bool releaseScene,
	SceneTransitionBehavior behavior
) {
	ExecuteTransition(
		[this, scene, releaseScene]() {
			SetCurrentSceneImmediate(scene, releaseScene);
		},
		behavior
	);
}

void _SceneManager::SetCurrentSceneImmediate(std::shared_ptr<Scene> scene, bool releaseScene) {
	if (currentScene) {
		_cancelUpdate = true;
		currentScene->ExitedMainScene.Emit();

		if (releaseScene) {
			currentScene->Release();
		}
	}
	currentScene = scene;
	currentScene->Load();
	currentScene->EnteredMainScene.Emit();
	DebuggerSetProperty("Current Scene", typeid(*currentScene).name());
	DebuggerRefreshSceneTree();
}

void _SceneManager::ExecuteTransition(
	TransitionRequest request,
	SceneTransitionBehavior behavior
) {
	if (not _isTransitioning) {
		request();
		return;
	}

	if (behavior == SceneTransitionBehavior::Ignore) {
		return;
	}
	if (behavior == SceneTransitionBehavior::QueueLatest) {
		_transitionQueue.clear();
	}
	_transitionQueue.push_back(std::move(request));
}

void _SceneManager::CompleteTransition() {
	_isTransitioning = false;
	while (not _isTransitioning && not _transitionQueue.empty()) {
		auto request = std::move(_transitionQueue.front());
		_transitionQueue.pop_front();
		request();
	}
}

void _SceneManager::TransSceneTo(
	std::shared_ptr<Scene> scene,
	double in,
	double out,
	bool releaseScene,
	SceneTransitionBehavior behavior
) {
	ExecuteTransition(
		[this, scene, in, out, releaseScene]() {
			_isTransitioning = true;
			TransSceneToImmediate(scene, in, out, releaseScene);
		},
		behavior
	);
}

void _SceneManager::TransSceneToImmediate(
	std::shared_ptr<Scene> scene,
	double in,
	double out,
	bool releaseScene
) {
	_transEffect->SetAlpha(0);
	AddEffectScene(_transEffect);
	auto tg = _transEffect->CreateTweenGroup();
	tg->CreateTween(&_transEffect->alpha, 255, out)
		->OnComplete([this, scene, in, releaseScene]() {
			SetCurrentSceneImmediate(scene, releaseScene);
		auto tg2 = scene->CreateTweenGroup();
		tg2->CreateTween(&_transEffect->alpha, 0, in)
			->OnComplete([this]() {
			DeleteEffectScene(_transEffect);
				CompleteTransition();
				});
		});
}

void _SceneManager::NaturalTransSceneTo(
		std::shared_ptr<Scene> scene,
		double duration,
		bool releaseScene,
		SceneTransitionBehavior behavior
) {
		ExecuteTransition(
			[this, scene, duration, releaseScene]() {
				_isTransitioning = true;
				NaturalTransSceneToImmediate(scene, duration, releaseScene);
			},
			behavior
		);
}

void _SceneManager::NaturalTransSceneToImmediate(
		std::shared_ptr<Scene> scene,
		double duration,
		bool releaseScene
) {
	if (scene == nullptr || duration < 0.0) {
			CompleteTransition();
		return;
	}

	const auto windowSize = PropertyManager.GetRealWindowSize();
	const int screenHandle = CoreBackend::Drawing::CreateDrawScreen(
		static_cast<int>(windowSize.x),
		static_cast<int>(windowSize.y)
	);

	if (screenHandle < 0 || not CoreBackend::Drawing::CaptureDrawScreen(
		screenHandle,
		static_cast<int>(windowSize.x),
		static_cast<int>(windowSize.y)
	)) {
		if (0 <= screenHandle) {
			CoreBackend::Texture::Release(screenHandle);
		}
		SetCurrentSceneImmediate(scene, releaseScene);
		CompleteTransition();
		return;
	}
	const int targetAlpha = scene->alpha;
	SetCurrentSceneImmediate(scene, releaseScene);
	scene->SetAlpha(0);

	auto transitionScreen = std::make_shared<NaturalTransitionScreen>(screenHandle);
	AddEffectScene(transitionScreen);

	transitionScreen->CreateTweenGroup()
		->CreateTween(&transitionScreen->alpha, 0, duration)
		->OnComplete([this, transitionScreen]() {
			DeleteEffectScene(transitionScreen);
			CompleteTransition();
		});
	scene->CreateTweenGroup()->CreateTween(&scene->alpha, targetAlpha, duration);
}

void _SceneManager::Update(float dt) {
	Update(dt, dt);
}

void _SceneManager::Update(float dt, float unscaledDt) {
	_drawTargets.clear();
	_ySortDrawTargets.clear();
	_drawUITargets.clear();
#ifdef _DEBUG
	_drawDebugTargets.clear();
#endif
	if (currentScene != nullptr) {
		ClearCancelUpdate();
		currentScene->Update(dt, unscaledDt);
	}
	const auto effectScenesCopy = _effectScenes;
	for (const auto& effectScene : effectScenesCopy) {
		effectScene->Update(dt, unscaledDt);
	}
}

void _SceneManager::EffectUpdate(float dt) {
	const auto effectScenesCopy = _effectScenes;
	for (const auto& effectScene : effectScenesCopy) {
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