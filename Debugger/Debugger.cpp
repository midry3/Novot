#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
#include "Debugger.h"
#include <NovotCore.h>
#include <SceneManager.h>
#include <PropertyManager.h>
#include <Asset/AssetManager.h>
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

ULONGLONG FileTimeToULL(const FILETIME& ft) {
	ULARGE_INTEGER uli;
	uli.LowPart = ft.dwLowDateTime;
	uli.HighPart = ft.dwHighDateTime;
	return uli.QuadPart;
}

SceneNodeSnapshot BuildSceneSnapshot(std::shared_ptr<Scene> scene) {
	SceneNodeSnapshot snapshot;
	if (!scene) return snapshot;
	snapshot.name = std::string(typeid(*scene).name()) + " " + PtrToHex(scene.get());
	snapshot.ptr = scene.get();
	for (const auto& child : scene->GetChildScenes()) {
		snapshot.children.push_back(BuildSceneSnapshot(child));
	}
	return snapshot;
}
}

LRESULT CALLBACK Debugger::WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_NCCREATE) {
		auto create = reinterpret_cast<CREATESTRUCT*>(lParam);
		SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
	}

	auto debugger = reinterpret_cast<Debugger*>(GetWindowLongPtr(window, GWLP_USERDATA));

	if (message == WM_USER_PROCESS_QUEUE && debugger) {
		debugger->ProcessEvents();
		return 0;
	}

	if (debugger && message == WM_COMMAND && LOWORD(wParam) == DebugModeCheckBoxId && HIWORD(wParam) == BN_CLICKED) {
		debugger->PushCommand({ DebuggerCommandType::ToggleDebugMode });
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
			debugger->PushCommand({ DebuggerCommandType::Resume });
		}
		else {
			debugger->PushCommand({ DebuggerCommandType::Pause });
		}
		return 0;
	}
	// Step frame button
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == StepFrameButtonId && HIWORD(wParam) == BN_CLICKED) {
		debugger->PushCommand({ DebuggerCommandType::StepFrame });
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
		debugger->PushCommand({ DebuggerCommandType::SetTimeScale, scale });
		return 0;
	}
	// Refresh scene tree
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == RefreshTreeButtonId && HIWORD(wParam) == BN_CLICKED) {
		debugger->PushCommand({ DebuggerCommandType::RequestSceneTree });
		return 0;
	}
	// Stop BGM
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == StopBGMButtonId && HIWORD(wParam) == BN_CLICKED) {
		debugger->PushCommand({ DebuggerCommandType::StopBGM });
		return 0;
	}
	// Mute All
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == MuteAllButtonId && HIWORD(wParam) == BN_CLICKED) {
		debugger->PushCommand({ DebuggerCommandType::MuteAll });
		return 0;
	}
	// Property candidate selection change
	if (debugger && message == WM_COMMAND && LOWORD(wParam) == PropNameEditId && HIWORD(wParam) == CBN_SELCHANGE) {
		int index = static_cast<int>(SendMessage(debugger->_PropNameCombo, CB_GETCURSEL, 0, 0));
		if (index != CB_ERR) {
			char nameBuf[128] = {};
			SendMessage(debugger->_PropNameCombo, CB_GETLBTEXT, index, reinterpret_cast<LPARAM>(nameBuf));
			std::string sName(nameBuf);
			std::string valStr = debugger->_Properties[sName];
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
			debugger->PushCommand({ DebuggerCommandType::SetPropertyFromString, 0.0f, sName, sVal });
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
		debugger->DrawCpuGraph(hdc);
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
	if (!_Window) return;
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
	const int debugChkW = 135;
	const int refreshBtnW = 110;
	MoveWindow(_DebugModeCheckBox, col2Left, treeHeaderTop, debugChkW, controlHeight, TRUE);
	MoveWindow(_RefreshTreeButton, col2Left + col2Width - refreshBtnW, treeHeaderTop, refreshBtnW, controlHeight, TRUE);

	const int audioPanelH = 350;
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

	const int bottomGraphW = (col3Width - graphGap) / 2;
	const int bottomGraphTop = margin + graphH + graphGap;
	_MemoryGraphRect = { col3Left, bottomGraphTop, col3Left + bottomGraphW, height - margin };
	_CpuGraphRect = { col3Left + bottomGraphW + graphGap, bottomGraphTop, col3Left + col3Width, height - margin };

	InvalidateRect(_Window, nullptr, TRUE);
}

void Debugger::PopulateTreeSnapshotItem(HTREEITEM parent, const SceneNodeSnapshot& snapshot, HWND tree) {
	if (snapshot.name.empty()) return;
	TVINSERTSTRUCT tvis{};
	tvis.hParent = parent;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM;
	tvis.item.pszText = const_cast<char*>(snapshot.name.c_str());
	tvis.item.lParam = reinterpret_cast<LPARAM>(snapshot.ptr);
	HTREEITEM item = reinterpret_cast<HTREEITEM>(SendMessage(tree, TVM_INSERTITEMA, 0, reinterpret_cast<LPARAM>(&tvis)));
	for (const auto& child : snapshot.children) {
		PopulateTreeSnapshotItem(item, child, tree);
	}
}

void Debugger::RefreshAssetsView() {}
void Debugger::RefreshSoundState() {}

void Debugger::PushEvent(DebuggerEvent ev) {
	{
		std::lock_guard<std::mutex> lock(_QueueMutex);
		_EventQueue.push_back(std::move(ev));
	}
	if (_Window) {
		PostMessage(_Window, WM_USER_PROCESS_QUEUE, 0, 0);
	}
}

void Debugger::PushCommand(DebuggerCommand cmd) {
	std::lock_guard<std::mutex> lock(_CommandMutex);
	_CommandQueue.push_back(std::move(cmd));
}

void Debugger::ProcessEvents() {
	std::vector<DebuggerEvent> events;
	{
		std::lock_guard<std::mutex> lock(_QueueMutex);
		events.swap(_EventQueue);
	}
	for (const auto& ev : events) {
		switch (ev.type) {
		case DebuggerEventType::WriteLine:
			_LogLines.push_back(ev.text);
			if (_LogLines.size() > 2000) {
				_LogLines.erase(_LogLines.begin(), _LogLines.begin() + 500);
			}
			_NeedsLogRefresh = true;
			break;
		case DebuggerEventType::SetProperty:
			if (_Properties.find(ev.key) == _Properties.end() || _Properties[ev.key] != ev.value) {
				_Properties[ev.key] = ev.value;
				_NeedsUpdate = true;
			}
			break;
		case DebuggerEventType::RecordFrameTime:
			_UpdateTimeMsHistory.push_back(ev.updateMs);
			_DrawTimeMsHistory.push_back(ev.drawMs);
			if (_UpdateTimeMsHistory.size() > FrameTimeHistoryLimit) {
				_UpdateTimeMsHistory.erase(_UpdateTimeMsHistory.begin());
				_DrawTimeMsHistory.erase(_DrawTimeMsHistory.begin());
			}
			InvalidateRect(_Window, &_FrameTimeGraphRect, TRUE);
			SetProperty("Frame.UpdateMs", std::to_string(ev.updateMs));
			SetProperty("Frame.DrawMs", std::to_string(ev.drawMs));
			SetProperty("Frame.TotalMs", std::to_string(ev.updateMs + ev.drawMs));
			UpdateMemoryStats();
			UpdateCpuStats();
			break;
		case DebuggerEventType::UpdateSceneTree:
			if (_SceneTreeView) {
				SendMessage(_SceneTreeView, TVM_DELETEITEM, 0, reinterpret_cast<LPARAM>(TVI_ROOT));
				PopulateTreeSnapshotItem(nullptr, ev.treeSnapshot, _SceneTreeView);
			}
			break;
		case DebuggerEventType::UpdateAssets:
			if (_AssetList) {
				SCROLLINFO scrollInfo{};
				scrollInfo.cbSize = sizeof(scrollInfo);
				scrollInfo.fMask = SIF_POS;
				GetScrollInfo(_AssetList, SB_VERT, &scrollInfo);

				SendMessage(_AssetList, WM_SETREDRAW, FALSE, 0);
				SendMessage(_AssetList, LVM_DELETEALLITEMS, 0, 0);
				int index = 0;
				for (const auto& asset : ev.assetSnapshots) {
					const std::wstring assetPath = asset.path.wstring();
					LVITEMW item{};
					item.mask = LVIF_TEXT;
					item.iItem = index;
					item.iSubItem = 0;
					item.pszText = const_cast<wchar_t*>(assetPath.c_str());
					int row = static_cast<int>(SendMessage(_AssetList, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&item)));
					if (row not_eq -1) {
						const std::wstring refStr = std::to_wstring(asset.refCount);
						LVITEMW subItem{};
						subItem.iSubItem = 1;
						subItem.pszText = const_cast<wchar_t*>(refStr.c_str());
						SendMessage(_AssetList, LVM_SETITEMTEXTW, row, reinterpret_cast<LPARAM>(&subItem));
					}
					index++;
				}
				SetScrollInfo(_AssetList, SB_VERT, &scrollInfo, FALSE);
				SendMessage(_AssetList, WM_SETREDRAW, TRUE, 0);
				InvalidateRect(_AssetList, nullptr, TRUE);
			}
			break;
		}
	}
	if (_NeedsLogRefresh) {
		RefreshLogView();
	}
	if (_NeedsUpdate) {
		RefreshPropertyView();
		_NeedsUpdate = false;
	}
}

