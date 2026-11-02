#include "Vector3D.h"
#include <Backend/Backend.h>

unsigned int Vector3D::ToColor() const {
	return CoreBackend::Drawing::Color(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z));
}