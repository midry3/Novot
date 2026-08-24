#pragma once
#include <DxLib.h>
#include "MathUtils.h"
#include "Singleton.h"
#include <string>
#include <vector>
#include <variant>
#include <algorithm>

class _PropertyManager : public Singleton<_PropertyManager> {
public:
    enum class PropertyType { Int, Float, Bool, String };

    struct PropertyEntry {
        std::string name;
        PropertyType type;
        std::variant<int*, float*, bool*, std::string*> ptr;
    };

private:
#ifdef _DEBUG
    bool debugMode = true;
#endif
    Vector2D windowSize = Vector2D(1280, 720);
    int FPS = 60;
    std::vector<PropertyEntry> _boundProperties;

public:
    Vector2D GetSettingWindowSize() const {
        return windowSize;
    }
    bool IsDebugMode() const {
#ifdef _DEBUG
        return debugMode;
#else
		return false;
#endif
    }
    bool ToggleDebugMode() {
#ifdef _DEBUG
        debugMode = !debugMode;
        return debugMode;
#else
        return false;
#endif
    }
    void SetWindowSize(const Vector2D& size) {
        windowSize = size;
    }
    void SetFPS(int fps) {
        FPS = fps;
    }
    int GetFPS() const {
        return FPS;
    }
    float GetFrameInterval() const {
        return 1.0f / FPS;
    }
    Vector2D GetRealWindowSize() const {
        int w, h;
        GetWindowSize(&w, &h);
        return Vector2D(w, h);
    }
#ifdef _DEBUG
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
        case PropertyType::Int:
            return std::to_string(*std::get<int*>(it->ptr));
        case PropertyType::Float:
            return std::to_string(*std::get<float*>(it->ptr));
        case PropertyType::Bool:
            return (*std::get<bool*>(it->ptr)) ? "true" : "false";
        case PropertyType::String:
            return *std::get<std::string*>(it->ptr);
        }
        return "";
    }

    bool SetPropertyFromString(const std::string& name, const std::string& value) {
        auto it = std::find_if(_boundProperties.begin(), _boundProperties.end(), [&](const PropertyEntry& e) { return e.name == name; });
        if (it == _boundProperties.end()) return false;
        try {
            switch (it->type) {
            case PropertyType::Int:
                *std::get<int*>(it->ptr) = std::stoi(value);
                return true;
            case PropertyType::Float:
                *std::get<float*>(it->ptr) = std::stof(value);
                return true;
            case PropertyType::Bool: {
                std::string v = value;
                std::transform(v.begin(), v.end(), v.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
                *std::get<bool*>(it->ptr) = (v == "1" || v == "true");
                return true;
            }
            case PropertyType::String:
                *std::get<std::string*>(it->ptr) = value;
                return true;
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