void Debugger::ProcessCommands() {
	std::vector<DebuggerCommand> cmds;
	{
		std::lock_guard<std::mutex> lock(_CommandMutex);
		cmds.swap(_CommandQueue);
	}
	for (const auto& cmd : cmds) {
		switch (cmd.type) {
		case DebuggerCommandType::ToggleDebugMode:
			PropertyManager.ToggleDebugMode();
			break;
		case DebuggerCommandType::Pause:
			GameState.Pause();
			break;
		case DebuggerCommandType::Resume:
			GameState.Resume();
			break;
		case DebuggerCommandType::StepFrame:
			GameState.RequestStepFrame();
			break;
		case DebuggerCommandType::SetTimeScale:
			GameState.SetTimeScale(cmd.floatVal);
			break;
		case DebuggerCommandType::StopBGM:
			SoundManager.StopBGM();
			break;
		case DebuggerCommandType::MuteAll:
			SoundManager.MuteAll();
			break;
		case DebuggerCommandType::UnmuteAll:
			SoundManager.UnmuteAll();
			break;
		case DebuggerCommandType::SetPropertyFromString:
			PropertyManager.SetPropertyFromString(cmd.name, cmd.value);
			break;
		case DebuggerCommandType::RequestSceneTree: {
			auto root = SceneManager.GetCurrentScene();
			DebuggerEvent ev;
			ev.type = DebuggerEventType::UpdateSceneTree;
			if (root) {
				ev.treeSnapshot = BuildSceneSnapshot(root);
			}
			PushEvent(ev);
			break;
		}
		}
	}

	for (const auto& prop : PropertyManager.GetBoundProperties()) {
		SetProperty(prop.name, PropertyManager.GetPropertyValueString(prop.name));
	}


	static ULONGLONG lastAssetUpdate = 0;
	const ULONGLONG now = GetTickCount64();
	if (now - lastAssetUpdate >= 500) {
		lastAssetUpdate = now;
		auto handles = AssetManager.GetAssetHandles();
		std::vector<AssetSnapshot> assetList;
		for (const auto& [path, handle] : handles) {
			assetList.push_back({ path, handle.references.size() });
		}
		DebuggerEvent evAssets;
		evAssets.type = DebuggerEventType::UpdateAssets;
		evAssets.assetSnapshots = std::move(assetList);
		PushEvent(evAssets);
	}
}

