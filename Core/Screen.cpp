#include "Screen.h"
#include <Backend/Backend.h>

int Screen::ScreenShot() {
	auto screen = CoreBackend::Drawing::GetCurrentDrawScreen();
	auto width = screen->GetWidth();
	auto height = screen->GetHeight();
	auto data = std::make_unique<unsigned char[]>(width * height * 4);
	screen->ReadPixels(data.get(), 0, 0, width, height);
	return CoreBackend::FileSystem::SaveImage("screenshot.png", data.get(), width, height);
}
