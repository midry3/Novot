#pragma once
#include "../Debugger/Debugger.h"
#include "Camera.h"
#include "PropertyManager.h"
#include "Asset/AssetLoader.h"
#include "Asset/Sound/SoundManager.h"
#include "Input/KeyMapping.h"
#include "GameState.h"
#include "Event/Signal.h"
#ifdef _DEBUG
#define IS_DEBUG_MODE 1
#else
#define IS_DEBUG_MODE 0
#endif