void Debugger::ThreadMain() {
	InitCommonControls();
	static const char* windowClassName = "NovotDebuggerWindow";
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
		SendMessage(_DebugModeCheckBox, BM_SETCHECK, PropertyManager.IsDebugMode() ? BST_CHECKED : BST_UNCHECKED, 0);

		_PauseButton = CreateWindowEx(0, "BUTTON", GameState.IsPaused() ? "Resume" : "Pause", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 420, 12, 120, 26, _Window, reinterpret_cast<HMENU>(PauseButtonId), GetModuleHandle(nullptr), nullptr);
		GameState.Paused.Connect([this]() {
			if (_PauseButton) {
				SetWindowText(_PauseButton, "Resume");
			}
			});
		GameState.Resumed.Connect([this]() {
			if (_PauseButton) {
				SetWindowText(_PauseButton, "Pause");
			}
			});
		_StepFrameButton = CreateWindowEx(0, "BUTTON", "Step Frame", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 548, 12, 120, 26, _Window, reinterpret_cast<HMENU>(StepFrameButtonId), GetModuleHandle(nullptr), nullptr);
		_TimeScaleCombo = CreateWindowEx(WS_EX_CLIENTEDGE, "COMBOBOX", "", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 676, 12, 140, 200, _Window, reinterpret_cast<HMENU>(TimeScaleComboId), GetModuleHandle(nullptr), nullptr);
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("0.1x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("0.25x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("0.5x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("1.0x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("2.0x"));
		SendMessage(_TimeScaleCombo, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>("4.0x"));
		SendMessage(_TimeScaleCombo, CB_SETCURSEL, 3, 0);

		_RefreshTreeButton = CreateWindowEx(0, "BUTTON", "Refresh Tree", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 760, 12, 120, 26, _Window, reinterpret_cast<HMENU>(RefreshTreeButtonId), GetModuleHandle(nullptr), nullptr);
		_SceneTreeView = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEWA, "", WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES, 760, 48, 220, 200, _Window, reinterpret_cast<HMENU>(SceneTreeViewId), GetModuleHandle(nullptr), nullptr);

		// Sound & Asset controls
		_StopBGMButton = CreateWindowEx(0, "BUTTON", "Stop BGM", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 760, 260, 100, 26, _Window, reinterpret_cast<HMENU>(StopBGMButtonId), GetModuleHandle(nullptr), nullptr);
		_MuteAllButton = CreateWindowEx(0, "BUTTON", "Mute All", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 868, 260, 100, 26, _Window, reinterpret_cast<HMENU>(MuteAllButtonId), GetModuleHandle(nullptr), nullptr);

		_AssetList = CreateWindowEx(0, WC_LISTVIEW, "", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL, 760, 296, 220, 140, _Window, reinterpret_cast<HMENU>(AssetListId), GetModuleHandle(nullptr), nullptr);
		LVCOLUMNW column{};
		column.mask = LVCF_TEXT | LVCF_WIDTH;
		column.pszText = const_cast<wchar_t*>(L"file");
		column.cx =380;
		SendMessage(_AssetList, LVM_INSERTCOLUMNW, 0, reinterpret_cast<LPARAM>(&column));
		column.pszText = const_cast<wchar_t*>(L"ref");
		column.cx = 50;
		SendMessage(_AssetList, LVM_INSERTCOLUMNW, 1, reinterpret_cast<LPARAM>(&column));

		_ReloadSpriteButton = CreateWindowEx(0, "BUTTON", "Reload", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 760, 444, 100, 26, _Window, reinterpret_cast<HMENU>(ReloadSpriteButtonId), GetModuleHandle(nullptr), nullptr);
		_ReleaseSpriteButton = CreateWindowEx(0, "BUTTON", "Release", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 868, 444, 100, 26, _Window, reinterpret_cast<HMENU>(ReleaseSpriteButtonId), GetModuleHandle(nullptr), nullptr);

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

		SendMessage(_LogEdit, EM_SETLIMITTEXT, 0, 0);
		SendMessage(_PropertyView, EM_SETLIMITTEXT, 0, 0);
		SetWindowText(_LogEdit, "--- Runtime Log ---\r\n");
		SetWindowText(_PropertyView, "--- Property Inspector ---\r\n");
		SendMessage(_Window, WM_SIZE, 0, MAKELPARAM(1000, 800));
		_NeedsUpdate = true;
		_NeedsLogRefresh = true;
		AdjustUI();
	}

	if (_WindowCreatedEvent) {
		SetEvent(_WindowCreatedEvent);
	}

	MSG msg{};
	while (_Running && GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

Debugger::Debugger() {
	SYSTEM_INFO sysInfo{};
	GetSystemInfo(&sysInfo);
	_NumProcessors = (std::max)(1, static_cast<int>(sysInfo.dwNumberOfProcessors));

	_WindowCreatedEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	_DebuggerThread = std::thread(&Debugger::ThreadMain, this);
	if (_WindowCreatedEvent) {
		WaitForSingleObject(_WindowCreatedEvent, 5000);
		CloseHandle(_WindowCreatedEvent);
		_WindowCreatedEvent = nullptr;
	}
}

Debugger::~Debugger() {
	_Running = false;
	if (_Window) {
		PostMessage(_Window, WM_CLOSE, 0, 0);
	}
	if (_DebuggerThread.joinable()) {
		_DebuggerThread.join();
	}
	if (_UIFont) {
		DeleteObject(_UIFont);
		_UIFont = nullptr;
	}
}

void Debugger::WriteLine(std::string msg) {
	PushEvent({ DebuggerEventType::WriteLine, msg });
}

void Debugger::SetProperty(std::string key, std::string value) {
	PushEvent({ DebuggerEventType::SetProperty, "", key, value });
}

void Debugger::Update() {
	ProcessCommands();
}

void Debugger::RefreshLogView() {
	if (!_LogEdit) return;
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
	for (const auto& prop : PropertyManager.GetBoundProperties()) {
		if (std::find(candidates.begin(), candidates.end(), prop.name) == candidates.end()) {
			candidates.push_back(prop.name);
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

	const int firstVisible = static_cast<int>(SendMessage(_PropertyView, EM_GETFIRSTVISIBLELINE, 0, 0));
	DWORD selStart = 0, selEnd = 0;
	SendMessage(_PropertyView, EM_GETSEL, reinterpret_cast<WPARAM>(&selStart), reinterpret_cast<LPARAM>(&selEnd));

	std::stringstream ss;
	ss << "--- Property Inspector ---\r\n";
	for (const auto& [key, value] : _Properties) {
		ss << key << ": " << value << "\r\n";
	}

	SetWindowText(_PropertyView, ss.str().c_str());

	SendMessage(_PropertyView, EM_SETSEL, selStart, selEnd);
	const int currentFirst = static_cast<int>(SendMessage(_PropertyView, EM_GETFIRSTVISIBLELINE, 0, 0));
	const int deltaLines = firstVisible - currentFirst;
	if (deltaLines != 0) {
		SendMessage(_PropertyView, EM_LINESCROLL, 0, deltaLines);
	}
}

void Debugger::UpdateMemoryStats() {
	const ULONGLONG now = GetTickCount64();
	if (_LastMemorySampleTick != 0 && (now - _LastMemorySampleTick < MemorySampleIntervalMs)) {
		return;
	}
	_LastMemorySampleTick = now;

	PROCESS_MEMORY_COUNTERS_EX memInfo{};
	memInfo.cb = sizeof(memInfo);
	if (!GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memInfo), sizeof(memInfo))) {
		PROCESS_MEMORY_COUNTERS stdMemInfo{};
		stdMemInfo.cb = sizeof(stdMemInfo);
		if (GetProcessMemoryInfo(GetCurrentProcess(), &stdMemInfo, sizeof(stdMemInfo))) {
			memInfo.WorkingSetSize = stdMemInfo.WorkingSetSize;
			memInfo.PeakWorkingSetSize = stdMemInfo.PeakWorkingSetSize;
			memInfo.PagefileUsage = stdMemInfo.PagefileUsage;
			memInfo.PrivateUsage = stdMemInfo.PagefileUsage;
		} else {
			return;
		}
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

void Debugger::UpdateCpuStats() {
	const ULONGLONG now = GetTickCount64();
	if (_LastCpuSampleTick != 0 && (now - _LastCpuSampleTick < CpuSampleIntervalMs)) {
		return;
	}

	if (_NumProcessors <= 0) {
		SYSTEM_INFO sysInfo{};
		GetSystemInfo(&sysInfo);
		_NumProcessors = (std::max)(1, static_cast<int>(sysInfo.dwNumberOfProcessors));
	}

	FILETIME ftCreation{}, ftExit{}, ftKernel{}, ftUser{}, ftSysNow{};
	if (!GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser)) {
		return;
	}
	GetSystemTimeAsFileTime(&ftSysNow);

	const ULONGLONG kTime = FileTimeToULL(ftKernel);
	const ULONGLONG uTime = FileTimeToULL(ftUser);
	const ULONGLONG sysTime = FileTimeToULL(ftSysNow);

	if (_LastSystemTime != 0 && sysTime > _LastSystemTime) {
		const ULONGLONG procDiff = (kTime - _LastProcessKernelTime) + (uTime - _LastProcessUserTime);
		const ULONGLONG sysDiff = sysTime - _LastSystemTime;

		double cpuPercent = (static_cast<double>(procDiff) / (static_cast<double>(sysDiff) * static_cast<double>(_NumProcessors))) * 100.0;
		cpuPercent = (std::clamp)(cpuPercent, 0.0, 100.0);

		_CpuUsageHistoryPercent.push_back(cpuPercent);
		if (_CpuUsageHistoryPercent.size() > CpuHistoryLimit) {
			_CpuUsageHistoryPercent.erase(_CpuUsageHistoryPercent.begin());
		}
		InvalidateRect(_Window, &_CpuGraphRect, TRUE);
		SetProperty("CPU.ProcessUsage(%)", std::to_string(static_cast<int>(std::round(cpuPercent))) + "%");
	}

	_LastProcessKernelTime = kTime;
	_LastProcessUserTime = uTime;
	_LastSystemTime = sysTime;
	_LastCpuSampleTick = now;
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
	TextOut(hdc, _MemoryGraphRect.left + 10, _MemoryGraphRect.top + 6, "Memory (MiB)", 12);

	RECT plotRect = _MemoryGraphRect;
	plotRect.left += 65;
	plotRect.right -= 10;
	plotRect.top += 24;
	plotRect.bottom -= 20;
	if (plotRect.right - plotRect.left < 30 || plotRect.bottom - plotRect.top < 30) {
		return;
	}

	FrameRect(hdc, &plotRect, static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH)));
	HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
	HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, gridPen));
	for (int i = 1; i < 4; ++i) {
		const int y = plotRect.top + ((plotRect.bottom - plotRect.top) * i / 4);
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

	// Y-axis tick marks
	COLORREF oldColor = SetTextColor(hdc, RGB(80, 80, 80));
	HFONT font = CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");
	HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));
	for (int i = 0; i <= 4; ++i) {
		double val = maxValue * (4 - i) / 4.0;
		int y = plotRect.top + ((plotRect.bottom - plotRect.top) * i / 4);
		std::ostringstream ss;
		if (maxValue >= 100.0) {
			ss << static_cast<int>(std::round(val)) << " MiB";
		} else {
			ss << std::fixed << std::setprecision(1) << val << " MiB";
		}
		std::string label = ss.str();
		TextOut(hdc, plotRect.left - 60, y - 7, label.c_str(), static_cast<int>(label.size()));
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

	SetTextColor(hdc, oldColor);
	SelectObject(hdc, oldFont);
	DeleteObject(font);

	const std::string workingText = "Working: " + FormatMiB(static_cast<SIZE_T>(_WorkingSetHistoryMiB.back() * 1024.0 * 1024.0)) + " MiB";
	TextOut(hdc, plotRect.left + 4, plotRect.top + 4, workingText.c_str(), static_cast<int>(workingText.size()));
}

