#pragma once
#include <Collision/MouseCollision.h>
#include <Scene.h>

class Button : public Scene {
protected:
	std::shared_ptr<MouseCollision> collision;
	void _Draw() const override;
	void _Update(float dt) override;
	void _Load() override;
public:
	Signal<const MouseEvent&> LeftPressed, LeftReleased, MiddlePressed, MiddleReleased, RightPressed, RightReleased;
	Button() = default;
	~Button() = default;
	template<typename... Args>
	void SetArea(Args&&... args) {
		collision->ClearVertices();
		for (auto p : std::initializer_list<Vector2D>{(args)...}) {
			collision->AddVertex(p);
		}
	}
};