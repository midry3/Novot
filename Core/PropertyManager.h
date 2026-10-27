#pragma once
#include <DxLib.h>
#include <Math/Vector2D.h>
#include "Singleton.h"
#include <string>
#include <vector>
#include <variant>
#include <algorithm>

class _PropertyManager : public Singleton<_PropertyManager> {
public:
    enum class PropertyType { Int, Float, Double, Bool, String };

    struct PropertyEntry {
        std::string name;
        PropertyType type;
        std::variant<int*, float*, double*, bool*, std::string*> ptr;
    };

private:
#ifdef _DEBUG
    bool debugMode = false;
    bool collisionDebugVisible = false;
#endif
    Vector2D windowSize = Vector2D(1280, 720);
    int FPS = 60;
    std::vector<PropertyEntry> _boundProperties;

public:
    Vector2D GetSettingWindowSize() const {
        return windowSize;
    }
    void SetWindowSize(const Vector2D& size) {
        windowSize = size;
		SetGraphMode(static_cast<int>(size.x), static_cast<int>(size.y), 32);
		DxLib::SetWindowSize(static_cast<int>(size.x), static_cast<int>(size.y));
    }
    void SetFPS(int fps) {
        FPS = fps;
    }
    int GetFPS() const {
        return FPS;
    }
    float GetFrameInterval() const {
        return 1000.0f / FPS;
    }
    float GetFrameIntervalAsSec() const {
        return 1.0f / FPS;
    }
    Vector2D GetRealWindowSize() const {
        int w, h;
        GetWindowSize(&w, &h);
        return Vector2D(w, h);
    }
	void ToFullScreen() {
        ChangeWindowMode(FALSE);
        SetDrawScreen(DX_SCREEN_BACK);
	}
	void ToWindowed() {
		ChangeWindowMode(TRUE);
		SetDrawScreen(DX_SCREEN_BACK);
	}
    void SwitchWindowMode() {
		if (GetWindowModeFlag()) {
			ToFullScreen();
		}
		else {
			ToWindowed();
		}
    }
#ifdef _DEBUG
    bool IsDebugMode() const {
        return debugMode;
    }
    bool ToggleDebugMode() {
        debugMode = !debugMode;
        return debugMode;
    }
    bool IsCollisionDebugVisible() const {
        return collisionDebugVisible;
    }
    bool ToggleCollisionDebugVisible() {
        collisionDebugVisible = !collisionDebugVisible;
        return collisionDebugVisible;
    }
    void BindProperty(const std::string& name, int* target) {
        auto it = std::find_if(_boundProperties.begin(), _boundProperties.end(), [&](const PropertyEntry& e) { return e.name == name; });
        if (it != _boundProperties.end()) {
            it->type = PropertyType::Int;
            it->ptr = target;
        } else {
            _boundProperties.push_back({ name, PropertyType::Int, target });
        }
    }
    void BindProperty(const std::string& name, float* target) {
        auto it = std::find_if(_boundProperties.begin(), _boundProperties.end(), [&](const PropertyEntry& e) { return e.name == name; });
        if (it != _boundProperties.end()) {
            it->type = PropertyType::Float;
            it->ptr = target;
        } else {
            _boundProperties.push_back({ name, PropertyType::Float, target });
        }
    }
    void BindProperty(const std::string& name, double* target) {
        auto it = std::find_if(_boundProperties.begin(), _boundProperties.end(), [&](const PropertyEntry& e) { return e.name == name; });
        if (it != _boundProperties.end()) {
            it->type = PropertyType::Double;
            it->ptr = target;
        }
        else {
            _boundProperties.push_back({ name, PropertyType::Double, target });
        }
    }
    void BindProperty(const std::string& name, bool* target) {
        auto it = std::find_if(_boundProperties.begin(), _boundProperties.end(), [&](const PropertyEntry& e) { return e.name == name; });
        if (it != _boundProperties.end()) {
            it->type = PropertyType::Bool;
            it->ptr = target;
        } else {
            _boundProperties.push_back({ name, PropertyType::Bool, target });
        }
    }
    void BindProperty(const std::string& name, std::string* target) {
        auto it = std::find_if(_boundProperties.begin(), _boundProperties.end(), [&](const PropertyEntry& e) { return e.name == name; });
        if (it != _boundProperties.end()) {
            it->type = PropertyType::String;
            it->ptr = target;
        } else {
            _boundProperties.push_back({ name, PropertyType::String, target });
        }
    }

    void UnbindProperty(const std::string& name) {
        std::erase_if(_boundProperties, [&](const PropertyEntry& e) { return e.name == name; });
    }

    void ClearBoundProperties() {
        _boundProperties.clear();
    }

    std::vector<PropertyEntry> GetBoundProperties() const {
        return _boundProperties;
    }

    std::string GetPropertyValueString(const std::string& name) const {
        auto it = std::find_if(_boundProperties.begin(), _boundProperties.end(), [&](const PropertyEntry& e) { return e.name == name; });
        if (it == _boundProperties.end()) return "";
        switch (it->type) {
        case PropertyType::Int: {
            auto p = std::get<int*>(it->ptr);
            return p ? std::to_string(*p) : "";
        }
        case PropertyType::Float: {
            auto p = std::get<float*>(it->ptr);
            return p ? std::to_string(*p) : "";
        }
        case PropertyType::Double: {
            auto p = std::get<double*>(it->ptr);
            return p ? std::to_string(*p) : "";
        }
        case PropertyType::Bool: {
            auto p = std::get<bool*>(it->ptr);
            return p ? (*p ? "true" : "false") : "";
        }
        case PropertyType::String: {
            auto p = std::get<std::string*>(it->ptr);
            return p ? *p : "";
        }
        }
        return "";
    }

    bool SetPropertyFromString(const std::string& name, const std::string& value) {
        auto it = std::find_if(_boundProperties.begin(), _boundProperties.end(), [&](const PropertyEntry& e) { return e.name == name; });
        if (it == _boundProperties.end()) return false;
        try {
            switch (it->type) {
            case PropertyType::Int: {
                auto p = std::get<int*>(it->ptr);
                if (p) *p = std::stoi(value);
                return p != nullptr;
            }
            case PropertyType::Float: {
                auto p = std::get<float*>(it->ptr);
                if (p) *p = std::stof(value);
                return p != nullptr;
            }
            case PropertyType::Double: {
                auto p = std::get<double*>(it->ptr);
                if (p) *p = std::stod(value);
                return p != nullptr;
            }
            case PropertyType::Bool: {
                auto p = std::get<bool*>(it->ptr);
                if (p) {
                    std::string v = value;
                    std::transform(v.begin(), v.end(), v.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                    *p = (v == "1" || v == "true");
                }
                return p != nullptr;
            }
            case PropertyType::String: {
                auto p = std::get<std::string*>(it->ptr);
                if (p) *p = value;
                return p != nullptr;
            }
            }
        }
        catch (...) {
            return false;
        }
        return false;
    }
#endif
};

#define PropertyManager _PropertyManager::GetInstance()

#ifdef _DEBUG
//#define DebuggerBindProperty(name, target) PropertyManager::GetInstance().BindProperty(name, target)
#else
#define DebuggerBindProperty(name, target)
#endif
