#pragma once
#include "../Debugger/Debugger.h"
#include "PropertyManager.h"

#include "Asset/Data/Json.h"
#include "Asset/Texture/Texture.h"
#include "Asset/Sound/Sound.h"
#include "Asset/AssetManager.h"
#include "Asset/Sound/SoundManager.h"

#include "Input/KeyMapping.h"
#include "Input/KeyInput.h"
#include "GameState.h"
#include "Event/Signal.h"
#include "String/FontManager.h"
#include "Math/Constants.h"
#include "Math/Utils.h"
#ifdef _DEBUG
#define IS_DEBUG_MODE
#endif

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template <typename T>
using Vector = std::vector<T>;

using String = std::string;
using Path = fs::path;
using Int = int;
using Double = double;

using Vec2 = Vector2D;