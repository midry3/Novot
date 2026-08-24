#ifdef _DEBUG
#include "Debugger.h"
#include <NovotCore.h>
#include <SceneManager.h>
#include <PropertyManager.h>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <psapi.h>
#include <chrono>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Comctl32.lib")

namespace {
std::string ToLower(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
		return static_cast<char>(std::tolower(c));
	});
	return value;
}

std::string FormatMiB(SIZE_T bytes) {
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(2)
		<< (static_cast<double>(bytes) / (1024.0 * 1024.0));
	return oss.str();
}

std::string PtrToHex(void* p) {
	std::ostringstream oss;
	oss << "0x" << std::hex << reinterpret_cast<uintptr_t>(p);
	return oss.str();
}
}

LRESULT CALLBACK Debugger::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_NCCREATE) {
		auto create = reinterpret_cast<CREATESTRUCT*>(lParam);
		SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
	}

	auto debugger = reinterpret_cast<Debugger*>(GetWindowLongPtr(window, GWLP_USERDATA));
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == DebugModeCheckBoxId && HIWORD(wParam) == BN_CLICKED) {
		const bool enabled = PropertyManager.ToggleDebugMode();
		SendMessage(debugger->_DebugModeCheckBox, BM_SETCHECK, enabled ? BST_CHECKED : BST_UNCHECKED, 0);
		debugger->SetProperty("Debug Mode", enabled ? "Enabled" : "Disabled");
		return 0;
	}
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == ClearLogButtonId && HIWORD(wParam) == BN_CLICKED) {
		debugger->_LogLines.clear();
		debugger->_NeedsLogRefresh = true;
		return 0;
	}
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == FilterEditId && HIWORD(wParam) == EN_CHANGE) {
		char text[256] = {};
		GetWindowText(debugger->_FilterEdit, text, 255);
		debugger->_FilterText = text;
		debugger->_NeedsLogRefresh = true;
		debugger->_NeedsUpdate = true;
		return 0;
	}
	// Pause button
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == PauseButtonId && HIWORD(wParam) == BN_CLICKED) {
		if (GameState.IsPaused()) {
			GameState.Resume();
			SetWindowText(debugger->_PauseButton, "Pause");
			debugger->SetProperty("Paused", "False");
		}
		else {
			GameState.Pause();
			SetWindowText(debugger->_PauseButton, "Resume");
			debugger->SetProperty("Paused", "True");
		}
		return 0;
	}
	// Step frame button
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == StepFrameButtonId && HIWORD(wParam) == BN_CLICKED) {
		GameState.RequestStepFrame();
		debugger->SetProperty("StepRequest", "Requested");
		return 0;
	}
	// TimeScale combobox change
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == TimeScaleComboId && HIWORD(wParam) == CBN_SELCHANGE) {
		int index = static_cast<int>(SendMessage(debugger->_TimeScaleCombo, CB_GETCURSEL, 0, 0));
		float scale = 1.0f;
		switch (index) {
		case 0: scale = 0.1f; break;
		case 1: scale = 0.25f; break;
		case 2: scale = 0.5f; break;
		case 3: scale = 1.0f; break;
		case 4: scale = 2.0f; break;
		case 5: scale = 4.0f; break;
		}
		GameState.SetTimeScale(scale);
		debugger->SetProperty("TimeScale", std::to_string(scale));
		return 0;
	}
	// Refresh scene tree
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == RefreshTreeButtonId && HIWORD(wParam) == BN_CLICKED) {
		debugger->RefreshSceneTree();
		return 0;
	}
	// Stop BGM
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == StopBGMButtonId && HIWORD(wParam) == BN_CLICKED) {
		SoundManager.StopBGM();
		debugger->SetProperty("BGM", "Stopped");
		return 0;
	}
	// Mute All
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == MuteAllButtonId && HIWORD(wParam) == BN_CLICKED) {
		if (SoundManager.IsMuted()) {
			SoundManager.UnmuteAll();
			debugger->SetProperty("Audio.Muted", "False");
			SetWindowText(debugger->_MuteAllButton, "Mute All");
		} else {
			SoundManager.MuteAll();
			debugger->SetProperty("Audio.Muted", "True");
			SetWindowText(debugger->_MuteAllButton, "Unmute");
		}
		return 0;
	}
	// Sprite reload/release
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == ReloadSpriteButtonId && HIWORD(wParam) == BN_CLICKED) {
		int idx = static_cast<int>(SendMessage(debugger->_AssetList, LB_GETCURSEL, 0, 0));
		if (idx != LB_ERR) {
			char buf[256] = {};
			SendMessage(debugger->_AssetList, LB_GETTEXT, idx, reinterpret_cast<LPARAM>(buf));
		}
		return 0;
	}
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == ReleaseSpriteButtonId && HIWORD(wParam) == BN_CLICKED) {
		int idx = static_cast<int>(SendMessage(debugger->_AssetList, LB_GETCURSEL, 0, 0));
		if (idx != LB_ERR) {
			char buf[256] = {};
			SendMessage(debugger->_AssetList, LB_GETTEXT, idx, reinterpret_cast<LPARAM>(buf));
		}
		return 0;
	}
	// Property candidate selection change
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == PropNameEditId && HIWORD(wParam) == CBN_SELCHANGE) {
		int index = static_cast<int>(SendMessage(debugger->_PropNameCombo, CB_GETCURSEL, 0, 0));
		if (index != CB_ERR) {
			char nameBuf[128] = {};
			SendMessage(debugger->_PropNameCombo, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(nameBuf));
			std::string sName(nameBuf);
			std::string valStr = PropertyManager.GetPropertyValueString(sName);
			if (valStr.empty() && debugger->_Properties.count(sName)) {
				valStr = debugger->_Properties[sName];
			}
			SetWindowText(debugger->_PropValueEdit, valStr.c_str());
		}
		return 0;
	}
	// Apply property edit
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == SetPropButtonId && HIWORD(wParam) == BN_CLICKED) {
		char name[128] = {};
		char val[128] = {};
		GetWindowText(debugger->_PropNameCombo, name, 127);
		GetWindowText(debugger->_PropValueEdit, val, 127);
		std::string sName(name);
		std::string sVal(val);
		if (!sName.empty()) {
			if (PropertyManager.SetPropertyFromString(sName, sVal)) {
				debugger->WriteLine("[Property] Updated bound property '" + sName + "' = " + sVal);
			} else {
				debugger->SetProperty(sName, sVal);
				debugger->WriteLine("[Property] Set '" + sName + "' = " + sVal);
			}
			debugger->_NeedsUpdate = true;
		}
		return 0;
	}

	if (debugger && message == WM_CTLCOLORSTATIC) {
		HWND hCtrl = reinterpret_cast<HWND>(lParam);
		HDC hdc = reinterpret_cast<HDC>(wParam);
		if (hCtrl == debugger->_PropertyView || hCtrl == debugger->_LogEdit) {
			SetBkColor(hdc, RGB(255, 255, 255));
			SetTextColor(hdc, RGB(0, 0, 0));
			return reinterpret_cast<INT_PTR>(GetStockObject(WHITE_BRUSH));
		}
		if (hCtrl == debugger->_FilterStatic) {
			SetBkMode(hdc, TRANSPARENT);
			return reinterpret_cast<INT_PTR>(GetSysColorBrush(COLOR_BTNFACE));
		}
	}
	if (debugger && message == WM_SIZE) {
		debugger->AdjustUI();
		return 0;
	}
	if (debugger && message == WM_PAINT) {
		PAINTSTRUCT ps{};
		HDC hdc = BeginPaint(window, &ps);
		debugger->DrawFrameTimeGraph(hdc);
		debugger->DrawMemoryGraph(hdc);
		EndPaint(window, &ps);
		return 0;
	}
	if (message == WM_DESTROY) {
		if (debugger && debugger->_UIFont) {
			DeleteObject(debugger->_UIFont);
			debugger->_UIFont = nullptr;
		}
	}

	return DefWindowProc(window, message, wParam, lParam);
}

