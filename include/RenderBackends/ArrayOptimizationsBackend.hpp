#ifndef ARRAY_OPTIMIZATIONS_BACKEND_HPP_
#define ARRAY_OPTIMIZATIONS_BACKEND_HPP_

#include <SFML/Config.hpp>
#include <cstddef>

#include "Camera.hpp"
#include "ErrorCode.hpp"

ErrorCode UpdateTextureArrayOptimized (sf::Uint8 *pixelArray, Camera *camera, size_t width, size_t height, size_t gradientNumber);

#endif
