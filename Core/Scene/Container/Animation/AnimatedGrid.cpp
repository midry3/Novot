#include "AnimatedGrid.h"
#include <Animation/Tween.h>

void  AnimatedGrid::_Update(float dt) {
	if (_direction == Horizontal) {
		_matrixSize.x += _bookedCount;
	}
	else {
		_matrixSize.y += _bookedCount;
	}
	GridContainer::_Update(dt);
	if (_direction == Horizontal) {
		_matrixSize.x -= _bookedCount;
	}
	else {
		_matrixSize.y -= _bookedCount;
	}
}

void AnimatedGrid::AnimateReflow() {
	for (size_t i = 0; i < _gridScenes.size(); i++) {
		if (auto scene = _gridScenes[i].lock()) {
			auto tweenGroup = scene->CreateTweenGroup();
			if (_direction == Horizontal) {
				tweenGroup->CreateTween(&scene->offset.x, scene->offset.x, _animationTime, _easing)
					->From(scene->position.x - GetPositionAt(i).x);
			}
			else {
				tweenGroup->CreateTween(&scene->offset.y, scene->offset.y, _animationTime, _easing)
					->From(scene->position.y - GetPositionAt(i).y);
			}
		}
	}
}

void AnimatedGrid::DeleteScene(int index) {
	if (index < 0 || index >= static_cast<int>(_scenes.size())) {
		return;
	}
	GridContainer::DeleteScene(index);
	UpdateMatrixSize();
	UpdateCellSize();
	AnimateReflow();
}

void AnimatedGrid::DeleteScene(std::shared_ptr<Scene> scene) {
	const bool hadScene = std::find(_scenes.begin(), _scenes.end(), scene) != _scenes.end();
	GridContainer::DeleteScene(scene);
	if (hadScene) {
		UpdateMatrixSize();
		UpdateCellSize();
		AnimateReflow();
	}
}

void AnimatedGrid::Book(int num) {
	_bookedCount += num;
	UpdateCellSize();
	if (not _gridScenes.empty()) {
		std::shared_ptr<TweenGroup> tg;
		for (size_t i = 0; i < _gridScenes.size(); i++) {
			if (auto scene = _gridScenes[i].lock()) {
				tg = scene->CreateTweenGroup();
				if (_direction == Horizontal) {
					tg->CreateTween(&scene->offset.x, scene->offset.x, _animationTime, _easing)
						->From(scene->position.x - GetPositionAt(i).x);
				} else {
					tg->CreateTween(&scene->offset.y, scene->offset.y, _animationTime, _easing)
						->From(scene->position.y - GetPositionAt(i).y);
				}
			}
		}
	}
}