void Debugger::DrawCpuGraph(HDC hdc) const {
	if (_CpuGraphRect.right <= _CpuGraphRect.left || _CpuGraphRect.bottom <= _CpuGraphRect.top) {
		return;
	}

	HBRUSH panelBrush = CreateSolidBrush(RGB(247, 247, 247));
	FillRect(hdc, &_CpuGraphRect, panelBrush);
	DeleteObject(panelBrush);
	FrameRect(hdc, &_CpuGraphRect, static_cast<HBRUSH>(GetStockObject(GRAY_BRUSH)));

	SetBkMode(hdc, TRANSPARENT);
	TextOut(hdc, _CpuGraphRect.left + 10, _CpuGraphRect.top + 6, "CPU Usage (%)", 13);

	RECT plotRect = _CpuGraphRect;
	plotRect.left += 65;
	plotRect.right -= 10;
	plotRect.top += 24;
	plotRect.bottom -= 20;
	if (plotRect.right - plotRect.left < 30 || plotRect.bottom - plotRect.top < 30) {
		return;
	}

	FrameRect(hdc, &plotRect, static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH)));
	HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(220, 220, 220));
	HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, gridPen));
	for (int i = 1; i < 4; ++i) {
		const int y = plotRect.top + ((plotRect.bottom - plotRect.top) * i / 4);
		MoveToEx(hdc, plotRect.left, y, nullptr);
		LineTo(hdc, plotRect.right, y);
	}
	SelectObject(hdc, oldPen);
	DeleteObject(gridPen);

	const size_t count = _CpuUsageHistoryPercent.size();
	if (count < 2) {
		TextOut(hdc, plotRect.left + 8, plotRect.top + 8, "Collecting CPU samples...", 25);
		return;
	}

	// Y-axis tick marks
	COLORREF oldColor = SetTextColor(hdc, RGB(80, 80, 80));
	HFONT font = CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");
	HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));
	for (int i = 0; i <= 4; ++i) {
		int val = 100 - (i * 25);
		int y = plotRect.top + ((plotRect.bottom - plotRect.top) * i / 4);
		std::string label = std::to_string(val) + " %";
		TextOut(hdc, plotRect.left - 60, y - 7, label.c_str(), static_cast<int>(label.size()));
	}

	HPEN pen = CreatePen(PS_SOLID, 2, RGB(231, 76, 60));
	HPEN prev = static_cast<HPEN>(SelectObject(hdc, pen));
	for (size_t i = 0; i < count; ++i) {
		const double xRatio = static_cast<double>(i) / static_cast<double>(count - 1);
		const double yRatio = (std::clamp)(_CpuUsageHistoryPercent[i], 0.0, 100.0) / 100.0;
		const int x = plotRect.left + static_cast<int>(xRatio * (plotRect.right - plotRect.left));
		const int y = plotRect.bottom - static_cast<int>(yRatio * (plotRect.bottom - plotRect.top));
		if (i == 0) {
			MoveToEx(hdc, x, y, nullptr);
		} else {
			LineTo(hdc, x, y);
		}
	}
	SelectObject(hdc, prev);
	DeleteObject(pen);
	SetTextColor(hdc, oldColor);
	SelectObject(hdc, oldFont);
	DeleteObject(font);

	const std::string curText = "CPU: " + std::to_string(static_cast<int>(std::round(_CpuUsageHistoryPercent.back()))) + " %";
	TextOut(hdc, plotRect.left + 4, plotRect.top + 4, curText.c_str(), static_cast<int>(curText.size()));
}

