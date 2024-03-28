#ifndef ARRAY_OPTIMIZATIONS_BACKEND_HPP_
#define ARRAY_OPTIMIZATIONS_BACKEND_HPP_

#include <SFML/Config.hpp>
#include <cstddef>

#include "Camera.hpp"
#include "ErrorCode.hpp"

ErrorCode UpdatePixelArrayOptimized (sf::Uint8 *pixelArray, Camera *camera, size_t pixelX, size_t pixelY, size_t gradientNumber);

#endif
