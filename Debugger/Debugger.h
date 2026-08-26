#pragma once
#ifdef _DEBUG
#include <Singleton.h>
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <map>
#include <memory>

class Debugger : public Singleton<Debugger> {
private:
	HWND _Window;
	HWND _LogEdit;
	HWND _PropertyView;
	HWND _DebugModeCheckBox;
	HWND _ClearLogButton;
	HWND _FilterStatic;
	HWND _FilterEdit;
	HFONT _UIFont = nullptr;
	std::map<std::string, std::string> _Properties;
	bool _NeedsUpdate = false;
	bool _NeedsLogRefresh = false;
	std::vector<std::string> _LogLines;
	std::string _FilterText;
	RECT _MemoryGraphRect{};
	std::vector<double> _WorkingSetHistoryMiB;
	std::vector<double> _PrivateUsageHistoryMiB;
	DWORD _LastMemorySampleTick = 0;
	static constexpr DWORD MemorySampleIntervalMs = 250;
	static constexpr size_t MemoryHistoryLimit = 240;

	// Frame time profiler
	RECT _FrameTimeGraphRect{};
	std::vector<double> _UpdateTimeMsHistory;
	std::vector<double> _DrawTimeMsHistory;
	static constexpr size_t FrameTimeHistoryLimit = 240;

	// Scene tree
	HWND _SceneTreeView;
	HWND _RefreshTreeButton;

	// Time control
	HWND _PauseButton;
	HWND _StepFrameButton;
	HWND _TimeScaleCombo;

		// Sound & Assets
		HWND _StopBGMButton;
		HWND _MuteAllButton;
		HWND _AssetList; // listbox
		HWND _ReloadSpriteButton;
		HWND _ReleaseSpriteButton;

		// Property editing & binding
	HWND _PropNameCombo;
	HWND _PropValueEdit;
	HWND _SetPropButton;
	std::vector<std::string> _LastCandidates;
	std::vector<std::string> _AssetNames;

	static constexpr int DebugModeCheckBoxId = 1001;
	static constexpr int ClearLogButtonId = 1002;
	static constexpr int FilterEditId = 1003;
	static constexpr int PauseButtonId = 1004;
	static constexpr int StepFrameButtonId = 1005;
	static constexpr int TimeScaleComboId = 1006;
	static constexpr int RefreshTreeButtonId = 1007;
	static constexpr int SceneTreeViewId = 1008;
	static constexpr int StopBGMButtonId = 1009;
	static constexpr int MuteAllButtonId = 1010;
	static constexpr int AssetListId = 1011;
	static constexpr int ReloadSpriteButtonId = 1012;
	static constexpr int ReleaseSpriteButtonId = 1013;
	static constexpr int PropNameEditId = 1014;
	static constexpr int PropValueEditId = 1015;
	static constexpr int SetPropButtonId = 1016;

	void RefreshLogView();
	void RefreshPropertyView();
	void PopulatePropertyCandidates();
	void DrawMemoryGraph(HDC hdc) const;
	void DrawFrameTimeGraph(HDC hdc) const;
	void UpdateMemoryStats();
	void PopulateTreeItem(HTREEITEM parent, std::shared_ptr<class Scene> scene, HWND tree);
	void RefreshAssetsView();
	void RefreshSoundState();
	static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

public:
	friend class Singleton<Debugger>;
	void WriteLine(std::string msg);
	void SetProperty(std::string key, std::string value);
	void Update();
	~Debugger();

	void RecordFrameTime(double updateMs, double drawMs);
	void RefreshSceneTree();
	void AdjustUI();

	void BindProperty(const std::string& key, int* target);
	void BindProperty(const std::string& key, float* target);
	void BindProperty(const std::string& key, bool* target);
	void BindProperty(const std::string& key, std::string* target);
	void UnbindProperty(const std::string& key);
	void ClearBindings();

protected:
	Debugger();
};

#define DebuggerWriteLine(msg) Debugger::GetInstance().WriteLine(msg)
#define DebuggerSetProperty(key, value) Debugger::GetInstance().SetProperty(key, value)
#define DebuggerUpdate() Debugger::GetInstance().Update()
#define DebuggerBindProperty(key, target) Debugger::GetInstance().BindProperty(key, target)
#define DebuggerRecordFrameTime(updateMs, drawMs) Debugger::GetInstance().RecordFrameTime(updateMs, drawMs)
#define DebuggerUnbindProperty(key) Debugger::GetInstance().UnbindProperty(key)
#define DebuggerRefreshSceneTree() Debugger::GetInstance().RefreshSceneTree()
#else
#define DebuggerWriteLine(msg)
#define DebuggerSetProperty(key, value)
#define DebuggerUpdate()
#define DebuggerBindProperty(key, target)
#define DebuggerRecordFrameTime(updateMs, drawMs)
#define DebuggerUnbindProperty(key)
#define DebuggerRefreshSceneTree()
#endif