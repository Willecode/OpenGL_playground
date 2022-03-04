#pragma once
#include <memory>
#include "object3d.h"
namespace ModelImporting
{
	std::shared_ptr<Object3D> importWavefront(std::string path, ImageCache &textureCache);
}