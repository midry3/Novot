#include "Backend.h"
#include <DxLib.h>

namespace CoreBackend {
    void Drawing::SetAlphaBlend(int alpha) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha); }
    void Drawing::SetNoBlend() { SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); }
    void Drawing::SetBrightness(int red, int green, int blue) { SetDrawBright(red, green, blue); }
    void Drawing::DrawSprite(int x, int y, int cx, int cy, double sx, double sy, double angle, int handle, int trans) { DrawRotaGraph3(x, y, cx, cy, sx, sy, angle, handle, trans); }
    void Drawing::DrawSpriteRegion(int x, int y, int sourceX, int sourceY, int width, int height, int cx, int cy, double sx, double sy, double angle, int handle, int trans) { DrawRectRotaGraph3(x, y, sourceX, sourceY, width, height, cx, cy, sx, sy, angle, handle, trans); }
    void Drawing::RenderText(int x, int y, double sx, double sy, int ex, int ey, double angle, unsigned int color, int font, unsigned int edge, int vertical, const char* text) { DrawRotaStringToHandle(x, y, sx, sy, ex, ey, angle, color, font, edge, vertical, text); }
    int Drawing::MeasureText(const char* text, int length, int font, int vertical) { return GetDrawNStringWidthToHandle(text, length, font, vertical); }
    int Drawing::FontLineSpace(int font) { return GetFontLineSpaceToHandle(font); }
    int Drawing::CreateTextFont(const char* name, int size, int thickness) { return CreateFontToHandle(name, size, thickness); }
    void Drawing::DeleteFont(int font) { DeleteFontToHandle(font); }
	int Drawing::CreateDrawScreen(int width, int height) { return MakeScreen(width, height, TRUE); }
	int Drawing::GetCurrentDrawScreen() { return GetDrawScreen(); }
	void Drawing::SetDrawScreen(int handle) { DxLib::SetDrawScreen(handle); }
	void Drawing::ClearDrawScreen() { DxLib::ClearDrawScreen(); }
	void Drawing::DeleteDrawScreen(int handle) { DeleteGraph(handle); }
    unsigned int Drawing::Color(int red, int green, int blue) { return GetColor(red, green, blue); }

    int Texture::Load(const void* data, int size) { return CreateGraphFromMem(data, size); }
    int Texture::LoadDiv(const void* data, int size, int all, int x, int y, int width, int height, int* handles) { return CreateDivGraphFromMem(data, size, all, x, y, width, height, handles); }
    void Texture::Release(int handle) { DeleteGraph(handle); }
    int Texture::Size(int handle, int* width, int* height) { return GetGraphSize(handle, width, height); }

    int Sound::Load(const void* data, int size) { return LoadSoundMemByMemImage(data, size); }
    void Sound::Play(int handle, bool loop) { PlaySoundMem(handle, loop ? DX_PLAYTYPE_LOOP : DX_PLAYTYPE_BACK, TRUE); }
    void Sound::Stop(int handle) { StopSoundMem(handle); }
    void Sound::Release(int handle) { DeleteSoundMem(handle); }
    void Sound::SetVolume(int handle, int volume) { ChangeVolumeSoundMem(volume, handle); }
    bool Sound::IsPlaying(int handle) { return CheckSoundMem(handle) == TRUE; }

    int MouseInput::Buttons() { return GetMouseInput(); }
    void MouseInput::Position(int* x, int* y) { GetMousePoint(x, y); }
    int MouseInput::Wheel() { return GetMouseWheelRotVol(); }
    bool MouseInput::IsLeft(int buttons) { return (buttons & MOUSE_INPUT_LEFT) not_eq 0; }
    bool MouseInput::IsRight(int buttons) { return (buttons & MOUSE_INPUT_RIGHT) not_eq 0; }
    bool MouseInput::IsMiddle(int buttons) { return (buttons & MOUSE_INPUT_MIDDLE) not_eq 0; }
    void Keyboard::State(char* state) { GetHitKeyStateAll(state); }
    int Keyboard::Character() { return GetInputChar(TRUE); }

    void Window::SetSize(int width, int height) { SetGraphMode(width, height, 32); DxLib::SetWindowSize(width, height); }
    void Window::Size(int* width, int* height) { GetWindowSize(width, height); }
    bool Window::IsWindowed() { return GetWindowModeFlag() not_eq 0; }
    void Window::SetMode(bool windowed) { ChangeWindowMode(windowed ? TRUE : FALSE); SetDrawScreen(DX_SCREEN_BACK); }

    unsigned int Clock::Tick() { return GetTickCount64(); }
    int Clock::Now() { return GetNowCount(); }
    int Model::Load(const char* path) { return MV1LoadModel(path); }
    void Model::Release(int handle) { MV1DeleteModel(handle); }
    int Shader::LoadPixel(const char* path) { return LoadPixelShader(path); }
    void Shader::Release(int handle) { DeleteShader(handle); }
    int Shader::CreateBuffer(std::size_t size) { return CreateShaderConstantBuffer(static_cast<int>(size)); }
    void* Shader::Buffer(int handle) { return GetBufferShaderConstantBuffer(handle); }
    void Shader::SetConstantBuffer(int handle, int slot) { SetShaderConstantBuffer(handle, DX_SHADERTYPE_PIXEL, slot); }
	void Shader::UpdateConstantBuffer(int handle) { UpdateShaderConstantBuffer(handle); }
}
