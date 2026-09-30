#pragma once
#include <Collision/MouseCollision.h>
#include <Scene.h>
#include <vector>

class Button : public Scene {
protected:
	std::shared_ptr<MouseCollision> collision;
	std::vector<Vector2D> areaVertices;
	void _Load() override;
public:
	Signal<const MouseEvent&> LeftPressed, LeftReleased, MiddlePressed, MiddleReleased, RightPressed, RightReleased;
	Button() = default;
	~Button() = default;
	template<typename... Args>
	Button* SetArea(Args&&... args) {
		areaVertices.clear();
		for (auto p : std::initializer_list<Vector2D>{(args)...}) {
			areaVertices.emplace_back(p);
		}
		if (collision) {
			collision->ClearVertices();
			for (const auto& vertex : areaVertices) {
				auto areaVertex = vertex;
				collision->AddVertices(areaVertex);
			}
		}
		return this;
	}
	[[nodiscard]] MouseCollision& GetMouseCollision() const {
		return *collision;
	}
	Button* SetPassThrough(bool passThrough) {
		if (collision) {
			collision->SetStop(not passThrough);
		}
		return this;
	}
};