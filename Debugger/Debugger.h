#pragma once

#if defined(_DEBUG) and not defined(_NOVOT_NO_DEBUGGER)
#include <Singleton.h>
#include <Windows.h>
#include <CommCtrl.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>

namespace fs = std::filesystem;

struct SceneNodeSnapshot {
	std::string name;
	void* ptr = nullptr;
	std::vector<SceneNodeSnapshot> children;
};

struct AssetSnapshot {
	fs::path path;
	size_t refCount = 0;
};

enum class DebuggerEventType {
	WriteLine,
	SetProperty,
	RecordFrameTime,
	UpdateSceneTree,
	UpdateAssets
};

struct DebuggerEvent {
	DebuggerEventType type;
	std::string text;
	std::string key;
	std::string value;
	double updateMs = 0.0;
	double drawMs = 0.0;
	SceneNodeSnapshot treeSnapshot;
	std::vector<AssetSnapshot> assetSnapshots;
};

enum class DebuggerCommandType {
	ToggleDebugMode,
	ToggleCollisionDebugVisible,
	Pause,
	Resume,
	StepFrame,
	SetTimeScale,
	StopBGM,
	MuteAll,
	UnmuteAll,
	SetPropertyFromString,
	RequestSceneTree
};

struct DebuggerCommand {
	DebuggerCommandType type;
	float floatVal = 1.0f;
	std::string name;
	std::string value;
};

class Debugger : public Singleton<Debugger> {
private:
	std::thread _DebuggerThread;
	std::atomic<bool> _Running{ true };
	HANDLE _WindowCreatedEvent = nullptr;

	std::mutex _QueueMutex;
	std::vector<DebuggerEvent> _EventQueue;

	std::mutex _CommandMutex;
	std::vector<DebuggerCommand> _CommandQueue;

	HWND _Window = nullptr;
	HWND _LogEdit = nullptr;
	HWND _PropertyView = nullptr;
	HWND _DebugModeCheckBox = nullptr;
	HWND _CollisionDebugCheckBox = nullptr;
	HWND _ClearLogButton = nullptr;
	HWND _FilterStatic = nullptr;
	HWND _FilterEdit = nullptr;
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
	HWND _SceneTreeView = nullptr;
	HWND _RefreshTreeButton = nullptr;

	// Time control
	HWND _PauseButton = nullptr;
	HWND _StepFrameButton = nullptr;
	HWND _TimeScaleCombo = nullptr;

	// Sound & Assets
	HWND _StopBGMButton = nullptr;
	HWND _MuteAllButton = nullptr;
	HWND _AssetList = nullptr; // listbox
	HWND _ReloadSpriteButton = nullptr;
	HWND _ReleaseSpriteButton = nullptr;

	// Property editing & binding
	HWND _PropNameCombo = nullptr;
	HWND _PropValueEdit = nullptr;
	HWND _SetPropButton = nullptr;
	std::vector<std::string> _LastCandidates;
	std::vector<std::string> _AssetNames;

	static constexpr int DebugModeCheckBoxId = 1001;
	static constexpr int CollisionDebugCheckBoxId = 1017;
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

	static constexpr UINT WM_USER_PROCESS_QUEUE = WM_USER + 101;

	void ThreadMain();
	void ProcessEvents();
	void PushEvent(DebuggerEvent ev);
	void PushCommand(DebuggerCommand cmd);

	// CPU profiler
	RECT _CpuGraphRect{};
	std::vector<double> _CpuUsageHistoryPercent;
	static constexpr size_t CpuHistoryLimit = 240;
	ULONGLONG _LastProcessKernelTime = 0;
	ULONGLONG _LastProcessUserTime = 0;
	ULONGLONG _LastSystemTime = 0;
	DWORD _LastCpuSampleTick = 0;
	static constexpr DWORD CpuSampleIntervalMs = 250;
	int _NumProcessors = 0;

	void RefreshLogView();
	void RefreshPropertyView();
	void PopulatePropertyCandidates();
	void DrawMemoryGraph(HDC hdc) const;
	void DrawFrameTimeGraph(HDC hdc) const;
	void DrawCpuGraph(HDC hdc) const;
	void UpdateMemoryStats();
	void UpdateCpuStats();
	void PopulateTreeSnapshotItem(HTREEITEM parent, const SceneNodeSnapshot& snapshot, HWND tree);
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
	void ProcessCommands();

	void BindProperty(const std::string& key, int* target);
	void BindProperty(const std::string& key, float* target);
	void BindProperty(const std::string& key, double* target);
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
#define DebuggerProcessCommands() Debugger::GetInstance().ProcessCommands()
#else
#define DebuggerWriteLine(msg)
#define DebuggerSetProperty(key, value)
#define DebuggerUpdate()
#define DebuggerBindProperty(key, target)
#define DebuggerRecordFrameTime(updateMs, drawMs)
#define DebuggerUnbindProperty(key)
#define DebuggerRefreshSceneTree()
#define DebuggerProcessCommands()
#endif
