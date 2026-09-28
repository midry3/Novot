#pragma once
#include "../Debugger/Debugger.h"
#include "PropertyManager.h"
#include "Asset/AssetManager.h"
#include "Asset/Sound/SoundManager.h"
#include "Input/KeyMapping.h"
#include "GameState.h"
#include "Event/Signal.h"
#include "String/FontManager.h"
#ifdef _DEBUG
#define IS_DEBUG_MODE 1
#else
#define IS_DEBUG_MODE 0
#endif