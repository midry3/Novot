#pragma once
#include <cstddef>

namespace CoreBackend {
    class Drawing {
    public:
        static void SetAlphaBlend(int alpha);
        static void SetNoBlend();
        static void SetBrightness(int red, int green, int blue);
        static void DrawSprite(int x, int y, int cx, int cy, double sx, double sy, double angle, int handle, int trans);
        static void DrawSpriteRegion(int x, int y, int sourceX, int sourceY, int width, int height, int cx, int cy, double sx, double sy, double angle, int handle, int trans);
        static void RenderText(int x, int y, double sx, double sy, int ex, int ey, double angle, unsigned int color, int font, unsigned int edge, int vertical, const char* text);
        static int MeasureText(const char* text, int length, int font, int vertical = 0);
        static int FontLineSpace(int font);
        static int CreateTextFont(const char* name, int size, int thickness);
        static void DeleteFont(int font);
        static unsigned int Color(int red, int green, int blue);
    };
    class Texture {
    public:
        static int Load(const void* data, int size);
        static int LoadDiv(const void* data, int size, int all, int x, int y, int width, int height, int* handles);
        static void Release(int handle);
        static int Size(int handle, int* width, int* height);
    };
    class Sound {
    public:
        static int Load(const void* data, int size);
        static void Play(int handle, bool loop);
        static void Stop(int handle);
        static void Release(int handle);
        static void SetVolume(int handle, int volume);
        static bool IsPlaying(int handle);
    };
    class MouseInput {
    public:
        static int Buttons();
        static void Position(int* x, int* y);
        static int Wheel();
        static bool IsLeft(int buttons);
        static bool IsRight(int buttons);
        static bool IsMiddle(int buttons);
    };
    class Keyboard {
    public:
        static void State(char* state);
        static int Character();
    };
    class Window {
    public:
        static void SetSize(int width, int height);
        static void Size(int* width, int* height);
        static bool IsWindowed();
        static void SetMode(bool windowed);
    };
    class Clock {
    public:
        static unsigned int Tick();
        static int Now();
    };
    class Model {
    public:
        static int Load(const char* path);
        static void Release(int handle);
    };
    class Shader {
    public:
        static int LoadPixel(const char* path);
        static void Release(int handle);
        static int CreateBuffer(std::size_t size);
        static void* Buffer(int handle);
        static void SetConstantBuffer(int handle, int slot);
    };
}