void Debugger::RecordFrameTime(double updateMs, double drawMs) {
	PushEvent({ DebuggerEventType::RecordFrameTime, "", "", "", updateMs, drawMs });
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
	TextOut(hdc, _FrameTimeGraphRect.left + 10, _FrameTimeGraphRect.top + 8, "Frame Time (ms)", 15);

	RECT plotRect = _FrameTimeGraphRect;
	plotRect.left += 65;
	plotRect.right -= 10;
	plotRect.top += 30;
	plotRect.bottom -= 10;
	if (plotRect.right - plotRect.left < 40 || plotRect.bottom - plotRect.top < 40) return;

	size_t count = _UpdateTimeMsHistory.size();
	if (count < 2) {
		TextOut(hdc, plotRect.left + 8, plotRect.top + 8, "Collecting frame times...", 24);
		return;
	}

	double maxValue = 16.67;
	for (size_t i = 0; i < count; ++i) {
		maxValue = (std::max)(maxValue, _UpdateTimeMsHistory[i]);
		maxValue = (std::max)(maxValue, _DrawTimeMsHistory[i]);
	}

	// Y-axis tick marks
	COLORREF oldColor = SetTextColor(hdc, RGB(80, 80, 80));
	HFONT font = CreateFont(-14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");
	HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, font));
	for (int i = 0; i <= 4; ++i) {
		double val = maxValue * (4 - i) / 4.0;
		int y = plotRect.top + ((plotRect.bottom - plotRect.top) * i / 4);
		std::ostringstream ss;
		ss << std::fixed << std::setprecision(1) << val << " ms";
		std::string label = ss.str();
		TextOut(hdc, plotRect.left - 60, y - 7, label.c_str(), static_cast<int>(label.size()));
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

	drawSeries(_UpdateTimeMsHistory, RGB(46, 204, 113));
	drawSeries(_DrawTimeMsHistory, RGB(155, 89, 182));

	double y60 = 16.67 / maxValue;
	int yline = plotRect.bottom - static_cast<int>(y60 * (plotRect.bottom - plotRect.top));
	HPEN dash = CreatePen(PS_DOT, 1, RGB(200, 0, 0));
	HPEN old = static_cast<HPEN>(SelectObject(hdc, dash));
	MoveToEx(hdc, plotRect.left, yline, nullptr);
	LineTo(hdc, plotRect.right, yline);
	SelectObject(hdc, old);
	DeleteObject(dash);

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

	SetTextColor(hdc, oldColor);
	SelectObject(hdc, oldFont);
	DeleteObject(font);
}

void Debugger::RefreshSceneTree() {
	PushCommand({ DebuggerCommandType::RequestSceneTree });
}

void Debugger::BindProperty(const std::string& key, int* target) {
	PropertyManager.BindProperty(key, target);
}
void Debugger::BindProperty(const std::string& key, float* target) {
	PropertyManager.BindProperty(key, target);
}
void Debugger::BindProperty(const std::string& key, double* target) {
	PropertyManager.BindProperty(key, target);
}
void Debugger::BindProperty(const std::string& key, bool* target) {
	PropertyManager.BindProperty(key, target);
}
void Debugger::BindProperty(const std::string& key, std::string* target) {
	PropertyManager.BindProperty(key, target);
}
void Debugger::UnbindProperty(const std::string& key) {
	PropertyManager.UnbindProperty(key);
}
void Debugger::ClearBindings() {
	PropertyManager.ClearBoundProperties();
}
#endif