void Debugger::AdjustUI() {
	RECT rect;
	GetClientRect(_Window, &rect);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	const int margin = 10;
	const int controlHeight = 24;

	// Calculate 3-column layout geometry
	const int totalAvailWidth = (std::max)(100, width - (margin * 4));
	const int col1Width = (std::max)(280, totalAvailWidth * 34 / 100);
	const int col2Width = (std::max)(280, totalAvailWidth * 33 / 100);
	const int col1Left = margin;
	const int col2Left = col1Left + col1Width + margin;
	const int col3Left = col2Left + col2Width + margin;
	const int col3Width = (std::max)(280, width - col3Left - margin);

	// --- Column 1: Log & Property Inspector ---
	const int filterTop = margin;
	const int filterLabelW = 45;
	const int clearBtnW = 85;
	const int filterFieldW = (std::max)(60, col1Width - filterLabelW - clearBtnW - 12);

	MoveWindow(_FilterStatic, col1Left, filterTop + 2, filterLabelW, controlHeight, TRUE);
	MoveWindow(_FilterEdit, col1Left + filterLabelW + 4, filterTop, filterFieldW, controlHeight, TRUE);
	MoveWindow(_ClearLogButton, col1Left + col1Width - clearBtnW, filterTop, clearBtnW, controlHeight, TRUE);

	const int propEditH = 24;
	const int propViewH = 130;
	const int propTop = height - margin - propViewH - propEditH - 6;
	const int logTop = filterTop + controlHeight + margin;
	const int logH = (std::max)(100, propTop - logTop - margin);

	MoveWindow(_LogEdit, col1Left, logTop, col1Width, logH, TRUE);
	MoveWindow(_PropertyView, col1Left, propTop, col1Width, propViewH, TRUE);

	const int setBtnW = 60;
	const int propInputW = (col1Width - setBtnW - 8) / 2;
	MoveWindow(_PropNameCombo, col1Left, propTop + propViewH + 4, propInputW, 200, TRUE);
	MoveWindow(_PropValueEdit, col1Left + propInputW + 4, propTop + propViewH + 4, propInputW, propEditH, TRUE);
	MoveWindow(_SetPropButton, col1Left + (propInputW + 4) * 2, propTop + propViewH + 4, setBtnW, propEditH, TRUE);

	// --- Column 2: Controls, Scene Tree, Audio & Assets ---
	const int col2Top = margin;
	const int timeBtnW = (col2Width - 8) / 3;
	MoveWindow(_PauseButton, col2Left, col2Top, timeBtnW, controlHeight, TRUE);
	MoveWindow(_StepFrameButton, col2Left + timeBtnW + 4, col2Top, timeBtnW, controlHeight, TRUE);
	MoveWindow(_TimeScaleCombo, col2Left + (timeBtnW + 4) * 2, col2Top, timeBtnW, 150, TRUE);

	const int treeHeaderTop = col2Top + controlHeight + 8;
	const int debugChkW = 100;
	const int refreshBtnW = 100;
	MoveWindow(_DebugModeCheckBox, col2Left, treeHeaderTop, debugChkW, controlHeight, TRUE);
	MoveWindow(_RefreshTreeButton, col2Left + col2Width - refreshBtnW, treeHeaderTop, refreshBtnW, controlHeight, TRUE);

	const int audioPanelH = 150;
	const int treeTop = treeHeaderTop + controlHeight + 4;
	const int treeH = (std::max)(100, height - margin * 3 - treeTop - audioPanelH);
	MoveWindow(_SceneTreeView, col2Left, treeTop, col2Width, treeH, TRUE);

	const int audioTop = treeTop + treeH + margin;
	const int halfAudioW = (col2Width - 4) / 2;
	MoveWindow(_StopBGMButton, col2Left, audioTop, halfAudioW, controlHeight, TRUE);
	MoveWindow(_MuteAllButton, col2Left + halfAudioW + 4, audioTop, halfAudioW, controlHeight, TRUE);

	const int spriteListTop = audioTop + controlHeight + 4;
	const int spriteListH = (std::max)(40, height - margin - spriteListTop - controlHeight - 6);
	MoveWindow(_AssetList, col2Left, spriteListTop, col2Width, spriteListH, TRUE);

	const int spriteBtnTop = spriteListTop + spriteListH + 4;
	MoveWindow(_ReloadSpriteButton, col2Left, spriteBtnTop, halfAudioW, controlHeight, TRUE);
	MoveWindow(_ReleaseSpriteButton, col2Left + halfAudioW + 4, spriteBtnTop, halfAudioW, controlHeight, TRUE);

	// --- Column 3: Performance Graphs ---
	const int graphGap = 10;
	const int graphH = (height - margin * 3 - graphGap) / 2;
	_FrameTimeGraphRect = { col3Left, margin, col3Left + col3Width, margin + graphH };
	_MemoryGraphRect = { col3Left, margin + graphH + graphGap, col3Left + col3Width, height - margin };

	InvalidateRect(_Window, nullptr, TRUE);
}

