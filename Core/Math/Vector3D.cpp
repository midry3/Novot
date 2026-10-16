#include "Vector3D.h"
#include <DxLib.h>

unsigned int Vector3D::ToColor() const {
	return GetColor(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
}