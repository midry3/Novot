#pragma once
#include <DxLib.h>
#include <Singleton.h>
#include "NovotCore.h"
#include <SceneManager.h>
//#include "Parser/ScriptParser/NVTLexer.h"
//#include "VM/VM.h"
#include <math.h>
#include "Core/Scene.h"
#include "Core/Timer.h"
#include <Mouse/Mouse.h>

class _Novot : public Singleton<_Novot> {
public:
	void Setup();
	void Update(float dt);
	void Draw();
	int StartGame();
};

#define Novot _Novot::GetInstance()