void Debugger::PopulateTreeItem(HTREEITEM parent, std::shared_ptr<Scene> scene, HWND tree) {
	if (!scene) return;
	TVINSERTSTRUCT tvis{};
	tvis.hParent = parent;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
	std::string text = std::string(typeid(*scene).name()) + " " + PtrToHex(scene.get());
	tvis.item.pszText = const_cast<char*>(text.c_str());
	tvis.item.lParam = reinterpret_cast<LPARAM>(scene.get());
	HTREEITEM item = reinterpret_cast<HTREEITEM>(SendMessage(tree, TVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&tvis)));
	for (const auto& child : scene->GetChildScenes()) {
		PopulateTreeItem(item, child, tree);
	}
}

void Debugger::RefreshSceneTree() {

	if (!_SceneTreeView) return;
	SendMessage(_SceneTreeView, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(TVI_ROOT));
	auto root = SceneManager.GetCurrentScene();
	if (root) {
		PopulateTreeItem(nullptr, root, _SceneTreeView);
	}
	_NeedsUpdate = true;

}

void Debugger::RefreshAssetsView() {

	if (!_AssetList) return;
	std::string ref;
	for (const auto& [path, asset] : AssetLoader.GetAssetHandles()) {
		ref = std::to_string(asset.references.size());
		auto it = std::find(_AssetNames.begin(), _AssetNames.end(), path);
		if (it == _AssetNames.end()) {
			LVITEM item{};
			item.mask = LVIF_TEXT;
			item.iItem = _AssetNames.size();
			item.iSubItem = 0;
			item.pszText = const_cast<LPSTR>(path.c_str());
			ListView_InsertItem(_AssetList, &item);
			ListView_SetItemText(
				_AssetList,
				_AssetNames.size(),
				1,
				const_cast<LPSTR>(ref.c_str())
			);
			_AssetNames.push_back(path);
			AssetLoader.RegisterAssetReleasedCallback(path, [this, path]() {
				auto it = std::find(_AssetNames.begin(), _AssetNames.end(), path);
				if (it != _AssetNames.end()) {
					int index = std::distance(_AssetNames.begin(), it);
					ListView_DeleteItem(_AssetList, index);
					_AssetNames.erase(it);
					_NeedsUpdate = true;
				}
				});
		}
		else {
			ListView_SetItemText(
				_AssetList,
				std::distance(_AssetNames.begin(), it),
				1,
				const_cast<LPSTR>(ref.c_str())
			);
		}
	}
	_NeedsUpdate = true;

}

