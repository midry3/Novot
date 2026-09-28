#pragma once
#include <Scene.h>

class PageController : public Scene {
protected:
	std::map<int, std::weak_ptr<Scene>> _pages;
	int _currentPageId = -1;
	std::weak_ptr<Scene> _currentPage;
public:
	PageController* AddPage(int pageId, std::shared_ptr<Scene> page) {
		page->Load();
		_pages[pageId] = page;
		AddScene(page);
		page->Disable();
		return this;
	}

	void Switch(int pageId) {
		if (auto current = _currentPage.lock()) {
			current->Disable();
		}
		_currentPageId = pageId;
		auto it = _pages.find(_currentPageId);
		if (it == _pages.end()) return;
		_currentPage = _pages[_currentPageId];
		if (auto newPage = _currentPage.lock()) {
			newPage->Enable();
		}
	}

	bool IsShowingPage() const {
		return not _currentPage.expired() && 0 <= _currentPageId;
	}

	void operator>>(int pageId) {
		Switch(pageId);
	}
};