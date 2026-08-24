#pragma once
#include <Collision/MouseCollision.h>
#include <Scene.h>

class Button : public Scene {
protected:
	std::shared_ptr<MouseCollision> collision;
	void _Draw() const override;
	void _Update(float dt) override;
	void _Load() override;
	void _Release() override {};
public:
	Button() = default;
	~Button() = default;
	void RegisterCallback(MouseButton button, MouseButtonState state, const std::function<void(const MouseEvent&)>& callback) {
		collision->RegisterCallback(button, state, callback);
	}
	template<typename... Args>
	void SetArea(Args&&... args) {
		collision->ClearVertices();
		for (auto p : std::initializer_list<Vector2D>{(args)...}) {
			collision->AddVertex(p);
		}
	}
};