void Debugger::RefreshSoundState() {

	auto bgm = SoundManager.GetBGM();
	if (bgm) {
		SetProperty("BGM", "Loaded");
	}
	else {
		SetProperty("BGM", "None");
	}
	SetProperty("Audio.Muted", SoundManager.IsMuted() ? "True" : "False");
	_NeedsUpdate = true;

}

Debugger::Debugger() {

	InitCommonControls();
	const char* windowClassName = "NovotDebuggerWindow";
	WNDCLASS windowClass{};
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszClassName = windowClassName;
	RegisterClass(&windowClass);

	_Window = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		windowClassName,
		"Novot Debugger",
		WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		100, 100,
		1500, 1000,
		nullptr,
		nullptr,
		GetModuleHandle(nullptr),
		this
	);

	if (_Window) {
		_LogEdit = CreateWindowEx(
			0, "EDIT", "",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
			12, 76, 776, 280,
			_Window, nullptr, GetModuleHandle(nullptr), nullptr
		);

		_PropertyView = CreateWindowEx(
			0, "EDIT", "",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
			12, 368, 776, 170,
			_Window, nullptr, GetModuleHandle(nullptr), nullptr
		);
		_FilterStatic = CreateWindowEx(
			0, "STATIC", "Filter:",
			WS_CHILD | WS_VISIBLE,
			12, 44, 50, 24,
			_Window, nullptr, GetModuleHandle(nullptr), nullptr
		);
		_FilterEdit = CreateWindowEx(
			WS_EX_CLIENTEDGE, "EDIT", "",
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
			70, 42, 590, 26,
			_Window, reinterpret_cast<HMENU>(FilterEditId), GetModuleHandle(nullptr), nullptr
		);
		_ClearLogButton = CreateWindowEx(
			0, "BUTTON", "Clear Logs",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			668, 42, 120, 26,
			_Window, reinterpret_cast<HMENU>(ClearLogButtonId), GetModuleHandle(nullptr), nullptr
		);

		_DebugModeCheckBox = CreateWindowEx(
			0, "BUTTON", "Debug Mode",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			668, 44, 120, 26,
			_Window, reinterpret_cast<HMENU>(DebugModeCheckBoxId), GetModuleHandle(nullptr), nullptr
		);

		// New controls
		_PauseButton = CreateWindowEx(0, "BUTTON", "Pause", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 420, 12, 120, 26, _Window, reinterpret_cast<HMENU>(PauseButtonId), GetModuleHandle(nullptr), nullptr);
		_StepFrameButton = CreateWindowEx(0, "BUTTON", "Step Frame", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 548, 12, 120, 26, _Window, reinterpret_cast<HMENU>(StepFrameButtonId), GetModuleHandle(nullptr), nullptr);
		_TimeScaleCombo = CreateWindowEx(WS_EX_CLIENTEDGE, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 676, 12, 140, 200, _Window, reinterpret_cast<HMENU>(TimeScaleComboId), GetModuleHandle(nullptr), nullptr);
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("0.1x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("0.25x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("0.5x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("1.0x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("2.0x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("4.0x"));
		SendMessage(_TimeScaleCombo, CB_SETCURSEL, 3, 0); // default 1.0x

		_RefreshTreeButton = CreateWindowEx(0, "BUTTON", "Refresh Tree", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 760, 12, 120, 26, _Window, reinterpret_cast<HMENU>(RefreshTreeButtonId), GetModuleHandle(nullptr), nullptr);
		_SceneTreeView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEWA, "", WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES, 760, 48, 220, 200, _Window, reinterpret_cast<HMENU>(SceneTreeViewId), GetModuleHandle(nullptr), nullptr);

		// Sound & Asset controls
		_StopBGMButton = CreateWindowEx(0, "BUTTON", "Stop BGM", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 760, 260, 100, 26, _Window, reinterpret_cast<HMENU>(StopBGMButtonId), GetModuleHandle(nullptr), nullptr);
		_MuteAllButton = CreateWindowEx(0, "BUTTON", "Mute All", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 868, 260, 100, 26, _Window, reinterpret_cast<HMENU>(MuteAllButtonId), GetModuleHandle(nullptr), nullptr);

		_AssetList = CreateWindowEx(0, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL, 760, 296, 220, 140, _Window, reinterpret_cast<HMENU>(AssetListId), GetModuleHandle(nullptr), nullptr);
		LVCOLUMN column{};
		column.mask = LVCF_TEXT | LVCF_WIDTH;
		column.pszText = (LPSTR)"file";
		column.cx = 200;
		ListView_InsertColumn(_AssetList, 0, &column);
		column.pszText = (LPSTR)"ref";
		column.cx = 50;
		ListView_InsertColumn(_AssetList, 1, &column);

		_ReloadSpriteButton = CreateWindowEx(0, "BUTTON", "Reload", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 760, 444, 100, 26, _Window, reinterpret_cast<HMENU>(ReloadSpriteButtonId), GetModuleHandle(nullptr), nullptr);
		_ReleaseSpriteButton = CreateWindowEx(0, "BUTTON", "Release", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 868, 444, 100, 26, _Window, reinterpret_cast<HMENU>(ReleaseSpriteButtonId), GetModuleHandle(nullptr), nullptr);

		// Property editing controls
		_PropNameCombo = CreateWindowEx(WS_EX_CLIENTEDGE, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_VSCROLL | CBS_AUTOHSCROLL, 12, 540, 140, 200, _Window, reinterpret_cast<HMENU>(PropNameEditId), GetModuleHandle(nullptr), nullptr);
		_PropValueEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 160, 540, 140, 26, _Window, reinterpret_cast<HMENU>(PropValueEditId), GetModuleHandle(nullptr), nullptr);
		_SetPropButton = CreateWindowEx(0, "BUTTON", "Apply", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 308, 540, 70, 26, _Window, reinterpret_cast<HMENU>(SetPropButtonId), GetModuleHandle(nullptr), nullptr);

		_UIFont = CreateFont(
			-16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, "Consolas"
		);
		if (_UIFont) {
			SendMessage(_LogEdit, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_PropertyView, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_FilterStatic, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_FilterEdit, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_ClearLogButton, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_DebugModeCheckBox, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_PauseButton, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_StepFrameButton, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_TimeScaleCombo, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_RefreshTreeButton, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_SceneTreeView, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_PropNameCombo, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_PropValueEdit, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
			SendMessage(_SetPropButton, WM_SETFONT, reinterpret_cast<WPARAM>(_UIFont), TRUE);
		}

		SendMessage(_DebugModeCheckBox, BM_SETCHECK,
			PropertyManager.IsDebugMode() ? BST_CHECKED : BST_UNCHECKED, 0);
		SetProperty("Debug Mode", PropertyManager.IsDebugMode() ? "Enabled" : "Disabled");
		_LastMemorySampleTick = GetTickCount();
		UpdateMemoryStats();

		SendMessage(_LogEdit, EM_SETLIMITTEXT, 0, 0);
		SendMessage(_PropertyView, EM_SETLIMITTEXT, 0, 0);
		SetWindowText(_LogEdit, "--- Runtime Log ---\r\n");
		SetWindowText(_PropertyView, "--- Property Inspector ---\r\n");
		SendMessage(_Window, WM_SIZE, 0, MAKELPARAM(1000, 800));
		_NeedsUpdate = true;
		_NeedsLogRefresh = true;
		RefreshAssetsView();
		RefreshSoundState();
		AdjustUI();
		Update();
	}

}

void Debugger::WriteLine(std::string msg) {

	if (!_LogEdit) return;
	_LogLines.push_back(msg);
	if (_LogLines.size() > 2000) {
		_LogLines.erase(_LogLines.begin(), _LogLines.begin() + 500);
	}
	_NeedsLogRefresh = true;

}

void Debugger::SetProperty(std::string key, std::string value) {

	if (_Properties.find(key) == _Properties.end() || _Properties[key] != value) {
		_Properties[key] = value;
		_NeedsUpdate = true;
	}

}

void Debugger::Update() {

	if (!_PropertyView) return;
	UpdateMemoryStats();
	if (_NeedsLogRefresh) {
		RefreshLogView();
	}
	if (!_NeedsUpdate) return;

	RefreshPropertyView();
	RefreshAssetsView();
	_NeedsUpdate = false;

}

void Debugger::RefreshLogView() {

	std::stringstream ss;
	ss << "--- Runtime Log ---\r\n";
	const std::string filterLower = ToLower(_FilterText);
	for (const auto& line : _LogLines) {
		if (!filterLower.empty() && ToLower(line).find(filterLower) == std::string::npos) {
			continue;
		}
		ss << line << "\r\n";
	}
	SetWindowText(_LogEdit, ss.str().c_str());
	SendMessage(_LogEdit, EM_SETSEL, GetWindowTextLength(_LogEdit), GetWindowTextLength(_LogEdit));
	SendMessage(_LogEdit, EM_SCROLLCARET, 0, 0);
	_NeedsLogRefresh = false;

}

void Debugger::PopulatePropertyCandidates() {

	if (!_PropNameCombo) return;

	std::vector<std::string> candidates;
	for (const auto& entry : PropertyManager.GetBoundProperties()) {
		if (std::find(candidates.begin(), candidates.end(), entry.name) == candidates.end()) {
			candidates.push_back(entry.name);
		}
	}
	std::sort(candidates.begin(), candidates.end());

	if (candidates == _LastCandidates) return;
	_LastCandidates = candidates;

	char curText[128] = {};
	GetWindowText(_PropNameCombo, curText, 127);
	std::string currentInput(curText);

	SendMessage(_PropNameCombo, CB_RESETCONTENT, 0, 0);
	for (const auto& name : candidates) {
		SendMessage(_PropNameCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(name.c_str()));
	}

	if (!currentInput.empty()) {
		SetWindowText(_PropNameCombo, currentInput.c_str());
	}

}

void Debugger::RefreshPropertyView() {

	if (!_PropertyView) return;

	PopulatePropertyCandidates();

	// Save scroll position and selection before updating text
	const int firstVisible = static_cast<int>(SendMessage(_PropertyView, EM_GETFIRSTVISIBLELINE, 0, 0));
	DWORD selStart = 0, selEnd = 0;
	SendMessage(_PropertyView, EM_GETSEL, reinterpret_cast<WPARAM>(&selStart), reinterpret_cast<LPARAM>(&selEnd));

	std::stringstream ss;
	ss << "--- Property Inspector ---\r\n";
	for (const auto& [key, value] : _Properties) {
		ss << key << ": " << value << "\r\n";
	}

	const auto boundProps = PropertyManager.GetBoundProperties();
	if (!boundProps.empty()) {
		ss << "\r\n-- Bound Properties (Editable) --\r\n";
		for (const auto& entry : boundProps) {
			std::string valStr = PropertyManager.GetPropertyValueString(entry.name);
			std::string typeStr;
			switch (entry.type) {
			case _PropertyManager::PropertyType::Int: typeStr = "int"; break;
			case _PropertyManager::PropertyType::Float: typeStr = "float"; break;
			case _PropertyManager::PropertyType::Bool: typeStr = "bool"; break;
			case _PropertyManager::PropertyType::String: typeStr = "string"; break;
			}
			ss << entry.name << " (" << typeStr << "): " << valStr << "\r\n";
		}
	}

	SetWindowText(_PropertyView, ss.str().c_str());

	// Restore selection and vertical scroll position
	SendMessage(_PropertyView, EM_SETSEL, selStart, selEnd);
	const int currentFirst = static_cast<int>(SendMessage(_PropertyView, EM_GETFIRSTVISIBLELINE, 0, 0));
	const int deltaLines = firstVisible - currentFirst;
	if (deltaLines != 0) {
		SendMessage(_PropertyView, EM_LINESCROLL, 0, deltaLines);
	}

}

void Debugger::UpdateMemoryStats() {

	const DWORD now = GetTickCount();
	if (now - _LastMemorySampleTick < MemorySampleIntervalMs) {
		return;
	}
	_LastMemorySampleTick = now;

	PROCESS_MEMORY_COUNTERS_EX memInfo{};
	memInfo.cb = sizeof(memInfo);
	if (!GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memInfo), sizeof(memInfo))) {
		return;
	}
	const double workingSetMiB = static_cast<double>(memInfo.WorkingSetSize) / (1024.0 * 1024.0);
	const double privateUsageMiB = static_cast<double>(memInfo.PrivateUsage) / (1024.0 * 1024.0);
	_WorkingSetHistoryMiB.push_back(workingSetMiB);
	_PrivateUsageHistoryMiB.push_back(privateUsageMiB);
	if (_WorkingSetHistoryMiB.size() > MemoryHistoryLimit) {
		_WorkingSetHistoryMiB.erase(_WorkingSetHistoryMiB.begin());
		_PrivateUsageHistoryMiB.erase(_PrivateUsageHistoryMiB.begin());
	}
	InvalidateRect(_Window, &_MemoryGraphRect, TRUE);

	SetProperty("Memory.WorkingSet(MiB)", FormatMiB(memInfo.WorkingSetSize));
	SetProperty("Memory.PrivateUsage(MiB)", FormatMiB(memInfo.PrivateUsage));
	SetProperty("Memory.PeakWorkingSet(MiB)", FormatMiB(memInfo.PeakWorkingSetSize));
	SetProperty("Memory.PagefileUsage(MiB)", FormatMiB(memInfo.PagefileUsage));

}

void Debugger::DrawMemoryGraph(HDC hdc) const {

	if (_MemoryGraphRect.right <= _MemoryGraphRect.left || _MemoryGraphRect.bottom <= _MemoryGraphRect.top) {
		return;
	}

	HBRUSH panelBrush = CreateSolidBrush(RGB(247, 247, 247));
	FillRect(hdc, &_MemoryGraphRect, panelBrush);
	DeleteObject(panelBrush);
	FrameRect(hdc, &_MemoryGraphRect, static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH)));

	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, _MemoryGraphRect.left + 10, _MemoryGraphRect.top + 8, "Memory Graph (MiB)", 18);

	RECT plotRect = _MemoryGraphRect;
	plotRect.left += 10;
	plotRect.right -= 10;
	plotRect.top += 30;
	plotRect.bottom -= 30;
	if (plotRect.right - plotRect.left < 40 || plotRect.bottom - plotRect.top < 40) {
		return;
	}

	FrameRect(hdc, &plotRect, static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH)));
	HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
	HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, gridPen));
	for (int i = 1; i < 5; ++i) {
		const int y = plotRect.top + ((plotRect.bottom - plotRect.top) * i / 5);
		MoveToEx(hdc, plotRect.left, y, nullptr);
		LineTo(hdc, plotRect.right, y);
	}
	SelectObject(hdc, oldPen);
	DeleteObject(gridPen);

	const size_t count = _WorkingSetHistoryMiB.size();
	if (count < 2) {
		TextOut(hdc, plotRect.left + 8, plotRect.top + 8, "Collecting memory samples...", 28);
		return;
	}

	double maxValue = 1.0;
	for (size_t i = 0; i < count; ++i) {
		maxValue = (std::max)(maxValue, _WorkingSetHistoryMiB[i]);
		maxValue = (std::max)(maxValue, _PrivateUsageHistoryMiB[i]);
	}

	auto drawSeries = [&](const std::vector<double>& series, COLORREF color) {
		HPEN pen = CreatePen(PS_SOLID, 2, color);
		HPEN prev = static_cast<HPEN>(SelectObject(hdc, pen));
		for (size_t i = 0; i < series.size(); ++i) {
			const double xRatio = static_cast<double>(i) / static_cast<double>(series.size() - 1);
			const double yRatio = series[i] / maxValue;
			const int x = plotRect.left + static_cast<int>(xRatio * (plotRect.right - plotRect.left));
			const int y = plotRect.bottom - static_cast<int>(yRatio * (plotRect.bottom - plotRect.top));
			if (i == 0) {
				MoveToEx(hdc, x, y, nullptr);
			}
			else {
				LineTo(hdc, x, y);
			}
		}
		SelectObject(hdc, prev);
		DeleteObject(pen);
	};

	drawSeries(_WorkingSetHistoryMiB, RGB(0, 120, 215));
	drawSeries(_PrivateUsageHistoryMiB, RGB(232, 95, 0));

	const std::string maxText = "Max: " + FormatMiB(static_cast<SIZE_T>(maxValue * 1024.0 * 1024.0)) + " MiB";
	TextOut(hdc, plotRect.left, plotRect.bottom + 8, maxText.c_str(), static_cast<int>(maxText.size()));

	const std::string workingText = "WorkingSet: " + FormatMiB(static_cast<SIZE_T>(_WorkingSetHistoryMiB.back() * 1024.0 * 1024.0)) + " MiB";
	const std::string privateText = "Private: " + FormatMiB(static_cast<SIZE_T>(_PrivateUsageHistoryMiB.back() * 1024.0 * 1024.0)) + " MiB";
	TextOut(hdc, plotRect.left + 4, plotRect.top + 4, workingText.c_str(), static_cast<int>(workingText.size()));
	TextOut(hdc, plotRect.left + 4, plotRect.top + 22, privateText.c_str(), static_cast<int>(privateText.size()));

}

// Frame time recording
void Debugger::RecordFrameTime(double updateMs, double drawMs) {

	_UpdateTimeMsHistory.push_back(updateMs);
	_DrawTimeMsHistory.push_back(drawMs);
	if (_UpdateTimeMsHistory.size() > FrameTimeHistoryLimit) {
		_UpdateTimeMsHistory.erase(_UpdateTimeMsHistory.begin());
		_DrawTimeMsHistory.erase(_DrawTimeMsHistory.begin());
	}
	InvalidateRect(_Window, &_FrameTimeGraphRect, TRUE);
	SetProperty("Frame.UpdateMs", std::to_string(updateMs));
	SetProperty("Frame.DrawMs", std::to_string(drawMs));
	SetProperty("Frame.TotalMs", std::to_string(updateMs + drawMs));

}

void Debugger::DrawFrameTimeGraph(HDC hdc) const {

	if (_FrameTimeGraphRect.right <= _FrameTimeGraphRect.left || _FrameTimeGraphRect.bottom <= _FrameTimeGraphRect.top) {
		return;
	}

	HBRUSH panelBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hdc, &_FrameTimeGraphRect, panelBrush);
	DeleteObject(panelBrush);
	FrameRect(hdc, &_FrameTimeGraphRect, static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH)));

	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, _FrameTimeGraphRect.left + 10, _FrameTimeGraphRect.top + 8, "Frame Time (ms)", 14);

	RECT plotRect = _FrameTimeGraphRect;
	plotRect.left += 10;
	plotRect.right -= 10;
	plotRect.top += 30;
	plotRect.bottom -= 10;
	if (plotRect.right - plotRect.left < 40 || plotRect.bottom - plotRect.top < 40) return;

	size_t count = _UpdateTimeMsHistory.size();
	if (count < 2) {
		TextOut(hdc, plotRect.left + 8, plotRect.top + 8, "Collecting frame times...", 24);
		return;
	}

	double maxValue = 16.67; // at least show 60FPS line
	for (size_t i = 0; i < count; ++i) {
		maxValue = (std::max)(maxValue, _UpdateTimeMsHistory[i]);
		maxValue = (std::max)(maxValue, _DrawTimeMsHistory[i]);
	}

	auto drawSeries = [&](const std::vector<double>& series, COLORREF color) {
		HPEN pen = CreatePen(PS_SOLID, 2, color);
		HPEN prev = static_cast<HPEN>(SelectObject(hdc, pen));
		for (size_t i = 0; i < series.size(); ++i) {
			const double xRatio = static_cast<double>(i) / static_cast<double>(series.size() - 1);
			const double yRatio = series[i] / maxValue;
			const int x = plotRect.left + static_cast<int>(xRatio * (plotRect.right - plotRect.left));
			const int y = plotRect.bottom - static_cast<int>(yRatio * (plotRect.bottom - plotRect.top));
			if (i == 0) MoveToEx(hdc, x, y, nullptr); else LineTo(hdc, x, y);
		}
		SelectObject(hdc, prev);
		DeleteObject(pen);
	};

	drawSeries(_UpdateTimeMsHistory, RGB(46, 204, 113)); // green
	drawSeries(_DrawTimeMsHistory, RGB(155, 89, 182)); // purple

	// 60FPS line
	double y60 = 16.67 / maxValue;
	int yline = plotRect.bottom - static_cast<int>(y60 * (plotRect.bottom - plotRect.top));
	HPEN dash = CreatePen(PS_DOT, 1, RGB(200, 0, 0));
	HPEN old = static_cast<HPEN>(SelectObject(hdc, dash));
	MoveToEx(hdc, plotRect.left, yline, nullptr);
	LineTo(hdc, plotRect.right, yline);
	SelectObject(hdc, old);
	DeleteObject(dash);

	// spike markers
	for (size_t i = 0; i < count; ++i) {
		double total = _UpdateTimeMsHistory[i] + _DrawTimeMsHistory[i];
		if (total > 16.67) {
			const double xRatio = static_cast<double>(i) / static_cast<double>(count - 1);
			const int x = plotRect.left + static_cast<int>(xRatio * (plotRect.right - plotRect.left));
			const int y = plotRect.top + 4;
			HBRUSH b = CreateSolidBrush(RGB(255, 0, 0));
			RECT r = { x - 3, y, x + 3, y + 6 };
			FillRect(hdc, &r, b);
			DeleteObject(b);
		}
	}

}

// Property binding helpers (Delegating to PropertyManager)
void Debugger::BindProperty(const std::string& key, int* target) {

	PropertyManager.BindProperty(key, target);
	_NeedsUpdate = true;

}
void Debugger::BindProperty(const std::string& key, float* target) {

	PropertyManager.BindProperty(key, target);
	_NeedsUpdate = true;

}
void Debugger::BindProperty(const std::string& key, bool* target) {

	PropertyManager.BindProperty(key, target);
	_NeedsUpdate = true;

}
void Debugger::BindProperty(const std::string& key, std::string* target) {

	PropertyManager.BindProperty(key, target);
	_NeedsUpdate = true;

}
void Debugger::UnbindProperty(const std::string& key) {

	PropertyManager.UnbindProperty(key);
	_NeedsUpdate = true;

}
void Debugger::ClearBindings() {

	PropertyManager.ClearBoundProperties();
	_NeedsUpdate = true;

}

Debugger::~Debugger() {

	if (_UIFont) {
		DeleteObject(_UIFont);
		_UIFont = nullptr;
	}

}
#endif